#include "ConditionInputPort.hxx"
#include "WhileLoop.hxx"
#include "OutputPort.hxx"
#include <iostream>
#include <string>

using namespace YACS::ENGINE;
using namespace std;

ConditionInputPort::ConditionInputPort(const std::string& name, WhileLoop *node):InputPort(name,node,Runtime::_tc_bool),
                                                                                 DataPort(name,node,Runtime::_tc_bool),
                                                                                 Port(node),_outOfScopeBackLink(0),_value(0)
{
}

ConditionInputPort::ConditionInputPort(const ConditionInputPort& other, Node *newHelder):InputPort(other,newHelder),
                                                                                         DataPort(other,newHelder),Port(other,newHelder),
                                                                                         _outOfScopeBackLink(0),_value(0)
{
  if(other._value)
    _value=other._value->clone();
}

ConditionInputPort::~ConditionInputPort()
{
  if(_value)
    _value->decrRef();
}

void ConditionInputPort::exSaveInit()
{
  if(_initValue) _initValue->decrRef();
  _initValue=_value;
  _initValue->incrRef();
}

void ConditionInputPort::exRestoreInit()
{
  if(!_initValue)
    return;
  if(_value)
    _value->decrRef();
  _value=_initValue;
  _value->incrRef();
}

InputPort *ConditionInputPort::clone(Node *newHelder) const
{
  return new ConditionInputPort(*this,newHelder);
}

bool ConditionInputPort::isLinkedOutOfScope() const
{
  return _outOfScopeBackLink!=0;
}

void ConditionInputPort::edNotifyReferencedBy(OutPort *fromPort)
{
  if(!((ComposedNode*)(_node))->isInMyDescendance(fromPort->getNode()))
    {
      if(_outOfScopeBackLink)
        throw Exception("ConditionInputPort::edNotifyReferenced : already linked from outside");
      _outOfScopeBackLink=fromPort;
      return ;
    }
  InputPort::edNotifyReferencedBy(fromPort);
}

void ConditionInputPort::edNotifyDereferencedBy(OutPort *fromPort)
{
  if(fromPort==_outOfScopeBackLink)
    _outOfScopeBackLink=0;
  else
    if(!((ComposedNode*)(_node))->isInMyDescendance(fromPort->getNode()))
      throw Exception("ConditionInputPort::edNotifyDereferencedBy link does not exists");
  InputPort::edNotifyDereferencedBy(fromPort);
}

void *ConditionInputPort::get() const
{
  return (void *)_value;
}

void ConditionInputPort::put(const void *data) throw(ConversionException)
{
  Any *dataCst=(Any *)data;
  if(_value)
    _value->decrRef();
  _value=dataCst;
  _value->incrRef();
}    

std::string ConditionInputPort::dump()
{
  string xmldump;
  if (_value->getBoolValue())
    xmldump="<value><boolean>true</boolean></value>\n";
  else
    xmldump="<value><boolean>false</boolean></value>\n";
  return xmldump;
}
