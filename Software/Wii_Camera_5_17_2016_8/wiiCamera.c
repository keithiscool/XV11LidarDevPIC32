#include <xc.h>
#include "wiiCamera.h"

unsigned char wiiInitStep = 0;
bool pendingTrans1 = false;
bool pendingTrans2 = false;

unsigned short result2, result1;

unsigned int camera = 0xB0;
unsigned int data_address = 0x36;

unsigned char data[2];
struct DataBuffer data_buf[2];
unsigned short Ix_buffer[2][4] = {
    {0, 0, 0, 0},
    {0, 0, 0, 0}
};
unsigned short Iy_buffer[2][4] = {
    {0, 0, 0, 0},
    {0, 0, 0, 0}
};
unsigned short xBlob[2][2] = {
    {0, 0},
    {0, 0}
};
unsigned short average[2];
unsigned char blobsSeen[2] = {0, 0};
#define FAILED_I2C 2
#define SUCCESS_I2C 1

void initializeCheck(void) {
    if ((StatusI2Cone() == FAILED_I2C) || !pendingTrans1) {


        SendI2Cone(camera, &data, 2);
        pendingTrans1 = true;
    }
    if ((StatusI2Ctwo() == FAILED_I2C) || !pendingTrans2) {

        SendI2Ctwo(camera, &data, 2);
        pendingTrans2 = true;
    }

    //printf("InitStep %d\r\n", wiiInitStep);
    //printf("I2c one %d\r\n", StatusI2Cone());
    //printf("I2c two %d\r\n", StatusI2Ctwo());


    if ((StatusI2Ctwo() == SUCCESS_I2C) && (StatusI2Cone() == SUCCESS_I2C)) {

        pendingTrans2 = false;
        pendingTrans1 = false;
        wiiInitStep++;
    }
    if (StatusI2Ctwo() == 0) {
        //LATBbits.LATB9^=1;
    }
    if (StatusI2Cone() == 0) {
        // LATBbits.LATB10^=1;
    }
}

void initializeWii(void) {
    //setup the camera
    if (wiiInitStep == 0) {
        data[0] = 0x30;
        data[1] = 0x01;
        initializeCheck();
    } else if (wiiInitStep == 1) {
        data[0] = 0x30;
        data[1] = 0x08;
        initializeCheck();
    } else if (wiiInitStep == 2) {
        data[0] = 0x06;
        data[1] = 0x90;
        initializeCheck();
    } else if (wiiInitStep == 3) {
        data[0] = 0x08;
        data[1] = 0xC0;
        initializeCheck();
    } else if (wiiInitStep == 4) {
        data[0] = 0x1A;
        data[1] = 0x40;
        initializeCheck();
    } else if (wiiInitStep == 5) {
        data[0] = 0x33;
        data[1] = 0x33;
        initializeCheck();
    }


}

void SortInput(unsigned char cameraValue, unsigned char loc) {
    unsigned char i = 0;
    unsigned char s = 0;

    short newX = data_buf[cameraValue].data_buffer[loc - 1];
    short newY = data_buf[cameraValue].data_buffer[loc];
    s = data_buf[cameraValue].data_buffer[loc + 1];
    newX += (s & 0x30) << 4;
    newY += (s & 0xC0) << 2;

    if (newY < MAX_Y && newY > MIN_Y) {
        if (blobsSeen[cameraValue] > 0) {

            while (newX < Ix_buffer[cameraValue][i] && blobsSeen[cameraValue] <= 4) {
                i++;
            }

            unsigned char j = blobsSeen[cameraValue] - 1;
            while (j > i) {
                Ix_buffer[cameraValue][j + 1] = Ix_buffer[cameraValue][j];
                Iy_buffer[cameraValue][j + 1] = Iy_buffer[cameraValue][j];
                j--;
            }
            Ix_buffer[cameraValue][i] = newX;
            Iy_buffer[cameraValue][i] = newY;
            blobsSeen[cameraValue]++;
        } else {

            Ix_buffer[cameraValue][0] = newX;
            Iy_buffer[cameraValue][0] = newY;
            blobsSeen[cameraValue]++;
        }
    }
}
// for the horizontal beacon

