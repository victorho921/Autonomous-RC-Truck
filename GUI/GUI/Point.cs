//******************************************************************************
// Project: RC Truck GUI
// File: Point.cs
// Author: Eylon Avigur
// Date: Dec 11, 2024
//
// Description: This file defines the Point class, which represents a waypoint 
//              in the RC truck's path. Each Point object encapsulates 
//              coordinates (X, Y) and an associated speed.
//
// Features:
//   - Encapsulates X and Y coordinates as integers.
//   - Stores the speed associated with the waypoint.
//   - Provides a string representation of the point for easy display.
//
// Usage Notes:
//   - Ensure coordinates and speed are validated prior to instantiation.
//   - The speed parameter must be a positive integer.
//
// Example:
//   Point waypoint = new Point(10, 20, 5);
//
//******************************************************************************

using Microsoft.UI.Xaml;
using System;

namespace Gui_Training
{
    public class Point
    {
        private double _x;
        private double _y;
        private double _speed;

        public double X
        {
            get => _x;
            set
            {
                if (value < -5 || value > 5)
                    throw new ArgumentOutOfRangeException(nameof(X), "X must be between (-5) and 5.");
                _x = value;
            }
        }

        public double Y
        {
            get => _y;
            set
            {
                if (value < -5 || value > 5)
                    throw new ArgumentOutOfRangeException(nameof(Y), "Y must be between (-5) and 5.");
                _y = value;
            }
        }

        public double Speed
        {
            get => _speed;
            set
            {
                if (value < 1 || value > 100)
                    throw new ArgumentOutOfRangeException(nameof(Speed), "Speed must be between 1 and 100.");
                _speed = value;
            }
        }

        public Point(double x, double y, double speed)
        {
            X = x;
            Y = y;
            Speed = speed;
        }

        public override string ToString()
        {
            return $"Point(X: {X}, Y: {Y}, Speed: {Speed})";
        }

        public double[] PointAsDoubleArray()
        {
            return new double[] { X, Y, Speed };
        }


    }
}