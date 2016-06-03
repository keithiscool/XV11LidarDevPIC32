/* File:   ObjectDetection.c
 * Author: Keith
 * Created on February 13, 2016, 1:46 PM
 */

//ALSO LOOK INTO "LINKED_LISTS" Referencing: http://www.cprogramming.com/tutorial/c/lesson15.html

#include <xc.h>
#include "ObjectDetection.h"



bool initObjectDetection(void) {
//    arrayofDetectedObjects[OBJECT_ARRAY_STRUCT_SIZE] = emptyObjectStruct; //initialize all struct array elements to zero
    DetectedObject = emptyObjectStruct; //initialize all struct elements to zero
}


bool sendRobotLocation(short degree) {
    if( (timeFlagOneHundMilSec == true) && (RobotDetected == true) ) {
        printf("Deg: %d / Dist: %d / Qual: %d / X: %d / Y: %d\r\n",degree, DistanceArr[degree],
            QualityArr[degree], XCoordMilliMeters[degree], XCoordMilliMeters[degree]);
            timeFlagOneHundMilSec = false;
    }
}


//Check whether the change in Distance between each degree is large -> indicates object or wall
//this function compares the ditances between polar magnitudes 4-at-a-time
short distDiffObjectDetection(void) {
    //temporary array degree values to remember
    short myDegrees[4] = {0, 0, 0, 0};
    short i = 0;
    //track object with degrees that are not adjacent (I have too many zeros as distances in output)
    static short lastDegree = 0;
    static short presentDegree = 0;

    //Acquire 4 distances at a time and constantly pull in data (do not print out data)
    //Exclude any data between 180 and 0 degrees because that is behind the collection bin
    //data element 176 is the last index received in the 90 packets (4 degrees per packet * 90 packets = 360 degrees total)
    //if data > 180 degrees, this check should protect against that
    if(LIDARdecode(myDegrees) == true) {//&& (myDegrees[0] < 178) ) { //Acquire 4 distances at a time and constantly pull in data (do not print out data)
//        printf("distDiffObjectDetection\r\n");
//
            
        //run through each distance measurement (4 distances in each 22 byte parsed packet from "LIDARdecode()")
        for(i=0;i<4;i++) {
            
            if( (DistanceArr[myDegrees[i]] > minDistanceAllowed) && ( (myDegrees[i] > objectTrackLower) && (myDegrees[i] < objectTrackUpper) ) ) {
                presentDegree = myDegrees[i];
//                    printf("valid_measurements\r\n");

                //take magnitude difference from present distance measurement and the one previous (this distance will be used to detect objects if the gap between adjacent distances is large
                DistanceDifferencesArr[myDegrees[i]] = abs( DistanceArr[presentDegree] - DistanceArr[lastDegree] );
//                printf("dist_diff_calc\r\n");

                //are the magnitudes different between degree measurements (first edge of object detected)
                if( (DistanceDifferencesArr[myDegrees[i]] > ObjectDetectionThreshold) && (ObjectStartEdgeDetected == false) ) { // object protruded from surrounding measurements by threshold
                    DetectedObject.startOfDetectedObject = myDegrees[i]; //found start of an object (object's corner was detected)
                    ObjectStartEdgeDetected = true; //first edge of object detected
                    printf("Deg: %d starting_edge\r\n", DetectedObject.startOfDetectedObject);
                }

                //if a difference in magnitude is detected and there is already an object detected (last edge of object detected)
                if( (DistanceDifferencesArr[myDegrees[i]] > ObjectDetectionThreshold) && (ObjectStartEdgeDetected == true) ) { // object protruded from surrounding measurements by threshold
                        DetectedObject.endOfDetectedObject = myDegrees[i]; //found start of an object (object's corner was detected)
                        printf("Deg: %d stop_edge\r\n", DetectedObject.endOfDetectedObject);

                        
//////******THIS CONDITION IS NEVER MET FOR SOME REASON... (DOES NOT GET IN THIS if() STATEMENT. NOT SURE WHY OBJECTS CANNOT BE VERIFIED TO BE SEPARATE OBJECTS (NOT NEXT TO EACH OTHER... *****/////////////////
                        //check if object is wide enough (are there enough degrees between the start and the start of the object?)
//                        if( ( ( abs(DetectedObject.startOfDetectedObject - DetectedObject.endOfDetectedObject) ) > DEGREES_BETWEEN_EACH_OBJECT) ) {

                        
                        //average and populate the data for the object into the object struct array
                        DetectedObject.polarDistance = (( DistanceArr[DetectedObject.startOfDetectedObject] + DistanceArr[DetectedObject.endOfDetectedObject] ) / 2 );

                        ////THANK YOU TO RYAN FOR REALIZING THE POSSIBILITY FOR THE TRIG FUNCTIONS USING RADIANS AND NOT DEGREES
                        //CONVERT TO RADIANS: RADIANS == deg * M_PI / 180.0;
                        DetectedObject.degree = ( ( DetectedObject.startOfDetectedObject + DetectedObject.endOfDetectedObject ) / 2 );
                        DetectedObject.degree = ( ( ( DetectedObject.startOfDetectedObject + DetectedObject.endOfDetectedObject ) * M_PI ) / 360 );

                        DetectedObject.qualityOfObject = ( ( QualityArr[DetectedObject.startOfDetectedObject] + QualityArr[DetectedObject.endOfDetectedObject] ) / 2 );

                        DetectedObject.yPos = DetectedObject.polarDistance * cos(DetectedObject.degree);
                        DetectedObject.xPos = DetectedObject.polarDistance * sin(DetectedObject.degree);
                        DetectedObject.xPos -= X_POSITION_OFFSET_LIDAR_PLACEMENT; //correct the offset of the collection beacon (the collection beacon is not in the center of the arena, because it is right-of-center of collection bin)

                        printf("obj_deg: %d / obj_mag: %d / X: %d / Y: %d\r\n",DetectedObject.degree, DetectedObject.polarDistance, DetectedObject.xPos, DetectedObject.yPos);
                        printf("start: %d / stop: %d \r\n",DetectedObject.startOfDetectedObject, DetectedObject.endOfDetectedObject);

                        //Object recorded, so reset flag that detects the first edge of the object
                        ObjectStartEdgeDetected = false; //last edge of object detected (reset flag)
                        
//                    }



                }
            }
            lastDegree = presentDegree;
        }
    }

    //Indicate when an object is detected
    if( (timeFlagOneHundMilSec == true) && (DetectedObject.polarDistance > 0) ) {
        //blinkLED(1000); //debug led to show object detected
        printf("obj\r\n deg: %4d mag: %4d\r\n", DetectedObject.degree, DetectedObject.polarDistance);
        LATBbits.LATB10 ^= 1; //toggle on LED to signify a detected object with LED_B10 (LED 2)
        timeFlagOneHundMilSec = false;
    }
    
    return 1;
}


