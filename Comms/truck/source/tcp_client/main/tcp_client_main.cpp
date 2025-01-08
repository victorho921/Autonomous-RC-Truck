extern "C"
{
#include "nvs_flash.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "esp_event.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
}
#include "TcpClient.h"


extern "C" void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // example_connect is a helper function to connect wifi
    // a custom wifi code maybe needed
    // https://developer.espressif.com/blog/getting-started-with-wifi-on-esp-idf/ 
    ESP_ERROR_CHECK(example_connect());

    // define IP address of the server
    std::string IPAddress = "192.168.173.105";
    // Convert string to char*
    char *IP = IPAddress.data();

    TruckComms::TcpClient client(IP, 3333);
    if (!client.CreateSocket()) 
    {
        return;
    }
    if (client.ConnectToServer() == false) 
    {
        return;
    }
    // while (1)
    // {
    // double dataToSend = 42.0;
    // if (!client.SendData(dataToSend)) 
    // {
    //     return;
    // }
    // vTaskDelay(1000);
    // }
    //std::array<double,3> ReceivedData = {0,0,0};
    //ReceivedData = client.ReceiveSetData();

    std::array<double,3> DataToSend = {1.1,2.2,3.3};
    if (!client.SendSetData(DataToSend)){
        return;
    }


    
}
