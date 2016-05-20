/* 
 * File:   Servos.h
 * Author: Dana
 *
 * Created on May 12, 2016, 7:54 PM
 */
#include <stdbool.h>



#ifndef SERVOS_H
#define	SERVOS_H

#ifdef	__cplusplus
extern "C" {
#endif
#define MIDDLE_X    511
#define BUFFER_X    100
#define DEGREE      13.8888888888888889
#define DEGREE180   4986 // 5000- buffer of 14
#define DEGREE0     2514 // 2500+ buffer of 14
#define SERVO1 0
#define SERVO2 1
#define SERVO_STEP_WIDE 15
#define SERVO_MAX 180
#define SERVO_MIN 0
#define INVALID 1023


extern unsigned char angle[2]; // 0 degrees to start for pwm
extern unsigned short anglex1;
extern unsigned short anglex2;
extern bool clockwise[2]; //false = left, true = right
extern bool beaconExists[2] ;
extern bool setHigh1;
extern bool setHigh2;

//extern void determineAngle(bool haveBlob, short blobX, unsigned char servo);
extern int isAbout(int compareThis, int toThis, int range);
extern void seekBeacon(int cameraNumber, int result);
extern unsigned char getAngle1(void);
extern unsigned char getAngle2(void);
extern void setAngle1(short d);
extern void setAngle2(short d);

#ifdef	__cplusplus
}
#endif

#endif	/* SERVOS_H */

