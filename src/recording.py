import os
import shutil
import threading
from time import sleep

import multiprocessing

def run_recording_process(near_pass_id_queue: multiprocessing.Queue):
    print("Starting Recording Process...")
    # use RearCameraModule here
    # spawn 2 threads, one for flagging, one for recording

