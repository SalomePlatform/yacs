
#include "TypeConversions.hxx"
#include "PresetPorts.hxx"
#include "TypeCode.hxx"
#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

OutputPresetPort::OutputPresetPort(const std::string& name,  Node* node, TypeCode* type)
  : OutputXmlPort(name, node, type),
    DataPort(name, node, type),
    Port(node)
{
}

OutputPresetPort::OutputPresetPort(const OutputPresetPort& other, Node *newHelder)
  : OutputXmlPort(other,newHelder),
    DataPort(other,newHelder),
    Port(other,newHelder)
{
}

void OutputPresetPort::setData(std::string data)
{
  _storeData = data;
  DEBTRACE( "OutputPresetPort::setData " << _storeData );
  modified();
}

void OutputPresetPort::checkBasicConsistency() const throw(Exception)
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
        case Struct:
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
  return convertXmlStrPyObject(edGetType(),getData());
}

std::string OutputPresetPort::getAsString()
{
  PyObject* ob=getPyObj();
  std::string s=convertPyObjectToString(ob);
  Py_DECREF(ob);
  return s;
}


InputPresetPort::InputPresetPort(const std::string& name,  Node* node, TypeCode* type)
  : InputXmlPort(name, node, type),
    DataPort(name, node, type),
    Port(node)
{
}

InputPresetPort::InputPresetPort(const InputPresetPort& other, Node *newHelder)
  : InputXmlPort(other,newHelder),
    DataPort(other,newHelder),
    Port(other,newHelder)
{
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
  return convertXmlStrPyObject(edGetType(),dump());
}

std::string InputPresetPort::getAsString()
{
  PyObject* ob=getPyObj();
  std::string s=convertPyObjectToString(ob);
  Py_DECREF(ob);
  return s;
}

