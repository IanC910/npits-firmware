
import time
import sys
from gpiozero import CPUTemperature

sys.path.append('../')

from ble.BLE_Interface import BLE_Interface

def ble_interface_test():
    print("BLE Interface Test")

    phone_ble = BLE_Interface()

    print("Temp update started")
    while True:
        message = "CPU TEMP: " + str(CPUTemperature().temperature)
        print(message)
        phone_ble.write(0, message)
        time.sleep(5)

if(__name__ == "__main__"):
    ble_interface_test()