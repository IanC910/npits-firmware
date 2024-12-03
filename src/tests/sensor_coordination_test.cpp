
#include <string>
#include <thread>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>

#include "../connection_params.h"

#include "../devices/MB1242.h"
#include "../devices/OPS243.h"

#include "../near_pass_detection/NearPassPredictor.h"
#include "../near_pass_detection/NearPassDetector.h"

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
    OPS243 radar(RADAR_SERIAL_PORT);
    MB1242 ultrasonic(ULTRASONIC_I2C_DEVICE, ULTRASONIC_STATUS_GPIO_NUM);

    NearPassPredictor near_pass_predictor(&radar);
    NearPassDetector near_pass_detector(&ultrasonic, &near_pass_predictor);

    // Start this one asynchronously on its own thread
    near_pass_predictor.start();

    // Run this one here, blocking. No stop condition except ctrl+c
    near_pass_detector.start();

    while(true) {
        if (kbhit()) {
            char ch = getchar();
            if (ch == 'q' || ch == 'Q') {
                std::cout << "Exiting program..." << std::endl;
                break;
            }
        }
    }

    near_pass_detector.stop();
    near_pass_predictor.stop();


}