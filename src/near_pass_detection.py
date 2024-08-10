
import time
from enum import Enum

import pin_defines
from util.RunningAvg import RunningAvg
from devices.HCSR04 import HCSR04
from devices.KLD2 import KLD2, KLD2_Param, KLD2_Status

from bluetooth.BLEInterface import BLEInterface

def init_doppler(doppler_radar: KLD2):
    doppler_radar.guarantee_set_param(KLD2_Param.SAMPLING_RATE, 6)
    doppler_radar.guarantee_set_param(KLD2_Param.USE_SENSITIVITY_POT, 0)
    doppler_radar.guarantee_set_param(KLD2_Param.SENSITIVITY, 7)

def run_near_pass_detector():
    print('Near Pass Detection Process Starting...')

    ultrasonic = HCSR04(pin_defines.HCSR04_TRIG_GPIO, pin_defines.HCSR04_ECHO_GPIO)
    doppler = KLD2(pin_defines.KLD2_UART_DEVICE)
    init_doppler(doppler)

    phone_ble = BLEInterface()

    new_sample_weight = 0.65
    distance_cm_running_avg = RunningAvg(new_sample_weight, 1000)

    class NearPassState(Enum):
        NO_NEAR_PASS = 0
        IN_NEAR_PASS = 1

    curr_near_pass_state = NearPassState.NO_NEAR_PASS

    HYSTERESIS_THRESHOLD_cm = 5
    NEAR_PASS_THRESHOLD_cm = 150 + HYSTERESIS_THRESHOLD_cm

    max_inbound_speed_kmph = 0

    frame = 0

    while True:
        if(frame % 10 == 0):
            print('---')

        distance_cm = ultrasonic.get_distance_cm()
        distance_cm_running_avg.sample(distance_cm)
        avg_distance_cm = distance_cm_running_avg.get()
        print("%8.0f" % avg_distance_cm)

        status, target_list = doppler.try_get_target_list()
        if(status == KLD2_Status.OK):
            inbound_speed_kmph = target_list[0]
            if(inbound_speed_kmph > max_inbound_speed_kmph):
                max_inbound_speed_kmph = inbound_speed_kmph
        else:
            print('K-LD2 Error: ' + status.name)

        prev_near_pass_state = curr_near_pass_state

        match curr_near_pass_state:
            case NearPassState.NO_NEAR_PASS:
                if(avg_distance_cm < NEAR_PASS_THRESHOLD_cm - HYSTERESIS_THRESHOLD_cm):
                    curr_near_pass_state = NearPassState.IN_NEAR_PASS
                    print('In Near Pass')
                    print('Avg D: %8.0f cm' % avg_distance_cm)

            case NearPassState.IN_NEAR_PASS:
                if(avg_distance_cm > NEAR_PASS_THRESHOLD_cm + HYSTERESIS_THRESHOLD_cm):
                    curr_near_pass_state = NearPassState.NO_NEAR_PASS
                    print('Near Pass Over')
                    print('Avg D: %8.0f cm' % avg_distance_cm)
                    print("Inbound speed: %8.0f kmph" % max_inbound_speed_kmph)
                    print('')

        if(prev_near_pass_state == NearPassState.IN_NEAR_PASS and curr_near_pass_state == NearPassState.NO_NEAR_PASS):
            print('Logging near pass')
            phone_ble.write(0, '1')
            max_inbound_speed_kmph = 0

        frame += 1
