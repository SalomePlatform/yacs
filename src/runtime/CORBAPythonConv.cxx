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
#include "RuntimeSALOME.hxx"
#include "CORBAPythonConv.hxx"
#include "PythonPorts.hxx"

#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

CorbaPyDouble::CorbaPyDouble(InputPyPort* p)
    : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode()) 
{}

void CorbaPyDouble::put(const void *data) throw(ConversionException)
{
  put((CORBA::Any *)data);
}

//!Convert a CORBA::Any double to PyObject Double
/*!
 *   \param data : CORBA::Any object
 */
void CorbaPyDouble::put(CORBA::Any *data) throw(ConversionException)
{
  InterpreterUnlocker loc;
  PyObject* ob=convertCorbaPyObject(edGetType(),data);
  DEBTRACE("ob refcnt: " << ob->ob_refcnt );
  _port->put(ob);
  Py_DECREF(ob);
}


CorbaPyInt::CorbaPyInt(InputPyPort* p)
    : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode()) 
{}

void CorbaPyInt::put(const void *data) throw(ConversionException)
{
  put((CORBA::Any *)data);
}

//!Convert a CORBA::Any long to a PyObject Int
/*!
 *   \param data : CORBA::Any object
 */
void CorbaPyInt::put(CORBA::Any *data) throw(ConversionException)
{
  InterpreterUnlocker loc;
  PyObject* ob=convertCorbaPyObject(edGetType(),data);
  DEBTRACE("ob refcnt: " << ob->ob_refcnt );
  _port->put(ob);
  Py_DECREF(ob);
}

CorbaPyString::CorbaPyString(InputPyPort* p)
    : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode()) 
{}
void CorbaPyString::put(const void *data) throw(ConversionException)
{
  put((CORBA::Any *)data);
}

//!Convert a CORBA::Any string to a PyObject String
/*!
 *   \param data : CORBA::Any object
 */
void CorbaPyString::put(CORBA::Any *data) throw(ConversionException)
{
  InterpreterUnlocker loc;
  PyObject* ob=convertCorbaPyObject(edGetType(),data);
  DEBTRACE("ob refcnt: " << ob->ob_refcnt );
  _port->put(ob);
  Py_DECREF(ob);
}

CorbaPyBool::CorbaPyBool(InputPyPort* p)
    : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode()) 
{}

/*!Convert a CORBA::Any boolean to a PyObject boolean
 * It's only a wrapper around put(CORBA::Any *data)
 */
void CorbaPyBool::put(const void *data) throw(ConversionException)
{
  put((CORBA::Any *)data);
}

//!Convert a CORBA::Any boolean to a PyObject boolean
/*!
 *   \param data : CORBA::Any object
 */
void CorbaPyBool::put(CORBA::Any *data) throw(ConversionException)
{
  InterpreterUnlocker loc;
  PyObject* ob=convertCorbaPyObject(edGetType(),data);
  DEBTRACE("ob refcnt: " << ob->ob_refcnt );
  _port->put(ob);
  Py_DECREF(ob);
}

CorbaPyObjref::CorbaPyObjref(InputPyPort* p)
    : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode()) 
{}

void CorbaPyObjref::put(const void *data) throw(ConversionException)
{
  put((CORBA::Any *)data);
}

//!Convert a CORBA::Any Objref to PyObject Objref
/*!
 *   \param data : CORBA::Any object
 */
void CorbaPyObjref::put(CORBA::Any *data) throw(ConversionException)
{
  InterpreterUnlocker loc;
  PyObject* ob=convertCorbaPyObject(edGetType(),data);
  DEBTRACE("ob refcnt: " << ob->ob_refcnt );
  _port->put(ob);
  Py_DECREF(ob);
}

//!Class to convert a CORBA::Any sequence to a PyObject Sequence
/*!
 *   \param p : input Python port to adapt to Corba output port
 */
CorbaPySequence::CorbaPySequence(InputPyPort* p)
  : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode())
{
  _dynfactory = getSALOMERuntime()->getDynFactory();
}

void CorbaPySequence::put(const void *data) throw(ConversionException)
{
  put((CORBA::Any *)data);
}

//!Convert a CORBA::Any sequence to PyObject Sequence
/*!
 *   \param data : CORBA::Any object
 */
void CorbaPySequence::put(CORBA::Any *data) throw(ConversionException)
{
  InterpreterUnlocker loc;
  PyObject* ob=convertCorbaPyObject(edGetType(),data);
  DEBTRACE("ob refcnt: " << ob->ob_refcnt );
  _port->put(ob);
  Py_DECREF(ob);
}

//!Class to convert a CORBA::Any struct into a PyObject struct
/*!
 *   \param p : input Python port to adapt to Corba output port
 */
CorbaPyStruct::CorbaPyStruct(InputPyPort* p)
  : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode())
{
}

void CorbaPyStruct::put(const void *data) throw(ConversionException)
{
  put((CORBA::Any *)data);
}

//!Convert a CORBA::Any sequence to PyObject Sequence
/*!
 *   \param data : CORBA::Any object
 */
void CorbaPyStruct::put(CORBA::Any *data) throw(ConversionException)
{
  InterpreterUnlocker loc;
  PyObject* ob=convertCorbaPyObject(edGetType(),data);
  DEBTRACE("ob refcnt: " << ob->ob_refcnt );
  _port->put(ob);
  Py_DECREF(ob);
}
