#include <xc.h>
#include <stdbool.h>
#include "lidar_decoder.h"
#include <math.h>
#include "SinLookupTable.h"
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
//[Data 0] to [Data 3] are the 4 readings. Each one is 4 bytes long, and organized as follows :
//<start> <index> <speed_L> <speed_H> [Data 0] [Data 1] [Data 2] [Data 3] <checksum_L> <checksum_H>


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


bool LIDARdecode(void){
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
                LATEbits.LATE4 ^= 1; // Toggle LEDtest1 AFTER CHECKSUM IS PASSES
                //Packet Index
                output_data[0] = (data_buffer[1] - 0xA0); //0xA0 is offset that lidar uses for index (not sure why...)
                //Degree Index
                DegreeIndex = (data_buffer[1] - 0xA0) * 4;
                //Speed - in 6 floating point precision format in 64th of an rpm
                returned_speed = ((data_buffer[3] << 8) | data_buffer[2]) / 64;

                //Keep Track of the Status Bits for Each Distance Measurement
                InvalidFlag[0] = (data_buffer[5] & 0x80) >> 7;
                InvalidFlag[1] = (data_buffer[9] & 0x80) >> 7;
                InvalidFlag[2] = (data_buffer[13] & 0x80) >> 7;
                InvalidFlag[3] = (data_buffer[17] & 0x80) >> 7;
                WarningFlag[0] = (data_buffer[5] & 0x40) >> 6;
                WarningFlag[1] = (data_buffer[9] & 0x40) >> 6;
                WarningFlag[2] = (data_buffer[13] & 0x40) >> 6;
                WarningFlag[3] = (data_buffer[17] & 0x40) >> 6;

                //Now pull the 4 distance measurements out of the current 22 byte packet
                //Increments the Distance Index (Degree Index after it updates tje current distance value)
                for (i = 0; i < 4; i++) {
                    if (!InvalidFlag[i]) { // the data is valid within the present 22byte packet
                        //Pull 4 polar distances (in millimeters) from each 22 byte packet
                        Distance[DegreeIndex+i] = data_buffer[4+(i*4)] + ((unsigned char)(data_buffer[5+(i*4)] & 0x3F) << 8);
                        //Compute 4 Cartesian Coordinates for Output
                        if((Distance[DegreeIndex+i] > 0) && (Distance[DegreeIndex+i] < 10000)) { // check if polar distance is useful data
                            YCoordMeters[DegreeIndex+i] = ((short)(((int)Distance[DegreeIndex+i]*(int)GetMySinLookup16bit(DegreeIndex+i))>>16)); //max 14 bit value for distance
                            XCoordMeters[DegreeIndex+i] = ((short)(((int)Distance[DegreeIndex+i]*(int)GetMyCosLookup16bit(DegreeIndex+i))>>16)); //max 14 bit value for distance
                        }
                        SuccessfulMeasurements[DegreeIndex] = 1;
                        AnglesCoveredTotal++;
                    } else { //The data is not valid and has invalid flags within the present 22byte packet
                        Distance[DegreeIndex+i] = 0;
                        XCoordMeters[DegreeIndex+i] = 0;
                        YCoordMeters[DegreeIndex+i] = 0;
                        SuccessfulMeasurements[DegreeIndex+i] = 0;
                    }
                }
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

//Check whether the change in Distance between each degree is large -> indicates object or wall
short objectDetection(unsigned short i, unsigned short *DistanceArr[360], unsigned short *DistanceDifferencesArr[360], unsigned short *DetectedObjects[360]) {

    short startOfDetectedObject = 0;
    short endOfDetectedObject = 0;
    short ObjectDetectionThreshold = 500;

    if(i>359) { // check rollover condition where 360 degrees is compared w/ 0degrees
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


unsigned short assemble(unsigned char lower, unsigned char upper) {
    return (unsigned short)(lower + ((upper << 8) & 0xff00));
}
