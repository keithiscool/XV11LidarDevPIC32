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
#include "interrupt_handler.h"

//100ms timer using
extern bool timeFlag;

//variables and parsing function from lidar_decoder.c
extern unsigned short QualityArr[360];
extern unsigned short DistanceArr[360];
extern unsigned short PreviousDistanceArr[360];
extern short XCoordMeters[360];
extern short YCoordMeters[360];
extern bool LIDARdecode(short offsetDegrees, unsigned short getDegrees[4]);


extern short objectDetection(void);
extern bool initObjectDetection(void);

//used to detect differences between adjacenet degree elements
unsigned short DistanceDifferencesArr[360];


struct ObjectNode {
    unsigned short startOfDetectedObject;
    unsigned short endOfDetectedObject;
    unsigned short Degree;
    unsigned short PolarDistance;
    short xPos;
    short yPos;
//    struct ObjectNode *next; //used for linked list to point to the adjacent struct
};

//Empty struct used to "reset to zero" the arrayofDetectedObjects[] struct array elements
static struct ObjectNode emptyObjectStruct = {0,0,0,0,0,0};


//Declaration of discovered objects "array of structs"
struct ObjectNode arrayofDetectedObjects[OBJECT_ARRAY_STRUCT_SIZE];
//keeps track of number of objects detected
unsigned short index_object = 0;


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

