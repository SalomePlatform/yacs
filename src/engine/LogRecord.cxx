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

#include "LogRecord.hxx"

#include <sstream>
#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;

LogRecord::LogRecord(const std::string& name,int level,const std::string& message, const std::string& file, int line)
        :_loggerName(name),_level(level),_message(message),_fileName(file),_line(line)
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

