/******************************************************************************
 * Project: RC Truck GUI 
 * File: MainWindow.xaml.cs
 * Author: Eylon Avigur
 * Date: Dec 11, 2024
 * 
 * Description: This file implements the main functionality of the GUI application 
 *              for waypoint management and communication with an RC car.
 *              
 * Features:
 *   - Adding waypoints with coordinates and speed.
 *   - Visualizing waypoints and paths on a canvas.
 *   - Saving and loading waypoint data from files.
 *   - Sending waypoint data to an RC car via a TCP server.
 *   - Displaying a live console log in the GUI.
 *   
 * Usage Notes:
 *   - Ensure the TCP server is running before sending data.
 *   - Files are saved in the "Paths" directory within the application folder.
 *****************************************************************************/


using Microsoft.UI;
using Microsoft.UI.Composition;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Shapes;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using TcpServerLibrary;

namespace Gui_Training
{
    // Main window functionality
    public sealed partial class MainWindow : Window
    {
        // Initialize
        private List<Point> Points;
        private List<Point> ActualPath;
        NetworkStream stream = null;

        // Constructor
        public MainWindow()
        {
            this.InitializeComponent();
            Points = new List<Point>();
            ActualPath = new List<Point>();
            // Create a new point and add it to the queue
            Point newPoint = new Point(0, 0, 100);
            Points.Add(newPoint);

            // Display the point in the ListBox
            PointsListBox.Items.Add($"X: 0, Y: 0, Speed: 0");

            // Plot the point on the Canvas
            PlotPointOnCanvas(newPoint);

            // Draw the point and lines on the canvas
            DrawLineOnCanvas(newPoint, Microsoft.UI.Colors.Blue);

            // Redirect console output to the TextBox
            Console.SetOut(new ListBoxWriter(StatusBox));
            Console.WriteLine("Test");
        }


        /*****Points******/

        // Initialize the lastDrawnPoint as null
        private Point? lastDrawnPoint = null;
        // Method to add a point to the list, Taken from the X, Y and Speed fields in the GUI
        private async void OnCreatePointClick(object sender, RoutedEventArgs e)
        {
            try
            {
                // Parse inputs from the text boxes as integers
                double x = double.Parse(XCoordinateTextBox.Text);
                double y = double.Parse(YCoordinateTextBox.Text);
                double speed = double.Parse(SpeedTextBox.Text);

                // Validate inputs
                if (x < -5 || x > 5 || y < -5 || y > 5 || speed < 1 || speed > 100)
                {
                    await ShowErrorMessage("Coordinates must be between (-5) and 5, Speed must be between 1 and 100.");
                    return;
                }

                // Create a new point and add it to the queue
                Point newPoint = new Point(x, y, speed);
                Points.Add(newPoint);

                // Display the point in the ListBox
                PointsListBox.Items.Add($"X: {x}, Y: {y}, Speed: {speed}");

                // Plot the point on the Canvas
                PlotPointOnCanvas(newPoint);

                // Draw the point and lines on the canvas
                DrawLineOnCanvas(newPoint, Microsoft.UI.Colors.Blue);

                // Log success
                Console.WriteLine($"Point added: {newPoint}");
            }
            catch (FormatException)
            {
                await ShowErrorMessage("Invalid input: Please enter numeric values.");
            }
            catch (Exception ex)
            {
                await ShowErrorMessage($"Unexpected error: {ex.Message}");
            }
        }
        // Method to plot a new point on the map
        private void PlotPointOnCanvas(Point point)
        {
            // Scale coordinates to fit within the Canvas dimensions
            double canvasWidth = MapCanvas.ActualWidth;
            double canvasHeight = MapCanvas.ActualHeight;

            // Scale X and Y to fit within the range -5 to 5
            double scaledX = ((point.X + 5) / 10.0) * canvasWidth;
            double scaledY = ((5 - point.Y) / 10.0) * canvasHeight;

            // Create a circle to represent the point
            Ellipse pointEllipse = new Ellipse
            {
                Width = 10, // Diameter of the point
                Height = 10,
                Fill = new SolidColorBrush(Microsoft.UI.Colors.Red)
            };

            // Set the position of the circle on the Canvas
            Canvas.SetLeft(pointEllipse, scaledX - 5); // Offset by radius to center
            Canvas.SetTop(pointEllipse, scaledY - 5);

            // Add the circle to the Canvas
            MapCanvas.Children.Add(pointEllipse);
        }
        // Method to draw a line on the map
        private void DrawLineOnCanvas(Point point, Windows.UI.Color color)
        {
            double scaledX = ScaleX(point.X);
            double scaledY = ScaleY(point.Y);

            // Draw the point as a small circle
            Ellipse ellipse = new Ellipse
            {
                Width = 5,
                Height = 5,
                Fill = new SolidColorBrush(Microsoft.UI.Colors.Red)
            };

            Canvas.SetLeft(ellipse, scaledX - 2.5); // Center the ellipse
            Canvas.SetTop(ellipse, scaledY - 2.5);
            MapCanvas.Children.Add(ellipse);

            if (lastDrawnPoint != null)
            {
                Line line = new Line
                {
                    X1 = ScaleX(lastDrawnPoint.X),
                    Y1 = ScaleY(lastDrawnPoint.Y),
                    X2 = scaledX,
                    Y2 = scaledY,
                    Stroke = new SolidColorBrush(color),
                    StrokeThickness = 2
                };

                MapCanvas.Children.Add(line);
            }

            lastDrawnPoint = point;
        }
        // Method to redraw all the points and lines on the map
        private void DrawAllPointsAndLines(List<Point> points, Windows.UI.Color color)
        {
            foreach (Point point in points)
            {
                for (int i = 0; i < points.Count; i++)
                {
                    DrawLineOnCanvas(point, color);
                }
            }
        }

