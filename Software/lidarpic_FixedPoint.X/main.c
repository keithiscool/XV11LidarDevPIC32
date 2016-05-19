 /* File:   main.c
 * Author: Igor
 * Created on July 3, 2015, 1:11 PM
 */
#include <xc.h>
#include <stdbool.h>
#include <stdio.h>
#include "main.h"
#include <math.h>
#include <string.h>


// PIC32MX575F512H Configuration Bit Settings

// 'C' source line config statements

// DEVCFG3
// USERID = No Setting
#pragma config FSRSSEL = PRIORITY_7     // SRS Select (SRS Priority 7)
#pragma config FCANIO = OFF             // CAN I/O Pin Select (Alternate CAN I/O)
#pragma config FUSBIDIO = OFF           // USB USID Selection (Controlled by Port Function)
#pragma config FVBUSONIO = OFF          // USB VBUS ON Selection (Controlled by Port Function)

// DEVCFG2
#pragma config FPLLIDIV = DIV_6         // PLL Input Divider (6x Divider)
#pragma config FPLLMUL = MUL_20         // PLL Multiplier (20x Multiplier)
#pragma config UPLLIDIV = DIV_6         // USB PLL Input Divider (6x Divider)
#pragma config UPLLEN = ON              // USB PLL Enable (Enabled)
#pragma config FPLLODIV = DIV_1         // System PLL Output Clock Divider (PLL Divide by 1)

// DEVCFG1
#pragma config FNOSC = PRIPLL           // Oscillator Selection Bits (Primary Osc w/PLL (XT+,HS+,EC+PLL))
#pragma config FSOSCEN = OFF            // Secondary Oscillator Enable (Disabled)
#pragma config IESO = OFF               // Internal/External Switch Over (Disabled)
#pragma config POSCMOD = HS             // Primary Oscillator Configuration (HS osc mode)
#pragma config OSCIOFNC = OFF           // CLKO Output Signal Active on the OSCO Pin (Disabled)
#pragma config FPBDIV = DIV_1           // Peripheral Clock Divisor (Pb_Clk is Sys_Clk/1)
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor Selection (Clock Switch Disable, FSCM Disabled)
#pragma config WDTPS = PS1048576        // Watchdog Timer Postscaler (1:1048576)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (WDT Disabled (SWDTEN Bit Controls))

// DEVCFG0
#pragma config DEBUG = ON               // Background Debugger Enable (Debugger is enabled)
#pragma config ICESEL = ICS_PGx1        // ICE/ICD Comm Channel Select (ICE EMUC1/EMUD1 pins shared with PGC1/PGD1)
#pragma config PWP = OFF                // Program Flash Write Protect (Disable)
#pragma config BWP = OFF                // Boot Flash Write Protect bit (Protection Disabled)
#pragma config CP = OFF
// Code Protect (Protection Disabled)



void delay(void){
    for(v=0; v<60000; v++);
}


void LEDmsBlinkHundMS(void) {
    if(timeFlagOneHundMilSec) {
        LATBbits.LATB9 ^= 1;
//        LATBbits.LATB10 ^= 1;
//        LATBbits.LATB11 ^= 1;
//        LATBbits.LATB12 ^= 1;
//        LATBbits.LATB13 ^= 1;
    }
    timeFlagOneHundMilSec = false; //reset timer flag
}



void main(void){

    initialize();
    printf("PIC_RESET\r\n");

    
    //Initialize all distances to '99' so I can see which ones are not getting written to
    for(i=0;i<181;i++) {
        DistanceArr[i] = 1;
    }
    
    //initialize arrays for object detection
    initObjectDetection();

    //Get some distance measurements before using the object detection functions
    unsigned short test[4];
    while( (LidarCalcPerm != true) && (timeFlagFiveSec != true) ) {
        //RUN THE FOLLOWING FUNCTION TO PARSE AND STORE THE DATA (WILL NOT PRINT THE DEBUG DATA AND WILL NOT LOOK FOR OBJECTS)
        LIDARdecode(test); //simply call to parse the Lidar data (4 distance measurements at a time)
    }
    timeFlagFiveSec = false; //reset timer flag

    LEDmsBlinkHundMS();

    //Gather data for 5 seconds, then use object detection function (need to populate as much data as possible at the beginning of code before while() loop
    while(timeFlagFiveSec != true);
    timeFlagFiveSec = false; //reset timer flag

    LEDmsBlinkHundMS();
    
    while(1) {

//RUN ONE OF THE FOLLOWING FUNCTIONS TO PARSE AND PRINT DATA TO UART 6
        if(debugLidarPolarData() == true) {
            LEDmsBlinkHundMS();
        }
        if(debugLidarCartesianData() == true) {
            LEDmsBlinkHundMS();
        }


////parse the data 4 measurements at a time and use the parsed distance data from the lidar to locate objects
//        if(distDiffObjectDetection() == true) {
//            LEDmsBlinkHundMS();
//        }




////NOT USING DMA FOR SMALL DATA TRANSMISSIONS
//        //kick the dma to UART 6 if the buffer exceeds the second level count
//        if(queue_send() == true) {
//            LATBbits.LATB10 ^= 1;    //toggle on LED 2
//        }
    }
}


static enum
{
    EXCEP_IRQ = 0, // interrupt
    EXCEP_AdEL = 4, // address error exception (load or ifetch)
    EXCEP_AdES, // address error exception (store)
    EXCEP_IBE, // bus error (ifetch)
    EXCEP_DBE, // bus error (load/store)
    EXCEP_Sys, // syscall
    EXCEP_Bp, // breakpoint
    EXCEP_RI, // reserved instruction
    EXCEP_CpU, // coprocessor unusable
    EXCEP_Overflow, // arithmetic overflow
    EXCEP_Trap, // trap (possible divide by zero)
    EXCEP_IS1 = 16, // implementation specfic 1
    EXCEP_CEU, // CorExtend Unuseable
    EXCEP_C2E // coprocessor 2
} _excep_code;


static unsigned int _epc_code;
static unsigned int _excep_addr;


// this function overrides the normal _weak_ generic handler
void _general_exception_handler(void) {
    asm volatile("mfc0 %0,$13" : "=r" (_excep_code));
    asm volatile("mfc0 %0,$14" : "=r" (_excep_addr));
    _excep_code = (_excep_code & 0x0000007C) >> 2;

    while (1)
    {
        LATBbits.LATB9 = 1;
        LATBbits.LATB10 = 1;
        LATBbits.LATB11 = 1;
        LATBbits.LATB12 = 0;
        LATBbits.LATB13 = 0;

        // Examine _excep_code to identify the type of exception
        // Examine _excep_addr to find the address that caused the exception
    }
}