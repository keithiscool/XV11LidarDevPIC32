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


//Send Object Detection Info to Navigation Processor on Robot
bool sendRobotLocation(void) {
    if(timeFlagOneHundMilSecObjDet == true) {
    
        printf("----\r\n");
//        printf("obj_deg: %d / obj_rad: %g / obj_mag: %d / X: %d / Y: %d / Quality: %d\r\n", DetectedObject.degree, DetectedObject.radians, DetectedObject.polarDistance, DetectedObject.xPos, DetectedObject.yPos, DetectedObject.qualityOfObject);
        printf("obj_deg: %d / obj_mag: %d / X: %d / Y: %d / Quality: %d\r\n", DetectedObject.degree, DetectedObject.polarDistance, DetectedObject.xPos, DetectedObject.yPos, DetectedObject.qualityOfObject);
        printf("start: %d / stop: %d / diam: %d\r\n",DetectedObject.startOfDetectedObject, DetectedObject.endOfDetectedObject, DetectedObject.diameter);

        LATBbits.LATB10 ^= 1; //toggle on LED to signify a detected object with LED_B10 (LED 2)

        timeFlagOneHundMilSecObjDet = false;
    }
}


//<> == average
//Determine robot velocity == [sqrt(x^2 + y^2)]/<time duration since last reading>
void calculateVelocity(short objectXpos, short lastObjectXpos, short objectYpos, short lastObjectYpos) {
    DetectedObject.velocity = (short)(sqrt( ( ( (objectXpos-lastObjectXpos)^2) + ( (objectYpos-lastObjectYpos)^2) ) * 10 ) / timeHundMillisSinceObjectMoved); //Units in [millimeters/100ms counts]
//    DetectedObject.bearing = (short)(atan2( ( (double)abs(objectYpos-lastObjectYpos) ), ( (double)abs(objectXpos-lastObjectXpos) ) ) ); //this is incorrect -- need to used sign and magnitude (absolute value should not be here)
    DetectedObject.bearing = (short)(atan2( ( (double)(objectYpos-lastObjectYpos) ), ( (double)(objectXpos-lastObjectXpos) ) ) * (180/M_PI) ); //bearing is converted from radians to degrees
    timeHundMillisSinceObjectMoved = 0;
}