        /*Error handling and status*/
        private async Task ShowErrorMessage(string message)
        {
            ContentDialog dialog = new ContentDialog
            {
                Title = "Input Error",
                Content = message,
                CloseButtonText = "OK",
                DefaultButton = ContentDialogButton.Close,
                XamlRoot = this.Content.XamlRoot // Ensure dialog aligns with the window
            };

            await dialog.ShowAsync();
        }
        public class ListBoxWriter : TextWriter
        {
            private ListBox _listBox;

            public ListBoxWriter(ListBox listBox)
            {
                _listBox = listBox;
            }

            public override void WriteLine(string value)
            {
                _listBox.DispatcherQueue.TryEnqueue(() =>
                {
                    _listBox.Items.Add(new ListBoxItem { Content = value });
                });
            }

            public override void Write(char value)
            {
                Write(value.ToString());
            }

            public override void Write(string value)
            {
                _listBox.DispatcherQueue.TryEnqueue(() =>
                {
                    if (_listBox.Items.Count > 0)
                    {
                        // Append to the last item
                        ListBoxItem? lastItem = _listBox.Items[^1] as ListBoxItem;
                        lastItem.Content += value;
                    }
                    else
                    {
                        // Create a new item for the first write
                        _listBox.Items.Add(new ListBoxItem { Content = value });
                    }
                });
            }

            // Encoding?
            public override Encoding Encoding => Encoding.UTF8;
        }

        /*****Scaling*****/

        //Scale X direction
        private double ScaleX(double x) => ((x + 5) / 10.0) * MapCanvas.ActualWidth;
        //Scale Y direction
        private double ScaleY(double y) => ((5 - y) / 10.0) * MapCanvas.ActualHeight;

        /******Buttons*****/

        // Button functionality methods
        private async void OnSavePointListClick(object sender, RoutedEventArgs e)
        {
            try
            {
                // Ensure the Paths folder exists
                string folderPath = System.IO.Path.Combine(AppContext.BaseDirectory, "Paths");
                System.IO.Directory.CreateDirectory(folderPath);

                // Generate a file name with the current date and time
                string fileName = $"PointList_{DateTime.Now:yyyyMMdd_HHmmss}.csv";
                string filePath = System.IO.Path.Combine(folderPath, fileName);

                // Prepare the content to save
                List<string> lines = new List<string>();
                foreach (var point in Points)
                {
                    lines.Add($"{point.X}, {point.Y}, {point.Speed}");
                }

                // Save the file asynchronously
                await System.IO.File.WriteAllLinesAsync(filePath, lines);

                // Log success
                StatusBox.Items.Add($"Point list saved to {filePath}");
            }
            catch (Exception ex)
            {
                // Handle errors
                Console.WriteLine($"Error saving point list: {ex.Message}");
            }
        }

