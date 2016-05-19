#include <xc.h>
#include <stdbool.h>
#include "lidar_decoder.h"
#include <math.h>
#include "SinLookupTable.h"
#include "defs.h"
//////////////////////////////////////////////////////////////////////////////
// To note for dsPIC32:
//8bits=char   16bits=short    32bits=integer
//////////////////////////////////////////////////////////////////////////////
// Operation of XV11 Lidar (packet description):
//start is always 0xFA
//index is (each 22 byte packet) in the 90 packets, going from 0xA0 (packet 0, readings 0 to 3) to 0xF9
//(packet 89, readings 356 to 359).
//4 distance measurements per 22 byte packet * 90 packets per revolution = 360 distance measurements

//speed is a two-byte information, little-endian. It represents the speed, in 64th of RPM
//(aka value in RPM represented in fixed point, with 6 bits used for the decimal part).

//A full revolution will yield 90 packets, containing 4 consecutive readings each.
//The length of a packet is 22 bytes.
//This amounts to a total of 360 readings (1 per degree) on 90*22 = 1980 bytes.

//"data_buffer[22]" IS ORGANIZED AS FOLLOWS (INPUT DATA FROM LIDAR TO PIC32)
//ARRAY IS ZERO BASED!!!! (FIRST ELEMENT IS data_buffer[0])
//[Data 0] to [Data 3] are the 4 readings. Each distance ("Data #") is 4 bytes long, and organized as follows :
//<start> <index> <speed_L> <speed_H> [Data 0] [Data 1] [Data 2] [Data 3] <checksum_L> <checksum_H>
//  B0      B1       B2         B3      B4-7     B8-11    B12-15   B16-19     B20          B21


void beginLIDARdecoder(unsigned short * _output_data, struct ringBuff* _input_data) {
    data = _input_data;
    output_data = _output_data;
}


unsigned short CRC_calculator(unsigned char * _this) {
    unsigned short dataList[10];
    unsigned short IndexTwoBytesGrouped = 0;
    unsigned char IndexEachByte = 0;
    unsigned int chk32 = 0;

    for (IndexTwoBytesGrouped = 0; IndexTwoBytesGrouped < 10; IndexTwoBytesGrouped++) {
        // group the data by word(16 bits), little-endian (LSB stays at LSB and MSB shifts left by 8 when bytes are "concatinated")
        dataList[IndexTwoBytesGrouped] = _this[2 * IndexEachByte] + (((unsigned short)_this[2 * IndexEachByte + 1]) << 8);
        IndexEachByte++;
    }

    for (IndexTwoBytesGrouped = 0; IndexTwoBytesGrouped < 10; IndexTwoBytesGrouped++) {
        chk32 = (chk32 << 1) + dataList[IndexTwoBytesGrouped];
    }

    // return a value wrapped around on 15bits, and truncated to still fit into 15 bits
    unsigned short checksum = (chk32 & 0x7FFF) + (chk32 >> 15);
    return (checksum & 0x7FFF);
}


