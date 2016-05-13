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
    
    arrayofDetectedObjects[index_object] = emptyObjectStruct; //initialize all struct array elements to zero
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
    

    if(LIDARdecode(offset, DegreeCPY) == true) { //Acquire 4 distances at a time and constantly pull in data (do not print out data)

        for(i=0;i<4;i++) {

    //        //check previous degree magnitude with the present magnitude
    //        if(myDegrees[0] > 355) { //check rollover condition where 360 degrees is compared w/ 0 degrees
    //            DistanceDifferencesArr[0] = abs((DistanceArr[0] - DistanceArr[359])); //use abs() function to get unsigned magnitude of polar distance
    //        } else{
            DistanceDifferencesArr[myDegrees[i]] = abs(( (DistanceArr[myDegrees[i]]) - (DistanceArr[myDegrees[i-1]])));
    //        }

            //check both of the adjacent measurements are valid (data is not zero)
            if( (DistanceDifferencesArr[myDegrees[i]]) && (DistanceDifferencesArr[myDegrees[i-1]]) ) {
                //is start of object not found?
    //            if(arrayofDetectedObjects[index_object].startOfDetectedObject < 1) {
    //                arrayofDetectedObjects[index_object].startOfDetectedObject = myDegrees[i];
    //            }
    //            //is start of object found?
    //            if(arrayofDetectedObjects[index_object].startOfDetectedObject > 0) {
    //                arrayofDetectedObjects[index_object].endOfDetectedObject = myDegrees[i]; //found end of an object (object was detected)
    //            }

                //are the magnitudes different between degree measurements
                if((DistanceDifferencesArr[myDegrees[i]]) > ObjectDetectionThreshold) { // object protruded from surrounding measurements by threshold
                    arrayofDetectedObjects[index_object].startOfDetectedObject = myDegrees[i]; //found start of an object (object's corner was detected)
                    if(index_object < OBJECT_ARRAY_STRUCT_SIZE) {
                        index_object++; //one object has been found, move to next struct array "arrayofDetectedObjects[30] element to populate next set of data when next object is found
                        if(timeFlag == true) {
                            //blinkLED(1000); //debug led to show object detected
                            LATBbits.LATB9 ^= 0; //toggle on LED
                            LATBbits.LATB10 ^= 0; //toggle on LED
                            LATBbits.LATB11 ^= 0; //toggle on LED
                            LATBbits.LATB12 ^= 0; //toggle on LED
                            LATBbits.LATB13 ^= 0; //toggle on LED
                            timeFlag = false;
                        }
                    }else {
                        index_object = 0; //reset because object array is only 30 elements large
                    }

                }
            }
        }
    }
    return 1;
}
