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

#include "XMLPythonConv.hxx"
#include "TypeConversions.hxx"

#include <libxml/parser.h>
#include <iostream>
#include <sstream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

XmlPython::XmlPython(InputPyPort* p)
  : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode())
{
}

void XmlPython::put(const void *data) throw(ConversionException)
{
  DEBTRACE((const char *)data);
  put((const char *)data);
}

//!Convert a XML (char *) to PyObject and push it into proxy port
 /*!
  *   \param data : Xml::char *
  */

void XmlPython::put(const char *data) throw(ConversionException)
{
  DEBTRACE(data);
  xmlDocPtr doc;
  xmlNodePtr cur;
  PyObject *ob=NULL;
{
  InterpreterUnlocker l;
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
          ob=convertXmlPyObject(edGetType(),doc,cur);
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
//  xmlCleanupParser();
#ifdef _DEVDEBUG_
  PyObject_Print(ob,stderr,Py_PRINT_RAW);
  cerr << endl;
#endif
  _port->put(ob);
  Py_XDECREF(ob);
  DEBTRACE("ob refcnt: " << ob->ob_refcnt);
  _port->setStringRef(data);
}
}
