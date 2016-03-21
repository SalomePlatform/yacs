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

#include "Switch.hxx"
#include "Visitor.hxx"
#include "LinkInfo.hxx"

#include <iostream>
#include <sstream>
#include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

const char Switch::DEFAULT_NODE_NAME[]="default";
const int Switch::ID_FOR_DEFAULT_NODE=-1973012217;
const char Switch::SELECTOR_INPUTPORT_NAME[]="select";

int CollectorSwOutPort::edGetNumberOfOutLinks() const
{
  return 1;
}

std::set<InPort *> CollectorSwOutPort::edSetInPort() const
{
  set<InPort *> ret;
  if(_consumer)
    ret.insert(_consumer);
  return ret;
}

bool CollectorSwOutPort::isAlreadyLinkedWith(InPort *with) const
{
  set<InPort *> s;
  with->getAllRepresentants(s);
  return s.find(_consumer)!=s.end();
}

std::string CollectorSwOutPort::getNameOfTypeOfCurrentInstance() const
{
  return _className;
}

void CollectorSwOutPort::edRemoveAllLinksLinkedWithMe() throw(YACS::Exception)
{
  map<int, OutPort *>::iterator pt;
  if(_consumer)
    for(pt=_potentialProducers.begin();pt!=_potentialProducers.end();pt++)
      ((*pt).second)->removeInPort(_consumer,true);
}

TypeOfChannel CollectorSwOutPort::getTypeOfChannel() const
{
  return (*(_potentialProducers.begin())).second->getTypeOfChannel();
}

void CollectorSwOutPort::getAllRepresented(std::set<OutPort *>& represented) const
{
  map<int, OutPort *>::const_iterator pt;
  for(pt=_potentialProducers.begin();pt!=_potentialProducers.end();pt++)
    ((*pt).second)->getAllRepresented(represented);
}

bool CollectorSwOutPort::addInPort(InPort *inPort) throw(YACS::Exception)
{
  if(_currentProducer)
    {//a specific link is beeing done
      bool ret=_currentProducer->addInPort(inPort);
      _currentProducer=0;
      return ret;
    }
  else//global links asked
    for(map<int, OutPort *>::iterator iter=_potentialProducers.begin();iter!=_potentialProducers.end();iter++)
      (*iter).second->addInPort(inPort);
}

int CollectorSwOutPort::removeInPort(InPort *inPort, bool forward) throw(YACS::Exception)
{
  if(_currentProducer)
    {
      return _currentProducer->removeInPort(inPort,forward);
    }
  else
    throw Exception("CollectorSwOutputPort::edRemoveInputPort : internal error on link removal.");
  _currentProducer=0;
}

/*!
 * \note : 'master' specifies the instance of Switch of which 'this' collects all of these direct 
 *          or indirect outports going to the same port 'port' (which is out of scope of 'master').
 */
CollectorSwOutPort::CollectorSwOutPort(Switch *master, InPort *port):OutPort("",master,port->edGetType()),
                                                                     DataPort("",master,port->edGetType()),
                                                                     Port(master),
                                                                     _consumer(port),_currentProducer(0)
{
  _name="Representant_of_"; _name+=master->getName(); _name+="_for_inport_"; _name+=master->getRootNode()->getInPortName(_consumer);
}

CollectorSwOutPort::CollectorSwOutPort(const CollectorSwOutPort& other, Switch *master):OutPort("",master,other.edGetType()),
                                                                                        DataPort("",master,other.edGetType()),
                                                                                        Port(master),
                                                                                        _consumer(0),_currentProducer(0)
{
  _name=other._name;
  Switch *othSw=(Switch *)other._node;
  for(map<int, OutPort *>::const_iterator iter=other._potentialProducers.begin();iter!=other._potentialProducers.end();iter++)
    {
      string name=othSw->getOutPortName((*iter).second);
      _potentialProducers[(*iter).first]=master->getOutPort(name);
    }
}

