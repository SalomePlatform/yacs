#ifndef __CORBANEUTRALCONV_HXX__
#define __CORBANEUTRALCONV_HXX__

#include <omniORB4/CORBA.h>

#include "InputPort.hxx"
#include "ConversionException.hxx"

namespace YACS
{
  namespace ENGINE
  {
    // Adaptator Ports Corba->Neutral for several types

    class CorbaNeutral : public ProxyPort
    {
    public:
      CorbaNeutral(InputPort* p);
      virtual void put(const void *data) throw(ConversionException);
      void put(CORBA::Any *data) throw(ConversionException);
    };
  }
}
#endif
