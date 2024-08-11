import os
import shutil
import multiprocessing
from gpiozero import LED # Import LED from gpiozero

import pin_defines
import bluetooth.init_bluetooth
import bluetooth.obex
from recording import run_recording_process

from near_pass_detection import run_near_pass_detector
from devices import RearCameraModule

class NPITS():

    DEVICES_FILE = "./bluetooth/devices.txt"

    def __init__(self):
        # Initialize the power LED using gpiozero
        # self.power_led = LED(pin_defines.POWER_LED_GPIO)

        # Turn power LED on
        # self.power_led.on()

        # Start bluetooth and pair with phone
        bluetooth.init_bluetooth.init_bluetooth_and_pair(self.DEVICES_FILE)

        # Connect the obex channel
        # bluetooth.obex.obex_init()

    def run(self):

        near_pass_id_queue = multiprocessing.Queue()

        # Start recording process
        # recording_process = multiprocessing.Process(target=run_recording_process, args=(near_pass_id_queue,))
        # recording_process.start()

        run_near_pass_detector(near_pass_id_queue)

        # Cleanup
        #  recording_process.join()

