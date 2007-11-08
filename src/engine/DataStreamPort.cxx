#include "DataStreamPort.hxx"
#include <iostream>

using namespace YACS::ENGINE;
using namespace std;

const char DataStreamPort::NAME[]="DataStreamPort";

DataStreamPort::DataStreamPort(const DataStreamPort& other, Node *newHelder):DataPort(other,newHelder),Port(other,newHelder),_propertyMap(other._propertyMap)
{
}

DataStreamPort::DataStreamPort(const std::string& name, Node *node, TypeCode* type):DataPort(name,node,type),Port(node)
{
}

DataStreamPort::~DataStreamPort()
{
}

string DataStreamPort::getNameOfTypeOfCurrentInstance() const
{
  return NAME;
}

void DataStreamPort::setProperty(const std::string& name, const std::string& value)
{
  _propertyMap[name]=value;
}

std::string DataStreamPort::getProperty(const std::string& name)
{
  return _propertyMap[name];
}
void DataStreamPort::initPortProperties()
{
}

