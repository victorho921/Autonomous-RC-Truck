#include "PathDataHandler.h"


#define PARTITION_NAME "pathData"

namespace NVS {

PathDataHandler::PathDataHandler(size_t write_offset, size_t read_offset){
    this->write_offset = write_offset;
    this->read_offset = read_offset;
}

// Write all data sequentially to the partition
void PathDataHandler::WriteAllData(const MovementData *data, size_t count) {
    // Find the partition by name
    const esp_partition_t *partition = esp_partition_find_first(
        ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, PARTITION_NAME);

    if (partition == NULL) {
        ESP_LOGE("PARTITION", "Partition not found!");
        return;
    }

    // Ensure there is enough space
    if (count * sizeof(MovementData) > partition->size) {
        ESP_LOGE("PARTITION", "Not enough space in partition to write data!");
        return;
    }

    // Write all data sequentially
    esp_err_t err = esp_partition_write(partition, write_offset, data, count * sizeof(MovementData));
    if (err != ESP_OK) {
        ESP_LOGE("PARTITION", "Failed to write data: %s", esp_err_to_name(err));
        return;
    }

    ESP_LOGI("PARTITION", "Wrote %zu data records", count);
    write_offset += count * sizeof(MovementData);
}

// Read the next data record from the partition
bool PathDataHandler::ReadNextData(MovementData *data) {
    const esp_partition_t *partition = esp_partition_find_first(
        ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, PARTITION_NAME);

    if (partition == NULL) {
        ESP_LOGE("PARTITION", "Partition not found!");
        return false;
    }

    // Check if there is data to read
    if (read_offset >= write_offset) {
        ESP_LOGW("PARTITION", "No more data to read!");
        return false;
    }

    // Read the next data record
    esp_err_t err = esp_partition_read(partition, read_offset, data, sizeof(MovementData));
    if (err != ESP_OK) {
        ESP_LOGE("PARTITION", "Failed to read data: %s", esp_err_to_name(err));
        return false;
    }

    ESP_LOGI("PARTITION", "Data: x=%f, y=%f, speed=%f, timestamp=%f", data->x, data->y, data->speed, data->timestamp);

    // Update the read offset
    read_offset += sizeof(MovementData);
    return true;
}

void PathDataHandler::ErasePartition() {
    // Find the partition by name
    const esp_partition_t *partition = esp_partition_find_first(
        ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, PARTITION_NAME);

    if (partition == NULL) {
        ESP_LOGE("PARTITION", "Partition not found!");
        return;
    }

    // Erase the partition by specifying the offset and size of the partition
    esp_err_t err = esp_partition_erase_range(partition, 0, partition->size);
    if (err != ESP_OK) {
        ESP_LOGE("PARTITION", "Failed to erase partition: %s", esp_err_to_name(err));
        return;
    }

    ESP_LOGI("PARTITION", "Partition '%s' erased successfully!", PARTITION_NAME);
}

} // namespace Canvas
