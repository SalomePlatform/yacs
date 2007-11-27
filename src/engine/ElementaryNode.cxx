#include "ElementaryNode.hxx"
#include "Runtime.hxx"
#include "InputPort.hxx"
#include "OutputPort.hxx"
#include "ComposedNode.hxx"
#include "InputDataStreamPort.hxx"
#include "OutputDataStreamPort.hxx"
#include "Visitor.hxx"
#include <iostream>

using namespace YACS::ENGINE;
using namespace std;

ElementaryNode::ElementaryNode(const std::string& name):Node(name)
{
}

ElementaryNode::ElementaryNode(const ElementaryNode& other, ComposedNode *father):Node(other,father)
{
  for(list<InputPort *>::const_iterator iter1=other._setOfInputPort.begin();iter1!=other._setOfInputPort.end();iter1++)
    _setOfInputPort.push_back((InputPort *)(*iter1)->clone(this));
  for(list<OutputPort *>::const_iterator iter2=other._setOfOutputPort.begin();iter2!=other._setOfOutputPort.end();iter2++)
    _setOfOutputPort.push_back((OutputPort *)(*iter2)->clone(this));
  for(list<InputDataStreamPort *>::const_iterator iter3=other._setOfInputDataStreamPort.begin();iter3!=other._setOfInputDataStreamPort.end();iter3++)
    _setOfInputDataStreamPort.push_back((InputDataStreamPort *)(*iter3)->clone(this));
  for(list<OutputDataStreamPort *>::const_iterator iter4=other._setOfOutputDataStreamPort.begin();iter4!=other._setOfOutputDataStreamPort.end();iter4++)
    _setOfOutputDataStreamPort.push_back((OutputDataStreamPort *)(*iter4)->clone(this));
}

void ElementaryNode::performDuplicationOfPlacement(const Node& other)
{
}

ElementaryNode::~ElementaryNode()
{
  for(list<InputPort *>::iterator iter1=_setOfInputPort.begin();iter1!=_setOfInputPort.end();iter1++)
    delete *iter1;
  for(list<OutputPort *>::iterator iter2=_setOfOutputPort.begin();iter2!=_setOfOutputPort.end();iter2++)
    delete *iter2;
  for(list<InputDataStreamPort *>::iterator iter3=_setOfInputDataStreamPort.begin();iter3!=_setOfInputDataStreamPort.end();iter3++)
    delete *iter3;
  for(list<OutputDataStreamPort *>::iterator iter4=_setOfOutputDataStreamPort.begin();iter4!=_setOfOutputDataStreamPort.end();iter4++)
    delete *iter4;
}

void ElementaryNode::init(bool start)
{
  for(list<OutputPort *>::iterator iter=_setOfOutputPort.begin();iter!=_setOfOutputPort.end();iter++)
    (*iter)->exInit();
  for(list<InputPort *>::iterator iter2=_setOfInputPort.begin();iter2!=_setOfInputPort.end();iter2++)
    (*iter2)->exInit(start);
  _inGate.exReset();
  if(_state == YACS::DISABLED)
    {
      exDisabledState(); // to refresh propagation of DISABLED state
      return;
    }
  if(start) //complete initialization
    setState(YACS::INITED);
  else //partial initialization (inside a loop)
    _state=YACS::LOADED;
}

bool ElementaryNode::isDeployable() const
{
  return false;
}

ComponentInstance *ElementaryNode::getComponent()
{
  return 0;
}

YACS::StatesForNode ElementaryNode::getState() const
{
  return Node::getState();
}

void ElementaryNode::exUpdateState()
{
  if(_state==YACS::DISABLED)return;
  if(_inGate.exIsReady())
    if(areAllInputPortsValid())
      if(_state == YACS::INITED)
	setState(YACS::TOLOAD);
      else
        setState(YACS::TOACTIVATE);
    else
      {
        string what("ElementaryNode::exUpdateState : Invalid graph given : Node with name \"");
        what+=_name; what+="\" ready to run whereas some inputports are not set correctly\nCheck coherence DF/CF";
        setState(YACS::INTERNALERR);
        throw Exception(what);
      }
}

