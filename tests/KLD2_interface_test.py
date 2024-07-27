
import time
import sys

sys.path.append('../')

import pin_defines
from devices.KLD2 import KLD2, KLD2_Status

def KLD2_interface_test():
    print("K-LD2 Interface Test")

    doppler = KLD2(pin_defines.KLD2_UART_DEVICE)

    while True:
        status, pot_index = doppler.get_sensitivity_pot_index()
        if(status == KLD2_Status.OK):
            print(pot_index)
        else:
            print(status.name)

if(__name__ == "__main__"):
    KLD2_interface_test()