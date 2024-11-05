
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <chrono>

#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

#define SERIAL_PORT "/dev/ttyACM0"
#define OPS241B_BAUD_RATE B115200

int main() {
    int serial_file = open(SERIAL_PORT, O_RDWR);

    struct termios tty;

    // Read in existing settings, and handle any error
    if(tcgetattr(serial_file, &tty) != 0) {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        return 1;
    }

    tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
    tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
    tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size
    tty.c_cflag |= CS8; // 8 bits per byte (most common)
    tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO; // Disable echo
    tty.c_lflag &= ~ECHOE; // Disable erasure
    tty.c_lflag &= ~ECHONL; // Disable new-line echo
    tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
    // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
    // tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

    tty.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    tty.c_cc[VMIN] = 0;

    // Set in/out baud rate to be 115200
    cfsetispeed(&tty, OPS241B_BAUD_RATE);
    cfsetospeed(&tty, OPS241B_BAUD_RATE);

    // Save tty settings, also checking for error
    if (tcsetattr(serial_file, TCSANOW, &tty) != 0) {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return 1;
    }

    char num_reports_cmd[] = "o9";
    write(serial_file, num_reports_cmd, sizeof(num_reports_cmd));

    char set_num_digits_cmd[] = "F2";
    write(serial_file, set_num_digits_cmd, sizeof(set_num_digits_cmd));

    char report_mag_cmd[] = "oM";
    write(serial_file, report_mag_cmd, sizeof(report_mag_cmd));

    char min_mag_cmd[] = "m>50";
    write(serial_file, min_mag_cmd, sizeof(min_mag_cmd));

    auto start_time = std::chrono::system_clock::now();

    while(1) {
        char read_buf[256];
        memset(read_buf, '\0', sizeof(read_buf));
        read(serial_file, read_buf, sizeof(read_buf));
        if(read_buf[0] != '\0') {
            auto current_time = std::chrono::system_clock::now();
            long long delta_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count();
            printf("%lld ms: %s", delta_time_ms, read_buf);
        }
        usleep(5000);
    }
}