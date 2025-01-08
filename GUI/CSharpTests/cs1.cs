using System;

namespace ConsoleApp1
{
    class Program
    {
        static void Main()
        {
            // Create a rectangle
            CRectangle rect = new CRectangle(5.0, 10.0);

            // Display dimensions
            Console.WriteLine($"Width: {rect.GetWidth()}");
            Console.WriteLine($"Height: {rect.GetHeight()}");
            Console.WriteLine($"Area: {rect.GetArea()}");
            Console.WriteLine($"Perimeter: {rect.GetPerimeter()}");

            // Update dimensions
            rect.SetWidth(8.0);
            rect.SetHeight(12.0);

            // Display updated dimensions
            Console.WriteLine("\nUpdated Dimensions:");
            Console.WriteLine($"Width: {rect.GetWidth()}");
            Console.WriteLine($"Height: {rect.GetHeight()}");
            Console.WriteLine($"Area: {rect.GetArea()}");
            Console.WriteLine($"Perimeter: {rect.GetPerimeter()}");
        }
    }
}
