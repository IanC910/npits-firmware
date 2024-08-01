
import time
import sys

sys.path.append('../')

import pin_defines
from devices.KLD2 import KLD2, KLD2_Status, KLD2_Param

def KLD2_set_sensitivity_test():
    print("K-LD2 Set Sensitivity Test")

    doppler = KLD2(pin_defines.KLD2_UART_DEVICE)

    SENSITIVITY_MAX = 9
    counter = 0
    while True:
        status, sensitivity = doppler.set_param(KLD2_Param.SENSITIVITY, counter)
        if(status == KLD2_Status.OK):
            print(sensitivity)
        else:
            print(status.name)

        time.sleep(1)

        counter = (counter + 1) % (SENSITIVITY_MAX + 1)

if(__name__ == "__main__"):
    KLD2_set_sensitivity_test()