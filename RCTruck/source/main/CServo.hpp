/************************************************************************************
 * Project:         Autonomous RC Truck                                             *
 * Area:            Car Computer (Local on MCU)                                     *
 *                                                                                  *
 * Author(s):       James Guest (35070432)                                          *
 * File:            CServo.hpp                                                      *
 * Purpose:         To contain the functions associated with the servo              *
 * Description:                                                                     *
 *                                                                                  *
 *                                                                                  *
 ***********************************************************************************/

#ifndef C_SERVO_H
#define C_SERVO_H

/************************************ INCLUDES *************************************/
//Libraries
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <assert.h>
#include <array>
#include <numeric>

extern "C" {
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "driver/mcpwm.h"
    #include "soc/mcpwm_periph.h"
    #include "driver/gpio.h"
    #include "esp_log.h"
}


/******************************* PIN DEFINITIONS ***********************************/
#define SERVO_PIN 15  // Pin for the servo signal


/****************************** CLASS DEFINITIONS **********************************/

/*===========================================================================
 * Class:           CServo
 * Description:     Class for managing the servo control
 */
class CServo {
    private:
        int PIN  = SERVO_PIN; 
        double maxAngle = 30;
        double minAngle = -30;
        double trimOffset = -2;
        double currentAngle = 0;
    public:
        //Default Constructor (Initialize Pins)
        CServo();
        //Sets the servo angle to a value between 70 and 130 degrees
        void SetServoAngle(double angle);
        //Increments the servo angle by a given amount
        void incrementServoAngle(double angularChange);
        //Inverts the servo angle
        void invertServoAngle();
};

#endif // C_SERVO_H