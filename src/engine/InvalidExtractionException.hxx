#ifndef __INVALIDEXTRACTIONEXCEPTION_HXX__
#define __INVALIDEXTRACTIONEXCEPTION_HXX__

#include "Exception.hxx"
#include "TypeCode.hxx"

#include <string>

namespace YACS
{
  namespace ENGINE
  {    
    class InvalidExtractionException : public Exception
    {
    public:
      InvalidExtractionException(DynType kindExpected, DynType myKind);
    private:
      static const char TYPEOFEXCEPTION[];
    };
  }
}

#endif
