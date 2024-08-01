
# This module provides interfacing methods for the K-LD2 Doppler Radar

import serial
from enum import Enum

class KLD2_Status(Enum):
    OK = 0
    DECODE_ERROR = 1
    ERROR = 2

class KLD2_Param(Enum):
    # S: System Params
    SYSTEM_ID               = 'S'
    SAMPLING_RATE           = 'S04'
    USE_SENSITIVITY_POT     = 'S0B'

    # D: Detection Params
    DETECTION_ID            = 'D'
    SENSITIVITY             = 'D01'

    # A: Array Params
    ARRAY_ID                = 'A'

    # F: Flash Read Params
    FLASH_READ_ID           = 'F'

    # R: Real-time Read Params
    REAL_TIME_READ_ID       = 'R'
    OPERATION_STATE         = 'R04'
    SENSITIVITY_POT_INDEX   = 'R06'

    # W: Basic Write Params
    BASIC_WRITE_ID          = 'W'

    # C: Complex Read Params
    COMPLEX_READ_ID         = 'C'
    TARGET_STRING           = 'C01'

    # T: Testing Params
    TESTING_ID              = 'T'

    # E: Error Messages
    ERROR_ID                = 'E'


class KLD2:
    DEFAULT_BAUD_RATE = 38400
    RESPONSE_PREFIX = '@'

    def __init__(self, uart_device):
        self.serial = serial.Serial(uart_device, KLD2.DEFAULT_BAUD_RATE)

        status = KLD2_Status.ERROR
        while(status != KLD2_Status.OK):
            status, self.sampling_rate_Hz = self.get_sampling_rate_Hz()



    def decode_response(self, response):
        try:
            decoded_response = response.decode('utf-8')
        except UnicodeDecodeError:
            return KLD2_Status.DECODE_ERROR, response

        return KLD2_Status.OK, decoded_response



    def get_param(self, param):
        command = '$' + param + '\r'
        self.serial.write(command.encode('utf-8'))
        response = self.serial.readline()

        status, decoded_response = self.decode_response(response)
        if(status != KLD2_Status.OK):
            return status, response

        # C class responses don't require a response prefix
        if(param[0] == KLD2_Param.COMPLEX_READ_ID):
            return status, response

        if(decoded_response[0] != KLD2.RESPONSE_PREFIX):
            return KLD2_Status.ERROR, response

        if(decoded_response[1] == KLD2_Param.ERROR_ID):
            return KLD2_Status.ERROR, response

        return_val = int(decoded_response[4:6])
        return KLD2_Status.OK, return_val



    def get_sampling_rate_Hz(self):
        status, sampling_rate = self.get_param(KLD2_Param.SAMPLING_RATE)

        if(status != KLD2_Status.OK):
            return status, sampling_rate

        self.sampling_rate_Hz = 1280 * sampling_rate
        return KLD2_Status.OK, self.sampling_rate_Hz



    def get_target_list(self):
        status, target_string = self.get_param(KLD2_Param.TARGET_STRING)
        if(status != KLD2_Status.OK):
            return status, target_string

        inbound_speed_bin       = int(target_string[0:3])
        outbound_speed_bin      = int(target_string[4:7])
        inbound_magnitude_dB    = int(target_string[8:11])
        outbound_magnitude_dB   = int(target_string[12:15])

        # See 'Speed Measurement' (page 11/15) in K-LD2 Datasheet
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

