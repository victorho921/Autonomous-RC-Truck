/************************************************************************************
 * Project:         Autonomous RC Truck                                             *
 * Area:            Car Computer (Local on MCU)                                     *
 *                                                                                  *
 * Author(s):       James Guest (35070432)                                          *
 * File:            CTruck.cpp                                                      *
 * Purpose:         To contain the functions associated with the truck              *
 * Description:                                                                     *
 *                                                                                  *
 *                                                                                  *
 ***********************************************************************************/
 
/************************************ INCLUDES *************************************/
#include "CTruck.hpp"
#include "CMemory.hpp"

// Define the static member variables
CServo CTruck::servo;
CMotor CTruck::motor;
CMemory CTruck::memory;
double CTruck::setSpeed = 0.0;
double CTruck::position[2] = {0.0, 0.0};
double CTruck::velocity = 0.0;
double CTruck::heading = 0.0;
double CTruck::rotationRate = 0.0;


/***************************** FUNCTIONS DEFINITIONS *******************************/

/*===========================================================================
 * Function:        CTruck::InitializeTruck
 * Arguments:       none
 * Returns:         none
 * Description:     Default Constructor (Initialize Pins)
 */
void CTruck::InitializeTruck() {
    ESP_LOGI("CTruck::InitializeTruck", "Initializing truck...");
    CSensor::InitializeSensor();
    CTruck::servo.SetServoAngle(0);
    CTruck::setSpeed = 0;
    CTruck::position[0] = 0;
    CTruck::position[1] = 0;
    CTruck::velocity = 0;
    CTruck::heading = 0;
    CTruck::rotationRate =  0;
    CTruck::CalibrateSensor(100);
    ESP_LOGI("CTruck::InitializeTruck", "Truck initialized.");
}

/*===========================================================================
 * Function:        CTruck::CalculateXDelta
 * Arguments:       double deltaTime - time step
 *                  double acceleration - acceleration
 * Returns:         double - X position
 * Description:     Calculate the X position of the truck
 */
double CTruck::CalculateXDelta(double deltaTime, double heading, double acceleration){
    double xDelta = 0;
    if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
        xDelta = (0.5 * acceleration * deltaTime * deltaTime 
                                + CTruck::velocity * deltaTime)
                                    * -sin(heading * M_PI / 180.0);
        xSemaphoreGive(xMutex);
    } else {
        ESP_LOGE("CTruck::CalculateXDelta", "Error in mutex access of velocity.");
    }
    return xDelta;
}

/*===========================================================================
 * Function:        CTruck::CalculateYDelta
 * Arguments:       double deltaTime - time step
 *                  double acceleration - acceleration
 * Returns:         double - Y position
 * Description:     Calculate the Y position of the truck
 */
double CTruck::CalculateYDelta(double deltaTime, double heading, double acceleration){
    double yDelta = 0;
    if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
        yDelta = (0.5 * acceleration * deltaTime * deltaTime 
                                + CTruck::velocity * deltaTime)
                                    * cos(heading * M_PI / 180.0);
        xSemaphoreGive(xMutex);
    } else {
        ESP_LOGE("CTruck::CalculateYDelta", "Error in mutex access of velocity.");
    }
    return yDelta;
}

/*===========================================================================
 * Function:        CTruck::CalculateHeadingDelta
 * Arguments:       double deltaTime - time step
 * Returns:         double - heading
 * Description:     Calculate the heading of the truck
 */
double CTruck::CalculateHeadingDelta(double deltaTime){
    double headingDelta = 0;
    if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
        headingDelta = CTruck::rotationRate * deltaTime;
        xSemaphoreGive(xMutex);
    } else {
        ESP_LOGE("CTruck::CalculateHeadingDelta", "Error in mutex access of rotation rate.");
    }
    return headingDelta;
}

/*===========================================================================
 * Function:        CTruck::CalculateEulerNorm
 * Arguments:       double deltaX - change in X position
 *                  double deltaY - change in Y position
 * Returns:         double - Euclidean norm
 * Description:     Calculate the Euclidean norm of a 2D vector
 */
double CTruck::CalculateEulerNorm(double deltaX, double deltaY){
    double norm = sqrt(pow(deltaX, 2) + pow(deltaY, 2));
    return norm;
}

/*===========================================================================
 * Function:        CTruck::EvaluateLinearFunction
 * Arguments:       double m - slope
 *                  double b - y-intercept
 *                  double w - x shift value
 *                  double x - x value
 * Returns:         double - y value
 * Description:     Evaluate a linear function
 */
double CTruck::EvaluateLinearFunction(double m, double b, double w, double x){
    double y = m * (x + w) + b;
    return y;
}

