#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <plib/delays.h>
#include <plib/adc.h>
#include <plib/pwm.h>
#include <plib/timers.h>
#include <ctype.h>
#include "LCD.h"


/********************************************************************
 * Functions called by the main loop
 *******************************************************************/
void LCDinit(void)
{
    E = 0;
    Delay10KTCYx(100);  //~1 second delay
    LCDcommand(0x30);   //Note that there are two 0x30 command sent, that's on purpose. Sometimes a third is recommended
    LCDcommand(0x30);
    LCDcommand(0x38);
    LCDcommand(0x10);
    LCDcommand(0x0C);
    LCDcommand(0x01);
    LCDcommand(0x06);
}

void LCDcommand(unsigned char command)
{
    LATD = command;
    RS = 0;
    RW = 0;
    E = 1;
    Delay10TCYx(1);
    E = 0;
    Delay1KTCYx(3);
}

void LCDwrite(unsigned char data)
{
    LATD = data;
    RS = 1;
    RW = 0;
    E = 1;
    Delay10TCYx(1);
    E = 0;
    Delay1KTCYx(3);
}

void DISPLAYadVALUE(unsigned int ADCresult, int side)
{
        unsigned char buffer[4];
        unsigned char i = 0;

        utoa(buffer, ADCresult, 10); //convert to string and store in buffer

        if (side == 0) {
            LCDcommand(right_screen);
        }
        else {
            LCDcommand(left_screen);
        }

        while(buffer[i] >= 48 && buffer[i] <= 57) //ascii values for 0-9 are 48-57
            {
                LCDwrite(buffer[i]);
                i++;
            }
}

void LCDmessage(const char *message, int side)
{
    unsigned char j=0;            //send the LCD a welcome message

    if (side == 0) {
        LCDcommand(right_screen);
    }
    else {
        LCDcommand(left_screen);
    }

    while(isalnum(*(message+j)) || (*(message+j) == ' '))
    {
        LCDwrite(*(message+j));
        j++;
        if (j == 8){
            break;
        }
    }
}

void DISPLAYtemp(unsigned int temperature)
{
    unsigned char tempbuff[4];
    const char *head = "Temp=\n";

    LCDmessage(head, LEFT);
    LCDcommand(right_screen);

    utoa(tempbuff, temperature, 10); //convert to string and store in buffer

    if (temperature < 1000 && temperature >= 100){
        LCDwrite(tempbuff[0]);
        LCDwrite(tempbuff[1]);
        LCDwrite(0b00101110);
        LCDwrite(tempbuff[2]);
        LCDwrite(0b11011111);
        LCDwrite('C');
    }

    if (temperature < 100 && temperature >= 10){
        LCDwrite(tempbuff[0]);
        LCDwrite(0b00101110);
        LCDwrite(tempbuff[1]);
        LCDwrite(0b11011111);
        LCDwrite('C');
    }

    if (temperature < 10){
        LCDwrite('0');
        LCDwrite(0b00101110);
        LCDwrite(tempbuff[0]);
        LCDwrite(0b11011111);
        LCDwrite('C');
    }
}

void DISPLAYint(signed int integer, int side)
{
        unsigned char buffer[5];
        unsigned char i = 0;

    //    utoa(buffer, integer, 10); //convert to string and store in buffer

        if (side == 0) {
            LCDcommand(right_screen);
        }
        else {
            LCDcommand(left_screen);
        }

        if (integer < 0) {
            LCDwrite(0b00101101);
            integer = -integer;
        }
        
        utoa(buffer, integer, 10); //convert to string and store in buffer
        

        while(buffer[i] >= 48 && buffer[i] <= 57) //ascii values for 0-9 are 48-57
            {
                LCDwrite(buffer[i]);
                i++;
                if (i == 5) {
                    break;
                }
            }
}