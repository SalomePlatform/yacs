#include "Node.hxx"
#include "InputPort.hxx"
#include "OutputPort.hxx"
#include "ComposedNode.hxx"
#include "Dispatcher.hxx"
#include "InputDataStreamPort.hxx"
#include "OutputDataStreamPort.hxx"
#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

const char Node::SEP_CHAR_IN_PORT[]=".";

int Node::_total = 0;
std::map<int,Node*> Node::idMap;

Node::Node(const std::string& name):_name(name),_inGate(this),_outGate(this),_father(0),_state(YACS::INITED),
                                    _implementation(Runtime::RUNTIME_ENGINE_INTERACTION_IMPL_NAME)
{
  // Should be protected by lock ??
  _numId = _total++;
  idMap[_numId]=this;
}

Node::Node(const Node& other, ComposedNode *father):_inGate(this),_outGate(this),_name(other._name),_father(father),
                                                   _state(YACS::INITED),_implementation(other._implementation),
                                                    _propertyMap(other._propertyMap)
{
  _numId = _total++;
  idMap[_numId]=this;
}

Node::~Node()
{
}

/**
 *  initialisation of all input and output ports and gates, for execution
 */

void Node::init(bool start)
{
  _inGate.exReset();
  _outGate.exReset();
  if(_state == YACS::DISABLED)
    {
      exDisabledState(); // to refresh propagation of DISABLED state 
      return;
    }
  setState(YACS::INITED);
}

Node *Node::clone(ComposedNode *father, bool editionOnly) const
{
  Node *ret=simpleClone(father,editionOnly);
  ret->performDuplicationOfPlacement(*this);
  return ret;
}

//! Change the name of the node
/*!
 *  raise an exception if the name is already used in the scope of its father 
 *  \param name : the new name
 */
void Node::setName(const std::string& name)
{
  if(_father)
    {
      if(_father->isNameAlreadyUsed(name))
        {
	  if ( _father->getChildByName(name) != this )
	    {
	      std::string what("Name "); 
	      what+=name;
	      what+=" already exists in the scope of "; what+=_father->getName();
	      throw Exception(what);
	    }
        }
    }
  _name=name;
}

/**
 *  get the set of all nodes connected to the outGate
 */

set<Node *> Node::getOutNodes() const
{
  set<Node *> ret;
  set<InGate *> inGates=_outGate.edSetInGate();
  for(set<InGate *>::iterator iter=inGates.begin();iter!=inGates.end();iter++)
    ret.insert((*iter)->getNode());
  return ret;
}

bool Node::exIsControlReady() const
{
  return _inGate.exIsReady();
}

/*!
 * \note : Update the '_state' attribute.
 *          Typically called by 'this->_inGate' when 'this->_inGate' is ready.
 *
 *          Called by InGate::exNotifyFromPrecursor 
 */

void Node::exUpdateState()
{
  if(_state==YACS::DISABLED)return;
  if(_inGate.exIsReady())
    setState(YACS::TOACTIVATE);
}

//! Notify this node that its execution has failed
/*!
 * The node goes in FAILED state and 
 * propagate the notification through the outGate port
 *
 */
void Node::exFailedState()
{
  DEBTRACE( "Node::exFailedState: " << getName() );
  setState(YACS::FAILED);
  _outGate.exNotifyFailed();
}

//! Notify this node that it has been disabled
/*!
 * The node goes in DISABLED state and
 * propagate the notification through the outGate port
 *
 */
void Node::exDisabledState()
{
  DEBTRACE( "Node::exDisabledState: " << getName() );
  setState(YACS::DISABLED);
  _outGate.exNotifyDisabled();
}

InPort *Node::getInPort(const std::string& name) const throw(Exception)
{
  InPort *ret;
  try
    {
      ret=getInputPort(name);
    }
  catch(Exception& e)
    {
      ret=getInputDataStreamPort(name);
    }
  return ret;
}

/*!
 * \note: Contrary to getOutputPort method, this method returns the output port at highest level, possible.
 *        That is to say in some ComposedNode, like ForEachLoop or Switch, an outport inside 'this' is seen differently than the true outport.
 */
OutPort *Node::getOutPort(const std::string& name) const throw(Exception)
{
  OutPort *ret;
  try
    {
      ret=getOutputPort(name);
    }
  catch(Exception& e)
    {
      ret=getOutputDataStreamPort(name);
    }
  return ret;
}

