This lab focuses on the controlling of proximity sensors and motor drivers (H-Bridges) using
interrupts. In addition to the core project, the group has decided to go above and beyond by
adding remote control and autonomy capabilities to the tank. The tank can be remotely
controlled by a smartphone or by using Leap Motion Camera

HandGestures.py: This is the python script that handles the gesture controls using the
Leap Motion device by reading the frame, finding the hand, and interpreting what the
gesture means based on palm direction and fist size

UART_SETUP.py: This is the python script that sets up the uart serial port for bluetooth

uart_drive.c: Configures the bluetooth after calling UART_SETUP.py and reads instructions from
the bluetooth port and puts them in the fifo for motor.c

motor.c (motor.h): Sets up GPIO ports and directions, and has a main loop where it controlls
the motors, either by reading instructions from the fifo or doing autonomous controlls
based on interrupts and sensor data