/* First iteration of code to go with the Gibbot v5 control board. This code
 * will be developed to interface with a python controller on the PC that
 * can drive the motor, control the moagnets and read data from the Gibbot.
 */

#include <p33EP512MC806.h>
#include "initializeV6.h"
#include "motor.h"
#include "test.h"

int main(void) {
    initialize();
    initialize_ADC_Offset();
    int duty = 500; //set duty (change number to test different duty cycles)
    initialize();
    write_duty(duty); //apply duty cycle change
    motoron = 1;//turn motor on
    kick();//kick start commutation
    while (1){
        if(USER){
            LED1 = !LED1;
            LED2 = !LED2;
            LED3 = !LED3;
            LED4 = !LED4;

            //TOPMAG = !TOPMAG;
            //commutate(7);
        }
       //test_MayDay();
    }
    return 0;
}
