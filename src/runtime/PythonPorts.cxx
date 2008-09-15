
#include "PythonPorts.hxx"
#include "TypeConversions.hxx"
#include "TypeCode.hxx"
#include "Node.hxx"
#include "ConversionException.hxx"

#include <iostream>
#include <sstream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

void releasePyObj(PyObject* data)
{
  DEBTRACE( "data refcnt: " << data->ob_refcnt );
  if (PyObject_HasAttrString(data, (char*)"_is_a"))
    {
      PyObject *result = PyObject_CallMethod(data, (char*)"_is_a", (char*)"s",(char*)"IDL:SALOME/GenericObj:1.0");
      if(result && PyInt_Check(result))
        {
          if(PyInt_AS_LONG(result))
            {
              PyObject* o=PyObject_CallMethod(data, (char*)"Destroy", (char*)"");
              if(o)
                Py_XDECREF( o);
              else
                {
#ifdef _DEVDEBUG_
                  PyErr_Print();
#else
                  PyErr_Clear();
#endif
                  throw ConversionException("Corba object does not exist: you have perhaps forgotten to call Register on a GenericObj");      
                }
            }
          Py_XDECREF(result); 
        }
      if(!result)
        {
#ifdef _DEVDEBUG_
          PyErr_Print();
#else
          PyErr_Clear();
#endif
          throw ConversionException("Corba object does not exist: you have perhaps forgotten to call Register on a GenericObj");      
        }
    }
}

void registerPyObj(PyObject* data)
{
  if (PyObject_HasAttrString(data, (char*)"_is_a"))
    {
      PyObject *result = PyObject_CallMethod(data, (char*)"_is_a", (char*)"s",(char*)"IDL:SALOME/GenericObj:1.0");
      if(result && PyInt_Check(result))
        {
          if(PyInt_AS_LONG(result))
            {
              PyObject* o= PyObject_CallMethod(data, (char*)"Register", (char*)"") ;
              if(o)
                Py_XDECREF( o);
              else
                {
#ifdef _DEVDEBUG_
                  PyErr_Print();
#else
                  PyErr_Clear();
#endif
                  throw ConversionException("Corba object does not exist: you have perhaps forgotten to call Register on a GenericObj");      
                }
            }
          Py_XDECREF(result); 
        }
      if(!result)
        {
#ifdef _DEVDEBUG_
          PyErr_Print();
#else
          PyErr_Clear();
#endif
          throw ConversionException("Corba object does not exist: you have perhaps forgotten to call Register on a GenericObj");      
        }
    }
}

InputPyPort::InputPyPort(const std::string& name, Node *node, TypeCode * type)
  : InputPort(name, node, type), DataPort(name, node, type), Port(node), _data(Py_None),_initData(Py_None)
{
  Py_INCREF(_data);
  Py_INCREF(_initData);
}
InputPyPort::~InputPyPort()
{
  PyGILState_STATE gstate = PyGILState_Ensure();
  DEBTRACE( "_data refcnt: " << _data->ob_refcnt );
  DEBTRACE( "_initData refcnt: " << _initData->ob_refcnt );
  // Release or not release : all GenericObj are deleted when the input port is deleted
  releasePyObj(_data);
  Py_XDECREF(_data); 
  Py_XDECREF(_initData); 
  PyGILState_Release(gstate);
}

InputPyPort::InputPyPort(const InputPyPort& other, Node *newHelder):InputPort(other,newHelder),DataPort(other,newHelder),Port(other,newHelder)
{
  _initData=other._initData;
  Py_INCREF(_initData);
  _data=other._data;
  Py_INCREF(_data);
}

bool InputPyPort::edIsManuallyInitialized() const
{
  return _initData!=Py_None;
}

void InputPyPort::edRemoveManInit()
{
  Py_XDECREF(_initData);
  _initData=Py_None;
  Py_INCREF(_initData);
  InputPort::edRemoveManInit();
}

void InputPyPort::put(const void *data) throw(ConversionException)
{
  put((PyObject *)data);
}

