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

#ifndef C_MOTOR_H
#define C_MOTOR_H

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
#define EN_PIN    (gpio_num_t)4    // PWM (Enable pin)
#define DIR1_PIN  (gpio_num_t)16   // Direction pin 1
#define DIR2_PIN  (gpio_num_t)17   // Direction pin 2


/****************************** CLASS DEFINITIONS **********************************/

/*===========================================================================
 * Class:           CMotor
 * Description:     Class for managing the motor control
 */
class CMotor {
    private:
        gpio_num_t PIN_DIR_1  = DIR1_PIN; 
        gpio_num_t PIN_DIR_2  = DIR2_PIN;
        gpio_num_t ENABLE_PIN = EN_PIN;
        double minThreshold = 40.0; // Minimum threshold for motor speed
        int pathPoints = 0; // Number of points in the path
        bool motorEnabled = false;
    public:
        //Default Constructor (Initialize Pins)
        CMotor();
        //Sets the motor speed to a value between -100% (reverse) and 100% (forward)
        double SetMotorSpeed(double speedPercentage);
        //Disables the motor
        void DisableMotor();
        //Enables the motor
        void EnableMotor();
};

#endif // C_MOTOR_H