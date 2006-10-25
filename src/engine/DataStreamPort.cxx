#include "DataStreamPort.hxx"

using namespace YACS::ENGINE;
using namespace std;

const char DataStreamPort::NAME[]="DataStreamPort";

DataStreamPort::DataStreamPort(const string& name, Node *node, TypeCode* type):Port(node),_name(name),_edType(type)
{
}

string DataStreamPort::getNameOfTypeOfCurrentInstance() const
{
  return NAME;
}
