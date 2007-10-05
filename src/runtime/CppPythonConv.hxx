#ifndef __CPPPYTHONCONV_HXX__
#define __CPPPYTHONCONV_HXX__

#include "PythonPorts.hxx"

namespace YACS
{
  namespace ENGINE
  {
    //Proxy port to adapt Python port to Cpp

    class CppPy : public ProxyPort
    {
    public:
      CppPy(InputPyPort* p);
      virtual void put(const void *data)  throw(ConversionException);
      virtual void put(Any *a)  throw(ConversionException);
    };
    int isAdaptablePyObjectCpp(const TypeCode *t1, const TypeCode *t2);
  }
}

#endif
