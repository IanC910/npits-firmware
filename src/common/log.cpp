
#include <stdio.h>
#include <string>

#include "time_tools.h"

#include "log.h"

static FILE* log_file = nullptr;

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
}