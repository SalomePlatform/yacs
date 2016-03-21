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

#include "AnyInputPort.hxx"
#include "TypeCode.hxx"
#include "Mutex.hxx"
#include "AutoLocker.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

#include <iostream>
#include <iomanip>
#include <sstream>

using namespace YACS::ENGINE;
using namespace std;

AnyInputPort::AnyInputPort(const std::string& name, Node *node, TypeCode* type, bool canBeNull)
  : InputPort(name, node, type, canBeNull),
    DataPort(name, node, type),
    Port(node),
    _value(0)
{
}

AnyInputPort::AnyInputPort(const AnyInputPort& other, Node *newHelder)
  : InputPort(other, newHelder),
    DataPort(other, newHelder),
    Port(other, newHelder),
    _value(0)
{
  if(other._value)
    _value=other._value->clone();
}

AnyInputPort::~AnyInputPort()
{
  if(_value)
    {
      DEBTRACE("_value ref count: " << _value->getRefCnt());
      _value->decrRef();
    }
}

//! Save the current data value for further reinitialization of the port
/*!
 *
 */
void AnyInputPort::exSaveInit()
{
  if(_initValue) _initValue->decrRef();
  _initValue=_value;
  _initValue->incrRef();
}

//! Restore the saved data value to current data value
/*!
 * If no data has been saved (_initValue == 0) don't restore
 */
void AnyInputPort::exRestoreInit()
{
  if(!_initValue)
    return;
  if(_value)
    _value->decrRef();
  _value=_initValue;
  _value->incrRef();
}

void AnyInputPort::put(Any *data)
{
  YACS::BASES::AutoLocker<YACS::BASES::Mutex> lock(&_mutex);
  if(_value)
    _value->decrRef();
  _value=data;
  if (_value) {
    _value->incrRef();
    DEBTRACE("value ref count: " << _value->getRefCnt());
  }
}

bool AnyInputPort::isEmpty()
{
  return !_value;
}

void *AnyInputPort::get() const
{
  return (void *)_value;
}

std::string AnyInputPort::getAsString()
{
  YACS::BASES::AutoLocker<YACS::BASES::Mutex> lock(&_mutex);
  return getRuntime()->convertNeutralAsString(edGetType(),_value);
}

void AnyInputPort::put(const void *data) throw(ConversionException)
{
  put((Any *)data);
}

InputPort *AnyInputPort::clone(Node *newHelder) const
{
  return new AnyInputPort(*this,newHelder);
}

std::string AnyInputPort::dump()
{
  if(!_value)
    {
      std::string what="AnyInputPort::get : no value currently in input port with name \""; what+=_name; what+="\"";
      throw Exception(what);
    }
  stringstream xmldump;
  switch (_value->getType()->kind())
    {
    case Double:
      xmldump << "<value><double>" << setprecision(16) << _value->getDoubleValue() << "</double></value>" << endl;
      break;
    case Int:
      xmldump << "<value><int>" << _value->getIntValue() << "</int></value>" << endl;
      break;
    case Bool:
      xmldump << "<value><boolean>" << _value->getBoolValue() << "</boolean></value>" << endl;
      break;
    case String:
      xmldump << "<value><string>" << _value->getStringValue() << "</string></value>" << endl;
      break;
    default:
      xmldump << "<value><error> NO_SERIALISATION_AVAILABLE </error></value>" << endl;
      break;
    }
  return xmldump.str();
}
