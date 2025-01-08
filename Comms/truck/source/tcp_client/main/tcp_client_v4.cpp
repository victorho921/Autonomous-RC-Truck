extern "C"
{
#include "sdkconfig.h"
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>            // struct addrinfo
#include <arpa/inet.h>
#include "esp_netif.h"
#include "esp_log.h"
}

// #if defined(CONFIG_EXAMPLE_SOCKET_IP_INPUT_STDIN)
// #include "addr_from_stdin.h"
// #endif

#define HOST_IP_ADDR "192.168.173.105"
// #if defined(CONFIG_EXAMPLE_IPV4)
// // #define HOST_IP_ADDR CONFIG_EXAMPLE_IPV4_ADDR
// // Define the IP address of the computer below
// #define HOST_IP_ADDR "192.168.173.221"
// #elif defined(CONFIG_EXAMPLE_SOCKET_IP_INPUT_STDIN)
// #define HOST_IP_ADDR ""
// #endif

#define PORT 3333

// static const char *TAG = "example";
// static const char *payload = "Message from ESP32 ";


void tcp_client(void)
{
    char rx_buffer[128];
    char host_ip[] = HOST_IP_ADDR;
    int addr_family = 0;
    int ip_protocol = 0;
    double received_float = 0.0;

    // while (1) 
    // {
// #if defined(CONFIG_EXAMPLE_IPV4)
        struct sockaddr_in dest_addr;
        // Convert Ipaddress (host_ip) into binary form and store in (dest_addr.sin_addr)
        // AF_INET mean IPv4
        inet_pton(AF_INET, host_ip, &dest_addr.sin_addr);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(PORT);
        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;

// #elif defined(CONFIG_EXAMPLE_SOCKET_IP_INPUT_STDIN)
//         struct sockaddr_storage dest_addr = { 0 };
//         ESP_ERROR_CHECK(get_addr_from_stdin(PORT, SOCK_STREAM, &ip_protocol, &addr_family, &dest_addr));
// #endif
        
        // // Create socket
        // int sock =  socket(addr_family, SOCK_STREAM, ip_protocol);
        // if (sock < 0) 
        // {
        //     ESP_LOGE("Error: ", "Unable to create socket: errno %d", errno);
        //     break;
        // }
        // ESP_LOGI("Info: ", "Socket created, connecting to %s:%d", host_ip, PORT);
        
        // // Connection
        // int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        // if (err != 0) 
        // {
        //     ESP_LOGE("Error: ", "Socket unable to connect: errno %d", errno);
        //     break;
        // }
        // ESP_LOGI("Info: ", "Successfully connected");

        // // Loop
        // while (1) 
        // {
        //     // Sending Data
        //     float data = 0;
        //     int err = send(sock, &data, sizeof(data), 0);
        //     ESP_LOGI("Info: ", "Sent Data: %f", data);
        //     if (err < 0) {
        //         ESP_LOGE("Error: ", "Error occurred during sending: errno %d", errno);
        //         break;
        //     }

        //     // Receive Data
        //     int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
        //     // Error occurred during receiving
        //     if (len < 0) 
        //     {
        //         ESP_LOGE("Error: ", "recv failed: errno %d", errno);
        //         break;
        //     }
        //     // Recover Data from binary to float
        //     else {
        //         // rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
        //         // ESP_LOGI(TAG, "Received %d bytes from %s:", len, host_ip);
        //         // ESP_LOGI(TAG, "%s", rx_buffer);
        //         if (len == sizeof(float)) 
        //         {
        //             // Copy received bytes into a float variable
        //             memcpy(&received_float, rx_buffer, sizeof(float));

        //             ESP_LOGI("Info: ", "Received float value: %f", received_float);
        //         } else 
        //         {
        //             ESP_LOGE("Error: ", "Unexpected data size received");
        //         }
        //     }
        // }

        // Close the socket
    //     if (sock != -1) 
    //     {
    //         ESP_LOGE("Error: ", "Shutting down socket and restarting...");
    //         shutdown(sock, 0);
    //         close(sock);
    //     }
    //     ESP_LOGI("Info: ","Restart the program");
    //     esp_restart();
    //     // Send -> Close socket -> Send -> Close ....
    // }
}

// Create Socket
int CreateSocket(int addr_family, int ip_protocol, char *host_ip, int PORT)
{
    int sock = socket(addr_family, SOCK_STREAM, ip_protocol);
    if (sock < 0) 
    {
        ESP_LOGE("Error: ", "Unable to create socket: errno %d", errno);
        return 1;
    }
    ESP_LOGI("Info: ", "Socket created, connecting to %s:%d", host_ip, PORT);
    return sock;
}

// Connection
int ConnectToServer(int sock, const struct sockaddr *dest_arr)
{
        // int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        int err = connect(sock, dest_addr, sizeof(dest_addr));
        if (err != 0) 
        {
            ESP_LOGE("Error: ", "Socket unable to connect: errno %d", errno);
            return -1;
        }
        ESP_LOGI("Info: ", "Successfully connected");
        return 0;
}

// // Connection with Multiple Retry
// int ConnectToServerWithRetries(int sock, const struct sockaddr *dest_addr, socklen_t addr_len, int max_retries, int delay_ms) {
//     int retries = 0;
//     while (retries < max_retries) {
//         int err = connect(sock, dest_addr, addr_len);
//         if (err == 0) {
//             ESP_LOGI("ConnectToServer", "Successfully connected on attempt %d", retries + 1);
//             return 0; // Indicate success
//         }

//         ESP_LOGE("ConnectToServer", "Connection attempt %d failed: errno %d", retries + 1, errno);
//         retries++;
//         vTaskDelay(delay_ms / portTICK_PERIOD_MS); // Delay before retrying
//     }

//     ESP_LOGE("ConnectToServer", "All %d connection attempts failed", max_retries);
//     return -1; // Indicate failure after retries
// }

void SendData(double data,int sock)
{
    int err = send(sock, &data, sizeof(data), 0);
    ESP_LOGI("Info: ", "Sent Data: %d", data);
    if (err < 0) 
    {
        ESP_LOGE("Error: ", "Error occurred during sending: errno %d", errno);
        break;
    }
}

double ReceiveData(int sock, char *rx_buffer, double &received_float)
{
    int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
        // Error occurred during receiving
        if (len < 0) 
        {
            ESP_LOGE("Error: ", "recv failed: errno %d", errno);
            return 1;
        }
        // Recover Data from binary to float
        if (len == sizeof(float)) 
        {
            // Copy received bytes into a float variable
            memcpy(&received_float, rx_buffer, sizeof(float));
            ESP_LOGI("Info: ", "Received float value: %f", received_float);
            return received_float;
        } 
        else 
        {
            ESP_LOGE("Error: ", "Unexpected data size received");
            return 1;
        }
        
}

// void ReadDataFromNVS(){}

// void StoreDataToNVS(){}
