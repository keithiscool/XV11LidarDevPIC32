/* 
 * File:   wiiCamera.h
 * Author: Dana
 *
 * Created on May 12, 2016, 8:42 PM
 */


#include <stdbool.h>


#ifndef WIICAMERA_H
#define	WIICAMERA_H

#ifdef	__cplusplus
extern "C" {
#endif

// bit flags for blobs
#define BLOB1 0x01
#define BLOB2 0x02
#define BLOB3 0x04
#define BLOB4 0x08
#define HEIGHT_VARIANCE 100
#define MAX_Y   800
#define MIN_Y   100


struct DataBuffer {
    unsigned char data_buffer[16];
};

extern struct DataBuffer data_buf[2];

extern unsigned char wiiInitStep;
extern bool pendingTrans1;
extern bool pendingTrans2;

extern unsigned short result2, result1;

extern unsigned int camera;
extern unsigned int data_address;

extern unsigned char data[2];
extern unsigned short Ix_buffer[2][4];
extern unsigned short Iy_buffer[2][4];
extern unsigned short average[2];
extern unsigned char blobsSeen[2];
extern unsigned short xBlob[2][2];


extern void initializeWii(void);
extern void SortInput(unsigned char cameraValue, unsigned char loc);
extern unsigned short PickBeacon(unsigned char cameraValue);
extern unsigned char getBlobsSeen(unsigned char cameraValue);
extern void read(unsigned char cameraValue);
extern void Sort(unsigned char cameraValue);
extern short getDifferenceX(unsigned char cameraValue);


#ifdef	__cplusplus
}
#endif

#endif	/* WIICAMERA_H */

