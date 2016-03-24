// Copyright (C) 2006-2016  CEA/DEN, EDF R&D
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

//#define REFCNT
//
#ifdef REFCNT
#define private public
#define protected public
#include <omniORB4/CORBA.h>
#include <omniORB4/internal/typecode.h>
#endif

#include "RuntimeSALOME.hxx"
#include "TypeConversions.hxx"
#include "TypeCode.hxx"
#include "AutoLocker.hxx"
#include "CORBAPorts.hxx"
#include "PythonPorts.hxx"
#include "ServiceNode.hxx"
#include "ComponentInstance.hxx"
#include "SALOME_GenericObj.hh"

#include <iostream>
#include <sstream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

void releaseObj(CORBA::Any& data)
{
  CORBA::Object_var obj;
  if(data >>= CORBA::Any::to_object(obj))
    {
      SALOME::GenericObj_var gobj;
      try
        {
          gobj=SALOME::GenericObj::_narrow(obj);
        }
      catch(const CORBA::SystemException& )
        {
          return;
        }
      if(!CORBA::is_nil(gobj))
        {
          DEBTRACE("It's a SALOME::GenericObj");
          gobj->UnRegister();
        }
      else
          DEBTRACE("It's a CORBA::Object but not a SALOME::GenericObj");
    }
  else
    DEBTRACE("It's not a CORBA::Object");
}

void registerObj(CORBA::Any& data)
{
  CORBA::Object_var obj;
  if(data >>= CORBA::Any::to_object(obj))
    {
      SALOME::GenericObj_var gobj;
      try
        {
          gobj=SALOME::GenericObj::_narrow(obj);
        }
      catch(const CORBA::SystemException& )
        {
          return;
        }
      if(!CORBA::is_nil(gobj))
        {
          DEBTRACE("It's a SALOME::GenericObj");
          gobj->Register();
        }
      else
          DEBTRACE("It's a CORBA::Object but not a SALOME::GenericObj");
    }
  else
    DEBTRACE("It's not a CORBA::Object");
}

InputCorbaPort::InputCorbaPort(const std::string& name,
                               Node *node,
                               TypeCode * type)
  : InputPort(name, node, type), DataPort(name, node, type), Port(node), _initData(0)
{
  _orb = getSALOMERuntime()->getOrb();
}

InputCorbaPort::InputCorbaPort(const InputCorbaPort& other, Node *newHelder):InputPort(other,newHelder),DataPort(other,newHelder),Port(other,newHelder),
                                                                             _initData(0)
{
  _orb = getSALOMERuntime()->getOrb();
  if(other._initData)
    {
      _initData=new CORBA::Any;
      *_initData=*(other._initData);
    }
  _data=other._data;
}

InputCorbaPort::~InputCorbaPort()
{
  delete _initData;
  // Release or not release : all GenericObj are deleted when the input port is deleted
  releaseObj(_data);
}

bool InputCorbaPort::edIsManuallyInitialized() const
{
  return _initData!=0;
}

void InputCorbaPort::edRemoveManInit()
{
  delete _initData;
  _initData=0;
  InputPort::edRemoveManInit();
}

void InputCorbaPort::put(const void *data) throw (ConversionException)
{
  put((CORBA::Any *)data);
}

void display(CORBA::Any* data)
{
  CORBA::TypeCode_var tc=data->type();
  switch(tc->kind())
    {
    case CORBA::tk_double:
      CORBA::Double d;
      *data >>= d;
      DEBTRACE( "Double: " << d );
      break;
    case CORBA::tk_long:
      CORBA::Long l;
      *data >>= l;
      DEBTRACE( "Int: " << l );
      break;
    default:
      break;
    }
}

void InputCorbaPort::put(CORBA::Any *data) throw (ConversionException)
{
#ifdef REFCNT
  DEBTRACE("refcount CORBA : " << ((omni::TypeCode_base*)data->pd_tc.in())->pd_ref_count);
#endif
  YACS::BASES::AutoLocker<YACS::BASES::Mutex> lock(&_mutex);
#ifdef _DEVDEBUG_
  display(data);
#endif

  releaseObj(_data);

  // make a copy of the any (protect against deletion of any source)
  _data=*data;
  _stringRef="";

  registerObj(_data);

#ifdef REFCNT
  DEBTRACE("refcount CORBA : " << ((omni::TypeCode_base*)_data.pd_tc.in())->pd_ref_count);
#endif
}

