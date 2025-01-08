
/************************************************************************************
 * Project:         Autonomous RC Truck                                             *
 * Area:            Car Computer (Local on MCU)                                     *
 *                                                                                  *
 * Author(s):       James Guest (35070432)                                          *
 * File:            CServo.cpp                                                      *
 * Purpose:         To contain the functions associated with the servo              *
 * Description:                                                                     *
 *                                                                                  *
 *                                                                                  *
 ***********************************************************************************/
 
/************************************ INCLUDES *************************************/
#include "CServo.hpp"

/***************************** FUNCTIONS DEFINITIONS *******************************/

/*===========================================================================
 * Function:        CServo::CServo
 * Arguments:       none
 * Returns:         none
 * Description:     Default Constructor (Initialize Pins)
 */
CServo::CServo() {
    // Initialize the servo PWM
    ESP_LOGI("CServo", "Initializing servo pin...");
    mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM0A, this->PIN); // Use MCPWM0A channel
    mcpwm_config_t pwm_config;
    pwm_config.frequency = 50; // Set frequency to 50Hz for servo
    pwm_config.cmpr_a = 0;     // Set initial duty cycle
    pwm_config.cmpr_b = 0;     // Not used
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    mcpwm_init(MCPWM_UNIT_1, MCPWM_TIMER_0, &pwm_config);
    ESP_LOGI("CServo", "Servo pin initialized.");
    SetServoAngle(0);
}

/*===========================================================================
 * Function:        CServo::SetServoAngle
 * Arguments:       int angle - angle between 70 and 130 degrees
 * Returns:         none
 * Description:     Sets the servo angle to a value between 70 and 130 degrees
 */
void CServo::SetServoAngle(double steeringAngle) {
    if (steeringAngle < this->minAngle) {
        ESP_LOGW("CServo", "Angle cannot be less than -50 degrees. Setting to -50 degrees instead of %3.0f.", steeringAngle);
        steeringAngle = this->minAngle;
    } else if (steeringAngle > this->maxAngle) {
        ESP_LOGW("CServo", "Angle cannot be greater than 50 degrees. Setting to 50 degrees instead of %3.0f.", steeringAngle);
        steeringAngle = this->maxAngle;
    }
    this->currentAngle = steeringAngle;
    double servoAngle = steeringAngle * 50 / 50 + 100 + this->trimOffset; // Scale angle to 70 - 130 degrees

    // Convert angle to duty cycle (0 - 180 degrees to PWM duty cycle)
    float dutyCycle = (servoAngle / 180.0) * (2.5 - 0.5) + 0.5; // Scale to 0.5ms - 2.5ms pulse width
    dutyCycle = dutyCycle / 20.0 * 100; // Convert to percentage

    // Set MCPWM duty
    ESP_LOGI("CServo", "Setting servo angle to %.2f degrees, for a steering angle of %.2f degress", servoAngle, steeringAngle);
    mcpwm_set_duty(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM_OPR_A, dutyCycle);
}

/*===========================================================================
 * Function:        CServo::incrementServoAngle
 * Arguments:       double angularChange - amount to increment the angle by
 * Returns:         none
 * Description:     Increments the servo angle by a given amount
 */
void CServo::incrementServoAngle(double angularChange){
    double newAngle = this->currentAngle + angularChange;
    SetServoAngle(newAngle);
}

/*===========================================================================
 * Function:        CServo::invertServoAngle
 * Arguments:       none
 * Returns:         none
 * Description:     Inverts the servo angle
 */
void CServo::invertServoAngle(){
    SetServoAngle(-this->currentAngle);
}