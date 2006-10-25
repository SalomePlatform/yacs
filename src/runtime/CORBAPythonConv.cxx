
#include "CORBAPythonConv.hxx"
#include "TypeConversions.hxx"
#include "RuntimeSALOME.hxx"

#include <iostream>

using namespace YACS::ENGINE;
using namespace std;

void CorbaPyDouble::put(const void *data) throw(ConversionException)
{
  put((CORBA::Any *)data);
}

//!Convertit un CORBA::Any de type double en PyObject de type Double
/*!
 *   \param data : CORBA::Any object
 */

void CorbaPyDouble::put(CORBA::Any *data) throw(ConversionException)
{
  CORBA::Double d;
  *data >>=d;
  PyObject *ob=PyFloat_FromDouble(d);
  cerr << "ob refcnt: " << ob->ob_refcnt << endl;
  _port->put(ob);
}


void CorbaPyInt::put(const void *data) throw(ConversionException)
{
  put((CORBA::Any *)data);
}

//!Convertit un CORBA::Any de type entier en PyObject de type entier
/*!
 *   \param data : CORBA::Any object
 */

void CorbaPyInt::put(CORBA::Any *data) throw(ConversionException)
{
  CORBA::Long l;
  *data >>=l;
  PyObject *ob=PyLong_FromLong(l);
  cerr << "ob refcnt: " << ob->ob_refcnt << endl;
  _port->put(ob);
}


void CorbaPyString::put(const void *data) throw(ConversionException)
{
  put((CORBA::Any *)data);
}

//!Convertit un CORBA::Any de type string en PyObject de type string
/*!
 *   \param data : CORBA::Any object
 */

void CorbaPyString::put(CORBA::Any *data) throw(ConversionException)
{
  char *s;
  *data >>=s;
  PyObject *ob=PyString_FromString(s);
  cerr << "ob refcnt: " << ob->ob_refcnt << endl;
  _port->put(ob);
}


void CorbaPyObjref::put(const void *data) throw(ConversionException)
{
  put((CORBA::Any *)data);
}

//!Convertit un CORBA::Any de type Objref en PyObject de type Objref
/*!
 *   \param data : CORBA::Any object
 */

void CorbaPyObjref::put(CORBA::Any *data) throw(ConversionException)
{
  CORBA::Object_ptr ObjRef ;
  *data >>= (CORBA::Any::to_object ) ObjRef ;
  //hold_lock is true: caller is supposed to hold the GIL. 
  //omniorb will not take the GIL
  PyObject *ob = getSALOMERuntime()->getApi()->cxxObjRefToPyObjRef(ObjRef, 1);
  cerr << "ob refcnt: " << ob->ob_refcnt << endl;
  _port->put(ob);
}

CorbaPySequence::CorbaPySequence(InputPyPort* p)
  : ProxyPort(p), Port(p->getNode())
{
  _dynfactory = getSALOMERuntime()->getDynFactory();
}

void CorbaPySequence::put(const void *data) throw(ConversionException)
{
  put((CORBA::Any *)data);
}

void CorbaPySequence::put(CORBA::Any *data) throw(ConversionException)
{
  cerr << "PyCorbaSequence::put" << endl;
  PyObject *ob=convertPyObjectCorba(type(),data);
  cerr << "ob refcnt: " << ob->ob_refcnt << endl;
  cerr << "Sequence= ";
  PyObject_Print(ob,stdout,Py_PRINT_RAW);
  cerr << endl;
  _port->put(ob);
}

