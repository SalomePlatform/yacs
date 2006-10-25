
#include "CORBAXMLConv.hxx"
#include "TypeConversions.hxx"

#include <iostream>

using namespace YACS::ENGINE;
using namespace std;

CorbaXml::CorbaXml(InputXmlPort* p)
  : ProxyPort(p), Port(p->getNode())
{
  cerr << "proxy port from CORBA to XML" << endl;
}

//!Convertit un Any convertible en Xml::char *
/*!
 *   \param data : CORBA::Any object
 */

void CorbaXml::put(const void *data) throw(ConversionException)
{
  put((CORBA::Any *)data);
}

void CorbaXml::put(CORBA::Any *data) throw(ConversionException)
{
  //conversion du Any data en any attendu (de type type())

  cerr << "CorbaXml::put" << endl;
  char *a = convertXmlCorba(type(),data);
  cerr << a << endl;
  cerr << _port->getName() << endl;
  cerr << _port->getImpl() << endl;
  _port->put((const char*)a);
  cerr << "Fin CorbaXml::put" << endl;
}
