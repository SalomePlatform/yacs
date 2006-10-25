#ifndef __CORBAXMLCONV_HXX__
#define __CORBAXMLCONV_HXX__

#include <omniORB4/CORBA.h>

#include "XMLPorts.hxx"

namespace YACS
{
  namespace ENGINE
  {
    // Ports adaptateurs Corba->Xml pour les différents types

    class CorbaXml : public ProxyPort
    {
    public:
      CorbaXml(InputXmlPort* p);
      virtual void put(const void *data) throw(ConversionException);
      void put(CORBA::Any *data) throw(ConversionException);
    };
  }
}
#endif
