#include <thread>
#include "near_pass_validator.cpp"
#include "near_pass_detector.cpp"


void le(int &ride_status) {
    le_server_run(ride_status);
};

void predictor(int &ride_status, int &approaching) {

    near_pass_validator npv(serial_port, baud_rate);
    npv.initialize_sensor();

    int speed_magnitudes[9];
    int range_magnitudes[9];
    float speeds[9];
    float ranges[9];
    
    struct timespec now;
    time_t seconds;
    int milliseconds;
    struct tm *timeinfo;
    char time_buffer[30];

	timespec_get(&now, TIME_UTC);
	seconds = now.tv_sec;
	milliseconds = now.tv_sec/1000000;
	timeinfo = localtime(&seconds);
    strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

    while (true) {
        // Assuming read_serial_data reads sensor data and populates the arrays
        read_serial_file(sensor, speed_magnitudes, range_magnitudes, speeds, ranges);
        if(sensor.is_vehicle_approaching(speeds, speed_magnitudes))
            if(sensor.is_vehicle_in_range(ranges, range_magnitudes))
                        approaching = true;
            
    }

    return 0;


};

void detector(int &ride_status, int &approaching) {

};



int main() {

    int ride_status = false;
    int approaching false;

    NearPassDetector detectorobj;
    
    std::thread le_server_thread(le, &ride_status);


    while (true) {
        while (!ride_status);
        std::thread predictor_thread(predictor, ride_status, approaching);
        std::thread detector_thread(detector, ride_status, approaching);
    }

}