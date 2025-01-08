# Software Development for Mechatronic Systems
# Final Project- RC Truck, GUI 

This is a Graphical User Interface (GUI) designed for controlling and monitoring an RC truck. 
The GUI allows users to configure waypoints, manage paths, log data, 
and Send/Receive path data to and from the RC truck. It is built using WinUI 3 and C# 
for a modern and responsive user experience.

---
## Table of Contents


## Features

### 1. **Path Management**
- Manual point Inputs: Textboxes for X Coordinates (-5)->5[m], Y Coordinates (-5)->5[m], 
and speed 1->100 [%}.
- Waypoint List: A data list view displays all configured waypoints
- Save Path: Export current path as a .CSV file
- Load Path from File: Load a pre planned .CSV path to current path
- Map: Display the planned path on a Map, Overlay the recorded path
### 2. **Connection to RC Truck**
- Connect and Send: A button to send the list of waypoints as a path to the RC truck.
- Receive Points From Truck: A button to receive recorded DATA from the truck, and overlay the recorded path on top of the planned path

### 4. **Error Reporting**
- Console: A mirror of the console, for Error and status check. 

### 5. **Display Alerts ir errir messages**
- Display alerts or error messages for issues like invalid waypoint inputs or connection errors.

---

## Prerequisites

### Tools and Technologies
- **WinUI 3**: For building the GUI.
- **C#**: The programming language used for the application.
- **Visual Studio**: Development environment with Git integration.

### System Requirements
- Windows 10 or later.
- .NET 5.0 or higher installed.
- Compatible Bluetooth or network connection for truck communication.

---

## Installation

1. **Clone the Repository**:
   ```bash
   git clone <repository-url>
   cd RC-Truck-GUI
   ```

1. **Open the Project**:
   Open the project in Visual Studio.

1. **Build the Project**:
   - Ensure all dependencies are installed.
   - Select the desired build configuration (Debug/Release).
   - Build the solution.

1. **Run the Application**:
   - Press `F5` to run the application.

---

## Usage

### Adding Waypoints
1. Enter X coordinate (0<X<5), Y Coordinate (0<Y<5), and speed (1<Speed<100) in the respective text boxes.
2. Click the **Add Waypoint** button to save the waypoint.
3. View the waypoint in the list.

### Managing the Path
1. Add all waypoints required for the path.
2. Click **Connect and Send** to send the waypoints to the RC truck.
3. Use **Receive Points from Truck** to get and plot the recorded data.

## Folder Structure
```
Autonomous_RC_Truck/
├── Comms/               # Communication libraries
├── GUI/                 # Gui code
├── RCTruck/             # Truck Firmware
├── README.md            # Project documentation
└── .gitignore           # Git ignore rules
```

---

## Created By
- James Guest, jguest@student.ubc.ca
- Victor Yeung vicye@student.ubc.ca
- Eylon Avigur EAvigur@gmail.com
