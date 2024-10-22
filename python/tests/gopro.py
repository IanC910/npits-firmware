import requests
import time
import os

# Set GoPro Wi-Fi credentials and camera IP
gopro_ip = "10.5.5.9"
password = "cmH-kvV-HFC"  # If password is set

# Start recording
start_url = f"http://{gopro_ip}/gp/gpControl/command/shutter?p=1"
requests.get(start_url)
print("Recording started...")

# Record for 10 seconds
time.sleep(10)

# Stop recording
stop_url = f"http://{gopro_ip}/gp/gpControl/command/shutter?p=0"
requests.get(stop_url)
print("Recording stopped.")

# Wait to ensure media is available
time.sleep(5)

# Get media list
media_url = f"http://{gopro_ip}/gp/gpMediaList"
media_list = requests.get(media_url).json()

# Print media list for debugging
print(media_list)

# Find the latest video
if 'media' in media_list and media_list['media']:
    latest_media = media_list['media'][-1]
    folder = latest_media['d']
    filename = latest_media['fs'][-1]['n']
    video_url = f"http://{gopro_ip}:8080/videos/DCIM/{folder}/{filename}"

    # Download the video
    video_response = requests.get(video_url)
    with open(filename, 'wb') as video_file:
        video_file.write(video_response.content)

    print(f"Video {filename} downloaded successfully!")
else:
    print("No media found.")