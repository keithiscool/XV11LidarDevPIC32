#include <xc.h>
#include <sys/attribs.h>
#include "Servos.h"

unsigned char angle [2] = {0, 0};
bool clockwise[] = {true, true};
bool beaconExists[2] = {false, false};
bool beaconCentered[2] = {false, false};
unsigned short anglex1 = 2500;
unsigned short anglex2 = 2500; //2.1ms 180degrees, 1.5 90degrees, 955 0degrees
bool setHigh1 = false;
bool setHigh2 = false;

int isAbout(int compareThis, int toThis, int range) {
    return ((compareThis > toThis - range) && (compareThis < toThis + range));
}
void seekBeacon(int cameraNumber, int result) {
    if (result >= INVALID || result <=0) {
        beaconExists[cameraNumber] = false;
        beaconCentered[cameraNumber] = false;
        //SWEEP
        if ((angle[cameraNumber] < (SERVO_MAX - SERVO_STEP_WIDE)) && clockwise[cameraNumber]) {
            angle[cameraNumber] += SERVO_STEP_WIDE;
//            if (cameraNumber == LEFT_CAMERA) setAngle1(angleSet[cameraNumber]);
//            else setAngle2(angleSet[cameraNumber]);
        } else if (clockwise[cameraNumber]) {
            clockwise[cameraNumber] = false;
//            numberSweeps[cameraNumber]++;
        }
        if ((angle[cameraNumber] > (SERVO_MIN + SERVO_STEP_WIDE)) && !clockwise[cameraNumber]) {
            angle[cameraNumber] -= SERVO_STEP_WIDE;
//            if (cameraNumber == LEFT_CAMERA) setAngle1(angleSet[cameraNumber]);
//            else setAngle2(angleSet[cameraNumber]);
        } else if (!clockwise[cameraNumber]) {
            clockwise[cameraNumber] = true;
//            numberSweeps[cameraNumber]++;
        }
    } else {
        int increment;
        
            if (!isAbout(result, MIDDLE_X, BUFFER_X)) {
                beaconExists[cameraNumber] = 1;
                beaconCentered[cameraNumber] = 0;
                if (result> MIDDLE_X) {
                    increment = ((result - MIDDLE_X)) / 20;
                    if (angle[cameraNumber] > (SERVO_MIN + increment)) {
                        angle[cameraNumber] -= increment;
//                        setAngle1(angleSet[cameraNumber]);
                    }
                } else {
                    increment = ((MIDDLE_X - result)) / 20;
                    if (angle[cameraNumber] < (SERVO_MAX - increment)) {
                        angle[cameraNumber] += increment;
//                        setAngle1(angleSet[cameraNumber]);
                    }
                }
            } else {
                beaconExists[cameraNumber] = 1;
//                beaconAngle[cameraNumber] = angleSet[cameraNumber];
                beaconCentered[cameraNumber] = 1;
            }
       
    }
}
//void determineAngle(bool haveBlob, short blobX, unsigned char servo) {
////    if (!haveBlob) {
////        if ((angle[servo] < 180) && clockwise[servo]) {// going to the right until end point of travel reached
////            angle[servo] += SERVO_WIDE_STEP;
////        } else if ((angle[servo] > (SERVO_WIDE_STEP + 1)) && !clockwise[servo]) {// going to the left
////            angle[servo] -= SERVO_WIDE_STEP;
////        }
////        if ((angle[servo] >= 180) || (angle[servo] <= SERVO_WIDE_STEP)) {
////
////            if (angle[servo] >= 180) {
////                angle[servo] = 179;
////                clockwise[servo] = false;
////            } else {
////                angle[servo] = 1;
////                clockwise[servo] = true;
////            }
////
////        }
////        beaconExists[servo] = false;
////    } else if (!beaconExists[servo]) {
////        if (blobX < MIN_CENTER_LIMIT)// blob is to the left of the center
////        {
////            if ((MIDDLE_X - blobX) < BUFFER_X)// go to the right small step
////                angle[servo]++;
////            else
////                angle[servo] += SERVO_MED_STEP;
////            clockwise[servo] = true;
////        } else if (blobX > MAX_CENTER_LIMIT) {// blob is to the right of the center, go left
////            if ((blobX - MIDDLE_X) < BUFFER_X) // go to the right small step
////                angle[servo]--;
////            else
////                angle[servo] -= SERVO_MED_STEP;
////            clockwise[servo] = false;
////        } else // blobX is in the middleish
////        {
////            beaconExists[servo] = true;
////        }
////
////        if ((angle[servo] >= 180) || (angle[servo] < SERVO_MED_STEP)) {
////
////            if (angle[servo] >= 180) {
////                angle[servo] = 179;
////                clockwise[servo] = false;
////            } else {
////                angle[servo] = 1;
////                clockwise[servo] = true;
////            }
////
////        }
////    }
//          if (!haveBlob) {
//                if (angle[servo] < 180 && clockwise[servo]) {// going to the right until end point of travel reached
//                    angle[servo]+=15;
//                } else if (angle[servo] > 15 && !clockwise[servo]) {// going to the left
//                    angle[servo]-=15;
//                }
//                beaconExists[servo]= false;
//            }
//          else if (!beaconExists[servo]) {
//           if (blobX < (MIDDLE_X - BUFFER_X))// blob is to the left of the center
//            {
//                // go to the right
//                angle[servo]++;
//                clockwise[servo] = true;
//            } else if (blobX > (MIDDLE_X + BUFFER_X)) {// blob is to the right of the center, go left
//                angle[servo]--;
//                clockwise[servo] = false;
//            } else // blobX is in the middle
//            {
//                beaconExists[servo] = true;
//            }
//        }
//        if (angle[servo] >= 180 || angle[servo] <= 15) {
//
//            if (angle[servo] >= 180) {
//                angle[servo] = 179;
//                clockwise[servo] = false;
//            } else{
//                angle[servo] = 1;
//                clockwise[servo] = true;
//            }
//        }
//
//}

