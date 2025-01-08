#ifndef PathDataHandler_H
#define PathDataHandler_H

extern "C"
{
#include "esp_partition.h"
#include "esp_log.h"
}


// Namespace for Canvas
namespace NVS {

// Structure to hold movement data
struct MovementData 
{
    double x;
    double y;
    double speed;
    double timestamp;
};

// Class to handle reading and writing of movement data
class PathDataHandler 
{
    private:
        size_t write_offset;  // Tracks the current write position
        size_t read_offset;   // Tracks the current read position

    public:
        // Constructor to initialize offsets
        PathDataHandler(size_t write_offset = 0, size_t read_offset = 0);

        // Function to write multiple data records to the partition
        void WriteAllData(const MovementData *data, size_t count);

        // Function to read the next data record from the partition
        bool ReadNextData(MovementData *data);

        void ErasePartition();

};

} 

#endif 