int ElementaryNode::getNumberOfInputPorts() const
{
  return _setOfInputPort.size();
}

int ElementaryNode::getNumberOfOutputPorts() const
{
  return _setOfOutputPort.size();
}

InputPort *ElementaryNode::getInputPort(const std::string& name) const throw(Exception)
{
  return getPort<InputPort>(name,_setOfInputPort);
}

OutputPort *ElementaryNode::getOutputPort(const std::string& name) const throw(Exception)
{
  return getPort<OutputPort>(name,_setOfOutputPort);
}

std::set<OutPort *> ElementaryNode::getAllOutPortsLeavingCurrentScope() const
{
  set<OutPort *> ret;
  list<OutPort *> temp=getSetOfOutPort();
  for(list<OutPort *>::iterator iter2=temp.begin();iter2!=temp.end();iter2++)
    {
      set<InPort *> temp2=(*iter2)->edSetInPort();
      if(temp2.size()!=0)
        ret.insert(*iter2);
    }
  return ret;
}

std::set<InPort *> ElementaryNode::getAllInPortsComingFromOutsideOfCurrentScope() const
{
  set<InPort *> ret;
  list<InPort *> temp=getSetOfInPort();
  for(list<InPort *>::iterator iter2=temp.begin();iter2!=temp.end();iter2++)
    {
      set<OutPort *> temp2=(*iter2)->edSetOutPort();
      if(temp2.size()!=0)
        ret.insert(*iter2);
    }
  return ret;
}

std::vector< std::pair<OutPort *, InPort *> > ElementaryNode::getSetOfLinksLeavingCurrentScope() const
{
  vector< pair<OutPort *, InPort *> > ret;
  std::set<OutPort *> ports=getAllOutPortsLeavingCurrentScope();
  for(set<OutPort *>::iterator iter2=ports.begin();iter2!=ports.end();iter2++)
    {
      set<InPort *> temp2=(*iter2)->edSetInPort();
      for(set<InPort *>::iterator iter3=temp2.begin();iter3!=temp2.end();iter3++)
        ret.push_back(pair<OutPort *, InPort *>(*iter2,*iter3));
    }
  return ret;
}

std::vector< std::pair<InPort *, OutPort *> > ElementaryNode::getSetOfLinksComingInCurrentScope() const
{
  vector< pair<InPort *, OutPort *> > ret;
  set<InPort *> ports=getAllInPortsComingFromOutsideOfCurrentScope();
  for(set<InPort *>::iterator iter2=ports.begin();iter2!=ports.end();iter2++)
    {
      set<OutPort *> temp2=(*iter2)->edSetOutPort();
      for(set<OutPort *>::iterator iter3=temp2.begin();iter3!=temp2.end();iter3++)
        ret.push_back(pair<InPort *, OutPort *>(*iter2,*iter3));
    }
  return ret;
}

InputDataStreamPort *ElementaryNode::getInputDataStreamPort(const std::string& name) const throw(Exception)
{
  return getPort<InputDataStreamPort>(name,_setOfInputDataStreamPort);
}

OutputDataStreamPort *ElementaryNode::getOutputDataStreamPort(const std::string& name) const throw(Exception)
{
  return getPort<OutputDataStreamPort>(name,_setOfOutputDataStreamPort);
}

