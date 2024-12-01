
#include <string>
#include <iostream>

#include "time_tools.h"

#include "log.h"

void log(std::string tag, std::string message) {
    std::cout <<
        get_timestamp_hms() <<
        " [" <<
        tag <<
        "] " <<
        message <<
        std::endl;
}