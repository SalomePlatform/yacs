//  Copyright (C) 2006-2010  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "ForEachLoop.hxx"
#include "TypeCode.hxx"
#include "Visitor.hxx"
#include "ComposedNode.hxx"
#include <iostream>
#include <sstream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

/*! \class YACS::ENGINE::ForEachLoop
 *  \brief Loop node for parametric calculation
 *
 *  \ingroup Nodes
 */

const char FakeNodeForForEachLoop::NAME[]="thisIsAFakeNode";

const char SplitterNode::NAME_OF_SEQUENCE_INPUT[]="SmplsCollection";

const char ForEachLoop::NAME_OF_SPLITTERNODE[]="splitter";

const int ForEachLoop::NOT_RUNNING_BRANCH_ID=-1;

InterceptorInputPort::InterceptorInputPort(const std::string& name, Node *node, TypeCode* type):AnyInputPort(name,node,type),
                                                                                                DataPort(name,node,type),Port(node),
                                                                                                _repr(0)
{
}

InterceptorInputPort::InterceptorInputPort(const  InterceptorInputPort& other, Node *newHelder):AnyInputPort(other,newHelder),DataPort(other,newHelder),
                                                                                                Port(other,newHelder),
                                                                                                _repr(0)
{
}

void InterceptorInputPort::getAllRepresentants(std::set<InPort *>& repr) const
{
  set<InPort *> ports=_repr->edSetInPort();
  for(set<InPort *>::iterator iter=ports.begin();iter!=ports.end();iter++)
    (*iter)->getAllRepresentants(repr);
}

InputPort *InterceptorInputPort::clone(Node *newHelder) const
{
  return new InterceptorInputPort(*this,newHelder);
}

void InterceptorInputPort::setRepr(AnySplitOutputPort *repr)
{
  _repr=repr;
}

bool AnySplitOutputPort::decrRef()
{
  return (--_cnt==0);
}

void AnySplitOutputPort::incrRef() const
{
  _cnt++;
}

AnySplitOutputPort::AnySplitOutputPort(const std::string& name, Node *node, TypeCode *type):OutputPort(name,node,type),
                                                                                            DataPort(name,node,type),Port(node),
                                                                                            _repr(0),_intercptr(0),_cnt(1)
{
}

AnySplitOutputPort::AnySplitOutputPort(const AnySplitOutputPort& other, Node *newHelder):OutputPort(other,newHelder),
                                                                                         DataPort(other,newHelder),
                                                                                         Port(other,newHelder),
                                                                                         _repr(0),_intercptr(0),_cnt(1)
{
}

bool AnySplitOutputPort::addInPort(InPort *inPort) throw(YACS::Exception)
{
  bool ret=OutputPort::addInPort(inPort);
  if(_repr)
    _repr->addInPort(_intercptr);
  return ret;
}

void AnySplitOutputPort::getAllRepresented(std::set<OutPort *>& represented) const
{
  if(!_repr)
    OutPort::getAllRepresented(represented);
  else
    _repr->getAllRepresented(represented);
}

int AnySplitOutputPort::removeInPort(InPort *inPort, bool forward) throw(YACS::Exception)
{
  bool ret=OutputPort::removeInPort(inPort,forward);
  if(_repr)
    if(_setOfInputPort.empty())
      _repr->removeInPort(_intercptr,forward);
  return ret;
}

void AnySplitOutputPort::addRepr(OutPort *repr, InterceptorInputPort *intercptr)
{
  _repr=repr;
  _intercptr=intercptr;
}

OutputPort *AnySplitOutputPort::clone(Node *newHelder) const
{
  return new AnySplitOutputPort(*this,newHelder);
}

SeqAnyInputPort::SeqAnyInputPort(const std::string& name, Node *node, TypeCodeSeq* type):AnyInputPort(name,node,type),DataPort(name,node,type),Port(node)
{
  _type->decrRef();
}

SeqAnyInputPort::SeqAnyInputPort(const  SeqAnyInputPort& other, Node *newHelder):AnyInputPort(other,newHelder),DataPort(other,newHelder),Port(other,newHelder)
{
}

