#include "InputDataStreamPort.hxx"

using namespace YACS::ENGINE;

const char InputDataStreamPort::NAME[]="InputDataStreamPort";

InputDataStreamPort::InputDataStreamPort(const std::string& name, Node *node, StreamType type):DataStreamPort(name,node,type),
											       InPort(node),
											       Port(node)
{
}

std::string InputDataStreamPort::getNameOfTypeOfCurrentInstance() const
{
  return NAME;
}
