#include "AnyInputPort.hxx"
#include "TypeCode.hxx"
#include <iostream>
#include <sstream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

AnyInputPort::AnyInputPort(const std::string& name, Node *node, TypeCode* type):InputPort(name,node,type),DataPort(name,node,type),Port(node),_value(0)
{
}

AnyInputPort::AnyInputPort(const  AnyInputPort& other, Node *newHelder):InputPort(other,newHelder),DataPort(other,newHelder),Port(other,newHelder),_value(0)
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
   if(_value)
    _value->decrRef();
  _value=data;
  _value->incrRef();
  DEBTRACE("value ref count: " << _value->getRefCnt());
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
      std::string what="AnyInputPort::get : no value currently in input whith name \""; what+=_name; what+="\"";
      throw Exception(what);
    }
  stringstream xmldump;
  switch (_value->getType()->kind())
    {
    case Double:
      xmldump << "<value><double>" << _value->getDoubleValue() << "</double></value>" << endl;
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
