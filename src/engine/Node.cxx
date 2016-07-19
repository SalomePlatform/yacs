// Copyright (C) 2006-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "Node.hxx"
#include "DynParaLoop.hxx"
#include "InputPort.hxx"
#include "OutputPort.hxx"
#include "InPropertyPort.hxx"
#include "ComposedNode.hxx"
#include "Dispatcher.hxx"
#include "InputDataStreamPort.hxx"
#include "OutputDataStreamPort.hxx"
#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

/*! \class YACS::ENGINE::Node
 *  \brief Base class for all nodes
 *
 * \ingroup Nodes
 */

const char Node::SEP_CHAR_IN_PORT[]=".";

int Node::_total = 0;
std::map<int,Node*> Node::idMap;

NodeStateNameMap::NodeStateNameMap()
{
  insert(make_pair(YACS::READY, "READY"));
  insert(make_pair(YACS::TOLOAD, "TOLOAD"));
  insert(make_pair(YACS::LOADED, "LOADED"));
  insert(make_pair(YACS::TOACTIVATE, "TOACTIVATE"));
  insert(make_pair(YACS::ACTIVATED, "ACTIVATED"));
  insert(make_pair(YACS::DESACTIVATED, "DESACTIVATED"));
  insert(make_pair(YACS::DONE, "DONE"));
  insert(make_pair(YACS::SUSPENDED, "SUSPENDED"));
  insert(make_pair(YACS::LOADFAILED, "LOADFAILED"));
  insert(make_pair(YACS::EXECFAILED, "EXECFAILED"));
  insert(make_pair(YACS::PAUSE, "PAUSE"));
  insert(make_pair(YACS::INTERNALERR, "INTERNALERR"));
  insert(make_pair(YACS::DISABLED, "DISABLED"));
  insert(make_pair(YACS::FAILED, "FAILED"));
  insert(make_pair(YACS::ERROR, "ERROR"));
}


Node::Node(const std::string& name):_name(name),_inGate(this),_outGate(this),_father(0),_state(YACS::READY),
                                    _implementation(Runtime::RUNTIME_ENGINE_INTERACTION_IMPL_NAME),_modified(1)
{
  // Should be protected by lock ??
  _numId = _total++;
  idMap[_numId]=this;

  // Every node has an InPropertyPort
  _inPropertyPort = new InPropertyPort("__InPropertyPort__Node__YACS_", this, Runtime::_tc_propvec);
}

Node::Node(const Node& other, ComposedNode *father):_inGate(this),_outGate(this),_name(other._name),_father(father),
                                                   _state(YACS::READY),_implementation(other._implementation),
                                                    _propertyMap(other._propertyMap),_modified(1)
{
  _numId = _total++;
  idMap[_numId]=this;

  // Every node has an InPropertyPort
  _inPropertyPort = new InPropertyPort("__InPropertyPort__Node__YACS_", this, Runtime::_tc_propvec);
}

Node::~Node()
{
  delete _inPropertyPort;
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
  setState(YACS::READY);
}

/*!
 * This method clones \a this by :
 *
 * - deep copying nodes, links, ports, types
 * - containers are either deep copied or shallow copied depending on _isAttachedOnCloning attribute.
 * - component are either deep copied or shallow copied depending on _isAttachedOnCloning attribute.
 *
 * So \b this \b method \b clone \b is \b dedicated \b for \b DynParaLoop \b class \b or \b subclasses.
 * It \b should \b not \b be \b used \b elsewhere, because
 * _isAttachedOnCloning attribute is an attribute in the engine not for GUI/TUI aspects.
 * For GUI/TUI manipulation cloneWithoutCompAndContDeepCpy method should be used preferably.
 *
 * \param [in] father - The new father of the returned clone.
 * \param [in] editionOnly ignored
 *
 * \sa cloneWithoutCompAndContDeepCpy
 */
Node *Node::clone(ComposedNode *father, bool editionOnly) const
{
  Node *ret(simpleClone(father,editionOnly));
  ret->performDuplicationOfPlacement(*this);
  return ret;
}

/*!
 * This method clones \a this by :
 * - deep copying nodes, links, ports, types
 * - shallow copy containers
 * - shallow copy components
 *
 * So this method simply ignores isAttachedOnCloning attribute for both containers and components.
 * So this method is dedicated for the GUI/TUI users.
 *
 * \param [in] father - The new father of the returned clone.
 * \param [in] editionOnly ignored
 */
