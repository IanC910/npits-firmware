
# This module provides interfacing methods for the K-LD2 Doppler Radar

import serial
from enum import Enum

class KLD2_Status(Enum):
    OK = 0
    ERROR_CLASS_RESPONSE = 1
    DECODE_ERROR = 2
    RESPONSE_LENGTH_MISMATCH = 3
    BAD_RESPONSE = 4
    ERROR = 20

class KLD2_Param_Class(Enum):
    SYSTEM                  = 'S'
    DETECTION               = 'D'
    ARRAY                   = 'A'
    FLASH_READ              = 'F'
    REAL_TIME_READ          = 'R'
    BASIC_WRITE             = 'W'
    COMPLEX_READ            = 'C'
    TESTING                 = 'T'
    ERROR                   = 'E'

class KLD2_Param(Enum):
    # S: System Params
    SAMPLING_RATE           = 'S04'
    USE_SENSITIVITY_POT     = 'S0B'

    # D: Detection Params
    SENSITIVITY             = 'D01'

    # A: Array Params

    # F: Flash Read Params

    # R: Real-time Read Params
    OPERATION_STATE         = 'R04'
    SENSITIVITY_POT_INDEX   = 'R06'

    # W: Basic Write Params

    # C: Complex Read Params
    TARGET_STRING           = 'C01'

    # T: Testing Params

    # E: Error Messages


class KLD2:
    DEFAULT_BAUD_RATE = 38400

    COMMAND_PREFIX = '$'
    COMMAND_SUFFIX = '\r'

    RESPONSE_PREFIX = '@'
    RESPONSE_SUFFIX = '\r\n'

    UART_TIMEOUT = 0.2

    STANDARD_RESPONSE_LENGTH = 8
    TARGET_LIST_LENGTH = 18

    def __init__(self, uart_device):
        self.serial = serial.Serial(uart_device, KLD2.DEFAULT_BAUD_RATE, timeout = self.UART_TIMEOUT)
        self.guarantee_set_param(KLD2_Param.SAMPLING_RATE, 2)



    def _decode_response(self, response):
        try:
            decoded_response = response.decode('utf-8')
        except UnicodeDecodeError:
            return KLD2_Status.DECODE_ERROR, response

        return KLD2_Status.OK, decoded_response



    def try_get_param(self, param: KLD2_Param, response_length = STANDARD_RESPONSE_LENGTH):
        command = self.COMMAND_PREFIX + param.value + self.COMMAND_SUFFIX
        self.serial.write(command.encode('utf-8'))
        response = self.serial.read(response_length)

        status, decoded_response = self._decode_response(response)
        if(status != KLD2_Status.OK):
            return status, response

        if(len(decoded_response) != response_length):
            return KLD2_Status.RESPONSE_LENGTH_MISMATCH, decoded_response

        # C class responses don't require a response prefix
        if(param.value[0] == KLD2_Param_Class.COMPLEX_READ.value):
            if(decoded_response[0] == KLD2.RESPONSE_PREFIX):
                return KLD2_Status.BAD_RESPONSE, decoded_response

            return status, decoded_response

        if(decoded_response[0] != KLD2.RESPONSE_PREFIX):
            return KLD2_Status.BAD_RESPONSE, decoded_response

        if(decoded_response[1] == KLD2_Param_Class.ERROR.value):
            return KLD2_Status.ERROR_CLASS_RESPONSE, decoded_response

        return_val = int(decoded_response[4:6])
        return KLD2_Status.OK, return_val



    def try_set_param(self, param: KLD2_Param, value, response_length = STANDARD_RESPONSE_LENGTH):
        value_as_hex_str = hex(value)[2:]
        if(len(value_as_hex_str) < 2):
            value_as_hex_str = '0' + value_as_hex_str

        command = self.COMMAND_PREFIX + param.value + value_as_hex_str + self.COMMAND_SUFFIX
        self.serial.write(command.encode('utf-8'))
        response = self.serial.read(response_length)

        status, decoded_response = self._decode_response(response)
        if(status != KLD2_Status.OK):
            return status, response

        if(len(decoded_response) != response_length):
            return KLD2_Status.RESPONSE_LENGTH_MISMATCH, decoded_response

        if(decoded_response[0] != KLD2.RESPONSE_PREFIX):
            return KLD2_Status.BAD_RESPONSE, decoded_response

        if(decoded_response[1] == KLD2_Param_Class.ERROR.value):
            return KLD2_Status.ERROR_CLASS_RESPONSE, decoded_response

        return_val = int(decoded_response[4:6])
        return KLD2_Status.OK, return_val



    def guarantee_get_param(self, param: KLD2_Param, response_length = STANDARD_RESPONSE_LENGTH):
        print('K-LD2: Guarantee Get Param +' + param.name + '...')

        status = KLD2_Status.ERROR
        while(status != KLD2_Status.OK):
            status, response = self.try_get_param(param, response_length)

        print('K-LD2: Success')
        return status, response



    def guarantee_set_param(self, param: KLD2_Param, value, response_length = STANDARD_RESPONSE_LENGTH):
        print('K-LD2: Guarantee Set Param ' + param.name + '...')

        status = KLD2_Status.ERROR
        while(status != KLD2_Status.OK):
            status, response = self.try_set_param(param, value, response_length)

        match(param):
            case KLD2_Param.SAMPLING_RATE:
                self.sampling_rate_Hz = 1280 * response

        print('K-LD2: Success')
        return status, response



    def try_get_target_list(self):
        status, target_string = self.try_get_param(KLD2_Param.TARGET_STRING, self.TARGET_LIST_LENGTH)
        if(status != KLD2_Status.OK):
            return status, target_string

        if not (target_string[3] == ';' and target_string[7] == ';' and target_string[11] == ';' and target_string[15] == ';'):
            return KLD2_Status.BAD_RESPONSE, target_string

        try:
            inbound_speed_bin       = int(target_string[0:3])
            outbound_speed_bin      = int(target_string[4:7])
            inbound_magnitude_dB    = int(target_string[8:11])
            outbound_magnitude_dB   = int(target_string[12:15])
        except:
            return KLD2_Status.BAD_RESPONSE, target_string

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

