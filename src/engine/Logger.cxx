// Copyright (C) 2006-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

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