void CollectorSwOutPort::addPotentialProducerForMaster(OutPort *port)
{
  int i=((Switch *)_node)->getRankOfNode(port->getNode());
  map<int, OutPort *>::iterator pt=_potentialProducers.find(i);
  if(pt==_potentialProducers.end())
    {
      _potentialProducers[i]=port;
      _currentProducer=port;
    }
  else
    {
      _currentProducer=(*pt).second;
      if(_currentProducer!=port)
        {
          string what("CollectorSwOutPort::addPotentialProducerForMaster : In switch node "); what+=_node->getName();
          what+=" for input named \'"; what+=_consumer->getName(); what+="\' the output "; what+=_currentProducer->getName();
          what+=" already got out for case of label "; 
          what+=Switch::getRepresentationOfCase((*pt).first); 
          throw Exception(what);
        }
    }
  _className=port->getNameOfTypeOfCurrentInstance();
}

bool CollectorSwOutPort::removePotentialProducerForMaster()
{
  int i;
  map<int, OutPort *>::iterator result;
  for(result=_potentialProducers.begin();result!=_potentialProducers.end();result++)
    if((*result).second==_currentProducer)
      {
        i=(*result).first;
        break;
      }
  if(result==_potentialProducers.end())
    {
      ostringstream stream; stream << "CollectorSwOutPort::removePotentialProducerForMaster : link from the branch whith id ";
      stream << i << " not defined";
      throw Exception(stream.str());
    }
  if((*result).second!=_currentProducer)
    {
      ostringstream stream; stream << "CollectorSwOutPort::removePotentialProducerForMaster : link from the branch whith id ";
      stream << i << " defined but the output specified is not compatible";
      throw Exception(stream.str());
    }
  _potentialProducers.erase(result);
  return _potentialProducers.empty();
}

bool CollectorSwOutPort::checkManagementOfPort(OutPort *port) throw(YACS::Exception)
{
  for(map<int, OutPort *>::iterator iter=_potentialProducers.begin();iter!=_potentialProducers.end();iter++)
    if((*iter).second==port)
      {
        _currentProducer=port;
        return _potentialProducers.size()==1;
      }
  throw Exception("CollectorSwOutPort::checkManagementOfPort : unexported port");
}

/*!
 * Called by Switch::checkCFLinks.
 */
void CollectorSwOutPort::checkConsistency(LinkInfo& info) const
{
  if(((Switch *)_node)->getNbOfCases()!=_potentialProducers.size())
    info.pushErrSwitch((CollectorSwOutPort *)this);
  for(map<int, OutPort *>::const_iterator iter=_potentialProducers.begin();iter!=_potentialProducers.end();iter++)
    (*iter).second->checkConsistency(info);
}

/*!
 * Called by LinkInfo::getErrRepr to have a comprehensible message on throw.
 * When called, typically checkCompletenessOfCases has detected that some potential producers were laking...
 */
void CollectorSwOutPort::getHumanReprOfIncompleteCases(std::ostream& stream) const
{
  set<int> lackingCases;
  for(map< int ,Node * >::const_iterator iter=((Switch *)_node)->_mapOfNode.begin();iter!=((Switch *)_node)->_mapOfNode.end();iter++)
    {
      if(_potentialProducers.find((*iter).first)==_potentialProducers.end())
        lackingCases.insert((*iter).first);
    }
  ostringstream streamForExc;
  stream << "For link to " <<  _consumer->getName() << " of node " << _consumer->getNode()->getName() 
         << " the cases of switch node named " << _node->getName() 
         << " do not define links for following cases ids :";
  for(set<int>::iterator iter=lackingCases.begin();iter!=lackingCases.end();iter++)
      stream << Switch::getRepresentationOfCase(*iter) << " ";
  stream << endl;
}

FakeNodeForSwitch::FakeNodeForSwitch(Switch *sw, bool normalFinish, bool internalError):ElementaryNode("thisIsAFakeNode"),
                                                                                        _sw(sw),
                                                                                        _normalFinish(normalFinish),
                                                                                        _internalError(internalError)
{
  _state=YACS::TOACTIVATE;
  _father=_sw->getFather();
}

FakeNodeForSwitch::FakeNodeForSwitch(const FakeNodeForSwitch& other):ElementaryNode(other),_sw(0),
                                                                     _normalFinish(false),
                                                                     _internalError(true)
{
}

