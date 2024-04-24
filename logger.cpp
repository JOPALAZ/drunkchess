#include"logger.h"
Logger::Logger(bool silent,std::ostream* destination)
:silent(silent),destination(destination)
{
    try
    {
        this->log("ЛОГГЕР СОЗДАН БЛЯТЬ1!!!");
    }
    catch(std::exception& e)
    {
       throwNotCriticalError("Unable to create logger: "+std::string(e.what()));
    }
}

Logger::~Logger()
{
    
}