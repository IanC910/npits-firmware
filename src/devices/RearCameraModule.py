from contextlib import contextmanager

@contextmanager
def camera_context(output_folder, queue_size=10):
    camera = Picamera2()
    encoder = H264Encoder()
    try:
        yield camera, encoder
    finally:
        camera.close()
        print("Camera closed.")

class RearCameraModule:
    def __init__(self, output_folder, queue_size=10):
        self.output_folder = output_folder
        self.queue_size = queue_size
        self.current_index = 1  # Start with video1
        self.recording_length = 10  # Length of each video recording (in seconds)
        self.number_of_saved_recordings = 3  # Number of recordings to save
        self.incident_number = 1  # Current incident number
        self.trigger_flag = threading.Event()  # Use threading.Event for flagging

    def start_recording_loop(self):
        with camera_context(self.output_folder, self.queue_size) as (camera, encoder):
            while True:
                output_file = os.path.join(self.output_folder, f"general/video{self.current_index}.h264")
                camera.configure(camera.create_video_configuration())
                camera.start_recording(encoder, output_file)
                print(f"Recording started: {output_file}")
                sleep(self.recording_length)
                camera.stop_recording()
                print(f"Recording stopped and saved as {output_file}")

                # Increment the index, and wrap around if it exceeds queue_size
                self.current_index += 1
                if self.current_index > self.queue_size:
                    self.current_index = 1  # Reset to video1

    def flag_recording(self):
        while True:
            # Wait until the trigger flag is set
            self.trigger_flag.wait()

            index = self.current_index

            incident_folder = os.path.join(self.output_folder, f"recording_{self.incident_number}")
            if not os.path.exists(incident_folder):
                os.makedirs(incident_folder)

            for i in range(self.number_of_saved_recordings):
                source = os.path.join(self.output_folder, f"general/video{index}.h264")
                destination = os.path.join(incident_folder, f"incident_number_{self.incident_number}_video{index}.h264")

                # Move the file if it exists
                if os.path.exists(source):
                    shutil.move(source, destination)
                    print(f"Moved video from {source} to {destination}")
                else:
                    print(f"File not found: {source}")

                index = index - 1
                if index == 0:
                    index = self.queue_size

            # Reset the trigger flag
            self.trigger_flag.clear()

    def trigger_flagging(self):
        # Set the trigger flag to start flagging recordings
        self.trigger_flag.set()
