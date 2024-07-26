
# This test tries to get device info from the K-LD2 doppler radar via
# via the rpi's ttyS0 serial device

import serial
from time import sleep

def doppler_interface_test():
    print("Doppler Interface Test")

    doppler_serial = serial.Serial("/dev/ttyS0", 38400)

    doppler_serial.write(b'$S06\x0D')
    # Should return '@S0601\r\n' by default

    sleep(0.1)

    num_received_bytes = doppler_serial.inWaiting()

    received_data = doppler_serial.read(num_received_bytes)

    print(received_data)


if(__name__ == "__main__"):
    doppler_interface_test()

