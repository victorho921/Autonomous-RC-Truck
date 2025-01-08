/************************************************************************************
 * Project:         Autonomous RC Truck                                             *
 *                                                                                  *
 * Author(s):       Victor Yeung (59967653)                                         *
 * File:            TcpClient.cpp                                                   *
 * Purpose:         To contain function associated with the TCP Client              *
 * Description:                                                                     *
 *                                                                                  *
 *                                                                                  *
 ***********************************************************************************/

/************************************ INCLUDES *************************************/
#include "TcpClient.h"

/********************************** NAMESPACE **************************************/
namespace TruckComms 
{
/*===========================================================================
 * Function:        TcpClient::TcpClient
 * Arguments:       IP address(char), port(int)
 * Returns:         none
 * Description:     Constructor (Initialize TCP Client)
 */
TcpClient::TcpClient(char* &Host_IP, int Port)
{
    host_ip = Host_IP; 
    port = Port; 
    sock = -1; 
    addr_family = AF_INET; 
    ip_protocol = IPPROTO_IP; 

    // Set up the destination address structure
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(Port);

    // Convert the host IP string to binary form
    inet_pton(AF_INET, Host_IP, &dest_addr.sin_addr);
}


/*===========================================================================
 * Function:        TcpClient::~TcpClient
 * Arguments:       none
 * Returns:         none
 * Description:     Destructor (Close the socket)
 */
TcpClient::~TcpClient() 
{
    CloseSocket();
}

/*===========================================================================
 * Function:        TcpClient::CreateSocket
 * Arguments:       none
 * Returns:         true on success, false on fail
 * Description:     Create socket stream for IPv4 
 */
void TcpClient::CreateSocket() 
{
    sock = socket(addr_family, SOCK_STREAM, ip_protocol);
    if (sock < 0) {
        ESP_LOGE("TcpClient","Unable to create socket");
    }
    ESP_LOGI("TcpClient", "Socket created");
}

/*===========================================================================
 * Function:        TcpClient::ConnectToServer
 * Arguments:       none
 * Returns:         none
 * Description:     Connect to the server with limited tries 
 */
void TcpClient::ConnectToServer() 
{
    // Define number of maximum try 
    int MAX_RETRIES = 10;
    for (int retries = 0; retries < MAX_RETRIES; ++retries) 
    {
        // Use connect to the server according with the ip address
        if (connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) == 0) 
        {
            ESP_LOGI("TcpClient", "Successfully connected on attempt %d", retries + 1);
        }
        ESP_LOGE("TcpClient", "Connection attempt %d failed: errno %d", retries + 1);
        // Wait 5 second before retrying
        vTaskDelay(pdMS_TO_TICKS(5000));  
    }  
}

/*===========================================================================
 * Function:        TcpClient::SendData
 * Arguments:       data (double)
 * Returns:         none
 * Description:     Send the data through the socket 
 */
void TcpClient::SendData(double data) 
{
    // Use send function to send data, it return number of bytes it sent
    int err = send(sock, &data, sizeof(data), 0);
    // Possible errors 
    if (err < 0) 
    {
        ESP_LOGE("TcpClient","Error sending data");
    }
    else if (err < sizeof(data))
    {
        ESP_LOGE("TcpClient","Only part of the data was sent");
    }
    ESP_LOGI("TcpClient", "Sent data: %.2f", data);
}
/*===========================================================================
 * Function:        TcpClient::SendDataSet
 * Arguments:       none
 * Returns:         true on success, false on fail
 * Description:     Connect to the server with limited tries 
 */
void TcpClient::SendDataSet(const std::array<double,3> &DataSet){
    // Send each element of the array
    for (int i = 0; i < DataSet.size(); i++) {
        SendData(DataSet.at(i));
    }
}

/*===========================================================================
 * Function:        TcpClient::ReceiveData
 * Arguments:       double &received_data
 * Returns:         none
 * Description:     Receives data from the server
 */
void TcpClient::ReceiveData(double &received_data) {
    // Use recv function to receive data from server, it returns number of bytes it receive
    int len = recv(sock, rx_buffer, sizeof(double), 0);
    // Check if the receive operation was successful
    if (len < 0) {
        ESP_LOGE("TcpClient", "Failed to receive");
    }
    else if (len == sizeof(double)) 
    {
        // Copy received data into received_data variable
        memcpy(&received_data, rx_buffer, sizeof(double));
        ESP_LOGI("TcpClient", "Received data: %.2f", received_data);
    } 
    else 
    {
        ESP_LOGE("TcpClient", "Unexpected data size received");
    }
}

/*===========================================================================
 * Function:        TcpClient::ReceiveDataSet
 * Arguments:       none
 * Returns:         an double array that store all 3 parameters (array<double,3>) 
 * Description:     Receive a set of Data
 */
std::array<double,3> TcpClient::ReceiveDataSet() 
{   
    // Placeholder for arguement for ReceivedData function 
    double received_data[3] = {0, 0, 0};
    // Create double array
    std::array<double,3> PointArray = {0,0,0};
    // Iterate 3 times to receive 3 data points
    for (int i = 0; i < 3; ++i) 
    {
        if (!ReceiveData(received_data[i])) 
        {
            ESP_LOGE("TcpClient", "Failed to receive data at index %d", i);
            // Return an array with all zeros to indicate failure
            return {0,0,0};
        }
    }
    // Log the received array
    ESP_LOGI("TcpClient", "Received array: [%.2f, %.2f, %.2f]", received_data[0], received_data[1], received_data[2]);
    // Set the received data to the PointArray 
    std::copy(received_data, received_data + 3, PointArray.begin());
    return PointArray;
}

/*===========================================================================
 * Function:        TcpClient::CloseSocket
 * Arguments:       none
 * Returns:         none
 * Description:     Closes the socket
 */
void TcpClient::CloseSocket() 
{
    // Check if there is any existing socket
    if (sock != -1) {
        ESP_LOGI("TcpClient", "Closing socket");
        // Shutdown the socket
        shutdown(sock, 0);
        // Close the socket
        close(sock);
        // Reset socket variable
        sock = -1;
    }
    else 
    {
        ESP_LOGI("TcpClient", "Socket is already closed");
    }
}
} 
