#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include "OPS243.cpp"  // Assuming OPS243.h provides the necessary class methods

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
    obj.set_number_of_range_reports(9);
    obj.set_number_of_speed_reports(9);
    obj.turn_units_output_on();
    obj.turn_fmcw_magnitude_reporting_on();
    obj.turn_doppler_magnitude_reporting_on();
    obj.turn_range_reporting_on();
    obj.set_data_precision(2);
    obj.turn_speed_reporting_on();
    obj.turn_range_reporting_on();

    int speed_magnitudes[9];
    int range_magnitudes[9];
    float speeds[9];
    float ranges[9];

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
        read_serial_file(obj, speed_magnitudes, range_magnitudes, speeds, ranges);

        // Sleep for 1ms
        usleep(1000);
    }

    /// Clean up before exiting
    obj.turn_range_reporting_off();
    //obj.turn_speed_reporting_off();
    obj.~OPS243();

    return 0;
}


