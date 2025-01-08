
/************************************************************************************
 * Project:         Autonomous RC Truck                                             *
 *                                                                                  *
 * Author(s):       Victor Yeung (59967653)                                         *
 * File:            TcpClient.h                                                     *
 * Purpose:         To contain function associated with the TCP Client              *
 * Description:                                                                     *
 *                                                                                  *
 *                                                                                  *
 ***********************************************************************************/
#ifndef TCPCLIENT_H
#define TCPCLIENT_H

/************************************ INCLUDES *************************************/
//Libraries
#include <string>
#include <array> 

extern "C"
{
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <esp_log.h>
    #include <errno.h>
    #include <unistd.h>
    #include "esp_partition.h"
}

/********************************** NAMESPACE **************************************/
namespace TruckComms
{
/****************************** CLASS DEFINITIONS **********************************/
/*===========================================================================
 * Class:           TcpClient
 * Description:     Class for managing the client side in TCP
 */
    class TcpClient
    {
        private:
            // Placeholder for socket
            int sock;
            // Placeholder for binary address 
            struct sockaddr_in dest_addr;
            // Data buffer for data packet
            char rx_buffer[24];
            // Specify IPv4
            int addr_family;
            // ESP IDF default ip protocol
            int ip_protocol;
            // IP address
            std::string host_ip;
            // Application port
            int port;
        public:
            // Constructor   
            TcpClient(char* &Host_IP, int Port);
            // Destructor
            ~TcpClient();
            // Create Socket
            void CreateSocket();
            // Connect to server
            void ConnectToServer();
            // Send a single value data 
            void SendData(double data);
            // Send an array of 3 single value data
            void SendDataSet(const std::array<double,3> &DataSet);
            // Receive a single value data
            void ReceiveData(double &received_data);
            // Receive 3 single value data and create an array from them
            std::array<double,3> ReceiveDataSet();
            // Close the socket
            void CloseSocket();
    };
}

#endif