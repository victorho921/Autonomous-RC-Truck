<a id="readme-top"></a>
# Autonomous RC Truck 
<div align="center">
  <img src="README_Images/project_logo.jpg" alt="Project Logo" width="250"/>
</div>

This project is part of the MECH524 course at UBC, focusing on the design and implementation of an autonomous navigation system for an RC truck. 
The primary goal is to develop a user-friendly GUI for creating and managing paths, transmit these paths to a modified RC truck equipped with a microcontroller and sensors, 
enable the truck to autonomously navigate the defined path, and finally, collect and display the truck's actual path within the GUI for comparison and analysis. 
This project consists of three main programs:

1. **Truck Firmware**: Handles the low-level control and communication of the RC truck.
2. **GUI**: A user interface for configuring waypoints, managing paths, and monitoring the truck in real-time.
3. **Comms**: Provide communication protocol and internet connection for connecting GUI with the truck  
---



## Table of Contents
1. [About the Project](#about-the-project)
1. [Truck Firmware](#truck-firmware)
   - Features
   - Prerequisites
   - Installation
   - Usage
1. [GUI](#gui)
   - Features
   - Prerequisites
   - Installation
   - Usage
1. [Comms](#comms)
   - Features
   - Prerequisites
   - Usage
1. [RoadMap](#roadmap)
1. [Project Structure](#project-structure)
1. [Contributing](#contributing)
1. [License](#license)
1. [Contact](#contact)

## About The Project

![Product Name Screen Shot][product-screenshot]

This project aims to remove the original control board from an RC car and use a 
microcontroller to control it instead. The microcontroller will be outfitted with 
an accelerometer for dead-reckoning positioning and a microSD card reader for data 
logging so that the device can follow a programmed path and correct for disturbances.

## Truck Firmware
### Features
#### 1. Custom Classes Structure

The firmware features custom classes for the sensor, servo, dc motor, and for accessing memory in the specific ways that the truck needs. These are all brought together by a truck class which contains instances of each of these classes. The classes are summarized in the diagram below:

![Class Diagram for Firmware][firmware-classes]


#### 2. Accelerometer Data Filtering

Due to the accelerometer's noisy readings, some post-processing must occur to counteract drift for dead-reckoning positioning. This post-processing is done by using a discrete low pass filter in the form of a moving average; this is implemented by averaging over the previous 5 samples. Additionally thresholds are used for acceleration, rotational speed, and velocity, that is if the value is within the range of (0 - threshold, 0 + threshold), then it is assigned a value of zero. This is done to avoid build up of position drift from small non-zero values - especially since a nonzero static acceleration causes an exponential position drift. These threshold values are 0.075 m/s2, 2 deg/s, and 0.01m/s for acceleration, rotation rate and velocity respectively.


#### 3. State Updating

The state updating task is one of two tasks running concurrently, the other is the path following task; therefore, mutex is used to lock out shared variables when they are being read or written. 

The state updating task consists of variable initialization before entering a while loop that cyclically reads the accelerometer and gyroscope values, converts them to m/s2 and deg/s, filters them as discussed in “Onboard Firmware Implementation”, and uses them to update the state of the vehicle, for discrete time steps. The time between measurements is measured and used to evaluate the state change. This task is also responsible for logging the path taken by the vehicle into memory; to do so it writes the state into memory every 10 iterations - this sparsity is added to reduce the data volume saved into memory.

#### 4. Path Following

The path-following task consists of two main parts: variable initialization and a while loop. During initialization, the first segment of the path is loaded into memory. Within the loop, the future position of the vehicle is predicted every 0.5 seconds, assuming constant velocity and rotation rate. If the predicted position remains within the defined path radius, no action is taken, and the loop proceeds to the next iteration. However, if the predicted position deviates from the path, course corrections are made using a PI-style (Proportional-Integral) discrete-time control method. When the truck reaches the endpoint of a path segment (within the specified radius), the next segment is loaded into memory, and the process continues.

### Prerequisites

#### Required Hardware

This project is a hybrid hardware/firmware/software project, as such you will need hardware to replicate it. The hardware used is:

 - Microcontroller (ESP32)
 - Intertial Measurement Unit (MPU-9250)
 - Radio Controlled Vehicle (WPL D12 R/C Kei Truck 1/10 Scale)
 - Various JST-HX/Molex Connectors & Wires
 - Micro SD Card Reader & Micro SD card
 - Brushless D/C Motor Controller (L293D)

#### Wiring

The wiring was installed as shown in this picture, for a different ESP32 board research which pins to use and change them in the source code. Our team does not accept any liability for this electrical set-up; the user may mimic the wiring at their own risk.

![Image of RC Truck Wiring][rc-truck-wires]


### Installation

1. Clone the repo
   ```sh
   git clone https://github.com/jguest/Autonomous_RC_Truck.git
   ```
2. Install ESP-IDF VSCode Extension
3. Open /RCTruck/source/
4. build and flash main.cpp onto ESP32 

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## GUI
### Features
#### 1. **Path Management**
- Manual point Inputs: Textboxes for X Coordinates (-5)->5[m], Y Coordinates (-5)->5[m], 
and speed 1->100 [%].
- Waypoint List: A data list view displays all configured waypoints
- Save Path: Export current path as a .CSV file
- Load Path from File: Load a pre planned .CSV path to current path
- Map: Display the planned path on a Map, Overlay the recorded path
#### 2. **Connection to RC Truck**
- Connect and Send: A button to send the list of waypoints as a path to the RC truck.
- Receive Points From Truck: A button to receive recorded DATA from the truck, and overlay the recorded path on top of the planned path

#### 4. **Error Reporting**
- Console: A mirror of the console, for Error and status check. 

#### 5. **Display Alerts in error messages**
- Display alerts or error messages for issues like invalid waypoint inputs or connection errors.

### Prerequisites
#### Tools and Technologies
- **WinUI 3**: For building the GUI.
- **C#**: The programming language used for the application.
- **Visual Studio**: Development environment with Git integration.

#### System Requirements
- Windows 10 or later.
- .NET 5.0 or higher installed.
- Compatible Bluetooth or network connection for truck communication.

---

### Installation

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

### Usage
#### Adding Waypoints
1. Enter X coordinate (0<X<5), Y Coordinate (0<Y<5), and speed (1<Speed<100) in the respective text boxes.
2. Click the **Add Waypoint** button to add the waypoint to the path.
3. View the waypoint in the list.

#### Saving a Path or Loading a path
1. after designing a path, The path can be exported as a .csv format using the "Save Path" button.
The path will then be saved to "Paths" folder within the GUI folder. If the folder doesn't already exist, it will be created automatically.
Every line in the resultant file represents a point
Each line is comprised of "\<X coordinate>, \<Y coordinate>, \<Speed>"
2. to load a preexisting path, press the "Load Path" button. 
The program will then prompt the user to choose a file from any chosen directory.
If the file formatting, and every point in it comply with the point constraints ((-5)<X,Y<5, 1<Speed<100), The path will be loaded to the list and displayed on the map.
If the point data doesn't match the constraints, an error message will be displayed.

#### Managing the Path
1. Add all waypoints required for the path.
2. Click **Connect and Send** to send the waypoints to the RC truck.
3. Use **Receive Points from Truck** to get and plot the recorded data.
<p align="right">(<a href="#readme-top">back to top</a>)</p>

## Comms
### Features
#### 1. Data Packaging 
- Information exchange through binary format
- Data are packeted as 3 byte per transmit (X,Y, Speed)
- Before each transmit, number of data will send to the receiver side and use for verification

#### 2. Transmission
- Transfer Path Datapoint to Client by Server
- IMU data can send back to GUI from truck

#### 3. **Truck**
- Acts as a TCP client.
- Connects to the GUI (TCP server) over Wi-Fi using the server’s IP address and port.
#### 4. **GUI** 
- Acts as a TCP server.
- Listens for available connection through predefined port.

### Prerequisites
#### Wifi Connection
- (Recommend) Start a mobile hotspot with phone and allow connection for devices
- Make sure the wifi access point authentication at least `WPA2_PSK`

### Usage
1. Code should be incorporated inside truck firmware and gui
2. To establish wifi connection, change the following `IP_ADDRESS` to be the ip address of your computer
```
#define HOST_IP_ADDR "IP_ADDRESS"
```
3. (optional) Change the following `PORT_NUMBER` to your desired port number, make sure the port is opened for application 
```
#define PORT "PORT_NUMBER"
```
<p align="right">(<a href="#readme-top">back to top</a>)</p>

## Roadmap

- [x] Hardware Acquired
- [x] Hardware Built
- [x] Data Collection Program (inputs and sensor readings)
- [x] Real-Time Control to drive straight (with gyro/accelerometer corrections)
- [x] Real-Time Control to drive any preprogrammed path
- [x] GUI to create a path file for position and velocity (program file into RTOS manually)
- [x] Create program to integrate actual path from data log
- [ ] Add actual path to GUI overlaid with original path (after the fact)
- [x] Comms over Wifi or BT to upload path automatically and download logged data at completion
- [ ] Continuous Comms 
    - [ ] Remotely start/stop
    - [ ] Continuous plotting of path driven
    - [ ] Manual Wireless Control from laptop


See the [open issues](https://github.ubc.ca/MECH-V-524-101-V2024W1/Autonomous_RC_Truck/issues) for a full list of proposed features (and known issues).

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## Project Structure

<!-- CONTRIBUTING -->

## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

### Project Structure

The project is structured as follows:

```
Autonomous_RC_Truck/
├── Comms/               # Communication libraries
├── GUI/                 # Gui code
├── RCTruck/             # Truck Firmware
├── README.md            # Project documentation
└── .gitignore           # Git ignore rules
```
The project is comprised of 2 different programs- 
1. Truck firmware (C++)
1. GUI (C#)


### Compiling

The firmware is compiled using the ESP-IDF VSCode extension. Running a ESP-IDF terminal, execute the following:

```
(base) username@dhcp-123-45-678-90 source % idf.py build
(base) username@dhcp-123-45-678-90 source % idf.py flash
```
Additionally, if desired, the user can maintain a wired connection to view serial monitor logging, to do this execute the following:

```
(base) username@dhcp-123-45-678-90 source % idf.py monitor
```

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- LICENSE -->

## License

Distributed under the GNU General Public License v3. See `LICENSE.txt` for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CONTACT -->

## Contact

**James Guest - 35070432**:  
[![Mail][mailto-shield]](mailto:jguest@student.ubc.ca)  
[![LinkedIn][linkedin-shield]][james-linkedin-url]  
[![GitHub][github-shield]][james-github-url]  

**Eylon Avigur - 22995047**: <!--Student Number-->  
[![Mail][mailto-shield]](mailto:Eavigur@gmail.com)  
[![LinkedIn][linkedin-shield]][eylon-linkedin-url]  
[![GitHub][github-shield]][eylon-github-url]  

**Victor Yeung - 59967653**: <!--Student Number-->  
[![Mail][mailto-shield]](mailto:Vicye@student.ubc.ca)  
[![LinkedIn][linkedin-shield]][victor-linkedin-url]  
[![GitHub][github-shield]][victor-github-url]  

---

Project Link: [github.ubc.ca/MECH-V-524-101-V2024W1/Autonomous_RC_Truck](https://github.ubc.ca/MECH-V-524-101-V2024W1/Autonomous_RC_Truck)

---

[mailto-shield]: https://img.shields.io/badge/Email-Contact-blue
[linkedin-shield]: https://img.shields.io/badge/LinkedIn-Profile-blue
[github-shield]: https://img.shields.io/badge/GitHub-Profile-black
[james-mailto]: mailto:jguest@student.ubc.ca
[eylon-mailto]: mailto:Eavigur@gmail.com
[victor-mailto]: mailto:Vicye@student.ubc.ca

<!-- ACKNOWLEDGMENTS -->

<!--reference-style-links -->

[mailto-shield]: https://img.shields.io/badge/-Mail-black.svg?style=flat-square&logo=Minutemailer&colorB=555&logoColor=white
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=flat-square&logo=linkedin&colorB=555
[github-shield]: https://img.shields.io/badge/-GitHub-black.svg?style=flat-square&logo=github&colorB=555


[james-mailto]: mailto:jguest@student.ubc.ca
[eylon-mailto]: mailto:EAvigur@gmail.com
[victor-mailto]: mailto:Vicye@student.ubc.ca

[james-linkedin-url]: https://linkedin.com/in/jamesguest01123
[eylon-linkedin-url]: https://www.linkedin.com/in/eylon-avigur-286a01155/
[victor-linkedin-url]: https://www.linkedin.com/in/victor-yeung-264886270/


[james-github-url]: https://github.com/jg0112358
[eylon-github-url]: https://github.com/
[victor-github-url]: https://github.com/

[README-template]: https://github.com/othneildrew/Best-README-Template

[product-screenshot]: README_Images/GUI.jpg
[rc-truck-wires]: README_Images/wiring.png
[firmware-classes]: README_Images/firmware-classes.png


[React.js]: https://img.shields.io/badge/React-20232A?style=for-the-badge&logo=react&logoColor=61DAFB
[React-url]: https://reactjs.org/

[JQuery.com]: https://img.shields.io/badge/jQuery-0769AD?style=for-the-badge&logo=jquery&logoColor=white
[JQuery-url]: https://jquery.com 

[Qt-project]: https://img.shields.io/badge/Qt_Project-0?style=for-the-badge&logo=qt&logoColor=white&logoSize=auto&color=%2341cd52
[Qt-url]: Qt-project.org

[Figma-badge]: https://img.shields.io/badge/Figma-0?style=for-the-badge&logo=figma&logoSize=auto&color=%23e6e6e6
[Figma-url]: https://www.figma.com/