Node *FakeNodeForSwitch::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new FakeNodeForSwitch(*this);
}

void FakeNodeForSwitch::exForwardFailed()
{
  _sw->exForwardFailed();
}

void FakeNodeForSwitch::exForwardFinished()
{ 
  _sw->exForwardFinished(); 
}

void FakeNodeForSwitch::execute()
{
  if(!_normalFinish)
    throw Exception("");//only to trigger ABORT on Executor
}

void FakeNodeForSwitch::aborted()
{
  if(_internalError)
    _sw->_state!=YACS::INTERNALERR;
  else
    _sw->setState(YACS::ERROR);
}

void FakeNodeForSwitch::finished()
{
  _sw->setState(YACS::DONE);
}

/*! \class YACS::ENGINE::Switch
 *  \brief Control node that emulates the C switch
 *
 *  \ingroup Nodes
 */

Switch::Switch(const Switch& other, ComposedNode *father, bool editionOnly):StaticDefinedComposedNode(other,father),_condition(other._condition,this),
                                                                            _undispatchableNotificationNode(0)
{
  for(map<int,Node *>::const_iterator iter=other._mapOfNode.begin();iter!=other._mapOfNode.end();iter++)
    _mapOfNode[(*iter).first]=(*iter).second->clone(this,editionOnly);
  if(!editionOnly)
    for(map<InPort *, CollectorSwOutPort * >::const_iterator iter2=other._outPortsCollector.begin();iter2!=other._outPortsCollector.end();iter2++)
      {
        CollectorSwOutPort *newCol=new CollectorSwOutPort(*((*iter2).second),this);
        _alreadyExistingCollectors.push_back(newCol);
      }
}

Switch::Switch(const std::string& name):StaticDefinedComposedNode(name),_condition(SELECTOR_INPUTPORT_NAME,this,Runtime::_tc_int),_undispatchableNotificationNode(0)
{
}

Switch::~Switch()
{
  if(_undispatchableNotificationNode)delete _undispatchableNotificationNode;

  for(map< int , Node * >::iterator iter=_mapOfNode.begin();iter!=_mapOfNode.end();iter++)
    delete (*iter).second;
  for(map<InPort *, CollectorSwOutPort * >::iterator iter2=_outPortsCollector.begin();iter2!=_outPortsCollector.end();iter2++)
    delete (*iter2).second;
  for(vector<CollectorSwOutPort *>::iterator iter3=_alreadyExistingCollectors.begin();iter3!=_alreadyExistingCollectors.end();iter3++)
    delete (*iter3);
}

Node *Switch::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new Switch(*this,father,editionOnly);
}

void Switch::exUpdateState()
{
  DEBTRACE("Switch::exUpdateState " << _state);
  if(_state == YACS::DISABLED)
    return;
  if(_inGate.exIsReady())
    {
      setState(YACS::ACTIVATED);
      if(_condition.isEmpty())
        _undispatchableNotificationNode=new FakeNodeForSwitch(this,false,true);
      else
        {
          map< int , Node * >::iterator iter=_mapOfNode.find(_condition.getIntValue());
          if(iter==_mapOfNode.end())
            {
              iter=_mapOfNode.find(ID_FOR_DEFAULT_NODE);
              if(iter==_mapOfNode.end())
                {
                  bool normalFinish=getAllOutPortsLeavingCurrentScope().empty();
                  delete _undispatchableNotificationNode;
                  _undispatchableNotificationNode=new FakeNodeForSwitch(this,normalFinish);
                }
              else
                ((*iter).second)->exUpdateState();
            }
          else
            ((*iter).second)->exUpdateState();
        }
    }
}

void Switch::init(bool start)
{
  DEBTRACE("Switch::init " << start);
  StaticDefinedComposedNode::init(start);
  int i=0;
  for(map< int , Node * >::iterator iter=_mapOfNode.begin();iter!=_mapOfNode.end();iter++, i++)
    {
      if(!(*iter).second)
        {
          ostringstream stream;
          stream << "Switch::init : initialization failed due to unitialized branch of id " << (*iter).first;
          throw Exception(stream.str());
        }
      ((*iter).second)->init(start);
    }
}

