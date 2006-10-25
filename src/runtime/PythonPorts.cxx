
#include "PythonPorts.hxx"

#include <iostream>

using namespace YACS::ENGINE;
using namespace std;

InputPyPort::InputPyPort(const string& name, Node *node, TypeCode * type)
  : InputPort(name, node, type), Port(node)
{
  _impl = "Python";
}

void InputPyPort::put(const void *data) throw(ConversionException)
{
  put((PyObject *)data);
  _empty = false;
}

void InputPyPort::put(PyObject *data) throw(ConversionException)
{
  cerr << "InputPyPort::put" << endl;
  _data = data;
}

PyObject * InputPyPort::getPyObj() const
{
  return _data;
}



OutputPyPort::OutputPyPort(const string& name, Node *node, TypeCode * type)
  : OutputPort(name, node, type), Port(node)
{
  _impl = "Python";
}

void OutputPyPort::put(const void *data) throw(ConversionException)
{
  put((PyObject *)data);
}

void OutputPyPort::put(PyObject *data) throw(ConversionException)
{
  cerr << "OutputPyPort::put" << endl;
  InputPort *p;
  cerr << "ob refcnt: " << data->ob_refcnt << endl;
  PyObject_Print(data,stdout,Py_PRINT_RAW);
  cerr << endl;
  _data = data;
  set<InputPort *>::iterator iter;
  for(iter = _setOfInputPort.begin(); iter != _setOfInputPort.end(); iter++)
    {
      p = *iter;
      p->put(data);
    }
}

PyObject * OutputPyPort:: get() const
{
  return _data;
}

