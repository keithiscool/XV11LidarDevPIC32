/* 
 * File:   main.h
 * Author: Dana
 *
 * Created on May 12, 2016, 9:52 PM
 */

#ifndef MAIN_H
#define	MAIN_H

#ifdef	__cplusplus
extern "C" {
#endif

extern unsigned char wiiInitStep;
extern bool pendingTrans1;
extern bool pendingTrans2;
    
extern bool timeFlag;
extern bool ms200done;
extern unsigned char initBitsSent;
extern bool awaitingData;
extern bool dataAvailable;
extern unsigned short xBlob[2][2];

extern void initializeWii(void);
extern void SortInput(unsigned char cameraValue, unsigned char loc);
extern unsigned short PickBeacon(unsigned char cameraValue);
extern unsigned char getBlobsSeen(unsigned char cameraValue);
extern void read(unsigned char cameraValue);
//extern void determineAngle(bool haveBlob, short blobX, unsigned char servo) ;
extern void Sort(unsigned char cameraValue);
extern int isAbout(int compareThis, int toThis, int range);
extern void seekBeacon(int cameraNumber, int result);
extern short getDifferenceX(unsigned char cameraValue);

#ifdef	__cplusplus
}
#endif

#endif	/* MAIN_H */

