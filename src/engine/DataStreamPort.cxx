#include "DataStreamPort.hxx"

using namespace YACS::ENGINE;

const char DataStreamPort::NAME[]="DataStreamPort";

DataStreamPort::DataStreamPort(const std::string& name, Node *node, StreamType type):Port(node),_name(name),_edType(type)
{
}

std::string DataStreamPort::getNameOfTypeOfCurrentInstance() const
{
  return NAME;
}