std::list<InPort *> Node::getSetOfInPort() const
{
  list<InPort *> ret;
  list<InputPort *> data=getSetOfInputPort();
  ret.insert(ret.end(),data.begin(),data.end());
  list<InputDataStreamPort *> ds=getSetOfInputDataStreamPort();
  ret.insert(ret.end(),ds.begin(),ds.end());
  return ret;
}

std::list<OutPort *> Node::getSetOfOutPort() const
{
  list<OutPort *> ret;
  list<OutputPort *> data=getSetOfOutputPort();
  ret.insert(ret.end(),data.begin(),data.end());
  list<OutputDataStreamPort *> ds=getSetOfOutputDataStreamPort();
  ret.insert(ret.end(),ds.begin(),ds.end());
  return ret;
}

/**
 * gets a set of the composed nodes that constitute the ascendancy of this node, starting from root
 * or from a particular ancestor
 * \b WARNING : returned set is not sorted !
 * @param  levelToStop   composed node which is the oldest ancestor required
 * @return               ascendancy, direct father first in set.
 */

std::list<ComposedNode *> Node::getAllAscendanceOf(ComposedNode *levelToStop) const
{
  list<ComposedNode *> ret;
  if(this==levelToStop)
    return ret;
  for(ComposedNode *iter=_father;iter!=levelToStop && iter!=0; iter=iter->_father)
      ret.push_back(iter);
  return ret;
}

bool Node::operator>(const Node& other) const
{
  const ComposedNode *iter=other._father;
  while(iter!=0 && iter!=this)
    iter=iter->_father;
  return iter==this;
}

bool Node::operator<(const Node& other) const
{
  const ComposedNode *iter=_father;
  while(iter!=0 && iter!=(&other))
    iter=iter->_father;
  return iter==(&other);
}

/**
 *  @return Implementation of node: C++, Python, CORBA...
 *  _implementation is set by a derived class in a Runtime
 *  it normally applies only to elementaryNodes and it is used by Ports to select Data Converters.
 *  Potential problem with Ports attached to composed Nodes...
 */

string Node::getImplementation() const
{
  return _implementation;
}

//! Becomes deprecated soon. Replaced by ComposedNode::CheckConsistency.
set<InputPort *> Node::edGetSetOfUnitializedInputPort() const
{
  set<InputPort *> setOfUnitializedInputPort;
  list<InputPort *> allOfInputPorts=getSetOfInputPort();
  for(list<InputPort *>::const_iterator iter=allOfInputPorts.begin();iter!=allOfInputPorts.end();iter++)
    {
      if ( ! (*iter)->edIsInitialized() )
        setOfUnitializedInputPort.insert(*iter);
    }
  return setOfUnitializedInputPort;
}

//! Becomes deprecated soon. Replaced by ComposedNode::CheckConsistency.
bool Node::edAreAllInputPortInitialized() const
{
  set<InputPort *> setOfUnitializedInputPort = edGetSetOfUnitializedInputPort();
  return ( setOfUnitializedInputPort.size() == 0);
}

/*!
 * Called typically by Bloc to notify failure on potentially next nodes on the same scope of 'this'
 */
void Node::exForwardFailed()
{
  _outGate.exNotifyFailed();
}

/*!
 * Called typically by Bloc to activate potentially next nodes on the same scope of 'this'
 */
void Node::exForwardFinished()
{
  _outGate.exNotifyDone();
}

/*!
 * Called typically by ComposedNode to correctly update DF/CF/DS links
 */
void Node::edDisconnectAllLinksWithMe()
{
  _inGate.edDisconnectAllLinksToMe();
  _outGate.edDisconnectAllLinksFromMe();
}

ComposedNode *Node::getRootNode() const throw(Exception)
{
  if(!_father)
    throw Exception("No root node");
  ComposedNode *iter=_father;
  while(iter->_father)
    iter=iter->_father;
  return (ComposedNode *)iter;
}

/**
 * checks validity of ports name, that must not contain a particular character '?'
 * USAGE NOT CLEAR, not used so far, when are those characters set ?
 */

void Node::checkValidityOfPortName(const std::string& name) throw(Exception)
{
  if(name.find(SEP_CHAR_IN_PORT, 0 )!=string::npos)
    {
      string what("Port name "); what+=name; what+="not valid because it contains character "; what+=SEP_CHAR_IN_PORT;
      throw Exception(what);
    }
}

/**
 * @note : Check that 'node1' and 'node2' have exactly the same father
 * @exception : If 'node1' and 'node2' have NOT exactly the same father
 */
