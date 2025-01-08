using System;

namespace ConsoleApp1
{
    public class CRectangle
    {
        // Private fields
        private double Width;
        private double Height;

        // Constructor
        public CRectangle(double width, double height)
        {
            if (width <= 0)
                throw new ArgumentException("Width must be positive.", nameof(width));
            if (height <= 0)
                throw new ArgumentException("Height must be positive.", nameof(height));

            Width = width;
            Height = height;
        }

        // Public methods to get width and height
        public double GetWidth() => Width;
        public double GetHeight() => Height;

        // Public methods to set width and height
        public bool SetWidth(double width)
        {
            if (width <= 0)
            {
                Console.WriteLine("Error: Width must be positive.");
                return false;
            }
            Width = width;
            return true;
        }

        public bool SetHeight(double height)
        {
            if (height <= 0)
            {
                Console.WriteLine("Error: Height must be positive.");
                return false;
            }
            Height = height;
            return true;
        }

        // Method to calculate area
        public double GetArea() => (Width * Height);

        // Method to calculate perimeter
        public double GetPerimeter() => 2 * (Width + Height);
    }
}
