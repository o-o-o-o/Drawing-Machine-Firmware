/*********************************************
 * File:   DMmain.c
 * Author: Dave Perry
 *
 * Created on February 14, 2014, 9:37 PM
 *
 * Firmware for the drawing machine microcontroller
 ******************************************/


/*******************************************/
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <ctype.h>
#include <plib/delays.h>
#include <plib/adc.h>
#include <plib/pwm.h>
#include <plib/timers.h>
#include <plib/usart.h>
#include <plib/EEP.h>
#include "LCD.h"
#include "servo.h"
#include "DMinitialize.h"

#define ENABLEM LATBbits.LB2
#define RELAY3 LATBbits.LB1
#define SERVO LATBbits.LB0 //RELAY4
#define RELAY_ON 1
#define RELAY_OFF 0

#define STOPRIGHT PORTBbits.RB3
#define UP PORTBbits.RB4
#define DOWN PORTBbits.RB5
#define NEXT PORTBbits.RB6
#define SELECT PORTBbits.RB7

#define MLSTEP  LATCbits.LC1
#define MRSTEP  LATCbits.LC5
#define MLDIR   LATCbits.LC0
#define MRDIR   LATCbits.LC4

#define PdelayADDR 0x0000
#define SdelayADDR 0x0001
/**********************************************/

/*********************************************
*Forward Declarations
**********************************************/
void SENDdouble(double number);
void SENDint(signed int number);
void CHECKusartERRORS(void);
void GOmotors(void);
void Dwrite(void);
double READstring(char * input);
signed int CHECKbuttons(void);
void MENUzero(void);
void MENUone(void);
void MENUtwo(void);
void MENUthree(void);
void MENUfour(void);
void MENUfive(void);
void MENUsix(void);
void LOADfile(void);
void PARSEdata(void);
void SENDinfo(void);
void REENTER(void);
void TESTpattern(void);
unsigned int GETdelay(void);


/*****************************************
 *stepper motor delays
 ****************************************/
    signed int point_delay;  //Delay between stopping and starting
    signed int step_delay;  //Delay between motor steps

/**************************************************
*Some strings
***************************************************/
    const char *Drawing = "Drawing";
    const char *Machine = "Machine";
    const char *Reset = "Reset";
    const char *Saved = "Saved";

/************************************************
 *Some idices
 ************************************************/
    unsigned int LEFTcount = 0;
    unsigned int RIGHTcount = 0;

    signed int button_index = 0;
    signed int count = 0;

/***************************************
 * Volatile registers, related to UART interrupt
 ************************************/
    volatile unsigned int execute_left_d = 0;
    volatile unsigned int execute_right_d = 0;
    volatile unsigned int execute_right_s = 0;
    volatile unsigned int execute_left_s = 0;

    volatile unsigned char data_ready = 0;

    volatile unsigned char Rxdata[12] = "d1000010000";
    volatile unsigned char *rxptr = &Rxdata;

    volatile unsigned char flag = 0; //Used in USART debug
/***************************************
 *MENU variables
 ***********************************/
    unsigned char menu_index = 0;

/*****************************************
*stepper motor delays
* Generally these should be minimized
****************************************/
    signed int point_delay;
    signed int step_delay;


main(void) {

/*********************************************
 *Initialize System
 *****************************************/
    DMinitialize();  // See DMinit.c and DMinitilize.h

 /*******************************************
  *Write to LCD
  ******************************************/
    LCDmessage(Drawing, LEFT);
    LCDmessage(Machine, RIGHT);

/***********************************************
 *Load Values from EEPROM
 ************************************************/
    point_delay = Read_b_eep(PdelayADDR);
    step_delay = Read_b_eep(SdelayADDR);
    
/***********************************************
 *Enable drawing board motor
 ************************************************/
    ENABLEM = 0;

/********************************************
*Main Loop
***********************************************/
    
    while(1)
    {
            if(menu_index == 0){
                MENUzero();  // first menu!!
            } else if (menu_index == 1){
                MENUone();
            } else if (menu_index == 2){
                MENUtwo();
            } else if (menu_index == 3){
                MENUthree();
            } else if (menu_index == 4){
                MENUfour();
            } else if (menu_index == 5){
                MENUfive();
            } else if (menu_index == 6){
                MENUsix();         //wrap back to menu zero
            } else if (menu_index > 6){
                menu_index = 0;
            }
    }                                   //End of main loop
    return 0;
}                                       // End of main


