#include "DataFlowPort.hxx"

using namespace YACS::ENGINE;
using namespace std;

const char DataFlowPort::NAME[]="DataFlowPort";

DataFlowPort::DataFlowPort(const DataFlowPort& other, Node *newHelder):DataPort(other,newHelder),Port(other,newHelder)
{
}

DataFlowPort::DataFlowPort(const std::string& name, Node *node, TypeCode* type):DataPort(name,node,type),Port(node)
{
}

DataFlowPort::~DataFlowPort()
{
}

string DataFlowPort::getNameOfTypeOfCurrentInstance() const
{
  return NAME;
}
