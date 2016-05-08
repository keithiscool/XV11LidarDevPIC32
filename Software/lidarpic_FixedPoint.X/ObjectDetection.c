/* File:   ObjectDetection.c
 * Author: Keith
 * Created on February 13, 2016, 1:46 PM
 */

//ALSO LOOK INTO "LINKED_LISTS" Referencing: http://www.cprogramming.com/tutorial/c/lesson15.html

#include "ObjectDetection.h"

//// object declaration for send class
//Queue_handler DMA_one;
//
////From the "C-Programming Reference", linked lists are similar to a conductor moving between train cars
//short linkedListSetup(struct ) {
//
//    struct ObjectNode *root; //This won't change, or we would lose the list in memory
//    struct ObjectNode *conductor; //This will point to each node as it traverses the list
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




//Check whether the change in Distance between each degree is large -> indicates object or wall
short objectDetection(unsigned short i, unsigned short *DistanceArr, unsigned short *DistanceDifferencesArr, unsigned short *DetectedObjects[360]) {

    short startOfDetectedObject = 0;
    short endOfDetectedObject = 0;
    short ObjectDetectionThreshold = 500;

    if(i>359) { // check rollover condition where 360 degrees is compared w/ 0 degrees
        DistanceDifferencesArr[i] = abs((DistanceArr[360]-DistanceArr[0])); //use abs() function to get unsigned magnitude
        i = 0; //reset index to - degrees after 359 degrees
    } else{
        DistanceDifferencesArr[i] = abs((DistanceArr[i]-DistanceArr[i+1]));
    }
    if(startOfDetectedObject > 0) {
        endOfDetectedObject = i; // found end of an object (object was detected)
    }
    if(DistanceDifferencesArr[i] > ObjectDetectionThreshold) // object protruded from surrounding measurements by 50cm (500mm)
        startOfDetectedObject = i; // found start of an object (object's corner was detected)

}
