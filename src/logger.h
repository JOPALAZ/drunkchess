#pragma once
#include "error.h"
#include <chrono>
#include <ctime>
#include <iostream>
#include <memory>
class Logger {
private:
  bool silent;
  std::ostream *destination;
  bool isOK;

public:
  Logger(bool silent, std::ostream *destination);
  template <typename T> void log(T str) {
    if (!silent && destination && isOK) {
      try {
        auto end = std::chrono::system_clock::now();
        std::time_t end_time = std::chrono::system_clock::to_time_t(end);
        std::string time = std::ctime(&end_time);
        time.pop_back();
        *destination << time << "\t" << str << std::endl;
      } catch (const std::exception &e) {
        throwNotCriticalError("Could not log, " + std::string(e.what()) +
                              "\tlogger will be stopped");
        isOK = false;
      }
    }
  }
  ~Logger();
};
