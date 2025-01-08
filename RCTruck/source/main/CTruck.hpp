/************************************************************************************
 * Project:         Autonomous RC Truck                                             *
 * Area:            Car Computer (Local on MCU)                                     *
 *                                                                                  *
 * Author(s):       James Guest (35070432)                                          *
 * File:            CTruck.hpp                                                      *
 * Purpose:         To contain the functions and values associated with the truck   *
 * Description:                                                                     *
 *                                                                                  *
 *                                                                                  *
 ***********************************************************************************/

#ifndef C_TRUCK_H
#define C_TRUCK_H

/************************************ INCLUDES *************************************/
//Libraries
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <assert.h>
#include <array>
#include <numeric>
#include <cmath>
#include <limits>

extern "C" {
    #include "esp_timer.h"
}

//Local
#include "CServo.hpp"
#include "CMotor.hpp"
#include "CSensor.hpp"
#include "CMemory.hpp"

/**************************** GLOBAL DEFINITIONS ***********************************/
extern SemaphoreHandle_t xMutex;

/****************************** CLASS DEFINITIONS **********************************/

/*===========================================================================
 * Class:           CTruck
 * Description:     Class for managing the truck systems
 */
class CTruck {
    public:
        static CMotor motor;
        static CServo servo;
        static CMemory memory;
        static double setSpeed;
    private:
        static double position[2];
        static double velocity;
        static double heading;
        static double rotationRate;
    private:
        // Calculate the X position of the truck
        static double CalculateXDelta(double deltaTime, double heading, double acceleration=0);
        // Calculate the Y position of the truck
        static double CalculateYDelta(double deltaTime, double heading, double acceleration=0);
        // Calculate the Heading of the truck
        static double CalculateHeadingDelta(double deltaTime);
        // Calculate the Euclidean norm of a 2D vector
        static double CalculateEulerNorm(double deltaX, double deltaY);
        // Evaluate a linear function
        static double EvaluateLinearFunction(double m, double b, double w, double x);
        // Predict the future position of the truck
        static std::array<double, 2> PredictFuturePosition(double predictionTime=0.5, double timeResolution=0.01);
    public:
        //Default Constructor (initialization)
        static void InitializeTruck();
        // Calibrate the sensor
        static void CalibrateSensor(int sampleSize = 500);
        // Continuously follow the path until path end reached
        static void FollowPath(void *pvParameters);
        // Continuously update the states of the truck
        static void UpdateStates(void *pvParameters);
};

#endif // C_TRUCK_H