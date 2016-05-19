/* File:   ObjectDetection.c
 * Author: Keith
 * Created on February 13, 2016, 1:46 PM
 */

//ALSO LOOK INTO "LINKED_LISTS" Referencing: http://www.cprogramming.com/tutorial/c/lesson15.html

#include <xc.h>
#include "ObjectDetection.h"


bool initObjectDetection(void) {
    arrayofDetectedObjects[OBJECT_ARRAY_STRUCT_SIZE] = emptyObjectStruct; //initialize all struct array elements to zero
}


//Check whether the change in Distance between each degree is large -> indicates object or wall
//this function compares the ditances between polar magnitudes 4-at-a-time
short objectDetection(void) {
    //temporary degree values to remember
    short myDegrees[4] = {0, 0, 0, 0};
    unsigned short i = 0;


//    if( (LIDARdecode(myDegrees) == true) && (myDegrees < 180) ) { //Acquire 4 distances at a time and constantly pull in data (do not print out data)
    if(LIDARdecode(myDegrees) == true) { //Acquire 4 distances at a time and constantly pull in data (do not print out data)

//        printf("Lidar_true\r\n");
//
//        for(i=0;i<4;i++) {
//            printf("%u \r\n",myDegrees[i]);
//        }

        //check that the object detection array of structs is not exceeding the limit
        if(index_object < OBJECT_ARRAY_STRUCT_SIZE) {
            
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
                        arrayofDetectedObjects[index_object].startOfDetectedObject = myDegrees[i]; //found start of an object (object's corner was detected)
                        ObjectStartEdgeDetected = true; //first edge of object detected
                        printf("object_start_edge_detected\r\n");
                    }

                    //if a difference in magnitude is detected and there is already an object detected (last edge of object detected)
                    if( (DistanceDifferencesArr[myDegrees[i]] > ObjectDetectionThreshold) && (ObjectStartEdgeDetected == true) ) { // object protruded from surrounding measurements by threshold
                        arrayofDetectedObjects[index_object].endOfDetectedObject = myDegrees[i]; //found start of an object (object's corner was detected)
                        printf("object_stop_edge_detected\r\n");
                        
                        //check if object is wide enough (are there enough degrees between the start and the start of the object?)
                        if( ( (arrayofDetectedObjects[index_object].startOfDetectedObject - arrayofDetectedObjects[index_object].endOfDetectedObject) > DEGREES_BETWEEN_EACH_OBJECT) ) {
                            //average and populate the data for the object into the object struct array
                            arrayofDetectedObjects[index_object].qualityOfObject = (( QualityArr[arrayofDetectedObjects[index_object].startOfDetectedObject] + QualityArr[arrayofDetectedObjects[index_object].endOfDetectedObject] ) / 2 );
                            arrayofDetectedObjects[index_object].xPos = (( XCoordMeters[arrayofDetectedObjects[index_object].startOfDetectedObject] + XCoordMeters[arrayofDetectedObjects[index_object].endOfDetectedObject] ) / 2 );
                            arrayofDetectedObjects[index_object].yPos = (( YCoordMeters[arrayofDetectedObjects[index_object].startOfDetectedObject] + YCoordMeters[arrayofDetectedObjects[index_object].endOfDetectedObject] ) / 2 );
                            arrayofDetectedObjects[index_object].polarDistance = (( DistanceArr[arrayofDetectedObjects[index_object].startOfDetectedObject] + DistanceArr[arrayofDetectedObjects[index_object].endOfDetectedObject] ) / 2 );
                            arrayofDetectedObjects[index_object].degree = (( arrayofDetectedObjects[index_object].startOfDetectedObject + arrayofDetectedObjects[index_object].endOfDetectedObject ) / 2 );
                            arrayofDetectedObjects[index_object].xPos -= X_POSITION_OFFSET_LIDAR_PLACEMENT; //correct the offset of the collection beacon (the collection beacon is not in the center of the arena)
                            printf("obj_deg: %u / obj_mag: %u\r\n",arrayofDetectedObjects[index_object].degree, arrayofDetectedObjects[index_object].polarDistance);
                        }

                        //CHECK IF THE LAST DETECTED OBJECT IS NOT IN THE SAME DEGREE PATH AS THE NEW DETECTED OBJECT AND THE OBJECT IS NOT IMMEDIATELY NEXT TO THE LAST OBJECT (avoid counting an object twice)
                        if( (arrayofDetectedObjects[index_object-1].degree - arrayofDetectedObjects[index_object-1].degree) > DEGREES_BETWEEN_EACH_OBJECT ) {
                            if(index_object < OBJECT_ARRAY_STRUCT_SIZE) {
                                index_object++; //one object has been found, move to next struct array "arrayofDetectedObjects[30] element to populate next set of data when next object is found
                                printf("object++: %u\r\n",index_object);
                                LATBbits.LATB11 ^= 1; //turn on LED
                            }else {
                                index_object = OBJECT_ARRAY_STRUCT_SIZE; //saturate the value if too many objects are detected
                                printf("Too many objects\r\n");
                            }
                        }

                        //Object recorded, so reset flag that detects the first edge of the object
                        ObjectStartEdgeDetected = false; //last edge of object detected (reset flag)

                    }
                }
            }
        }
    }

    //Indicate when an object is detected
    if((timeFlagOneHundMilSec == true) && (index_object > 0)) {
        //blinkLED(1000); //debug led to show object detected
        printf("%d objects detected\r\n",index_object);
//        LATBbits.LATB9 ^= 1; //toggle on LED
        LATBbits.LATB10 ^= 1; //toggle on LED
//        LATBbits.LATB11 ^= 1; //toggle on LED
//        LATBbits.LATB12 ^= 1; //toggle on LED
//        LATBbits.LATB13 ^= 1; //toggle on LED
        timeFlagOneHundMilSec = false;
    }
    
    return 1;
}
