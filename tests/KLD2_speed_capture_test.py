
import time
import sys

sys.path.append('../')

import pin_defines
from devices.KLD2 import KLD2, KLD2_Status

def KLD2_speed_capture_test():
    print("K-LD2 Speed Capture Test")

    doppler = KLD2(pin_defines.KLD2_UART_DEVICE)

    while True:
        status, target_list = doppler.get_target_list()
        if(status == KLD2_Status.OK):
            inbound_speed_kmph, outbound_speed_kmph = doppler.extract_speeds_kmph(target_list)
            print("Inbound: %8.1f   kmph, Outbound: %8.1f   kmph" % (inbound_speed_kmph, outbound_speed_kmph))
        else:
            print(status.name)

        time.sleep(0.1)

if(__name__ == "__main__"):
    KLD2_speed_capture_test()