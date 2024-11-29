
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

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

        struct timespec now;
        timespec_get(&now, TIME_UTC);
        time_t seconds = now.tv_sec;
        int milliseconds = now.tv_sec/1000000;
        struct tm *timeinfo = localtime(&seconds);
        char time_buffer[30];
        strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

        // Assuming read_serial_data reads sensor data and populates the arrays
        near_pass_predictor.update_speeds_and_ranges();

        if(near_pass_predictor.is_vehicle_approaching()) {
            if(near_pass_predictor.is_vehicle_in_range()){
                printf("%s.%03d: A vehicle is approaching\n", time_buffer, milliseconds);
            }
        }
    }

    return 0;
}