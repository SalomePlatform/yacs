#include "DataPort.hxx"
#include <iostream>

using namespace YACS::ENGINE;
using namespace std;

const char DataPort::NAME[]="DataPort";

DataPort::~DataPort()
{
  _type->decrRef();
}

DataPort::DataPort(const std::string& name, Node *node, TypeCode* type):Port(node),_name(name),_type(type)
{
  _type->incrRef();
}

DataPort::DataPort(const DataPort& other, Node *newHelder):Port(other,newHelder),_name(other._name),_type(other._type)
{
  _type->incrRef();
}

void DataPort::edSetType(TypeCode* type)
{
  if(_type)
    _type->decrRef();
  _type=type;
  if(_type)
    _type->incrRef();
}

string DataPort::getNameOfTypeOfCurrentInstance() const
{
  return NAME;
}

bool DataPort::isDifferentTypeOf(const DataPort *other) const
{
  return getTypeOfChannel()!=other->getTypeOfChannel();
}

/*!
 * If in historyOfLink different type of Port are detected : The first one (by starting from the end of 'historyOfLink')
 * is returned. Else 0 is returned if they are all of the same type.
 */
DataPort *DataPort::isCrossingType(const std::vector<DataPort *>& historyOfLink)
{
  vector<DataPort *>::const_reverse_iterator iter=historyOfLink.rbegin()+1;
  const DataPort *base=historyOfLink.back();
  for(;iter!=historyOfLink.rend();iter++)
    if(base->isDifferentTypeOf(*iter))
      return *iter;
  return 0;
}