InputPort *SeqAnyInputPort::clone(Node *newHelder) const
{
  return new SeqAnyInputPort(*this,newHelder);
}

unsigned SeqAnyInputPort::getNumberOfElements() const
{
  const SequenceAny * valCsted=(const SequenceAny *) _value;
  if (valCsted) return valCsted->size();
  return 0;
}

Any *SeqAnyInputPort::getValueAtRank(int i) const
{
  const SequenceAny * valCsted=(const SequenceAny *) _value;
  AnyPtr ret=(*valCsted)[i];
  ret->incrRef();
  return ret;
}

std::string SeqAnyInputPort::dump()
{
  stringstream xmldump;
  int nbElem = getNumberOfElements();
  xmldump << "<value><array><data>" << endl;
  for (int i = 0; i < nbElem; i++)
    {
      Any *val = getValueAtRank(i);
      switch (val->getType()->kind())
        {
        case Double:
          xmldump << "<value><double>" << val->getDoubleValue() << "</double></value>" << endl;
          break;
        case Int:
          xmldump << "<value><int>" << val->getIntValue() << "</int></value>" << endl;
          break;
        case Bool:
          xmldump << "<value><boolean>" << val->getBoolValue() << "</boolean></value>" << endl;
          break;
        case String:
          xmldump << "<value><string>" << val->getStringValue() << "</string></value>" << endl;
          break;
        case Objref:
          xmldump << "<value><objref>" << val->getStringValue() << "</objref></value>" << endl;
          break;
        default:
          xmldump << "<value><error> NO_SERIALISATION_AVAILABLE </error></value>" << endl;
          break;
        }
    }
  xmldump << "</data></array></value>" << endl;
  return xmldump.str();
}

SplitterNode::SplitterNode(const std::string& name, TypeCode *typeOfData, 
                           ForEachLoop *father):ElementaryNode(name),
                                                _dataPortToDispatch(NAME_OF_SEQUENCE_INPUT,
                                                                    this,(TypeCodeSeq *)TypeCode::sequenceTc("","",typeOfData))
{
  _father=father;
}

SplitterNode::SplitterNode(const SplitterNode& other, ForEachLoop *father):ElementaryNode(other,father),
                                                                           _dataPortToDispatch(other._dataPortToDispatch,this)
{
}

InputPort *SplitterNode::getInputPort(const std::string& name) const throw(YACS::Exception)
{
  if(name==NAME_OF_SEQUENCE_INPUT)
    return (InputPort *)&_dataPortToDispatch;
  else
    return ElementaryNode::getInputPort(name);
}

Node *SplitterNode::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new SplitterNode(*this,(ForEachLoop *)father);
}

unsigned SplitterNode::getNumberOfElements() const
{
  return _dataPortToDispatch.getNumberOfElements();
}

void SplitterNode::execute()
{
  //Nothing : should never been called elsewhere big problem...
}

void SplitterNode::init(bool start)
{
  ElementaryNode::init(start);
  _dataPortToDispatch.exInit(start);
}

void SplitterNode::putSplittedValueOnRankTo(int rankInSeq, int branch, bool first)
{
  Any *valueToDispatch=_dataPortToDispatch.getValueAtRank(rankInSeq);
  ForEachLoop *fatherTyped=(ForEachLoop *)_father;
  fatherTyped->putValueOnBranch(valueToDispatch,branch,first);
  valueToDispatch->decrRef();
}

FakeNodeForForEachLoop::FakeNodeForForEachLoop(ForEachLoop *loop, bool normalFinish):ElementaryNode(NAME),
                                                                                     _loop(loop),
                                                                                     _normalFinish(normalFinish)
{
  _state=YACS::TOACTIVATE;
  _father=_loop->getFather();
}

FakeNodeForForEachLoop::FakeNodeForForEachLoop(const FakeNodeForForEachLoop& other):ElementaryNode(other),_loop(0),
                                                                                    _normalFinish(false)
{
}

Node *FakeNodeForForEachLoop::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new FakeNodeForForEachLoop(*this);
}

void FakeNodeForForEachLoop::exForwardFailed()
{
  _loop->exForwardFailed();
}

void FakeNodeForForEachLoop::exForwardFinished()
{ 
  _loop->exForwardFinished();
}

