#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <plib/delays.h>
#include <plib/adc.h>
#include <plib/pwm.h>
#include <plib/timers.h>
#include <plib/usart.h>
#include "LCD.h"
#include "DMinitialize.h"

void DMinitialize(void){
    unsigned char ADCconfig1 = 0x00, ADCconfig2 = 0x00, portconfig = 0x00;

    unsigned char TMR0config1 = 0x00, TMR0config2 = 0x00;
    unsigned int TMR0_value=0x00;

    unsigned char TMR1config1 = 0x00, TMR1config2 = 0x00;
    unsigned int TMR1_value=0x00;

    unsigned char USARTconfig = 0;
    unsigned int spbrg = 0;

    TRISA = 0b11111111;
    TRISB = 0b11111000;
    TRISC = 0b00000000;
    TRISD = 0b00000000;
    TRISE = 0b00000000;

    LATB = 0b00000000;
    LATC = 0b00000000;
    LATD = 0b00000000;
    LATE = 0b00000000;

    INTCON = 0b00000000;
    INTCON2 = 0b00000000;
    INTCON3 = 0b00000000;

    PIR1 = 0b00000000;
    PIR2 = 0b00000000;

    PIE1 = 0b00000000;
    PIE2 = 0b00000000;

//Initialize ADC
    ADCconfig1 = ADC_FOSC_4 & ADC_RIGHT_JUST & ADC_12_TAD;
    ADCconfig2 = ADC_CH0 & ADC_INT_OFF & ADC_REF_VDD_VSS;
    portconfig = ADC_3ANA;

    OpenADC(ADCconfig1, ADCconfig2, portconfig);

//Initialize PWM
    OpenPWM1(0xf0); //set PWM frequency
    SetDCPWM1(0);

//Initialize LCD
    LCDinit();      //Initialize LCD hardware

//Initialize Timer0
    TMR0config1 = TIMER_INT_OFF & T0_SOURCE_INT & T0_PS_1_1 & T0_16BIT & T0_EDGE_RISE;

//Initialize Timer1
    TMR1config1 = TIMER_INT_OFF & T1_SOURCE_INT & T1_PS_1_1;

//Initialize USART
    USARTconfig = USART_TX_INT_OFF & USART_RX_INT_ON & USART_ASYNCH_MODE & USART_EIGHT_BIT
            & USART_BRGH_HIGH & USART_CONT_RX;
    spbrg = 25;
    OpenUSART(USARTconfig, spbrg);
    PIE1bits.RCIE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;

    SSPCON1bits.SSPEN = 0; //Free PORTC by disabling the I2C module
}
