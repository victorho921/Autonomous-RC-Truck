#include <unity.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "CTruck.hpp"

extern "C" void app_main();

void test_shared_counter_initial_value() {
    TEST_ASSERT_EQUAL(0, shared_counter);
}

void test_task1_increments_counter() {
    xSemaphoreTake(xMutex, portMAX_DELAY);
    shared_counter = 0;
    xSemaphoreGive(xMutex);

    task1(NULL);

    vTaskDelay(pdMS_TO_TICKS(150)); // Allow task1 to run
    TEST_ASSERT_EQUAL(1, shared_counter);
}

void test_task2_decrements_counter() {
    xSemaphoreTake(xMutex, portMAX_DELAY);
    shared_counter = 0;
    xSemaphoreGive(xMutex);

    task2(NULL);

    vTaskDelay(pdMS_TO_TICKS(250)); // Allow task2 to run
    TEST_ASSERT_EQUAL(-1, shared_counter);
}

extern "C" void app_main() {
    UNITY_BEGIN();

    RUN_TEST(test_shared_counter_initial_value);
    RUN_TEST(test_task1_increments_counter);
    RUN_TEST(test_task2_decrements_counter);

    UNITY_END();
}