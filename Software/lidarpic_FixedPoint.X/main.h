/* File:   main.h
 * Author: Igor
 * Created on July 3, 2015, 1:14 PM
 */

#ifndef MAIN_H
#define	MAIN_H

//#define PI 3.14159265358

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
int i = 0;

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

