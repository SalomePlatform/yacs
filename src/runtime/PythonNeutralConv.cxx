
#include "TypeConversions.hxx"
#include "PythonNeutralConv.hxx"

#include <iostream>
#include <sstream>
#include "Any.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

PyNeutral::PyNeutral(InputPort* p)
  : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode())
{
}

void PyNeutral::put(const void *data) throw(ConversionException)
{
  DEBTRACE( " PyNeutral::put(const void *data)" );
  put((PyObject *)data);
}

//!Convert received Python (PyObject *) value to Neutral (Any *) value and send it to proxy port
 /*!
  *   \param data : Python value
  */

void PyNeutral::put(PyObject *data) throw(ConversionException)
{
  DEBTRACE( "PyNeutral::put " );
  YACS::ENGINE::Any *ob;
  //Do not need to take the Python GIL as put is called from Python node
  ob=convertPyObjectNeutral(edGetType(),data);
  DEBTRACE( "before put refcnt: " << ob->getRefCnt() );
  _port->put(ob);
  //_port has called incRef
  ob->decrRef();
  DEBTRACE( "after put refcnt: " << ob->getRefCnt() );
}
