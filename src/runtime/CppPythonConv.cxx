
#include "CppPythonConv.hxx"
#include "TypeConversions.hxx"

#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

namespace YACS 
{
  namespace ENGINE
  {

    PyObject *convertCppPyObject(const TypeCode *t, Any *a)
    {
      return convertNeutralPyObject(t, (Any *) a);
    }

    CppPy::CppPy(InputPyPort* p)
      : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode())
    {
    }

    //!Convert the received C++ value to PyObject * and send it to proxy port
    /*!
     *   \param data : C++ value received 
     */
    void CppPy::put(const void *data) throw(ConversionException)
    {
      put((Any *) data);
    }

    void CppPy::put(Any *data) throw(ConversionException)
    {
      DEBTRACE(" CppPython::put(Any *data)");
      PyObject *ob;
      {
	InterpreterUnlocker l;
	ob=convertCppPyObject(edGetType(), data);
	PyObject_Print(ob,stderr,Py_PRINT_RAW);
        DEBTRACE( "ob refcnt: " << ob->ob_refcnt );
      }
      _port->put(ob);
      Py_DECREF(ob);
      DEBTRACE( "ob refcnt: " << ob->ob_refcnt );
    }

    int isAdaptablePyObjectCpp(const TypeCode *t1, const TypeCode *t2)
    {
      return isAdaptablePyObjectNeutral(t1, t2);
    }

  }
}
