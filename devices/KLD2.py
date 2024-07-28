
# This module provides interfacing methods for the K-LD2 Doppler Radar

import serial
from enum import Enum

class KLD2_Status(Enum):
    OK = 0
    ERROR = 1

class KLD2:
    BAUD_RATE = 38400
    RESPONSE_PREFIX = '@'
    ERROR_CLASS = 'E'

    def __init__(self, uart_device):
        self.serial = serial.Serial(uart_device, KLD2.BAUD_RATE)

        status = KLD2_Status.ERROR
        while(status == KLD2_Status.ERROR):
            status, self.sampling_rate_Hz = self.get_sampling_rate_Hz()



    def decode_response(self, response):
        try:
            decoded_response = response.decode('utf-8')
        except UnicodeDecodeError:
            return KLD2_Status.ERROR, response

        return KLD2_Status.OK, decoded_response



    def send_command_with_1_byte_response(self, command):
        self.serial.write(command.encode('utf-8'))
        response = self.serial.readline()

        status, decoded_response = self.decode_response(response)
        if(status != KLD2_Status.OK):
            return status, response

        if(decoded_response[0] != KLD2.RESPONSE_PREFIX):
            return KLD2_Status.ERROR, response

        if(decoded_response[1] == KLD2.ERROR_CLASS):
            return KLD2_Status.ERROR, response

        return_val = int(decoded_response[4:6])
        return KLD2_Status.OK, return_val



    def get_sampling_rate_Hz(self):
        command = '$S04\r'
        status, response = self.send_command_with_1_byte_response(command)

        if(status != KLD2_Status.OK):
            return KLD2_Status.ERROR, response

        sampling_rate_Hz = 1280 * response
        return status, sampling_rate_Hz



    def get_sensitivity_pot_index(self):
        command = '$R06\r'
        return self.send_command_with_1_byte_response(command)



    def get_targets(self):
        command = '$C01\r'
        self.serial.write(command.encode('utf-8'))
        response = self.serial.readline()

        status, decoded_response = self.decode_response(response)
        if(status != KLD2_Status.OK):
            return status, response

        inbound_speed_bin       = int(decoded_response[0:3])
        outbound_speed_bin      = int(decoded_response[4:7])
        inbound_magnitude_dB    = int(decoded_response[8:11])
        outbound_magnitude_dB   = int(decoded_response[12:15])

        # See
        conversion_factor = self.sampling_rate_Hz / (256 * 44.7)
        inbound_speed_kmph = inbound_speed_bin * conversion_factor
        outbound_speed_kmph = outbound_speed_bin * conversion_factor

        target_list = [
            inbound_speed_kmph,
            outbound_speed_kmph,
            inbound_magnitude_dB,
            outbound_magnitude_dB
        ]

        return KLD2_Status.OK, target_list



    def extract_speeds_kmph(self, target_list):
        inbound_speed_kmph = target_list[0]
        outbound_speed_kmph = target_list[1]
        return inbound_speed_kmph, outbound_speed_kmph

