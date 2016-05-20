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
short distDiffObjectDetection(void) {
    //temporary array degree values to remember
    short myDegrees[4] = {0, 0, 0, 0};
    short i = 0;
    //track object with degrees that are not adjacent (I have too many zeros as distances in output)
    static short lastDegree = 0;
    static short presentDegree = 0;

    //Acquire 4 distances at a time and constantly pull in data (do not print out data)
    //Exclude any data between 180 and 0 degrees because that is behind the collection bin
    //data element 176 is the last index received in the 90 packets (4 degrees per packet * 90 packets = 360 degrees total)
    //if data > 180 degrees, this check should protect against that
    if(LIDARdecode(myDegrees) == true) {//&& (myDegrees[0] < 178) ) { //Acquire 4 distances at a time and constantly pull in data (do not print out data)
//        printf("distDiffObjectDetection\r\n");
//
            
        //run through each distance measurement in each 22 byte parsed packet from "LIDARdecode"
        for(i=0;i<4;i++) {
            
            if( (DistanceArr[myDegrees[i]] > minDistanceAllowed) && ( (myDegrees[i] > objectTrackLower) && (myDegrees[i] < objectTrackUpper) ) ) {
                presentDegree = myDegrees[i];
//                    printf("valid_measurements\r\n");

                //take magnitude difference from present distance measurement and the one previous (this distance will be used to detect objects if the gap between adjacent distances is large
                DistanceDifferencesArr[myDegrees[i]] = abs( DistanceArr[presentDegree] - DistanceArr[lastDegree] );
//                printf("dist_diff_calc\r\n");

                //are the magnitudes different between degree measurements (first edge of object detected)
                if( (DistanceDifferencesArr[myDegrees[i]] > ObjectDetectionThreshold) && (ObjectStartEdgeDetected == false) ) { // object protruded from surrounding measurements by threshold
                    DetectedObject.startOfDetectedObject = myDegrees[i]; //found start of an object (object's corner was detected)
                    ObjectStartEdgeDetected = true; //first edge of object detected
                    printf("Deg: %d starting_edge\r\n", DetectedObject.startOfDetectedObject);
                }

                //if a difference in magnitude is detected and there is already an object detected (last edge of object detected)
                if( (DistanceDifferencesArr[myDegrees[i]] > ObjectDetectionThreshold) && (ObjectStartEdgeDetected == true) ) { // object protruded from surrounding measurements by threshold
                    DetectedObject.endOfDetectedObject = myDegrees[i]; //found start of an object (object's corner was detected)
                    printf("Deg: %d stop_edge\r\n", DetectedObject.endOfDetectedObject);

                    //check if object is wide enough (are there enough degrees between the start and the start of the object?)
//                    if( ( ( abs(DetectedObject.startOfDetectedObject - DetectedObject.endOfDetectedObject) ) > DEGREES_BETWEEN_EACH_OBJECT) ) {
                        //average and populate the data for the object into the object struct array
                        DetectedObject.polarDistance = (( DistanceArr[DetectedObject.startOfDetectedObject] + DistanceArr[DetectedObject.endOfDetectedObject] ) / 2 );

                        ////THANK YOU TO RYAN FOR REALIZING THE POSSIBILITY FOR THE TRIG FUNCTIONS USING RADIANS AND NOT DEGREES
                        //CONVERT TO RADIANS: RADIANS == deg * M_PI / 180.0;
                        DetectedObject.degree = (( DetectedObject.startOfDetectedObject + DetectedObject.endOfDetectedObject ) / 2 );
                        DetectedObject.degree = ( ( ( DetectedObject.startOfDetectedObject + DetectedObject.endOfDetectedObject ) * M_PI ) / 360 );

                        DetectedObject.qualityOfObject = (( QualityArr[DetectedObject.startOfDetectedObject] + QualityArr[DetectedObject.endOfDetectedObject] ) / 2 );

                        DetectedObject.yPos = DetectedObject.polarDistance * cos(DetectedObject.degree);
                        DetectedObject.xPos = DetectedObject.polarDistance * sin(DetectedObject.degree);
                        DetectedObject.xPos -= X_POSITION_OFFSET_LIDAR_PLACEMENT; //correct the offset of the collection beacon (the collection beacon is not in the center of the arena)

                        printf("obj_deg: %d / obj_mag: %d / X: %d / Y: %d\r\n",DetectedObject.degree, DetectedObject.polarDistance, DetectedObject.xPos, DetectedObject.yPos);
                        printf("start: %d / stop: %d \r\n",DetectedObject.startOfDetectedObject, DetectedObject.endOfDetectedObject);
//                    }

                    //Object recorded, so reset flag that detects the first edge of the object
                    ObjectStartEdgeDetected = false; //last edge of object detected (reset flag)
                }
            }
            lastDegree = presentDegree;
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
