/*********************************************
 * File:   DMinterrupt.c
 * Author: Dave Perry
 *
 * Created on February 14, 2014, 9:37 PM
 *
 * Interrupt Service Routine for the Drawing Machine Microcontroller
 ******************************************/


/*******************************************/
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <plib/delays.h>
#include <plib/adc.h>
#include <plib/pwm.h>
#include <plib/timers.h>
#include <plib/usart.h>
#include <ctype.h>
#include "servo.h"
/**********************************************/

void interrupt high_isr(void)
{
    static unsigned char rx_index;
    unsigned char buffer;

    extern volatile unsigned char data_ready; //Funcky declaration
    extern volatile unsigned char Rxdata[12];
    extern volatile unsigned char *rxptr;

    if (PIE1bits.RCIE && PIR1bits.RCIF){
        buffer = RCREG;

        if (buffer == 'd'){
            rx_index = 0;
        }

        if (buffer == 'm'){
            PENdown();    
            return;
        }

        if (buffer == 'c'){
            PENup();
            return;       
        }

        if (rx_index < 11){
            *(rxptr + rx_index) = buffer;
        }

        if (rx_index == 10) {
            data_ready = 1;
        }

        rx_index += 1;
        return;
    }
}