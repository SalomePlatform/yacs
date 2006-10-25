#include "InputDataStreamPort.hxx"

using namespace YACS::ENGINE;
using namespace std;

const char InputDataStreamPort::NAME[]="InputDataStreamPort";

InputDataStreamPort::InputDataStreamPort(const string& name, Node *node, TypeCode* type):DataStreamPort(name,node,type),
											       InPort(node),
											       Port(node)
{
}

string InputDataStreamPort::getNameOfTypeOfCurrentInstance() const
{
  return NAME;
}
