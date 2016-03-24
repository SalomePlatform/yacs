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
#include "PythonXMLConv.hxx"
#include "Node.hxx"
#include "PythonPorts.hxx"

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
  YACS::ENGINE::InterpreterSaveThread _loc;
  ((InputXmlPort*)_port)->put((const char*)sss.c_str());
}
