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

#include "Loop.hxx"
#include "InputPort.hxx"
#include "OutputPort.hxx"
#include "InputDataStreamPort.hxx"
#include "OutputDataStreamPort.hxx"
#include "LinkInfo.hxx"
#include "Runtime.hxx"
#include "Visitor.hxx"
#include <cassert>
#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

InputPort4DF2DS::InputPort4DF2DS(DFToDSForLoop *node, TypeCode* type):
  InputPort("", node, type),
  DataPort("", node, type),
  Port(node),_data(0)
{
}

void InputPort4DF2DS::getAllRepresentants(std::set<InPort *>& repr) const
{
  set<InPort *> s=_node->getOutputDataStreamPort("")->edSetInPort();
  repr.insert(s.begin(),s.end());
}

void *InputPort4DF2DS::get() const throw(YACS::Exception)
{
  if(!_data)
    {
      std::string what="InputPort4DF2DS::get : no value currently in input whith name \""; what+=_name; what+="\"";
      throw Exception(what);
    }
  return (void *)_data;
}

void InputPort4DF2DS::exRestoreInit()
{
  if(!_initValue)
    return;
  if(_data)
    _data->decrRef();
  _data=_initValue;
  _data->incrRef();
}

void InputPort4DF2DS::exSaveInit()
{
  if(_initValue)
    _initValue->decrRef();
  _initValue=_data;
  _initValue->incrRef();
}

void InputPort4DF2DS::put(const void *data) throw(ConversionException)
{
  put((Any *)data);
}

InputPort *InputPort4DF2DS::clone(Node *newHelder) const
{
  throw Exception("InputPort4DF2DS::clone : internal error");
}

void InputPort4DF2DS::put(Any *data)
{
  if(_data)
    _data->decrRef();
  _data=data;
  _data->incrRef();
}

InputPort4DF2DS::~InputPort4DF2DS()
{
  if(_data)
    _data->decrRef();
}

DFToDSForLoop::DFToDSForLoop(Loop *loop, const std::string& name, TypeCode* type):ElementaryNode(""),_nbOfTimeUsed(1)
{
  _name="DF2DS For "; _name+=loop->getName(); _name+=" representing port "; _name+=name;
  _father=loop;
  _setOfInputPort.push_back(new InputPort4DF2DS(this,type));
  _setOfOutputDataStreamPort.push_back(new OutputDataStreamPort("",this,Loop::MappingDF2DS(type)));
}

DFToDSForLoop::~DFToDSForLoop()
{
  edDisconnectAllLinksWithMe();
}

void DFToDSForLoop::getReadyTasks(std::vector<Task *>& tasks)
{
}

InputPort *DFToDSForLoop::getInputPort(const std::string& name) const throw(YACS::Exception)
{
  try {
    return Node::getInputPort(name);
  }
  catch(Exception& e) {}

  list<InputPort *>::const_iterator it =_setOfInputPort.begin();
  return (*it);
}

OutputDataStreamPort *DFToDSForLoop::getOutputDataStreamPort(const std::string& name) const throw(YACS::Exception)
{
  list<OutputDataStreamPort *>::const_iterator it =_setOfOutputDataStreamPort.begin();
  return (*it);
}

void DFToDSForLoop::load()
{
}

void DFToDSForLoop::execute()
{
  //TO IMPLEMENT
}

Node *DFToDSForLoop::simpleClone(ComposedNode *father, bool editionOnly) const
{
  throw Exception("DFToDSForLoop::simpleClone : Internal error");
}

OutputPort4DS2DF::OutputPort4DS2DF(DSToDFForLoop *node, TypeCode *type):
  OutputPort("", node, type),
  DataPort("", node, type),
  Port(node),_data(0)
{
}

void OutputPort4DS2DF::getAllRepresented(std::set<OutPort *>& represented) const
{
  set<OutPort *> setO=_node->getInputDataStreamPort("")->edSetOutPort();
  for(set<OutPort *>::iterator iter=setO.begin();iter!=setO.end();iter++)
    (*iter)->getAllRepresented(represented);
}

