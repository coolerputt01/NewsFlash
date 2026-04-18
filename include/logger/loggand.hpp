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
    return std::format("[{:02}:{:02}:{:02}]",localTime.tm_hour,localTime.tm_min,localTime.tm_sec);
}


class Logger {
  public:
    std::string message;
    LOG_LEVEL status;
    std::string logColor;
    
    Logger(std::string msg="Stuff Happened",LOG_LEVEL status = LOG_LEVEL::SUCCESS);
    
    void log() noexcept {
      std::string time = getCurrentTime();
      std::cout<<logColor<<message<<RESET<<" "<<time<<std::endl;
    }
};

Logger::Logger(std::string msg,LOG_LEVEL status){
  this->status = status;
  switch(status){
    case LOG_LEVEL::INFO:
      this->message = "INFO: " + msg;
      logColor = WHITE;
      break;
    case LOG_LEVEL::ERROR:
      this->message = "ERROR: " + msg;
      logColor = RED;
      break;
    case LOG_LEVEL::WARN:
      this->message = "WARN: " + msg;
      logColor = YELLOW;
      break;
    case LOG_LEVEL::DEBUG:
      this->message = "DEBUG: " + msg;
      logColor = CYAN;
      break;
    case LOG_LEVEL::FATAL:
      this->message = "FATAL: " + msg;
      logColor = MAGENTA;
      break;
    case LOG_LEVEL::TRACE:
      this->message = "TRACE: " + msg;
      logColor = BLUE;
      break;
    case LOG_LEVEL::DEFAULT:
      this->message = "DEFAULT: " + msg;
      logColor = RESET;
      break;
    case LOG_LEVEL::SUCCESS:
      this->message = "SUCCESS: " + msg;
      logColor = GREEN;
      break;
    case LOG_LEVEL::FAILED:
      this->message = "FAILED: " + msg;
      logColor = BOLDMAGENTA;
      break;
  }
}