void InputPyPort::put(PyObject *data) throw(ConversionException)
{
  InterpreterUnlocker l;
  releasePyObj(_data);
  Py_XDECREF(_data); 
  _data = data;
  _stringRef="";
  Py_INCREF(_data); 
  registerPyObj(_data);
  DEBTRACE( "_data refcnt: " << _data->ob_refcnt );
}

InputPort *InputPyPort::clone(Node *newHelder) const
{
  return new InputPyPort(*this,newHelder);
}

PyObject * InputPyPort::getPyObj() const
{
  return _data;
}

void *InputPyPort::get() const throw(Exception)
{
  return (void*) _data;
}

bool InputPyPort::isEmpty()
{
  return _data == Py_None;
}

//! Save the current data value for further reinitialization of the port
/*!
 *
 */
void InputPyPort::exSaveInit()
{
  Py_XDECREF(_initData); 
  _initData=_data;
  Py_INCREF(_initData); 
  DEBTRACE( "_initData.ob refcnt: " << _initData->ob_refcnt );
  DEBTRACE( "_data.ob refcnt: " << _data->ob_refcnt );
}

//! Restore the saved data value to current data value
/*!
 * If no data has been saved (_initData == 0) don't restore
 */
void InputPyPort::exRestoreInit()
{
  if(!_initData)return;
  Py_XDECREF(_data); 
  _data=_initData;
  Py_INCREF(_data); 
  DEBTRACE( "_initData.ob refcnt: " << _initData->ob_refcnt );
  DEBTRACE( "_data.ob refcnt: " << _data->ob_refcnt );
}

std::string InputPyPort::dump()
{
  if( _data == Py_None)
    return "<value>None</value>";

  InterpreterUnlocker l;
  if (edGetType()->kind() != YACS::ENGINE::Objref)
    return convertPyObjectXml(edGetType(), _data);
  if (! _stringRef.empty())
    return _stringRef;
  else 
    return convertPyObjectXml(edGetType(), _data);
//     {
//       stringstream msg;
//       msg << "Cannot retreive init reference string for port " << _name
//           << " on node " << _node->getName();
//       throw Exception(msg.str());      
//     }
}


OutputPyPort::OutputPyPort(const std::string& name, Node *node, TypeCode * type)
  : OutputPort(name, node, type), DataPort(name, node, type), Port(node)
{
  _data = Py_None;
  Py_INCREF(_data); 
}
OutputPyPort::~OutputPyPort()
{
  PyGILState_STATE gstate = PyGILState_Ensure();
  DEBTRACE( "_data refcnt: " << _data->ob_refcnt );
  // Release or not release : all GenericObj are deleted when the output port is deleted
  releasePyObj(_data);
  Py_XDECREF(_data); 
  PyGILState_Release(gstate);
}

OutputPyPort::OutputPyPort(const OutputPyPort& other, Node *newHelder):OutputPort(other,newHelder),DataPort(other,newHelder),Port(other,newHelder),
                                                                       _data(Py_None)
{
}

void OutputPyPort::put(const void *data) throw(ConversionException)
{
  put((PyObject *)data);
}

void OutputPyPort::put(PyObject *data) throw(ConversionException)
{
  InputPort *p;
  DEBTRACE( "OutputPyPort::put.ob refcnt: " << data->ob_refcnt );
#ifdef _DEVDEBUG_
  PyObject_Print(data,stderr,Py_PRINT_RAW);
  cerr << endl;
#endif
  releasePyObj(_data);
  Py_XDECREF(_data); 
  _data = data;
  Py_INCREF(_data); 
  //no registerPyObj : we steal the output reference of the node
  DEBTRACE( "OutputPyPort::put.ob refcnt: " << data->ob_refcnt );
  OutputPort::put(data);
}

OutputPort *OutputPyPort::clone(Node *newHelder) const
{
  return new OutputPyPort(*this,newHelder);
}

PyObject * OutputPyPort::get() const
{
  return _data;
}

PyObject * OutputPyPort::getPyObj() const
{
  return _data;
}

std::string OutputPyPort::dump()
{
  if( _data == Py_None)
    return "<value>None</value>";
  InterpreterUnlocker l;
  string xmldump = convertPyObjectXml(edGetType(), _data);
  return xmldump;
}