void OutputPort4DS2DF::put(const void *data) throw(ConversionException)
{
  put((Any *)data);
  OutputPort::put(data);
}

OutputPort *OutputPort4DS2DF::clone(Node *newHelder) const
{
  throw Exception("OutputPort4DS2DF::clone : Internal error");
}

void OutputPort4DS2DF::put(Any *data)
{
  if(_data)
    _data->decrRef();
  _data=data;
  _data->incrRef();
}

OutputPort4DS2DF::~OutputPort4DS2DF()
{
  if(_data)
    _data->decrRef();
}

InputDataStreamPort4DS2DF::InputDataStreamPort4DS2DF(DSToDFForLoop *node, TypeCode* type):
  InputDataStreamPort("", node, type),
  DataPort("", node, type),
  Port(node)
{
}

void InputDataStreamPort4DS2DF::getAllRepresentants(std::set<InPort *>& repr) const
{
  set<InPort *> s=_node->getOutputPort("")->edSetInPort();
  repr.insert(s.begin(),s.end());
}

DSToDFForLoop::DSToDFForLoop(Loop *loop, const std::string& name, TypeCode* type):ElementaryNode(""),_nbOfTimeUsed(1)
{
  _name="DS2DF For "; _name+=loop->getName(); _name+=" representing port "; _name+=name;
  _father=loop;
  _setOfOutputPort.push_back(new OutputPort4DS2DF(this,type));
  _setOfInputDataStreamPort.push_back(new InputDataStreamPort4DS2DF(this,type));
}

Node *DSToDFForLoop::simpleClone(ComposedNode *father, bool editionOnly) const
{
  throw Exception("DSToDFForLoop::simpleClone : Internal error");
}

DSToDFForLoop::~DSToDFForLoop()
{
  edDisconnectAllLinksWithMe();
}

void DSToDFForLoop::getReadyTasks(std::vector<Task *>& tasks)
{
}

OutputPort *DSToDFForLoop::getOutputPort(const std::string& name) const throw(YACS::Exception)
{
  list<OutputPort *>::const_iterator it = _setOfOutputPort.begin();
  return (*it);
}

InputDataStreamPort *DSToDFForLoop::getInputDataStreamPort(const std::string& name) const throw(YACS::Exception)
{
  list<InputDataStreamPort *>::const_iterator it = _setOfInputDataStreamPort.begin();
  return (*it);
}

void DSToDFForLoop::load()
{
}

void DSToDFForLoop::execute()
{
  //TO IMPLEMENT
}

FakeNodeForLoop::FakeNodeForLoop(Loop *loop, bool normalFinish, bool internalError):ElementaryNode("thisIsAFakeNode"),
                                                                                    _loop(loop),
                                                                                    _normalFinish(normalFinish),
                                                                                    _internalError(internalError)
{
  setState(YACS::TOACTIVATE);
  _father=_loop->getFather();
}

FakeNodeForLoop::FakeNodeForLoop(const FakeNodeForLoop& other):ElementaryNode(other),_loop(0),
                                                               _normalFinish(false),_internalError(true)
{
}

Node *FakeNodeForLoop::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new FakeNodeForLoop(*this);
}

void FakeNodeForLoop::exForwardFailed()
{
  _loop->exForwardFailed();
}

void FakeNodeForLoop::exForwardFinished()
{ 
  _loop->exForwardFinished();
}

void FakeNodeForLoop::execute()
{
  if(!_normalFinish)
    throw Exception("");//only to trigger ABORT on Executor
}

void FakeNodeForLoop::aborted()
{
  if(_internalError)
    _loop->setState(YACS::INTERNALERR);
  else
    _loop->setState(YACS::ERROR);
}

void FakeNodeForLoop::finished()
{
  _loop->setState(YACS::DONE);
}

Loop::Loop(const Loop& other, ComposedNode *father, bool editionOnly):StaticDefinedComposedNode(other,father),_nbOfTurns(0),_nodeForNullTurnOfLoops(0),_node(0)
{
  if(other._node)
    _node=other._node->simpleClone(this,editionOnly);
}

