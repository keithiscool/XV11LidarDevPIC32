/* File:   defs.h
 * Author: Keith
 * Created on April 30, 2016, 10:58 AM
 */

#ifndef DEFS_H
#define	DEFS_H

#ifdef	__cplusplus
extern "C"
{
#endif




//DMA parameters for tuning the size of the output DMA (UART 6: U6TX)
//#define TOP_LEVEL_QUEUE_DEPTH 5
//#define SECOND_LEVEL_QUEUE_DEPTH 193
//#define SIZE_OF_DMA_ARRAY 200
#define TOP_LEVEL_QUEUE_DEPTH 5
#define SECOND_LEVEL_QUEUE_DEPTH 1500
#define SIZE_OF_DMA_ARRAY 2000
    

//Ring Buffer parameters for input UART (UART 4: U4RX)
#define RING_BUF_SIZE 500






#ifdef	__cplusplus
}
#endif

#endif	/* DEFS_H */

