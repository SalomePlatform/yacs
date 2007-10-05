#include "TypeConversions.hxx"
#include "NeutralXMLConv.hxx"

#include <iostream>
#include <sstream>
#include "Any.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

NeutralXml::NeutralXml(InputXmlPort* p)
  : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode())
{
}

void NeutralXml::put(const void *data) throw(ConversionException)
{
  DEBTRACE(" NeutralXml::put(const void *data)");
  put((YACS::ENGINE::Any *)data);
}

//!Convert received Neutral (Any *) value to XML value and send it to proxy port
 /*!
  *   \param data : YACS::ENGINE::Any object
  */

void NeutralXml::put(YACS::ENGINE::Any *data) throw(ConversionException)
{
  DEBTRACE("NeutralXml::put " << data);
  std::string sss = convertNeutralXml(edGetType(),data);
  ((InputXmlPort*)_port)->put((const char*)sss.c_str());
}
