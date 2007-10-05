
#include "XMLPorts.hxx"

#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

InputXmlPort::InputXmlPort(const std::string& name, Node * node, TypeCode * type)
  : InputPort(name, node, type), DataPort(name, node, type), Port(node), _initData(""), _data("")
{
}

InputXmlPort::InputXmlPort(const InputXmlPort& other, Node *newHelder):InputPort(other,newHelder),DataPort(other,newHelder),Port(other,newHelder),_initData(other._initData),_data(other._data)
{
}

bool InputXmlPort::edIsManuallyInitialized() const
{
  return _initData != "";
}

void InputXmlPort::edRemoveManInit()
{
  _initData="";
  InputPort::edRemoveManInit();
}

const char *InputXmlPort::getXml() const
{
  DEBTRACE(_data);
  return _data.c_str();
}

void *InputXmlPort::get() const throw(Exception)
{
  return (void *) _data.c_str();
}

void InputXmlPort::put(const void *data) throw (ConversionException)
{
  DEBTRACE("put(void *)");
  put((const char*)data);
}

void InputXmlPort::put(const char *data) throw (ConversionException)
{
  DEBTRACE(data);
  _data = data;
}

bool InputXmlPort::isEmpty()
{
  return _data.empty();
}

InputPort *InputXmlPort::clone(Node *newHelder) const
{
  return new InputXmlPort(*this,newHelder);
}

//! Save the current data value for further reinitialization of the port
/*!
 *
 */
void InputXmlPort::exSaveInit()
{
  _initData=_data;
}

//! Restore the saved data value to current data value
/*!
 * If no data has been saved (_initData == 0) don't restore
 */
void InputXmlPort::exRestoreInit()
{
  if(_initData!="")return;
  _data=_initData;
}

std::string InputXmlPort::dump()
{
  return _data;
}

OutputXmlPort::OutputXmlPort(const std::string& name, Node* node, TypeCode * type)
  : OutputPort(name, node, type), DataPort(name, node, type), Port(node)
{
}

OutputXmlPort::OutputXmlPort(const OutputXmlPort& other, Node *newHelder):OutputPort(other,newHelder),DataPort(other,newHelder),Port(other,newHelder)
{
}

const char * OutputXmlPort::get() const throw (ConversionException)
{
  return _data.c_str();
}

void OutputXmlPort::put(const void *data) throw (ConversionException)
{
  put((const char*)data);
}

void OutputXmlPort::put(const char *data)  throw (ConversionException)
{
  DEBTRACE(data);
  InputPort *p;
  _data=data;
  OutputPort::put(data);
}

OutputPort *OutputXmlPort::clone(Node *newHelder) const
{
  return new OutputXmlPort(*this,newHelder);
}

std::string OutputXmlPort::dump()
{
  return _data;
}

