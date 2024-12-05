
import time
from enum import Enum
import multiprocessing
from gpiozero import LED

import pin_defines
from util.RunningAvg import RunningAvg
from devices.HCSR04 import HCSR04
from devices.KLD2 import KLD2, KLD2_Param, KLD2_Status

from bluetooth.BLEInterface import BLEInterface
from bluetooth.Characteristic import Characteristic

def config_doppler(doppler_radar: KLD2):
    print('Configuring K-LD2...')
    doppler_radar.guarantee_set_param(KLD2_Param.SAMPLING_RATE, 6)
    doppler_radar.guarantee_set_param(KLD2_Param.USE_SENSITIVITY_POT, 0)
    doppler_radar.guarantee_set_param(KLD2_Param.SENSITIVITY, 7)
    print('Done Configuring K-LD2')

def run_near_pass_detector(near_pass_id_queue: multiprocessing.Queue):
    print('Near Pass Detection Process Starting...')

    ble_interface = BLEInterface()

    ultrasonic = HCSR04(pin_defines.HCSR04_TRIG_GPIO, pin_defines.HCSR04_ECHO_GPIO)
    doppler = KLD2(pin_defines.KLD2_UART_DEVICE)
    config_doppler(doppler)
    indicator_led = LED(pin_defines.NEAR_PASS_INDICATOR_GPIO)
    indicator_status = 0
    indicator_start_time_s = 0
    INDICATOR_PULSE_DURATION_S = 1

    new_sample_weight = 0.65
    distance_cm_running_avg = RunningAvg(new_sample_weight, 1000)

    class NearPassState(Enum):
        NO_NEAR_PASS = 0
        IN_NEAR_PASS = 1

    curr_near_pass_state = NearPassState.NO_NEAR_PASS
    near_pass_id = 0

    HYSTERESIS_THRESHOLD_cm = 5
    NEAR_PASS_THRESHOLD_cm = 150 + HYSTERESIS_THRESHOLD_cm

    pass_speed_kmph = 0
    pass_distance_cm = NEAR_PASS_THRESHOLD_cm

    frame = 0

    while True:
        distance_cm = ultrasonic.get_distance_cm()
        distance_cm_running_avg.sample(distance_cm)
        avg_distance_cm = distance_cm_running_avg.get()
        print("    %8.0f" % avg_distance_cm)

        status, target_list = doppler.try_get_target_list()
        if(status == KLD2_Status.OK):
            inbound_speed_kmph = target_list[0]

            # Take the passing speed as the max inbound speed since the last pass (TODO: find a better way to do this)
            if(inbound_speed_kmph > pass_speed_kmph):
                pass_speed_kmph = inbound_speed_kmph
        else:
            print('K-LD2 Error: ' + status.name + ', ' + target_list)

        prev_near_pass_state = curr_near_pass_state

        # State Machine
        match curr_near_pass_state:
            case NearPassState.NO_NEAR_PASS:
                if(avg_distance_cm < NEAR_PASS_THRESHOLD_cm - HYSTERESIS_THRESHOLD_cm):
                    curr_near_pass_state = NearPassState.IN_NEAR_PASS

            case NearPassState.IN_NEAR_PASS:
                # Get pass distance as min distance while in this state
                if(avg_distance_cm < pass_distance_cm):
                    pass_distance_cm = avg_distance_cm

                if(avg_distance_cm > NEAR_PASS_THRESHOLD_cm + HYSTERESIS_THRESHOLD_cm):
                    curr_near_pass_state = NearPassState.NO_NEAR_PASS

        # On state transition
        match (prev_near_pass_state, curr_near_pass_state):
            case (NearPassState.NO_NEAR_PASS, NearPassState.IN_NEAR_PASS):
                print('Near Pass started')

            case (NearPassState.IN_NEAR_PASS, NearPassState.NO_NEAR_PASS):
                print('Near Pass Over')
                print("Pass distance: %8.0f cm" % pass_distance_cm)
                print("Inbound speed: %8.0f kmph" % pass_speed_kmph)

                # Send near pass data to phone
                time_stamp_as_str16         = "%16d" % 0
                pass_speed_kmph_as_str16    = "%16.0f" % pass_speed_kmph
                pass_distance_cm_as_str16   = "%16.0f" % pass_distance_cm
                video_id_as_str16           = "%16d" % 0
                near_pass_flag_as_str16     = "%16d" % 1

                ble_interface.write(Characteristic.TIME_STAMP.value,        time_stamp_as_str16)
                ble_interface.write(Characteristic.SPEED.value,             pass_speed_kmph_as_str16)
                ble_interface.write(Characteristic.DISTANCE.value,          pass_distance_cm_as_str16)
                ble_interface.write(Characteristic.VIDEO_ID.value,          video_id_as_str16)
                ble_interface.write(Characteristic.NEAR_PASS_FLAG.value,    near_pass_flag_as_str16)

                # Notifty other process of near pass
                near_pass_id_queue.put(near_pass_id)

                # Indicate to cyclist
                indicator_led.on()
                indicator_start_time_s = time.time()
                indicator_status = 1

                # Reset near pass params
                pass_speed_kmph = 0
                pass_distance_cm = NEAR_PASS_THRESHOLD_cm
                near_pass_id += 1

        if(indicator_status == 1):
            if(time.time() - indicator_start_time_s > INDICATOR_PULSE_DURATION_S):
                indicator_led.off()

        frame += 1