ComposedNode *Node::checkHavingCommonFather(Node *node1, Node *node2) throw(Exception)
{
  if(node1!=0 && node2!=0)
    {
      if(node1->_father==node2->_father)
        return node1->_father;
    }
  throw Exception("check failed : nodes have not the same father");
}

const std::string Node::getId() const
{
    std::string id=getRootNode()->getName();
    if(getRootNode() != this)
      id= id+'.'+ getRootNode()->getChildName(this);
    string::size_type debut =id.find_first_of('.');
    while(debut != std::string::npos){
        id[debut]='_';
        debut=id.find_first_of('.',debut);
    }
    return id;
}

void Node::setProperty(const std::string& name, const std::string& value)
{
    _propertyMap[name]=value;
}

//! Return the node state in the context of its father
/*!
 * \return the effective node state
 *
 * The node state is stored in a private attribute _state.
 * This state is relative to its father state : a node with a
 * TOACTIVATE state with a father node in a INITED state is not
 * to activate. Its effective state is only INITED.
 * This method returns the effective state of the node taking
 * into account that of its father.
 */
YACS::StatesForNode Node::getEffectiveState() const
{
  if(!_father)   //the root node
    return _state;
  if(_state==YACS::DISABLED)
    return YACS::DISABLED;
  return _father->getEffectiveState(this);
}

//! Return the effective state of a node in the context of this one (its father)
/*!
 * \param node: the node which effective state is queried
 * \return the effective node state
 */
YACS::StatesForNode Node::getEffectiveState(const Node* node) const
{
  if(node->getState()==YACS::DISABLED)
    return YACS::DISABLED;

  YACS::StatesForNode effectiveState=getEffectiveState();
  switch(effectiveState)
    {
    case YACS::INITED:
      return YACS::INITED;
    case YACS::TOACTIVATE:
      return YACS::INITED;
    case YACS::DISABLED:
      return YACS::DISABLED;
    case YACS::ERROR:
      return YACS::FAILED;
    default:
      return node->getState();
    }
}

//! Return the color associated to a state
/*!
 * \param state : the node state
 * \return the associated color
 */
std::string Node::getColorState(YACS::StatesForNode state) const
{
  switch(state)
    {
    case YACS::INITED:
      return "pink";
    case YACS::TOLOAD:
      return "magenta";
    case YACS::LOADED:
      return "magenta";
    case YACS::TOACTIVATE:
      return "purple";
    case YACS::ACTIVATED:
      return "blue";
    case YACS::DONE:
      return "green";
    case YACS::ERROR:
      return "red";
    case YACS::FAILED:
      return "orange";
    case YACS::DISABLED:
      return "grey";
    case YACS::PAUSE:
      return "white";
    default:
      return "white";
    }
}

//! Dump to the input stream a dot representation of the node
/*!
 *  \param os : the input stream
 */
void Node::writeDot(std::ostream &os) const
{
  os << getId() << "[fillcolor=\"" ;
  YACS::StatesForNode state=getEffectiveState();
  os << getColorState(state);
  os << "\" label=\"" << getImplementation() << "Node:" ;
  os << getQualifiedName() <<"\"];\n";
}

//! same as Node::getName() in most cases, but differs for children of switch
/*!
 *  used by writeDot to distinguish children of switch, by adding a prefix to the name.
 *  prefix is built on case id.
 */

std::string Node::getQualifiedName() const
{
  if(_father)
    return _father->getMyQualifiedName(this);
  return getName();
}

//! return node instance identifiant, unique for each node instance 
/*!
 * node instance identifiant is used to check if to nodes pointers refers to the same instance
 */ 
int Node::getNumId()
{
  return _numId;
}

//! Sets the given state for node.
/*! It is strongly recommended to use this function if you want to
 *  change the state of the node, instead of direct access to _state field (_state = ...).
 */
void Node::setState(YACS::StatesForNode theState)
{
  _state = theState;
  // emit notification to all observers registered with the dispatcher on any change of the node's state
  sendEvent("status");
}

//! emit notification to all observers registered with  the dispatcher 
/*!
 * The dispatcher is unique and can be obtained by getDispatcher()
 */
void Node::sendEvent(const std::string& event)
{
  Dispatcher* disp=Dispatcher::getDispatcher();
  disp->dispatch(this,event);
}

/*!
 *  For use only when loading a previously saved execution
 */

void YACS::ENGINE::StateLoader(Node* node, YACS::StatesForNode state)
{
  node->setState(state);
}