//parses data from the lidar and turns the stream into usable data (fetches measurements, flags, warnings and sotres them into arrays)
bool LIDARdecode(short getDegrees[4]) {
    unsigned short i;

    if (transmission_in_progress == false) {
        if (ring_buff_size(data) > 22) {
            while (ring_buff_get(data) != 0xFA) { // Wait until start byte is read from ring buffer5 (lidar data)
                if (ring_buff_size(data) <= 0){ //safety break to leave the "while(!0xFA)" loop
                    return false;
                }
            }
            transmission_in_progress = true;
            data_buffer[buff_index] = 0xFA;
            buff_index++;
        }
    }
    if (transmission_in_progress == true) {
        while (ring_buff_size(data) && (buff_index <= 22)) {
            data_buffer[buff_index] = ring_buff_get(data);
            buff_index++;
        }
        if (buff_index >= 22) {
            unsigned short calculated_CRC = CRC_calculator(data_buffer);
            unsigned short received_CRC = assemble(data_buffer[20], data_buffer[21]);

            //Now check if the lidar data can pass the CRC error-checking
            if (received_CRC == calculated_CRC) { // Used to see if incoming data passed the CRC

                //DegreeIndex is the index byte in the 90 packets, going from 0xA0 (packet 0, readings 0 to 3) to 0xF9 (packet 89, readings 356 to 359)
                //offset degrees in order to allow the lidar to be used in the arena beacon mount (hook part opposite of motor is degree 0)
                DegreeIndex = (data_buffer[1] - 0xA0) * 4; //pull degree base (1 degree index per packet) out of parsed lidar data

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //***SEE INCLUDED PICTURE OF REMAPPING LIDAR DEGREES***//
            //**ReOrientLidarForBeaconAgainstCollectionWall.jpg**//

////Something is not right with DegreeIndex...
//                if(timeFlagOneHundMilSec){
//                    printf("O: %d\r\n",DegreeIndex);
//                    timeFlagOneHundMilSec = false;
//                }

                //Shift remap to Polar Coordinate system
                //Keep degrees in front of collection bin (originally 270 to zero to 90), but converted to 0 to 90 to 180 to match conventional polar degrees
                DegreeIndex = DegreeIndex + 90;
/****************************************************************/
//For Reed: THIS IS WEIRD...
//I DO NOT KNOW WHY THE 1ST DEGREE IN EACH GROUP OF 4 DISTANCES CAUSES THE PROCESSOR TO HALT...
//Attempt to ignore the information for the distances behind the collection bin
/****************************************************************/
//////Throw out data behind lidar (do not populate out of bounds data)
//                if(DegreeIndex > 177) {
////                    printf("FALSE %d\r\n",DegreeIndex);
//                    return false;
//                }

                //Check Rollover Condition where quadrant 1 (270 deg) moves over original 360 degrees
                if(DegreeIndex > 359) {
                    //Shift the Data from notch being 270 degrees to notch being 0 degrees. (shift 0 degrees clockwise by 90 degrees)
                    DegreeIndex = DegreeIndex - 360; //offset degrees to be used in object recognition (if "offsetDegrees" is negative, this, shifts the degrees)
//                    printf("ROLL: %d\r\n",DegreeIndex);
                }


/****************************************************************/
//For Reed: THIS IS ALSO KIND OF WEIRD...
//I DO NOT KNOW WHY THE 1ST DEGREE IN EACH GROUP OF 4 DISTANCES CAUSES THE PROCESSOR TO HALT AROUND 177 DEGREES...
//I am Attempting to ignore the information for the distances behind the collection bin (only use [0,180] inclusive degrees
/****************************************************************/
//////Throw out data behind lidar (do not populate out of bounds data)
//                if(DegreeIndex > 177) {
////                    printf("FALSE %d\r\n",DegreeIndex);
//                    return false;
//                }
                
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                //Packet Index
                output_data[0] = (data_buffer[1] - 0xA0); //0xA0 is offset that lidar uses for index (not sure why...)

                //Speed - in 6 floating point precision format in 64th of an rpm
                returned_speed = ((data_buffer[3] << 8) | data_buffer[2]) / 64;

                //Keep Track of the Status Bits for Each Distance Measurement
                //Quality Flags are bytes 2&3 of each 4 bytes from "Data #" in each 22 byte packet
                QualityFlag[0] = (data_buffer[6] + (data_buffer[7] << 8));
                QualityFlag[1] = (data_buffer[10] + (data_buffer[11] << 8));
                QualityFlag[2] = (data_buffer[14] + (data_buffer[15] << 8));
                QualityFlag[3] = (data_buffer[18] + (data_buffer[19] << 8));

                InvalidFlag[0] = (data_buffer[5] & 0x80) >> 7; //check bit 7 of distance byte 1 for invalid flag
                InvalidFlag[1] = (data_buffer[9] & 0x80) >> 7;
                InvalidFlag[2] = (data_buffer[13] & 0x80) >> 7;
                InvalidFlag[3] = (data_buffer[17] & 0x80) >> 7;

                WarningFlag[0] = (data_buffer[5] & 0x40) >> 6; //check bit 6 of distance byte 1 for warning flag (bad signal strength / low reflectance)
                WarningFlag[1] = (data_buffer[9] & 0x40) >> 6;
                WarningFlag[2] = (data_buffer[13] & 0x40) >> 6;
                WarningFlag[3] = (data_buffer[17] & 0x40) >> 6;

                //Now pull the 4 distance measurements out of the current 22 byte packet
                //Increments the Distance Index (Degree Index after it updates tje current distance value)
                for (i = 0; i < 4; i++) {
                    if (!InvalidFlag[i]) { // the data is valid within the present 22byte packet

                        if(PreviousDistanceArr[DegreeIndex+i] != DistanceArr[DegreeIndex+i]) {
                            //VERIFY THERE WAS A CHANGE IN CASE THE OBJECT BEING TRACKED IS MOVING
                        }

                        //Pull 4 polar distances (in millimeters) from each 22 byte packet
                        DistanceArr[DegreeIndex+i] = data_buffer[4+(i*4)] + ((unsigned char)(data_buffer[5+(i*4)] & 0x3F) << 8);
                        //Copy Quality Info to Quality Array to be Analyzed
                        QualityArr[DegreeIndex+i] = QualityFlag[i];
                        //Compute 4 Cartesian Coordinates for Output
                        if((DistanceArr[DegreeIndex+i] > 0) && (DistanceArr[DegreeIndex+i] < maxDistanceAllowed)) { // check if polar distance is useful data
//                            YCoordMilliMeters[DegreeIndex+i] = ((short)(((int)DistanceArr[DegreeIndex+i]*(int)GetMySinLookup16bit(DegreeIndex+i))>>16)); //max 14 bit value for distance
//                            XCoordMilliMeters[DegreeIndex+i] = ((short)(((int)DistanceArr[DegreeIndex+i]*(int)GetMyCosLookup16bit(DegreeIndex+i))>>16)); //max 14 bit value for distance
                            YCoordMilliMeters[DegreeIndex+i] = ((short)(((unsigned int)DistanceArr[DegreeIndex+i]*(unsigned int)GetMySinLookup16bit(DegreeIndex+i))>>16)); //max 14 bit value for distance
                            XCoordMilliMeters[DegreeIndex+i] = ((short)(((unsigned int)DistanceArr[DegreeIndex+i]*(unsigned int)GetMyCosLookup16bit(DegreeIndex+i))>>16)); //max 14 bit value for distance
                        }
                        PreviousDistanceArr[DegreeIndex+i] = DistanceArr[DegreeIndex+i]; //"old" copy of data is kept to compare with the next iteration of "newer" data

                        //Prevent the angle of the
                        if(AnglesCoveredTotal < 181)
                            AnglesCoveredTotal++;
                        else
                            AnglesCoveredTotal = 180;

                        if(AnglesCoveredTotal > mostAnglesRead) {
                            LidarCalcPerm = true;
                        }

                        //record which degree measurements were determined from the parsed input (used in object detection)
                        getDegrees[i] = DegreeIndex + i;

                    } else { //The data is not valid and has invalid flags within the present 22byte packet
                        //data is invalid and should not be used
                        DistanceArr[DegreeIndex+i] = 0;
                        YCoordMilliMeters[DegreeIndex+i] = 0;
                        XCoordMilliMeters[DegreeIndex+i] = 0;
                        PreviousDistanceArr[DegreeIndex+i] = 0;
                        BadReadings++; //mark each distance measurement as a "bad reading" if the invalid flags are set in their data packets
                        getDegrees[i] = 0;
                    }
                }
                //successful CRC parsed packet
                //permit the acquisition of the next packet of 4 distances
                transmission_in_progress = false;
                buff_index = 0;
                return true;
            }
            else {
                transmission_in_progress = false;
                buff_index = 0;
                return false;
            }
        }
        else {
            return false;
        }
    }
    return false;
}



