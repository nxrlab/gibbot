#include <stdio.h>
#include <p33EP512MC806.h>
#include "UART.h"
#include "initializeV6.h"
#include "debug.h"
#include "linkedlist.h"
#include "encoder.h"
#include "MPU.h"
#include "Temperature.h"

#define LOWMAG_ON '1'
#define LOWMAG_OFF '2'
#define LOWMAG_TOGGLE '3'
#define LOWMAGENC_READ '4'
#define MOTENC_READ '5'
#define LOWMAGENC_SET '6'
#define MOTENC_SET '7'
#define ACCEL_READ '9'
#define GYRO_READ 'a'
#define MOTOR_TEMP_READ 'b'
#define MPU_TEST 'c'
#define READ_VALUE 'X'


volatile struct buffer_t uart_buffer;

unsigned char read_UART(void){
    return dequeue();
}

void read_string_UART(unsigned char *data, int n){
    int i = 0,j = 0;
    while(i < n && j != 1){
       data[i] = read_UART();
       if(data[i] == '\n'){
           j = 1;
       }
       else{
           j = 0;
       }
        i++;
    }
    data[i] = '\n';
}

void write_UART2(unsigned char data){
    while(U2STAbits.UTXBF);
    U2TXREG = data;
}

void write_string_UART2(unsigned char *data, int n){
    int i = 0;
    while(data[i] != '\n' && i < n){
        write_UART2(data[i]);
        i++;
    }
    //write_UART2('\n');
}

/* Clear every element in the uart_buffer and release the memory */
void clear_queue(void) {
    LinkedList *l;                      //Create temporary pointer

    while (uart_buffer.first != NULL) { //While elements remain in the buffer
        l = uart_buffer.first;          //Point temp ptr to first element
        uart_buffer.first = l->next;    //Change pointer to first element to
                                        //point to the second element
        free(l);                        //Clear the first element
    }
    uart_buffer.last = NULL;            //Clear pointer to last element
    uart_buffer.len = 0;                //Clear tally
}

/* Read value from the first element in the uart_buffer and remove that element
 */
unsigned char dequeue(void){
    unsigned char c = EOF;
    LinkedList *l;                   //Create temporary pointer

    if (uart_buffer.first != NULL) { //If elements remain in the buffer
        l = uart_buffer.first;       //Point temp ptr to first element
        uart_buffer.first = l->next; //Change pointer to first element to
                                     //point to the second element
        c = l->c;                    //Store value from first element
        free(l);                     //Clear the first element
        uart_buffer.len--;           //Decrement tally
    }

    return c;                        //If no elements remain return EOF,
                                     //otherwise return value in element
}

/* Add a new element to the uart_buffer */
void enqueue(unsigned char c) {
    LinkedList *l; //Create temporary pointer to a linked list

    l = (LinkedList *) malloc(sizeof (LinkedList)); //Try to allocate memory
    if (l == NULL) { //If pointer is empty there is no remaining memory
        read_error(); //Remove two entries from error buffer
        read_error();
        log_error(ERR_BUFF_FULL); //Add buffer full entry
        l = (LinkedList *) malloc(sizeof (LinkedList)); //Allocate memory
    }

    // add data
    l->c = c;       //Write data to variable in element

    // link
    l->next = NULL; //clear pointer
    if (uart_buffer.first == NULL) { //If no elements in uart_buffer
        uart_buffer.first = l;      //First and last both point to the only
        uart_buffer.last = l;       //  element
    } else {
        uart_buffer.last->next = l; //Set next ptr of last element to new element
        uart_buffer.last = l;       //Set last ptr to new element
    }
    uart_buffer.len++;              //Increment tally
}

void __attribute__((interrupt, no_auto_psv)) _U2RXInterrupt(void){
    while (U2STAbits.URXDA){ //If there is data in the recieve register
        enqueue(U2RXREG);    //Add data to uart_buffer
    }
    LED3 = !LED3;
    LED4 = !LED4;
    IFS1bits.U2RXIF = 0; //Clear interrupt flag
}

