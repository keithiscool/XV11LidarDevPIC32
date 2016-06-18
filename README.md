This project uses the XV11 Lidar from a Neato Vacuum Cleaner to track a robot in an 4meter x 8meter arena
The object tracking uses the robot reflectance in order to only track reflective surfaces and objects that are far away from the walls
The MPLabX project "lidarpic_FloatingPoint.X" in the software folder uses a PIC32MX575F512H-80I/PT (ordered from Digikey) micro-controller running at 80MHz
The hardware PCB used is the "FAB_xv11_lidar_dev_2016_01_24" EagleCAD file located in the Hardware folder.

Note: you need to use an FTDI chip to debug the project.
Look at the 6 pin (looks like Arduino ISP header. 
FTDI board rx pin goes to Uart 6 on pic32
Ground goes to ground on the same 6 pin header

Next, run a serial debugger program on your pc like realterm, teraterm, arduino to debug the UART line from the pic32
Also double check the lidar is connected.
The headers are loose on the pic32 header for UART4. hook lidar brown/yellow wire (Lidar tx) to the pic32 (UART4RX). Hook up the lidar power and ground.

The MOSFET and Diode that are being pulsed use the circuit in the included PDF: "Lidar Development Using PIC32MX Microcontroller"
Next hook up the motor power to the +5V pin next to the air wired mosfet.
YOU WILL HAVE TO REPROGRAM THE pic32 when you first power it on with the pickit3 programmer. The pic32 will stop spinning the lidar until the pic32 is re-programmed.



I recommend you make the following changes for the Wii Camera/Lidar board.
1) Add 3.3V pin for Wii Cameras
2) Choose a pic32MX with more independent timers (1 or 2 more 16 bit timers) - the pic32 only has 4 independent timers
3) Switching buck converters (switching down power converters) to help with battery efficiency
4) Add more output capacitance for voltage LDO regulators if you do not use a switching regulator
5) Beef up 3.3V trace on board
6) Place 5V power for lidar next to lidar input pins (place +5V next to UA4RX)

7) Choose an isolated mosfet driver (the one we used was not isolated (grounds were tied – too much noise)
8) Integrate a large mosfet onto the board and use the mosfet driver with it with the attached circuit (see attachment)
9) Choosing locking connectors OR solder all wires to board
10) Move programming header closer to pic32 and avoid meandering traces under & over board

Recommendations:
11) Add a non-ultrasonic distance meter to help with backing up the robot
12) Get ultra-reflective surfaces for TRACEE to help with LIDAR (Aluminum reflectance varied too much from 70 to 130 quality)
13) Use a Gimbal/gyro on the Lidar to help keep it level with arena sand
14) Lower Lidar to sit in the arena like I left it (this will prevent false positives from the judges
15) Look into dust tolerance methods for Lidar (static electricity/magnetism/sound vibrations/solenoid)
