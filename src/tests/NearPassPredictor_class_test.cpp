
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <string>

#include "../common/log.h"
#include "../devices/OPS243.h"
#include "../near_pass_detection/NearPassPredictor.h"

// Function to check for keyboard 'q' press
int kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

int main() {
    const char RADAR_SERIAL_PORT[] = "/dev/ttyACM0";
    OPS243 radar(RADAR_SERIAL_PORT);
    NearPassPredictor near_pass_predictor(&radar);

    // Initialize the sensor with custom settings
    near_pass_predictor.initialize_radar();
    
    while (true) {
        if (kbhit()) {
            char ch = getchar();
            if (ch == 'q' || ch == 'Q') {
                std::cout << "Exiting program..." << std::endl;
                break;
            }
        }

        near_pass_predictor.update_speeds_and_ranges();

        if(near_pass_predictor.is_vehicle_approaching()) {
            OPS243::speed_report_t speed_report = near_pass_predictor.get_speed_of_approaching_vehicle_mps();
            log("Test",
                std::string("Vehicle approaching! ") + 
                std::string(" Speed mps: ") + std::to_string(speed_report.speed_mps) +
                std::string(" Magnitude: ") + std::to_string(speed_report.magnitude)
            );
        }
            
        if(near_pass_predictor.is_vehicle_in_range()){
            OPS243::range_report_t range_report = near_pass_predictor.get_distance_of_highest_mag_m();
            log("Test",
                std::string("Vehicle in range! ") + 
                std::string(" Range m: ") + std::to_string(range_report.range_m) +
                std::string(" Magnitude: ") + std::to_string(range_report.magnitude)
            );
        }
    }

    return 0;
}