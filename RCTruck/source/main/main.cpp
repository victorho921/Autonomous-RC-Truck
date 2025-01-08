/************************************************************************************
 * Project:         Autonomous RC Truck                                             *
 * Area:            Car Computer (Local on MCU)                                     *
 *                                                                                  *
 * Author(s):       James Guest (35070432)                                          *
 * File:            main.cpp                                                        *
 * Purpose:         To contain the main function.                                   *
 * Description:                                                                     *
 *                                                                                  *
 *                                                                                  *
 ***********************************************************************************/
 

/************************************ INCLUDES *************************************/
//Libraries
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <assert.h>
#include <array>
#include <numeric>

extern "C" {
    #include "esp_timer.h"
}


//Local
#include "CServo.hpp"
#include "CMotor.hpp"
#include "CSensor.hpp"
#include "CTruck.hpp"

/**************************** GLOBAL DEFINITIONS ***********************************/
SemaphoreHandle_t xMutex;

/***************************** FUNCTIONS DEFINITIONS *******************************/

int shared_counter = 0;

void task1(void *pvParameters) {
    while (1) {
        if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
            shared_counter++;
            xSemaphoreGive(xMutex);
        }
        printf("Task 1: %d\n", shared_counter);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void task2(void *pvParameters) {
    while (1) {
        if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
            shared_counter--;
            xSemaphoreGive(xMutex);
        }
        printf("Task 2: %d\n", shared_counter);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}


/*===========================================================================
 * Function:        app_main
 * Arguments:       none
 * Returns:         none
 */
extern "C" void app_main() {
    // vTaskDelay(pdMS_TO_TICKS(2000));
    // xMutex = xSemaphoreCreateMutex();
    // xTaskCreatePinnedToCore(task1, "Task1", 2048, NULL, 5, NULL, 0);
    // xTaskCreatePinnedToCore(task2, "Task2", 2048, NULL, 5, NULL, 1);


    CTruck::InitializeTruck();
    CTruck::memory.WritePath({{0,0,0}, {0,1,40}, {1,1.2,40}, {1,0.2,40}, {0,0,40}});
    // CTruck::servo.SetServoAngle(0);
    // CTruck::motor.SetMotorSpeed(0);
    // CTruck::servo.incrementServoAngle(10);
    // vTaskDelay(1000 / portTICK_PERIOD_MS);
    // CTruck::servo.incrementServoAngle(10);
    // vTaskDelay(1000 / portTICK_PERIOD_MS);
    // CTruck::servo.incrementServoAngle(-20);
    // vTaskDelay(1000 / portTICK_PERIOD_MS);
    // CTruck::servo.incrementServoAngle(-20);
    // vTaskDelay(1000 / portTICK_PERIOD_MS);
    // CTruck::servo.incrementServoAngle(10);
    // vTaskDelay(1000 / portTICK_PERIOD_MS);
    // CTruck::servo.incrementServoAngle(10);
    // vTaskDelay(1000 / portTICK_PERIOD_MS);


    xMutex = xSemaphoreCreateMutex();
    xTaskCreatePinnedToCore(CTruck::FollowPath, "PathFollowing", 4096, NULL, 5, NULL, 1);
    xTaskCreatePinnedToCore(CTruck::UpdateStates, "StateUpdating", 4096, NULL, 5, NULL, 0);

    printf("Main task complete.\n");
    
}