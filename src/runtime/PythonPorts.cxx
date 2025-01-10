// Copyright (C) 2006-2025  CEA, EDF
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

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

static void RegisterReleasePyObj(PyObject* data, const char *method1)
{
  if(!data)
    return ;
  DEBTRACE( "data refcnt: " << data->ob_refcnt );
  if (PyObject_HasAttrString(data, (char*)"_is_a"))
    {
      PyObject *result = PyObject_CallMethod(data, (char*)"_is_a", (char*)"s",(char*)"IDL:SALOME/GenericObj:1.0");
      if(result && PyLong_Check(result))
        {
          if(PyLong_AS_LONG(result))
            {
              PyObject* o=PyObject_CallMethod(data, (char*)method1, (char*)"");
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

void releasePyObj(PyObject* data)
{
  RegisterReleasePyObj(data,"Destroy");
}

void registerPyObj(PyObject* data)
{
  RegisterReleasePyObj(data,"Register");
}

InputPyPort::InputPyPort(const std::string& name, Node *node, TypeCode * type)
  : InputPort(name, node, type)
  , DataPort(name, node, type)
  , Port(node)
  , _data(Py_None)
  ,_initData(nullptr)
  ,_isEmpty(true)
{
  Py_INCREF(_data);
}
InputPyPort::~InputPyPort()
{
  PyGILState_STATE gstate = PyGILState_Ensure();
  DEBTRACE( "_data refcnt: " << (_data ? _data->ob_refcnt : -1) );
  DEBTRACE( "_initData refcnt: " << (_initData ? _initData->ob_refcnt : -1));
  // Release or not release : all GenericObj are deleted when the input port is deleted
  releasePyObj(_data);
  Py_XDECREF(_data); 
  Py_XDECREF(_initData); 
  PyGILState_Release(gstate);
}

InputPyPort::InputPyPort(const InputPyPort& other, Node *newHelder):InputPort(other,newHelder),DataPort(other,newHelder),Port(other,newHelder)
{
  _initData=other._initData;
  Py_XINCREF(_initData);
  _data=other._data;
  Py_INCREF(_data);
  _isEmpty=other._isEmpty;
}

bool InputPyPort::edIsManuallyInitialized() const
{
  return _initData!=nullptr;
}

void InputPyPort::edRemoveManInit()
{
  Py_XDECREF(_initData);
  _initData=nullptr;
  Py_XDECREF(_data);
  _data=Py_None;
  Py_INCREF(_data);
  _isEmpty=true;
  InputPort::edRemoveManInit();
}

void InputPyPort::put(const void *data)
{
  put((PyObject *)data);
}

void InputPyPort::releaseDataUnsafe()
{
  releasePyObj(_data);
  Py_XDECREF(_data); 
  _data = nullptr;
}

void InputPyPort::releaseData()
{
  InterpreterUnlocker l;
  InputPyPort::releaseDataUnsafe();
}

void InputPyPort::put(PyObject *data)
{
  InterpreterUnlocker l;
  InputPyPort::releaseDataUnsafe();
  _data = data;
  _stringRef="";
  Py_INCREF(_data); 
  registerPyObj(_data);
  _isEmpty=false;
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

void *InputPyPort::get() const
{
  return (void*) _data;
}

std::string InputPyPort::getAsString()
{
  std::string ret;
  //protect _data against modification or delete in another thread
  PyObject* data=_data;
  Py_INCREF(data);
  ret = convertPyObjectToString(data);
  Py_XDECREF(data);
  return ret;
}

std::string InputPyPort::getHumanRepr()
{
  if(!_data)
    return dump();
  PyObject *ret(PyObject_Str(_data));
  if(!ret)
    return dump();
  std::string retCpp;
  char *val(PyBytes_AsString(ret));
  if(val)
    retCpp=val;
  Py_XDECREF(ret);
  return retCpp;
}

bool InputPyPort::isEmpty()
{
  return _isEmpty;
}

//! Save the current data value for further reinitialization of the port
/*!
 *
 */
void InputPyPort::exSaveInit()
{
  // Interpreter lock seems necessary when deleting lists in Python 2.7
  PyGILState_STATE gstate = PyGILState_Ensure();
  Py_XDECREF(_initData);
  _initData=_data;
  Py_INCREF(_initData); 
  PyGILState_Release(gstate);
  DEBTRACE( "_initData.ob refcnt: " << (_initData ? _initData->ob_refcnt : -1));
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
  Py_XINCREF(_data);
  _isEmpty = false;
  DEBTRACE( "_initData.ob refcnt: " << _initData->ob_refcnt );
  DEBTRACE( "_data.ob refcnt: " << _data->ob_refcnt );
}

std::string InputPyPort::dump()
{
  if( _isEmpty)
    return "<value>None</value>";

  InterpreterUnlocker l;
  return convertPyObjectXml(edGetType(), _data);
}

std::string InputPyPort::valToStr()
{
  int isString = PyBytes_Check(getPyObj());
  //DEBTRACE("isString=" << isString);
  PyObject *strPyObj = PyObject_Str(getPyObj());
  //DEBTRACE(PyString_Size(strPyObj));
  string val = PyBytes_AsString(strPyObj);
  if (isString)
    val = "\"" + val + "\"";
  //DEBTRACE(val);
  Py_DECREF(strPyObj);
  return val;
}

void InputPyPort::valFromStr(std::string valstr)
{
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
  Py_INCREF(_data);
}

void OutputPyPort::put(const void *data)
{
  put((PyObject *)data);
}

void OutputPyPort::putWithoutForward(PyObject *data)
{
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
}

void OutputPyPort::put(PyObject *data)
{
  putWithoutForward(data);
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

std::string OutputPyPort::getAsString()
{
  std::string ret;
  //protect _data against modification or delete in another thread
  PyObject* data=_data;
  Py_INCREF(data); 
  ret = convertPyObjectToString(data);
  Py_XDECREF(data); 
  return ret;
}

std::string OutputPyPort::dump()
{
  InterpreterUnlocker l;
  string xmldump = convertPyObjectXml(edGetType(), _data);
  return xmldump;
}

std::string OutputPyPort::valToStr()
{
  PyObject *strPyObj = PyObject_Str(getPyObj());
  string val = PyBytes_AsString(strPyObj);
  Py_DECREF(strPyObj);
  return val;
}

void OutputPyPort::valFromStr(std::string valstr)
{
}
