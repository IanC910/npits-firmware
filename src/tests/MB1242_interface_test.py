
import time
import sys

sys.path.append('../')

import smbus
from gpiozero import DigitalInputDevice

import pin_defines

address = 0x70

i2c_channel = 1
i2c_bus = smbus.SMBus(i2c_channel)

status_gpio = DigitalInputDevice(17)

while(True):
    i2c_bus.write_byte(address, 81)

    # is_data_ready = False
    # while(not is_data_ready):
    #     try:
    #         bytes = i2c_bus.read_i2c_block_data(address, 0, 2)
    #         is_data_ready = True
    #     except:
    #         time.sleep(0.01)

    while(status_gpio.is_active):
        time.sleep(0.01)

    bytes = i2c_bus.read_i2c_block_data(address, 0, 2)

    distance_cm = bytes[0] * 256 + bytes[1]
    print(distance_cm)

    time.sleep(0.1)