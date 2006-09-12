#include "DataFlowPort.hxx"

using namespace YACS::ENGINE;
using YACS::DynType;

const char DataFlowPort::NAME[]="DataFlowPort";

DataFlowPort::DataFlowPort(const std::string& name, Node *node, DynType type):Port(node),_name(name),_data(type)
{
}

std::string DataFlowPort::getNameOfTypeOfCurrentInstance() const
{
  return NAME;
}

DynType DataFlowPort::edGetType() const
{
  return _data.edGetType();
}

void DataFlowPort::edSetType(DynType type)
{
  _data.edInitToType(type);
}
