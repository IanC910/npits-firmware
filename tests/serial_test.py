
# This test sends data from the rpi's ttyS0 serial device to itself
# Connect pin 8 (TXD) to pin 10 (RXD)

import serial
from time import sleep

def serial_test():
    print("Serial Test")

    serial_device = serial.Serial("/dev/ttyS0", 38400)

    while True:
        serial_device.write(b's')

        sleep(0.01)

        received_data = serial_device.read()

        print(received_data.decode('utf-8'))
        # Should print 's' repeatedly

        sleep(1)

if(__name__ == "__main__"):
    serial_test()