/* The Analog-to-digital converter module reads the analog voltage from the
 * current sensors and converts it to a 12-bit word. The ADC is initialized to
 * continuously read using automatically triggered sampling and conversion. The
 * direct memory access modules writes the data from each reading to a
 * buffer in the DPSRAM memory space. The ADC_Read() function returns the
 * average of the values in this buffer.
 *
 * This file also contains functions for reading the ACD using single reads,
 * which resulted in significant noise in early tests.
 */
#include <libpic30.h>
#include <p33EP512MC806.h>
#include "initializeV6.h"
#include "ADC.h"
#define ADC_BUFF_LEN 128 //Length of the DMA Buffer, should be a power of 2


//Create DMA buffer for ADC, the macro is required because the buffer is
//outside of the normal memory space
__eds__ unsigned int BufferA[ADC_BUFF_LEN] __attribute__((eds,space(dma)));

/* ADC interrupt is disabled
 * Interrupt vector names for the dsPIC33F (we are using a dsPIC33E) are in 
 * Table 7-4 p101 of the MPLAB C30 User's Guide. The ADC1 interrupt name is 
 * listed as _ADC1Interrupt in that document, but that doesn't compile. Since
 * _AD1Interrupt does compile we use that.
 */
//void __attribute__((interrupt, no_auto_psv)) _AD1Interrupt(void) {
//    LED1 = !LED1; //Toggle LED for timer reading.
//    IFS0bits.AD1IF = 0;
//}

void initialize_ADC(void){
    //*****Initialize ADC*****
    //The following lines are commented out because they modify the registers
    //to their default state.
    //AD1CON1bits.FORM = 0; //Output is an unsigned int
    //AD1CON2bits.ALTS    = 0;  // Disable alternate input selection
    //ADC trigger can be coupled to the PWM module by modifying SSRC and SSRCG
    //AD1CON2bits.VCFG    = 0;//AVDD and AVSS used as Vref+ and Vref-
    //AD1CON2bits.SMPI = 0; //Increment DMA address after every conversion
    //AD1CON3bits.ADRC = 0; //Use system clock for ADC clock source
    AD1CON1bits.ASAM = 1;   //Automatically sample at the end of each conversion
    AD1CON1bits.AD12B = 1;  //ADC is in 12 bit, 1 channel mode
    AD1CON1bits.ADDMABM = 1;//DMA buffers are written in order of conversion
    AD1CON1bits.SSRC    = 0b111; // Use an automatic trigger
    AD1CON1bits.SSRCG    = 0;    // Use an automatic trigger
    AD1CON3bits.SAMC = 15; //Set the automatic sampling time to 15*Tad
                           //Datasheet says minimum sample time is 3*Tad
    AD1CON3bits.ADCS = 9;  // Tad = (ADCS + 1)*Tcy
                           // Tcy = 1 / 40MHz = 25 ns
                           // Tad = 10*25ns = 250ns
                           // Datasheet sets Tad minimum at 117.6ns
    AD1CON4bits.ADDMAEN = 1; //Use DMA module to automatically write data
    AD1CHS0bits.CH0SA = 26;   //Current sensor connected to AN26

    //Uncomment the next two lines to enable the ADC interrupt
    //IFS0bits.AD1IF = 0; //Clear ADC interrupt flag
    //IEC0bits.AD1IE = 1; //Turn on ADC interrupt

    //*****Initialize DMA*****
    //The following lines are commented out because they modify the registers
    //to their default state.
    //DMA0CONBits.SIZE = 0; //Data transfer size is a word
    //DMA0CONbits.AMODE = 0; //Register indirect mode with post-increment
    //DMA0CONbits.MODE = 0; //Continuous mode with ping-pong mode disabled
    DMA0REQ = 0b00001101; //Connect DMA0 channel to ACD1
    DMA0PAD = (volatile unsigned int) &ADC1BUF0; //Point DMA0 to ADC1 buffer
    DMA0CNT = ADC_BUFF_LEN-1; //Set DMA buffer length (Buffer Length=DMA0CNT+1)

    //This pair of registers holds the start address of the buffer in the DMA
    //memory space that holds the ACD data. More info on the __builtin_dmaoffset
    //macro can be found on page 307 of the MPLAB XC16 C Compiler User's Guide.
    DMA0STAH = 0x0000; //DMA0 start address high byte
    DMA0STAL = __builtin_dmaoffset(BufferA); //DMA0 start address low byte

    //Ensure the DMA interrupt is off
    IFS0bits.DMA0IF = 0; //Clear DMA interrupt flag
    IEC0bits.DMA0IE = 0; //Disable DMA interrupt

    DMA0CONbits.CHEN = 1; //Turn on the DMA Channel
    AD1CON1bits.SAMP = 0; //Ensure ADC sampling is turned off
    AD1CON1bits.ADON = 1; //Turn on the ADC module
}

unsigned short read_ADC(void){
    int i;
    unsigned long temp=0;
    for(i=0;i<ADC_BUFF_LEN; i++){
        temp=temp+BufferA[i]; //Sum the buffer values
    }
    temp = temp/ADC_BUFF_LEN; //take the average
    return temp;
}

/* The following code is to use the ADC in a single read mode that does not
 * use DMA. The ADC will return the results of a single sample instead of the
 * average of multiple samples.
 *
 * To use this code replace the initialize_ADC function with
 * initialize_ADC_Single from below and replace all instances of read_ADC with
 * read_ADC_Single.*/

void initialize_ADC_Single(void) {
    /* The analog-digital voltage converter reads the voltage output
     * from the current sensor which is proportional to the motor drive current.
     */
    AD1CON1bits.FORM    = 0;  // Unsigned integer output
    AD1CON1bits.AD12B   = 1;  // 12-bit data output
    AD1CON2bits.ALTS    = 0;  // Disable alternate input selection
    AD1CON1bits.ASAM    = 0;  // Use manual sampling
    AD1CON1bits.SSRC    = 0b111; // Use an automatic trigger
    AD1CON1bits.SSRCG    = 0; // Use an automatic trigger
    AD1CON2bits.VCFG    = 0;  // Ensure AVDD and AVSS are used as Vref+ and Vref-
    AD1CON3bits.SAMC    = 15; //
    AD1CON3bits.ADCS = 9;     // Tad = (ADCS + 1)*Tcy
                              // Tcy = 1 / 40MHz = 25 ns
                              // Tad = 10*25ns = 250ns
                              // Datasheet sets Tad minimum at 117.6ns

    // Initialize MUXA Input Selection
    AD1CHS0bits.CH0SA = 26;
    AD1CON1bits.SAMP = 0; // Ensure sampling is turned off
    AD1CON1bits.ADON = 1; //Turn on the ADC converter
}

unsigned short ADC_Read_Single(void) { //manual sampling and conversion function
    AD1CON1bits.SAMP = 1; //Start sampling, sampling is stopped after 1us
    while (!AD1CON1bits.DONE); //wait for sampling and conversion to finish
    return (unsigned short) ADC1BUF0;
}
