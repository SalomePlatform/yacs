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

      LogRecord(const std::string& name,int level,const std::string& message, const char* file, int line);
      virtual ~LogRecord();
      /** The logger name. */
      const std::string _loggerName;
      /** The message of logging record */
      const std::string _message;
      /** Level of logging record */
      int _level;
      /** filename of logging record */
      char* _fileName;
      /** line number of logging record */
      int _line;
      virtual std::string getLevelName(int level);

      virtual std::string getStr();

    protected:
    };
  }
}

#endif
