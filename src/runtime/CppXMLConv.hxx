#ifndef __CPPXMLCONV_HXX__
#define __CPPXMLCONV_HXX__

#include "XMLPorts.hxx"

namespace YACS
{
  namespace ENGINE
  {
    // Adaptator Ports Corba->Xml for several types

    class CppXml : public ProxyPort
    {
    public:
      CppXml(InputXmlPort* p);
      virtual void put(const void *data) throw(ConversionException);
      void put(YACS::ENGINE::Any *data) throw(ConversionException);
    };
    int isAdaptableXmlCpp(const TypeCode *t1, const TypeCode *t2);
  }
}
#endif