void Switch::getReadyTasks(std::vector<Task *>& tasks)
{
  /*
   * To change the way ComposedNode state is handled, uncomment the following line
   * see Bloc::getReadyTasks
   */
  if(_state==YACS::TOACTIVATE) setState(YACS::ACTIVATED);
  if(_state==YACS::TOACTIVATE || _state==YACS::ACTIVATED)
    {
      map< int , Node * >::iterator iter=_mapOfNode.find(_condition.getIntValue());
      if(iter!=_mapOfNode.end())
        ((*iter).second)->getReadyTasks(tasks);
      else
        {
          iter=_mapOfNode.find(ID_FOR_DEFAULT_NODE);
          if(iter!=_mapOfNode.end())
            (*iter).second->getReadyTasks(tasks);//Default Node is returned
          else
            if(_undispatchableNotificationNode)
              _undispatchableNotificationNode->getReadyTasks(tasks);
            else
              throw Exception("Switch::getReadyTasks : internal error");
        }
    }
}

list<Node *> Switch::edGetDirectDescendants() const
{
  list<Node *> ret;
  for(map< int , Node * >::const_iterator iter=_mapOfNode.begin();iter!=_mapOfNode.end();iter++)
    if((*iter).second)
      ret.push_back((*iter).second);
  return ret;
}

int Switch::getNumberOfInputPorts() const
{ 
  return StaticDefinedComposedNode::getNumberOfInputPorts()+1;
}

int Switch::getMaxLevelOfParallelism() const
{
  int ret(0);
  for(std::map< int , Node * >::const_iterator it=_mapOfNode.begin();it!=_mapOfNode.end();it++)
    ret=std::max(ret,((*it).second)->getMaxLevelOfParallelism());
  return ret;
}

void Switch::edRemoveChild(Node *node) throw(YACS::Exception)
{
  map< int , Node * >::iterator iter=_mapOfNode.begin();
  for(;iter!=_mapOfNode.end();iter++)
    if(node==(*iter).second)
      {
        edReleaseCase((*iter).first);
        return;
      }
  ostringstream what; what << "Switch::edRemoveChild : node with name " << node->getName() << " is not a direct child of Switch node " << _name; 
  throw Exception(what.str());
}

std::list<InputPort *> Switch::getSetOfInputPort() const
{
  list<InputPort *> ret=StaticDefinedComposedNode::getSetOfInputPort();
  ret.push_back((InputPort *)&_condition);
  return ret;
}


std::list<InputPort *> Switch::getLocalInputPorts() const
{
  list<InputPort *> ret=StaticDefinedComposedNode::getLocalInputPorts();
  ret.push_back((InputPort *)&_condition);
  return ret;
}
OutPort *Switch::getOutPort(const std::string& name) const throw(YACS::Exception)
{
  for(map<InPort *, CollectorSwOutPort * >::const_iterator iter=_outPortsCollector.begin();iter!=_outPortsCollector.end();iter++)
    if(name==(*iter).second->getName())
      return (*iter).second;
  for(vector<CollectorSwOutPort *>::const_iterator iter2=_alreadyExistingCollectors.begin();iter2!=_alreadyExistingCollectors.end();iter2++)
    if(name==(*iter2)->getName())
      return *iter2;
  return StaticDefinedComposedNode::getOutPort(name);
}

InputPort *Switch::getInputPort(const std::string& name) const throw(YACS::Exception)
{
  if(name==SELECTOR_INPUTPORT_NAME)
    return (InputPort *)&_condition;
  return StaticDefinedComposedNode::getInputPort(name);
}

Node *Switch::getChildByShortName(const std::string& name) const throw(YACS::Exception)
{
  if(name==DEFAULT_NODE_NAME)
    {
      map< int , Node * >::const_iterator iter=_mapOfNode.find(ID_FOR_DEFAULT_NODE);
      if(iter!=_mapOfNode.end())
        return (Node *)((*iter).second);
      else
        {
          string what("Switch::getChildByShortName : no default node defined for switch of name "); what+=getName();
          throw Exception(what);
        }
    }
  for(map< int , Node * >::const_iterator iter=_mapOfNode.begin();iter!=_mapOfNode.end();iter++)
    {
      if(name==((*iter).second)->getQualifiedName())
        return (*iter).second;
    }
  string what("node "); what+= name ; what+=" is not a child of node switch "; what += getName();
  throw Exception(what);
}

