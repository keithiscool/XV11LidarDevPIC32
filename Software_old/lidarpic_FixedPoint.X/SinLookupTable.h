 /* File:   SinLookupTable.h
 * Author: Keith
 * Created on December 6, 2015, 8:06 PM*/

#ifndef SINLOOKUPTABLE_H
#define	SINLOOKUPTABLE_H



int SinLookupTableArray[91] = { //Shorts are 16-bits large for PIC32.
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

short GetMySinLookup16bit(short Degrees) {
//    Degrees = Degrees%360; // limit degrees between (-360<->360)

//    if(Degrees<0) 
//        Degrees=(360+Degrees); // Only deal with positive degree values

    if (Degrees <= 90) return SinLookupTableArray[Degrees]; // 0<Angle<90
    if (Degrees <= 180) return SinLookupTableArray[(180-Degrees)]; // 90<Angle<180
    if (Degrees <= 270) return (-1*(SinLookupTableArray[(Degrees-180)])); // 180<Angle<270
    if (Degrees < 360) return (-1*(SinLookupTableArray[(360-Degrees)])); // 270<Angle<360
}


short GetMyCosLookup16bit(short Degrees){
//    Degrees = Degrees%360; // limit degrees between (-360<->360)

//    if(Degrees<0)
//        Degrees=(360+Degrees); // Only deal with positive degree values

    if (Degrees <= 90) return SinLookupTableArray[(90-Degrees)]; // 0<Angle<90
    if (Degrees <= 180) return (-1*(SinLookupTableArray[(Degrees-90)])); // 90<Angle<180
    if (Degrees <= 270) return (-1*(SinLookupTableArray[(270-Degrees)])); // 180<Angle<270
    if (Degrees < 360) return (SinLookupTableArray[(Degrees-270)]); // 270<Angle<360
}


#endif	/* SINLOOKUPTABLE_H */
