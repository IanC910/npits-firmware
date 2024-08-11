
import os
import shutil
import RPi.GPIO as GPIO # TODO change to gpiozero (RPI.GPIO doesn't work on pi5)

import pin_defines
import bluetooth.init_bluetooth
import bluetooth.obex

# from picamera2 import Picamera2
# from picamera2.encoders import H264Encoder

from near_pass_detection import run_near_pass_detector

class NPITS():

    DEVICES_FILE = "./bluetooth/devices.txt"



    def __init__(self):

        # Init GPIO
        GPIO.setmode(GPIO.BCM)
        # GPIO.setup(pin_defines.POWER_LED_GPIO, GPIO.OUT)

        # Turn power LED on
        # GPIO.output(pin_defines.POWER_LED_GPIO, GPIO.HIGH)

        # Start bluetooth and pair with phone
        # bluetooth.init_bluetooth.init_bluetooth_and_pair(self.DEVICES_FILE)

        # Connect the obex channel
        # bluetooth.obex.obex_init()



    def run(self):

        # Start recording process

        run_near_pass_detector()

        # Start

        # # Initialize the camera module and BLE interface
        # camera_module = RearCameraModule(output_folder="/home/pi/output_folder", queue_size=10)

        # # Start threads for continuous operation
        # # 1. Recording thread: Continuously records 10-second video clips in a circular queue
        # recording_thread = threading.Thread(target=camera_module.start_recording_loop)

        # # 2. Flagging thread: Saves flagged recordings when a near pass is detected
        # flagging_thread = threading.Thread(target=camera_module.flag_recording)

        # # 3. Detection thread: Monitors for near pass events and triggers actions (e.g., flagging, BLE communication)
        # detection_thread = threading.Thread(target=passer.near_pass_detector, args=(camera_module))

        # # Start the threads
        # recording_thread.start()
        # flagging_thread.start()
        # detection_thread.start()

        # # Ensure threads run indefinitely
        # recording_thread.join()
        # flagging_thread.join()
        # detection_thread.join()

