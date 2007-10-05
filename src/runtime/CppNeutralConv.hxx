#ifndef __CPPNEUTRALCONV_HXX__
#define __CPPNEUTRALCONV_HXX__

#include "InputPort.hxx"

namespace YACS
{
  namespace ENGINE
  {
    //Proxy port to adapt Neutral port to Python port

    class CppNeutral : public ProxyPort
    {
    public:
      CppNeutral(InputPort* p);
      virtual void put(const void *data)  throw(ConversionException);
      void put(Any *data) throw(ConversionException);
    };
    int isAdaptableNeutralCpp(const TypeCode * t1, const TypeCode * t2);
  }
}

#endif