Node *Node::cloneWithoutCompAndContDeepCpy(ComposedNode *father, bool editionOnly) const
{
  Node *ret(simpleClone(father,editionOnly));
  ret->performShallowDuplicationOfPlacement(*this);
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

list<Node *> Node::getOutNodes() const
{
  list<Node *> ret;
  list<InGate *> inGates=_outGate.edSetInGate();
  for(list<InGate *>::iterator iter=inGates.begin();iter!=inGates.end();iter++)
    ret.push_back((*iter)->getNode());
  return ret;
}

bool Node::exIsControlReady() const
{
  return _inGate.exIsReady();
}

//! Update the node state
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

InPort *Node::getInPort(const std::string& name) const throw(YACS::Exception)
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

InPropertyPort *
Node::getInPropertyPort() const throw(YACS::Exception)
{
  return _inPropertyPort;
}

InputPort *
Node::getInputPort(const std::string& name) const throw(YACS::Exception)
{
  if (name == "__InPropertyPort__Node__YACS_")
    return _inPropertyPort;
  else
  {
    std::string what("Node::getInputPort : the port with name "); what+=name; what+=" does not exist on the current level";
    throw Exception(what);
  }
}

/*!
 * \note: Contrary to getOutputPort method, this method returns the output port at highest level, possible.
 *        That is to say in some ComposedNode, like ForEachLoop or Switch, an outport inside 'this' is seen differently than the true outport.
 */
OutPort *Node::getOutPort(const std::string& name) const throw(YACS::Exception)
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
  DEBTRACE("Node::exForwardFinished");
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

Proc *Node::getProc()
{
  if(!_father)
    return 0;
  return _father->getProc();
}

const Proc * Node::getProc() const
{
  if(!_father)
    return 0;
  return _father->getProc();
}

ComposedNode *Node::getRootNode() const throw(YACS::Exception)
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

void Node::checkValidityOfPortName(const std::string& name) throw(YACS::Exception)
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
ComposedNode *Node::checkHavingCommonFather(Node *node1, Node *node2) throw(YACS::Exception)
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
    DEBTRACE("Node::setProperty " << name << " " << value);
    _propertyMap[name]=value;
}

std::string Node::getProperty(const std::string& name)
{
  std::map<std::string,std::string>::iterator it=_propertyMap.find(name);

  if(it != _propertyMap.end())
    return it->second;
  else if(_father)
    return _father->getProperty(name);
  else
    return "";
}

std::map<std::string,std::string> Node::getProperties()
{
  std::map<std::string,std::string> amap=_propertyMap;
  if(_father)
    {
      std::map<std::string,std::string> fatherMap=_father->getProperties();
      amap.insert(fatherMap.begin(),fatherMap.end());
    }

  return amap;
}

void Node::setProperties(std::map<std::string,std::string> properties)
{
  _propertyMap.clear();
  _propertyMap=properties;
}

//! Return the node state in the context of its father
/*!
 * \return the effective node state
 *
 * The node state is stored in a private attribute _state.
 * This state is relative to its father state : a node with a
 * TOACTIVATE state with a father node in a READY state is not
 * to activate. Its effective state is only READY.
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
    case YACS::READY:
      return YACS::READY;
    case YACS::TOACTIVATE:
      return YACS::READY;
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
    case YACS::READY:
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
  DEBTRACE("Node::setState: " << getName() << " " << theState);
  _state = theState;
  // emit notification to all observers registered with the dispatcher on any change of the node's state
  sendEvent("status");
}

std::vector<std::pair<std::string,int> > Node::getDPLScopeInfo(ComposedNode *gfn)
{ 
  std::vector< std::pair<std::string,int> > ret;
  Node *work2(this);
  ComposedNode *work(getFather());
  while(work!=gfn && work!=0)
    {
      DynParaLoop *workc(dynamic_cast<DynParaLoop *>(work));
      if(workc)
        {
          std::pair<std::string,int> p(gfn->getChildName(workc),workc->getBranchIDOfNode(work2));
          ret.push_back(p);
        }
      work2=work;
      work=work->getFather();
    }
  return ret;
}

/*!
 * Method called by the Executor only if the executor is sensitive of scope of DynParaLoop.
 * This method is virtual and empty because by default nothing is done.
 */
void Node::applyDPLScope(ComposedNode *gfn)
{
}

//! emit notification to all observers registered with  the dispatcher 
/*!
 * The dispatcher is unique and can be obtained by getDispatcher()
 */