Node *Switch::edSetDefaultNode(Node *node)
{
  return edSetNode(ID_FOR_DEFAULT_NODE,node);
}

Node *Switch::edReleaseDefaultNode() throw(YACS::Exception)
{
  return edReleaseCase(ID_FOR_DEFAULT_NODE);
}

Node *Switch::edReleaseCase(int caseId) throw(YACS::Exception)
{
  map< int , Node * >::iterator iter=_mapOfNode.find(caseId);
  if(iter==_mapOfNode.end())
    { 
      string what("Switch::edReleaseCase : the case # "); what+=getRepresentationOfCase(caseId);  what+=" is not set yet.";
      throw Exception(what);
    }
  else
    {
      Node *ret=(*iter).second;
      StaticDefinedComposedNode::edRemoveChild(ret);
      _mapOfNode.erase(iter);
      modified();
      return ret;
    }
}

Node *Switch::edGetNode(int caseId)
{
  if (!_mapOfNode.count(caseId)) return 0;
  return _mapOfNode[caseId];
}


/*!
 * \param caseId : the case ID chosen to place 'node'
 * \param node   : the node for the specified 'caseId'
 * \return : If an old node with id equal to 'caseId' exists before, this old node is returned so that to be deallocated.
 *           0 is returned if caseId is a new ID.
 *  \b WARNING : 'node' is held by 'this' after call, whereas returned node is no more held. 
 */
Node *Switch::edSetNode(int caseId, Node *node) throw(YACS::Exception)
{
  if(!node)
    throw Exception("Switch::edSetNode : null node cannot be set as a case in switch node");
  if(node->_father!=0)
    throw Exception("Switch::edSetNode : node already held by another father");
  checkNoCrossHierachyWith(node);
  node->_father=this;
  map< int , Node * >::iterator iter=_mapOfNode.find(caseId);
  if(iter==_mapOfNode.end())
    {
      _mapOfNode[caseId]=node;
      modified();
      return 0;
    }
  else
    {
      if(node!=(*iter).second)
        {
          Node *ret=(*iter).second;
          (*iter).second=node;
          modified();
          return ret;
        }
    }
}

//! Change the case of a node
/*!
 *  \param oldCase : the case value to change
 *  \param newCase : the new value to set
 *  raise an exception if the old case does not exist or if the new case already exists
 */
void Switch::edChangeCase(int oldCase, int newCase)
{
  std::map< int , Node * >::iterator iter=_mapOfNode.find(oldCase);
  if(iter==_mapOfNode.end())
    {
      //the case does not exists
      throw Exception("Switch::edChangeCase : case does not exist");
    }
  iter=_mapOfNode.find(newCase);
  if(iter != _mapOfNode.end())
    {
      //the new case exists
      throw Exception("Switch::edChangeCase : new case exists");
    }
  Node* node=_mapOfNode[oldCase];
  _mapOfNode.erase(oldCase);
  _mapOfNode[newCase]=node;
  modified();
}

int Switch::getMaxCase()
{
  int aCase = 0;
  map<int, Node*>::const_iterator it = _mapOfNode.begin();
  for(; it != _mapOfNode.end(); ++it)
    if ((*it).first > aCase)
      aCase = (*it).first;
  return aCase;
}

//! Get the progress weight of the graph
/*!
 * Only elementary nodes have weight. If the switch node is not done, we add the weight of all his descendants,
 * otherwise only the weight of the used case count.
 */
list<ProgressWeight> Switch::getProgressWeight() const
{
  list<ProgressWeight> ret;
  list<Node *> setOfNode=edGetDirectDescendants();
  if (getState() == YACS::DONE)
    {
      for(list<Node *>::const_iterator iter=setOfNode.begin();iter!=setOfNode.end();iter++)
      {
        if (getEffectiveState(*iter) == YACS::DONE)
          ret=(*iter)->getProgressWeight();
      }
    }
  else
    {
      for(list<Node *>::const_iterator iter=setOfNode.begin();iter!=setOfNode.end();iter++)
        {
          list<ProgressWeight> myCurrentSet=(*iter)->getProgressWeight();
          ret.insert(ret.end(),myCurrentSet.begin(),myCurrentSet.end());
        }
    }
  return ret;
}

