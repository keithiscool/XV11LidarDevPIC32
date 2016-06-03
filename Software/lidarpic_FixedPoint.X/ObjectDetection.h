/* File:   ObjectDetection.h
 * Author: Keith
 * Created on February 13, 2016, 1:46 PM
 */

#ifndef OBJECTDETECTION_H
#define	OBJECTDETECTION_H

#include <stdbool.h>
#include <stdio.h>      /* printf, scanf, puts */
#include <stdlib.h>     /* realloc, free, exit, NULL */
#include "defs.h"
#include "math.h"
//#include "interrupt_handler.h"

//timer usage flags
extern bool timeFlagOneHundMilSec;
extern bool timeFlagFiveSec;

//variables and parsing function from lidar_decoder.c
//Arrays are only 181 in length to get "arena facing" 180 degrees, inclusive
extern short QualityArr[181];
extern short DistanceArr[181];
extern short PreviousDistancesAvgSum[181];
extern short XCoordMilliMeters[181];
extern short YCoordMilliMeters[181];
extern bool LIDARdecode(short getDegrees[4]);

//functions in ObjectDetection.c
//extern short objectDetection(void);
//extern bool initObjectDetection(void);
//extern bool sendRobotLocation(short degree);

//used to detect differences between adjacenet degree elements
unsigned short DistanceDifferencesArr[181];


struct ObjectNode {
    unsigned short startOfDetectedObject;
    unsigned short endOfDetectedObject;
    unsigned short degree;
    short polarDistance;
    unsigned short qualityOfObject; //if the object is shiny or reflective, number is higher (above 100)
    short xPos;
    short yPos;
    short velocity;
    char orientation; //North, South, East, West
};


//Empty Object struct used to "reset to zero" the arrayofDetectedObjects[] struct array elements
static struct ObjectNode emptyObjectStruct = {0,0,0,0,0,0,0};
//Declaration of discovered objects "array of structs"
//struct ObjectNode arrayofDetectedObjects[OBJECT_ARRAY_STRUCT_SIZE];
struct ObjectNode DetectedObject;
//keeps track of number of objects detected
unsigned short index_object = 0;


//flag used to designate a corner of an object has been detected (object is closer than surroundings)
static bool ObjectStartEdgeDetected = 0;

//flag to signify the object (Robot CHRISTEE) is found
bool RobotDetected = false;



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////Kalman filter variables:
//
//struct kalmanFilter {
//    float P[2][2]; // Error covariance matrix - This is a 2x2 matrix
//    float Q_angle; // Process noise variance for the accelerometer
//    float Q_bias; // Process noise variance for the gyro bias
//    float R_measure; // Measurement noise variance - this is actually the variance of the measurement noise
//    float rateOfMovement; // Unbiased rate calculated from the rate and the calculated bias - you have to call getAngle to update the rate
//    float locationOfRobot;
//};
//
//extern void initKalmanFilter(void);


#endif	/* OBJECTDETECTION_H */

