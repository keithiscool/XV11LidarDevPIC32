/* File:   ObjectDetection.c
 * Author: Keith
 * Created on February 13, 2016, 1:46 PM
 */

//ALSO LOOK INTO "LINKED_LISTS" Referencing: http://www.cprogramming.com/tutorial/c/lesson15.html

#include <xc.h>
#include "ObjectDetection.h"


/* MOTOVATION FOR USING LINKED LIST: NEED VARIABLE ARRAY OF STRUCTS "ObjectNode" SIZE FOR EACH DISCOVERED OBJECT LIDAR SEES
 * Let's imagine a conductor who can only enter the train through the first car and
 * can walk through the train down the line as long as the connector connects to
 * another car. This is how the program will traverse the linked list. The conductor
 * will be a pointer to node, and it will first point to root, and then, if the
 * root's pointer to the next node is pointing to something, the "conductor"
 * (not a technical term) will be set to point to the next node. In this fashion,
 * the list can be traversed. Now, as long as there is a pointer to something,
 * the traversal will continue. Once it reaches a null pointer (or dummy node),
 * meaning there are no more nodes (train cars) then it will be at the end of the
 * list, and a new node can subsequently be added if so desired.*/

//From the "C-Programming Reference", linked lists are similar to a conductor moving between train cars
//short linkedListSetup(struct ) {
//
//
//
//    root = malloc( sizeof(struct ObjectNode) );
//    root->next = 0;
//    conductor = root; //start the beginning of the
//    if ( conductor != 0 ) {
//        while ( conductor->next != 0) {
//            conductor = conductor->next;
//        }
//    }
//    /* Creates a node at the end of the list */
//    conductor->next = malloc( sizeof(struct ObjectNode) );
//    conductor = conductor->next;
//
//    if ( conductor == 0 ) {
//        printf( "Out of memory" );
//        return 0;
//    }
//    /* initialize the new memory */
//    conductor->next = 0;
//
//    return 0;
//}


bool initObjectDetection(void) {
    
    arrayofDetectedObjects[OBJECT_ARRAY_STRUCT_SIZE] = emptyObjectStruct; //initialize all struct array elements to zero
}



//Check whether the change in Distance between each degree is large -> indicates object or wall
//this function compares the ditances between polar magnitudes 4-at-a-time
short objectDetection(void) {
    //temporary degree values to remember
    unsigned short myDegrees[4] = {0, 0, 0, 0};
    unsigned short *DegreeCPY;
    DegreeCPY = myDegrees;
    unsigned short i = 0;
    unsigned short offset = 90; //offset lidar rotation by 90 degrees to get it to used forward facing data from 0 to 180 degrees
    static bool ObjectStartDetected = 0;
    

    if(LIDARdecode(offset, DegreeCPY) == true) { //Acquire 4 distances at a time and constantly pull in data (do not print out data)

        //check that the object detection array of structs is not exceeding the limit
        if(index_object < OBJECT_ARRAY_STRUCT_SIZE) {
            
            //run through each distance measurement in each 22 byte parsed packet from "LIDARdecode"
            for(i=0;i<4;i++) {
                //take magnitude difference from present distance measurement and the one previous (this distance will be used to detect objects if the gap between adjacent distances is large
                DistanceDifferencesArr[myDegrees[i]] = abs( DistanceArr[myDegrees[i]] - DistanceArr[myDegrees[i-1]] );

                //check both of the adjacent measurements are valid (data is not zero)
                if( DistanceDifferencesArr[myDegrees[i]] && DistanceDifferencesArr[myDegrees[i-1]] ) {

                    //are the magnitudes different between degree measurements (first edge of object detected)
                    if( (DistanceDifferencesArr[myDegrees[i]] > ObjectDetectionThreshold) && (ObjectStartDetected == false) ) { // object protruded from surrounding measurements by threshold
                        arrayofDetectedObjects[index_object].startOfDetectedObject = myDegrees[i]; //found start of an object (object's corner was detected)
                        ObjectStartDetected = true; //first edge of object detected
                    }

                    //if a difference in magnitude is detected and there is already an object detected (last edge of object detected)
                    if( (DistanceDifferencesArr[myDegrees[i]] > ObjectDetectionThreshold) && (ObjectStartDetected == true) ) { // object protruded from surrounding measurements by threshold
                        arrayofDetectedObjects[index_object].endOfDetectedObject = myDegrees[i]; //found start of an object (object's corner was detected)

                        //check if object is wide enough (are there enough degrees between the start and the start of the object?)
                        if( ((arrayofDetectedObjects[index_object].startOfDetectedObject - arrayofDetectedObjects[index_object].endOfDetectedObject) > DEGREES_BETWEEN_EACH_OBJECT) ) {
                            //average and populate the data for the object into the object struct array
                            arrayofDetectedObjects[index_object].qualityOfObject = (( QualityArr[arrayofDetectedObjects[index_object].startOfDetectedObject] + QualityArr[arrayofDetectedObjects[index_object].endOfDetectedObject] ) / 2 );
                            arrayofDetectedObjects[index_object].xPos = (( XCoordMeters[arrayofDetectedObjects[index_object].startOfDetectedObject] + XCoordMeters[arrayofDetectedObjects[index_object].endOfDetectedObject] ) / 2 );
                            arrayofDetectedObjects[index_object].yPos = (( YCoordMeters[arrayofDetectedObjects[index_object].startOfDetectedObject] + YCoordMeters[arrayofDetectedObjects[index_object].endOfDetectedObject] ) / 2 );
                            arrayofDetectedObjects[index_object].polarDistance = (( DistanceArr[arrayofDetectedObjects[index_object].startOfDetectedObject] + DistanceArr[arrayofDetectedObjects[index_object].endOfDetectedObject] ) / 2 );
                            arrayofDetectedObjects[index_object].degree = (( arrayofDetectedObjects[index_object].startOfDetectedObject + arrayofDetectedObjects[index_object].endOfDetectedObject ) / 2 );
                        }

                        ObjectStartDetected = false; //last edge of object detected (reset flag)

                        //CHECK IF THE LAST DETECTED OBJECT IS NOT IN THE SAME DEGREE PATH AS THE NEW DETECTED OBJECT AND THE OBJECT IS NOT IMMEDIATELY NEXT TO THE LAST OBJECT (avoid counting an object twice)
                        if( (arrayofDetectedObjects[index_object-1].degree - arrayofDetectedObjects[index_object-1].degree) > DEGREES_BETWEEN_EACH_OBJECT ) {
                            index_object++; //one object has been found, move to next struct array "arrayofDetectedObjects[30] element to populate next set of data when next object is found
                            LATBbits.LATB11 ^= 0; //turn on LED
                        }
                    }
                }
            }
        }
    }
    
    if((timeFlag == true)){// && (index_object > 0)) {
        //blinkLED(1000); //debug led to show object detected
        hundredMillis++;
        LATBbits.LATB9 ^= 0; //toggle on LED
        LATBbits.LATB10 ^= 0; //toggle on LED
        LATBbits.LATB11 ^= 0; //toggle on LED
        LATBbits.LATB12 ^= 0; //toggle on LED
        LATBbits.LATB13 ^= 0; //toggle on LED
        timeFlag = false;
    }
    
    return 1;
}