bool Switch::edAddChild(Node *node) throw(YACS::Exception)
{
  int aCase = getMaxCase() + 1;
  DEBTRACE(aCase);
  bool ret = edSetNode(aCase, node);
  DEBTRACE(ret);
  return ret;
}

YACS::Event Switch::updateStateOnFinishedEventFrom(Node *node)
{
  setState(YACS::DONE);
  return YACS::FINISH;//notify to father node that 'this' has becomed finished.
}

std::set<InPort *> Switch::getAllInPortsComingFromOutsideOfCurrentScope() const
{
  set<InPort *> ret=StaticDefinedComposedNode::getAllInPortsComingFromOutsideOfCurrentScope();
  set<OutPort *> temp2=_condition.edSetOutPort();
  for(set<OutPort *>::iterator iter3=temp2.begin();iter3!=temp2.end();iter3++)
    if(!isInMyDescendance((*iter3)->getNode()))
      {
        ret.insert((InPort *)&_condition);
        break;
      }
  return ret;
}

void Switch::checkCFLinks(const std::list<OutPort *>& starts, InputPort *end, unsigned char& alreadyFed, bool direction, LinkInfo& info) const
{
  map<InPort *, CollectorSwOutPort * >::const_iterator iter=_outPortsCollector.find(end);
  if(iter!=_outPortsCollector.end())
    {
      set<OutPort *> represented;
      (*iter).second->getAllRepresented(represented);
      list<OutPort *> others;
      for(list<OutPort *>::const_iterator iter2=starts.begin();iter2!=starts.end();iter2++)
        if(represented.find(*iter2)==represented.end())
          others.push_back(*iter2);
      if(others.empty())
        alreadyFed=FED_ST;
      else
        StaticDefinedComposedNode::checkCFLinks(others,end,alreadyFed,direction,info);//should never happend;
    }
  else
    StaticDefinedComposedNode::checkCFLinks(starts,end,alreadyFed,direction,info);
}

void Switch::checkControlDependancy(OutPort *start, InPort *end, bool cross,
                                    std::map < ComposedNode *,  std::list < OutPort * > >& fw,
                                    std::vector<OutPort *>& fwCross,
                                    std::map< ComposedNode *, std::list < OutPort *> >& bw,
                                    LinkInfo& info) const
{
  throw Exception("Switch::checkControlDependancy : a link was dectected between 2 cases of a switch. Impossible !");
}

void Switch::checkNoCyclePassingThrough(Node *node) throw(YACS::Exception)
{
  throw Exception("Switch::checkNoCyclePassingThrough : uncorrect control flow link relative to switch");
}

void Switch::checkLinkPossibility(OutPort *start, const std::list<ComposedNode *>& pointsOfViewStart,
                                  InPort *end, const std::list<ComposedNode *>& pointsOfViewEnd) throw(YACS::Exception)
{
  throw Exception("Switch::checkLinkPossibility : A link between 2 different cases of a same Switch requested -> Impossible");
}

void Switch::buildDelegateOf(std::pair<OutPort *, OutPort *>& port, InPort *finalTarget, const std::list<ComposedNode *>& pointsOfView)
{
  map<InPort *, CollectorSwOutPort * >::iterator result=_outPortsCollector.find(finalTarget);
  CollectorSwOutPort *newCollector;
  if(result!=_outPortsCollector.end())
    newCollector=(*result).second;
  else
    {
      newCollector=new CollectorSwOutPort(this,finalTarget);
      newCollector->edSetType((port.first)->edGetType());
      _outPortsCollector[finalTarget]=newCollector;
    }
  newCollector->addPotentialProducerForMaster(port.first);
  port.second=newCollector;
  port.first=newCollector;
}

