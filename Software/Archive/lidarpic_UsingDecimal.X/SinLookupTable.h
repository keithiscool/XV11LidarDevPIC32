 /* File:   SinLookupTable.h
 * Author: Keith
 * Created on December 6, 2015, 8:06 PM*/

#ifndef SINLOOKUPTABLE_H
#define	SINLOOKUPTABLE_H



short SinLookupTableArray[91] = { //Shorts are 16-bits large for PIC32.
    0,
    17,
    35,
    52,
    70,
    87,
    105,
    122,
    139,
    156,
    174,
    191,
    208,
    225,
    242,
    259,
    276,
    292,
    309,
    326,
    342,
    358,
    375,
    391,
    407,
    423,
    438,
    454,
    469,
    485,
    500,
    515,
    530,
    545,
    559,
    574,
    588,
    602,
    616,
    629,
    643,
    656,
    669,
    682,
    695,
    707,
    719,
    731,
    743,
    755,
    766,
    777,
    788,
    799,
    809,
    819,
    829,
    839,
    848,
    857,
    866,
    875,
    883,
    891,
    899,
    906,
    914,
    921,
    927,
    934,
    940,
    946,
    951,
    956,
    961,
    966,
    970,
    974,
    978,
    982,
    985,
    988,
    990,
    993,
    995,
    996,
    998,
    999,
    999,
    1000,
    1000
};

short GetMySinLookup16bit(unsigned short Degrees) {
    Degrees = Degrees%360; // limit degrees between (0<->360)

    if (Degrees <= 90) return SinLookupTableArray[Degrees]; // 0<Angle<90
    if (Degrees <= 180) return SinLookupTableArray[(180-Degrees)]; // 90<Angle<180
    if (Degrees <= 270) return (-1*(SinLookupTableArray[(Degrees-180)])); // 180<Angle<270
    if (Degrees < 360) return (-1*(SinLookupTableArray[(360-Degrees)])); // 180<Angle<360
}


short GetMyCosLookup16bit(unsigned short Degrees){
    Degrees = Degrees%360; // limit degrees between (0<->360)

    if (Degrees <= 90) return SinLookupTableArray[(90-Degrees)]; // 0<Angle<90
    if (Degrees <= 180) return (-1*(SinLookupTableArray[(Degrees-90)])); // 90<Angle<180
    if (Degrees <= 270) return (-1*(SinLookupTableArray[(270-Degrees)])); // 90<Angle<270
    if (Degrees < 360) return (SinLookupTableArray[(Degrees-270)]); // 270<Angle<360
}


#endif	/* SINLOOKUPTABLE_H */