Loop::Loop(const std::string& name):StaticDefinedComposedNode(name),_node(0),_nbOfTurns(0),_nodeForNullTurnOfLoops(0)
{
}

Loop::~Loop()
{
  delete _node;
  if(_nodeForNullTurnOfLoops)delete _nodeForNullTurnOfLoops;
  for(set<DSToDFForLoop *>::iterator iter1=_inputsTraducer.begin();iter1!=_inputsTraducer.end();iter1++)
    delete (*iter1);
  for(set<DFToDSForLoop *>::iterator iter2=_outputsTraducer.begin();iter2!=_outputsTraducer.end();iter2++)
    delete (*iter2);
}

void Loop::init(bool start)
{
  StaticDefinedComposedNode::init(start);
  _nbOfTurns=0;
  if(_node)
    _node->init(start); // if start is true, refresh the internal node
  else
    throw Exception("Loop::initLoop : no nodes specifies to be repeated ");
  delete _nodeForNullTurnOfLoops;
  _nodeForNullTurnOfLoops=0;
}

Node *Loop::edSetNode(Node *node)
{
  if(_node==node)
    return 0;
  if(node)
    {
      if(node->_father)
        {
          string what = "Loop::edSetNode: node "; what += node->getName(); what += " is not orphan ! "; 
          throw Exception(what);
        }
    }
  checkNoCrossHierachyWith(node);
  StaticDefinedComposedNode::edRemoveChild(_node);
  Node *ret=_node;
  _node=node;
  _node->_father=this;
  //set _modified flag so that edUpdateState() can refresh state
  modified();
  return ret;
}

bool Loop::edAddChild(Node *node) throw(YACS::Exception)
{
  return edSetNode(node);
}

Node *Loop::edRemoveNode()
{
  StaticDefinedComposedNode::edRemoveChild(_node);
  Node *ret=_node;
  _node=0;
  modified();
  return ret;
}

//! Collect all the child nodes that are ready
/*!
 * \param tasks : vector of tasks to collect ready nodes
 */
void Loop::getReadyTasks(std::vector<Task *>& tasks)
{
  if(!_node)
    return;
  /*
   * To change the way ComposedNode state is handled, uncomment the following line
   * see Bloc::getReadyTasks
   */
  if(_state==YACS::TOACTIVATE) setState(YACS::ACTIVATED);
  if(_state==YACS::TOACTIVATE || _state==YACS::ACTIVATED)
    if(_nodeForNullTurnOfLoops)
      _nodeForNullTurnOfLoops->getReadyTasks(tasks);
    else
      {
        _node->getReadyTasks(tasks);
        for(set<DSToDFForLoop *>::iterator iter1=_inputsTraducer.begin();iter1!=_inputsTraducer.end();iter1++)
          (*iter1)->getReadyTasks(tasks);
        for(set<DFToDSForLoop *>::iterator iter2=_outputsTraducer.begin();iter2!=_outputsTraducer.end();iter2++)
          (*iter2)->getReadyTasks(tasks);
      }
}

void Loop::edRemoveChild(Node *node) throw(YACS::Exception)
{
  StaticDefinedComposedNode::edRemoveChild(node);
  if(_node==node)
    _node=0;
  modified();
}

std::list<Node *> Loop::edGetDirectDescendants() const
{
  list<Node *> ret;
  if(_node)
    ret.push_back(_node);
  return ret;
}

std::list<InputPort *> Loop::getSetOfInputPort() const
{
  list<InputPort *> ret=StaticDefinedComposedNode::getSetOfInputPort();
  ret.push_back(getDecisionPort());
  return ret;
}

int Loop::getNumberOfInputPorts() const
{
  return StaticDefinedComposedNode::getNumberOfInputPorts()+1;
}

int Loop::getMaxLevelOfParallelism() const
{
  if(!_node)
    return 0;
  return _node->getMaxLevelOfParallelism();
}