InputPort *InputCorbaPort::clone(Node *newHelder) const
{
  return new InputCorbaPort(*this,newHelder);
}

void *InputCorbaPort::get() const throw(YACS::Exception)
{
  return (void *)&_data;
}

bool InputCorbaPort::isEmpty()
{
  CORBA::TypeCode_var tc=_data.type();
  return tc->equivalent(CORBA::_tc_null);
}

CORBA::Any * InputCorbaPort::getAny()
{
  // --- return a pointer to internal any
  return &_data;
}

PyObject * InputCorbaPort::getPyObj()
{
  YACS::BASES::AutoLocker<YACS::BASES::Mutex> lock(&_mutex);
  CORBA::TypeCode_var tc=getAny()->type();
  if (!tc->equivalent(CORBA::_tc_null))
    return convertCorbaPyObject(edGetType(),getAny());
  else
    {
      Py_INCREF(Py_None);
      return Py_None;
    }
}

std::string InputCorbaPort::getAsString()
{
  InterpreterUnlocker loc;
  PyObject* ob=getPyObj();
  std::string s=convertPyObjectToString(ob);
  Py_DECREF(ob);
  return s;
}


//! Save the current data value for further reinitialization of the port
/*!
 *
 */
void InputCorbaPort::exSaveInit()
{
  if(_initData)
    delete _initData;
  _initData=new CORBA::Any;
  *_initData=_data;
}

//! Restore the saved data value to current data value
/*!
 * If no data has been saved (_initData == 0) don't restore
 */
void InputCorbaPort::exRestoreInit()
{
  if(!_initData)return;
  put(_initData);
}

std::string InputCorbaPort::dump()
{
  CORBA::TypeCode_var tc=_data.type();
  if (tc->equivalent(CORBA::_tc_null))
    return "<value>nil</value>";
  if (edGetType()->kind() != YACS::ENGINE::Objref)
    return convertCorbaXml(edGetType(), &_data);
  if (! _stringRef.empty())
    return _stringRef;
  else 
    return convertCorbaXml(edGetType(), &_data);
//     {
//       stringstream msg;
//       msg << "Cannot retreive init string reference string for port " << _name
//           << " on node " << _node->getName();
//       throw Exception(msg.str());      
//     }
}

std::string InputCorbaPort::valToStr()
{
  int isString = PyString_Check(getPyObj());
  PyObject *strPyObj = PyObject_Str(getPyObj());
  string val = PyString_AsString(strPyObj);
  if (isString)
    val = "\"" + val + "\"";
  Py_DECREF(strPyObj);
  return val;
}

void InputCorbaPort::valFromStr(std::string valstr)
{
}

OutputCorbaPort::OutputCorbaPort(const std::string& name,
                                 Node *node,
                                 TypeCode * type)
  : OutputPort(name, node, type), DataPort(name, node, type), Port(node)
{
  _orb = getSALOMERuntime()->getOrb();
}

OutputCorbaPort::OutputCorbaPort(const OutputCorbaPort& other, Node *newHelder):OutputPort(other,newHelder),DataPort(other,newHelder),Port(other,newHelder)
{
  _orb = getSALOMERuntime()->getOrb();
}

OutputCorbaPort::~OutputCorbaPort()
{
  DEBTRACE(getName());
  // Release or not release : all GenericObj are deleted when the output port is deleted
  releaseObj(_data);
#ifdef REFCNT
  DEBTRACE("refcount CORBA : " << ((omni::TypeCode_base*)_data.pd_tc.in())->pd_ref_count);
  DEBTRACE("refcount CORBA tc_double: " << ((omni::TypeCode_base*)CORBA::_tc_double)->pd_ref_count);
#endif
}

void OutputCorbaPort::put(const void *data) throw (ConversionException)
{
  put((CORBA::Any *)data);
}

