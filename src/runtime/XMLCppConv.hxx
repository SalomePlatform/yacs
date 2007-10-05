#ifndef XMLCPPCONV_HXX_
#define XMLCPPCONV_HXX_

#include "CppPorts.hxx"

namespace YACS
{
  namespace ENGINE
  {
    //Proxy port to adapt C++ port to XML

    class XmlCpp : public ProxyPort
    {
    public:
      XmlCpp(InputPort* p);
      virtual void put(const void *data)  throw(ConversionException);
      void put(const char *data) throw(ConversionException);
    };
    int isAdaptableCppXml(const TypeCode *t1, const TypeCode *t2);
  }
}

#endif /*XMLCPPCONV_HXX_*/
