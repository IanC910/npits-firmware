#!/usr/bin/python3

import os
import sys
from time import *
from decimal import *
import serial

SERIAL_PORT = '/dev/ttyACM0'

# Ops241B module settings:  ftps, dir off, 5Ksps, min -9dB pwr, squelch 5000
Ops241B_Output_Units_OFF = 'Ou'
Ops241B_Output_Magnitude_ON = 'OM'
Ops241B_Range_Output_Units = 'uM'
Ops241B_Wait_Interval = 'WL'
Ops241B_Transmit_Power = 'PD'    # miD power
Ops241B_Threshold_Control = 'm>200/r/n'
Ops241B_Reporting_Preference = 'Ov' # OV is oprder by the value.  /=sm to lg`
#Ops241B_Reporting_Preference = 'OVO\\' # OV is oprder by the value.  /=sm to lg`
Ops241B_Module_Information = '??'

# sendSerialCommand: function for sending commands to the OPS-241B module
def send_serial_cmd(print_prefix, command) :
    data_for_send_str = command
    data_for_send_bytes = str.encode(data_for_send_str)
    print(print_prefix, command)
    ser.write(data_for_send_bytes)
    # Initialize message verify checking
    ser_message_start = '{'
    ser_write_verify = False
    # Print out module response to command string
    while not ser_write_verify :
        data_rx_bytes = ser.readline()
        data_rx_length = len(data_rx_bytes)
        if (data_rx_length != 0) :
            data_rx_str = str(data_rx_bytes)
            if data_rx_str.find(ser_message_start) :
                ser_write_verify = True

# Initialize the USB port to read from the OPS-241B module
ser = serial.Serial(
    port = SERIAL_PORT,
    baudrate = 115200,
    parity = serial.PARITY_NONE,
    stopbits = serial.STOPBITS_ONE,
    bytesize = serial.EIGHTBITS,
    timeout = 1,
    writeTimeout = 2
)

ser.flushInput()
ser.flushOutput()

# Initialize and query Ops241B Module
print("\nInitializing Ops241B Module")
send_serial_cmd("\nModule Information: ", Ops241B_Module_Information)
send_serial_cmd("\nSet Wait interval: ", Ops241B_Wait_Interval)
send_serial_cmd("\nSet Range Output Units to Meters: ", Ops241B_Range_Output_Units)
send_serial_cmd("\nSet Reporting order pref (by mag or distance): ", Ops241B_Reporting_Preference)
send_serial_cmd("\nSet Output Units OFF: ", Ops241B_Output_Units_OFF)
send_serial_cmd("\nSet Magnitude Output ON: ", Ops241B_Output_Magnitude_ON)
#send_serial_cmd("\nSet Transmit Power: ", Ops241B_Transmit_Power)
#send_serial_cmd("\nSet Threshold Control: ", Ops241B_Threshold_Control)


# Main Loop
def read_and_render():
    max_discovered_mag = 500
    done = False
    while not done:
        range_available = False
        Ops241_rx_bytes = ser.readline()
        # Check for range information from OPS241
        Ops241_rx_bytes_length = len(Ops241_rx_bytes)
        if (Ops241_rx_bytes_length != 0) :
            Ops241_rx_str = Ops241_rx_bytes.decode("utf-8")
            #print("RX:"+Ops241_rx_str)
            if len(Ops241_rx_str) > 3 and Ops241_rx_str.find('{') == -1 :
                # Speed data found
                try:
                    maybe_nums = Ops241_rx_str.split(',')
                    Ops241_mag_float = float(maybe_nums[0])
                    Ops241_range_float = float(maybe_nums[1])
                    range_available = True

                    # color is proportional to the max mag ever seen
                    if Ops241_mag_float > max_discovered_mag:
                        max_discovered_mag = Ops241_mag_float
                        # though note that it permanenty changes, until restart
                except ValueError:
                    print("Unable to convert the strings {} and {}".format(maybe_nums[0],maybe_nums[1]) )
                    range_available = False

        if range_available == True :
            range_rnd = round(Ops241_range_float, 3)
            print("Range", range_rnd)


read_and_render()
