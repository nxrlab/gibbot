#include <p33EP512MC806.h>
#include "encoder.h"
#include "I2CSlave.h"

void initialize_QEI(void){
    //Turn on QEI
    ANSELBbits.ANSB9 = 0;   //Turn off analog functionality on input pin
    ANSELBbits.ANSB10 = 0;  //Turn off analog functionality on input pin
    RPINR14bits.QEA1R = 42; //Set RPI42 (B10) as QEI1 A
    RPINR14bits.QEB1R = 41; //Set RPI41 (B9) as QEI1 B
    POS1CNTL = 0;           //LOWMAGENC
    POS1CNTH = 0;
    RPINR16bits.QEA2R = 98; //Set RP69 (D5) as QEI2 A
    RPINR16bits.QEB2R = 99; //Set RP68 (D4) as QEI2 B
    POS2CNTL = 0;           //MOTENC
    POS2CNTH = 0;
    QEI1CONbits.QEIEN = 1; //Turn on QEI 1
    QEI2CONbits.QEIEN = 1; //Turn on QEI 2
}

void read_encoder(void){
    unsigned short temp1L;
    unsigned short temp1H;
    unsigned short temp2L;
    unsigned short temp2H;
    //store CNTL register, automatically stores CNTH register in HLD
    temp2L = POS2CNTL;
    //store HLD register which is the same as CNTH when CNTL was read
    temp2H = POS2HLD;
    RegBuffer[MOTENC+3] = temp2H>>8;
    RegBuffer[MOTENC+2] = temp2H;
    RegBuffer[MOTENC+1] = temp2L>>8;
    RegBuffer[MOTENC] = temp2L;
    //store CNTL register, automatically stores CNTH register in HLD
    temp1L = POS1CNTL;
    //store HLD register which is the same as CNTH when CNTL was read
    temp1H = POS1HLD;
    RegBuffer[LOWMAGENC+3] = temp1H>>8;
    RegBuffer[LOWMAGENC+2] = temp1H;
    RegBuffer[LOWMAGENC+1] = temp1L>>8;
    RegBuffer[LOWMAGENC] = temp1L;
}