void Node::sendEvent(const std::string& event)
{
  DEBTRACE("Node::sendEvent " << event);
  Dispatcher* disp=Dispatcher::getDispatcher();
  disp->dispatch(this,event);
}

//! emit notification to all observers registered with  the dispatcher 
/*!
 * The dispatcher is unique and can be obtained by getDispatcher()
 */
void Node::sendEvent2(const std::string& event, void *something)
{
  Dispatcher* disp=Dispatcher::getDispatcher();
  disp->dispatch2(this,event,something);
}

/*!
 *  For use only when loading a previously saved execution
 */

void YACS::ENGINE::StateLoader(Node* node, YACS::StatesForNode state)
{
  node->setState(state);
}

//! indicates if the node is valid (returns 1) or not (returns 0)
/*!
 * This method is useful when editing a schema. It has no meaning in execution.
 * When a node is edited, its modified method must be called so when isValid is called, its state
 * is updated (call to edUpdateState) before returning the validity check
 */
int Node::isValid()
{
  if(_modified)
    edUpdateState();
  if(_state > YACS::INVALID)
    return 1;
  else
    return 0;
}

//! update the status of the node
/*!
 * Only useful when editing a schema
 * Do nothing in base Node : to implement in derived classes
 */
void Node::edUpdateState()
{
  DEBTRACE("Node::edUpdateState(): " << _modified);
  _modified=0;
}

//! returns a string that contains an error report if the node is in error
/*!
 * 
 */
std::string Node::getErrorReport()
{
  if(getState()==YACS::DISABLED)
    return "<error node= "+getName()+ "state= DISABLED/>\n";

  YACS::StatesForNode effectiveState=getEffectiveState();

  DEBTRACE("Node::getErrorReport: " << getName() << " " << effectiveState << " " << _errorDetails);
  if(effectiveState != YACS::INVALID &&  effectiveState != YACS::ERROR && 
     effectiveState != YACS::FAILED && effectiveState != YACS::INTERNALERR)
    return "";

  std::string report="<error node= " ;
  report=report + getName() ;
  switch(effectiveState)
    {
    case YACS::INVALID:
      report=report+" state= INVALID";
      break;
    case YACS::ERROR:
      report=report+" state= ERROR";
      break;
    case YACS::FAILED:
      report=report+" state= FAILED";
      break;
    case YACS::INTERNALERR:
      report=report+" state= INTERNALERR";
      break;
    default:
      break;
    }
  report=report + ">\n" ;
  report=report+_errorDetails;
  report=report+"\n</error>";
  return report;
}

//! returns a string that contains the name of the container log file if it exists
/*!
 * Do nothing here. To subclass
 */
std::string Node::getContainerLog()
{
  return "";
}

//! Sets Node in modified state and its father if it exists
/*!
 * 
 */
void Node::modified()
{
  DEBTRACE("Node::modified() " << getName());
  _modified=1;
  if(_father)
    _father->modified();
}

//! Put this node into TOLOAD state when possible
/*!
 * 
 */
void Node::ensureLoading()
{
  if(_state == YACS::READY)
    setState(YACS::TOLOAD);
}

//! Return the name of a state
/*!
 * 
 */
std::string Node::getStateName(YACS::StatesForNode state)
{
  static NodeStateNameMap nodeStateNameMap;
  return nodeStateNameMap[state];
}

//! Stop all pending activities of the node
/*!
 * This method should be called when a Proc is finished and must be deleted from the YACS server
 */
void Node::shutdown(int level)
{
  if(level==0)return;
}

//! Clean the node in case of not clean exit
/*!
 * This method should be called on a control-C or sigterm
 */
void Node::cleanNodes()
{
}

//! Reset the node state depending on the parameter level
void Node::resetState(int level)
{
  DEBTRACE("Node::resetState " << getName() << "," << level << "," << _state);
  if(_state==YACS::ERROR || _state==YACS::FAILED || _state==YACS::ACTIVATED)
    {
      setState(YACS::READY);
      InGate* inGate = getInGate();
      std::list<OutGate*> backlinks = inGate->getBackLinks();
      for (std::list<OutGate*>::iterator io = backlinks.begin(); io != backlinks.end(); io++)
        {
          Node* fromNode = (*io)->getNode();
          if(fromNode->getState() == YACS::DONE)
            {
              inGate->setPrecursorDone(*io);
            }
        }
    }
}
