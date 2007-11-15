
#include "PresetPorts.hxx"
#include <iostream>

using namespace YACS::ENGINE;
using namespace std;

OutputPresetPort::OutputPresetPort(const std::string& name,  Node* node, TypeCode* type)
  : OutputXmlPort(name, node, type),
    DataPort(name, node, type),
    Port(node)
{
}

OutputPresetPort::OutputPresetPort(const OutputPresetPort& other, Node *newHelder)
  : OutputXmlPort(other,newHelder),
    DataPort(other,newHelder),
    Port(other,newHelder)
{
}

void OutputPresetPort::setData(std::string data)
{
  _storeData = data;
  cerr << "OutputPresetPort::setData " << _storeData << endl;
}

std::string OutputPresetPort::getData()
{
  cerr << "OutputPresetPort::getData " << _storeData << endl;
  return _storeData;
}
