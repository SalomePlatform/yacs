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
#include "PresetPorts.hxx"
#include "PythonPorts.hxx"
#include "TypeCode.hxx"
#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

/*! \class YACS::ENGINE::OutputPresetPort
 *  \brief Class for PRESET output Ports
 *
 * \ingroup Ports
 *
 * \see PresetNode
 */

OutputPresetPort::OutputPresetPort(const std::string& name,  Node* node, TypeCode* type)
  : OutputXmlPort(name, node, type),
    DataPort(name, node, type),
    Port(node)
{
}

OutputPresetPort::OutputPresetPort(const OutputPresetPort& other, Node *newHelder)
  : OutputXmlPort(other,newHelder),
    DataPort(other,newHelder),
    Port(other,newHelder),_storeData(other._storeData)
{
}

OutputPort* OutputPresetPort::clone(Node *newHelder) const
{
  return new OutputPresetPort(*this,newHelder);
}

void OutputPresetPort::setData(std::string data)
{
  _storeData = data;
  DEBTRACE( "OutputPresetPort::setData " << _storeData );
  modified();
}

void OutputPresetPort::checkBasicConsistency() const throw(YACS::Exception)
{
  DEBTRACE("OutputPresetPort::checkBasicConsistency " << _storeData);
  if (_storeData.empty())
    {
      std::string what("OutputPresetPort: ");
      what += getName();
      what += " is not initialised";
      throw Exception(what);
    }
}

std::string OutputPresetPort::getData()
{
  DEBTRACE("OutputPresetPort::getData " << _storeData);
  if(_storeData.substr(0,7) == "<value>")
    {
      return _storeData;
    }
  else
    {
      std::string value;
      switch(edGetType()->kind())
        {
        case Double:
          value="<value><double>"+_storeData+"</double></value>";
          break;
        case Int:
          value="<value><int>"+_storeData+"</int></value>";
          break;
        case String:
          value="<value><string>"+_storeData+"</string></value>";
          break;
        case Bool:
          value="<value><boolean>"+_storeData+"</boolean></value>";
          break;
        case Objref:
          value="<value><objref>"+_storeData+"</objref></value>";
          break;
        case Sequence:
        case Array:
          value="<value><array><data>"+_storeData+"</data></array></value>";
          break;
        case Struct:
          value="<value><struct><data>"+_storeData+"</data></struct></value>";
          break;
        default:
          break;
        }
      return value;
    }
}

std::string OutputPresetPort::dump()
{
  return getData();
}

PyObject * OutputPresetPort::getPyObj()
{
  DEBTRACE(getData());
  if(_storeData=="")
    {
      Py_INCREF(Py_None);
      return Py_None;
    }
  else
    return convertXmlStrPyObject(edGetType(),getData());
}

std::string OutputPresetPort::getAsString()
{
  InterpreterUnlocker loc;
  PyObject* ob=getPyObj();
  DEBTRACE(PyObject_Str(ob));
  std::string s=convertPyObjectToString(ob);
  DEBTRACE(s);
  Py_DECREF(ob);
  return s;
}

/*! \class YACS::ENGINE::InputPresetPort
 *  \brief Class for PRESET input Ports
 *
 * \ingroup Ports
 *
 * \see OutNode
 */

InputPresetPort::InputPresetPort(const std::string& name,  Node* node, TypeCode* type)
  : InputXmlPort(name, node, type),
    DataPort(name, node, type),
    Port(node)
{
}

InputPresetPort::InputPresetPort(const InputPresetPort& other, Node *newHelder)
  : InputXmlPort(other,newHelder),
    DataPort(other,newHelder),
    Port(other,newHelder),_storeData(other._storeData)
{
}

InputPort* InputPresetPort::clone(Node *newHelder) const
{
  return new InputPresetPort(*this,newHelder);
}

void InputPresetPort::setData(std::string data)
{
  _storeData = data;
  DEBTRACE( "InputPresetPort::setData " << _storeData );
  modified();
}

std::string InputPresetPort::getData()
{
  DEBTRACE("InputPresetPort::getData " << _storeData);
  return _storeData;
}

std::string InputPresetPort::dump()
{
  return _data;
}

PyObject * InputPresetPort::getPyObj()
{
  DEBTRACE(dump());
  if(_data=="")
    {
      Py_INCREF(Py_None);
      return Py_None;
    }
  else
    return convertXmlStrPyObject(edGetType(),dump());
}

std::string InputPresetPort::getAsString()
{
  InterpreterUnlocker loc;
  PyObject* ob=getPyObj();
  DEBTRACE(PyObject_Str(ob));
  std::string s=convertPyObjectToString(ob);
  DEBTRACE(s);
  Py_DECREF(ob);
  return s;
}

