/************************************************************************************
 * Project:         Autonomous RC Truck                                             *
 * Area:            Car Computer (Local on MCU)                                     *
 *                                                                                  *
 * Author(s):       James Guest (35070432)                                          *
 * File:            CSensor.cpp                                                     *
 * Purpose:         To contain the functions associated with the sensor             *
 * Description:                                                                     *
 *                                                                                  *
 *                                                                                  *
 ***********************************************************************************/
 
/************************************ INCLUDES *************************************/
#include "CSensor.hpp"


// Define the static member variables
int CSensor::SDA = ACCEL_SDA;
int CSensor::CLK = ACCEL_CLK;
int CSensor::I2C_ADDRESS = ACCEL_I2C_ADDRESS;
int CSensor::ACC_X_HIGH_REGISTER = ACCEL_XOUT_H;
int CSensor::GYR_X_HIGH_REGISTER = GYRO_XOUT_H;
int CSensor::PWR_MGMT_1_ADDRESS = ACCEL_PWR_MGMT_1;
int CSensor::ACCEL_CONFIG_ADDRESS = ACCEL_CONFIG;
int CSensor::GYRO_CONFIG_ADDRESS = GYRO_CONFIG;
double CSensor::GyroCalibrationOffsets[3] = {0, 0, 0};
double CSensor::AccelCalibrationOffsets[3] = {0, 0, 0};

/***************************** FUNCTIONS DEFINITIONS *******************************/

/*===========================================================================
 * Function:        CSensor::CSensor
 * Arguments:       none
 * Returns:         none
 * Description:     Default Constructor (Initialize Pins)
 */
void CSensor::InitializeSensor() {
    ESP_LOGI("CSensor", "Initializing sensor I2C...");

    i2c_config_t conf = {}; // Create a configuration object for the I2C interface.
    conf.mode = I2C_MODE_MASTER;        // Set I2C to master mode.
    conf.sda_io_num = CSensor::SDA;          // Assign the GPIO pin for SDA.
    conf.scl_io_num = CSensor::CLK;          // Assign the GPIO pin for SCL.
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE; // Enable pull-up resistor for SDA.
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE; // Enable pull-up resistor for SCL.
    conf.master.clk_speed = 100000;     // Set I2C clock speed to 100 kHz.

    ESP_LOGV("CSensor", "Configuring I2C parameters...");
    ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &conf)); // Configure I2C with these parameters.

    ESP_LOGV("CSensor", "Installing I2C driver...");
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0)); // Install I2C driver.

    ESP_LOGI("CSensor", "Sensor I2C initialized.");

    ESP_LOGV("CSensor", "Delaying for stability...");
    vTaskDelay(pdMS_TO_TICKS(200)); // Delay for stability.
    ESP_LOGV("CSensor", "Delay complete.");


    ESP_LOGI("CSensor", "Creating I2C command link...");
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    if (cmd == NULL) {
        ESP_LOGE("CSensor", "Failed to create I2C command link");
        return; // Handle error appropriately
    }

    ESP_LOGI("CSensor", "Waking up sensor...");
    ESP_ERROR_CHECK(i2c_master_start(cmd));                                   // Start I2C communication.
    ESP_LOGV("CSensor", "i2c_master_start(cmd)");
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (CSensor::I2C_ADDRESS << 1) | I2C_MASTER_WRITE, 1)); // Address the MPU6050 in write mode.
    ESP_LOGV("CSensor", "i2c_master_write_byte(cmd, (CSensor::I2C_ADDRESS << 1) | I2C_MASTER_WRITE, 1)");
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, CSensor::PWR_MGMT_1_ADDRESS, 1)); // Set the pointer to the power management register.
    ESP_LOGV("CSensor", "i2c_master_write_byte(cmd, CSensor::PWR_MGMT_1_ADDRESS, 1)");
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, 0, 1));                        // Write 0 to wake up the MPU6050.
    ESP_LOGV("CSensor", "i2c_master_write_byte(cmd, 0, 1)");
    ESP_ERROR_CHECK(i2c_master_stop(cmd));                                    // Stop I2C communication.
    ESP_LOGV("CSensor", "i2c_master_stop(cmd)");
    ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(1000))); // Execute the command.
    ESP_LOGV("CSensor", "i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS)");
    i2c_cmd_link_delete(cmd);                                                // Clean up the command link.
    ESP_LOGV("CSensor", "i2c_cmd_link_delete(cmd)");
    ESP_LOGI("CSensor", "Sensor woken up.");

    ESP_LOGI("CSensor", "Configuring sensor ranges...");
    // Set accelerometer range to ±8g
    WriteRegister(CSensor::ACCEL_CONFIG_ADDRESS, 0x00); // 0x10 corresponds to ±2g.

    // Set gyroscope range to ±250 degrees/second
    WriteRegister(CSensor::GYRO_CONFIG_ADDRESS, 0x00); // 0x10 corresponds to ±250 dps.
    ESP_LOGI("CSensor", "Sensor ranges configured.");
}

