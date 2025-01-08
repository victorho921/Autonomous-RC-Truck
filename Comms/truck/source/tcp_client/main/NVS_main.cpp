extern "C"
{
#include "nvs_flash.h"
#include "esp_partition.h"
#include "esp_log.h"
}
#include "PathDataHandler.h"

// #define PARTITION_NAME "pathData"  // Name of your custom partition in the partition table
// #define STORAGE_SIZE 1024         // Total storage size in bytes

// static size_t write_offset = 0;  // Tracks the current write position
// static size_t read_offset = 0;   // Tracks the current read position

// typedef struct {
//     double x;
//     double y;
//     double speed;
//     double timestamp;
// } MovementData;

// void write_all_data(const MovementData *data, size_t count) {
//     const esp_partition_t *partition = esp_partition_find_first(
//         ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, PARTITION_NAME);

//     if (partition == NULL) {
//         ESP_LOGE("PARTITION", "Partition not found!");
//         return;
//     }

//     // Ensure there is enough space
//     if (count * sizeof(MovementData) > partition->size) {
//         ESP_LOGE("PARTITION", "Not enough space in partition to write data!");
//         return;
//     }

//     // Write all data sequentially
//     esp_err_t err = esp_partition_write(partition, write_offset, data, count * sizeof(MovementData));
//     if (err != ESP_OK) {
//         ESP_LOGE("PARTITION", "Failed to write data: %s", esp_err_to_name(err));
//         return;
//     }

//     ESP_LOGI("PARTITION", "Wrote %zu data records", count);
//     write_offset += count * sizeof(MovementData);
// }



// bool read_next_data(MovementData *data) {
//     const esp_partition_t *partition = esp_partition_find_first(
//         ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, PARTITION_NAME);

//     if (partition == NULL) {
//         ESP_LOGE("PARTITION", "Partition not found!");
//         return false;
//     }

//     // Check if there is data to read
//     if (read_offset >= write_offset) {
//         ESP_LOGW("PARTITION", "No more data to read!");
//         return false;
//     }

//     // Read the next data record
//     esp_err_t err = esp_partition_read(partition, read_offset, data, sizeof(MovementData));
//     if (err != ESP_OK) {
//         ESP_LOGE("PARTITION", "Failed to read data: %s", esp_err_to_name(err));
//         return false;
//     }
//     ESP_LOGI("Partition: ", "Data:%f", data->timestamp);

//     //ESP_LOGI("PARTITION", "Read data at offset %zu: x=%.2f, y=%.2f, speed=%.2f, timestamp=%u",
//              //read_offset, data->x, data->y, data->speed, data->timestamp);

//     // Update the read offset
//     read_offset += sizeof(MovementData);
//     return true;
// }




extern "C" void app_main(void) {
    // Initialize NVS for storing offsets (optional)
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    // Simulate receiving data and writing it
    NVS::MovementData data[] = {
        {1.0, 1.0, 5.0, 1000},
        {2.0, 2.0, 10.0, 2000},
        {3.0, 3.0, 15.0, 3000},
    };

    NVS::PathDataHandler path;
    path.ErasePartition();
    path.WriteAllData(data,3);
    path.ReadNextData(data);
    path.ReadNextData(data);
    path.ReadNextData(data);
    // write_all_data(data, sizeof(data) / sizeof(data[0]));
    // read_next_data(data);
    // read_next_data(data);
    // read_next_data(data);
}




