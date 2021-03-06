/* File:   main.h
 * Author: Igor
 * Created on July 3, 2015, 1:14 PM
 */

#ifndef MAIN_H
#define	MAIN_H

#include "interrupt_handler.h"


extern void initialize(void);

extern bool timeFlagFiveSec;
extern bool timeFlagOneHundMilSec;

unsigned short returned_data[10];


bool FirstTimePrinting = true;
unsigned int DATA[4];
unsigned int i = 0;

void delay(void);
unsigned int v = 0;

//////////////////////////////////////////////////////////////////////////////
//LIDAR DATA STUFF

extern bool LidarCalcPerm;
extern bool debugLidarPolarData(void);
extern bool debugLidarCartesianData(void);
extern bool initObjectDetection(void);
extern bool distDiffObjectDetection(void);
extern short objectDetection(void);
//
extern short AnglesCoveredTotal;

//extern float returned_speed;
extern short DistanceArr[360];

//////////////////////////////////////////////////////////////////////////////



#endif	/* MAIN_H */

