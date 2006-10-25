#ifndef __CORBACORBACONV_HXX__
#define __CORBACORBACONV_HXX__

#include "RuntimeSALOME.hxx"

namespace YACS
{
  namespace ENGINE
  {

    // Ports adaptateurs Corba->Corba pour les différents types

    class CorbaCorba : public ProxyPort
    {
    public:
      CorbaCorba(InputCorbaPort* p);
      virtual void put(const void *data) throw(ConversionException);
      void put(CORBA::Any *data) throw(ConversionException);
    };

  }
}
#endif