//used to print out the parsed Polar data from xv11 lidar to a serial monitor
//this can also print quality data
bool debugLidarPolarData(void) {
    unsigned short i = 0;
    unsigned short blah[4]; //do not need degree measurements for each parsing of data

    //read all 360 degrees, disable uart 4 input from lidar, then print data out
    if(LIDARdecode(blah) == true) { //88 is rotation offset for lidar (rotate clockwise if positive)

//        if(AnglesCoveredTotal > 181) {
            //Show first index as zero
            //printf("Degree:\r\n%4d: ",0);

                U4STAbits.URXEN = 0; // disable uart receive (Do not allow Receive Data from Lidar UART5)
                U4MODEbits.ON = 0; // disable whole uart5 module

//            printf("\r\n");//new line (carriage return)
//            printf("\x1b[H"); //clear screen // ANSI Terminal code: (ESC[H == home) & (ESC[2J == clear screen) // THIS IS FORMATTING FOR ANSI (DOES NOT WORK WITH DMA!)

                printf("\r\n");//new line (carriage return)
                printf("DisplayPolarData\r\n");
                for(i=0;i<181;i++) {
                    if(U6STAbits.UTXBF == 0) { //check to see if the UART buffer is not full - if it is not full, send debug data out UART
                        if ((i % PRINT_NUM_PER_LINE) == 0) { //print 24 distances per line
                            printf("\r\n%4u: ",i); //print 16 distances per line '\r\n' causes prompt to carraige return
                        }
                        printf("%4u ",DistanceArr[i]); //Print out the data to 4, 16-bit unsigned integer digits
                    } else {
                        i--; //keep index at same value if the UART1 TX debug buffer is full
                    }
                }

////                        printf("\x1b[0J"); // ANSI Terminal code: (ESC[2J == clear screen below cursor)
//                printf("\r\n");//new line (carriage return)
//                printf("DisplayQualityData\r\n");
//                for(i=0;i<181;i++) {
//                    if(U6STAbits.UTXBF == 0) { //check to see if the UART buffer is not full - if it is not full, send debug data out UART
//                        if ((i % PRINT_NUM_PER_LINE) == 0) { //print 24 quality elements per line
//                            printf("\r\n%4u: ",i); //print 16 distances per line '\r\n' causes prompt to carraige return
//                        }
//                    printf("%4u ",QualityArr[i]); //Print out the data to 4, 16-bit unsigned integer digits
//                    } else {
//                        i--; //keep index at same value if the UART1 TX debug buffer is full
//                    }
//                }


                if(U6STAbits.UTXBF == 0) { //check to see if the UART buffer is not full - if it is not full, send debug data out UART
                    printf("RPM: %f\r\n", returned_speed);
                    printf("===========\r\n");
                }
//            }
        U4STAbits.URXEN = 1; // enable uart transmit (Allow Receive Data from Lidar)
        U4MODEbits.ON = 1; // enable whole uart module
    }
    return true;
}


