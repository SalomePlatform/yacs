
#include "CppNode.hxx"
#include "InputPort.hxx"
#include "OutputPort.hxx"
#include "CppPorts.hxx"
#include "CppContainer.hxx"
#include "CppComponent.hxx"

#include <iostream>
#include <set>
#include <sstream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

const char CppNode::IMPL_NAME[]="Cpp";
const char CppNode::KIND[]="Cpp";

CppNode::CppNode(const CppNode& other,ComposedNode *father):ServiceNode(other,father),
                                                            _run(other._run),
                                                            _componentName(other._componentName)
{
  DEBTRACE("CppNode::CppNode");
  _implementation=IMPL_NAME;
}

CppNode::CppNode(const std::string & name) : ServiceNode(name), _run(NULL)
{
  _implementation=IMPL_NAME;
}

void CppNode::setCode(const::string & componentName, const string & method)
{
  _method = method;
  _componentName = componentName;
  _run = NULL;
}

void CppNode::setFunc(MYRUN fonc) { 
  
  if (_component) 
    {
      _component->decrRef();
      _component = NULL;
      _componentName = "";
      _method = "";
      _component = NULL;
    }
  _run = fonc;
}

void CppNode::load()
{
  if (_run) return;
  
  if (!_component) {
    setRef(_componentName);
  }
  ServiceNode::load();
}

void CppNode::execute()
{
  std::list<InputPort *>::iterator iter1;
  int nIn, nOut, it;
  
  nIn = _setOfInputPort.size();
  nOut = _setOfOutputPort.size();
  
  Any ** In = new Any * [nIn], ** Out = new Any * [nOut];
  
  try 
    {
      
      for(iter1 = _setOfInputPort.begin(), it = 0; iter1 != _setOfInputPort.end(); 
          iter1++, it++)
        {
          InputCppPort *p = dynamic_cast<InputCppPort *> (*iter1);
          In[it] = p->getCppObj();
        }
      
      if (_component) 
	{
	  CppComponent * _componentC = dynamic_cast<CppComponent *>(_component);
	  if (!_componentC)
	    throw YACS::Exception("CppNode::execute : bad type of component");
	  _componentC->run(_method.c_str(), nIn, nOut, In, Out);
        }
      else if (_run)
	_run(nIn, nOut, In, Out);
      
      //output parameters
      std::list<OutputPort *>::iterator iter2;
      for(iter2 = _setOfOutputPort.begin(), it=0; iter2 != _setOfOutputPort.end(); iter2++, it++)
        {
          OutputCppPort *p = dynamic_cast<OutputCppPort *> (*iter2);
          p->put(Out[it]);
          //decref it, we don't need it more
          Out[it]->decrRef();
          DEBTRACE("ref count: " << Out[it]->getRefCnt());
        }
    }
  catch (YACS::Exception & e) {
    delete [] In;
    delete [] Out;
    throw e;
  }
  
  delete [] In;
  delete [] Out;
}

ServiceNode* CppNode::createNode(const std::string& name)
{
  CppNode* node=  new CppNode(name);
  node->setComponent(_component);
  return node;
}

//! Clone the node : must also clone the component instance ?
Node * CppNode::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new CppNode(*this,father);
}

//! Create a new node of same type with a given name
CppNode* CppNode::cloneNode(const std::string& name)
{
  DEBTRACE("CppNode::cloneNode");
  CppNode* n=new CppNode(name);
  
  if (_run)
    n->setFunc(_run);
  
  if (_component)
    n->setCode(_componentName, _method);
  
  list<InputPort *>::iterator iter;
  for(iter = _setOfInputPort.begin(); iter != _setOfInputPort.end(); iter++)
    {
      InputCppPort *p=(InputCppPort *)*iter;
      DEBTRACE("port name: " << p->getName());
      DEBTRACE("port kind: " << p->edGetType()->kind());
      n->edAddInputPort(p->getName(),p->edGetType());
    }
  list<OutputPort *>::iterator iter2;
  for(iter2 = _setOfOutputPort.begin(); iter2 != _setOfOutputPort.end(); iter2++)
    {
      OutputCppPort *p=(OutputCppPort *)*iter2;
      DEBTRACE("port name: " << p->getName());
      DEBTRACE("port kind: " << p->edGetType()->kind());
      n->edAddOutputPort(p->getName(),p->edGetType());
    }
  return n;
}

