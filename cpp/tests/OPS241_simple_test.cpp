
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <chrono>

#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

#define SERIAL_PORT "/dev/ttyACM2"
#define OPS241B_BAUD_RATE B115200

int64_t millis()
{
    struct timespec now;
    timespec_get(&now, TIME_UTC);
    return ((int64_t) now.tv_sec) * 1000 + ((int64_t) now.tv_nsec) / 1000000;
}

void format_unix_timestamp(int64_t milliseconds) {
    // Split the milliseconds into seconds and milliseconds
    time_t seconds = milliseconds / 1000;
    int ms = milliseconds % 1000;

    // Convert seconds to struct tm
    struct tm *timeinfo = localtime(&seconds);

    // Format the time into a human-readable format (YYYY-MM-DD HH:MM:SS)
    char time_buffer[30];
    strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

    // Print the formatted time with millisecond precision
    printf("Formatted timestamp: %s.%03d\n", time_buffer, ms);
}

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

    auto start_time = std::chrono::system_clock::now();

    for(long long frame = 0; frame < 10000; frame++) {
        switch(frame) {
            case 5: {
                char module_info_cmd[] = "??";
                write(serial_file, module_info_cmd, sizeof(module_info_cmd));
            }
            case 10: {
                char num_reports_cmd[] = "o9";
                write(serial_file, num_reports_cmd, sizeof(num_reports_cmd));
                break;
            }
            case 15: {
                char set_num_digits_cmd[] = "F1";
                write(serial_file, set_num_digits_cmd, sizeof(set_num_digits_cmd));
                break;
            }
            case 20: {
                char report_mag_cmd[] = "oM";
                write(serial_file, report_mag_cmd, sizeof(report_mag_cmd));
                break;
            }
            case 25: {
                char min_mag_cmd[] = "m>50";
                write(serial_file, min_mag_cmd, sizeof(min_mag_cmd));
                break;
            }
            default: {
                break;
            }
        } // switch (frame)

        char line_buf[256];
        memset(line_buf, 0, sizeof(line_buf));

        int line_buf_index = 0;
        while(line_buf_index < sizeof(line_buf)) {
            int num_bytes_read = read(serial_file, line_buf + line_buf_index, 1);
            if(line_buf[line_buf_index] == '\n') {
                line_buf[line_buf_index] = 0;
                break;
            }

            line_buf_index += num_bytes_read;
        }

        struct timespec now;
        timespec_get(&now, TIME_UTC);
        time_t seconds = now.tv_sec;
        int milliseconds = now.tv_nsec / 1000000;
        struct tm *timeinfo = localtime(&seconds);
        char time_buffer[30];
        strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
        printf("%s.%03d: %s\n", time_buffer, milliseconds, line_buf);

        usleep(10000);
    }

    close(serial_file);
}