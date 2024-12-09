#include "Logger.hpp"

#define LOG_SPACER std::left << std::setw(25)

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::log(LogLevel level, const std::string& message) const {
    std::stringstream ss;

    switch (level) {
    case LOG_DEBUG:
        ss << BLUE << LOG_SPACER << "[ Debug ]: " << RESET;
        break;
    case LOG_INFO:
        ss << GREEN << LOG_SPACER << "[ Info ]: " << RESET;
        break;
    case LOG_WARNING:
        ss << YELLOW << LOG_SPACER << "[ Warning ]: " << RESET;
        break;
    case LOG_ERROR:
        ss << RED << LOG_SPACER << "[ Error ]: " << RESET;
        break;
    }
    ss << message << std::endl;
    std::cout << ss.str();
}
