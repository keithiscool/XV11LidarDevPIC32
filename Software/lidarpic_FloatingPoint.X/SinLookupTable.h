 /* File:   SinLookupTable.h
 * Author: Keith
 * Created on December 6, 2015, 8:06 PM*/

#ifndef SINLOOKUPTABLE_H
#define	SINLOOKUPTABLE_H


//This code is tested in Software/Archive/TestingTrigLookupTable
/*
 * This lookup table is used for fixed point sine and cosine calculation to
 * get x,y position in arena
*/
//////extern unsigned short SinLookupTableArray[91];


//Used unit circle to line up sine and cosine functions using just the first 90 degrees of the generated sine lookup table from excel:
//Lookup Table in Excel: \XV11LidarDevPIC32\Software\Archive\TestingTrigLookupTable

//Unit Circle (simple)
//https://upload.wikimedia.org/wikipedia/commons/thumb/4/4c/Unit_circle_angles_color.svg/2000px-Unit_circle_angles_color.svg.png


//Function Prototypes for trig fixed point using sine lookup table
extern unsigned short GetMySinLookup16bit(short Degrees, bool *negativeNum);
extern unsigned short GetMyCosLookup16bit(short Degrees, bool *negativeNum);




#endif	/* SINLOOKUPTABLE_H */