/*===========================================================================
 * Function:        CTruck::PredictFuturePosition
 * Arguments:       double deltaTime - time step
 * Returns:         std::array<double, 2> - future position
 * Description:     Predict the future position of the truck as a 1st order 
 *                  approximation using the current velocity, rotation rate,
 *                  and position, for a given time step.
 */
std::array<double, 2> CTruck::PredictFuturePosition(double predictionTime, double timeResolution){
    std::array<double, 2> futurePosition;
    double futureHeading;
    if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
        futurePosition = {CTruck::position[0], CTruck::position[1]};
        futureHeading = CTruck::heading;
        xSemaphoreGive(xMutex);
    }
    for (double t = 0; t < predictionTime; t += timeResolution) {
        futurePosition[0] += CTruck::CalculateXDelta(timeResolution, futureHeading);
        futurePosition[1] += CTruck::CalculateYDelta(timeResolution, futureHeading);
        futureHeading += 0; //CTruck::CalculateHeadingDelta(timeResolution);
    }
    ESP_LOGI("CTruck::PredictFuturePosition", "Future position predicted to be X: %f, Y: %f", futurePosition[0], futurePosition[1]);
    return futurePosition;
}

/*===========================================================================
 * Function:        CTruck::CalibrateSensor
 * Arguments:       int sampleSize - number of samples to take for calibration
 * Returns:         none
 * Description:     Calibrate the sensor
 */
void CTruck::CalibrateSensor(int sampleSize){
    CSensor::CalibrateAccel(sampleSize);
    CSensor::CalibrateGyro(sampleSize);
}

/*===========================================================================
 * Function:        CTruck::FollowPath
 * Arguments:       none
 * Returns:         none
 * Description:     Continuously follow the path until the end of the path is 
 *                  reached.
 */