void ElementaryNode::edDisconnectAllLinksWithMe()
{
  //CF
  Node::edDisconnectAllLinksWithMe();
  //Leaving part
  // - DF
  for(list<InputPort *>::iterator iter=_setOfInputPort.begin();iter!=_setOfInputPort.end();iter++)
    (*iter)->edRemoveAllLinksLinkedWithMe();
  // - DS
  for(list<InputDataStreamPort *>::iterator iter2=_setOfInputDataStreamPort.begin();iter2!=_setOfInputDataStreamPort.end();iter2++)
    (*iter2)->edRemoveAllLinksLinkedWithMe();
  //Arriving part
  // - DF
  for(list<OutputPort *>::iterator iter=_setOfOutputPort.begin();iter!=_setOfOutputPort.end();iter++)
    (*iter)->edRemoveAllLinksLinkedWithMe();
  // - DS
  for(list<OutputDataStreamPort *>::iterator iter2=_setOfOutputDataStreamPort.begin();iter2!=_setOfOutputDataStreamPort.end();iter2++)
    (*iter2)->edRemoveAllLinksLinkedWithMe();
}

/**
 * checks if all input ports contains a valid data. Used at execution to change the state of the node
 * for activation.
 */

bool ElementaryNode::areAllInputPortsValid() const
{
  bool ret=true;
  for(list<InputPort *>::const_iterator iter=_setOfInputPort.begin();iter!=_setOfInputPort.end();iter++)
    {
      ret=!(*iter)->isEmpty();
      if (!ret) break;
    }
  return ret;
}

/**
 * add this node task to a given set of ready tasks, if this task is ready to activate
 */

void ElementaryNode::getReadyTasks(std::vector<Task *>& tasks)
{
  if(_state==YACS::TOACTIVATE or _state==YACS::TOLOAD)
    tasks.push_back(this);
}

/**
 * remove port from node at edition. Ports are typed.
 */

void ElementaryNode::edRemovePort(Port *port) throw(Exception)
{
  if(port->getNode()!=this)
    throw Exception("ElementaryNode::edRemovePort : Port is not held by this");
  string typeOfPortInstance=port->getNameOfTypeOfCurrentInstance();
  if(typeOfPortInstance==InputPort::NAME)
    edRemovePortTypedFromSet<InputPort>(dynamic_cast<InputPort *>(port),_setOfInputPort);
  else if(typeOfPortInstance==OutputPort::NAME)
    edRemovePortTypedFromSet<OutputPort>(dynamic_cast<OutputPort *>(port),_setOfOutputPort);
  else if(typeOfPortInstance==InputDataStreamPort::NAME)
    edRemovePortTypedFromSet<InputDataStreamPort>(dynamic_cast<InputDataStreamPort *>(port),_setOfInputDataStreamPort);
  else if(typeOfPortInstance==OutputDataStreamPort::NAME)
    edRemovePortTypedFromSet<OutputDataStreamPort>(dynamic_cast<OutputDataStreamPort *>(port),_setOfOutputDataStreamPort);
  else
    throw Exception("ElementaryNode::edRemovePort : unknown port type");
  delete port;
}

/**
 * @return a set with only this node. (Same method in composed nodes)
 */

set<ElementaryNode *> ElementaryNode::getRecursiveConstituents() const
{
  set<ElementaryNode *> ret;
  ret.insert((ElementaryNode *)this);
  return ret;
}

Node *ElementaryNode::getChildByName(const std::string& name) const throw(Exception)
{
  string what("ElementaryNode does not agregate any nodes particullary node with name "); what+=name;
  throw Exception(what);
}

void ElementaryNode::checkBasicConsistency() const throw(Exception)
{
  list<InputPort *>::const_iterator iter;
  for(iter=_setOfInputPort.begin();iter!=_setOfInputPort.end();iter++)
    (*iter)->checkBasicConsistency();  
}

ComposedNode *ElementaryNode::getDynClonerIfExists(const ComposedNode *levelToStop) const
{
  for(ComposedNode *iter=_father;iter!=levelToStop && iter!=0; iter=iter->_father)
    if(!iter->isPlacementPredictableB4Run())
      return iter;
  return 0;
}

