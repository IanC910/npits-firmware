import os
import shutil
import threading
import RPi.GPIO as GPIO
import time
from datetime import datetime

from ble.BLEInterface import BLEInterface
from devices.RearCameraModule import RearCameraModule
from devices import HCSR04, KLD2
from ble import ble

from picamera2 import Picamera2
from picamera2.encoders import H264Encoder

# GPIO pin definitions
POWER_LED_GPIO_PIN = 17
BLUETOOTH_LED_GPIO_PIN = 27  # GPIO pin for Bluetooth LED

# File path for paired devices
devices_file = "../ble/devices.txt"

class NPITS:
    def __init__(self):
        self.near_pass_detected = False
        self.incident_number = 1

    @staticmethod
    def toggle_led(pin: int, state: bool):
        """
        Toggles the state of an LED connected to the specified GPIO pin.

        Args:
            pin (int): GPIO pin number.
            state (bool): True to turn the LED on, False to turn it off.
        """
        GPIO.setmode(GPIO.BCM)
        GPIO.setup(pin, GPIO.OUT)
        GPIO.output(pin, GPIO.HIGH if state else GPIO.LOW)

    def connect_to_phone(self):  # WIP: Add actual logic here
        """
        Attempts to connect to a phone via Bluetooth.

        Returns:
            bool: True if connection is successful, False otherwise.
        """
        phone_connected_successfully = False  # Placeholder for actual connection logic

        return phone_connected_successfully

    def near_pass_detector(self, camera_module, ble_interface):
        """
        Continuously monitors for a near pass event and handles it when detected.

        Args:
            camera_module (RearCameraModule): The camera module for recording and flagging.
            ble_interface (BLEInterface): The BLE interface for communication with the phone.
        """
        while True:
            # Replace PACKAGE_NAME with the actual module or class providing near_pass_detector
            self.near_pass_detected = PACKAGE_NAME.near_pass_detector()
            
            if self.near_pass_detected:
                camera_module.trigger_flagging()
                camera_module.video_saved.wait()

                # Construct and send a message with event details
                message = f"Time: {datetime.now()}, Speed: {PACKAGE.get_speed()}, Distance: {PACKAGE.get_distance()}"
                ble_interface.write(0, message)

                # Pause briefly before sending video
                time.sleep(5)

                # Send the recorded video file to the phone
                ble.send_video.sendfileobex(
                    4, f"/home/pi/output_folder/recording_incident_{self.incident_number}/video_{self.incident_number}.h264"
                )
                self.incident_number += 1


if __name__ == "__main__":
    # Turn on the power LED at startup
    passer = NPITS()
    passer.toggle_led(pin=POWER_LED_GPIO_PIN, state=True)
    
    # Initialize the camera module and BLE interface
    camera_module = RearCameraModule(output_folder="/home/pi/output_folder", queue_size=10)
    ble_interface = BLEInterface(devices_file)  # Starts a Bluetooth server process

    # Check if a smartphone is successfully paired
    if not passer.connect_to_phone():
        print("Could not pair with a smartphone device!")
        exit()
    else:
        print("Smartphone device paired successfully. Begin riding!")

    # Start threads for continuous operation
    # 1. Recording thread: Continuously records 10-second video clips in a circular queue
    recording_thread = threading.Thread(target=camera_module.start_recording_loop)

    # 2. Flagging thread: Saves flagged recordings when a near pass is detected
    flagging_thread = threading.Thread(target=camera_module.flag_recording)

    # 3. Detection thread: Monitors for near pass events and triggers actions (e.g., flagging, BLE communication)
    detection_thread = threading.Thread(target=passer.near_pass_detector, args=(camera_module, ble_interface))

    # Start the threads
    recording_thread.start()
    flagging_thread.start()
    detection_thread.start()

    # Ensure threads run indefinitely
    recording_thread.join()
    flagging_thread.join()
    detection_thread.join()
