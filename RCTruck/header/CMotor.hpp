/************************************************************************************
 * Project:         Autonomous RC Truck                                             *
 * Area:            Car Computer (Local on MCU)                                     *
 *                                                                                  *
 * Author(s):       James Guest (35070432)                                          *
 * File:            main.cpp                                                        *
 * Purpose:         To contain supporting function declarations                     * 
 * Description:                                                                     *
 *                                                                                  *
 *                                                                                  *
 ***********************************************************************************/


#ifndef C_MOTOR_H
#define C_MOTOR_H

extern "C" {
    #include "/Users/jamesguest/esp-idf/components/freertos/FreeRTOS-Kernel/include/freertos/FreeRTOS.h"
    #include "/Users/jamesguest/esp-idf/components/freertos/FreeRTOS-Kernel/include/freertos/task.h"
    #include "/Users/jamesguest/esp-idf/components/esp_driver_mcpwm/include/esp_private/mcpwm.h"
    #include "/Users/jamesguest/esp-idf/components/soc/include/soc/mcpwm_periph.h"
    #include "/Users/jamesguest/esp-idf/components/esp_driver_gpio/include/driver/gpio.h"
    #include "/Users/jamesguest/esp-idf/components/log/include/esp_log.h"
}

class CMotor {
    private:
        gpio_num_t MOTOR_PIN_DIR_1  = (gpio_num_t)16; 
        gpio_num_t MOTOR_PIN_DIR_2  = (gpio_num_t)17;
        gpio_num_t MOTOR_ENABLE_PIN = (gpio_num_t)4;
    public:
        //Default Constructor (Initialize Pins)
        CMotor();
        //Sets the motor speed to a value between -100% (reverse) and 100% (forward)
        bool SetMotorSpeed(double speedPercentage);
};

#endif // C_MOTOR_H