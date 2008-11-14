#ifndef __CONVERSIONEXCEPTION_HXX__
#define __CONVERSIONEXCEPTION_HXX__

#include "Exception.hxx"

#include <string>

namespace YACS
{
  namespace ENGINE
  {    
    class ConversionException : public Exception
    {
    public:
      ConversionException(const std::string& what,bool prefix=true);
      virtual ~ConversionException() throw();
    private:
      static const char TYPEOFEXCEPTION[];
    };
  }
}

#endif
