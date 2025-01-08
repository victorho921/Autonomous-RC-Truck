//  // Find the partition map in the partition table
//   const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");
//   assert(partition != NULL);

//   static char store_data[] = “EXAMPLE”;
//   static char read_data[sizeof(store_data)];

//   // Erase entire partition
//   memset(read_data, 0xFF, sizeof(read_data));
//   ESP_ERROR_CHECK(esp_partition_erase_range(partition, 0, partition->size));

//   // Write the data, starting from the beginning of the partition
//   ESP_ERROR_CHECK(esp_partition_write(partition, 0, store_data, sizeof(store_data)));
//   ESP_LOGI(TAG, "Written data: %s", store_data);

//   // Read back the data, checking that read data and written data match
//   ESP_ERROR_CHECK(esp_partition_read(partition, 0, read_data, sizeof(read_data)));
//   ESP_LOGI(TAG, "Read data: %s", read_data);
//   assert(memcmp(store_data, read_data, sizeof(read_data)) == 0);

//   // Erase the area where the data was written. Erase size should be a multiple of SPI_FLASH_SEC_SIZE
//   // and also be SPI_FLASH_SEC_SIZE aligned
//   ESP_ERROR_CHECK(esp_partition_erase_range(partition, 0, SPI_FLASH_SEC_SIZE));

//   // // Read back the data (should all now be 0xFF's)
//   memset(store_data, 0xFF, sizeof(read_data));
//   ESP_ERROR_CHECK(esp_partition_read(partition, 0, read_data, sizeof(read_data)));
//   assert(memcmp(store_data, read_data, sizeof(read_data)) == 0);

//   ESP_LOGI(TAG, "Erased data");

//   ESP_LOGI(TAG, "Example end");