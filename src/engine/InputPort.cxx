#include "InputPort.hxx"

using namespace YACS::ENGINE;

const char InputPort::NAME[]="InputPort";

InputPort::InputPort(const std::string& name, Node *node, DynType type):DataFlowPort(name,node,type),InPort(node),Port(node),_manuallySet(false)
{
}

std::string InputPort::getNameOfTypeOfCurrentInstance() const
{
  return NAME;
}

void InputPort::edInit(Data data) throw(ConversionException)
{
  _data=data;
  _manuallySet=true;
}

void InputPort::edNotifyReferenced()
{
  _manuallySet=false;
}

void InputPort::exInit()
{
  if(!_manuallySet)
    _data.exInit();
}

Data InputPort::exGet() const
{
  return _data;
}

void InputPort::exAccept(Data data) throw(ConversionException)
{
  _data=data;
}

InputPort::~InputPort()
{
}
