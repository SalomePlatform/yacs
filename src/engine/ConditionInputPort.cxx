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

#include "ConditionInputPort.hxx"
#include "WhileLoop.hxx"
#include "OutputPort.hxx"
#include <iostream>
#include <string>

using namespace YACS::ENGINE;
using namespace std;

ConditionInputPort::ConditionInputPort(const std::string& name, WhileLoop *node):InputPort(name,node,Runtime::_tc_bool),
                                                                                 DataPort(name,node,Runtime::_tc_bool),
                                                                                 Port(node),_outOfScopeBackLink(0),_value(0)
{
}

ConditionInputPort::ConditionInputPort(const ConditionInputPort& other, Node *newHelder):InputPort(other,newHelder),
                                                                                         DataPort(other,newHelder),Port(other,newHelder),
                                                                                         _outOfScopeBackLink(0),_value(0)
{
  if(other._value)
    _value=other._value->clone();
}

ConditionInputPort::~ConditionInputPort()
{
  if(_value)
    _value->decrRef();
}

void ConditionInputPort::exSaveInit()
{
  if(_initValue) _initValue->decrRef();
  _initValue=_value;
  _initValue->incrRef();
}

void ConditionInputPort::exRestoreInit()
{
  if(!_initValue)
    return;
  if(_value)
    _value->decrRef();
  _value=_initValue;
  _value->incrRef();
}

InputPort *ConditionInputPort::clone(Node *newHelder) const
{
  return new ConditionInputPort(*this,newHelder);
}

bool ConditionInputPort::isLinkedOutOfScope() const
{
  return _outOfScopeBackLink!=0;
}

void ConditionInputPort::edNotifyReferencedBy(OutPort *fromPort)
{
  if(!((ComposedNode*)(_node))->isInMyDescendance(fromPort->getNode()))
    {
      if(_outOfScopeBackLink)
        throw Exception("ConditionInputPort::edNotifyReferenced : already linked from outside");
      _outOfScopeBackLink=fromPort;
    }
  InputPort::edNotifyReferencedBy(fromPort);
}

void ConditionInputPort::edNotifyDereferencedBy(OutPort *fromPort)
{
  if(fromPort==_outOfScopeBackLink)
    _outOfScopeBackLink=0;
  else
    if(!((ComposedNode*)(_node))->isInMyDescendance(fromPort->getNode()))
      throw Exception("ConditionInputPort::edNotifyDereferencedBy link does not exists");
  InputPort::edNotifyDereferencedBy(fromPort);
}

void *ConditionInputPort::get() const
{
  return (void *)_value;
}

void ConditionInputPort::put(const void *data) throw(ConversionException)
{
  put((Any*)data);
}

void ConditionInputPort::put(Any *data) throw(ConversionException)
{
  if(_value)
    _value->decrRef();
  _value=data;
  _value->incrRef();
}    

std::string ConditionInputPort::dump()
{
  string xmldump;
  if (_value->getBoolValue())
    xmldump="<value><boolean>true</boolean></value>\n";
  else
    xmldump="<value><boolean>false</boolean></value>\n";
  return xmldump;
}

std::string ConditionInputPort::getAsString()
{
  return (getValue() ? "True" : "False");
}

