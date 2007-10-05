#ifndef __CPPCORBACONV_HXX__
#define __CPPCORBACONV_HXX__

#include "CORBAPorts.hxx"

namespace YACS
{
  namespace ENGINE
  {

    // --- adaptator ports C++ -> Corba for several types

    class CppCorba : public ProxyPort
    {
    public:
      CppCorba(InputCorbaPort* p)
        : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode()) {}
      virtual void put(const void *data) throw(ConversionException);
      void put(Any *data) throw(ConversionException);
    };
    int isAdaptableCorbaCpp(const TypeCode *t1, const TypeCode *t2);
  }
}
#endif