/********************************************************************
 * Functions called by the main loop or by Menu loops
 *******************************************************************/


/*********************************************
 *Check if a button is being pressed
 * -Pressing more than one button at a time may cause unexpected behavior
 *********************************************/
signed int CHECKbuttons(void){
    if (UP == 0){
        Delay1KTCYx(250); //debounce 250 ms
        return 1;
    } else if (DOWN == 0){
        Delay1KTCYx(250); //debounce 250 ms
        return 2;
    } else if (NEXT == 0){
        Delay1KTCYx(250); //debounce 250 ms
        return 3;
    } else if (SELECT == 0){
        Delay1KTCYx(250); //debounce 250 ms
        return 4;
    } else {
        return 0;
    }
}

/**********************************************
 *Send the appropriate logic to the stepper motor driver
 *This function moves the pen to the next point
 ************************************************/
void GOmotors(void){
        LEFTcount = 0;
        RIGHTcount = 0;

        MLDIR = execute_left_d;
        MRDIR = execute_right_d;

        data_ready = 0;

        while (LEFTcount != execute_left_s || RIGHTcount != execute_right_s){
            if (STOPRIGHT==0 && MLDIR==0){
                ENABLEM = 1;
            } else ENABLEM = 0;

            if (LEFTcount < execute_left_s){
                MLSTEP = 1;
                LEFTcount += 1;
            }

            if (RIGHTcount < execute_right_s){
                MRSTEP = 1;
                RIGHTcount += 1;
            }

            Delay10TCYx(step_delay); 

            MLSTEP = 0;
            MRSTEP = 0;

            Delay10TCYx(step_delay); 
        }
}

/***************************************
 *Send 'D', which functions as a request for a new line of data
 ******************************/
void Dwrite(void){
    while(BusyUSART());
    WriteUSART('D');
}

/***********************************
 *MENU ZERO
 ***********************************/

void MENUzero(void){
    const char *Drawing = "Drawing";
    const char *Machine = "Machine";

    LCDcommand(clear_screen);

    while(menu_index == 0){
        LCDmessage(Drawing, LEFT);
        LCDmessage(Machine, RIGHT);
        button_index = CHECKbuttons();
        switch (button_index){
            case 1: Dwrite(); //UP button pressed
                    break;
            case 2: if(STOPRIGHT==0){ //DOWN button pressed
                        menu_index +=1;
                    }
                    break;
            case 3: menu_index += 1;  //NEXT button pressed
                    break;
            case 4: Write_b_eep(SdelayADDR, 50); //SELECT button pressed
                    Busy_eep();
                    Write_b_eep(PdelayADDR, 1);
                    Busy_eep();
                    point_delay = Read_b_eep(PdelayADDR); //added since last compile
                    step_delay = Read_b_eep(SdelayADDR); //added since last compile
                    LCDcommand(clear_screen);
                    LCDmessage(Reset, LEFT);
                    Delay10KTCYx(255);
                    LCDcommand(clear_screen);
                    break;
            default: break;
            }
        }
}

/***********************************
 *MENU ONE
 ***********************************/

