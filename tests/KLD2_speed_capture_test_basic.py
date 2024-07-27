
# This test tries to get device info from the K-LD2 doppler radar via
# via the rpi's ttyS0 serial device

import serial
from time import sleep

def doppler_speed_capture_test_basic():
    print("Doppler Speed Capture Test Basic")

    doppler_serial = serial.Serial("/dev/ttyS0", 38400)

    doppler_serial.write(b'$S04\r')
    sleep(0.1)
    num_received_bytes = doppler_serial.inWaiting()
    received_data = doppler_serial.read(num_received_bytes)
    print(received_data)

    counter = 0

    while True:

        doppler_serial.write(b'$C01\x0D')
        sleep(0.1)
        num_received_bytes = doppler_serial.inWaiting()
        received_data = doppler_serial.read(num_received_bytes)

        counter += 1
        if(counter == 5):
            print('s')
            counter = 0

        print(received_data)

        sleep(0.1)

if(__name__ == "__main__"):
    doppler_speed_capture_test_basic()