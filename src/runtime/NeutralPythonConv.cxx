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
#include "NeutralPythonConv.hxx"
#include "RuntimeSALOME.hxx"

#include <iostream>
#include <string>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

void NeutralPyDouble::put(const void *data) throw(ConversionException)
{
  put((YACS::ENGINE::Any *)data);
}

//!Convert a YACS::ENGINE::Any (double) to a PyObject (PyFloat)
/*!
 *   \param data : YACS::ENGINE::Any object
 */
void NeutralPyDouble::put(YACS::ENGINE::Any *data) throw(ConversionException)
{
  InterpreterUnlocker loc;
  PyObject* ob=convertNeutralPyObject(edGetType(),data);
  DEBTRACE( "ob refcnt: " << ob->ob_refcnt );
  _port->put(ob);
  Py_DECREF(ob);
}


void NeutralPyInt::put(const void *data) throw(ConversionException)
{
  put((YACS::ENGINE::Any *)data);
}

//!Convert a YACS::ENGINE::Any (integer) to a PyObject (PyLong)
/*!
 *   \param data : YACS::ENGINE::Any object
 */
void NeutralPyInt::put(YACS::ENGINE::Any *data) throw(ConversionException)
{
  InterpreterUnlocker loc;
  PyObject* ob=convertNeutralPyObject(edGetType(),data);
  DEBTRACE( "ob refcnt: " << ob->ob_refcnt );
  _port->put(ob);
  Py_DECREF(ob);
}

void NeutralPyString::put(const void *data) throw(ConversionException)
{
  put((YACS::ENGINE::Any *)data);
}

//!Convert a YACS::ENGINE::Any (string) to a PyObject (PyString)
/*!
 *   \param data : YACS::ENGINE::Any object
 */
void NeutralPyString::put(YACS::ENGINE::Any *data) throw(ConversionException)
{
  InterpreterUnlocker loc;
  PyObject* ob=convertNeutralPyObject(edGetType(),data);
  DEBTRACE( "ob refcnt: " << ob->ob_refcnt );
  _port->put(ob);
  Py_DECREF(ob);
}

void NeutralPyBool::put(const void *data) throw(ConversionException)
{
  put((YACS::ENGINE::Any *)data);
}

//!Convert a YACS::ENGINE::Any (bool) to a PyObject (PyLong)
/*!
 *   \param data : YACS::ENGINE::Any object
 */
void NeutralPyBool::put(YACS::ENGINE::Any *data) throw(ConversionException)
{
  InterpreterUnlocker loc;
  PyObject* ob=convertNeutralPyObject(edGetType(),data);
  DEBTRACE( "ob refcnt: " << ob->ob_refcnt );
  _port->put(ob);
  Py_DECREF(ob);
}

void NeutralPyObjref::put(const void *data) throw(ConversionException)
{
  put((YACS::ENGINE::Any *)data);
}

//!Convert a Neutral::Any Objref to a PyObject Objref
/*!
 *   \param data : Neutral::Any object
 */
void NeutralPyObjref::put(YACS::ENGINE::Any *data) throw(ConversionException)
{
  InterpreterUnlocker loc;
  PyObject* ob=convertNeutralPyObject(edGetType(),data);
  DEBTRACE( "ob refcnt: " << ob->ob_refcnt );
  _port->put(ob);
  Py_DECREF(ob);
  DEBTRACE( "ob refcnt: " << ob->ob_refcnt );
}

void NeutralPySequence::put(const void *data) throw(ConversionException)
{
  put((YACS::ENGINE::Any *)data);
}

//!Convert a Neutral::Any sequence to a PyObject Sequence
/*!
 *   \param data : Neutral::Any object
 */
void NeutralPySequence::put(YACS::ENGINE::Any *data) throw(ConversionException)
{
  DEBTRACE( "--------NeutralPySequence::put" );
  InterpreterUnlocker loc;
  PyObject* ob=convertNeutralPyObject(edGetType(),data);
  DEBTRACE( "ob refcnt: " << ob->ob_refcnt );
#ifdef _DEVDEBUG_
  cerr << "Sequence= ";
  PyObject_Print(ob,stderr,Py_PRINT_RAW);
  cerr << endl;
#endif
  _port->put(ob);
  Py_DECREF(ob);
  DEBTRACE( "ob refcnt: " << ob->ob_refcnt );
}

void NeutralPyStruct::put(const void *data) throw(ConversionException)
{
  put((YACS::ENGINE::Any *)data);
}

//!Convert a Neutral::Any Struct to a PyObject Struct
/*!
 *   \param data : Neutral::Any object
 */
void NeutralPyStruct::put(YACS::ENGINE::Any *data) throw(ConversionException)
{
  InterpreterUnlocker loc;
  PyObject* ob=convertNeutralPyObject(edGetType(),data);
  DEBTRACE( "ob refcnt: " << ob->ob_refcnt );
  _port->put(ob);
  Py_DECREF(ob);
  DEBTRACE( "ob refcnt: " << ob->ob_refcnt );
}
