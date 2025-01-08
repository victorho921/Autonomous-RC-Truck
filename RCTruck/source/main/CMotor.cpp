/************************************************************************************
 * Project:         Autonomous RC Truck                                             *
 * Area:            Car Computer (Local on MCU)                                     *
 *                                                                                  *
 * Author(s):       James Guest (35070432)                                          *
 * File:            CMotor.cpp                                                      *
 * Purpose:         To contain the functions associated with the motor              *
 * Description:                                                                     *
 *                                                                                  *
 *                                                                                  *
 ***********************************************************************************/
 
/************************************ INCLUDES *************************************/
#include "CMotor.hpp"
#include "CTruck.hpp"

/***************************** FUNCTIONS DEFINITIONS *******************************/

/*===========================================================================
 * Function:        CMotor::CMotor
 * Arguments:       none
 * Returns:         none
 * Description:     Default Constructor (Initialize Pins)
 */
CMotor::CMotor() {
    // Configure direction pins as GPIO outputs
    ESP_LOGI("CMotor", "Initializing motor pins...");
    gpio_config_t gpio_conf = {
        .pin_bit_mask = (1ULL << this->PIN_DIR_1) | (1ULL << this->PIN_DIR_2),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&gpio_conf);

    // Initialize MCPWM module
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, this->ENABLE_PIN); // Use MCPWM0A channel

    mcpwm_config_t pwm_config = {
        .frequency = 1000, // 1 kHz frequency
        .cmpr_a = 0,       // Initial duty cycle of PWMxA = 0%
        .cmpr_b = 0,       // Not used in this example
        .duty_mode = MCPWM_DUTY_MODE_0, // Active high
        .counter_mode = MCPWM_UP_COUNTER,
    };
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);
    ESP_LOGI("CMotor", "Motor pins initialized.");
}



/*===========================================================================
 * Function:        CMotor::SetMotorSpeed
 * Arguments:       float speed_percent - speed percentage (0-100)
 *                  bool forward - true for forward, false for reverse
 * Returns:         none
 * Description:     Default Constructor (Initialize Pins)
 */
double CMotor::SetMotorSpeed(double speedPercentage) {
    bool forward = true;
    // Check if speed is negative and set direction
    if (speedPercentage < 0) {
        speedPercentage = abs(speedPercentage);
        forward = false;
    } else if (speedPercentage > 100 || speedPercentage < -100) {
        // Limit speed to 100%
        ESP_LOGW("CMotor", "Absolute speed percentage cannot be greater than 100. Setting to 100%%.");
        speedPercentage = speedPercentage > 0 ? 100 : -100;
    }
    if (speedPercentage == 0) {
        // Disable motor if speed is 0
        ESP_LOGI("CMotor", "Disabling Motor.");
        this->DisableMotor();
        return 0.0;
    } else if (!this->motorEnabled) {
        // Enable motor if speed is not 0
        ESP_LOGI("CMotor", "Enabling Motor.");
        this->EnableMotor();
    }

    if (forward) {
        gpio_set_level(this->PIN_DIR_1, 1); // Set DIR1 high
        gpio_set_level(this->PIN_DIR_2, 0); // Set DIR2 low
    } else {
        gpio_set_level(this->PIN_DIR_1, 0); // Set DIR1 low
        gpio_set_level(this->PIN_DIR_2, 1); // Set DIR2 high
    }
    
    // Scale speed to minThreshold - 100
    double setSpeed = minThreshold + (speedPercentage / 100.0) * (100 - minThreshold); 
    // Set PWM duty cycle
    ESP_LOGI("CMotor", "Setting motor speed to %.2f", speedPercentage);
    ESP_LOGI("CMotor", "Setting motor pwm to %.2f", setSpeed);
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, setSpeed);
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
    return speedPercentage;
}

/*===========================================================================
 * Function:        CMotor::DisableMotor
 * Arguments:       none
 * Returns:         none
 * Description:     Disables the motor
 */
void CMotor::DisableMotor() {
    ESP_LOGI("CMotor", "Disabling motor.");
    mcpwm_stop(MCPWM_UNIT_0, MCPWM_TIMER_0);
}

/*===========================================================================
 * Function:        CMotor::EnableMotor
 * Arguments:       none
 * Returns:         none
 * Description:     Enables the motor
 */
void CMotor::EnableMotor() {
    ESP_LOGI("CMotor", "Enabling motor.");
    mcpwm_start(MCPWM_UNIT_0, MCPWM_TIMER_0);
}
