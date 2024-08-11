
import os
import shutil
import RPi.GPIO as GPIO # TODO change to gpiozero (RPI.GPIO doesn't work on pi5)

import pin_defines
import bluetooth.init_bluetooth
import bluetooth.obex

from near_pass_detection import run_near_pass_detector
from devices import RearCameraModule

class NPITS():

    DEVICES_FILE = "./bluetooth/devices.txt"

    def __init__(self):
        
        # Init GPIO
        GPIO.setmode(GPIO.BCM)
        GPIO.setup(pin_defines.POWER_LED_GPIO, GPIO.OUT)

        # Turn power LED on
        GPIO.output(pin_defines.POWER_LED_GPIO, GPIO.HIGH)

        # Start bluetooth and pair with phone
        bluetooth.init_bluetooth.init_bluetooth_and_pair(self.DEVICES_FILE)

        # Connect the obex channel
        bluetooth.obex.obex_init()
        

    def run(self):
        camera_module = RearCameraModule.RearCameraModule(output_folder="/home/pi/output_folder", queue_size=10)

        near_pass_id_queue = multiprocessing.Queue()

        # Start recording process
        recording_process = multiprocessing.Process(target=run_recording_process, args=(camera_module, near_pass_id_queue,))
        recording_process.start()

        # run_near_pass_detector()

        # Cleanup
        recording_process.join()

if __name__ == "__main__":
    passer = NPITS()
    passer.run()
