/************************************************************************************
 * Project:         Autonomous RC Truck                                             *
 * Area:            Car Computer (Local on MCU)                                     *
 *                                                                                  *
 * Author(s):       James Guest (35070432)                                          *
 * File:            CMemory.hpp                                                     *
 * Purpose:         To contain the functions associated with reading/writing nvs    *
 ***********************************************************************************/

#ifndef C_MEMORY_H
#define C_MEMORY_H

/************************************ INCLUDES *************************************/
//Libraries
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <assert.h>
#include <array>
#include <vector>


extern "C" {
    #include "esp_partition.h"
    #include "spi_flash_mmap.h"
    #include "esp_log.h"
}


/******************************* PIN DEFINITIONS ***********************************/



/****************************** CLASS DEFINITIONS **********************************/

/*===========================================================================
 * Class:           CMemory
 * Description:     Class for managing reading and writing to non-volatile 
 *                  memory using partitions
 */
class CMemory {
    private:
        const esp_partition_t *pathData = esp_partition_find_first(
                                                ESP_PARTITION_TYPE_DATA, 
                                                ESP_PARTITION_SUBTYPE_ANY, 
                                                "pathData");
        const esp_partition_t *sensorData = esp_partition_find_first(
                                                ESP_PARTITION_TYPE_DATA, 
                                                ESP_PARTITION_SUBTYPE_ANY, 
                                                "sensorData");
        int loggedPointsInMemory = 0;
        int numberOfPointsInPath = 0;
        size_t write_offset = 0;
        size_t read_offset = 0;
    public:
        //Default Constructor (Initialize Partition Addresses)
        CMemory();
        // Write path points sequentially to the pathData partition
        void WritePath(std::array<double,3> &PointData);
        // Read path points from the pathData partition, at a given index and number of points
        std::array<double,3> ReadPath(int pointIndex);

        // Log the state data to the sensorData partition, after the previously logged point
        void LogData(double x, double y, double speed);
        // Read the state data from the sensorData partition
        void ReadData();
        int NumberOfPointsInPath(){
            return this->numberOfPointsInPath;
        }
        // Receive number of point in path directly from the gui
        void ReceiveNumberOfPoint();
};

#endif // C_MEMORY_H