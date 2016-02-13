/* File:   main.h
 * Author: Igor
 * Created on July 3, 2015, 1:14 PM
 */

#ifndef MAIN_H
#define	MAIN_H

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
#pragma config CP = OFF                 // Code Protect (Protection Disabled)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#define RUNMODE true
#define TESTMODE false
bool operationMode = TESTMODE;

#define LCD_SCALING 4 // scale lcd x,y distances by 16
//#define LCD_SCALING 5 // scale lcd x,y distances by 32


extern void initialize(void);
extern bool timeFlag;


unsigned short returned_data[10];

extern bool Done_Printing_Debug(void);
bool FirstTimePrinting = true;
unsigned int DATA[4];
short i = 0;

void delay(void);
unsigned int v = 0;

extern short AnglesCoveredTotal;
extern unsigned int SuccessfulMeasurements[360];

extern float returned_speed;
extern unsigned char InvalidFlag[4];
extern unsigned char WarningFlag[4];
extern unsigned short Distance[360];

extern short XCoordMeters[360];
extern short YCoordMeters[360];
extern unsigned short DegreeIndex;
extern unsigned short GoodReadings;
extern unsigned short BadReadings;

#endif	/* MAIN_H */