Node *Loop::getChildByShortName(const std::string& name) const throw(YACS::Exception)
{
  if (_node)
    if(name==_node->getName())
      return _node;
  string what("node "); what+= name ; what+=" is not a child of loop node "; what += getName();
  throw Exception(what);
}

TypeCode* Loop::MappingDF2DS(TypeCode* type) throw(YACS::Exception)
{
  return type;
}

TypeCode* Loop::MappingDS2DF(TypeCode* type) throw(YACS::Exception)
{
  return type;
}

void Loop::buildDelegateOf(InPort * & port, OutPort *initialStart, const std::list<ComposedNode *>& pointsOfView)
{
  string typeOfPortInstance=port->getNameOfTypeOfCurrentInstance();
  if(typeOfPortInstance!=InputPort::NAME ||
     (typeOfPortInstance == InputPort::NAME && 
      initialStart->getNameOfTypeOfCurrentInstance()== OutputPort::NAME && 
      !isNecessaryToBuildSpecificDelegateDF2DS(pointsOfView)) )
    return ;
  InputPort *portCasted=(InputPort *)port;
  set<DSToDFForLoop*>::iterator iter;
  //Determinig if a DSToDFForLoop node has already been created for delegation of 'port'
  for(iter=_inputsTraducer.begin();iter!=_inputsTraducer.end();iter++)
    if((*iter)->getOutputPort("")->isAlreadyInSet(portCasted))
      break;
  if(iter==_inputsTraducer.end())
    {//first time that 'port' is delegated on higher level
      pair<set<DSToDFForLoop*>::iterator, bool> iter2=_inputsTraducer.insert(new DSToDFForLoop(this,portCasted->getName(),Loop::MappingDF2DS(portCasted->edGetType())));
      iter=iter2.first;
      (*iter)->getOutputPort("")->addInPort(portCasted);
    }
  else
    (*iter)->loopHasOneMoreRef();
  port=(*iter)->getInputDataStreamPort("");
}

void Loop::buildDelegateOf(std::pair<OutPort *, OutPort *>& port, InPort *finalTarget, const std::list<ComposedNode *>& pointsOfView)
{
  string typeOfPortInstance=(port.first)->getNameOfTypeOfCurrentInstance();
  if(typeOfPortInstance!=OutputPort::NAME ||
    ( typeOfPortInstance == OutputPort::NAME && 
      finalTarget->getNameOfTypeOfCurrentInstance()== InputPort::NAME && 
      !isNecessaryToBuildSpecificDelegateDF2DS(pointsOfView)) )
    return ;
  OutPort *portCasted=port.first;
  set<DFToDSForLoop*>::iterator iter;
  //Determinig if a DFToDSForLoop node has already been created for delegation of 'port'
  for(iter=_outputsTraducer.begin();iter!=_outputsTraducer.end();iter++)
    if(portCasted->isAlreadyLinkedWith((*iter)->getInputPort("")))
      break;
  DFToDSForLoop *kl;
  if(iter==_outputsTraducer.end())
    {//first time that 'port' is delegated on higher level
      //_outputsTraducer.insert(new DFToDSForLoop(this,portCasted->getName(),portCasted->edGetType()));
      kl=new DFToDSForLoop(this,portCasted->getName(),portCasted->edGetType());
      pair<set<DFToDSForLoop*>::iterator, bool> iter2=_outputsTraducer.insert(kl);
      iter=iter2.first;
      portCasted->addInPort((*iter)->getInputPort(""));
    }
  else
    {
      kl=*iter;
      kl->loopHasOneMoreRef();
    }
  edAddLink(isInMyDescendance((port.first)->getNode())->getOutGate(),kl->getInGate());
  port.first=(*iter)->getOutputDataStreamPort("");
}

