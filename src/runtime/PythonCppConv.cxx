
#include "PythonCppConv.hxx"
#include "TypeConversions.hxx"
#include "RuntimeSALOME.hxx"

#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

namespace YACS
{
  namespace ENGINE
  {
    Any * convertPyObjectCpp(const TypeCode *t, PyObject *data)
    {
      return convertPyObjectNeutral(t, data);
    }

    void PyCpp::put(const void *data)  throw(ConversionException)
    {
      put((PyObject *)data);
    }

    //!Convertit un PyObject de type entier en YACS::ENGINE::Any entier
    /*!
     *   \param data : python object
     */

    void PyCpp::put(PyObject *data)  throw(ConversionException)
    {
      YACS::ENGINE::Any *a;
      //Do not need to take the Python GIL as put is called from Python node
      a = convertPyObjectCpp(edGetType(), data);
      DEBTRACE( "before put refcnt: " << a->getRefCnt() );
      _port->put(a);
      //_port has called incRef
      a->decrRef();
      DEBTRACE( "after put refcnt: " << a->getRefCnt() );
    }

    int isAdaptableCppPyObject(const TypeCode *t1, const TypeCode *t2)
    {
      return isAdaptableNeutralPyObject(t1, t2);
    }
  }
}
