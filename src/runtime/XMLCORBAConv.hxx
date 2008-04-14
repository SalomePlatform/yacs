#ifndef __XMLCORBACONV_HXX__
#define __XMLCORBACONV_HXX__

#include "InputPort.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class InputCorbaPort;

    //Adaptator Ports Xml->Corba for several types

    class XmlCorba : public ProxyPort
    {
    public:
      XmlCorba(InputCorbaPort* p);
      virtual void put(const void *data)  throw(ConversionException);
      void put(const char *data) throw(ConversionException);
    };
  }
}

#endif
