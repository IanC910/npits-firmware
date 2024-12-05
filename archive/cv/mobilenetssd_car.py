# Import the OpenCV extra functionalities
import cv2
import os
from picamera2 import Picamera2

# Find user
users  = []
users.append(os.getlogin())

# Pull the information about what each object is called
classNames = []
classFile = "/home/" + users[0] + "/Desktop/Object_Detection_Files/coco.names"
with open(classFile, "rt") as f:
    classNames = f.read().rstrip("\n").split("\n")

# Pull the information about what each object should look like
configPath = "/home/" + users[0] + "/Desktop/Object_Detection_Files/ssd_mobilenet_v3_large_coco_2020_01_14.pbtxt"
weightsPath = "/home/" + users[0] + "/Desktop/Object_Detection_Files/frozen_inference_graph.pb"

# Set up values for detection
net = cv2.dnn_DetectionModel(weightsPath, configPath)
net.setInputSize(320, 320)
net.setInputScale(1.0 / 127.5)
net.setInputMean((127.5, 127.5, 127.5))
net.setInputSwapRB(True)

# Function to draw the detection boxes and labels
def getObjects(img, thres, nms, draw=True, objects=[]):
    classIds, confs, bbox = net.detect(img, confThreshold=thres, nmsThreshold=nms)

    if len(objects) == 0:
        objects = classNames
    objectInfo = []

    if len(classIds) != 0:
        for classId, confidence, box in zip(classIds.flatten(), confs.flatten(), bbox):
            className = classNames[classId - 1]
            if className in objects:
                objectInfo.append([box, className])
                if draw:
                    cv2.rectangle(img, box, color=(0, 255, 0), thickness=2)
                    cv2.putText(img, classNames[classId-1].upper(), (box[0]+10, box[1]+30),
                                cv2.FONT_HERSHEY_COMPLEX, 1, (0, 255, 0), 2)
                    cv2.putText(img, str(round(confidence * 100, 2)), (box[0] + 200, box[1] + 30),
                                cv2.FONT_HERSHEY_COMPLEX, 1, (0, 255, 0), 2)

    return img, objectInfo

# List of vehicle-related objects to detect
vehicle_objects = ["car", "motorcycle", "bus", "truck", "bicycle"]

# Below determines the size of the live feed window that will be displayed on the Raspberry Pi OS
if __name__ == "__main__":
    # Start Pi camera
    picam2 = Picamera2()
    picam2.configure(picam2.create_preview_configuration(main={"format": 'XRGB8888', "size": (640, 480)}))
    picam2.start()

    # Loop to display the live feed and detect vehicles
    while True:
        # Get an image from Pi camera
        img = picam2.capture_array("main")
        img = cv2.cvtColor(img, cv2.COLOR_BGRA2BGR)

        # Detect only vehicles (threshold: 0.45, NMS: 0.2)
        result, objectInfo = getObjects(img, 0.45, 0.2, objects=vehicle_objects)

        # Display the live output
        cv2.imshow("Vehicle Detection", img)

        k = cv2.waitKey(200)
        if k == 27:  # Esc key to stop
            # Exit the loop and close the camera
            picam2.stop()
            cv2.destroyAllWindows()
            break
