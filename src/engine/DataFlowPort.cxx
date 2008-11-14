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

//! Gives a string representation of the data, for user interfaces.
/*! Implemented in derived classes, depending on runtime, to provide
 *  the data representation for user interfaces. 
 *  Typical use, a Python string representation that can be used in
 *  edition or restitution.
 */
std::string DataFlowPort::valToStr()
{
  return "no display conversion available";
}

//! Allows to set data from a string representation used in user interface.
/*! Implemented in derived classes, depending on runtime, to set the data
 *  from its user interface representation. 
 *  Typical use, a Python string representation that can be used in
 *  edition or restitution.
 */
void DataFlowPort::valFromStr(std::string valstr)
{
}
