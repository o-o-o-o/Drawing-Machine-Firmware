#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <plib/delays.h>
#include <ctype.h>
#include <plib/usart.h>
#include "servo.h"

void PENdown(void){
    static unsigned int counter;

    for(counter = 0; counter < 50; counter++){
        SERVO = 1;
        Delay10TCYx(115); //some fraction of 2.5 ms
        SERVO = 0;
        Delay10TCYx(140); //the remaining fraction of 2.5 ms
        Delay1KTCYx(17); //17 ms (total period is 20ms
    }
        while(BusyUSART());
        WriteUSART('D');
}

void PENup(void){
    static unsigned int counter;

    for(counter = 0; counter < 50; counter++){
        SERVO = 1;
        Delay10TCYx(180); //some fraction of 2.5 ms
        SERVO = 0;
        Delay10TCYx(75); //the remaining fraction of 2.5 ms
        Delay1KTCYx(17); //17 ms (total period is 20ms
    }
        while(BusyUSART());
        WriteUSART('D');
}