void FakeNodeForForEachLoop::execute()
{
  if(!_normalFinish)
    throw Exception("");//only to trigger ABORT on Executor
  else
    _loop->pushAllSequenceValues();
}

void FakeNodeForForEachLoop::aborted()
{
  _loop->setState(YACS::ERROR);
}

void FakeNodeForForEachLoop::finished()
{
  _loop->setState(YACS::DONE);
}

ForEachLoop::ForEachLoop(const std::string& name, TypeCode *typeOfDataSplitted):DynParaLoop(name,typeOfDataSplitted),
                                                                                _splitterNode(NAME_OF_SPLITTERNODE,typeOfDataSplitted,this),
                                                                                _execCurrentId(0),_nodeForSpecialCases(0)
{
}

ForEachLoop::ForEachLoop(const ForEachLoop& other, ComposedNode *father, bool editionOnly):DynParaLoop(other,father,editionOnly),
                                                                                           _splitterNode(other._splitterNode,this),
                                                                                           _execCurrentId(0),_nodeForSpecialCases(0)
{
  int i=0;
  if(!editionOnly)
    for(vector<AnySplitOutputPort *>::const_iterator iter2=other._outGoingPorts.begin();iter2!=other._outGoingPorts.end();iter2++,i++)
      {
        AnySplitOutputPort *temp=new AnySplitOutputPort(*(*iter2),this);
        InterceptorInputPort *interc=new InterceptorInputPort(*other._intecptrsForOutGoingPorts[i],this);
        temp->addRepr(getOutPort((*iter2)->getName()),interc);
        interc->setRepr(temp);
        _outGoingPorts.push_back(temp);
        _intecptrsForOutGoingPorts.push_back(interc);
      }
}

Node *ForEachLoop::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new ForEachLoop(*this,father,editionOnly);
}

ForEachLoop::~ForEachLoop()
{
  cleanDynGraph();
  for(vector<AnySplitOutputPort *>::iterator iter=_outGoingPorts.begin();iter!=_outGoingPorts.end();iter++)
    delete *iter;
  for(vector<InterceptorInputPort *>::iterator iter2=_intecptrsForOutGoingPorts.begin();iter2!=_intecptrsForOutGoingPorts.end();iter2++)
    delete *iter2;
}

void ForEachLoop::init(bool start)
{
  DynParaLoop::init(start);
  _splitterNode.init(start);
  _execCurrentId=0;
  cleanDynGraph();
}

void ForEachLoop::exUpdateState()
{
  if(_state == YACS::DISABLED)
    return;
  if(_inGate.exIsReady())
    {
      //setState(YACS::TOACTIVATE); // call this method below
      //internal graph update
      int i;
      int nbOfBr=_nbOfBranches.getIntValue();
      int nbOfElts=_splitterNode.getNumberOfElements();
      if(nbOfElts==0)
        {
          prepareSequenceValues(0);
          delete _nodeForSpecialCases;
          _nodeForSpecialCases=new FakeNodeForForEachLoop(this,true);
          return ;
        }
      if(nbOfBr<=0)
        {
          delete _nodeForSpecialCases;
          _nodeForSpecialCases=new FakeNodeForForEachLoop(this,getAllOutPortsLeavingCurrentScope().empty());
          return ;
        }
      if(nbOfBr>nbOfElts)
        nbOfBr=nbOfElts;
      _execNodes.resize(nbOfBr);
      _execIds.resize(nbOfBr);
      _execOutGoingPorts.resize(nbOfBr);
      prepareSequenceValues(nbOfElts);
      if(_initNode)
        _execInitNodes.resize(nbOfBr);
      _initializingCounter = 0;
      if (_finalizeNode)
        _execFinalizeNodes.resize(nbOfBr);

      vector<Node *> origNodes;
      origNodes.push_back(_initNode);
      origNodes.push_back(_node);
      origNodes.push_back(_finalizeNode);

      //Conversion exceptions can be thrown by createOutputOutOfScopeInterceptors 
      //so catch them to control errors
      try
        {
          for(i=0;i<nbOfBr;i++)
            {
              DEBTRACE( "-------------- 1 " << i << " " << _execCurrentId);
              _execIds[i]=_execCurrentId;
              DEBTRACE( "-------------- 2" );
              vector<Node *> clonedNodes = cloneAndPlaceNodesCoherently(origNodes);
              if(_initNode)
                _execInitNodes[i] = clonedNodes[0];
              _execNodes[i] = clonedNodes[1];
              if(_finalizeNode)
                _execFinalizeNodes[i] = clonedNodes[2];
              DEBTRACE( "-------------- 4" );
              prepareInputsFromOutOfScope(i);
              DEBTRACE( "-------------- 5" );
              createOutputOutOfScopeInterceptors(i);
              DEBTRACE( "-------------- 6" );
              _splitterNode.putSplittedValueOnRankTo(_execCurrentId++,i,true);
              DEBTRACE( "-------------- 7" );
            } 
        }
      catch(YACS::Exception& ex)
        {
          //ForEachLoop must be put in error and the exception rethrown to notify the caller
          DEBTRACE( "ForEachLoop::exUpdateState: " << ex.what() );
          setState(YACS::ERROR);
          exForwardFailed();
          throw;
        }

      setState(YACS::ACTIVATED); // move the calling of setState method there for adding observers for clone nodes in GUI part

      //let's go
      for(i=0;i<nbOfBr;i++)
        if(_initNode)
          {
            _execInitNodes[i]->exUpdateState();
            _initializingCounter++;
          }
        else
          {
            _nbOfEltConsumed++;
            _execNodes[i]->exUpdateState();
          }

      forwardExecStateToOriginalBody(_execNodes[nbOfBr-1]);
    }
}