/**
 * the input port is also published recursively in ancestors because it may be visible from everywhere.
 * WARNING: CHECK CASE OF BLOC: ONLY INPUT PORTS NOT INTERNALLY CONNECTED MUST BE VISIBLE.
 */

InputPort *ElementaryNode::edAddInputPort(const std::string& inputPortName, TypeCode* type) throw(Exception)
{
  InputPort *ret = 0;
  if (edCheckAddPort<InputPort, TypeCode*>(inputPortName,_setOfInputPort,type))
    {
      ret = getRuntime()->createInputPort(inputPortName, _implementation, this, type);
      _setOfInputPort.push_back(ret);
      ComposedNode *iter=_father;
      while(iter)
        iter=iter->_father;
    }
  return ret;
}

/**
 * TO SOLVE : The output port is not published in father. Father must create an output port.
 * for now, publication is done the same way as input ports
 */ 

OutputPort *ElementaryNode::edAddOutputPort(const std::string& outputPortName, TypeCode* type) throw(Exception)
{
  OutputPort *ret =0;
  if (edCheckAddPort<OutputPort, TypeCode*>(outputPortName,_setOfOutputPort,type))
    {
      ret = getRuntime()->createOutputPort(outputPortName, _implementation, this, type);
      _setOfOutputPort.push_back(ret);
      ComposedNode *iter=_father;
      while(iter)
        iter=iter->_father;
    }
  return ret;
}

InputDataStreamPort *ElementaryNode::edAddInputDataStreamPort(const std::string& inputPortDSName, TypeCode* type) throw(Exception)
{
  InputDataStreamPort *ret = 0;
  if (edCheckAddPort<InputDataStreamPort, TypeCode*>(inputPortDSName,_setOfInputDataStreamPort,type))
    {
      ret = getRuntime()->createInputDataStreamPort(inputPortDSName, this, type);
      _setOfInputDataStreamPort.push_back(ret);
    }
  return ret;
}

OutputDataStreamPort *ElementaryNode::edAddOutputDataStreamPort(const std::string& outputPortDSName, TypeCode* type) throw(Exception)
{
  OutputDataStreamPort *ret = 0;
  if (edCheckAddPort<OutputDataStreamPort, TypeCode*>(outputPortDSName,_setOfOutputDataStreamPort,type))
    {
      ret = getRuntime()->createOutputDataStreamPort(outputPortDSName, this, type);
      _setOfOutputDataStreamPort.push_back(ret);
    }
  return ret;
}

/**
 * get the input port name used by the current node (see composed nodes)
 */

string ElementaryNode::getInPortName(const InPort * inPort) const throw (Exception)
{
  Node *node = inPort->getNode();
  if ( node != this ) 
    {
      string what("InputPort "); what += inPort->getName(); what += " does not belong to node "; what += node->getName();
      throw Exception(what);
    }
  return inPort->getName();
}

string ElementaryNode::getOutPortName(const OutPort *outPort) const throw (Exception)
{
  Node *node = outPort->getNode();
  if ( node != this ) 
    {
      string what("OutputPort "); what += outPort->getName(); what += " does not belong to node "; what += node->getName();
      throw Exception(what);
    }
  return outPort->getName();
}

void ElementaryNode::begin()
{
  setState(ACTIVATED);
}

bool ElementaryNode::isReady()
{
  return _state==TOACTIVATE;
}

void ElementaryNode::finished()
{
  setState(DONE);
}
void ElementaryNode::aborted()
{
  setState(ERROR);
}

//! Notify this node that it is loaded
/*!
 * When an elementary node has been loaded it goes to TOACTIVATE state
 * It is then ready to be executed
 *
 */
void ElementaryNode::loaded()
{
  setState(TOACTIVATE);
}

void ElementaryNode::accept(Visitor *visitor)
{
  visitor->visitElementaryNode(this);
}

//! Give a description of error when node status is ERROR
/*!
 *
 */
std::string ElementaryNode::getErrorDetails()
{
  return _errorDetails;
}
