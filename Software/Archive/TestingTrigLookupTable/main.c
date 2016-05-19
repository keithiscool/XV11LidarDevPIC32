#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "main.h"
//Help With Making Lookup Table:
//https://jfdube.wordpress.com/2011/12/06/trigonometric-look-up-tables-revisited/
/////////////////////////////////////////////////////////////////////////////
    //These are the functions that I call from my other program:
//    http://stackoverflow.com/questions/187713/converting-floating-point-to-fixed-point

int16_t GetMySinLookup16bit(TrigStuff *Trig, int16_t Degrees) {
    if (Degrees < 90) return  Trig->SinTable[Degrees];
    if (Degrees < 180) return  Trig->SinTable[90-Degrees];
    if (Degrees < 270) return -(Trig->SinTable[Degrees]);

    return -(Trig->SinTable[90-Degrees]);
}

int16_t GetMyCosLookup16bit(TrigStuff *Trig, int16_t Degrees){
    return GetMySinLookup16bit(Trig, (Degrees + 90));
}

int16_t lookup_tan(TrigStuff *Trig, int16_t Degrees){
    return (GetMySinLookup16bit(Trig,Degrees) / GetMyCosLookup16bit(Trig,Degrees));
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
    if(mod<0)
        output=(360+mod); // Only deal with positive degree values

    printf("OriginalDegrees: %d , Mod: %d, Degrees Corrected: %d , Output: %d\n",Original,mod,Degrees,output);



    int16_t SinTableTestVal2 = 48703; //48703 == 48 degrees in 16 bit (out of 65535)
//    int16_t InputDistance = 16384; //Maximum input Distance as 14bit unsigned integer from XV11
    int16_t InputDistance = 512; //raw distance in millimeters
//    printf("LookupTableArray: Element 21 is: %d\n",LookupTableArray[21]);
    int16_t FinalMaxVal2 = (uint16_t)(((int32_t)SinTableTestVal2*(int32_t)InputDistance)>>16); //max 14 bit value for distance

    printf("SineTable Val      /       FinalOutput \n");
    printf("%d                 /       %d\n",SinTableTestVal2,FinalMaxVal2);//THIS ONE WORKS!!!

    system("pause");
    return 1;
}
