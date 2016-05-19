#define SinTableSize 90 //16384 == 16 bits of for approximating 0->pi/2
#define var int16_t
#define PI 3.14159265359

#define exp2_16 65536
#define exp2_32 4294967296

typedef struct NONSENSE{
    float SinTable[SinTableSize];
    float SinTableInterpolated[SinTableSize];
    int16_t XCoord;
    int16_t YCoord;
}TrigStuff;

