
import time
import sys
from enum import Enum

sys.path.append('../')

import pin_defines
from util.StatTracker import StatTracker
from devices.HCSR04 import HCSR04
from devices.KLD2 import KLD2, KLD2_Param, KLD2_Status

class NearPassState(Enum):
    NO_NEAR_PASS = 0
    NEAR_PASS_UNDER_50 = 1
    NEAR_PASS_OVER_50 = 2

def near_pass_counter_test():
    print('Near Pass Counter Test')

    running_avg_new_sample_weight = 0.65
    distance_cm_tracker = StatTracker(running_avg_new_sample_weight)

    ultrasonic = HCSR04(pin_defines.HCSR04_TRIG_GPIO, pin_defines.HCSR04_ECHO_GPIO)
    doppler = KLD2(pin_defines.KLD2_UART_DEVICE)

    SAMPLING_FREQUENCY_Hz = 10

    near_pass_state = NearPassState.NO_NEAR_PASS

    HYSTERESIS_THRESHOLD_cm = 5
    NEAR_PASS_THRESHOLD_OVER_50_cm = 150 + HYSTERESIS_THRESHOLD_cm
    NEAR_PASS_THRESHOLD_UNDER_50_cm = 100 + HYSTERESIS_THRESHOLD_cm

    max_inbound_speed_kmph = 0

    frame = 0

    while True:
        if(frame % 10 == 0):
            print('')

        distance_cm = ultrasonic.get_distance_cm()
        distance_cm_tracker.sample(distance_cm)
        avg_distance_cm = distance_cm_tracker.get_running_avg()

        status, target_list = doppler.try_get_target_list()

        if(status == KLD2_Status.OK):
            inbound_speed_kmph = target_list[0]
            if(inbound_speed_kmph > max_inbound_speed_kmph):
                max_inbound_speed_kmph = inbound_speed_kmph
        else:
            print('Error getting target list')

        match near_pass_state:
            case NearPassState.NO_NEAR_PASS:
                if(avg_distance_cm < NEAR_PASS_THRESHOLD_UNDER_50_cm - HYSTERESIS_THRESHOLD_cm):
                    near_pass_state = NearPassState.NEAR_PASS_UNDER_50
                    print('NO to <50')
                    print('Avg D: %8.0f cm' % avg_distance_cm)

                elif(avg_distance_cm < NEAR_PASS_THRESHOLD_OVER_50_cm - HYSTERESIS_THRESHOLD_cm):
                    near_pass_state = NearPassState.NEAR_PASS_OVER_50
                    print('NO to >50')
                    print('Avg D: %8.0f cm' % avg_distance_cm)

            case NearPassState.NEAR_PASS_UNDER_50:
                if(avg_distance_cm > NEAR_PASS_THRESHOLD_OVER_50_cm + HYSTERESIS_THRESHOLD_cm):
                    near_pass_state = NearPassState.NO_NEAR_PASS
                    print('<50 to NO')
                    print('Avg D: %8.0f cm' % avg_distance_cm)
                    print("Inbound speed: %8.0f kmph" % max_inbound_speed_kmph)
                    print('')
                    max_inbound_speed_kmph = 0

            case NearPassState.NEAR_PASS_OVER_50:
                if(avg_distance_cm < NEAR_PASS_THRESHOLD_UNDER_50_cm - HYSTERESIS_THRESHOLD_cm):
                    near_pass_state = NearPassState.NEAR_PASS_UNDER_50
                    print('>50 to <50')
                    print('Avg D: %8.0f cm' % avg_distance_cm)

                elif(avg_distance_cm > NEAR_PASS_THRESHOLD_OVER_50_cm + HYSTERESIS_THRESHOLD_cm):
                    near_pass_state = NearPassState.NO_NEAR_PASS
                    print('>50 to NO')
                    print('Avg D: %8.0f cm' % avg_distance_cm)
                    print("Inbound speed: %8.0f kmph" % max_inbound_speed_kmph)
                    print('')
                    max_inbound_speed_kmph = 0

        frame += 1

if(__name__ == "__main__"):
    near_pass_counter_test()