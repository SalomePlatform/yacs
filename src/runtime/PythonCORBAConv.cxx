
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
  CORBA::Any *a= convertPyObjectCorba(_port->edGetType(),data);
  _port->put(a);
  delete a;
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
  CORBA::Any *a= convertPyObjectCorba(_port->edGetType(),data);
  _port->put(a);
  delete a;
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
  CORBA::Any *a= convertPyObjectCorba(_port->edGetType(),data);
  _port->put(a);
  delete a;
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
  CORBA::Any *a= convertPyObjectCorba(_port->edGetType(),data);
  _port->put(a);
  delete a;
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

