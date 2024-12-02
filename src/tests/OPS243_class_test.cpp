#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#include "OPS243.h"

#define OPS243_BAUD_RATE 115200
const char RADAR_SERIAL_PORT[] = "/dev/ttyACM0";

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
    // Initialize the radar object
    OPS243 obj(RADAR_SERIAL_PORT, OPS243_BAUD_RATE);
    obj.set_num_range_reports(OPS243::MAX_REPORTS);
    obj.set_num_speed_reports(OPS243::MAX_REPORTS);
    obj.turn_units_output_on();
    obj.turn_fmcw_magnitude_reporting_on();
    obj.turn_doppler_magnitude_reporting_on();
    obj.turn_range_reporting_on();
    obj.set_data_precision(2);
    obj.turn_speed_reporting_on();
    obj.turn_range_reporting_on();

    int speed_magnitudes[OPS243::MAX_REPORTS];
    int range_magnitudes[OPS243::MAX_REPORTS];
    float speeds[OPS243::MAX_REPORTS];
    float ranges[OPS243::MAX_REPORTS];

    // Main loop
    while (true) {
        // Check if 'q' is pressed to exit
        if (kbhit()) {
            char ch = getchar();
            if (ch == 'q' || ch == 'Q') {
                std::cout << "Exiting program..." << std::endl;
                break;
            }
        }

        // Simulate reading from the radar
        obj.read_speeds_and_ranges(speed_magnitudes, range_magnitudes, speeds, ranges);

        // Sleep for 1ms
        usleep(1000);
    }

    obj.turn_range_reporting_off();

    return 0;
}