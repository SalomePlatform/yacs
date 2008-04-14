#include "LogRecord.hxx"

#include <sstream>
#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;

LogRecord::LogRecord(const std::string& name,int level,const std::string& message, const char* file, int line)
        :_loggerName(name),_level(level),_message(message),_fileName((char*)file),_line(line)
{
}

LogRecord::~LogRecord()
{
}

std::string LogRecord::getLevelName(int level)
{
  switch(level)
    {
    case FATAL:
      return "FATAL";
    case ERROR:
      return "ERROR";
    case WARNING:
      return "WARNING";
    case INFO:
      return "INFO";
    case DEBUG:
      return "DEBUG";
    default:
      return "NOTSET";
    }
}

std::string LogRecord::getStr()
{
  std::stringstream msg;
  msg << "LogRecord: " << _loggerName << ":" << getLevelName(_level) << ":" << _message;
  if(_fileName != "")
    msg << " (" << _fileName << ":" << _line << ")";
  return msg.str();
}

