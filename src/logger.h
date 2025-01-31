#pragma once

#include <chrono>
#include <ctime>
#include <iostream>
#include <memory>
#include "error.h"

/**
 * @struct Move
 * @brief Forward declaration of Move struct (defined elsewhere).
 */
struct Move;

/**
 * @class Logger
 * @brief Provides a simple logging facility with optional silent mode and an output destination.
 *
 * The Logger can write log messages along with timestamps to a given output stream. If silent mode
 * is activated or if the logger encounters critical failures, it will stop logging.
 */
class Logger {
private:
  /**
   * @brief If true, suppresses log output.
   */
  bool silent;

  /**
   * @brief The destination output stream to which logs are written.
   */
  std::ostream *destination;

  /**
   * @brief Tracks whether the logger is functioning correctly.
   */
  bool isOK = true;

public:
  /**
   * @brief Constructs a Logger.
   * 
   * @param silent If true, suppresses logging output entirely.
   * @param destination The output stream where log messages will be written.
   *        If nullptr or any error occurs, the logger may become non-functional.
   */
  Logger(bool silent, std::ostream *destination);

  /**
   * @brief Logs a message to the output stream with a timestamp, unless silent mode is on.
   *
   * This is a template method to allow logging of various types (strings, numbers, etc.).
   * 
   * @tparam T The type of the message to log.
   * @param str The message to write to the log.
   */
  template <typename T>
  void log(T str) {
    if (!silent && destination && isOK) {
      try {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::string timeStr = std::ctime(&now_time);
        timeStr.pop_back(); // Remove trailing newline
        *destination << timeStr << "\t" << str << std::endl;
      } catch (const std::exception &e) {
        // If an error occurs, log a non-critical error and stop further logging.
        throwNotCriticalError("Could not log, " + std::string(e.what()) +
                              "\tlogger will be stopped");
        isOK = false;
      }
    }
  }

  /**
   * @brief Converts a Move struct into a human-readable string (e.g., "a2->a3").
   *
   * @param mv A Move object describing a piece's start and end coordinates.
   * @return A formatted string representing the move.
   */
  static std::string moveToString(Move mv);

  /**
   * @brief Destructor that flushes and (if necessary) deletes the output stream.
   */
  ~Logger();
};
