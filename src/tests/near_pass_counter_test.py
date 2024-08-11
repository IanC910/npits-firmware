
import time
import sys
from enum import Enum

sys.path.append('../')

import pin_defines
from util.RunningAvg import RunningAvg
from devices.HCSR04 import HCSR04
from devices.KLD2 import KLD2, KLD2_Param, KLD2_Status

class NearPassState(Enum):
    NO_NEAR_PASS = 0
    NEAR_PASS_CLOSE = 1
    NEAR_PASS_FAR = 2

def near_pass_counter_test():
    print('Near Pass Counter Test')

    new_sample_weight = 0.65
    distance_cm_running_avg = RunningAvg(new_sample_weight, 1000)

    ultrasonic = HCSR04(pin_defines.HCSR04_TRIG_GPIO, pin_defines.HCSR04_ECHO_GPIO)
    doppler = KLD2(pin_defines.KLD2_UART_DEVICE)
    doppler.guarantee_set_param(KLD2_Param.SAMPLING_RATE, 6)
    doppler.guarantee_set_param(KLD2_Param.USE_SENSITIVITY_POT, 0)
    doppler.guarantee_set_param(KLD2_Param.SENSITIVITY, 7)

    curr_near_pass_state = NearPassState.NO_NEAR_PASS

    HYSTERESIS_THRESHOLD_cm = 5
    NEAR_PASS_FAR_THRESHOLD_cm = 150 + HYSTERESIS_THRESHOLD_cm
    NEAR_PASS_CLOSE_THRESHOLD_cm = 100 + HYSTERESIS_THRESHOLD_cm

    max_inbound_speed_kmph = 0

    frame = 0

    while True:
        if(frame % 10 == 0):
            print('')

        distance_cm = ultrasonic.get_distance_cm()
        distance_cm_running_avg.sample(distance_cm)
        avg_distance_cm = distance_cm_running_avg.get()

        status, target_list = doppler.try_get_target_list()
        if(status == KLD2_Status.OK):
            inbound_speed_kmph = target_list[0]
            if(inbound_speed_kmph > max_inbound_speed_kmph):
                max_inbound_speed_kmph = inbound_speed_kmph
        else:
            print('K-LD2 Error: ' + status.name)

        next_near_pass_state = curr_near_pass_state

        match curr_near_pass_state:
            case NearPassState.NO_NEAR_PASS:
                if(avg_distance_cm < NEAR_PASS_CLOSE_THRESHOLD_cm - HYSTERESIS_THRESHOLD_cm):
                    next_near_pass_state = NearPassState.NEAR_PASS_CLOSE
                    print('NO to <50')
                    print('Avg D: %8.0f cm' % avg_distance_cm)

                elif(avg_distance_cm < NEAR_PASS_FAR_THRESHOLD_cm - HYSTERESIS_THRESHOLD_cm):
                    next_near_pass_state = NearPassState.NEAR_PASS_FAR
                    print('NO to >50')
                    print('Avg D: %8.0f cm' % avg_distance_cm)

            case NearPassState.NEAR_PASS_CLOSE:
                if(avg_distance_cm > NEAR_PASS_FAR_THRESHOLD_cm + HYSTERESIS_THRESHOLD_cm):
                    next_near_pass_state = NearPassState.NO_NEAR_PASS
                    print('<50 to NO')
                    print('Avg D: %8.0f cm' % avg_distance_cm)
                    print("Inbound speed: %8.0f kmph" % max_inbound_speed_kmph)
                    print('')
                    max_inbound_speed_kmph = 0

            case NearPassState.NEAR_PASS_FAR:
                if(avg_distance_cm < NEAR_PASS_CLOSE_THRESHOLD_cm - HYSTERESIS_THRESHOLD_cm):
                    next_near_pass_state = NearPassState.NEAR_PASS_CLOSE
                    print('>50 to <50')
                    print('Avg D: %8.0f cm' % avg_distance_cm)

                elif(avg_distance_cm > NEAR_PASS_FAR_THRESHOLD_cm + HYSTERESIS_THRESHOLD_cm):
                    next_near_pass_state = NearPassState.NO_NEAR_PASS
                    print('>50 to NO')
                    print('Avg D: %8.0f cm' % avg_distance_cm)
                    print("Inbound speed: %8.0f kmph" % max_inbound_speed_kmph)
                    print('')
                    max_inbound_speed_kmph = 0

        curr_near_pass_state = next_near_pass_state

        frame += 1

if(__name__ == "__main__"):
    near_pass_counter_test()