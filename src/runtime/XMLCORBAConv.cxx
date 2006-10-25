
#include "XMLCORBAConv.hxx"
#include "CORBAXMLConv.hxx"
#include "TypeConversions.hxx"

#include <libxml/parser.h>
#include <iostream>

using namespace YACS::ENGINE;
using namespace std;

XmlCorba::XmlCorba(InputCorbaPort* p)
  : ProxyPort(p), Port(p->getNode())
{
  cerr << "proxy port from XML to CORBA" << endl;
}

void XmlCorba::put(const void *data) throw(ConversionException)
{
  cerr << " XmlCorba::put(const void *data)" << endl;
  put((const char *)data);
}

//!Convertit la valeur XML (char *) recue en CORBA::Any et la transmet au proxy port
 /*!
  *   \param data : Xml::char *
  */

void XmlCorba::put(const char *data) throw(ConversionException)
{
  cerr << "XmlCorba::put " << data << endl;
  xmlDocPtr doc;
  xmlNodePtr cur;
  CORBA::Any *a;
  
  doc = xmlParseMemory(data, strlen(data));
  cur = xmlDocGetRootElement(doc);
  while (cur != NULL)
    {
      if ((!xmlStrcmp(cur->name, (const xmlChar *)"value")))
	{
	  a=convertCorbaXml(type(),doc,cur);
	  break;
	}
      cur = cur->next;
    }
  xmlFreeDoc(doc);
  xmlCleanupParser();
  _port->put(a);
  cerr << "Fin XmlCorba" << endl;
}
