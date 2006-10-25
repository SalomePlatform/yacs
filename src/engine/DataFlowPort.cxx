#include "DataFlowPort.hxx"

using namespace YACS::ENGINE;
using namespace std;

const char DataFlowPort::NAME[]="DataFlowPort";

DataFlowPort::DataFlowPort(const string& name, Node *node, TypeCode* type):Port(node),_name(name)
{
  _type = type;
}

string DataFlowPort::getNameOfTypeOfCurrentInstance() const
{
  return NAME;
}

TypeCode* DataFlowPort::edGetType() const
{
  return _type;
}

TypeCode * DataFlowPort::type()
{
  return _type;
}

void DataFlowPort::edSetType(TypeCode* type)
{
  _type = type;
}

