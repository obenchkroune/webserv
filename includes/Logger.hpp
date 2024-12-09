#pragma once

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#define RED    "\x1b[31m"
#define GREEN  "\x1b[32m"
#define BLUE   "\x1b[34m"
#define YELLOW "\x1b[33m"
#define CYAN   "\x1b[36m"
#define RESET  "\x1b[0m"

enum LogLevel { LOG_DEBUG = 0, LOG_INFO, LOG_WARNING, LOG_ERROR };

class Logger {
public:
    static Logger& getInstance();
    void           log(LogLevel level, const std::string& message) const;

private:
    Logger();
    Logger(const Logger& other);
    Logger& operator=(const Logger& other);
    ~Logger();
};
