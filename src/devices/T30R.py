#!/usr/bin/env python
import iolhat
import time
import struct


PORT = 0
VENDOR_STRING_INDEX = 16
SENSOR_DATA_INDEX = 67
SENSOR_DATA_SUBINDEX = 1
SENSOR_DATA_LENGTH = 4  # 32-bit integer (4 bytes)


def main():

    # Set POWER ON for port 1
    try:
        iolhat.power(PORT, 1)
        iolhat.led(PORT, iolhat.LED_GREEN)
    except Exception as e:
        print(f"Power/LED error: {e}")
        iolhat.led(PORT, iolhat.LED_RED)

    # Read vendor name from index 16
    try:
        data = iolhat.read(PORT, VENDOR_STRING_INDEX, 0x00, 64)  # Vendor Name string, expected to be up to 64 bytes
        vendor_name = data.decode('utf-8').strip('\x00')
        print(f"Vendor Name: {vendor_name}")
    except Exception as e:
        print(f"Read exception: {e}")
        iolhat.led(PORT, iolhat.LED_RED)

    # Continuously read sensor measurement data
    while True:
        try:
            # Read sensor data (32-bit integer) from index 67, subindex 1
            sensor_data = iolhat.read(PORT, SENSOR_DATA_INDEX, SENSOR_DATA_SUBINDEX, SENSOR_DATA_LENGTH)
            sensor_value = struct.unpack('!I', sensor_data)[0]  # Unpack as big-endian 32-bit integer
            print(f"Sensor Measurement Value: {sensor_value}")
        except Exception as e:
            print(f"Sensor read exception: {e}")
            iolhat.led(PORT, iolhat.LED_RED)


if __name__ == '__main__':
    main()

