#ifndef __XMLPYTHONCONV_HXX__
#define __XMLPYTHONCONV_HXX__

#include "PythonPorts.hxx"

namespace YACS
{
  namespace ENGINE
  {
    //Proxy port to adapt Python port to XML

    class XmlPython : public ProxyPort
    {
    public:
      XmlPython(InputPyPort* p);
      virtual void put(const void *data)  throw(ConversionException);
      void put(const char *data) throw(ConversionException);
    };
  }
}

#endif
