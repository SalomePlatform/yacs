
//#define REFCNT
#ifdef REFCNT
#define private public
#define protected public
#include <omniORB4/CORBA.h>
#include <omniORB4/internal/typecode.h>
#endif

#include "PythonCORBAConv.hxx"
#include "TypeConversions.hxx"
#include "RuntimeSALOME.hxx"

#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

/*!Convert a PyObject (integer) to CORBA::Any (integer)
 * It's only a wrapper around put(PyObject *data)
 */
void PyCorbaInt::put(const void *data)  throw(ConversionException)
{
  put((PyObject *)data);
}

//!Convert a PyObject (integer) to CORBA::Any (integer)
/*!
 *   \param data : python object
 */
void PyCorbaInt::put(PyObject *data)  throw(ConversionException)
{
  CORBA::Long l= 0;
  if (PyInt_Check(data))l=PyInt_AS_LONG(data);
  else if(PyLong_Check(data))l=PyLong_AsLong(data);
  else throw ConversionException("Not an int");

  CORBA::Any a;
  a <<= l;
  _port->put(&a);
}

//!Convert a PyObject (boolean) to CORBA::Any (boolean)
/*!
 * It's only a wrapper around PyCorbaBool::put(PyObject *data)
 *
 *   \param data : python object
 */
void PyCorbaBool::put(const void *data)  throw(ConversionException)
{
  put((PyObject *)data);
}

//!Convert a PyObject (boolean) to CORBA::Any (boolean)
/*!
 * Convert it and push it to proxy port
 *
 *   \param data : python object
 */
void PyCorbaBool::put(PyObject *data)  throw(ConversionException)
{
  CORBA::Any *a= convertPyObjectCorba(_port->edGetType(),data);
  _port->put(a);
  //delete Any that has been allocated by convertPyObjectCorba
  delete a;
}

void PyCorbaString::put(const void *data)  throw(ConversionException)
{
  put((PyObject *)data);
}

//!Convert a PyObject (string) to CORBA::Any (string)
/*!
 *   \param data : python object
 */

void PyCorbaString::put(PyObject *data)  throw(ConversionException)
{
  char * s;
  if(PyString_Check(data))s=PyString_AsString(data);
  else throw ConversionException("Not a string");

  CORBA::Any a;
  a <<= s;
  _port->put(&a);
}


void PyCorbaDouble::put(const void *data)  throw(ConversionException)
{
  put((PyObject *)data);
}

//!Convert a PyObject (double) to CORBA::Any (double)
/*!
 *   \param data : python object
 */

void PyCorbaDouble::put(PyObject *data)  throw(ConversionException)
{
  CORBA::Double d = 0;
  if (PyFloat_Check(data)) d = (CORBA::Double)PyFloat_AS_DOUBLE(data);
  else if (PyInt_Check(data)) d = (CORBA::Double)PyInt_AS_LONG(data);
  else if (PyLong_Check(data)) d = (CORBA::Double)PyLong_AsDouble(data);
  else throw ConversionException("Not a double");

  CORBA::Any a;
  a <<= d;
  _port->put(&a);
}

//!Class PyCorbaSequence is a proxy port that converts a PyObject object (of type sequence) to a CORBA::Any object (of type sequence)
/*!
 *   \param p : the input CORBA port to adapt to Python output port
 */
PyCorbaSequence::PyCorbaSequence(InputCorbaPort* p)
  : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode())
{
}

//!Convert a PyObject (sequence) to CORBA::Any (sequence)
/*!
 *   \param data : python object
 */

void PyCorbaSequence::put(const void *data)  throw(ConversionException)
{
  put((PyObject *)data);
}

void PyCorbaSequence::put(PyObject *data)  throw(ConversionException)
{
  DEBTRACE("data refcnt: " << data->ob_refcnt);
#ifdef _DEVDEBUG_
  PyObject_Print(data,stderr,Py_PRINT_RAW);
  std::cerr << std::endl;
#endif
  CORBA::Any *a= convertPyObjectCorba(_port->edGetType(),data);
  _port->put(a);
#ifdef REFCNT
  DEBTRACE("refcount CORBA seqTC: " << ((omni::TypeCode_base*)a->pd_tc.in())->pd_ref_count);
#endif
  //delete Any that has been allocated by convertPyObjectCorba
  delete a;
#ifdef REFCNT
  DEBTRACE("refcount CORBA seqTC: " << ((omni::TypeCode_base*)((InputCorbaPort*)_port)->getAny()->pd_tc.in())->pd_ref_count);
#endif
}

//!Class PyCorbaObjref is a proxy port that converts a PyObject object (of type objref) to a CORBA::Any object (of type objref)
/*!
 *   \param p : the input CORBA port to adapt to Python output port
 */
PyCorbaObjref::PyCorbaObjref(InputCorbaPort* p)
  : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode())
{
  _pyorb = getSALOMERuntime()->getPyOrb();
  _orb = getSALOMERuntime()->getOrb();
  //  _dynFactory = getSALOMERuntime()->getDynFactory();
}

//!Convert a PyObject (Objref) to CORBA::Any (Objref)
/*!
 *   \param data : python object
 */

void PyCorbaObjref::put(const void *data)  throw(ConversionException)
{
  put((PyObject *)data);
}

void PyCorbaObjref::put(PyObject *data)  throw(ConversionException)
{
  DEBTRACE("data refcnt: " << data->ob_refcnt);
#ifdef _DEVDEBUG_
  PyObject_Print(data,stderr,Py_PRINT_RAW);
  std::cerr << std::endl;
#endif

  //does not work : replace by a call to object_to_string - string_to_object
  //hold_lock is true: caller is supposed to hold the GIL. 
  //omniorb will not take the GIL
  //CORBA::Object_ptr ob=api->pyObjRefToCxxObjRef(data,(CORBA::Boolean)1);

  PyObject *pystring = PyObject_CallMethod(_pyorb, "object_to_string", "O", data);
  if(pystring == NULL)
    {
      PyErr_Print();
      throw ConversionException("can't get objref");
    }
  CORBA::Object_var ob= _orb->string_to_object(PyString_AsString(pystring));
  Py_DECREF(pystring);

  CORBA::Any a;
  a <<= ob;
  _port->put(&a);
}

//!Class PyCorbaStruct is a proxy port that converts a PyObject object (of type struct) to a CORBA::Any object (of type struct)
/*!
 *   \param p : the input CORBA port to adapt to Python output port
 */
PyCorbaStruct::PyCorbaStruct(InputCorbaPort* p)
  : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode())
{
}

void PyCorbaStruct::put(const void *data)  throw(ConversionException)
{
  put((PyObject *)data);
}

//!Convert a PyObject (struct) to CORBA::Any (struct)
/*!
 *   \param data : python object
 */
void PyCorbaStruct::put(PyObject *data)  throw(ConversionException)
{
  DEBTRACE("data refcnt: " << data->ob_refcnt);
#ifdef _DEVDEBUG_
  PyObject_Print(data,stderr,Py_PRINT_RAW);
  std::cerr << std::endl;
#endif
  CORBA::Any *a= convertPyObjectCorba(_port->edGetType(),data);
  _port->put(a);
#ifdef REFCNT
  DEBTRACE("refcount CORBA structTC: " << ((omni::TypeCode_base*)a->pd_tc.in())->pd_ref_count);
#endif
  //delete Any that has been allocated by convertPyObjectCorba
  delete a;
#ifdef REFCNT
  DEBTRACE("refcount CORBA structTC: " << ((omni::TypeCode_base*)((InputCorbaPort*)_port)->getAny()->pd_tc.in())->pd_ref_count);
#endif
}

