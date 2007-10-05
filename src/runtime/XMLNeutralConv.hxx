#ifndef __XMLNEUTRALCONV_HXX__
#define __XMLNEUTRALCONV_HXX__

#include "InputPort.hxx"

namespace YACS
{
  namespace ENGINE
  {
    //Proxy port to adapt Neutral port to XML

    class XmlNeutral : public ProxyPort
    {
    public:
      XmlNeutral(InputPort* p);
      virtual void put(const void *data)  throw(ConversionException);
      void put(const char *data) throw(ConversionException);
    };
  }
}

#endif
