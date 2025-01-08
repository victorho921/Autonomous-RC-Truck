using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Collections.Generic;
using System.IO;
using Windows.Media.Protection.PlayReady;

namespace TcpServerLibrary
{
    public class TcpServer
    {
        private const string Host = "0.0.0.0";
        private const int Port = 3333;
        private TcpListener _server;
        private TcpClient _client;
        private NetworkStream _stream;

        // public void Main(string[] args)
        // {
        //     TcpListener server = StartServer();

        //     // float dataToSend = 1.151f;
        //     NetworkStream stream = null;

        //     try
        //     {
        //         TcpClient client = server.AcceptTcpClient();
        //         Console.WriteLine($"Connection established with {((IPEndPoint)client.Client.RemoteEndPoint).Address}");

        //         stream = client.GetStream();
        //         while (true)
        //         {
        //             float? receivedData = ReceiveData(stream);
        //             Console.WriteLine($"Received Data: {receivedData}");

        //             Thread.Sleep(5000);
        //         }
        //     }
        //     catch (Exception ex)
        //     {
        //         Console.WriteLine($"Error: {ex.Message}");
        //     }
        //     finally
        //     {
        //         stream?.Close();
        //         Console.WriteLine("Connection closed.");
        //         server.Stop();
        //         Console.WriteLine("Server shut down.");
        //     }
        // }

        // public TcpListener StartServer()
        // {
        //     TcpListener server = SetupServer(Host, Port);

        //     try
        //     {
        //         server.Start();
        //         Console.WriteLine($"Server listening on {Host}:{Port}");
        //         // Updating
        //         while (true)
        //         {
        //             try
        //             {
        //                 TcpClient client = listener.AcceptTcpClient();
        //                 NetworkStream stream = client.GetStream();
        //                 Console.WriteLine($"Client connected: {((IPEndPoint)client.Client.RemoteEndPoint).Address}");
        //             }
        //         }
        //     }
        //     catch (Exception ex)
        //     {
        //         Console.WriteLine($"Error starting server: {ex.Message}");
        //     }



        //     return server;
        // }

        public static TcpListener SetupServer(string host, int port)
        {
            IPAddress ipAddress = IPAddress.Parse(host);
            return new TcpListener(ipAddress, port);
        }

        public static double? ReceiveData(NetworkStream stream)
        {
            try
            {
                byte[] buffer = new byte[8];
                int bytesRead = stream.Read(buffer, 0, buffer.Length);
                if (bytesRead == 0)
                    return null;

                return BitConverter.ToDouble(buffer);
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error receiving data: {ex.Message}");
                return null;
            }
        }
        
        public static double[] ReceiveDataSet(NetworkStream stream) 
        { 
            double[] receivedData = new double[3]; 
            for (int i = 0; i < 3; i++) 
            { 
                double? data = ReceiveData(stream); 
                if (data.HasValue) 
                { 
                    receivedData[i] = data.Value; 
                } 
                else 
                { 
                    Console.WriteLine($"Error receiving data at index {i}"); 
                    receivedData[i] = 0; // Assign a default value or handle as needed 
                } 
            } // Log the received array 
            Console.WriteLine($"Received array: [{receivedData[0]:F2}, {receivedData[1]:F2}, {receivedData[2]:F2}]"); 
            return receivedData;
        }




        public static void SendData(NetworkStream stream, double data)
        {
            try
            {
                byte[] buffer = BitConverter.GetBytes(data);

                if (!BitConverter.IsLittleEndian)
                {
                    Array.Reverse(buffer);
                }

                stream.Write(buffer, 0, buffer.Length);

                Console.WriteLine("Data sent to client.");
                Console.WriteLine($"Sending bytes for float: {BitConverter.ToString(buffer)}");
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error sending data: {ex.Message}");
            }
            // finally
            // {
            //     stream?.Close();
            //     Console.WriteLine("Connection closed.");
            //     server.Stop();
            //     Console.WriteLine("Server shut down.");
            // }
        }

        public static void SendDataSet(NetworkStream stream, double[] dataSet) 
        { 
            foreach (double data in dataSet) 
            { 
                SendData(stream, data); 
            } 
        }

        //public NetworkStream StartServer()
        //{
        //    TcpListener server = null;

        //    try
        //    {
        //        _server = SetupServer(Host, Port);
        //        _server.Start();
        //        Console.WriteLine($"Server listening on {Host}:{Port}");

        //        // Accept a single client connection
        //        TcpClient client = server.AcceptTcpClient();
        //        Console.WriteLine($"Client connected: {((IPEndPoint)client.Client.RemoteEndPoint).Address}");

        //        // Return the client's NetworkStream
        //        return client.GetStream();
        //    }

        //    catch (Exception ex)
        //    {
        //        Console.WriteLine($"Error starting server: {ex.Message}");
        //        server?.Stop();
        //        throw; // Re-throw exception to notify the caller
        //    }
        //}

        public NetworkStream StartServer()
        {
            try
            {
                _server = SetupServer(Host, Port);
                _server.Start();
                Console.WriteLine($"Server listening on {Host}:{Port}");

                // Accept a single client connection
                _client = _server.AcceptTcpClient();
                Console.WriteLine($"Client connected: {((IPEndPoint)_client.Client.RemoteEndPoint).Address}");

                // Return the client's NetworkStream
                _stream = _client.GetStream();
                return _stream;
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error starting server: {ex.Message}");
                Disconnect();
                throw; // Re-throw exception to notify the caller
            }
        }
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
    }

}

