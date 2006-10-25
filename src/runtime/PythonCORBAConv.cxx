
#include "PythonCORBAConv.hxx"
#include "TypeConversions.hxx"
#include "RuntimeSALOME.hxx"

#include <iostream>

using namespace YACS::ENGINE;
using namespace std;


void PyCorbaInt::put(const void *data)  throw(ConversionException)
{
  put((PyObject *)data);
}

//!Convertit un PyObject de type entier en CORBA::Any entier
/*!
 *   \param data : python object
 */

void PyCorbaInt::put(PyObject *data)  throw(ConversionException)
{
  CORBA::Long l= PyLong_AsLong(data);
  CORBA::Any a;
  a <<= l;
  _port->put(&a);
}


void PyCorbaString::put(const void *data)  throw(ConversionException)
{
  put((PyObject *)data);
}

//!Convertit un PyObject de type string en CORBA::Any string
/*!
 *   \param data : python object
 */

void PyCorbaString::put(PyObject *data)  throw(ConversionException)
{
  char * s=PyString_AsString(data);
  CORBA::Any a;
  a <<= s;
  _port->put(&a);
}


void PyCorbaDouble::put(const void *data)  throw(ConversionException)
{
  put((PyObject *)data);
}

//!Convertit un PyObject de type double en CORBA::Any Double
/*!
 *   \param data : python object
 */

void PyCorbaDouble::put(PyObject *data)  throw(ConversionException)
{
  CORBA::Double d = 0;
  if (PyFloat_Check(data))
    d = (CORBA::Double)PyFloat_AS_DOUBLE(data);
  else if (PyInt_Check(data))
    d = (CORBA::Double)PyInt_AS_LONG(data);
  else
    d = (CORBA::Double)PyLong_AsDouble(data);
  CORBA::Any a;
  a <<= d;
  _port->put(&a);
}

PyCorbaSequence::PyCorbaSequence(InputCorbaPort* p)
  : ProxyPort(p), Port(p->getNode())
{
  cerr << "CorbaPySequence" << endl;
}

//!Convertit un PyObject de type Sequence en CORBA::Any Sequence
/*!
 *   \param data : python object
 */

void PyCorbaSequence::put(const void *data)  throw(ConversionException)
{
  put((PyObject *)data);
}

void PyCorbaSequence::put(PyObject *data)  throw(ConversionException)
{
  cerr << "PyCorbaSequence::put" << endl;
  cerr << "data refcnt: " << data->ob_refcnt << endl;
  PyObject_Print(data,stdout,Py_PRINT_RAW);
  cerr << endl;
  int length=PySequence_Size(data);
  cerr <<"length: " << length << endl;
  CORBA::Any *a= convertCorbaPyObject(_port->type(),data);
  _port->put(a);
}


PyCorbaObjref::PyCorbaObjref(InputCorbaPort* p)
  : ProxyPort(p), Port(p->getNode())
{
  _pyorb = getSALOMERuntime()->getPyOrb();
  _orb = getSALOMERuntime()->getOrb();
  //  _dynFactory = getSALOMERuntime()->getDynFactory();
}

//!Convertit un PyObject de type Objref en CORBA::Any Objref
/*!
 *   \param data : python object
 */

void PyCorbaObjref::put(const void *data)  throw(ConversionException)
{
  put((PyObject *)data);
}

void PyCorbaObjref::put(PyObject *data)  throw(ConversionException)
{
  cerr << "PyCorbaObjref::put" << endl;
  cerr << "data refcnt: " << data->ob_refcnt << endl;
  PyObject_Print(data,stdout,Py_PRINT_RAW);
  cerr << endl;

  //ne marche pas ???
  //hold_lock is true: caller is supposed to hold the GIL. 
  //omniorb will not take the GIL
  //CORBA::Object_ptr ob=api->pyObjRefToCxxObjRef(data,(CORBA::Boolean)1);

  PyObject_Print(_pyorb,stdout,Py_PRINT_RAW);
  cerr << endl;
  PyObject *pystring = PyObject_CallMethod(_pyorb, "object_to_string", "O", data);
  if(pystring == NULL)
    {
      PyErr_Print();
      throw Exception("not possible to get objref");
    }

  PyObject_Print(pystring,stdout,Py_PRINT_RAW);
  cerr << endl;
  CORBA::Object_ptr ob= _orb->string_to_object(PyString_AsString(pystring));
  Py_DECREF(pystring);
  cerr << "data refcnt: " << data->ob_refcnt << endl;

  CORBA::Any a;
  a <<= ob;
  _port->put(&a);
}