void MENUone(void){
    const char *message = "SDelay";

    LCDcommand(clear_screen);

    while(menu_index == 1){
        LCDmessage(message, LEFT);
        DISPLAYint(step_delay, RIGHT);
        button_index = CHECKbuttons();
        switch (button_index){
            case 1: step_delay += 1;  //UP button
                    if (step_delay > 2000){
                        step_delay = 2000;
                    } else if (step_delay < 1){
                        step_delay = 1;
                    }
                    break;
            case 2: step_delay -= 1;  //Down button
                    if (step_delay > 2000){
                        step_delay = 2000;
                    } else if (step_delay < 1){
                        step_delay = 1;
                    }
                    break;
            case 3: menu_index += 1;  //NEXT button pressed
                    break;
            case 4: Write_b_eep(SdelayADDR, step_delay);
                    Busy_eep();  //SELECT button pressed
                    LCDcommand(clear_screen);
                    LCDmessage(Saved, LEFT);
                    Delay10KTCYx(255);
                    LCDcommand(clear_screen);
                    break;
            default: break;
        }
    }
 }

/***********************************
 *MENU TWO
 ***********************************/

void MENUtwo(void){
    const char *message = "PDelay";

    LCDcommand(clear_screen);

    while(menu_index == 2){
        LCDmessage(message, LEFT);
        DISPLAYint(point_delay, RIGHT);
        button_index = CHECKbuttons();
        switch (button_index){
            case 1: point_delay += 1;  //UP button
                    if (point_delay > 2000){
                        point_delay = 2000;
                    } else if (point_delay < 1){
                        point_delay = 1;
                    }
                    break;
            case 2: point_delay -= 1;  //DOWN button
                    if (point_delay > 2000){
                        point_delay = 2000;
                    } else if (point_delay < 1){
                        point_delay = 1;
                    }
                    break;
            case 3: menu_index += 1;  //NEXT button pressed
                    break;
            case 4: Write_b_eep(PdelayADDR, point_delay);
                    Busy_eep();  //SELECT button pressed
                    LCDcommand(clear_screen);
                    LCDmessage(Saved, LEFT);
                    Delay10KTCYx(255);
                    LCDcommand(clear_screen);
                    break;
            default: break;
        }
    }
 }

/***********************************
 *MENU THREE
 ***********************************/

void MENUthree(void){
    const char *messagel = "Stream F";
    const char *messager = "ile";

    LCDcommand(clear_screen);

    while(menu_index == 3){
        LCDmessage(messagel, LEFT);
        LCDmessage(messager, RIGHT);
        button_index = CHECKbuttons();
        switch (button_index){
            case 1: REENTER();  //UP button - do nothing
                    break;
            case 2: menu_index = 3; //DOWN button - do nothing
                    break;
            case 3: menu_index += 1;  //NEXT button pressed, wrap to next menu
                    break;
            case 4: LOADfile();  //SELECT button pressed, start machine drawing
                    break;
            default: break;
        }
    }
 }

/***********************************
 *MENU THREE
 ***********************************/

void MENUfour(void){
    const char *messagel = "Servo";
    const char *messager = "";

    static unsigned int delay;

    LCDcommand(clear_screen);

    while(menu_index == 4){
        LCDmessage(messagel, LEFT);
        LCDmessage(messager, RIGHT);
        button_index = CHECKbuttons();
        switch (button_index){
            case 1: PENup();  //UP button - Do nothing
                    break;
            case 2: PENdown(); //DOWN button - Do nothing
                    break;
            case 3: menu_index += 1;  //NEXT button pressed, wrap to next menu
                    break;
            case 4: break;  //SELECT button pressed, do nothing
                   
            default: break;
        }
    }
 }

