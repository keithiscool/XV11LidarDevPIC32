/* File:   ObjectDetection.c
 * Author: Keith
 * Created on February 13, 2016, 1:46 PM
 */

//ALSO LOOK INTO "LINKED_LISTS" Referencing: http://www.cprogramming.com/tutorial/c/lesson15.html

#include "ObjectDetection.h"

//// object decloration for send class
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