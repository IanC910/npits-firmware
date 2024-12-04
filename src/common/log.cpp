
#include <stdio.h>
#include <string>

#include "time_tools.h"

#include "log.h"

const int SAVE_PERIOD_ms = 100;

static FILE* log_file = nullptr;

static long long time_of_last_save_ms = 0;

void log(std::string tag, std::string message) {
    if(log_file == nullptr) {
        log_file = fopen(LOG_FILE_NAME.c_str(), "w");
    }

    std::string text =
        get_timestamp_hms() +
        " [" + tag + "] " +
        message;

    printf("%s\n", text.c_str());
    fprintf(log_file, "%s\n", text.c_str());

    long long now_ms = get_time_ms();
    if(now_ms > time_of_last_save_ms + SAVE_PERIOD_ms) {
        fclose(log_file);
        log_file = fopen(LOG_FILE_NAME.c_str(), "a");
        time_of_last_save_ms = now_ms;
    }
}