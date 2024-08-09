import os
from time import sleep
from datetime import datetime
from picamera2 import Picamera2
from picamera2.encoders import H264Encoder

class RearCameraModule:
    def __init__(self, output_folder):
        self.output_folder = output_folder
        if not os.path.exists(output_folder):
            os.makedirs(output_folder)
        self.camera = Picamera2()
        self.encoder = H264Encoder()

    def record_video(self, go_ahead):
        if go_ahead:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            output_file = os.path.join(self.output_folder, f"video_{timestamp}.h264")
            self.camera.configure(self.camera.create_video_configuration())
            self.camera.start_recording(self.encoder, output_file)
            print(f"Recording started: {output_file}")
            sleep(10)
            self.camera.stop_recording()
            print(f"Recording stopped and saved as {output_file}")
        else:
            print("Recording not started. Go-ahead signal is False.")
# Uncomment for Example usage
#if __name__ == "__main__":
#    camera_module = RearCameraModule("/home/pi/output")
#    go_ahead = True  # Set this to True to start recording
#    camera_module.record_video(go_ahead)