/*===========================================================================
 * Function:        CSensor::WriteRegister
 * Arguments:       uint8_t reg - register address
 *                  uint8_t value - value to write
 * Returns:         none
 * Description:     Function to write to a register
 */
void CSensor::WriteRegister(uint8_t registerAddress, uint8_t value) {
    ESP_LOGI("CSensor", "Setting register 0x%02x to 0x%02x...", registerAddress, value);
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    ESP_ERROR_CHECK(i2c_master_start(cmd));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (I2C_ADDRESS << 1) | I2C_MASTER_WRITE, 1));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, registerAddress, 1));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, value, 1));
    ESP_ERROR_CHECK(i2c_master_stop(cmd));
    ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS));
    i2c_cmd_link_delete(cmd);
    ESP_LOGD("CSensor", "Register 0x%02x set to 0x%02x", registerAddress, value);
}

/*===========================================================================
 * Function:        CSensor::ReadRegister
 * Arguments:       uint8_t registerAddress - register address to read from
 *                  uint8_t *data - pointer to data buffer
 *                  int length - length of data to read in # of bytes
 * Returns:         none
 * Description:     Function to read a specified length from a register
 */
void CSensor::ReadRegister(uint8_t registerAddress, uint8_t *data, int length) {
    ESP_LOGD("CSensor", "Reading %d bytes from register 0x%02x...", length, registerAddress);
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    ESP_ERROR_CHECK(i2c_master_start(cmd));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (I2C_ADDRESS << 1) | I2C_MASTER_WRITE, 1));
    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, registerAddress, 1));
    ESP_ERROR_CHECK(i2c_master_start(cmd));

    ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (I2C_ADDRESS << 1) | I2C_MASTER_READ, 1));
    ESP_ERROR_CHECK(i2c_master_read(cmd, data, length, I2C_MASTER_LAST_NACK));
    ESP_ERROR_CHECK(i2c_master_stop(cmd));
    ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, cmd, portMAX_DELAY));
    i2c_cmd_link_delete(cmd);
    ESP_LOGD("CSensor", "Read %d bytes from register 0x%02x", length, registerAddress);
}

/*===========================================================================
 * Function:        CSensor::XYZLeastSignificantBitParser
 * Arguments:       uint8_t *data - pointer to data buffer
 *                  std::array<short,3> &LSB - reference to array to store LSB values
 * Returns:         none
 * Description:     Function to parse the least significant bits of the XYZ values
 */
void CSensor::XYZLeastSignificantBitParser(uint8_t  const *data, std::array<short,3> &LSB) {
    for (int i = 0; i < 3; i++) {
        LSB[i] = (data[i*2] << 8) | data[i*2 + 1];
    }
    ESP_LOGD("CSensor", "LSB values: X: %d, Y: %d, Z: %d", LSB[0], LSB[1], LSB[2]);
}

/*===========================================================================
 * Function:        CSensor::XYZScaler
 * Arguments:       uint8_t *data - pointer to data buffer
 *                  std::array<short,3> &LSB - reference to array to store LSB values
 * Returns:         none
 * Description:     Function to parse the least significant bits of the XYZ values
 */
void CSensor::XYZScaler(std::array<short,3> const &LSB, std::array<double,3> &readings, double const scalingFactor) {
    for (int i = 0; i < 3; i++) {
        readings[i] = (double)LSB[i] / 16384.0 * scalingFactor;
    }
}

/*===========================================================================
 * Function:        CSensor::ReadSensor
 * Arguments:       enum SensorReadingType type - type of sensor reading
 * Returns:         std::array<double, 3> - array of X, Y, Z readings
 * Description:     Reads the accelerometer/gyroscope values
 */
