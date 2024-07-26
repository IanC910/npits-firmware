
# This test sends data from the rpi's ttyS0 device (uart/serial) to itself
# Connect pin 8 (TXD) to pin 10 (RXD)


import serial
from time import sleep

def interface_test():
    print("Interface Test")

    serial_device = serial.Serial("/dev/ttyS0", 38400)

    while True:
        serial_device.write(b't')

        sleep(0.01)

        received_data = serial_device.read()

        print(received_data.decode('utf-8'))

        sleep(1)

if(__name__ == "__main__"):
    interface_test()