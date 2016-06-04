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


//Send Object Detection Info to Navigation Processor on Robot
bool sendRobotLocation(void) {
    if(timeFlagOneHundMilSecObjDet == true) {

//        printf("Deg: %d / Dist: %d / Qual: %d / X: %d / Y: %d\r\n",degree, DistanceArr[degree],
//            QualityArr[degree], XCoordMilliMeters[degree], XCoordMilliMeters[degree]);
    
        printf("----\r\n");
        printf("obj_deg: %d / obj_mag: %d / X: %d / Y: %d / Quality: %d\r\n",
                DetectedObject.degree, DetectedObject.polarDistance, DetectedObject.xPos, DetectedObject.yPos, DetectedObject.qualityOfObject);
        printf("start: %d / stop: %d \r\n",DetectedObject.startOfDetectedObject, DetectedObject.endOfDetectedObject);

        LATBbits.LATB10 ^= 1; //toggle on LED to signify a detected object with LED_B10 (LED 2)

        timeFlagOneHundMilSecObjDet = false;
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
            //only take certain degree measurements and valid distance measurements
            if( (DistanceArr[myDegrees[i]] > minDistanceAllowed) && (myDegrees[i] > objectTrackLower) && (myDegrees[i] < objectTrackUpper) ) {
                presentDegree = myDegrees[i];

                //take magnitude difference from present distance measurement and the one previous (this distance will be used to detect objects if the gap between adjacent distances is large
                DistanceDifferencesArr[myDegrees[i]] = abs( DistanceArr[presentDegree] - DistanceArr[lastDegree] );

                //are the magnitudes different between degree measurements (first edge of object detected)
                if( (DistanceDifferencesArr[myDegrees[i]] > ObjectDetectionThreshold) && (ObjectStartEdgeDetected == false) ) { // object protruded from surrounding measurements by threshold
                    DetectedObject.startOfDetectedObject = myDegrees[i]; //found start of an object (object's corner was detected)
                    ObjectStartEdgeDetected = true; //first edge of object detected
//                    printf("Deg: %d starting_edge\r\n", DetectedObject.startOfDetectedObject);
                }

                //if a difference in magnitude is detected and there is already an object detected (last edge of object detected)
                else if( (DistanceDifferencesArr[myDegrees[i]] > ObjectDetectionThreshold) && (ObjectStartEdgeDetected == true) && (myDegrees[i] != DetectedObject.startOfDetectedObject) ) { // object protruded from surrounding measurements by threshold
                    DetectedObject.endOfDetectedObject = myDegrees[i]; //found end of an object (object's corner was detected)
//                    printf("Deg: %d stop_edge\r\n", DetectedObject.endOfDetectedObject);

                    
            //NOTE: THIS BROKE THE CODE (THIS CONDITION NEVER PASSED... NOT SURE WHY)
//                    check if object is wide enough (are there enough degrees between the start and the start of the object?)
//                    if( ( ( abs(DetectedObject.startOfDetectedObject - DetectedObject.endOfDetectedObject) ) > DEGREES_BETWEEN_EACH_OBJECT) ) {

                    //average and populate the data for the object into the object struct array
                    DetectedObject.polarDistance = (( DistanceArr[DetectedObject.startOfDetectedObject] + DistanceArr[DetectedObject.endOfDetectedObject] ) / 2 );
                    DetectedObject.qualityOfObject = (( QualityArr[DetectedObject.startOfDetectedObject] + QualityArr[DetectedObject.endOfDetectedObject] ) / 2 );

////THANK YOU TO RYAN FOR REALIZING THE POSSIBILITY FOR THE TRIG FUNCTIONS USING RADIANS AND NOT DEGREES
                        //CONVERT TO RADIANS: RADIANS == (deg/2) * M_PI / 180 == deg * M_PI / 90
                        //also divide "deg" by 2 to get the center of DetectedObject.startOfDetectedObject and DetectedObject.endOfDetectedObject
                        DetectedObject.degree = ( ( DetectedObject.startOfDetectedObject + DetectedObject.endOfDetectedObject ) * M_PI ) / 90;

                        //Do the math using floating point math to reduce error (fixed point math was not working for cosine terms and had +/- 10to50mm sometimes
//                        float oldY = ((float)DetectedObject.polarDistance) * sin(DetectedObject.degree);
                        DetectedObject.yPos = (short)( (double)DetectedObject.polarDistance * (double)sin(DetectedObject.degree) );
//                        float oldX = ((float)DetectedObject.polarDistance) * cos(DetectedObject.degree);
                        DetectedObject.xPos = (short)( (double)DetectedObject.polarDistance * (double)cos(DetectedObject.degree) );
//                        printf("OldX: %f / NewX: %d / OldY: %f / NewY: %d\r\n", oldX, DetectedObject.xPos, oldY, DetectedObject.yPos);
                        DetectedObject.xPos -= X_POSITION_OFFSET_LIDAR_PLACEMENT; //correct the offset of the collection beacon (the collection beacon is not in the center of the arena)

                        //check theat the entire object is located within the forward facing 180 degrees (prevent the larger degrees from only see one edge of a new object)
                        if(DetectedObject.startOfDetectedObject < DetectedObject.endOfDetectedObject) {
                            //print out detected object and send the data to the navigation processor on the robot from the beacon
                            sendRobotLocation();
                        }
//                    }

                    //Object recorded, so reset flag that detects the first edge of the object
                    ObjectStartEdgeDetected = false; //last edge of object detected (reset flag)
                }
            }
            
            lastDegree = presentDegree;
        }
    }
  
    return 1;
}
