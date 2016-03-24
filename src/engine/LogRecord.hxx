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

#ifndef _LOGRECORD_HXX_
#define _LOGRECORD_HXX_

#include <string>

namespace YACS
{
  namespace ENGINE
  {
/*! \brief Class for logging record
 *
 */
    class LogRecord
    {
    public:
      typedef int LogLevel;
      static const LogLevel FATAL    = 50;
      static const LogLevel ERROR    = 40;
      static const LogLevel WARNING  = 30;
      static const LogLevel INFO     = 20;
      static const LogLevel DEBUG    = 10;
      static const LogLevel NOTSET   = 0;

      LogRecord(const std::string& name,int level,const std::string& message, const std::string& file, int line);
      virtual ~LogRecord();
      /** The logger name. */
      const std::string _loggerName;
      /** The message of logging record */
      const std::string _message;
      /** Level of logging record */
      int _level;
      /** filename of logging record */
      std::string _fileName;
      /** line number of logging record */
      int _line;
      virtual std::string getLevelName(int level);

      virtual std::string getStr();

    protected:
    };
  }
}

#endif
