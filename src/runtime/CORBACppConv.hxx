#ifndef __CORBACPPCONV_HXX__
#define __CORBACPPCONV_HXX__

#include <omniORB4/CORBA.h>

#include "InputPort.hxx"
#include "ConversionException.hxx"

namespace YACS
{
  namespace ENGINE
  {
    // Adaptator Ports Corba->C++

    class CorbaCpp : public ProxyPort
    {
    public:
      CorbaCpp(InputPort* p);
      virtual void put(const void *data) throw(ConversionException);
      void put(CORBA::Any *data) throw(ConversionException);
    };
    int isAdaptableCppCorba(const TypeCode *t1,const TypeCode *t2);
  }
}
#endif
