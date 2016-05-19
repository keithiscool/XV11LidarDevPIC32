/* File:   ObjectDetection.c
 * Author: Keith
 * Created on February 13, 2016, 1:46 PM
 */

//ALSO LOOK INTO "LINKED_LISTS" Referencing: http://www.cprogramming.com/tutorial/c/lesson15.html

#include <xc.h>
#include "ObjectDetection.h"

//struct ObjectNode {
//    unsigned short startOfDetectedObject;
//    unsigned short endOfDetectedObject;
//    unsigned short degree;
//    unsigned short polarDistance;
//    unsigned short qualityOfObject; //if the object is shiny or reflective, number is higher (above 100)
//    short xPos;
//    short yPos;
////    struct ObjectNode *next; //used for linked list to point to the adjacent struct
//};


bool initObjectDetection(void) {
//    arrayofDetectedObjects[OBJECT_ARRAY_STRUCT_SIZE] = emptyObjectStruct; //initialize all struct array elements to zero
    DetectedObject = emptyObjectStruct; //initialize all struct elements to zero
}


bool sendRobotLocation(short degree) {
    if( (timeFlagOneHundMilSec == true) && (RobotDetected == true) ) {
        printf("Deg: %d / Dist: %d / Qual: %d / X: %d / Y: %d\r\n",degree, DistanceArr[degree],
            QualityArr[degree], XCoordMilliMeters[degree], XCoordMilliMeters[degree]);
            timeFlagOneHundMilSec = false;
    }
}


//Check whether the change in Distance between each degree is large -> indicates object or wall
//this function compares the ditances between polar magnitudes 4-at-a-time
short objectDetection(void) {
    //temporary array degree values to remember
    short myDegrees[4] = {0, 0, 0, 0};
    short i = 0;

    //Acquire 4 distances at a time and constantly pull in data (do not print out data)
    //Exclude any data between 180 and 0 degrees because that is behind the collection bin
    //data element 176 is the last index received in the 90 packets (4 degrees per packet * 90 packets = 360 degrees total)
    //if data > 180 degrees, this check should protect against that
    if( (LIDARdecode(myDegrees) == true) && (myDegrees[0] < 178) ) { //Acquire 4 distances at a time and constantly pull in data (do not print out data)
//        printf("Lidar_Degrees_Passed_Checksum\r\n");
//
//        for(i=0;i<4;i++) {
//            printf("%u \r\n",myDegrees[i]);
//        }
            
        //run through each distance measurement in each 22 byte parsed packet from "LIDARdecode"
        for(i=0;i<4;i++) {
            //take magnitude difference from present distance measurement and the one previous (this distance will be used to detect objects if the gap between adjacent distances is large
            DistanceDifferencesArr[myDegrees[i]] = abs( DistanceArr[myDegrees[i]] - DistanceArr[myDegrees[i-1]] );
//                printf("dist_diff_calc\r\n");

            //check both of the adjacent measurements are valid (data is not zero)
//                if( DistanceDifferencesArr[myDegrees[i]] && DistanceDifferencesArr[myDegrees[i-1]] ) {
            if( (DistanceArr[myDegrees[i]] > 0) && (DistanceArr[myDegrees[i-1]] > 0) ) {
//                    printf("valid_measurements\r\n");

                //are the magnitudes different between degree measurements (first edge of object detected)
                if( (DistanceDifferencesArr[myDegrees[i]] > ObjectDetectionThreshold) && (ObjectStartEdgeDetected == false) ) { // object protruded from surrounding measurements by threshold
                    DetectedObject.startOfDetectedObject = myDegrees[i]; //found start of an object (object's corner was detected)
                    ObjectStartEdgeDetected = true; //first edge of object detected
                    printf("object_start_edge_detected\r\n");
                }

                //if a difference in magnitude is detected and there is already an object detected (last edge of object detected)
                if( (DistanceDifferencesArr[myDegrees[i]] > ObjectDetectionThreshold) && (ObjectStartEdgeDetected == true) ) { // object protruded from surrounding measurements by threshold
                    DetectedObject.endOfDetectedObject = myDegrees[i]; //found start of an object (object's corner was detected)
                    printf("object_stop_edge_detected\r\n");

                    //check if object is wide enough (are there enough degrees between the start and the start of the object?)
                    if( ( (DetectedObject.startOfDetectedObject - DetectedObject.endOfDetectedObject) > DEGREES_BETWEEN_EACH_OBJECT) ) {
                        //average and populate the data for the object into the object struct array
                        DetectedObject.qualityOfObject = (( QualityArr[DetectedObject.startOfDetectedObject] + QualityArr[DetectedObject.endOfDetectedObject] ) / 2 );
                        DetectedObject.xPos = (( XCoordMilliMeters[DetectedObject.startOfDetectedObject] + XCoordMilliMeters[DetectedObject.endOfDetectedObject] ) / 2 );
                        DetectedObject.yPos = (( YCoordMilliMeters[DetectedObject.startOfDetectedObject] + YCoordMilliMeters[DetectedObject.endOfDetectedObject] ) / 2 );
                        DetectedObject.polarDistance = (( DistanceArr[DetectedObject.startOfDetectedObject] + DistanceArr[DetectedObject.endOfDetectedObject] ) / 2 );
                        DetectedObject.degree = (( DetectedObject.startOfDetectedObject + DetectedObject.endOfDetectedObject ) / 2 );
                        DetectedObject.xPos -= X_POSITION_OFFSET_LIDAR_PLACEMENT; //correct the offset of the collection beacon (the collection beacon is not in the center of the arena)
                        printf("obj_deg: %u / obj_mag: %u\r\n",DetectedObject.degree, DetectedObject.polarDistance);
                    }

                    //Object recorded, so reset flag that detects the first edge of the object
                    ObjectStartEdgeDetected = false; //last edge of object detected (reset flag)

                }
            }
        }
    }

    //Indicate when an object is detected
    if( (timeFlagOneHundMilSec == true) && (index_object > 0) ) {
        //blinkLED(1000); //debug led to show object detected
        printf("obj\r\n deg: %4d mag: %4d\r\n", DetectedObject.degree, DetectedObject.polarDistance);
        LATBbits.LATB10 ^= 1; //toggle on LED to signify a detected object with LED_B10 (LED 2)
        timeFlagOneHundMilSec = false;
    }
    
    return 1;
}
