
import time
import sys
import bluetooth.init_bluetooth
import bluetooth.BLEInterface
from gpiozero import CPUTemperature

sys.path.append('../')

import bluetooth

def ble_interface_test():
    print("BLE Interface Test")

    devices_file = "../ble/devices.txt"
    bluetooth.init_bluetooth.init_bluetooth_and_pair(devices_file)
    phone_ble = bluetooth.BLEInterface.BLEInterface()

    print("Temp update started")
    while True:
        message = "CPU TEMP: " + str(CPUTemperature().temperature)
        print(message)
        phone_ble.write(0, message)
        time.sleep(5)

if(__name__ == "__main__"):
    ble_interface_test()