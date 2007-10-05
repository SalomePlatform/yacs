#include "TypeConversions.hxx"
#include "PythonXMLConv.hxx"
#include "Node.hxx"

#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

PyXml::PyXml(InputXmlPort* p)
  : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode())
{
}

//!Convert a PyObject that is convertible to Xml::char * and send it to proxy port
/*!
 *   \param data : PyObject object as a void * pointer
 */

void PyXml::put(const void *data) throw(ConversionException)
{
  put((PyObject *)data);
}

//!Convert a PyObject that is convertible to Xml::char * and send it to proxy port
/*!
 *   \param data : PyObject object
 */
void PyXml::put(PyObject *data) throw(ConversionException)
{
  DEBTRACE("PyXml::put" );
  std::string sss = convertPyObjectXml(edGetType(),data);
  ((InputXmlPort*)_port)->put((const char*)sss.c_str());
}
