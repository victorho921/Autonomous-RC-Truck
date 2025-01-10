/************************************************************************************
 * Project:         Autonomous RC Truck                                             *
 *                                                                                  *
 * Author(s):       Victor Yeung (59967653)                                         *
 * File:            TcpServer.cs                                                    *
 * Purpose:         To contain function associated with the TCP Server              *
 * Description:                                                                     *
 *                                                                                  *
 *                                                                                  *
 ***********************************************************************************/
 /********************************** NAMESPACE **************************************/
using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Collections.Generic;

namespace TcpServerLibrary
{
/****************************** CLASS DEFINITIONS **********************************/
/*===========================================================================
 * Class:           TcpServer
 * Description:     Class for managing the server side in TCP
 */
    public class TcpServer
    {
        // IP Address for listening (listen to all available IP address) 
        private const string Host = "0.0.0.0";
        // Port for listening
        private const int Port = 3333;
        // Server object
        private TcpListener _server;
        // Client object
        private TcpClient _client;
        // Stream object
        private NetworkStream _stream;

        // Declare a lock object for synchronization
        private static readonly object lockObject = new object();

        // Declare the static array to hold received data
        public static double[] receivedData = new double[3];

/*===========================================================================
 * Function:        TcpListener::SetupServer
 * Arguments:       host(string), port(int)
 * Returns:         TcpListener object
 * Description:     Parse arguement to initilize TcpListener object
 */
        public static TcpListener SetupServer(string host, int port)
        {
            // Convert string to IPAddress object
            IPAddress ipAddress = IPAddress.Parse(host);
            return new TcpListener(ipAddress, port);
        }

/*===========================================================================
 * Function:        TcpServer::ReceiveData
 * Arguments:       stream (NetworkStream object)
 * Returns:         data (double)
 * Description:     Receive data using stream.Read()
 */
        public static double? ReceiveData(NetworkStream stream)
        {
            try
            {
                // Create a empty buffer for a double
                byte[] buffer = new byte[8];
                // Receive Data
                int bytesRead = stream.Read(buffer, 0, buffer.Length);
                if (bytesRead == 0)
                    return null;
                // Convert byte to double
                return BitConverter.ToDouble(buffer);
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error receiving data: {ex.Message}");
                return null;
            }
        }

/*===========================================================================
 * Function:        TcpServer::ReceiveDataSet
 * Arguments:       stream (NetworkStream object)
 * Returns:         dataset (double[])
 * Description:     Call ReceiveData() three times
 */        
        public static double[] ReceiveDataSet(NetworkStream stream) 
        { 
            // Creat an empty double array with 3 units
            //static double[] receivedData = new double[3];
            // Console.WriteLine($"Start receiving data thread");

            lock (lockObject)
            {
                for (int i = 0; i < 3; i++) 
                { 
                    // Call ReceiveData three times
                    double? data = ReceiveData(stream); 
                    if (data.HasValue) 
                    { 
                        receivedData[i] = data.Value; 
                    } 
                    else 
                    { 
                        Console.WriteLine($"Error receiving data at index {i}"); 
                        receivedData[i] = 0;
                    } 
                }
            }       
            // Log the received array 
            Console.WriteLine($"Received array: [{receivedData[0]:F2}, {receivedData[1]:F2}, {receivedData[2]:F2}]"); 
            return receivedData;
        }


/*===========================================================================
 * Function:        TcpServer::SendData
 * Arguments:       stream (NetworkStream object), data(double)
 * Returns:         none
 * Description:     Send data using stream.Write()
 */
        public static void SendData(NetworkStream stream, double data)
        {
            try
            {
                // Convert a double value into byte array
                byte[] buffer = BitConverter.GetBytes(data);
                // Check if the system is little endian
                if (!BitConverter.IsLittleEndian)
                {
                    // Convert it if big endian
                    Array.Reverse(buffer);
                }
                // Send the value
                stream.Write(buffer, 0, buffer.Length);
                // Log the sending
                Console.WriteLine("Data sent to client.");
                Console.WriteLine($"Sending bytes for double: {BitConverter.ToString(buffer)}");
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error sending data: {ex.Message}");
            }
        }

/*===========================================================================
 * Function:        TcpServer::SendDataSet
 * Arguments:       stream (NetworkStream object), data(double[])
 * Returns:         none
 * Description:     Call SendData() three times 
 */
        public static void SendDataSet(NetworkStream stream, double[] dataSet) 
        { 
            foreach (double data in dataSet) 
            { 
                // Call SendData three times
                SendData(stream, data); 

            } 
        }

/*===========================================================================
 * Function:        TcpServer::StartServer
 * Arguments:       none
 * Returns:         NetworkStream object
 * Description:     Call SetupServer and wait for connection 
 */
        public NetworkStream StartServer()
        {
            try
            {
                // Initiate  server object
                _server = SetupServer(Host, Port);
                // Finding client
                _server.Start();
                Console.WriteLine($"Version 3");
                Console.WriteLine($"Server listening on {Host}:{Port}");

                while (true)
                {
                    // Accept a single client connection
                    _client = _server.AcceptTcpClient();
                    if (_client.Connected == true){
                        Console.WriteLine($"Client connected: {((IPEndPoint)_client.Client.RemoteEndPoint).Address}");
                        break;
                    }
                    Console.WriteLine($"Waiting for client connection"); 
                    Thread.Sleep(500);
                }
                // Return the client's NetworkStream
                _stream = _client.GetStream();
                return _stream;
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error starting server: {ex.Message}");
                Disconnect();
                throw; 
            }
        }

/*===========================================================================
 * Function:        TcpServer::Disconnect
 * Arguments:       none
 * Returns:         none
 * Description:     Disconnect the server
 */
        public void Disconnect()
        {
            try
            {
                // Close the stream
                if (_stream != null)
                {
                    _stream.Close();
                    Console.WriteLine("NetworkStream closed.");
                }

                // Close the client connection
                if (_client != null)
                {
                    _client.Close();
                    Console.WriteLine("Client connection closed.");
                }

                // Stop the server
                if (_server != null)
                {
                    _server.Stop();
                    Console.WriteLine("Server stopped.");
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error during disconnect: {ex.Message}");
            }
            finally
            {
                _stream = null;
                _client = null;
                _server = null;
            }
        }

        // 2 way handshake 
        // Receive Permission
        public static bool HandShakeSYN(NetworkStream stream) 
        {
            byte[] buffer = new byte[1024];
            string ClientMessage = "NOTREADY"; 
            string ServerResponse = "NOTOK";

            try
            {
                // Step 1: Wait for "READY?" signal from client
                // Set timeout for reading
                stream.ReadTimeout = 10000;
                int bytesRead = stream.Read(buffer, 0, buffer.Length);
                ClientMessage = Encoding.ASCII.GetString(buffer, 0, bytesRead);
                Console.WriteLine($"Client says: {ClientMessage}");

                // Step 2: Respond with "OK" if "READY" is received
                if (ClientMessage == "READY?")
                {
                    ServerResponse = "OK";
                    // Convert ServerResponse to byte
                    byte[] ResponseBytes = Encoding.ASCII.GetBytes(ServerResponse);
                    // Send the Response
                    stream.Write(ResponseBytes, 0, ResponseBytes.Length);
                    // Log
                    Console.WriteLine($"Server responds: {ServerResponse}");
                    return true;
                }
                else
                {
                    return false;
                }
            }
            catch (IOException ex)
            {
                Console.WriteLine($"Timeout or network error: {ex.Message}");
                return false;
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Unexpected error: {ex.Message}");
                return false;
            }
        }

        // 2 way handshake
        // Ackonwledgement

        //public static void RespondClient(bool CheckLength)
        public static void RespondClient(NetworkStream stream)
        {
            string ServerResponse = "Not Data Received";
            // Change ServerResponse
            ServerResponse = "Data Received";
            // Convert ServerResponse to byte
            byte[] ResponseBytes = Encoding.ASCII.GetBytes(ServerResponse);
            // Send the Response
            stream.Write(ResponseBytes, 0, ResponseBytes.Length);
            // Log
            Console.WriteLine($"Server acknowledges: {ServerResponse}");
        }

        // Send Permission
        public static bool SendSYN(NetworkStream stream)
        {
            byte[] buffer = new byte[1024];
            string ClientMessage = "READY"; 
            string ServerResponse = "NOTOK";
            try
            {
                // Send SYN signal (READY?)
                // Convert ServerResponse to byte
                byte[] SYNBytes = Encoding.ASCII.GetBytes(ClientMessage);
                // Send the Response
                stream.Write(SYNBytes, 0, SYNBytes.Length);
                // Log
                Console.WriteLine($"Permission sent");
                
                // Wait ACK
                while (true)
                {
                    int bytesRead = stream.Read(buffer, 0, buffer.Length);
                    ClientMessage = Encoding.ASCII.GetString(buffer, 0, bytesRead);
                    Console.WriteLine($"Client says: {ClientMessage}");
                    if (ClientMessage == "READY")
                    {
                        ServerResponse = "OK";
                        // Convert ServerResponse to byte
                        byte[] ResponseBytes = Encoding.ASCII.GetBytes(ServerResponse);
                        // Send the Response
                        stream.Write(ResponseBytes, 0, ResponseBytes.Length);
                        // Log
                        Console.WriteLine($"Server responds: {ServerResponse}");
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Unexpected error: {ex.Message}");
                return false;
            }

        }

    }
}


        

