#!/usr/bin/python3
import time
import cv2
import sys
import termios
import tty
from picamera2 import MappedArray, Picamera2
from picamera2.encoders import H264Encoder

def apply_timestamp(request):
    """Overlay a timestamp on the current frame."""
    timestamp = time.strftime("%Y-%m-%d %X")
    with MappedArray(request, "main") as m:
        cv2.putText(m.array, timestamp, (0, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)

def check_for_exit():
    """Check for 'q' key press to exit gracefully."""
    fd = sys.stdin.fileno()
    old_settings = termios.tcgetattr(fd)
    try:
        tty.setcbreak(fd)
        while True:
            if sys.stdin.read(1) == 'q':
                return
    finally:
        termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)

def main(output_filename='test.h264'):
    """Main function to record a video with a timestamp overlay and exit on 'q' key press."""
    # Initialize the camera
    picam2 = Picamera2()

    # Configure the camera for video recording
    picam2.configure(picam2.create_video_configuration())

    # Set the callback to apply the timestamp overlay
    picam2.pre_callback = apply_timestamp

    # Initialize the H264 encoder
    encoder = H264Encoder(10000000)

    # Start recording with the timestamp overlay
    print(f"Recording started. Saving to {output_filename}")
    picam2.start_recording(encoder, output_filename)

    try:
        # Continuously record until 'q' is pressed
        print("Press 'q' to stop recording.")
        check_for_exit()

    finally:
        # Stop recording and clean up resources
        picam2.stop_recording()
        picam2.close()
        print(f"Recording stopped. Video saved as {output_filename}")

if __name__ == "__main__":
    # Call the main function
    main(output_filename="timed_video.h264")
