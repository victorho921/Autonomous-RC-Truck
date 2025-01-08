/************************************************************************************
 * Project:         Autonomous RC Truck                                             *
 * Area:            Car Computer (Local on MCU)                                     *
 *                                                                                  *
 * Author(s):       James Guest (35070432)                                          *
 * File:            CMemory.cpp                                                     *
 * Purpose:         To contain the functions associated with reading/writing nvs    *
 ***********************************************************************************/
 
/************************************ INCLUDES *************************************/
#include "CMemory.hpp"

/***************************** FUNCTIONS DEFINITIONS *******************************/

/*===========================================================================
 * Function:        CMemory::CMemory
 * Arguments:       none
 * Returns:         none
 * Description:     Default Constructor (Initialize Partition Addresses)
 */
CMemory::CMemory() {
    // Configure direction pins as GPIO outputs
    ESP_LOGI("CMemory", "Initializing partition addresses...");
    assert(this->pathData != NULL);
    assert(this->sensorData != NULL);
    ESP_LOGI("CMemory", "Partition addresses initialized.");
    ESP_ERROR_CHECK(esp_partition_erase_range(pathData, 0, pathData->size));
    ESP_ERROR_CHECK(esp_partition_erase_range(sensorData, 0, sensorData->size));  
    ESP_LOGI("CMemory", "Partitions erased.");
}

/*===========================================================================
 * Function:        CMemory::WritePath
 * Arguments:       array<double,3> &PointData - array of data to be write
 * Returns:         none
 * Description:     Write the path to the pathData partition
 */

void CMemory::WritePath(std::array<double,3> &PointData){
    ESP_LOGI("CMemory", "Writing path to memory...");
    // Find the pathData partition 
    if (pathData == NULL) {
        ESP_LOGE("CMemory", "pathData Partition not found!");
        return;
    }
    // Ensure there is enough space
    if (numberOfPointsInPath * sizeof(PointData) > partition->size) {
        ESP_LOGE("CMemory", "Not enough space in partition to write data!");
        return;
    }
    // Write all data sequentially
    esp_err_t err = esp_partition_write(pathData, write_offset, PointData.data(), sizeof(pathData));
    if (err != ESP_OK) {
        ESP_LOGE("CMemory", "Failed to write data: %s", esp_err_to_name(err));
        return;
    }
    // Updata offset for writing the next PointData
    ESP_LOGI("CMemory", "Wrote %zu data records", sizeof(PointData));
    write_offset += sizeof(PointData);
}

/*===========================================================================
 * Function:        CMemory::ReadPath
 * Arguments:       none 
 * Returns:         array<double,3> PointData
 * Description:     Read the path from the pathData partition
 */

std::array<double,3> CMemory::ReadPath(){
    ESP_LOGI("CMemory", "Reading pathpoint from memory...");

    // Initialize placeholder for array
    std::array<double,3> PointData = {0,0,0};
    // Find the pathData partition 
    if (pathData == NULL) {
        ESP_LOGE("CMemory", "pathData Partition not found!");
        return;
    }
    // Check if there is data to read
    if (read_offset >= write_offset) {
        ESP_LOGW("CMemory", "No more data to read!");
        return data;
    }
    // Read the next data record
    esp_err_t err = esp_partition_read(pathData, read_offset, PointData.data(), sizeof(double)*3);
    if (err != ESP_OK) {
        ESP_LOGE("CMemory", "Failed to read data: %s", esp_err_to_name(err));
        return data;
    }
    // Output the data
    ESP_LOGI("CMemory", "Data: x=%f, y=%f, speed=%f", data[0], data[1], data[2]);
    // Update the read offset
    read_offset += sizeof(double)*3;
    return PointData;
}

/*===========================================================================
 * Function:        CMemory::LogData
 * Arguments:       double x - x position
 *                  double y - y position
 *                  double speed - speed of the truck
 * Returns:         none
 * Description:     Write one point of the state data to the sensorData partition
 */
void CMemory::LogData(double x, double y, double speed){
    ESP_LOGI("CMemory", "Logging datapoint X{%7.3f}, Y{%7.3f}, Speed{%7.3f}", x, y, speed);
    printf("%.3f, %.3f\n", x, y);
    double writeValues[3] = {x, y, speed};
    size_t writeSize = sizeof(writeValues);
    size_t offset = (this->loggedPointsInMemory) * writeSize;
    ESP_ERROR_CHECK(esp_partition_write(this->sensorData, offset, writeValues, writeSize));
    this->loggedPointsInMemory++;
    ESP_LOGD("CMemory", "Data logged.");
}

/*===========================================================================
 * Function:        CMemory::ReadData
 * Arguments:       none
 * Returns:         none
 * Description:     Read all of the state data to the sensorData partition
 */
void CMemory::ReadData(){
    double loggedData[3][this->loggedPointsInMemory];
    double readValues[3];
    size_t readSize = sizeof(readValues);
    size_t offset = 0;
    for (int i = 0; i < this->loggedPointsInMemory+1; i++) {
        ESP_ERROR_CHECK(esp_partition_read(this->sensorData, offset, readValues, readSize));
        loggedData[0][i] = readValues[0];
        loggedData[1][i] = readValues[1];
        loggedData[2][i] = readValues[2];
        offset += readSize;
    }

    for (int i = 0; i < loggedPointsInMemory; i++) {
        ESP_LOGI("CMemory", "Data read: X{%7.3f}, Y{%7.3f}, Speed{%7.3f}", loggedData[0][i], loggedData[1][i], loggedData[2][i]);
    }
}