unsigned short PickBeacon(unsigned char cameraValue) {

    switch (blobsSeen[cameraValue]) {
        case 0:
            return 0;
            break;
        case 1:
            xBlob[cameraValue][0] = 9999;
            xBlob[cameraValue][1] = 0;
            return 9999;
            break;
        case 2:
            //LATBbits.LATB10 =0;
            if ((Iy_buffer[cameraValue][0] - Iy_buffer[cameraValue][1]) < HEIGHT_VARIANCE) {
                xBlob[cameraValue][0] = Ix_buffer[cameraValue][0];
                xBlob[cameraValue][1] = Ix_buffer[cameraValue][1];
                return (Ix_buffer[cameraValue][0] + Ix_buffer[cameraValue][1]) / 2;
            }
            return 2000;
            break;
        case 3:
            LATBbits.LATB9 ^= 1;
            if ((Iy_buffer[cameraValue][0] - Iy_buffer[cameraValue][1]) < HEIGHT_VARIANCE) {
                xBlob[cameraValue][0] = Ix_buffer[cameraValue][0];
                xBlob[cameraValue][1] = Ix_buffer[cameraValue][1];
                return (Ix_buffer[cameraValue][0] + Ix_buffer[cameraValue][1]) / 2;
            }
            if ((Iy_buffer[cameraValue][1] - Iy_buffer[cameraValue][2]) < HEIGHT_VARIANCE) {
                xBlob[cameraValue][0] = Ix_buffer[cameraValue][1];
                xBlob[cameraValue][1] = Ix_buffer[cameraValue][2];
                return (Ix_buffer[cameraValue][1] + Ix_buffer[cameraValue][2]) / 2;
            }
            return 3000;
            break;
        case 4:
            if ((Iy_buffer[cameraValue][0] - Iy_buffer[cameraValue][1]) < HEIGHT_VARIANCE) {
                xBlob[cameraValue][0] = Ix_buffer[cameraValue][0];
                xBlob[cameraValue][1] = Ix_buffer[cameraValue][1];
                return (Ix_buffer[cameraValue][0] + Ix_buffer[cameraValue][1]) / 2;
            }
            if ((Iy_buffer[cameraValue][1] - Iy_buffer[cameraValue][2]) < HEIGHT_VARIANCE) {
                xBlob[cameraValue][0] = Ix_buffer[cameraValue][1];
                xBlob[cameraValue][1] = Ix_buffer[cameraValue][2];
                return (Ix_buffer[cameraValue][1] + Ix_buffer[cameraValue][2]) / 2;
            }
            if ((Iy_buffer[2] - Iy_buffer[3]) < HEIGHT_VARIANCE) {
                xBlob[cameraValue][0] = Ix_buffer[cameraValue][2];
                xBlob[cameraValue][1] = Ix_buffer[cameraValue][3];
                return (Ix_buffer[cameraValue][1] + Ix_buffer[cameraValue][2]) / 2;
            }
            return 4000;
            break;
    }
}

// case for the vertical beacon, do not care about Iy buffer
// need to comment out the y buffer all the times.
//unsigned short PickBeacon(unsigned char cameraValue) {
//    switch (blobsSeen[cameraValue]) {
//        case 0:
//            return 0;
//            break;
//        case 1:
//            // no error checking of validity
//            return Ix_buffer[cameraValue][0];
//            break;
//        case 2:
//            if ((Ix_buffer[cameraValue][0] - Ix_buffer[cameraValue][1]) < WIDTH_VARIANCE)
//                return (Ix_buffer[cameraValue][0] + Ix_buffer[cameraValue][1]) / 2;
//            return 2000;
//            break;
//        case 3:
//            if ((Ix_buffer[cameraValue][0] - Ix_buffer[cameraValue][1]) < WIDTH_VARIANCE)
//                return (Ix_buffer[cameraValue][0] + Ix_buffer[cameraValue][1]) / 2;
//            if ((Ix_buffer[cameraValue][1] - Ix_buffer[cameraValue][2]) < WIDTH_VARIANCE)
//                return (Ix_buffer[cameraValue][1] + Ix_buffer[cameraValue][2]) / 2;
//            return 3000;
//            break;
//        case 4:
//            if ((Ix_buffer[cameraValue][0] - Ix_buffer[cameraValue][1]) < WIDTH_VARIANCE)
//                return (Ix_buffer[cameraValue][0] + Ix_buffer[cameraValue][1]) / 2;
//            if ((Ix_buffer[cameraValue][1] - Ix_buffer[cameraValue][2]) < WIDTH_VARIANCE)
//                return (Ix_buffer[cameraValue][1] + Ix_buffer[cameraValue][2]) / 2;
//            if ((Ix_buffer[cameraValue][2] - Ix_buffer[cameraValue][3]) < WIDTH_VARIANCE)
//                return (Ix_buffer[cameraValue][1] + Ix_buffer[cameraValue][2]) / 2;
//            return 4000;
//            break;
//    }
//}

void read(unsigned char cameraValue) {

    if (cameraValue == 0) {
        ReceiveI2Cone(camera, data_address, &(data_buf[cameraValue].data_buffer), 16);
        while (StatusI2Cone() == 0);
    } else {
        ReceiveI2Ctwo(camera, data_address, &(data_buf[cameraValue].data_buffer), 16);

        while (StatusI2Ctwo() == 0);
    }


}

void Sort(unsigned char cameraValue) {
    // clears all the things
    blobsSeen[cameraValue] = 0;
    unsigned char k = 0;
    while (k < 4) {
        Ix_buffer[cameraValue][k] = 0;
        Iy_buffer[cameraValue][k] = 0;
        k++;
    }

    // is the data valid?
    if (data_buf[cameraValue].data_buffer[2] < 255)
        SortInput(cameraValue, 2);
    if (data_buf[cameraValue].data_buffer[5] < 255)
        SortInput(cameraValue, 5);
    if (data_buf[cameraValue].data_buffer[8] < 255)
        SortInput(cameraValue, 8);

    if (data_buf[cameraValue].data_buffer[11] < 255)
        SortInput(cameraValue, 11);


}

unsigned char getBlobsSeen(unsigned char cameraValue) {
    return blobsSeen[cameraValue];

}

short getDifferenceX(unsigned char cameraValue) {
    return abs(xBlob[cameraValue][0] - xBlob[cameraValue][1]);
}
