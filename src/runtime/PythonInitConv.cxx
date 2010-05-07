//  Copyright (C) 2006-2010  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "TypeConversions.hxx"
#include "PythonInitConv.hxx"
#include "Node.hxx"

#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

PyInit::PyInit(InputPyPort* p)
  : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode())
{
}

//!Check a PyObject against the port type and send it to the input port
/*!
 *   \param data : PyObject object as a void * pointer
 */

void PyInit::put(const void *data) throw(ConversionException)
{
  put((PyObject *)data);
}

//!Check a PyObject against the port type and send it to the input port
/*!
 *   \param data : PyObject object
 */
void PyInit::put(PyObject *data) throw(ConversionException)
{
  DEBTRACE("PyInit::put(PyObject *data)"<<data->ob_refcnt);
  InterpreterUnlocker l;
  checkPyObject(edGetType(),data);
  //PyObject* ob=convertPyObjectPyObject(edGetType(),data);
  _port->put(data);
  //Py_DECREF(ob);
}