void MENUfive(void){
    const char *messagel = "Steppers";
    const char *messagerone = "       1";
    const char *messagerzero = "       0";

    static unsigned int delay;

    LCDcommand(clear_screen);

    while(menu_index == 5){
        LCDmessage(messagel, LEFT);

        if(MLDIR == 0){
            LCDmessage(messagerone, RIGHT);
        } else{
            LCDmessage(messagerzero, RIGHT);
        }

        button_index = CHECKbuttons();
        switch (button_index){
            case 1: while(1){     //UPbutton
                        MLSTEP = 1;
                        Delay10TCYx(step_delay);
                        MLSTEP = 0;
                        Delay10TCYx(step_delay);
                        if (UP == 1){
                            break;
                        }
                    }
                    break;
            case 2: while(1){     //UPbutton
                        MRSTEP = 1;
                        Delay10TCYx(step_delay);
                        MRSTEP = 0;
                        Delay10TCYx(step_delay);
                        if (DOWN == 1){
                            break;
                        }
                    }
                    break;
            case 3: menu_index += 1;  //NEXT button pressed, wrap to next menu
                    break;
            case 4: MLDIR = !MLDIR;
                    MRDIR = !MRDIR;
                    break;  //SELECT button pressed, do nothing

            default: break;
        }
    }
 }

void MENUsix(void){
    const char *messagel = "Test";
    int size = 1000;
    int reps = 30;
    int i = 0;
    int j = 0;

    LCDcommand(clear_screen);

    while(menu_index == 6){
        LCDmessage(messagel, LEFT);
        DISPLAYint(reps, RIGHT);

        button_index = CHECKbuttons();
        switch (button_index){
            case 1: reps += 1;
                    break;
            case 2: reps -= 1;
                    break;
            case 3: menu_index += 1;  //NEXT button pressed, wrap to next menu
                    break;
            case 4: for(i=0; i<reps; i++){
                        TESTpattern();
                    }
                    break;  //SELECT button pressed, do nothing

            default: break;
        }
    }
 }
/************************************************************
 *Loop to gets data via USART and makes the motors draw
 ****************************************************/
void LOADfile(void){
    const char *messagel = "Streamin";
    const char *messager = "g";
    static char breakflag = 0;

    LCDcommand(clear_screen);
    LCDmessage(messagel, LEFT);
    LCDmessage(messager, RIGHT);

    while(1){           //Load file loop
        Dwrite();       //Send request to advance the pen to next point
        Delay1KTCYx(point_delay);
        while(data_ready == 0){
            if(NEXT == 0){
                breakflag = 1;
                break;
            }
        }
        if(breakflag == 1){
            breakflag = 0;
            break;
        }
        PARSEdata();
        GOmotors();     //advance pen to next point, since data is ready
        data_ready = 0; //data is no longer ready
    }
    Delay1KTCYx(250); //debounce before returning to Menu 3
}

/************************************************************
 *Loop to get data via USART and make the motors draw
 ****************************************************/
void REENTER(void){
    const char *messagel = "Streamin";
    const char *messager = "g";
    static char breakflag = 0;

    LCDcommand(clear_screen);
    LCDmessage(messagel, LEFT);
    LCDmessage(messager, RIGHT);

    while(1){           //Load file loop
        Delay1KTCYx(point_delay);

        while(data_ready == 0){
            if(NEXT == 0){
                breakflag = 1;
                break;
            }
        }

        if(breakflag == 1){
            breakflag = 0;
            break;
        }

        PARSEdata();
        GOmotors();     //advance pen to next point, since data is ready
        data_ready = 0; //data is no longer ready
        Dwrite();
    }
    Delay1KTCYx(250); //debounce before returning to Menu 3
}

/*********************************************
 *Parses a line of ascii data received by USART
 *  -A line of data takes the form: daxxxxbyyyy
 *  -d signifies beginning of line (see interrupt routine)
 *  -a is direction of left motor (0 is CCW, 1 is CW)
 *  -xxxx is number of steps for left motor to move (range of 0000-9999)
 *  -b is direction of right motor (0 is CCW, 1 is CW
 *  -yyyy is number of steps for right motor to move (range of 0000-9999)
 *
 *********************************************/