void ForEachLoop::getReadyTasks(std::vector<Task *>& tasks)
{
  if(!_node)
    return;
  if(_state==YACS::TOACTIVATE) setState(YACS::ACTIVATED);
  if(_state==YACS::TOACTIVATE || _state==YACS::ACTIVATED)
    {
      if(_nodeForSpecialCases)
        {
          _nodeForSpecialCases->getReadyTasks(tasks);
          return ;
        }
      vector<Node *>::iterator iter;
      for (iter=_execNodes.begin() ; iter!=_execNodes.end() ; iter++)
        (*iter)->getReadyTasks(tasks);
      for (iter=_execInitNodes.begin() ; iter!=_execInitNodes.end() ; iter++)
        (*iter)->getReadyTasks(tasks);
      for (iter=_execFinalizeNodes.begin() ; iter!=_execFinalizeNodes.end() ; iter++)
        (*iter)->getReadyTasks(tasks);
    }
}

int ForEachLoop::getNumberOfInputPorts() const
{
  return DynParaLoop::getNumberOfInputPorts()+1;
}

void ForEachLoop::checkNoCyclePassingThrough(Node *node) throw(YACS::Exception)
{
  //TO DO
}

void ForEachLoop::selectRunnableTasks(std::vector<Task *>& tasks)
{
}

std::list<InputPort *> ForEachLoop::getSetOfInputPort() const
{
  list<InputPort *> ret=DynParaLoop::getSetOfInputPort();
  ret.push_back((InputPort *)&_splitterNode._dataPortToDispatch);
  return ret;
}

std::list<InputPort *> ForEachLoop::getLocalInputPorts() const
{
  list<InputPort *> ret=DynParaLoop::getLocalInputPorts();
  ret.push_back((InputPort *)&_splitterNode._dataPortToDispatch);
  return ret;
}

InputPort *ForEachLoop::getInputPort(const std::string& name) const throw(YACS::Exception)
{
  if(name==SplitterNode::NAME_OF_SEQUENCE_INPUT)
    return (InputPort *)&_splitterNode._dataPortToDispatch;
  else
    return DynParaLoop::getInputPort(name);
}

OutputPort *ForEachLoop::getOutputPort(const std::string& name) const throw(YACS::Exception)
{
  for(vector<AnySplitOutputPort *>::const_iterator iter=_outGoingPorts.begin();iter!=_outGoingPorts.end();iter++)
    {
      if(name==(*iter)->getName())
        return (OutputPort *)(*iter);
    }
  return DynParaLoop::getOutputPort(name);
}