void CTruck::FollowPath(void *pvParameters){
    double steeringWaitTimeInMilliseconds = 20;
    double timeResolution = 0.01;
    double predictionTime = 0.5;
    double pathRadius = 0.03;
    bool atEndOfPath = false;
    std::array<double, 2> futurePosition;
    double pointRadius = 0.125;
    int pointIndex = 1;
    std::array<double,3> previousPoint = CTruck::memory.ReadPath(0);
    std::array<double,3> nextPoint = CTruck::memory.ReadPath(1);
    double slope, lineOffset, intercept;
    if ((nextPoint[0] - previousPoint[0]) == 0) {
        slope = std::numeric_limits<double>::infinity();
        lineOffset = pathRadius;
        intercept = 0;
    } else {
        slope = (nextPoint[1] - previousPoint[1]) / (nextPoint[0] - previousPoint[0]);
        if (slope != 0) {
            lineOffset = pathRadius / slope * sqrt(1 + pow(slope, 2));
        } else {
            lineOffset = pathRadius;
        }
        intercept = previousPoint[1] - previousPoint[0] * slope;
    }
    CTruck::setSpeed = CTruck::motor.SetMotorSpeed(nextPoint[2]);
    double upperBound, lowerBound;
    double position[2];

    double integrator = 0;
    double proportional = 0;
    double P = 1.6;
    double I = 0.5;
    
    int64_t lastTime = esp_timer_get_time();
    int64_t currentTime;  
    bool onCourse = true;
    ESP_LOGI("CTruck::FollowPath", "Beginning Path Follow Loop");

    while (!atEndOfPath) {
        currentTime = esp_timer_get_time();
        ESP_LOGV("CTruck::FollowPath", "Control Loop Time: %.4f seconds", (currentTime - lastTime) / 1000000.0);
        lastTime = currentTime;
        ESP_LOGV("CTruck::FollowPath", "Querying Position...");
        if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
            position[0] = CTruck::position[0];
            position[1] = CTruck::position[1];
            xSemaphoreGive(xMutex);
        }
        ESP_LOGV("CTruck::FollowPath", "Position Acquired: X{%5.2f}, Y{%5.2f}.", position[0], position[1]);
        //Check if the truck is at the next point
        if (CalculateEulerNorm(position[0] - nextPoint[0], position[1] - nextPoint[1]) < pointRadius) {
            ESP_LOGI("CTruck::FollowPath", "Waypoint %d achieved at X{%5.2f}, Y{%5.2f}.", 
                                    pointIndex, position[0], position[1]);
            previousPoint = nextPoint;
            pointIndex++;
            nextPoint = CTruck::memory.ReadPath(pointIndex);
            if ((nextPoint[0] - previousPoint[0]) == 0) {
                slope = std::numeric_limits<double>::infinity();
                intercept = 0;
                lineOffset = pathRadius;
            } else {
                slope = (nextPoint[1] - previousPoint[1]) / (nextPoint[0] - previousPoint[0]);
                intercept = previousPoint[1] - previousPoint[0] * slope;
                if (slope != 0) {
                    lineOffset = pathRadius / slope * sqrt(1 + pow(slope, 2));
                } else {
                    lineOffset = pathRadius;
                }
            }
            CTruck::setSpeed = CTruck::motor.SetMotorSpeed(nextPoint[2]);
        }

        //Check if the truck is at the end of the path
        if (pointIndex == CTruck::memory.NumberOfPointsInPath()) {
            atEndOfPath = true;
            ESP_LOGI("CTruck::FollowPath", "End of path reached.");
            break;
        }

        // Predict the future position of the truck
        futurePosition = CTruck::PredictFuturePosition(predictionTime, timeResolution);

        // Check if future position is on the path
        if (slope == std::numeric_limits<double>::infinity()) { // change in x is 0 therefore compare x position of prediction
            if ((futurePosition[0] < previousPoint[0] + lineOffset) 
                && (futurePosition[0] > previousPoint[0] - lineOffset)) {
                ESP_LOGV("CTruck::FollowPath", "Future position is on the path.");
                if (!onCourse) {
                    ESP_LOGV("CTruck::FollowPath", "Returning to course.");
                    CTruck::servo.invertServoAngle();
                    onCourse = true;
                }
                proportional = 0;
                integrator = 0;
                continue;
            } else if (futurePosition[0] >= previousPoint[0] + lineOffset) {
                onCourse = false;
                ESP_LOGV("CTruck::FollowPath", "Future position is left of the path, turning right.");
                proportional = abs(futurePosition[0] - previousPoint[0]);
                CTruck::servo.incrementServoAngle(I * integrator + P * proportional);
                vTaskDelay(steeringWaitTimeInMilliseconds / portTICK_PERIOD_MS);
                integrator += 0.01;
            } else if (futurePosition[0] <= previousPoint[0] - lineOffset) {
                onCourse = false;
                ESP_LOGV("CTruck::FollowPath", "Future position is right of the path, turning left.");
                proportional = abs(previousPoint[0] - futurePosition[0]);
                CTruck::servo.incrementServoAngle(-I * integrator - P * proportional);
                vTaskDelay(steeringWaitTimeInMilliseconds / portTICK_PERIOD_MS);
                integrator += 0.01;
            } else {
                ESP_LOGW("CTruck::FollowPath", "Error in path following.");
            }
        } else if (slope == 0){ // change in y is zero so compare y position of prediction
            if ((futurePosition[1] < previousPoint[1] + lineOffset) 
                && (futurePosition[1] > previousPoint[1] - lineOffset)) {
                ESP_LOGV("CTruck::FollowPath", "Future position is on the path.");
                if (!onCourse) {
                    ESP_LOGV("CTruck::FollowPath", "Returning to course.");
                    CTruck::servo.invertServoAngle();
                    onCourse = true;
                }
                proportional = 0;
                integrator = 0;
                continue;
            } else if (futurePosition[1] >= previousPoint[1] + lineOffset) {
                onCourse = false;
                ESP_LOGV("CTruck::FollowPath", "Future position is left of the path, turning right.");
                proportional = abs(futurePosition[1] - previousPoint[1]);
                CTruck::servo.incrementServoAngle(I * integrator + P * proportional);
                integrator += 0.01;
                vTaskDelay(steeringWaitTimeInMilliseconds / portTICK_PERIOD_MS);
            } else if (futurePosition[1] <= previousPoint[1] - lineOffset) {
                onCourse = false;
                ESP_LOGV("CTruck::FollowPath", "Future position is right of the path, turning left.");
                proportional = abs(previousPoint[1] - futurePosition[1]);
                CTruck::servo.incrementServoAngle(- I * integrator - P * proportional);
                integrator += 0.01;
                vTaskDelay(steeringWaitTimeInMilliseconds / portTICK_PERIOD_MS);
            } else {
                ESP_LOGW("CTruck::FollowPath", "Error in path following.");
            }
        } else {
            upperBound = EvaluateLinearFunction(slope, intercept, lineOffset, futurePosition[0]);
            lowerBound = EvaluateLinearFunction(slope, intercept, -lineOffset, futurePosition[0]);
            if ((futurePosition[1] < upperBound) 
                && (futurePosition[1] > lowerBound)) {
                ESP_LOGV("CTruck::FollowPath", "Future position is on the path.");
                if (!onCourse) {
                    ESP_LOGV("CTruck::FollowPath", "Returning to course.");
                    CTruck::servo.invertServoAngle();
                    onCourse = true;
                }
                proportional = 0;
                integrator = 0;
                continue;
            } else if (futurePosition[1] >= upperBound) {
                onCourse = false;
                ESP_LOGV("CTruck::FollowPath", "Future position is left of the path, turning right.");
                proportional = abs(futurePosition[1] - (upperBound + lowerBound) / 2);
                CTruck::servo.incrementServoAngle(-I * integrator - P * proportional);
                integrator += 0.01;
                vTaskDelay(steeringWaitTimeInMilliseconds / portTICK_PERIOD_MS);
            } else if (futurePosition[1] <= lowerBound) {
                onCourse = false;
                ESP_LOGV("CTruck::FollowPath", "Future position is right of the path, turning left.");
                proportional = abs((upperBound + lowerBound) / 2 - futurePosition[1]);
                CTruck::servo.incrementServoAngle(I * integrator + P * proportional);
                integrator += 0.01;
                vTaskDelay(steeringWaitTimeInMilliseconds / portTICK_PERIOD_MS);
            } else {
                ESP_LOGW("CTruck::FollowPath", "Error in path following.");
            }
        }

    }
}

