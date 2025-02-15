/** acrobot:

This app implements how to read and write bytes over UART, radio, and USB.
*/

#include <wixel.h>
#include <usb.h>
#include <usb_com.h>
#include <radio_link.h>
#include <uart0.h>
#include <stdio.h>

#define DELAY_MS 750

// these are commands that the wixel responds to
#define COMMAND_TOGGLE_YELLOW_LED 'y'
#define COMMAND_PRINT_HI 'h'
#define COMMAND_GET_TIME 't'
#define COMMAND_SEND_OVER_RADIO 'r'
#define COMMAND_RADIO_RESPONSE 'w'
#define COMMAND_SEND_OVER_UART 'u'

/* VARIABLES ******************************************************************/

/** True if the yellow LED should currently be on. */
BIT yellowLedOn = 0;

/** A temporary buffer used for composing responses to the computer before
 * they are sent.  Must be bigger than the longest possible response to any
 * command.
 */
uint8 XDATA response[32];

/* FUNCTIONS ******************************************************************/

void updateLeds()
{
    usbShowStatusWithGreenLed();
    LED_YELLOW(yellowLedOn);
}

/** Processes a new byte received from a serial connection
 * */
void processByte(uint8 c)
{
    uint8 responseLength;
    uint32 time;
    uint8 XDATA *tmt;

    switch(c) {
    // these cases handle encoder readings; a better
    // implementation would use sscanf in the processBytesUart0
    // function
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '\r':
    case '\n':
        usbComTxSendByte(c);
        break;

    // just in case we can't print anything to screen
    // this command allows for a visual that we are
    // at least getting serial commands
    case COMMAND_TOGGLE_YELLOW_LED:
        yellowLedOn ^= 1;
        break;
        
    // send this packet to another wixel over wireless comm
    case COMMAND_SEND_OVER_RADIO:
        // print to USB first to help debug
        responseLength = sprintf(response, "radio 1...\r\n");
        usbComTxSend(response, responseLength);

        tmt = radioLinkTxCurrentPacket();
        if(tmt != NULL) {
            tmt[0] = 1; // payload length
            tmt[1] = COMMAND_RADIO_RESPONSE; // payload data
            radioLinkTxSendPacket(0);
        }
        break;

    // response after receiving a byte over radio
    case COMMAND_RADIO_RESPONSE:
        // print to USB first to help debug
        responseLength = sprintf(response, "radio 2...\r\n");
        usbComTxSend(response, responseLength);

        tmt = radioLinkTxCurrentPacket();
        if(tmt != NULL) {
            tmt[0] = 1; // payload length
            tmt[1] = COMMAND_GET_TIME; // payload data
            radioLinkTxSendPacket(0);
        }
        break;

    // respond to a byte sent over UART, requires
    // dsPIC or RX/TX lines being tied together on
    // wixel.
    case COMMAND_SEND_OVER_UART:
        // print to USB first to help debug
        responseLength = sprintf(response, "uart0...\r\n");
        usbComTxSend(response, responseLength);

        uart0TxSendByte(COMMAND_GET_TIME);
        break;

    // send packet over USB
    case COMMAND_GET_TIME:
        time = getMs();
        responseLength = sprintf(response, "time=0x%04x\r\n", (uint16)time);
        usbComTxSend(response, responseLength);
        break;
    }
}

/** Checks for new bytes available on the USB virtual COM port
 * and processes all that are available. */
void processBytesFromUsb()
{
    uint8 bytesLeft = usbComRxAvailable();
    while(bytesLeft && usbComTxAvailable() >= sizeof(response))
    {
        processByte(usbComRxReceiveByte());
        bytesLeft--;
    }
}

/** Checks for new bytes available from the wireless radio
 * and processes all that are available. */
void processBytesFromRadio()
{
    uint8 i, n;
    uint8 XDATA *rcv;
    
    rcv = radioLinkRxCurrentPacket();
    if(rcv != NULL) {
        n = rcv[0]; // payload length
        for(i = 1; i <= n && radioLinkTxAvailable(); i++) {
            processByte(rcv[i]);
        }
        radioLinkRxDoneWithPacket();
    }
}

/** Checks for new bytes available on UART0
 * and processes all that are available. */
void processBytesFromUart0()
{
    // this code should be modified to use sscanf.
    uint8 bytesLeft = uart0RxAvailable();
    while(bytesLeft && uart0TxAvailable() >= sizeof(response))
    {
        processByte(uart0RxReceiveByte());
        bytesLeft--;
    }
}

void main()
{
    // required by wixel api:
    // http://pololu.github.io/wixel-sdk/group__libraries.html
    systemInit();
    usbInit();
    radioLinkInit();
    uart0Init();

    // set uart
    uart0SetBaudRate(9600);
    uart0SetParity(PARITY_NONE);
    uart0SetStopBits(STOP_BITS_1);

    // wait for a wireless pairing
    // between two wixels
    // blink yellow LED while connection is being established
    while(!radioLinkConnected()) {
        yellowLedOn ^= 1;
        updateLeds();
        delayMs(DELAY_MS);
    }

    // turn off LED
    yellowLedOn = 0;

    // process bytes
    while(1)
    {
        boardService();
        updateLeds();
        usbComService();
        processBytesFromUsb();
        processBytesFromRadio();
        processBytesFromUart0();
    }
}