short avgLidarPolarDist(void) {
    /*average the last 3 distance measurements with the present distance measurement
     * I assume the update rate for the lidar is much faster than when I debugged the lidar
     * -> I had to shut off receiving data from the lidar in order to print the debug data*/
    short previousMeasScaled = 1 << 6;
    short presentMeasScaled = 1 << 6;



}





/* I WOULD LIKE TO USE A RELIABLE PROBABILITY VERIFICATION FOR THE DATA TO USE PREVIOUS AND CURRENT MEASUREMENTS TO GET THE UP-TO-DATE MEASUREMENT,
 * BUT THE DATA IS NOT CONSTANT, SO COMPLEMENTARY FILTER CANNOT BE USED. ALSO, I NEED TO DETERMINE THE COVARIANCE MATRICES AND HAVE NO IDEA HOW TO DO THAT...
 * NOTE: SOME PEOPLE CALL THE COVARIANCE MATRIX THE IDENTITY... WHY?????
 * 
 * REFERENCE: http://robottini.altervista.org/kalman-filter-vs-complementary-filter
 */
//void initKalmanFilter(void) {
//    /* We will set the variables like so, these can also be tuned by the user */
//    kalmanFilter.Q_angle = 0.001f;
//    kalmanFilter.Q_bias = 0.003f;
//    kalmanFilter.R_measure = 0.03f;
//
//    kalmanFilter.P[0][0] = 0.0f; // Since we assume that the bias is 0 and we know the starting angle (use setAngle), the error covariance matrix is set like so - see: http://en.wikipedia.org/wiki/Kalman_filter#Example_application.2C_technical
//    kalmanFilter.P[0][1] = 0.0f;
//    kalmanFilter.P[1][0] = 0.0f;
//    kalmanFilter.P[1][1] = 0.0f;
//    kalmanFilter.rateOfMovement = 0.0f;
//    kalmanFilter.locationOfRobot = 0.0f;
//}
//
//
//// The angle should be in degrees and the rate should be in degrees per second and the delta time in seconds
//float kalmanFilterGetRobotPosition(float newPos, float newRate, float dt) {
//    // Kalman filter module - http://www.x-firm.com/?page_id=145
//    // Modified by Kristian Lauszus
//    // See Kristian's blog post for more information: http://blog.tkjelectronics.dk/2012/09/a-practical-approach-to-kalman-filter-and-how-to-implement-it
//
//    // Discrete Kalman filter time update equations - Time Update ("Predict")
//    // Update xhat - Project the state ahead
//    /* Step 1 */
//    kalmanFilter.rate = newRate - kalmanFilter.bias;
//    kalmanFilter.angle += dt * kalmanFilter.rate;
//
//    // Update estimation error covariance - Project the error covariance ahead
//    /* Step 2 */
//    kalmanFilter.P[0][0] += dt * (dt*kalmanFilter.P[1][1] - kalmanFilter.P[0][1] - kalmanFilter.P[1][0] + kalmanFilter.Q_angle);
//    kalmanFilter.P[0][1] -= dt * kalmanFilter.P[1][1];
//    kalmanFilter.P[1][0] -= dt * kalmanFilter.P[1][1];
//    kalmanFilter.P[1][1] += kalmanFilter.Q_bias * dt;
//
//    // Calculate angle and bias - Update estimate with measurement zk (newAngle)
//    /* Step 3 */
//    float y = newAngle - angle; // Angle difference
//
//    // Discrete Kalman filter measurement update equations - Measurement Update ("Correct")
//    // Calculate Kalman gain - Compute the Kalman gain
//    /* Step 4 */
//    float S = kalmanFilter.P[0][0] + kalmanFilter.R_measure; // Estimate error
//
//    /* Step 5 */
//    float K[2]; // Kalman gain - This is a 2x1 vector
//    K[0] = kalmanFilter.P[0][0] / S;
//    K[1] = kalmanFilter.P[1][0] / S;
//
//    /* Step 6 */
//    kalmanFilter.angle += K[0] * y;
//    kalmanFilter.bias += K[1] * y;
//
//    // Calculate estimation error covariance - Update the error covariance
//    /* Step 7 */
//    float P00_temp = P[0][0];
//    float P01_temp = P[0][1];
//
//    kalmanFilter.P[0][0] -= K[0] * P00_temp;
//    kalmanFilter.P[0][1] -= K[0] * P01_temp;
//    kalmanFilter.P[1][0] -= K[1] * P00_temp;
//    kalmanFilter.P[1][1] -= K[1] * P01_temp;
//
//    return kalmanFilter.angle;
//};



//COMPLEMENTARY FILTER:
//float tau=0.075;
//float a=0.0;
//
//// a=tau / (tau + loop time)
//// newAngle = angle measured with atan2 using the accelerometer
//// newRate =  angle measured using the gyro
//// looptime = loop time in millis()
//
//
//float Complementary(float newAngle, float newRate,int looptime) {
//  float dtC = float(looptime)/1000.0;
//  a=tau/(tau+dtC) ;
//  x_angleC= a* (x_angleC + newRate * dtC) + (1-a) * (newAngle);
//
//  return x_angleC;
//}