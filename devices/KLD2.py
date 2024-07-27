
# This module provides interfacing methods for the K-LD2 Doppler Radar

import serial
from enum import Enum

class KLD2_Status(Enum):
    OK = 0
    ERROR = 1


class KLD2:
    BAUD_RATE = 38400
    RESPONSE_PREFIX = ord('@')

    def __init__(self, uart_device):
        self.serial = serial.Serial(uart_device, KLD2.BAUD_RATE)

    def get_sensitivity_pot_index(self):
        self.serial.write(b'$R06\r')
        response = self.serial.readline()

        if(response[0] == KLD2.RESPONSE_PREFIX):
            response = response.decode('utf-8')
            pot_index = int(response[4:6])
            return KLD2_Status.OK, pot_index

        return KLD2_Status.ERROR, response