void Switch::getDelegateOf(std::pair<OutPort *, OutPort *>& port, InPort *finalTarget, const std::list<ComposedNode *>& pointsOfView) throw(YACS::Exception)
{
  map<InPort *, CollectorSwOutPort * >::iterator iter=_outPortsCollector.find(finalTarget);
  if(iter==_outPortsCollector.end())
    {
      string what("Switch::getDelegateOf : not exported OuputPort with name "); what+=(port.first)->getName(); what+=" for target inport of name ";
      what+=finalTarget->getName();
      throw Exception(what);
    }
  ((*iter).second)->checkManagementOfPort(port.first);
  port.second=(*iter).second;
  port.first=(*iter).second;
}

void Switch::releaseDelegateOf(OutPort *portDwn, OutPort *portUp, InPort *finalTarget, const std::list<ComposedNode *>& pointsOfView) throw(YACS::Exception)
{
  set<OutPort *> repr;
  portDwn->getAllRepresented(repr);
  if(repr.size()==1)
    {
      CollectorSwOutPort *portCasted=dynamic_cast<CollectorSwOutPort *>(portUp);
      if(portCasted->removePotentialProducerForMaster())//normally always true
        {
          delete portCasted;
          _outPortsCollector.erase(finalTarget);
        }
    }
}

int Switch::getNbOfCases() const
{
  return _mapOfNode.size();
}

int Switch::getRankOfNode(Node *node) const
{
  Node *directSon=isInMyDescendance(node);
  for(map< int , Node * >::const_iterator iter=_mapOfNode.begin();iter!=_mapOfNode.end();iter++)
    if((*iter).second==directSon)
      return (*iter).first;
  throw Exception("Switch::getRankOfNode : node not in switch");
}

string Switch::getRepresentationOfCase(int i)
{
  if(i!=ID_FOR_DEFAULT_NODE)
    {
      ostringstream stream;
      stream << i;
      return stream.str();
    }
  else
    return DEFAULT_NODE_NAME;
}

//! Return the effective state of a node in the context of this switch (its father)
/*!
 * \param node: the node which effective state is queried
 * \return the effective node state
 */
YACS::StatesForNode Switch::getEffectiveState(const Node* node) const
{
  YACS::StatesForNode effectiveState=Node::getEffectiveState();
  if(effectiveState==YACS::READY)
    return YACS::READY;
  if(effectiveState==YACS::TOACTIVATE)
    return YACS::READY;
  if(effectiveState==YACS::DISABLED)
    return YACS::DISABLED;

  return node->getState();
}
YACS::StatesForNode Switch::getEffectiveState() const
{
  return Node::getEffectiveState();
}

void Switch::writeDot(std::ostream &os) const
{
  os << "  subgraph cluster_" << getId() << "  {\n" ;
  for(map<int,Node*>::const_iterator iter=_mapOfNode.begin();iter!=_mapOfNode.end();iter++)
    {
      Node* n=(*iter).second;
      n->writeDot(os);
      os << getId() << " -> " << n->getId() << ";\n";
    }
  os << "}\n" ;
  os << getId() << "[fillcolor=\"" ;
  YACS::StatesForNode state=Node::getEffectiveState();
  os << getColorState(state);
  os << "\" label=\"" << "Switch:" ;
  os << getQualifiedName() <<"\"];\n";
}

std::string Switch::getMyQualifiedName(const Node *directSon) const
{
  string id=getCaseId(directSon);
  id+=directSon->getName();
  return id;
}

std::string Switch::getCaseId(const Node *node) const throw(YACS::Exception)
{
  const char sep='_';
  map<int, Node*>::const_iterator iter;
  for (iter = _mapOfNode.begin(); iter != _mapOfNode.end(); iter++)
      if (iter->second == node)
        {
          stringstream a;
          if (iter->first == Switch::ID_FOR_DEFAULT_NODE)
            a << DEFAULT_NODE_NAME << sep;
          else if (iter->first <0)
            a << "m" << -iter->first << sep;
          else a  << "p" << iter->first << sep; 
          return a.str();
        }
  string what("node "); what+= node->getName() ; what+=" is not a child of node "; what += getName();
  throw Exception(what);  
}

void Switch::accept(Visitor *visitor)
{
  visitor->visitSwitch(this);
}
