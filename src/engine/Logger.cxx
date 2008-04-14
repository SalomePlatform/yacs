#include "Logger.hxx"
#include "LogRecord.hxx"

#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;

Logger::Logger(const std::string& name):_name(name)
{
}

Logger::~Logger()
{
  reset();
}

const std::string& Logger::getName()
{
  return _name;
}

void Logger::log(int level,const std::string& message,const char* filename,int line)
{
  LogRecord* record=makeRecord(getName(),level,message,filename,line);
  handle(record);
}

void Logger::error(const std::string& message,const char* filename,int line)
{
  log(LogRecord::ERROR,message,filename,line);
}

void Logger::fatal(const std::string& message,const char* filename,int line)
{
  log(LogRecord::FATAL,message,filename,line);
}

void Logger::warning(const std::string& message,const char* filename,int line)
{
  log(LogRecord::WARNING,message,filename,line);
}

LogRecord* Logger::makeRecord(const std::string& name,int level,const std::string& message,const char* filename,int line)
{
  return new LogRecord(name,level,message,filename,line);
}

void Logger::handle(LogRecord* record)
{
  _records.push_back(record);
}

void Logger::reset()
{
  std::vector<LogRecord*>::iterator it;
  for( it = _records.begin(); it != _records.end(); it++)
    {
      delete (*it);
    }
  _records.clear();
}

std::string Logger::getStr()
{
  std::vector<LogRecord*>::iterator it;
  std::string msg;
  for( it = _records.begin(); it != _records.end(); it++)
    {
      msg=msg + (*it)->getStr() + '\n';
    }
  return msg;
}

bool Logger::isEmpty()
{
  return _records.size() == 0;
}

bool Logger::hasErrors()
{
  bool ret=false;
  std::vector<LogRecord*>::iterator it;
  for( it = _records.begin(); it != _records.end(); it++)
    {
      if((*it)->_level > LogRecord::WARNING)
        {
          ret=true;
          break;
        }
    }
  return ret;
}
