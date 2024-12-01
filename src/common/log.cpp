
#include <string>
#include <iostream>

#include "time_tools.h"

#include "log.h"

void log(std::string tag, std::string message) {
    printf("%s [%s] %s\n",
        get_timestamp_hms().c_str(),
        tag.c_str(),
        message.c_str()
    );
}