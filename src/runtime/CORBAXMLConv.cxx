
#include "TypeConversions.hxx"
#include "CORBAXMLConv.hxx"

#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

CorbaXml::CorbaXml(InputXmlPort* p)
  : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode())
{
}

//!Convert a CORBA::Any that is convertible to Xml::char * and send it to proxy port
/*!
 *   \param data : CORBA::Any object as a void * pointer
 */

void CorbaXml::put(const void *data) throw(ConversionException)
{
  put((CORBA::Any *)data);
}

//!Convert a CORBA::Any that is convertible to Xml::char * and send it to proxy port
/*!
 *   \param data : CORBA::Any object
 */
void CorbaXml::put(CORBA::Any *data) throw(ConversionException)
{
  DEBTRACE("CorbaXml::put" );
  std::string sss = convertCorbaXml(edGetType(),data);
  ((InputXmlPort*)_port)->put((const char*)sss.c_str());
}