OutPort *ForEachLoop::getOutPort(const std::string& name) const throw(YACS::Exception)
{
  for(vector<AnySplitOutputPort *>::const_iterator iter=_outGoingPorts.begin();iter!=_outGoingPorts.end();iter++)
    {
      if(name==(*iter)->getName())
        return (OutPort *)(*iter);
    }
  return DynParaLoop::getOutPort(name);
}

Node *ForEachLoop::getChildByShortName(const std::string& name) const throw(YACS::Exception)
{
  if(name==NAME_OF_SPLITTERNODE)
    return (Node *)&_splitterNode;
  else
    return DynParaLoop::getChildByShortName(name);
}

//! Method used to notify the node that a child node has finished
/*!
 * Update the current state and return the change state
 *
 *  \param node : the child node that has finished
 *  \return the state change
 */
YACS::Event ForEachLoop::updateStateOnFinishedEventFrom(Node *node)
{
  DEBTRACE("updateStateOnFinishedEventFrom " << node->getName() << " " << node->getState());
  unsigned int id;
  switch(getIdentityOfNotifyerNode(node,id))
    {
    case INIT_NODE:
      _execNodes[id]->exUpdateState();
      _nbOfEltConsumed++;
      _initializingCounter--;
      if (_initializingCounter == 0) _initNode->setState(DONE);
      break;
    case WORK_NODE:
      storeOutValsInSeqForOutOfScopeUse(_execIds[id],id);
      if(_execCurrentId==_splitterNode.getNumberOfElements())
        {//No more elements of _dataPortToDispatch to treat
          _execIds[id]=NOT_RUNNING_BRANCH_ID;
          //analyzing if some samples are still on treatment on other branches.
          bool isFinished=true;
          for(int i=0;i<_execIds.size() && isFinished;i++)
            isFinished=(_execIds[i]==NOT_RUNNING_BRANCH_ID);
          if(isFinished)
            {
              try 
                {
                  pushAllSequenceValues();
  
                  if (_node)
                    {
                      _node->setState(YACS::DONE);
     
                      ComposedNode* compNode = dynamic_cast<ComposedNode*>(_node);
                      if (compNode)
                        {
                          std::list<Node *> aChldn = compNode->getAllRecursiveConstituents();
                          std::list<Node *>::iterator iter=aChldn.begin();
                          for(;iter!=aChldn.end();iter++)
                            (*iter)->setState(YACS::DONE);
                        }
                    }

                  if (_finalizeNode == NULL)
                    {
                      // No finalize node, we just finish the loop at the end of exec nodes execution
                      setState(YACS::DONE);
                      return YACS::FINISH;
                    }
                  else
                    {
                      // Run the finalize nodes, the loop will be done only when they all finish
                      _unfinishedCounter = 0;  // This counter indicates how many branches are not finished
                      for (int i=0 ; i<_execIds.size() ; i++)
                        {
                          YASSERT(_execIds[i] == NOT_RUNNING_BRANCH_ID);
                          DEBTRACE("Launching finalize node for branch " << i);
                          _execFinalizeNodes[i]->exUpdateState();
                          _unfinishedCounter++;
                        }
                      return YACS::NOEVENT;
                    }
                }
              catch(YACS::Exception& ex)
                {
                  DEBTRACE("ForEachLoop::updateStateOnFinishedEventFrom: "<<ex.what());
                  //no way to push results : put following nodes in FAILED state
                  //TODO could be more fine grain : put only concerned nodes in FAILED state
                  exForwardFailed();
                  setState(YACS::ERROR);
                  return YACS::ABORT;
                }
            }
        }
      else if(_state == YACS::ACTIVATED)
        {//more elements to do and loop still activated
          _execIds[id]=_execCurrentId;
          node->init(false);
          _splitterNode.putSplittedValueOnRankTo(_execCurrentId++,id,false);
          node->exUpdateState();
          forwardExecStateToOriginalBody(node);
          _nbOfEltConsumed++;
        }
      else
        {//elements to process and loop no more activated
          DEBTRACE("foreach loop state " << _state);
        }
      break;
    case FINALIZE_NODE:
    {
      DEBTRACE("Finalize node finished on branch " << id);
      _unfinishedCounter--;
      DEBTRACE(_unfinishedCounter << " finalize nodes still running");
      if (_unfinishedCounter == 0)
        {
          _finalizeNode->setState(YACS::DONE);
          setState(YACS::DONE);
          return YACS::FINISH;
        }
      else
        return YACS::NOEVENT;
      break;
    }
    default:
      YASSERT(false);
    }
  return YACS::NOEVENT;
}

