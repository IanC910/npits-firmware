import os
import shutil
import multiprocessing
from gpiozero import LED

import pin_defines
import bluetooth.init_bluetooth

from recording import run_recording_process
from near_pass_detection import run_near_pass_detector

class NPITS():

    DEVICES_FILE = "./bluetooth/devices.txt"

    def __init__(self):
        # Initialize the power LED using gpiozero
        # self.power_led = LED(pin_defines.POWER_LED_GPIO)

        # Turn power LED on
        # self.power_led.on()

        # Start bluetooth and pair with phone
        bluetooth.init_bluetooth.init_bluetooth_and_pair(self.DEVICES_FILE)

    def run(self):

        near_pass_id_queue = multiprocessing.Queue()

        # Start recording process
        recording_process = multiprocessing.Process(target=run_recording_process, args=(near_pass_id_queue,))
        near_pass_process = multiprocessing.Process(target=run_near_pass_detector, args=(near_pass_id_queue,))

        try:
            recording_process.start()
            near_pass_process.start()

            recording_process.join()
            near_pass_process.join()
        except KeyboardInterrupt:
            print("Main process interrupted with keyboard")
            print("Terminating all child processes")
            recording_process.terminate()
            near_pass_process.terminate()

            recording_process.join()
            near_pass_process.join()
        finally:
            print("All processes have been terminated")