void Loop::getDelegateOf(InPort * & port, OutPort *initialStart, const std::list<ComposedNode *>& pointsOfView) throw(YACS::Exception)
{
  string typeOfPortInstance=port->getNameOfTypeOfCurrentInstance();
  if(typeOfPortInstance!=InputPort::NAME ||
     (typeOfPortInstance == InputPort::NAME && 
      initialStart->getNameOfTypeOfCurrentInstance()== OutputPort::NAME && 
      !isNecessaryToBuildSpecificDelegateDF2DS(pointsOfView)) )
    return ;
  InputPort *portCasted=(InputPort *)port;
  set<DSToDFForLoop*>::iterator iter;
  for(iter=_inputsTraducer.begin();iter!=_inputsTraducer.end();iter++)
    if((*iter)->getOutputPort("")->isAlreadyInSet(portCasted))
      break;
  if(iter==_inputsTraducer.end())
    {
      string what("Loop::getDelegateOf : Port with name "); what+=portCasted->getName(); what+=" not exported by loop "; what+=_name; 
      throw Exception(what);
    }
  else
    port=(*iter)->getInputDataStreamPort("");
}

void Loop::getDelegateOf(std::pair<OutPort *, OutPort *>& port, InPort *finalTarget, 
                         const std::list<ComposedNode *>& pointsOfView) throw(YACS::Exception)
{
  string typeOfPortInstance=(port.first)->getNameOfTypeOfCurrentInstance();
  if(typeOfPortInstance!=OutputPort::NAME ||
    ( typeOfPortInstance == OutputPort::NAME && 
      finalTarget->getNameOfTypeOfCurrentInstance()== InputPort::NAME && 
      !isNecessaryToBuildSpecificDelegateDF2DS(pointsOfView)) )
    return ;
  OutPort *portCasted=port.first;
  set<DFToDSForLoop*>::iterator iter;
  for(iter=_outputsTraducer.begin();iter!=_outputsTraducer.end();iter++)
    if(portCasted->isAlreadyLinkedWith((*iter)->getInputPort("")))
      break;
  if(iter==_outputsTraducer.end())
    {
      string what("Loop::getDelegateOf : Port with name "); what+=portCasted->getName(); what+=" not exported by loop "; what+=_name; 
      throw Exception(what);
    }
  else
    port.first=(*iter)->getOutputDataStreamPort("");
}

void Loop::releaseDelegateOf(InPort * & port, OutPort *initialStart, const std::list<ComposedNode *>& pointsOfView) throw(YACS::Exception)
{
  string typeOfPortInstance=port->getNameOfTypeOfCurrentInstance();
  if(typeOfPortInstance!=InputPort::NAME ||
    ( typeOfPortInstance == InputPort::NAME && 
      initialStart->getNameOfTypeOfCurrentInstance()== OutputPort::NAME && 
      !isNecessaryToBuildSpecificDelegateDF2DS(pointsOfView)) )
    return ;
  InputPort *portCasted=(InputPort *)port;
  set<DSToDFForLoop*>::iterator iter;
  for(iter=_inputsTraducer.begin();iter!=_inputsTraducer.end();iter++)
    if((*iter)->getOutputPort("")->isAlreadyInSet(portCasted))
      break;
  if(iter==_inputsTraducer.end())
    {
      string what("Loop::releaseDelegateOf Port with name "); what+=portCasted->getName(); what+=" not exported by loop "; what+=_name; 
      throw Exception(what);
    }
  else
    {
      port=(*iter)->getInputDataStreamPort("");
      if((*iter)->loopHasOneLessRef())
        {
          (*iter)->getOutputPort("")->removeInPort(portCasted,false);
          delete (*iter);
          _inputsTraducer.erase(iter);
        }
    }
}

void Loop::releaseDelegateOf(OutPort *portDwn, OutPort *portUp, InPort *finalTarget, const std::list<ComposedNode *>& pointsOfView) throw(YACS::Exception)
{
  if(portDwn==portUp)
    return ;
  set<DFToDSForLoop*>::iterator iter;
  for(iter=_outputsTraducer.begin();iter!=_outputsTraducer.end();iter++)
    if((*iter)->getOutputDataStreamPort("")==portUp)
      break;
  if((*iter)->loopHasOneLessRef())
    {
      portDwn->removeInPort((*iter)->getInputPort(""),false);
      delete (*iter);
      _outputsTraducer.erase(iter);
    }
}

