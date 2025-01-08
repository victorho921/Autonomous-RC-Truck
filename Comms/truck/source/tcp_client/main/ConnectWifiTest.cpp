#include <stdio.h>
extern "C"
{
#include "esp_log.h"
#include "esp_wifi.h"
#include "freertos/task.h"
}
#include "TcpWifiConnection.h"

#define TAG "main"

// Enter the Wi-Fi credentials here
#define WIFI_SSID "SSID"
#define WIFI_PASSWORD "PASSWORD"

using namespace TruckComms;

void app_main(void)
{
    ESP_LOGI(TAG, "Starting tutorial...");
    // ESP_ERROR_CHECK(Wifi::wifi_init());
    Wifi::Wifi(WIFI_SSID,WIFI_PASSWORD);
    ESP_ERROR_CHECK(Wifi::wifi_init());

    esp_err_t ret = wifi_connect(WIFI_SSID, WIFI_PASSWORD);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to connect to Wi-Fi network");
    }

    wifi_ap_record_t ap_info;
    ret = esp_wifi_sta_get_ap_info(&ap_info);
    if (ret == ESP_ERR_WIFI_CONN) {
        ESP_LOGE(TAG, "Wi-Fi station interface not initialized");
    }
    else if (ret == ESP_ERR_WIFI_NOT_CONNECT) {
        ESP_LOGE(TAG, "Wi-Fi station is not connected");
    } else {
        ESP_LOGI(TAG, "--- Access Point Information ---");
        ESP_LOG_BUFFER_HEX("MAC Address", ap_info.bssid, sizeof(ap_info.bssid));
        ESP_LOG_BUFFER_CHAR("SSID", ap_info.ssid, sizeof(ap_info.ssid));
        ESP_LOGI(TAG, "Primary Channel: %d", ap_info.primary);
        ESP_LOGI(TAG, "RSSI: %d", ap_info.rssi);

        ESP_LOGI(TAG, "Disconnecting in 5 seconds...");
        vTaskDelay(pdMS_TO_TICKS(5000));
    }

    ESP_ERROR_CHECK(wifi_disconnect());

    ESP_ERROR_CHECK(wifi_deinit());

    ESP_LOGI(TAG, "End of tutorial...");
}
