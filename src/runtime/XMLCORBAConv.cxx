
#include "TypeConversions.hxx"
#include "XMLCORBAConv.hxx"
#include "CORBAXMLConv.hxx"

#include <libxml/parser.h>
#include <iostream>
#include <sstream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

XmlCorba::XmlCorba(InputCorbaPort* p)
  : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode())
{
}

void XmlCorba::put(const void *data) throw(ConversionException)
{
  DEBTRACE((const char *)data);
  put((const char *)data);
}

//!Convert a XML (char *) to CORBA::Any  and push it in the proxy port
 /*!
  *   \param data : Xml::char *
  */
void XmlCorba::put(const char *data) throw(ConversionException)
{
  DEBTRACE(data);
  xmlDocPtr doc;
  xmlNodePtr cur;
  CORBA::Any *a=NULL;
  
  doc = xmlParseMemory(data, strlen(data));
  if (doc == NULL ) 
    {
      stringstream msg;
      msg << "Problem in conversion: XML Document not parsed successfully ";
      msg << " (" << __FILE__ << ":" << __LINE__ << ")";
      throw ConversionException(msg.str());
    }
  cur = xmlDocGetRootElement(doc);
  if (cur == NULL) 
    {
      xmlFreeDoc(doc);
      stringstream msg;
      msg << "Problem in conversion: empty XML Document";
      msg << " (" << __FILE__ << ":" << __LINE__ << ")";
      throw ConversionException(msg.str());
    }
  while (cur != NULL)
    {
      if ((!xmlStrcmp(cur->name, (const xmlChar *)"value")))
        {
          try
            {
              a=convertXmlCorba(edGetType(),doc,cur);
            }
          catch(ConversionException)
            {
              throw;
            }
          catch(...)
            {
              stringstream msg;
              msg << "Problem in conversion: kind= " << edGetType()->kind() ;
              msg << " (" << __FILE__ << ":" << __LINE__ << ")";
              throw ConversionException(msg.str());
            }
          break;
        }
      cur = cur->next;
    }
  xmlFreeDoc(doc);
  if(a==NULL)
    {
      stringstream msg;
      msg << "Problem in conversion: incorrect XML value";
      msg << " (" << __FILE__ << ":" << __LINE__ << ")";
      throw ConversionException(msg.str());
    }

  //xmlCleanupParser();
  _port->put(a);
  _port->setStringRef(data);
  //delete Any that has been allocated by convertXmlCorba
  delete a;
}