void Loop::checkNoCyclePassingThrough(Node *node) throw(YACS::Exception)
{
  //throw Exception("Loop::checkNoCyclePassingThrough : Internal error occured");
}

void Loop::checkCFLinks(const std::list<OutPort *>& starts, InputPort *end, unsigned char& alreadyFed, bool direction, LinkInfo& info) const
{
  Node *nodeEnd=end->getNode();
  if(nodeEnd==this)
    {//In this case 'end' port is a special port of this (for exemple ForLoop::_nbOfTimesPort)
      //ASSERT(!direction) see Loop::checkControlDependancy (bw only)
      solveObviousOrDelegateCFLinks(starts,end,alreadyFed,direction,info);
    }
  else
    StaticDefinedComposedNode::checkCFLinks(starts,end,alreadyFed,direction,info);
}

/*!
 * \note : States if a DF port must be considered on an upper level in hierarchy as a DS port or not from 'pointsOfView' observers.
 * \return : 
 *            - True : a traduction DF->DS has to be done
 *            - False : no traduction needed
 */
bool Loop::isNecessaryToBuildSpecificDelegateDF2DS(const std::list<ComposedNode *>& pointsOfView)
{
  bool ret=false;
  for(list<ComposedNode *>::const_iterator iter=pointsOfView.begin();iter!=pointsOfView.end() && !ret;iter++)
    ret=(*iter)->isRepeatedUnpredictablySeveralTimes();
  return ret;
}

//! Connect an OutPort to an InPort and add control link if necessary
/*!
 * Connect the ports with a data link (edAddLink) 
 * In a Loop don't add control flow link : use this only to add data back links
 *
 * \param start : the OutPort to connect
 * \param end : the InPort to connect
 * \return  true if a new link has been created, false otherwise.
 */
bool Loop::edAddDFLink(OutPort *start, InPort *end) throw(YACS::Exception)
{
  return edAddLink(start,end);
}

//! Dump the node state to a stream
/*!
 * \param os : the output stream
 */
void Loop::writeDot(std::ostream &os) const
{
  os << "  subgraph cluster_" << getId() << "  {\n" ;
  //only one node in a loop
  if(_node)
    {
      _node->writeDot(os);
      os << getId() << " -> " << _node->getId() << ";\n";
    }
  os << "}\n" ;
  os << getId() << "[fillcolor=\"" ;
  YACS::StatesForNode state=getEffectiveState();
  os << getColorState(state);
  os << "\" label=\"" << "Loop:" ;
  os << getQualifiedName() <<"\"];\n";
}


void Loop::accept(Visitor *visitor)
{
  visitor->visitLoop(this);
}

void Loop::checkControlDependancy(OutPort *start, InPort *end, bool cross,
                                  std::map < ComposedNode *,  std::list < OutPort * >, SortHierarc >& fw,
                                  std::vector<OutPort *>& fwCross,
                                  std::map< ComposedNode *, std::list < OutPort *>, SortHierarc >& bw,
                                  LinkInfo& info) const
{
  //First testing if end==getDecisionPort. This is the only case possible in theory.
  if(end!=getDecisionPort())
    return StaticDefinedComposedNode::checkControlDependancy(start,end,cross,fw,fwCross,bw,info);
  if(cross)
    throw Exception("Internal error occured - cross type link detected on decision port of a loop. Forbidden !");
  fw[(ComposedNode *)this].push_back(start);
}

void Loop::checkBasicConsistency() const throw(YACS::Exception)
{
  DEBTRACE("Loop::checkBasicConsistency");
  ComposedNode::checkBasicConsistency();
  if(!_node)
    throw Exception("For a loop, internal node is mandatory");
}

/*!
 *  For use only when loading a previously saved execution
 */

void YACS::ENGINE::NbDoneLoader(Loop* node, int val)
{
  node->_nbOfTurns = val;
}