void setAngle1(short d) {
    if (d < 180 && d > 0) {
        anglex1 = d * DEGREE + 2500;
    } else if (d >= 180) {
        anglex1 = DEGREE180;
    } else
        anglex1 = DEGREE0;
}

void setAngle2(short d) {
    if (d <= 180 && d >= 0) {
        anglex2 = d * DEGREE + 2500;
    } else if (d > 180) {
        anglex1 = DEGREE180;
    } else
        anglex1 = DEGREE0;
}

unsigned char getAngle1(void)// internal angle of the servo1
{
    return ((anglex1 - 2500) / DEGREE);
}

unsigned char getAngle2(void) // internal angle of the servo2
{
    return(180- ((anglex2 - 2500) / DEGREE));
}


//where does th 80000000 come from?// clock of the processor 80 MHz

void __ISR(_TIMER_4_VECTOR, IPL1AUTO) Timer4Handler(void) {
    // if off change anglex for next time



    //    if(angle[0] >0)
    //    {
    //    LATBbits.LATB10 = 0;
    //    }
    //    else
    //    {
    //        LATBbits.LATB10 =1;
    //    }

    if (!setHigh1) {
        setAngle1(angle[SERVO1]);
        //y = P-x
        TMR4 = 65536 - (50000 - anglex1);
        LATBbits.LATB13 = 0;
        setHigh1 = true;


    } else {
        //TMR = R-x
        TMR4 = 65536 - anglex1;
        LATBbits.LATB13 = 1;
        setHigh1 = false;
    }

    IFS0CLR = _IFS0_T4IF_MASK;
}

void __ISR(_TIMER_5_VECTOR, IPL1AUTO) Timer5Handler(void) {
    // if off change anglex for next time


    //    if(angle[0] >0)
    //    {
    //    LATBbits.LATB10 = 0;
    //    }
    //    else
    //    {
    //        LATBbits.LATB10 =1;
    //    }

    if (!setHigh2) {
        setAngle2(angle[SERVO2]);
        //y = P-x
        TMR5 = 65536 - (50000 - anglex2);
        LATBbits.LATB12 = 0;
        setHigh2 = true;

    } else {
        //TMR = R-x
        TMR5 = 65536 - anglex2;
        LATBbits.LATB12 = 1;
        setHigh2 = false;
    }

    IFS0CLR = _IFS0_T5IF_MASK;
}



