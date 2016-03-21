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

#ifndef _LOGGER_HXX_
#define _LOGGER_HXX_

#include "YACSlibEngineExport.hxx"

#include <string>
#include <vector>

namespace YACS
{
  namespace ENGINE
  {
    class LogRecord;

/*! \brief Class for logging error messages
 *
 */
    class YACSLIBENGINE_EXPORT Logger
    {
    public:
      Logger(const std::string& name);
      virtual ~Logger();
      virtual const std::string& getName();
      virtual void log(int level,const std::string& message,const char* filename,int line);
      virtual void error(const std::string& message,const char* filename,int line);
      virtual void fatal(const std::string& message,const char* filename,int line);
      virtual void warning(const std::string& message,const char* filename,int line);
      virtual LogRecord* makeRecord(const std::string& name,int level,const std::string& message,const char* filename,int line);
      virtual void handle(LogRecord* record);
      virtual void reset();
      virtual std::string getStr();
      virtual bool isEmpty();
      virtual bool hasErrors();
    protected:
      std::string _name;
      std::vector<LogRecord*> _records;
    };
  }
}

#endif
