// Copyright (C) 2006-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "TypeConversions.hxx"
#include "XMLNeutralConv.hxx"

#include <libxml/parser.h>
#include <iostream>
#include <sstream>
#include "Any.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

XmlNeutral::XmlNeutral(InputPort* p)
  : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode())
{
}

void XmlNeutral::put(const void *data) throw(ConversionException)
{
  DEBTRACE(" XmlNeutral::put(const void *data)");
  put((const char *)data);
}

//!Convert received XML (char *) value to Neutral (Any *) value and send it to proxy port
 /*!
  *   \param data : Xml::char *
  */

void XmlNeutral::put(const char *data) throw(ConversionException)
{
  DEBTRACE("XmlNeutral::put " << data);
  xmlDocPtr doc;
  xmlNodePtr cur;
  YACS::ENGINE::Any *ob=NULL;
{
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
          ob=convertXmlNeutral(edGetType(),doc,cur);
          break;
        }
      cur = cur->next;
    }
  xmlFreeDoc(doc);
  if(ob==NULL)
    {
      stringstream msg;
      msg << "Problem in conversion: incorrect XML value";
      msg << " (" << __FILE__ << ":" << __LINE__ << ")";
      throw ConversionException(msg.str());
    }
}
  _port->put(ob);
  ob->decrRef();
}
