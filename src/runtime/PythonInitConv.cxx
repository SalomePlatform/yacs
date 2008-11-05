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
