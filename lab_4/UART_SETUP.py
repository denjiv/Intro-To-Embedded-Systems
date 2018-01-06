import Adafruit_BBIO.UART as UART
import serial
# Setup UART
UART.setup("UART4")
print("UART4 is setup\n")
ser = serial.Serial(port = "/dev/ttyO4", baudrate=115200)
ser.close()
ser.open()
