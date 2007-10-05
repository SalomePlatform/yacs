#include "InputDataStreamPort.hxx"
#include <iostream>

using namespace YACS::ENGINE;
using namespace std;

const char InputDataStreamPort::NAME[]="InputDataStreamPort";

InputDataStreamPort::InputDataStreamPort(const InputDataStreamPort& other, Node *newHelder):
    DataStreamPort(other,newHelder),
    InPort(other,newHelder),
    DataPort(other,newHelder),
    Port(other,newHelder)
{
}

InputDataStreamPort::InputDataStreamPort(const std::string& name, Node *node, TypeCode* type):
    DataStreamPort(name,node,type),
    InPort(name,node,type),
    DataPort(name,node,type),
    Port(node)
{
}

InputDataStreamPort::~InputDataStreamPort()
{
}

string InputDataStreamPort::getNameOfTypeOfCurrentInstance() const
{
  return NAME;
}

InputDataStreamPort *InputDataStreamPort::clone(Node *newHelder) const
{
  return new InputDataStreamPort(*this,newHelder);
}