void ForEachLoop::buildDelegateOf(std::pair<OutPort *, OutPort *>& port, InPort *finalTarget, const std::list<ComposedNode *>& pointsOfView)
{
  DynParaLoop::buildDelegateOf(port,finalTarget,pointsOfView);
  string typeOfPortInstance=(port.first)->getNameOfTypeOfCurrentInstance();
  if(typeOfPortInstance==OutputPort::NAME)
    {
      vector<AnySplitOutputPort *>::iterator iter=_outGoingPorts.begin();
      int i=0;
      for(;iter!=_outGoingPorts.end();iter++,i++)
        if((*iter)->getRepr()==port.first)
          break;
      if(iter!=_outGoingPorts.end())
        {
          (*iter)->incrRef();
          (*iter)->addRepr(port.first,_intecptrsForOutGoingPorts[i]);
          port.first=*iter;
        }
      else
        {
          TypeCodeSeq *newTc=(TypeCodeSeq *)TypeCode::sequenceTc("","",port.first->edGetType());
          AnySplitOutputPort *newPort=new AnySplitOutputPort(getPortName(port.first),this,newTc);
          InterceptorInputPort *intercptor=new InterceptorInputPort(string("intercptr for ")+getPortName(port.first),this,port.first->edGetType());
          intercptor->setRepr(newPort);
          newTc->decrRef();
          newPort->addRepr(port.first,intercptor);
          _outGoingPorts.push_back(newPort);
          _intecptrsForOutGoingPorts.push_back(intercptor);
          port.first=newPort;
        }
    }
  else
    throw Exception("ForEachLoop::buildDelegateOf : not implemented for DS because not specified");
}

void ForEachLoop::getDelegateOf(std::pair<OutPort *, OutPort *>& port, InPort *finalTarget, const std::list<ComposedNode *>& pointsOfView) throw(YACS::Exception)
{
  string typeOfPortInstance=(port.first)->getNameOfTypeOfCurrentInstance();
  if(typeOfPortInstance==OutputPort::NAME)
    {
      vector<AnySplitOutputPort *>::iterator iter=_outGoingPorts.begin();
      for(;iter!=_outGoingPorts.end();iter++)
        if((*iter)->getRepr()==port.first)
          break;
      if(iter==_outGoingPorts.end())
        {
          string what("ForEachLoop::getDelegateOf : Port with name "); what+=port.first->getName(); what+=" not exported by ForEachLoop "; what+=_name; 
          throw Exception(what);
        }
      else
        port.first=(*iter);
    }
  else
    throw Exception("ForEachLoop::getDelegateOf : not implemented because not specified");
}

void ForEachLoop::releaseDelegateOf(OutPort *portDwn, OutPort *portUp, InPort *finalTarget, const std::list<ComposedNode *>& pointsOfView) throw(YACS::Exception)
{
  string typeOfPortInstance=portDwn->getNameOfTypeOfCurrentInstance();
  if(typeOfPortInstance==OutputPort::NAME)
    {
      vector<AnySplitOutputPort *>::iterator iter=_outGoingPorts.begin();
      vector<InterceptorInputPort *>::iterator iter2=_intecptrsForOutGoingPorts.begin();
      for(;iter!=_outGoingPorts.end();iter++,iter2++)
        if((*iter)->getRepr()==portDwn)
          break;
      //ASSERT(portUp==*iter.second)
      if((*iter)->decrRef())
        {
          AnySplitOutputPort *p=*iter;
          _outGoingPorts.erase(iter);
          delete p;
          InterceptorInputPort *ip=*iter2;
          _intecptrsForOutGoingPorts.erase(iter2);
          delete ip;
        }
    }
}

