import time
import cv2
from picamera2 import Picamera2

# Path to the HaarCascade XML file for car detection
cascade_path = './haarcascade_car.xml'  # Ensure this file exists and is valid
cascade = cv2.CascadeClassifier(cascade_path)

# Function to detect objects and check if a car is approaching
def detect_and_display(frame, prev_bounding_boxes):
    # Convert the frame to grayscale for detection
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    # Detect objects in the grayscale image
    objects = cascade.detectMultiScale(gray, scaleFactor=1.1, minNeighbors=5, minSize=(30, 30))

    for (x, y, w, h) in objects:
        # Draw bounding boxes around detected objects
        cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)

        # Check if the car is approaching by comparing the width of the bounding box to previous frames
        approaching = False
        if len(prev_bounding_boxes) > 0:  # Check if previous bounding boxes exist
            for (px, py, pw, ph) in prev_bounding_boxes:
                if abs(w - pw) > 10 and w > pw:  # Car is approaching if the width increases significantly
                    approaching = True

        # Display 'Car Approaching' if detected
        if approaching:
            cv2.putText(frame, "Car Approaching!", (x, y - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.9, (0, 0, 255), 2)

    # Display the frame with the bounding boxes
    cv2.imshow('Car Detection', frame)

    # Return the current bounding boxes for use in the next frame
    return objects

# Main function to capture video from Picamera2 and detect objects
def main():
    # Initialize Picamera2
    picam2 = Picamera2()

    # Configure the camera for video capture
    config = picam2.create_preview_configuration(main={"size": (640, 480)})
    picam2.configure(config)

    # Start the camera
    picam2.start()

    prev_bounding_boxes = []  # Store bounding boxes from the previous frame

    try:
        while True:
            # Capture a frame from Picamera2
            frame = picam2.capture_array()

            # Perform object detection on the frame
            prev_bounding_boxes = detect_and_display(frame, prev_bounding_boxes)

            # Press 'q' to quit the program
            if cv2.waitKey(1) & 0xFF == ord('q'):
                break

            time.sleep(0.1)  # Add a slight delay to reduce CPU usage

    finally:
        # Stop the camera and close all OpenCV windows
        picam2.stop()
        cv2.destroyAllWindows()

if __name__ == '__main__':
    main()

