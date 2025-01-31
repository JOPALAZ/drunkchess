#include "logger.h"
#include "chess-board.h"

/**
 * @brief Constructs a Logger, immediately attempts to log its creation.
 * 
 * If logging fails during creation, the logger is marked as non-functional (isOK = false).
 */
Logger::Logger(bool silent, std::ostream *destination)
    : silent(silent), destination(destination) {
  try {
    this->log("LOGGER CREATED");
  } catch (std::exception &e) {
    throwNotCriticalError("Unable to create logger: " + std::string(e.what()));
    isOK = false;
  }
}

/**
 * @brief Converts a Move object to chess notation-like string for logging.
 *
 * The format is `<file><rank>-><file><rank>`, where file is a-h and rank is 1-8.
 * 
 * @param mv The Move to convert.
 * @return A string describing the move.
 */
std::string Logger::moveToString(Move mv) {
  // Start coordinates
  std::string out;
  out += static_cast<char>('a' + mv.start.second);
  out += static_cast<char>('1' + mv.start.first);
  out += "->";
  // End coordinates
  out += static_cast<char>('a' + mv.end.second);
  out += static_cast<char>('1' + mv.end.first);
  return out;
}

/**
 * @brief Destructor that cleans up the logger. 
 *
 * If the destination stream is not std::cout, it flushes and deletes it.
 */
Logger::~Logger() {
  if (this->destination && this->destination != &std::cout) {
    destination->flush();
    delete destination;
  }
}
