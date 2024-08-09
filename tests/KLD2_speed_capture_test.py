
import time
import sys

sys.path.append('../')

import pin_defines
from devices.KLD2 import KLD2, KLD2_Status, KLD2_Param

def KLD2_speed_capture_test():
    print("K-LD2 Speed Capture Test")

    doppler = KLD2(pin_defines.KLD2_UART_DEVICE)

    status, sampling_rate = doppler.guarantee_set_param(KLD2_Param.SAMPLING_RATE, 6)
    print('Sampling Rate Setting: ' + str(sampling_rate))

    status, use_pot = doppler.guarantee_set_param(KLD2_Param.USE_SENSITIVITY_POT, 1)
    print('Use pot: ' + str(use_pot))

    counter = 0
    while True:
        if(counter == 0):
            status, pot_index = doppler.try_get_param(KLD2_Param.SENSITIVITY_POT_INDEX)
            print("Pot index: " + str(pot_index))

        status, target_list = doppler.try_get_target_list()
        if(status == KLD2_Status.OK):
            inbound_speed_kmph = target_list[0]
            outbound_speed_kmph = target_list[1]
            print("Inbound: %8.1f   kmph, Outbound: %8.1f   kmph" % (inbound_speed_kmph, outbound_speed_kmph))
        else:
            print(status.name)

        time.sleep(0.1)

        counter = (counter + 1) % 10

if(__name__ == "__main__"):
    KLD2_speed_capture_test()