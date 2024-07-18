#include "logger.h"
#include "chess-board.h"
Logger::Logger(bool silent, std::ostream *destination)
    : silent(silent), destination(destination) {
  try {
    this->log("LOGGER CREATED");
  } catch (std::exception &e) {
    throwNotCriticalError("Unable to create logger: " + std::string(e.what()));
    isOK=false;
  }
}
std::string Logger::moveToString(Move mv)
{
  std::string out;
  out+='a'+mv.start.second;
  out+='1'+mv.start.first;
  out+="->";
  out+='a'+mv.end.second;
  out+='1'+mv.end.first;
  return out;
}

Logger::~Logger() 
{
  if(this->destination&&this->destination!=&std::cout)
  {
    destination->flush();
    delete destination;
  }
}