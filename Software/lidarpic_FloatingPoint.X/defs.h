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

//define limits for object tracking
//NOTE: AVOID USING THE ENDS BECAUSE THEY OVERLAP WHEN I SHIFTED THE LIDAR ORIENTATION(lidar_decoder.c, line 103, "DegreeIndex = DegreeIndex + 90;")
#define objectTrackUpperDegree 170
#define objectTrackLowerDegree 10

//threshold of object tracking the robot (the robot must be at least this reflective)
#define ROBOT_QUALITY_LOWER_THRESHOLD 35//40 //30
#define ROBOT_QUALITY_UPPER_THRESHOLD 110 //100
//Robot must have moved this far in either the x or y plane in order to run the velocity and bearing calulations
#define objectMovedThreshold 50

#endif	/* DEFS_H */

