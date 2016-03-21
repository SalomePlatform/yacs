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

#include "TypeConversions.hxx"
#include "CppPorts.hxx"
#include "Node.hxx"
#include "TypeCode.hxx"

#include <iostream>
#include <sstream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

InputCppPort::InputCppPort(const std::string& name, Node *node, TypeCode * type)
  : InputPort(name, node, type), DataPort(name, node, type), Port(node), _data(NULL),_initData(NULL)
{
}

InputCppPort::~InputCppPort()
{
  if(_data)
    {
      DEBTRACE("_data ref count: " << _data->getRefCnt());
      _data->decrRef();
    }
}

InputCppPort::InputCppPort(const InputCppPort& other, Node *newHelder):InputPort(other,newHelder),DataPort(other,newHelder),Port(other,newHelder)
{
  _initData=other._initData;
  _data=other._data;
}

bool InputCppPort::edIsManuallyInitialized() const
{
  return _initData!= NULL;
}

void InputCppPort::edRemoveManInit()
{
  _initData=NULL;
  InputPort::edRemoveManInit();
}

void InputCppPort::put(const void *data) throw(ConversionException)
{
  put((YACS::ENGINE::Any *)data);
}

void InputCppPort::put(YACS::ENGINE::Any *data) throw(ConversionException)
{
  if(_data)
    _data->decrRef();
  _data=data;
  _data->incrRef();
  DEBTRACE("value ref count: " << _data->getRefCnt());
}

InputPort *InputCppPort::clone(Node *newHelder) const
{
  return new InputCppPort(*this,newHelder);
}

YACS::ENGINE::Any * InputCppPort::getCppObj() const
{
  return _data;
}

void *InputCppPort::get() const throw(YACS::Exception)
{
  return (void*) _data;
}

bool InputCppPort::isEmpty()
{
  return _data == NULL;
}

//! Save the current data value for further reinitialization of the port
/*!
 *
 */
void InputCppPort::exSaveInit()
{
  _initData=_data;
  //DEBTRACE("_initData.ob refcnt: " << _initData->ob_refcnt);
  //DEBTRACE("_data.ob refcnt: " << _data->ob_refcnt);
}

//! Restore the saved data value to current data value
/*!
 * If no data has been saved (_initData == 0) don't restore
 */
void InputCppPort::exRestoreInit()
{
  if(!_initData)return;
  _data=_initData;
  //DEBTRACE("_initData.ob refcnt: " << _initData->ob_refcnt);
  //DEBTRACE("_data.ob refcnt: " << _data->ob_refcnt);
}

std::string InputCppPort::dump()
{
  if( _data == NULL)
    return "<value>None</value>";

  if (edGetType()->kind() != YACS::ENGINE::Objref)
    return convertNeutralXml(edGetType(), _data);
    //return convertCppXml(edGetType(), _data);
  if (! _stringRef.empty())
    return _stringRef;
  else 
    return convertNeutralXml(edGetType(), _data);
//     {
//       stringstream msg;
//       msg << "Cannot retreive init reference string for port " << _name
//           << " on node " << _node->getName();
//       throw Exception(msg.str());      
//     }
}


OutputCppPort::OutputCppPort(const std::string& name, Node *node, TypeCode * type)
  : OutputPort(name, node, type), DataPort(name, node, type), Port(node)
{
  _data = NULL;
}

OutputCppPort::~OutputCppPort()
{
  if(_data)
    {
      DEBTRACE("_data ref count: " << _data->getRefCnt());
      _data->decrRef();
    }
}

OutputCppPort::OutputCppPort(const OutputCppPort& other, Node *newHelder):OutputPort(other,newHelder),DataPort(other,newHelder),Port(other,newHelder),
                                                                       _data(NULL)
{
}

void OutputCppPort::put(const void *data) throw(ConversionException)
{
  put((YACS::ENGINE::Any *)data);
}

void OutputCppPort::put(YACS::ENGINE::Any *data) throw(ConversionException)
{
  InputPort *p;
  if(_data)
    _data->decrRef();
  _data = data;
  if(_data)
    _data->incrRef();
  OutputPort::put(data);
}

OutputPort *OutputCppPort::clone(Node *newHelder) const
{
  return new OutputCppPort(*this,newHelder);
}

YACS::ENGINE::Any * OutputCppPort::get() const
{
  return _data;
}

std::string OutputCppPort::dump()
{
  if( _data == NULL)
    return "<value>None</value>";
  string xmldump = convertNeutralXml(edGetType(), _data);
  return xmldump;
}