        private async void OnLoadPointsFromFileClick(object sender, RoutedEventArgs e)
        {
            try
            {
                // Open file dialog to select the file
                var filePicker = new Windows.Storage.Pickers.FileOpenPicker();
                filePicker.SuggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.Desktop;
                filePicker.FileTypeFilter.Add(".csv");

                // Initialize file picker with the current window handle
                var hwnd = WinRT.Interop.WindowNative.GetWindowHandle(this);
                WinRT.Interop.InitializeWithWindow.Initialize(filePicker, hwnd);

                // Pick a file
                var file = await filePicker.PickSingleFileAsync();
                if (file == null)
                {
                    await ShowErrorMessage("No file selected.");
                    return;
                }

                // Read the file content
                var linesList = await Windows.Storage.FileIO.ReadLinesAsync(file);
                var lines = linesList.ToArray();

                // Clear the current points and the PointsListBox
                Points.Clear();
                PointsListBox.Items.Clear();

                // Process each line in the file and add points to the queue
                foreach (var line in lines)
                {
                    try
                    {
                        // Expecting each line in format: <X value>, <Y value>, <Speed value>"
                        string[] parts = line.Split(',');

                        if (parts.Length == 3)// &&
                            //parts[0].Trim().StartsWith("X:") &&
                            //parts[1].Trim().StartsWith(" ") &&
                            //parts[2].Trim().StartsWith(" "))
                        {
                            int x = int.Parse(parts[0]);// int.Parse(parts[0].Trim().Substring(3));
                            int y = int.Parse(parts[1]);//.Trim().Substring(3));
                            int speed = int.Parse(parts[2]);//.Trim().Substring(7));

                            // Validate values
                            if (x >= -5 && x <= 5 && y >= -5 && y <= 5 && speed >= 1 && speed <= 100)
                            {
                                Point newPoint = new Point(x, y, speed);
                                Points.Add(newPoint);
                                PlotPointOnCanvas(newPoint);
                                DrawLineOnCanvas(newPoint, Microsoft.UI.Colors.Green);
                                PointsListBox.Items.Add($"X: {x}, Y: {y}, Speed: {speed}");
                            }
                            else
                            {
                                await ShowErrorMessage($"Invalid values in line: {line}");
                            }
                        }
                        else
                        {
                            await ShowErrorMessage($"Invalid format in line: {line}");
                        }
                    }
                    catch (FormatException)
                    {
                        // Handle any formatting issues
                        await ShowErrorMessage($"Invalid format in line: {line}");
                    }
                }
                // Success message
                StatusBox.Items.Add("Points loaded successfully.");
            }
            catch (Exception ex)
            {
                // Handle any errors while reading the file
                await ShowErrorMessage($"Error loading file: {ex.Message}");
            }
        }
        private void OnConnectAndSendClick(object sender, RoutedEventArgs e)
        {
            TcpServer server = new();
            stream = server.StartServer();
            TcpServer.SendData(stream, (double)Points.Count);
            for (int i = 0; i < Points.Count; i++)
            {
                //double x = Points.Peek().X;
                //double y = Points.Peek().Y;
                //double speed = Points.Peek().Speed;

                // Send point to ESP
                Thread.Sleep(2000);
                TcpServer.SendDataSet(stream, Points[i].PointAsDoubleArray());
            }
            server.Disconnect();
        }

        private void OnConnectAndReceive(object sender, RoutedEventArgs e)
        {
            TcpServer server = new();
            stream = server.StartServer();

            while (true)
            {
                // Assume TcpServer.ReceiveDataSet returns [x, y, speed]
                double[] dataPoint = TcpServer.ReceiveDataSet(stream);

                if (dataPoint.Length == 3)  // Ensure dataPoint has 3 values
                {
                    // Create a new Point object with X, Y, and Speed
                    //Point receivedPoint = new Point(dataPoint[0], dataPoint[1], dataPoint[2]);

                    // Add the received point to the path
                    //ActualPath.Add(receivedPoint);
                }
                else
                {
                    // Handle the case where the received data is not in the expected format
                    Console.WriteLine("Invalid data received");
                }
            }

            server.Disconnect();
        }

    }

}