std::array<double, 3> CSensor::ReadSensor(enum SensorReadingType type){
    uint8_t data[14]; 
    std::array<short,3> LSB = {0,0,0}; 
    std::array<double,3> readings = {0,0,0};
    if (type == ACCELEROMETER) {
        ESP_LOGD("CSensor", "Reading accelerometer values...");
        ReadRegister(CSensor::ACC_X_HIGH_REGISTER, data, 6);
    } else if (type == GYROSCOPE) {
        ESP_LOGD("CSensor", "Reading gyroscope values...");
        ReadRegister(CSensor::GYR_X_HIGH_REGISTER, data, 6);
    } else {
        ESP_LOGE("CSensor", "Invalid sensor reading type.");
    }
    XYZLeastSignificantBitParser(data, LSB);
    if (type == ACCELEROMETER) {
        XYZScaler(LSB, readings, 9.81);
        for (int i = 0; i < 3; i++) {
            readings[i] -= CSensor::AccelCalibrationOffsets[i];
        }
        ESP_LOGD("CSensor", "Sensor values returned: X: %.2fm/s2, Y: %.2fm/s2, Z: %.2fm/s2", 
                    readings[0], readings[1], readings[2]);
    } else if (type == GYROSCOPE) {
        XYZScaler(LSB, readings, 250);
        for (int i = 0; i < 3; i++) {
            readings[i] -= CSensor::GyroCalibrationOffsets[i];
        }
        ESP_LOGD("CSensor", "Sensor values returned: X: %.2fdeg/s, Y: %.2fdeg/s, Z: %.2fdeg/s", 
                    readings[0], readings[1], readings[2]);
    } else {
        ESP_LOGE("CSensor", "Invalid sensor reading type.");
    }
    
    return readings;
}

/*===========================================================================
 * Function:        CSensor::CalibrateGyro
 * Arguments:       int sampleSize - number of samples to take for calibration
 * Returns:         none
 * Description:     Calibrates the gyroscope
 */
void CSensor::CalibrateGyro(int sampleSize) {
    if (sampleSize < 1 || sampleSize > 1000) {
        ESP_LOGE("CSensor", "Sample size must be greater than 0 and less than 1000, setting to 100.");
        sampleSize = 100;
    }
    std::array<double, 3> readings = {0,0,0};
    ESP_LOGI("CSensor", "Calibrating gyro...");
    for (int i = 0; i < sampleSize; i++) {
        std::array<double, 3> singleReading = ReadSensor(GYROSCOPE);
        for (int j = 0; j < 3; j++) {
            readings[j] += singleReading[j] / sampleSize;
        }
        vTaskDelay(50 / portTICK_PERIOD_MS);
        printf("\rCalibrating Gyro... %.1f%%", (i + 1) * 100.0 / sampleSize);
    }
    printf("\n");
    for (int i = 0; i < 3; i++) {
        CSensor::GyroCalibrationOffsets[i] = readings[i];
    }
    ESP_LOGI("CSensor", "Gyro calibrated with offsets: X: %.2f, Y: %.2f, Z: %.2f", 
                readings[0], readings[1], readings[2]);
}

/*===========================================================================
 * Function:        CSensor::CalibrateAccel
 * Arguments:       int sampleSize - number of samples to take for calibration
 * Returns:         none
 * Description:     Calibrates the accelerometer
 */
void CSensor::CalibrateAccel(int sampleSize) {
    if (sampleSize < 1 || sampleSize > 1000) {
        ESP_LOGE("CSensor", "Sample size must be greater than 0 and less than 1000, setting to 100.");
        sampleSize = 100;
    }
    std::array<double, 3> readings = {0,0,0};
    ESP_LOGI("CSensor", "Calibrating accelerometer...");
    for (int i = 0; i < sampleSize; i++) {
        std::array<double, 3> singleReading = ReadSensor(ACCELEROMETER);
        for (int j = 0; j < 3; j++) {
            readings[j] += singleReading[j] / sampleSize;
        }
        vTaskDelay(50 / portTICK_PERIOD_MS);
        printf("\rCalibrating Accelerometer... %.1f%%", (i + 1) * 100.0 / sampleSize);
    }
    printf("\n");
    for (int i = 0; i < 3; i++) {
        CSensor::AccelCalibrationOffsets[i] = readings[i];
    }
    ESP_LOGI("CSensor", "Accelerometer calibrated with offsets: X: %.2f, Y: %.2f, Z: %.2f", 
                readings[0], readings[1], readings[2]);
}

