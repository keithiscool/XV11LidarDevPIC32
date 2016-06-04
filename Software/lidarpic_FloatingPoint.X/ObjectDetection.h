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
extern bool timeFlagOneHundMilSecObjDet;

//variables and parsing function from lidar_decoder.c
extern short QualityArr[181];
extern short DistanceArr[181];
extern short PreviousDistanceArr[181];
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
    short startOfDetectedObject;
    short endOfDetectedObject;
    short degree;
    short polarDistance;
    short qualityOfObject; //if the object is shiny or reflective, number is higher (above 100)
    short xPos;
    short yPos;
//    struct ObjectNode *next; //used for linked list to point to the adjacent struct
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


//Using Linked List
//struct ObjectNode *root; //This won't change, or we would lose the list in memory (this is the first
//struct ObjectNode *conductor; //This will point to each node as it traverses the list




////Object Detection "Objects" using C programming (cannot use classes in C)
//typedef struct DetectedRock{
//    short xPos;
//    short yPos;
//    short size;
//};
//
//typedef struct DetectedCrater{
//    short xPos;
//    short yPos;
//    short craterRadius;
//    short craterDepth; //crater depth is approximately the difference in polar ditances while sweeping lidar vertically
//};
//
//typedef struct DetectedRobot{
//    short x;
//    short y;
//    short startingXPos;
//    short startingYPos;
//};


#endif	/* OBJECTDETECTION_H */

