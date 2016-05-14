/* File:   defs.h
 * Author: Keith
 * Created on April 30, 2016, 10:58 AM
 */

#ifndef DEFS_H
#define	DEFS_H



//DMA parameters for tuning the size of the output DMA (UART 6: U6TX)
//ORIGINAL VALUES FOR RouterPic:
    //#define TOP_LEVEL_QUEUE_DEPTH 5
    //#define SECOND_LEVEL_QUEUE_DEPTH 193
    //#define SIZE_OF_DMA_ARRAY 200
//NEW VALUES FOR USING LIDAR BOARD:
    //#define TOP_LEVEL_QUEUE_DEPTH 5
    //#define SECOND_LEVEL_QUEUE_DEPTH 3950
    //#define SIZE_OF_DMA_ARRAY 4000
    //#define CELL_SIZE_MAXIMUM 3000

#define TOP_LEVEL_QUEUE_DEPTH 5
#define SECOND_LEVEL_QUEUE_DEPTH 7950
#define SIZE_OF_DMA_ARRAY 8000
#define CELL_SIZE_MAXIMUM 8000

//#define TOP_LEVEL_QUEUE_DEPTH 5
//#define SECOND_LEVEL_QUEUE_DEPTH 400
//#define SIZE_OF_DMA_ARRAY 450


//distance between adjacent lidar degree measurements (if the measurement is large, an object is detected)
#define ObjectDetectionThreshold 400
//50 possible object struct array elements in "arrayofDetectedObjects[OBJECT_ARRAY_STRUCT_SIZE]" can be detected of type "ObjectNode"
#define OBJECT_ARRAY_STRUCT_SIZE 50
#define DEGREES_BETWEEN_EACH_OBJECT 10

//Ring Buffer parameters for input UART (UART 4: U4RX)
#define RING_BUF_SIZE 500




#endif	/* DEFS_H */

