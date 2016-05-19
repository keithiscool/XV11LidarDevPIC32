#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "main.h"
//Help With Making Lookup Table:
//https://jfdube.wordpress.com/2011/12/06/trigonometric-look-up-tables-revisited/
/////////////////////////////////////////////////////////////////////////////
    //These are the functions that I call from my other program:
//    http://stackoverflow.com/questions/187713/converting-floating-point-to-fixed-point

SinTable[91] = {
    0,
    1144,
    2287,
    3430,
    4572,
    5712,
    6850,
    7987,
    9121,
    10252,
    11380,
    12505,
    13626,
    14742,
    15855,
    16962,
    18064,
    19161,
    20252,
    21336,
    22415,
    23486,
    24550,
    25607,
    26656,
    27697,
    28729,
    29753,
    30767,
    31772,
    32768,
    33754,
    34729,
    35693,
    36647,
    37590,
    38521,
    39441,
    40348,
    41243,
    42126,
    42995,
    43852,
    44695,
    45525,
    46341,
    47143,
    47930,
    48703,
    49461,
    50203,
    50931,
    51643,
    52339,
    53020,
    53684,
    54332,
    54963,
    55578,
    56175,
    56756,
    57319,
    57865,
    58393,
    58903,
    59396,
    59870,
    60326,
    60764,
    61183,
    61584,
    61966,
    62328,
    62672,
    62997,
    63303,
    63589,
    63856,
    64104,
    64332,
    64540,
    64729,
    64898,
    65048,
    65177,
    65287,
    65376,
    65446,
    65496,
    65526,
    65536
};



int16_t GetMySinLookup16bit(int16_t Degrees) {
    if (Degrees < 90) return  SinTable[Degrees];
    if (Degrees < 180) return  SinTable[90-Degrees];
    if (Degrees < 270) return -(SinTable[Degrees]);
    if (Degrees < 360) return -(SinTable[90-Degrees]);
}

//int16_t GetMyCosLookup16bit(int16_t Degrees) {
//    if (Degrees < 90) return  SinTable[Degrees];
//    if (Degrees < 180) return  SinTable[90-Degrees];
//    if (Degrees < 270) return -(SinTable[Degrees]);
//    if (Degrees < 360) return -(SinTable[90-Degrees]);
//}

int16_t GetMyCosLookup16bit(int16_t Degrees){
    return GetMySinLookup16bit(Degrees + 90);
}

int16_t lookup_tan(int16_t Degrees){
    return (GetMySinLookup16bit(Degrees) / GetMyCosLookup16bit(Degrees));
}

////////////////////////////////////////////////////////////////////////////////

int16_t float2fixed(float a){
    return (int16_t)(round(a*exp2_16)); //added 'round' for better accuracy
}

int32_t float32Tofixed(float a){
    return (int32_t)(round(a*exp2_16)); //added 'round' for better accuracy
}


int32_t floatlonglongtofixed(float a){
    return (long long)(round(a*exp2_32)); //added 'round' for better accuracy
}

float fix2float(int16_t a){
    return (float)(a/exp2_16);
}

////////////////////////////////////////////////////////////////////////////////

int main(void){
    int Degrees = 85;
    int Original = Degrees;
    int output;

    int mod= Degrees % 360; // limit degrees between 0<->360
//    if(Degrees<0) Degrees=(360+Degrees); // Only deal with positive degree values
    //if(mod<0)
        output=(360+mod); // Only deal with positive degree values

    printf("OriginalDegrees: %d , Mod: %d, Degrees Corrected: %d , Output: %d\n",Original,mod,Degrees,output);



    int16_t SinTableTestVal2 = 48703; //48703 == 48 degrees in 16 bit (out of 65535)
//    int16_t InputDistance = 16384; //Maximum input Distance as 14bit unsigned integer from XV11

    int16_t InputDistance = 512; //raw distance in millimeters
    int16_t InputDegree = 48;

    //testing sine with lookup table
    int16_t FinalMaxVal2 = (int16_t)( ((int32_t)SinTableTestVal2*(int32_t)InputDistance) >> 16 ); //max 14 bit value for distance

    printf("output: %d\n",output);
    printf("SineTable Val      /       FinalOutput_Sine\n");
//    printf("%d                 /       %d\n",SinTableTestVal2,FinalMaxVal2);//THIS ONE WORKS!!!
    printf("%d                 /       %d\n",SinTable[48],FinalMaxVal2);//THIS ONE WORKS!!!


    //testing cosine with sine lookup table
    int16_t FinalMaxVal3 = (int16_t)( ( ( (int32_t)abs(SinTable[InputDegree-90]) ) * ( (int32_t)InputDistance) ) >> 16 ); //max 14 bit value for distance
    int16_t FinalMaxVal4 = (int16_t)( ( ( (int32_t)abs(SinTable[InputDegree-90]) ) * ( (int32_t)InputDistance) ) >> 16 ); //max 14 bit value for distance
    printf("   Cosine Val      /       FinalOutput_Cosine\n");
    printf("%d                 /       %d\n",SinTable[output],FinalMaxVal3);
    printf("%d                 /       %d\n",SinTable[abs(InputDegree-90)],FinalMaxVal4);

    system("pause");
    return 1;
}