//Tracking robot direction and velocity vector
bool findRobotVector(void) {
    
    static short lastXpos = 0;
    static short lastYpos = 0;
    static short lastVelocity = 0;
    static short lastBearing = 0;
    static short lastObjectDiameter = 0;
    static char lastCompass[3] = "00";

    //check to see if the object moved (translation in x,y plane)
    if( (abs(DetectedObject.xPos-lastXpos) > objectMovedThreshold) || (abs(DetectedObject.yPos-lastYpos) > objectMovedThreshold) ) {

            if( (lastXpos > DetectedObject.xPos) && (lastYpos < DetectedObject.yPos) ) { //robot is moving NorthWest
                strcpy(DetectedObject.compass,"NW");
            }
            else if( (lastXpos < DetectedObject.xPos) && (lastYpos < DetectedObject.yPos) ) { //robot is moving NorthEast
                strcpy(DetectedObject.compass,"NE");
            }
            else if( (lastXpos > DetectedObject.xPos) && (lastYpos > DetectedObject.yPos) ) { //robot is moving SouthWest
                strcpy(DetectedObject.compass,"SW");
            }
            else if( (lastXpos < DetectedObject.xPos) && (lastYpos > DetectedObject.yPos) ) { //robot is moving SouthEast
                strcpy(DetectedObject.compass,"SE");
            }
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            else if(lastYpos < DetectedObject.yPos) { //robot is moving North
                strcpy(DetectedObject.compass,"_N");
            }
            else if(lastYpos > DetectedObject.yPos) { //robot is moving South
                strcpy(DetectedObject.compass,"_S");
            }
            else if(lastXpos > DetectedObject.xPos) { //robot is moving West
                strcpy(DetectedObject.compass,"_W");
            }
            else if(lastXpos < DetectedObject.xPos) { //robot is moving East
                strcpy(DetectedObject.compass,"_E");
            }
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            else { //the robot did not move, so it does not have a velocity vector
                //I suggest using the gyro to find the robot orientation while the robot does not move translationally
                strcpy(DetectedObject.compass,"00");
                DetectedObject.bearing = 0;
                DetectedObject.velocity = 0;
                DetectedObject.diameter = 0;
                return false;
            }

            if(DetectedObject.compass == lastCompass) { //robot headed in same direction as last measurement
                calculateVelocity(DetectedObject.xPos, lastXpos, DetectedObject.yPos, lastYpos);
            }
        }

    //record the last measurements taken in order to calculate the velocity vector the next iteration this function is called
    lastXpos = DetectedObject.xPos;
    lastYpos = DetectedObject.yPos;;
    lastVelocity = DetectedObject.velocity;
    strcpy(lastCompass, DetectedObject.compass);
    lastBearing = DetectedObject.bearing;
    lastObjectDiameter = DetectedObject.diameter;
    return true;
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
            
        //run through each distance measurement in each 22 byte parsed packet from "LIDARdecode"
        for(i=0;i<4;i++) {
            //only take certain degree measurements and valid distance measurements
            if( (DistanceArr[myDegrees[i]] > minDistanceAllowed) && (myDegrees[i] > objectTrackLowerDegree) && (myDegrees[i] < objectTrackUpperDegree) ) {
                presentDegree = myDegrees[i];

                //take magnitude difference from present distance measurement and the one previous (this distance will be used to detect objects if the gap between adjacent distances is large
                DistanceDifferencesArr[myDegrees[i]] = abs( DistanceArr[presentDegree] - DistanceArr[lastDegree] );

                //are the magnitudes different between degree measurements (first edge of object detected)
                if( (DistanceDifferencesArr[myDegrees[i]] > ObjectDetectionThreshold) && (ObjectStartEdgeDetected == false) ) { // object protruded from surrounding measurements by threshold
                    DetectedObject.startOfDetectedObject = myDegrees[i]; //found start of an object (object's corner was detected)
                    ObjectStartEdgeDetected = true; //first edge of object detected
//                    printf("Deg: %d starting_edge\r\n", DetectedObject.startOfDetectedObject);
                }
                //if a difference in magnitude is detected and there is already an object detected (last edge of object detected)
                else if( (DistanceDifferencesArr[myDegrees[i]] > ObjectDetectionThreshold) && (ObjectStartEdgeDetected == true) && (myDegrees[i] != DetectedObject.startOfDetectedObject) ) { // object protruded from surrounding measurements by threshold
                    DetectedObject.endOfDetectedObject = myDegrees[i]; //found end of an object (object's opposite corner was detected)
//                    printf("Deg: %d stop_edge\r\n", DetectedObject.endOfDetectedObject);

                    
////NOTE: THIS BROKE THE CODE (THIS CONDITION NEVER PASSED... NOT SURE WHY)
//                    check if object is wide enough (are there enough degrees between the start and the start of the object?)
//                    if( ( ( abs(DetectedObject.startOfDetectedObject - DetectedObject.endOfDetectedObject) ) > DEGREES_BETWEEN_EACH_OBJECT) ) {

                        //average and populate the data for the object into the object struct array
                        DetectedObject.polarDistance = ( (DistanceArr[DetectedObject.startOfDetectedObject] + DistanceArr[DetectedObject.endOfDetectedObject] ) / 2 );
                        DetectedObject.qualityOfObject = ( (QualityArr[DetectedObject.startOfDetectedObject] + QualityArr[DetectedObject.endOfDetectedObject] ) / 2 );

                        //only use objects that are reflective enough -- the robot bucket should be reflective (need to tune this threshold)
                        if( (DetectedObject.qualityOfObject > ROBOT_QUALITY_LOWER_THRESHOLD) && (DetectedObject.qualityOfObject < ROBOT_QUALITY_UPPER_THRESHOLD) ) {

////THANK YOU TO RYAN FOR REALIZING THE POSSIBILITY FOR THE TRIG FUNCTIONS USING RADIANS AND NOT DEGREES
                            //CONVERT TO RADIANS: RADIANS == (deg/2) * M_PI / 180
                            //also divide "deg" by 2 to get the center of DetectedObject.startOfDetectedObject and DetectedObject.endOfDetectedObject
                            DetectedObject.degree = ( DetectedObject.startOfDetectedObject + DetectedObject.endOfDetectedObject ) / 2;
                            DetectedObject.radians = ( (double)DetectedObject.degree ) * M_PI / 180;

                            //Do the math using floating point math to reduce error (fixed point math was not working for cosine terms and had +/- 10to50mm sometimes
    //                        float oldY = ((float)DetectedObject.polarDistance) * sin(DetectedObject.radians);
                            DetectedObject.yPos = (short)( (double)DetectedObject.polarDistance * sin(DetectedObject.radians) );
    //                        float oldX = ((float)DetectedObject.polarDistance) * cos(DetectedObject.radians);
                            DetectedObject.xPos = (short)( (double)DetectedObject.polarDistance * cos(DetectedObject.radians) );
    //                        printf("OldX: %f / NewX: %d / OldY: %f / NewY: %d\r\n", oldX, DetectedObject.xPos, oldY, DetectedObject.yPos);
                            DetectedObject.xPos -= X_POSITION_OFFSET_LIDAR_PLACEMENT; //correct the offset of the collection beacon (the collection beacon is not in the center of the arena)

                            //Calculate the size of the object (Side-Angle-Side - Use Law of Cosines to Get Unknown Side)
                            //a^2 = b^2 + c^2 - 2bc*cos(A)
                            //sqrt(a) == object diameter
                            short b = DistanceArr[DetectedObject.startOfDetectedObject]; //side
                            short A = DetectedObject.endOfDetectedObject - DetectedObject.startOfDetectedObject; //angle
                            short c = DistanceArr[DetectedObject.endOfDetectedObject]; //side
                            DetectedObject.diameter = (short)sqrt( (b^2 + c^2) - (2*b*c*(short)cos(A) ) ); //Diameter of object in millimeters

                            //check theat the entire object is located within the forward facing 180 degrees (prevent the larger degrees from only see one edge of a new object)
                            if(DetectedObject.startOfDetectedObject < DetectedObject.endOfDetectedObject) {
                                //Only track objects that are within (please see the MathCalcs.pdf for the cartesian limits of the arena
                                //NOTE: THE ARENA LIMITS INCLUDE THE X-AXIS OFFSET FROM THE LIDAR NOT BEING MOUNTED IN THE CENTER OF THE COLLECTION BIN WALL
                                if( (DetectedObject.xPos < 1102) && (DetectedObject.xPos > -2678) ) {
                                    //
                                    if(findRobotVector() == true) {
                                        printf("Compass: %s / Bearing: %d / Velocity: %d\r\n", DetectedObject.compass, DetectedObject.bearing, DetectedObject.velocity);
                                    }
                                    //print out detected object and send the data to the navigation processor on the robot from the beacon
                                    sendRobotLocation();
                                }
                            }
                        }

//                    }

                    //Object recorded, so reset flag that detects the first edge of the object
                    ObjectStartEdgeDetected = false; //last edge of object detected (reset flag)
                }
            }
            
            lastDegree = presentDegree;
        }
    }
  
    return 1;
}
