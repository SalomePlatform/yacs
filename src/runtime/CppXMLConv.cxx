
#include "TypeConversions.hxx"
#include "CppXMLConv.hxx"

#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

namespace YACS 
{
  namespace ENGINE 
  {
    std::string convertCppXml(const TypeCode *t, void *ob)
    {
      return convertNeutralXml(t, (Any *) ob);
    }

    CppXml::CppXml(InputXmlPort* p)
      : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode())
    {
      DEBTRACE("proxy port from C++ to XML");
    }

    //!Convert a YACS::ENGINE::Any that is convertible to Xml::char * and send it to proxy port
    /*!
     *   \param data : YACS::ENGINE::Any object as a void * pointer
     */

    void CppXml::put(const void *data) throw(ConversionException)
    {
      put((YACS::ENGINE::Any *)data);
    }

    //!Convert a YACS::ENGINE::Any that is convertible to Xml::char * and send it to proxy port
    /*!
     *   \param data : YACS::ENGINE::Any object
     */
    void CppXml::put(YACS::ENGINE::Any *data) throw(ConversionException)
    {
      DEBTRACE("CppXml::put");
      std::string s = convertCppXml(edGetType(), data);
      DEBTRACE(s);
      DEBTRACE(_port->getName());
      ((InputXmlPort*)_port)->put(s.c_str());
      DEBTRACE("End CppXml::put");
    }

    int isAdaptableXmlCpp(const TypeCode *t1, const TypeCode *t2)
    {
      return isAdaptableXmlNeutral(t1, t2);
    }
  }
}
