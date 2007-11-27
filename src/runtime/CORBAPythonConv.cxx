
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
  CORBA::Double d;
  *data >>=d;
  PyObject *ob;
  {
  InterpreterUnlocker loc;
  ob=PyFloat_FromDouble(d);
  DEBTRACE("ob refcnt: " << ob->ob_refcnt );
  }
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
  CORBA::Long l;
  *data >>=l;
  PyObject *ob;
  {
  InterpreterUnlocker loc;
  ob=PyLong_FromLong(l);
  DEBTRACE("ob refcnt: " << ob->ob_refcnt );
  }
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
  const char *s;
  *data >>=s;
  PyObject *ob;
  {
  InterpreterUnlocker loc;
  ob=PyString_FromString(s);
  DEBTRACE("ob refcnt: " << ob->ob_refcnt );
  }
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
  PyObject* ob=convertCorbaPyObject(edGetType(),data);
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
  PyObject *ob;
  {
    InterpreterUnlocker loc;
    ob=convertCorbaPyObject(edGetType(),data);
    DEBTRACE("ob refcnt: " << ob->ob_refcnt );
    _port->put(ob);
    Py_DECREF(ob);
    DEBTRACE("ob refcnt: " << ob->ob_refcnt );
  }
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
  PyObject *ob;
  {
  InterpreterUnlocker loc;
  ob=convertCorbaPyObject(edGetType(),data);
  DEBTRACE("ob refcnt: " << ob->ob_refcnt );
  _port->put(ob);
  Py_DECREF(ob);
  DEBTRACE("ob refcnt: " << ob->ob_refcnt );
  }
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
  PyObject *ob;
  {
    InterpreterUnlocker loc;
    ob=convertCorbaPyObject(edGetType(),data);
    DEBTRACE("ob refcnt: " << ob->ob_refcnt );
    _port->put(ob);
    Py_DECREF(ob);
    DEBTRACE("ob refcnt: " << ob->ob_refcnt );
  }
}