void PARSEdata(void){
    static char left[5];
    static char right[5];
    unsigned char i = 0;

    for(i = 0; i != 4; i++){
        left[i] = *(rxptr+2+i);
        right[i] = *(rxptr+7+i);
    }

    left[4] = '\0';
    right[4] = '\0';

    execute_left_s = strtol(left, NULL, 10);
    execute_right_s = strtol(right, NULL, 10);
    execute_left_d = (*(rxptr+1)=='1');
    execute_right_d = (*(rxptr+6)=='1');
}

/******************************************
 *Below are functions useful for debugging over a TCP/IP network
 ***************************************/

/*****************************************
 *This sends the parsed data back to USART for debugging purposes
 *Only used for debugging
 *
 ***************************************/

void SENDinfo(void){
    while(BusyUSART());  
    WriteUSART('d');

    SENDint(execute_left_d);
    SENDint(execute_left_s);
    SENDint(execute_right_d);
    SENDint(execute_right_s);

    while(BusyUSART());  
    WriteUSART('\n');
}

/****************************************************
 *Send an ascii representation of a double to USART
 *only used for debugging
 ********************************************8*/
void SENDdouble(double number){
    char *double_buffer;
    int double_status;
    double_buffer = ftoa(number, &double_status);
    while(BusyUSART());
    putsUSART(double_buffer);
    while(BusyUSART());
    WriteUSART('\t');
}

/*******************************************************
 *Send an ascii representation of an int to USART
 *Only used for debugging
 **********************************************8*/
void SENDint(signed int number){
    signed char index = 0;
    char int_buf[6];

    if (number < 0) {
        while(BusyUSART());
        WriteUSART('-');
        number = -number;
    }
    itoa(int_buf, number, 10);
    while(isdigit(int_buf[index])){
        while(BusyUSART());
        WriteUSART(int_buf[index]);
        index += 1;
    }
    while(BusyUSART());
    WriteUSART('\t');
}


/********************************************
 *If an error occured in hardware USART, display error message via USART
 *Only used for debugging
 *****************************************/
void CHECKusartERRORS(void){
    unsigned char over_run[] = "over run\n";
    unsigned char interrupd[] = "interrupt\n";
    unsigned char frame_error[] = "frame error\n";
    unsigned char trash;

    if (RCSTAbits.OERR){
        while(BusyUSART());
        putsUSART((char *)over_run);
        RCSTAbits.CREN = 0;
        RCSTAbits.CREN = 1;
    }

    if (RCSTAbits.FERR){
        while(BusyUSART());
        putsUSART((char *)frame_error);
        trash = RCREG;
    }

    if (flag == 1){
        while(BusyUSART());
        putsUSART((char *)interrupd);
        flag = 0;
    }
}

/************************************
 *returns a double of an ascii string representing that double
 * -e.g. input is pointer to "4.5656", and output is 4.5656 (represented as a double)
 * *Only used for debugging
 ************************************/
double READstring(char * input){
    return strtod(input, NULL);
}


/************************************
 *A test pattern to check for mechanic alignment
 *************************************/
void TESTpattern(void){
    int xsize = 5000;
    int ysize = 1000;
    int i = 0;
    int j = 0;

    MLDIR = 1;
    for(j=0; j<xsize; j++){
        MLSTEP = 1;
        Delay10TCYx(step_delay);
        MLSTEP = 0;
        Delay10TCYx(step_delay);
    }

    MRDIR = 1;
    for(j=0; j<ysize; j++){
        MRSTEP = 1;
        Delay10TCYx(step_delay);
        MRSTEP = 0;
        Delay10TCYx(step_delay);
    }

    MLDIR = 0;
    for(j=0; j<xsize; j++){
        MLSTEP = 1;
        Delay10TCYx(step_delay);
        MLSTEP = 0;
        Delay10TCYx(step_delay);
    }

    MRDIR = 1;
    for(j=0; j<ysize; j++){
        MRSTEP = 1;
        Delay10TCYx(step_delay);
        MRSTEP = 0;
        Delay10TCYx(step_delay);
    }   
}