void initialize_UART2(void){
    // UART2 is for communication between dsPICs
    /* Connections for UART2 are as follows:
     * RX: RP100 (RF4)
     * TX: RP101 (RF5)
     */
    TRISFbits.TRISF4 = 1; //RX is input
    TRISFbits.TRISF5 = 0; //TX is output

    RPINR19bits.U2RXR = 100; //UART2 RX tied to RP100 (RF4)
    RPOR9bits.RP101R = 3; //RP101 (RF5) tied to UART2 TX

    U2MODEbits.BRGH = 1; //Turn High Baud Rate Mode on
    U2BRG = 89; //Baud Rate = 112044
    U2MODEbits.UEN = 0b00;

    IPC7bits.U2RXIP = 7; //Set RX interrupt priority to 7
    IFS1bits.U2RXIF = 0; //Clear the receive interrupt flag
    IEC1bits.U2RXIE = 1; //Enable receive interrupts

    U2MODEbits.UARTEN = 1; //Enable the UART
    U2STAbits.UTXEN = 1; //Enable transmitting
}

void UART2_task(void){
    /*Performs a set task depenedent upon command received over UART from the
     dsPIC on the primary board.  These tasks include turning on/off the lower
     magnet, reading the motor/lower magnet encoder, reading the IMU on the secondary
     board, etc.*/
    unsigned char task;
     if(uart_buffer.len>0){
        task = read_UART();
        LED2 = !LED2;
         if(task == LOWMAG_ON){
             LOWMAG = 1; //Turn on low magnet
         }
         else if(task == LOWMAG_OFF){
             LOWMAG = 0; //Turn off low magnet
         }
         else if(task == LOWMAG_TOGGLE){
             LOWMAG = !LOWMAG; //Toggle low magnet
             write_UART2('w'); //Simple test to ensure UART communication between boards is working
         }
         else if(task == LOWMAGENC_READ){
             //Reads the encoder value of the lower magnet
             long value;
             unsigned short i;
             unsigned char temp[4];
             value = read_LOWMAGENC();
             temp[0] = value;
             temp[1] = value>>8;
             temp[2] = value>>16;
             temp[3] = value>>24;
             for (i=0; i<4; i++){
                 write_UART2(temp[i]);
             }
             //write_string_UART2(temp,4);
         }
         else if(task == MOTENC_READ){
             //Reads the encoder value of the motor
             long value;
             unsigned short i;
             unsigned char temp[4];
             value = read_MOTENC();
             temp[0] = value;
             temp[1] = value>>8;
             temp[2] = value>>16;
             temp[3] = value>>24;
             for (i=0; i<4; i++){
                 write_UART2(temp[i]);
             }
             //write_string_UART2(temp,4);
         }
         else if(task == LOWMAGENC_SET){
             //Sets the encoder value of the lower magnet
             unsigned char temp[4];
             long val;
             while(!(uart_buffer.len>3));
             read_string_UART(temp,4);
             val = *((long *)temp);
             write_LOWMAGENC(val);
             LED4 = !LED4;
         }
         else if(task == MOTENC_SET){
             //Sets the encoder value of the motor
             unsigned char temp[4];
             long val;
             while(!(uart_buffer.len>3));
             read_string_UART(temp,4);
             val = *((long *)temp);
             write_MOTENC(val);
             LED4 = !LED4;
         }
         else if(task == MPU_TEST){
             //Reads MPU9150 address to check connection.  Should return 0x68.
             unsigned char data[1];
             read_MPU_test(data);
         }
         else if(task == ACCEL_READ){
             //Read accelerometer data from IMU
             unsigned char data[6];
             unsigned short i;
             read_Accel(data);
             for (i=0;i<6;i++){
                 write_UART2(data[i]);
             }
         }
         else if(task == GYRO_READ){
             //Read gyroscope data from IMU
             unsigned char data[6];
             unsigned short i;
             read_Gyro(data);
             for (i=0;i<6;i++){
                 write_UART2(data[i]);
             }
         }
         else if(task == MOTOR_TEMP_READ){
             //Read ADC value and convert to temperature
             float mot_temp;
             unsigned short i;
             char *send = &mot_temp;
             mot_temp = motor_temp(thermistor_resistance());
             for (i=0;i<sizeof(mot_temp);i++){
                 write_UART2(send[i]);
             }
         }
         else if(task == '8'){ // test
             unsigned char data[5] = {'a','b','c','d','e'};
             write_string_UART2(data, 5);
         }
		 else if (task == READ_VALUE){
			 //Test the communication between the dsPIC and PIC32.
			 //Returns the character "Y" if the user enters "X".
			 write_UART2(task);
			 LED3 = !LED3;
		 }
     }

}