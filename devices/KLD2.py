
# This module provides interfacing methods for the K-LD2 Doppler Radar

import serial

KLD2_BAUD_RATE = 38400

class KLD2:
    def __init__(uart_device):
        self.serial = serial.Serial(uart_device, KLD2_BAUD_RATE)

    def get_sensitivity_pot_index(self):
        self.serial.write(b'$R06\r')
        pot_index = self.serial.read(1)
        return pot_index
