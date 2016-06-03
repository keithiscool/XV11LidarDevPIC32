/* File:   defs.h
 * Author: Keith
 * Created on April 30, 2016, 10:58 AM
 */

#ifndef DEFS_H
#define	DEFS_H



//DMA parameters for tuning the size of the output DMA (UART 6: U6TX)
//ORIGINAL DMA VALUES FOR RouterPic (old pic32 dev board):
    //#define TOP_LEVEL_QUEUE_DEPTH 5
    //#define SECOND_LEVEL_QUEUE_DEPTH 193
    //#define SIZE_OF_DMA_ARRAY 200

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//NEW DMA VALUES FOR USING LIDAR BOARD:


////DMA SETTING USED FOR WHEN "debugLidarPolarData()" IS CALLED
//#define TOP_LEVEL_QUEUE_DEPTH 5
//#define SECOND_LEVEL_QUEUE_DEPTH 7950
//#define SIZE_OF_DMA_ARRAY 8000
//#define CELL_SIZE_MAXIMUM 8000

//DMA SETTING USED FOR WHEN "debugLidarPolarData()" IS CALLED
#define TOP_LEVEL_QUEUE_DEPTH 5
#define SECOND_LEVEL_QUEUE_DEPTH 1500
#define SIZE_OF_DMA_ARRAY 2000
#define CELL_SIZE_MAXIMUM 2000


//////DMA SETTING USED FOR WHEN "objectDetection()" IS CALLED
//#define TOP_LEVEL_QUEUE_DEPTH 5
//#define SECOND_LEVEL_QUEUE_DEPTH 400
//#define SIZE_OF_DMA_ARRAY 450


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Ring Buffer parameters for input UART (UART 4: U4RX)
#define RING_BUF_SIZE 500

//tells how many lines should be printed in debug mode
#define PRINT_NUM_PER_LINE 24

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Lidar definitions for object detection with Lidar

//distance between adjacent lidar degree measurements (if the measurement is large, an object is detected)
#define ObjectDetectionThreshold 900
//50 possible object struct array elements in "arrayofDetectedObjects[OBJECT_ARRAY_STRUCT_SIZE]" can be detected of type "ObjectNode"
#define OBJECT_ARRAY_STRUCT_SIZE 50
#define DEGREES_BETWEEN_EACH_OBJECT 5
//offset the x position (in millimeters) of the lidar cartesian data (beacon is not in the center of the arena, but to the right of the collection bin facing the digging area)
//NOTE: THE OFFSET IS TO THE RIGHT OF CENTER (MUST SUBTRACT FROM THE INITIAL CALCULATED CARTESIAN X-VALUE
#define X_POSITION_OFFSET_LIDAR_PLACEMENT 787 //offset in millimeters to right of collection bin center of back wall

//limit on valid distance data magnitude in millimeters
#define maxDistanceAllowed 6000
#define minDistanceAllowed 50
//must read # of distances in order to run object tracking
#define minAnglesRead 60

//define limits for object tracking (I eliminated the edge degrees to prevent the detection of the arena walls and possibly judges near the collection bin arena wall)
#define objectTrackUpper 170
#define objectTrackLower 10

//Take rolling sum of previous values and determine the actual polar magnitudes with fairly high certainty
#define AvgDistNum 3 //quantity of the numbers being averaged for each degree in "PreviousDistancesAvgSum[181]"
#define PresentDistScaler 60 //percent in integer format
#define PrevDistScaler 40 //percent in integer format


#endif	/* DEFS_H */