void OutputCorbaPort::put(CORBA::Any *data) throw (ConversionException)
{
  InputPort *p;

  {  
    YACS::BASES::AutoLocker<YACS::BASES::Mutex> lock(&_mutex);
#ifdef REFCNT
    DEBTRACE("refcount CORBA : " << ((omni::TypeCode_base*)data->pd_tc.in())->pd_ref_count);
#endif
#ifdef REFCNT
    DEBTRACE("refcount CORBA : " << ((omni::TypeCode_base*)_data.pd_tc.in())->pd_ref_count);
#endif

    releaseObj(_data);

    _data=*data;

    //no registerObj : we steal the output reference of the node
  }

#ifdef REFCNT
  DEBTRACE("refcount CORBA : " << ((omni::TypeCode_base*)_data.pd_tc.in())->pd_ref_count);
#endif
  OutputPort::put(data);
#ifdef REFCNT
  DEBTRACE("refcount CORBA : " << ((omni::TypeCode_base*)data->pd_tc.in())->pd_ref_count);
#endif
#ifdef REFCNT
  DEBTRACE("refcount CORBA : " << ((omni::TypeCode_base*)_data.pd_tc.in())->pd_ref_count);
#endif
}

OutputPort *OutputCorbaPort::clone(Node *newHelder) const
{
  return new OutputCorbaPort(*this,newHelder);
}

CORBA::Any * OutputCorbaPort::getAny()
{
  // return a pointer to the internal any
  return &_data;
}

CORBA::Any * OutputCorbaPort::getAnyOut() 
{
  CORBA::Any* a=new CORBA::Any;
  DynType kind=edGetType()->kind();
  CORBA::TypeCode_var t;

  if(kind == Int)
    {
      a->replace(CORBA::_tc_long, (void*) 0);
    }
  else if(kind == String)
    {
      a->replace(CORBA::_tc_string, (void*) 0);
    }
  else if(kind == Double)
    {
      a->replace(CORBA::_tc_double, (void*) 0);
    }
  else if(kind == Objref)
    {
      t = getCorbaTC(edGetType());
      a->replace(t, (void*) 0);
    }
  else if(kind == Sequence)
    {
      t = getCorbaTC(edGetType());
      a->replace(t, (void*) 0);
    }
  else if(kind == Struct)
    {
      t = getCorbaTC(edGetType());
#ifdef REFCNT
  DEBTRACE("refcount CORBA : " << ((omni::TypeCode_base*)t.in())->pd_ref_count);
#endif
      a->replace(t, (void*) 0);
#ifdef REFCNT
  DEBTRACE("refcount CORBA : " << ((omni::TypeCode_base*)t.in())->pd_ref_count);
#endif
    }
  else if(kind == Bool)
    {
      a->replace(CORBA::_tc_boolean, (void*) 0);
    }
  else if(kind == NONE)
    {
      stringstream msg;
      msg << "Cannot set Any Out for None" << __FILE__ << ":" << __LINE__;
      throw Exception(msg.str());
    }
  else
    {
      stringstream msg;
      msg << "Cannot set Any Out for unknown type" << __FILE__
          << ":" << __LINE__;
      throw Exception(msg.str());
    }
    
  DEBTRACE( "getAnyOut:a: " << a );
#ifdef REFCNT
  DEBTRACE("refcount CORBA : " << ((omni::TypeCode_base*)a->pd_tc.in())->pd_ref_count);
#endif
  return a;
}

PyObject * OutputCorbaPort::getPyObj()
{
  YACS::BASES::AutoLocker<YACS::BASES::Mutex> lock(&_mutex);
  CORBA::TypeCode_var tc=getAny()->type();
  if (!tc->equivalent(CORBA::_tc_null))
    return convertCorbaPyObject(edGetType(),getAny());
  else
    {
      Py_INCREF(Py_None);
      return Py_None;
    }
}

std::string OutputCorbaPort::getAsString()
{
  InterpreterUnlocker loc;
  PyObject* ob=getPyObj();
  std::string s=convertPyObjectToString(ob);
  Py_DECREF(ob);
  return s;
}

std::string OutputCorbaPort::dump()
{
  CORBA::TypeCode_var tc=_data.type();
  if (tc->equivalent(CORBA::_tc_null))
    return "<value>nil</value>";
  string xmldump = convertCorbaXml(edGetType(), &_data);
  return xmldump;
}
namespace YACS {
  namespace ENGINE {
    ostream& operator<<(ostream& os, const OutputCorbaPort& p)
    {
      CORBA::Double l;
      p._data>>=l;
      os << p._name << " : " << l ;
      return os;
    }
  };
};

std::string OutputCorbaPort::valToStr()
{
  PyObject *strPyObj = PyObject_Str(getPyObj());
  string val = PyString_AsString(strPyObj);
  Py_DECREF(strPyObj);
  return val;
}

void OutputCorbaPort::valFromStr(std::string valstr)
{
}

