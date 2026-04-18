#pragma once

#define RED "\033[31m"
#define RESET "\033[0m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define MAGENTA "\033[35m"
#define BLUE "\033[34m"
#define BOLDMAGENTA "\033[1m\033[35m"

#include<iostream>
#include<chrono>
#include<format>
#include<string>
#include<exception>

enum class LOG_LEVEL : short {
    INFO,
    ERROR,
    WARN,
    DEBUG,
    FATAL,
    TRACE,
    SUCCESS,
    FAILED,
    DEFAULT
};

std::string getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t nowTimeValue = std::chrono::system_clock::to_time_t(now);
    std::tm localTime = *std::localtime(&nowTimeValue);
    return std::format("[{:02}:{:02}:{:02}]", localTime.tm_hour, localTime.tm_min, localTime.tm_sec);
}

class Logger {
private:
    std::string rawMessage;
    LOG_LEVEL status;
    std::string logColor;
    
    std::string getStatusPrefix() const {
        switch(status) {
            case LOG_LEVEL::INFO:    return "INFO: ";
            case LOG_LEVEL::ERROR:   return "ERROR: ";
            case LOG_LEVEL::WARN:    return "WARN: ";
            case LOG_LEVEL::DEBUG:   return "DEBUG: ";
            case LOG_LEVEL::FATAL:   return "FATAL: ";
            case LOG_LEVEL::TRACE:   return "TRACE: ";
            case LOG_LEVEL::SUCCESS: return "SUCCESS: ";
            case LOG_LEVEL::FAILED:  return "FAILED: ";
            case LOG_LEVEL::DEFAULT: return "";
            default: return "";
        }
    }
    
    void updateColor() {
        switch(status) {
            case LOG_LEVEL::INFO:    logColor = WHITE; break;
            case LOG_LEVEL::ERROR:   logColor = RED; break;
            case LOG_LEVEL::WARN:    logColor = YELLOW; break;
            case LOG_LEVEL::DEBUG:   logColor = CYAN; break;
            case LOG_LEVEL::FATAL:   logColor = MAGENTA; break;
            case LOG_LEVEL::TRACE:   logColor = BLUE; break;
            case LOG_LEVEL::SUCCESS: logColor = GREEN; break;
            case LOG_LEVEL::FAILED:  logColor = BOLDMAGENTA; break;
            case LOG_LEVEL::DEFAULT: logColor = RESET; break;
        }
    }
    
    std::string getFormattedMessage() const {
        return getStatusPrefix() + rawMessage;
    }
    
public:
    Logger(std::string msg = "Stuff Happened", LOG_LEVEL level = LOG_LEVEL::SUCCESS) 
        : rawMessage(msg), status(level) {
        updateColor();
    }
    
    void log() const noexcept {
        std::string time = getCurrentTime();
        std::cout << logColor << getFormattedMessage() << RESET << " " << time << std::endl;
    }
    
    void updateMessage(const std::string& newMsg, LOG_LEVEL newStatus) {
        rawMessage = newMsg;
        status = newStatus;
        updateColor();
    }
};