OutPort *ForEachLoop::getDynOutPortByAbsName(int branchNb, const std::string& name)
{
  string portName, nodeName;
  splitNamesBySep(name,Node::SEP_CHAR_IN_PORT,nodeName,portName,false);
  Node *staticChild = getChildByName(nodeName);
  return _execNodes[branchNb]->getOutPort(portName);//It's impossible(garanteed by YACS::ENGINE::ForEachLoop::buildDelegateOf)
  //that a link starting from _initNode goes out of scope of 'this'.
}

void ForEachLoop::cleanDynGraph()
{
  DynParaLoop::cleanDynGraph();
  for(vector< SequenceAny *>::iterator iter3=_execVals.begin();iter3!=_execVals.end();iter3++)
    (*iter3)->decrRef();
  _execVals.clear();
  for(vector< vector<AnyInputPort *> >::iterator iter4=_execOutGoingPorts.begin();iter4!=_execOutGoingPorts.end();iter4++)
    for(vector<AnyInputPort *>::iterator iter5=(*iter4).begin();iter5!=(*iter4).end();iter5++)
      delete *iter5;
  _execOutGoingPorts.clear();
}

void ForEachLoop::storeOutValsInSeqForOutOfScopeUse(int rank, int branchNb)
{
  vector<AnyInputPort *>::iterator iter;
  int i=0;
  for(iter=_execOutGoingPorts[branchNb].begin();iter!=_execOutGoingPorts[branchNb].end();iter++,i++)
    {
      Any *val=(Any *)(*iter)->getValue();
      _execVals[i]->setEltAtRank(rank,val);
    }
}

void ForEachLoop::prepareSequenceValues(int sizeOfSamples)
{
  _execVals.resize(_outGoingPorts.size());
  vector<AnySplitOutputPort *>::iterator iter=_outGoingPorts.begin();
  for(int i=0;iter!=_outGoingPorts.end();iter++,i++)
    _execVals[i]=SequenceAny::New((*iter)->edGetType()->contentType(),sizeOfSamples);
}

void ForEachLoop::pushAllSequenceValues()
{
  vector<AnySplitOutputPort *>::iterator iter=_outGoingPorts.begin();
  int i=0;
  for(;iter!=_outGoingPorts.end();iter++,i++)
    (*iter)->put((const void *)_execVals[i]);
}

void ForEachLoop::createOutputOutOfScopeInterceptors(int branchNb)
{
  vector<AnySplitOutputPort *>::iterator iter=_outGoingPorts.begin();
  int i=0;
  for(;iter!=_outGoingPorts.end();iter++,i++)
    {
      DEBTRACE( (*iter)->getName() << " " << (*iter)->edGetType()->kind() );
      //AnyInputPort *interceptor=new AnyInputPort((*iter)->getName(),this,(*iter)->edGetType());
      OutPort *portOut=getDynOutPortByAbsName(branchNb,getOutPortName(((*iter)->getRepr())));
      DEBTRACE( portOut->getName() );
      AnyInputPort *interceptor=new AnyInputPort((*iter)->getName(),this,portOut->edGetType());
      portOut->addInPort(interceptor);
      _execOutGoingPorts[branchNb].push_back(interceptor);
    }
}

void ForEachLoop::checkLinkPossibility(OutPort *start, const std::list<ComposedNode *>& pointsOfViewStart,
                                       InPort *end, const std::list<ComposedNode *>& pointsOfViewEnd) throw(YACS::Exception)
{
  if(isInMyDescendance(start->getNode())==_node)
    throw Exception("ForEachLoop::checkLinkPossibility : A link from work node to init node not permitted");
}

std::list<OutputPort *> ForEachLoop::getLocalOutputPorts() const
{
  list<OutputPort *> ret;
  ret.push_back(getOutputPort(NAME_OF_SPLITTED_SEQ_OUT)); 
  return ret;
}

void ForEachLoop::accept(Visitor *visitor)
{
  visitor->visitForEachLoop(this);
}

//! Dump the node state to a stream
/*!
 * \param os : the output stream
 */
void ForEachLoop::writeDot(std::ostream &os) const
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
  os << getName() <<"\"];\n";
}
