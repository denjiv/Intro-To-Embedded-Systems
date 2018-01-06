This lab focuses on the development of a vehicle that utilizes both the
Bluetooth chip as well as startup scripts to allow the BeagleBone to
automatically execute vehicle firmware. In addition to the core project,
the group has decided to implement lane detection feature for the vehicle
which enables the vehicle to drive within a given lane.

UART_SETUP.py: This is the python script that sets up the uart serial port for bluetooth

uart_drive.c: Configures the bluetooth after calling UART_SETUP.py and reads instructions from
the bluetooth port and puts them in the fifo for motor.c

pi_uart.c: Initialize UART on Raspberry Pi and send direction signal to BeagleBone

uart_drive_pi.c: Initialize the UART on BeagleBone that is connected to the UART on Pi

motor.c (motor.h): Sets up GPIO ports and directions, and has a main loop where it controlls
the motors, either by reading instructions from the fifo or doing line following
from the commands that it gets from lines.cpp and the uart connection

lines.cpp: Uses OpenCV to detect lines in camera frame and dispatch a command
based on that data through uart to the BeagleBone