//used to print out the parsed X,Y (cartesian) data from xv11 lidar to a serial monitor
//this can also print quality data
bool debugLidarCartesianData(void) {
    unsigned short i = 0;
    unsigned short blah[4]; //do not need degree measurements for each parsing of data

    //read all 360 degrees, disable uart 4 input from lidar, then print data out
    if(LIDARdecode(blah) == true) { //90 is rotation offset for lidar (rotate clockwise if positive)

//        if(AnglesCoveredTotal > 181) {
            //Show first index as zero
            //printf("Degree:\r\n%4d: ",0);

            U4STAbits.URXEN = 0; // disable uart receive (Do not allow Receive Data from Lidar UART5)
            U4MODEbits.ON = 0; // disable whole uart5 module

//            printf("\r\n");//new line (carriage return)
//            printf("\x1b[H"); //clear screen // ANSI Terminal code: (ESC[H == home) & (ESC[2J == clear screen) // THIS IS FORMATTING FOR ANSI (DOES NOT WORK WITH DMA!)

            printf("\r\n");//new line (carriage return)
            printf("DisplayCartesianData\r\n");
            printf("XCoordMeters , YCoordMeters:\r\n");
            for(i=0;i<181;i++) {
//                        while(U1STAbits.TRMT == 1) { //check to see if the UART buffer is empty - if it is, send debug data out UART1
                if(U6STAbits.UTXBF == 0) { //check to see if the UART buffer is not full - if it is not full, send debug data out UART1
                    if ((i % PRINT_NUM_PER_LINE) == 0)  //print 4 x,y distances per line
                        printf("\r\n%4d: ",i); //print 4 distances per line '\r\n' causes prompt to carraige return

                    printf(" %d,%d//", XCoordMilliMeters[i], YCoordMilliMeters[i]);
                }
                else
                    i--; //keep index at same value if the UART1 TX debug buffer is full
            }
//            printf("\r\n--------------------------------\r\n");
//
////                        printf("\x1b[0J"); // ANSI Terminal code: (ESC[2J == clear screen below cursor)
//            printf("\r\n");//new line (carriage return)
//            printf("DisplayQualityData\r\n");
//            for(i=0;i<181;i++) {
//                if(U6STAbits.UTXBF == 0) { //check to see if the UART buffer is not full - if it is not full, send debug data out UART
//                    if ((i % PRINT_NUM_PER_LINE) == 0) { //print 24 quality elements per line
//                        printf("\r\n%4u: ",i); //print 16 distances per line '\r\n' causes prompt to carraige return
//                    }
//                printf("%4u ",QualityArr[i]); //Print out the data to 4, 16-bit unsigned integer digits
//                } else {
//                    i--; //keep index at same value if the UART1 TX debug buffer is full
//                }
//            }


            if(U6STAbits.UTXBF == 0) { //check to see if the UART buffer is not full - if it is not full, send debug data out UART
                printf("RPM: %f\r\n", returned_speed);
                printf("===========\r\n");
            }

//        }
        U4STAbits.URXEN = 1; // enable uart transmit (Allow Receive Data from Lidar)
        U4MODEbits.ON = 1; // enable whole uart module
    }
    return true;
}



unsigned short assemble(unsigned char lower, unsigned char upper) {
    return (unsigned short)(lower + ((upper << 8) & 0xff00));
}
