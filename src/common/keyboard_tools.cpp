

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include "time_tools.h"

#include "keyboard_tools.h"

bool was_key_pressed() {
    struct termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
    struct termios newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    int oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    char ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return true;
    }

    return false;
}

void wait_for_key_press(char key) {
    while(true) {
        if(was_key_pressed()) {
            char ch = getchar();
            if(ch == key) {
                break;
            }
        }

        sleep_ms(10);
    }
}

