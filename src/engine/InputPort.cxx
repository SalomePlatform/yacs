#include "InputPort.hxx"

#include <sstream>
#include <iostream>

using namespace YACS::ENGINE;
using namespace std;

const char InputPort::NAME[]="InputPort";

InputPort::InputPort(const string& name, Node *node, TypeCode* type)
  : DataFlowPort(name,node,type), InPort(node),Port(node), _manuallySet(false), _empty(true)
{
}

string InputPort::getNameOfTypeOfCurrentInstance() const
{
  return NAME;
}

// void InputPort::edInit(Data data) throw(ConversionException)
// {
//   _data=data;
//   _manuallySet=true;
// }

void InputPort::edNotifyReferenced()
{
  _manuallySet=false;
}

void InputPort::exInit()
{
//   if(!_manuallySet)
//     _data.exInit();
}

bool InputPort::isEmpty()
{
  return _empty;
}

void InputPort::edInit(const void *data) throw(ConversionException)
{
  _manuallySet=true;
  put(data);
}

void InputPort::put(const void *data) throw(ConversionException)
{
//   _data = (void *)data;
  cerr << _name << endl;
  cerr << _impl << endl;
  stringstream msg;
  msg << "Not implemented (" << __FILE__ << ":" << __LINE__ << ")";
  throw Exception(msg.str());
}



InputPort::~InputPort()
{
}
