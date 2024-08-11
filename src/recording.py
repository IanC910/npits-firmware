import os
import shutil
import threading
from time import sleep
from devices.RearCameraModule import RearCameraModule
import multiprocessing

def run_recording_process(near_pass_id_queue : multiprocessing.Queue):
    print("Starting Recording Process...")

    camera_module = RearCameraModule(output_folder="/home/pi/output_folder", queue_size=10)

    recording = threading.Thread(target=recording_thread, args=(camera_module,))
    flagging = threading.Thread(target=flagging_thread, args=(camera_module, near_pass_id_queue,))

    recording.start()
    flagging.start()

    recording.join()
    flagging.join()

def recording_thread(camera_module):
    camera_module.start_recording_loop()

def flagging_thread(camera_module, near_pass_id_queue):
    while True:
        if not near_pass_id_queue.empty():
            near_pass_id = near_pass_id_queue.get()
            if near_pass_id is not None:
                print(f"Near pass detected with ID: {near_pass_id}")
                camera_module.near_pass_id = near_pass_id
                camera_module.trigger_flagging()
            else:
                continue

        sleep(0.1)  # Polling interval