/*===========================================================================
    * Function:        CTruck::UpdateStates
    * Arguments:       none
    * Returns:         none
    * Description:     Continuously update the states of the truck.
    */
void CTruck::UpdateStates(void *pvParameters){
    double deltaTime;
    std::array<double, 3> accelReadings;
    std::array<double, 3> gyroReadings;
    std::array<double, 3> filteredAccelReadings = {0, 0, 0};
    std::array<double, 3> filteredGyroReadings = {0, 0, 0};
    #define movingAverageSize 5
    std::array<std::array<double, movingAverageSize>, 3> accelHistory = {};
    std::array<std::array<double, movingAverageSize>, 3> gyroHistory = {};  
    int index = 0;  // To track the current index in the history buffer  
    double accelerometerThreshold = 0.075;
    double gyroThreshold = 2;
    double velocityThreshold = 0.01;
    int64_t lastTime = esp_timer_get_time();
    int64_t currentTime;   
    int memoryLogCount = 0; int logEveryXthSample = 10;
    double xPosition = 0, yPosition = 0, velocity = 0, heading = 0, rotationRate = 0;
    
    ESP_LOGI("CTruck::UpdateStates", "Beginning State Update Loop");

    while (true) {
        ESP_LOGV("CTruck::UpdateStates", "Reading Sensors...");
        currentTime = esp_timer_get_time();
        accelReadings = CSensor::ReadSensor(ACCELEROMETER);
        gyroReadings = CSensor::ReadSensor(GYROSCOPE);
        deltaTime = (currentTime - lastTime) / 1000000.0;

        // Update history with the current readings for each axis
        for (int i = 0; i < 3; i++) {
            accelHistory[i][index] = accelReadings[i];
            gyroHistory[i][index] = gyroReadings[i];    
        }
        index = (index + 1) % movingAverageSize;

        // Compute the moving average (low-pass filter) for each axis
        for (int i = 0; i < 3; i++) {
            filteredAccelReadings[i] = std::accumulate(accelHistory[i].begin(), accelHistory[i].end(), 0.0) / movingAverageSize;
            filteredGyroReadings[i] = std::accumulate(gyroHistory[i].begin(), gyroHistory[i].end(), 0.0) / movingAverageSize;
            if (filteredAccelReadings[i] < accelerometerThreshold && filteredAccelReadings[i] > -accelerometerThreshold) {
                filteredAccelReadings[i] = 0;
            }
            if (filteredGyroReadings[i] < gyroThreshold && filteredGyroReadings[i] > -gyroThreshold) {
                filteredGyroReadings[i] = 0;
            }
        }

        velocity += filteredAccelReadings[1] * deltaTime;
        if (velocity < velocityThreshold && velocity > -velocityThreshold) {
                velocity = 0;
        }
        xPosition += CTruck::CalculateXDelta(deltaTime, heading, filteredAccelReadings[1]);
        yPosition += CTruck::CalculateYDelta(deltaTime, heading, filteredAccelReadings[1]);
        rotationRate = filteredGyroReadings[2];
        heading += CTruck::CalculateHeadingDelta(deltaTime);
        
        if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
            if (memoryLogCount == 0) {
                CTruck::memory.LogData(xPosition, yPosition, CTruck::setSpeed);
            }
            CTruck::position[0] = xPosition;
            CTruck::position[1] = yPosition;
            CTruck::velocity = velocity;
            CTruck::heading = heading;
            CTruck::rotationRate = rotationRate;

            // printf("\r  X {%7.3f}, Y {%7.3f} -|- "
            //             "Heading {%7.2f} -|- V {%7.3f} -|-"
            //             " Omega {%7.3f} -|- A {%7.3f} -|- T {%7.3f}", 
            //             CTruck::position[0], CTruck::position[1], 
            //             CTruck::heading, CTruck::velocity, CTruck::rotationRate,
            //             filteredAccelReadings[1], currentTime / 1000000.0);
            xSemaphoreGive(xMutex);
        }
        memoryLogCount = (memoryLogCount + 1) % logEveryXthSample;
        lastTime = currentTime;
    }

    }