#ifndef _LOGGER_HXX_
#define _LOGGER_HXX_

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
    class Logger
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
