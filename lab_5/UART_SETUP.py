import Adafruit_BBIO.UART as UART
import serial
# Setup UART
UART.setup("UART4")
UART.setup("UART1")
print("UART4 is setup\n")
print("UART1 is setup\n")
ser_4 = serial.Serial(port = "/dev/ttyO4", baudrate=115200)
ser_1 = serial.Serial(port = "/dev/ttyO1", baudrate=115200)
ser_4.close()
ser_1.close()
ser_4.open()
ser_1.open()

