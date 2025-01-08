/************************************************************************************
 * Project:         Autonomous RC Truck                                             *
 * Area:            Car Computer (Local on MCU)                                     *
 *                                                                                  *
 * Author(s):       James Guest (35070432)                                          *
 * File:            CSensor.hpp                                                     *
 * Purpose:         To contain the functions associated with the sensor             *
 * Description:                                                                     *
 *                                                                                  *
 *                                                                                  *
 ***********************************************************************************/

#ifndef C_SENSOR_H
#define C_SENSOR_H

/************************************ INCLUDES *************************************/
//Libraries
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <assert.h>
#include <array>
#include <numeric>

extern "C" {
    #include <driver/i2c.h>
    #include <esp_log.h>
    #include <freertos/FreeRTOS.h>
    #include <freertos/task.h>
    #include <math.h>
    #include "sdkconfig.h"
}


/******************************* PIN DEFINITIONS ***********************************/
#define ACCEL_SDA 21        // Define GPIO pin for I2C SDA (data line).
#define ACCEL_CLK 22        // Define GPIO pin for I2C SCL (clock line).

/**************************** ADDRESS DEFINITIONS **********************************/
#define ACCEL_I2C_ADDRESS 0x68            // Define the I2C address of the MPU6050 sensor.

#define ACCEL_XOUT_H 0x3B   // Register address for the high byte of accelerometer X-axis output.
#define GYRO_XOUT_H  0x43   // Register address for the high byte of gyroscope X-axis output.

#define ACCEL_PWR_MGMT_1   0x6B   // Register address for the power management configuration.

#define ACCEL_CONFIG 0x1C   // Register address for accelerometer configuration.
#define GYRO_CONFIG  0x1B   // Register address for gyroscope configuration.

/******************************* ENUM DEFINITIONS **********************************/

/*===========================================================================
 * Class:           SensorReadingType
 * Description:     Enum for managing the sensor readings
 */
enum SensorReadingType {
    ACCELEROMETER,
    GYROSCOPE
};


/****************************** CLASS DEFINITIONS **********************************/

/*===========================================================================
 * Class:           CSensor
 * Description:     Class for managing the accelerometer readings
 */
class CSensor {
    private:
        static int SDA; 
        static int CLK;
        static int I2C_ADDRESS;
        static int ACC_X_HIGH_REGISTER;
        static int GYR_X_HIGH_REGISTER;
        static int PWR_MGMT_1_ADDRESS;
        static int ACCEL_CONFIG_ADDRESS;
        static int GYRO_CONFIG_ADDRESS;
        static double GyroCalibrationOffsets[3];
        static double AccelCalibrationOffsets[3];
    private:
        // Function to write to a register
        static void WriteRegister(uint8_t reg, uint8_t value);
        // Function to read from a register
        static void ReadRegister(uint8_t registerAddress, uint8_t *data, int length);
        // Function to parse the least significant bits of the XYZ values
        static void XYZLeastSignificantBitParser(uint8_t const *data, std::array<short,3> &LSB);
        // Function to scale the XYZ values
        static void XYZScaler(std::array<short,3> const &LSB, std::array<double,3> &accel, double scalingFactor);
    public:
        //Default Constructor (Initialize Pins)
        static void InitializeSensor();
        //Reads the sensor values
        static std::array<double, 3> ReadSensor(enum SensorReadingType type);
        //Calibrates the gyroscope
        static void CalibrateGyro(int sampleSize = 500);
        //Calibrates the accelerometer
        static void CalibrateAccel(int sampleSize = 500);
};

#endif // C_SENSOR_H