// Copyright (C) 2006-2025  CEA, EDF
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

#include "ForEachLoop.hxx"
#include "TypeCode.hxx"
#include "Visitor.hxx"
#include "ComposedNode.hxx"
#include "Executor.hxx"
#include "AutoLocker.hxx"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>    // std::replace_if
#include <functional>   // std::bind

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

#ifdef WIN32
#include <functional>
#endif

using namespace YACS::ENGINE;
using namespace std;

/*! \class YACS::ENGINE::ForEachLoop
 *  \brief Loop node for parametric calculation
 *
 *  \ingroup Nodes
 */

const char FakeNodeForForEachLoop::NAME[]="thisIsAFakeNode";

const char SplitterNode::NAME_OF_SEQUENCE_INPUT[]="SmplsCollection";

const char ForEachLoopGen::NAME_OF_SPLITTERNODE[]="splitter";

const int ForEachLoopGen::NOT_RUNNING_BRANCH_ID=-1;

const char ForEachLoopGen::INTERCEPTOR_STR[]="_interceptor";

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

bool AnySplitOutputPort::addInPort(InPort *inPort)
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

int AnySplitOutputPort::removeInPort(InPort *inPort, bool forward)
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
      switch (((YACS::ENGINE::TypeCodeSeq *)edGetType())->contentType()->kind())
        {
        case Double:
          xmldump << "<value><double>" << setprecision(16) << val->getDoubleValue() << "</double></value>" << endl;
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
          xmldump << "<value><objref>" << ToBase64(val->getStringValue()) << "</objref></value>" << endl;
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
                           ForEachLoopGen *father):ElementaryNode(name),
                                                _dataPortToDispatch(NAME_OF_SEQUENCE_INPUT,
                                                                    this,(TypeCodeSeq *)TypeCode::sequenceTc("","",typeOfData))
{
  _father=father;
}

SplitterNode::SplitterNode(const SplitterNode& other, ForEachLoopGen *father):ElementaryNode(other,father),
                                                                           _dataPortToDispatch(other._dataPortToDispatch,this)
{
}

InputPort *SplitterNode::getInputPort(const std::string& name) const
{
  if(name==NAME_OF_SEQUENCE_INPUT)
    return (InputPort *)&_dataPortToDispatch;
  else
    return ElementaryNode::getInputPort(name);
}

Node *SplitterNode::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new SplitterNode(*this,(ForEachLoopGen *)father);
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
  ForEachLoopGen *fatherTyped=(ForEachLoopGen *)_father;
  fatherTyped->putValueOnBranch(valueToDispatch,branch,first);
  valueToDispatch->decrRef();
}

FakeNodeForForEachLoop::FakeNodeForForEachLoop(ForEachLoopGen *loop, bool normalFinish):ElementaryNode(NAME),
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

ForEachLoopPassedData::ForEachLoopPassedData(const std::vector<unsigned int>& passedIds, const std::vector<SequenceAny *>& passedOutputs, const std::vector<std::string>& nameOfOutputs):_passedIds(passedIds),_passedOutputs(passedOutputs),_nameOfOutputs(nameOfOutputs)
{
  std::size_t sz(_passedIds.size()),sz1(passedOutputs.size()),sz2(nameOfOutputs.size());
  if(sz1!=sz2)
    throw YACS::Exception("ForEachLoopPassedData::ForEachLoopPassedData : nameOfOutputs and passedOutputs must have the same size !");
  for(std::vector<SequenceAny *>::iterator it=_passedOutputs.begin();it!=_passedOutputs.end();it++)
    {
      const SequenceAny *elt(*it);
      if(elt)
        if(sz!=(std::size_t)elt->size())
          throw YACS::Exception("ForEachLoopPassedData::ForEachLoopPassedData : incoherent input of passed data !");
    }
  for(std::vector<SequenceAny *>::iterator it=_passedOutputs.begin();it!=_passedOutputs.end();it++)
    {
      SequenceAny *elt(*it);
      if(elt)
        elt->incrRef();
    }
}

ForEachLoopPassedData::ForEachLoopPassedData(const ForEachLoopPassedData& copy)
: _passedIds(copy._passedIds),
  _passedOutputs(copy._passedOutputs),
  _nameOfOutputs(copy._nameOfOutputs),
  _flagsIds(copy._flagsIds)
{
}

ForEachLoopPassedData::~ForEachLoopPassedData()
{
  for(std::vector<SequenceAny *>::iterator it=_passedOutputs.begin();it!=_passedOutputs.end();it++)
    {
      SequenceAny *elt(*it);
      if(elt)
        elt->decrRef();
    }
}

void ForEachLoopPassedData::init()
{
  _flagsIds.clear();
}

void ForEachLoopPassedData::checkCompatibilyWithNb(int nbOfElts) const
{
  if(nbOfElts<0)
    throw YACS::Exception("ForEachLoopPassedData::checkCompatibilyWithNb : nb of elts is expected to be > 0 !");
  std::size_t sizeExp(_passedIds.size()),nbOfElts2(nbOfElts);
  if(nbOfElts2<sizeExp)
    throw YACS::Exception("ForEachLoopPassedData::checkCompatibilyWithNb : Invalid nb of elemts in input seq regarding passed data set !");
  for(std::vector<unsigned int>::const_iterator it=_passedIds.begin();it!=_passedIds.end();it++)
    {
      if((*it)>=nbOfElts2)
        throw YACS::Exception("ForEachLoopPassedData::checkCompatibilyWithNb : Invalid nb of elemts in input seq regarding passed data set 2 !");
    }
  _flagsIds.resize(nbOfElts);
  std::fill(_flagsIds.begin(),_flagsIds.end(),false);
  for(std::vector<unsigned int>::const_iterator it=_passedIds.begin();it!=_passedIds.end();it++)
    {
      if(*it<nbOfElts)
        {
          if(!_flagsIds[*it])
            _flagsIds[*it]=true;
          else
            {
              std::ostringstream oss; oss << "ForEachLoopPassedData::checkCompatibilyWithNb : id " << *it << " in list of ids appears more than once !";
              throw YACS::Exception(oss.str());
            }
        }
      else
        {
          std::ostringstream oss; oss << "ForEachLoopPassedData::checkCompatibilyWithNb : Presence of id " << *it << " in list of ids ! Must be in [0," <<  nbOfElts << ") !";
          throw YACS::Exception(oss.str());
        }
    }
}

void ForEachLoopPassedData::checkLevel2(const std::vector<AnyInputPort *>& ports) const
{
  std::size_t sz(_nameOfOutputs.size());
  if(sz!=ports.size())
    throw YACS::Exception("ForEachLoopPassedData::checkLevel2 : mismatch of size of vectors !");
  for(std::size_t i=0;i<sz;i++)
    {
      AnyInputPort *elt(ports[i]);
      if(!elt)
        throw YACS::Exception("ForEachLoopPassedData::checkLevel2 : presence of null instance !");
      if(_nameOfOutputs[i]!=elt->getName())
        {
          std::ostringstream oss; oss << "ForEachLoopPassedData::checkLevel2 : At pos #" << i << " the name is not OK !";
          throw YACS::Exception(oss.str());
        }
    }
}

/*!
 * performs local to abs id. Input \a localId refers to an id in all jobs to perform. Returned id refers to pos in whole output sequences.
 */
int ForEachLoopPassedData::toAbsId(int localId) const
{
  if(localId<0)
    throw YACS::Exception("ForEachLoopPassedData::toAbsId : local pos must be >= 0 !");
  int ret(0),curLocId(0);
  for(std::vector<bool>::const_iterator it=_flagsIds.begin();it!=_flagsIds.end();it++,ret++)
    {
      if(!*it)
        {
          if(localId==curLocId)
            return ret;
          curLocId++;
        }
    }
  throw YACS::Exception("ForEachLoopPassedData::toAbsId : not referenced Id !");
}

/*!
 * Equivalent to toAbsId except that only ON are considered here.
 */
int ForEachLoopPassedData::toAbsIdNot(int localId) const
{
  if(localId<0)
    throw YACS::Exception("ForEachLoopPassedData::toAbsIdNot : local pos must be >= 0 !");
  int ret(0),curLocId(0);
  for(std::vector<bool>::const_iterator it=_flagsIds.begin();it!=_flagsIds.end();it++,ret++)
    {
      if(*it)//<- diff is here !
        {
          if(localId==curLocId)
            return ret;
          curLocId++;
        }
    }
  throw YACS::Exception("ForEachLoopPassedData::toAbsIdNot : not referenced Id !");
}

int ForEachLoopPassedData::getNumberOfElementsToDo() const
{
  std::size_t nbAllElts(_flagsIds.size());
  std::size_t ret(nbAllElts-_passedIds.size());
  return ret;
}

void ForEachLoopPassedData::assignAlreadyDone(const std::vector<SequenceAny *>& execVals) const
{
  std::size_t sz(execVals.size());
  if(_passedOutputs.size()!=sz)
    throw YACS::Exception("ForEachLoopPassedData::assignedAlreadyDone : mismatch of size of vectors !");
  for(std::size_t i=0;i<sz;i++)
    {
      SequenceAny *elt(_passedOutputs[i]);
      SequenceAny *eltDestination(execVals[i]);
      if(!elt)
        throw YACS::Exception("ForEachLoopPassedData::assignedAlreadyDone : presence of null elt !");
      unsigned int szOfElt(elt->size());
      for(unsigned int j=0;j<szOfElt;j++)
        {
          AnyPtr elt1((*elt)[j]);
          int jAbs(toAbsIdNot(j));
          eltDestination->setEltAtRank(jAbs,elt1);
        }
    }
}

ForEachLoopGen::ForEachLoopGen(const std::string& name, TypeCode *typeOfDataSplitted, std::unique_ptr<NbBranchesAbstract>&& branchManager):
                                                DynParaLoop(name,typeOfDataSplitted,std::move(branchManager)),
                                                _splitterNode(NAME_OF_SPLITTERNODE,typeOfDataSplitted,this),
                                                _execCurrentId(0),_nodeForSpecialCases(0),_currentIndex(0),_passedData(0)
{
}

ForEachLoopGen::ForEachLoopGen(const ForEachLoopGen& other, ComposedNode *father, bool editionOnly):DynParaLoop(other,father,editionOnly),
                                                                                                   _splitterNode(other._splitterNode,this),
                                                                                                   _execCurrentId(0),_nodeForSpecialCases(0),_currentIndex(0),_passedData(0)
{
  int i=0;
  if(!editionOnly)
    for(vector<AnySplitOutputPort *>::const_iterator iter2=other._outGoingPorts.begin();iter2!=other._outGoingPorts.end();iter2++,i++)
      {
        AnySplitOutputPort *temp=new AnySplitOutputPort(*(*iter2),this);
        InterceptorInputPort *interc=new InterceptorInputPort(*other._intecptrsForOutGoingPorts[i],this);
        temp->addRepr(getOutPort(other.getOutPortName((*iter2)->getRepr())),interc);
        interc->setRepr(temp);
        _outGoingPorts.push_back(temp);
        _intecptrsForOutGoingPorts.push_back(interc);
      }
}

ForEachLoopGen::~ForEachLoopGen()
{
  cleanDynGraph();
  for(vector<AnySplitOutputPort *>::iterator iter=_outGoingPorts.begin();iter!=_outGoingPorts.end();iter++)
    delete *iter;
  for(vector<InterceptorInputPort *>::iterator iter2=_intecptrsForOutGoingPorts.begin();iter2!=_intecptrsForOutGoingPorts.end();iter2++)
    delete *iter2;
  delete _passedData;
}

void ForEachLoopGen::init(bool start)
{
  DynParaLoop::init(start);
  _splitterNode.init(start);
  _execCurrentId=0;
  cleanDynGraph();
  _currentIndex = 0;
  exUpdateProgress();
  if(_passedData)
    _passedData->init();
}

void ForEachLoopGen::exUpdateState()
{
  DEBTRACE("ForEachLoopGen::exUpdateState");
  if(_state == YACS::DISABLED)
    return;
  if(_state == YACS::DONE)
    return;
  if(_inGate.exIsReady())
    {
      //internal graph update
      int i;
      int nbOfElts(_splitterNode.getNumberOfElements()),nbOfEltsDone(0);
      int nbOfBr(_nbOfBranches->getNumberOfBranches(nbOfElts));
      if(_passedData)
        {
          _passedData->checkCompatibilyWithNb(nbOfElts);
          nbOfEltsDone=_passedData->getNumberOfEltsAlreadyDone();
        }
      int nbOfEltsToDo(nbOfElts-nbOfEltsDone);

      DEBTRACE("nbOfElts=" << nbOfElts);
      DEBTRACE("nbOfBr=" << nbOfBr);
      INFO_YACSTRACE("nbOfBr (" << getName() << ") = " << nbOfBr);

      if(nbOfEltsToDo==0)
        {
          prepareSequenceValues(0);
          delete _nodeForSpecialCases;
          _nodeForSpecialCases=new FakeNodeForForEachLoop(this,true);
          setState(YACS::ACTIVATED);
          return ;
        }
      if(nbOfBr<=0)
        {
          delete _nodeForSpecialCases;
          _nodeForSpecialCases=new FakeNodeForForEachLoop(this,getAllOutPortsLeavingCurrentScope().empty());
          setState(YACS::ACTIVATED);
          return ;
        }
      if(nbOfBr>nbOfEltsToDo)
        nbOfBr=nbOfEltsToDo;
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
            }
          for(i=0;i<nbOfBr;i++)
            {
              DEBTRACE( "-------------- 1 " << i << " " << _execCurrentId);
              _execIds[i]=_execCurrentId;
              int posInAbs(_execCurrentId);
              if(_passedData)
                posInAbs=_passedData->toAbsId(_execCurrentId);
              _splitterNode.putSplittedValueOnRankTo(posInAbs,i,true);
              _execCurrentId++;
              DEBTRACE( "-------------- 7" );
            }
          if(_passedData)
            {
              _passedData->checkLevel2(_execOutGoingPorts[0]);
              _passedData->assignAlreadyDone(_execVals);
            }
          // clean inputs data coming from the outside in _node
          set< InPort * > portsToSetVals=getAllInPortsComingFromOutsideOfCurrentScope();
          for(auto iter : portsToSetVals)
            {
              InputPort *curPortCasted=(InputPort *) iter;//Cast granted by ForEachLoopGen::buildDelegateOf(InPort)
              if(!curPortCasted->canSafelySqueezeMemory())// this can appear strange ! if not safelySqueeze -> release. Nevertheless it is true.
                curPortCasted->releaseData();             // these input ports have been incremented with InputPort::put into DynParaLoop::prepareInputsFromOutOfScope. So they can be released now.
            }
        }
      catch(YACS::Exception& ex)
        {
          //ForEachLoop must be put in error and the exception rethrown to notify the caller
          DEBTRACE( "ForEachLoopGen::exUpdateState: " << ex.what() );
          setState(YACS::ERROR);
          setErrorDetails(ex.what());
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

void ForEachLoopGen::exUpdateProgress()
{
  // emit notification to all observers registered with the dispatcher on any change of the node's state
  sendEvent("progress");
}

void ForEachLoopGen::getReadyTasks(std::vector<Task *>& tasks)
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

int ForEachLoopGen::getNumberOfInputPorts() const
{
  return DynParaLoop::getNumberOfInputPorts()+1;
}

void ForEachLoopGen::checkNoCyclePassingThrough(Node *node)
{
  //TO DO
}

void ForEachLoopGen::selectRunnableTasks(std::vector<Task *>& tasks)
{
}

std::list<InputPort *> ForEachLoopGen::getSetOfInputPort() const
{
  list<InputPort *> ret=DynParaLoop::getSetOfInputPort();
  ret.push_back((InputPort *)&_splitterNode._dataPortToDispatch);
  return ret;
}

std::list<InputPort *> ForEachLoopGen::getLocalInputPorts() const
{
  list<InputPort *> ret=DynParaLoop::getLocalInputPorts();
  ret.push_back((InputPort *)&_splitterNode._dataPortToDispatch);
  return ret;
}

InputPort *ForEachLoopGen::getInputPort(const std::string& name) const
{
  if(name==SplitterNode::NAME_OF_SEQUENCE_INPUT)
    return (InputPort *)&_splitterNode._dataPortToDispatch;
  else
    return DynParaLoop::getInputPort(name);
}

OutputPort *ForEachLoopGen::getOutputPort(const std::string& name) const
{
  for(vector<AnySplitOutputPort *>::const_iterator iter=_outGoingPorts.begin();iter!=_outGoingPorts.end();iter++)
    {
      if(name==(*iter)->getName())
        return (OutputPort *)(*iter);
    }
  return DynParaLoop::getOutputPort(name);
}

OutPort *ForEachLoopGen::getOutPort(const std::string& name) const
{
  for(vector<AnySplitOutputPort *>::const_iterator iter=_outGoingPorts.begin();iter!=_outGoingPorts.end();iter++)
    {
      if(name==(*iter)->getName())
        return (OutPort *)(*iter);
    }
  return DynParaLoop::getOutPort(name);
}

Node *ForEachLoopGen::getChildByShortName(const std::string& name) const
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
YACS::Event ForEachLoopGen::updateStateOnFinishedEventFrom(Node *node)
{
  DEBTRACE("updateStateOnFinishedEventFrom " << node->getName() << " " << node->getState());
  unsigned int id;
  switch(getIdentityOfNotifyerNode(node,id))
    {
    case INIT_NODE:
      return updateStateForInitNodeOnFinishedEventFrom(node,id);
    case WORK_NODE:
      return updateStateForWorkNodeOnFinishedEventFrom(node,id,true);
    case FINALIZE_NODE:
      return updateStateForFinalizeNodeOnFinishedEventFrom(node,id);
    default:
      YASSERT(false);
    }
  return YACS::NOEVENT;
}

YACS::Event ForEachLoopGen::updateStateForInitNodeOnFinishedEventFrom(Node *node, unsigned int id)
{
  _execNodes[id]->exUpdateState();
  _nbOfEltConsumed++;
  _initializingCounter--;
  _currentIndex++;
  if (_initializingCounter == 0)
    _initNode->setState(DONE);
  return YACS::NOEVENT;
}

/*!
 * \param [in] isNormalFinish - if true
 */
YACS::Event ForEachLoopGen::updateStateForWorkNodeOnFinishedEventFrom(Node *node, unsigned int id, bool isNormalFinish)
{
  _currentIndex++;
  exUpdateProgress();
  if(isNormalFinish)
    {
      int globalId(_execIds[id]);
      if(_passedData)
        globalId=_passedData->toAbsId(globalId);
      sendEvent2("progress_ok",&globalId);
      storeOutValsInSeqForOutOfScopeUse(globalId,id);
    }
  else
    {
      int globalId(_execIds[id]);
      if(_passedData)
        globalId=_passedData->toAbsId(globalId);
      sendEvent2("progress_ko",&globalId);
    }
  //
  if(_execCurrentId==getFinishedId())
    {//No more elements of _dataPortToDispatch to treat
      _execIds[id]=NOT_RUNNING_BRANCH_ID;
      //analyzing if some samples are still on treatment on other branches.
      bool isFinished(true);
      for(int i=0;i<_execIds.size() && isFinished;i++)
        isFinished=(_execIds[i]==NOT_RUNNING_BRANCH_ID);
      if(isFinished)
        {
          try
          {
              if(_failedCounter!=0)
                {// case of keepgoing mode + a failed
                  std::ostringstream oss; oss << "Keep Going mode activated and some errors (" << _failedCounter << ")reported !";
                  DEBTRACE("ForEachLoopGen::updateStateOnFinishedEventFrom : "<< oss.str());
                  setState(YACS::FAILED);
                  return YACS::ABORT;
                }
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
              DEBTRACE("ForEachLoopGen::updateStateOnFinishedEventFrom: "<<ex.what());
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
      int posInAbs(_execCurrentId);
      if(_passedData)
        posInAbs=_passedData->toAbsId(_execCurrentId);
      _splitterNode.putSplittedValueOnRankTo(posInAbs,id,false);
      //forwardExecStateToOriginalBody(node);
      node->init(false);
      _execCurrentId++;
      node->exUpdateState();
      //forwardExecStateToOriginalBody(node);
      _nbOfEltConsumed++;
    }
  else
    {//elements to process and loop no more activated
      DEBTRACE("foreach loop state " << _state);
    }
  return YACS::NOEVENT;
}

YACS::Event ForEachLoopGen::updateStateForFinalizeNodeOnFinishedEventFrom(Node *node, unsigned int id)
{
  DEBTRACE("Finalize node finished on branch " << id);
  _unfinishedCounter--;
  _currentIndex++;
  exUpdateProgress();
  DEBTRACE(_unfinishedCounter << " finalize nodes still running");
  if (_unfinishedCounter == 0)
    {
      _finalizeNode->setState(YACS::DONE);
      setState(YACS::DONE);
      return YACS::FINISH;
    }
  else
    return YACS::NOEVENT;
}

YACS::Event ForEachLoopGen::updateStateOnFailedEventFrom(Node *node, const Executor *execInst)
{
  unsigned int id;
  DynParaLoop::TypeOfNode ton(getIdentityOfNotifyerNode(node,id));
  // TODO: deal with keepgoing without the dependency to Executor
  if(ton!=WORK_NODE || !execInst->getKeepGoingProperty())
    return DynParaLoop::updateStateOnFailedEventFrom(node,execInst);
  else
    {
      _failedCounter++;
      return updateStateForWorkNodeOnFinishedEventFrom(node,id,false);
    }
}

void ForEachLoopGen::InterceptorizeNameOfPort(std::string& portName)
{
  std::replace_if(portName.begin(), portName.end(), std::bind(std::equal_to<char>(), '.',std::placeholders::_1), '_');
  portName += INTERCEPTOR_STR;
}

void ForEachLoopGen::buildDelegateOf(std::pair<OutPort *, OutPort *>& port, InPort *finalTarget, const std::list<ComposedNode *>& pointsOfView)
{
  DynParaLoop::buildDelegateOf(port,finalTarget,pointsOfView);
  string typeOfPortInstance=(port.first)->getNameOfTypeOfCurrentInstance();
  if(typeOfPortInstance==OutputPort::NAME)
    {
      vector<AnySplitOutputPort *>::iterator iter=_outGoingPorts.begin();
      int i=0;
      for(;iter!=_outGoingPorts.end();iter++,i++)
        if((*iter)->getRepr()==port.first || *iter==port.first)
          break;
      if(iter!=_outGoingPorts.end())
        {
          if(*iter!=port.first)
            {
              (*iter)->incrRef();
              (*iter)->addRepr(port.first,_intecptrsForOutGoingPorts[i]);
            }
          port.first=*iter;
        }
      else
        {
          TypeCode *tcTrad((YACS::ENGINE::TypeCode*)finalTarget->edGetType()->subContentType(getFEDeltaBetween(port.first,finalTarget)));
          TypeCodeSeq *newTc=(TypeCodeSeq *)TypeCode::sequenceTc("","",tcTrad);
          // The out going ports belong to the ForEachLoop, whereas
          // the delegated port belongs to a node child of the ForEachLoop.
          // The name of the delegated port contains dots (bloc.node.outport),
          // whereas the name of the out going port shouldn't do.
          std::string outputPortName(getPortName(port.first));
          InterceptorizeNameOfPort(outputPortName);
          AnySplitOutputPort *newPort(new AnySplitOutputPort(outputPortName,this,newTc));
          InterceptorInputPort *intercptor(new InterceptorInputPort(outputPortName + "_in",this,tcTrad));
          intercptor->setRepr(newPort);
          newTc->decrRef();
          newPort->addRepr(port.first,intercptor);
          _outGoingPorts.push_back(newPort);
          _intecptrsForOutGoingPorts.push_back(intercptor);
          port.first=newPort;
        }
    }
  else
    throw Exception("ForEachLoopGen::buildDelegateOf : not implemented for DS because not specified");
}

void ForEachLoopGen::getDelegateOf(std::pair<OutPort *, OutPort *>& port, InPort *finalTarget, const std::list<ComposedNode *>& pointsOfView)
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
          string what("ForEachLoopGen::getDelegateOf : Port with name "); what+=port.first->getName(); what+=" not exported by ForEachLoop "; what+=_name; 
          throw Exception(what);
        }
      else
        port.first=(*iter);
    }
  else
    throw Exception("ForEachLoopGen::getDelegateOf : not implemented because not specified");
}

void ForEachLoopGen::releaseDelegateOf(OutPort *portDwn, OutPort *portUp, InPort *finalTarget, const std::list<ComposedNode *>& pointsOfView)
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

OutPort *ForEachLoopGen::getDynOutPortByAbsName(int branchNb, const std::string& name)
{
  string portName, nodeName;
  splitNamesBySep(name,Node::SEP_CHAR_IN_PORT,nodeName,portName,false);
  Node *staticChild = getChildByName(nodeName);
  return _execNodes[branchNb]->getOutPort(portName);//It's impossible(garanteed by YACS::ENGINE::ForEachLoopGen::buildDelegateOf)
  //that a link starting from _initNode goes out of scope of 'this'.
}

void ForEachLoopGen::cleanDynGraph()
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

void ForEachLoopGen::storeOutValsInSeqForOutOfScopeUse(int rank, int branchNb)
{
  vector<AnyInputPort *>::iterator iter;
  int i=0;
  for(iter=_execOutGoingPorts[branchNb].begin();iter!=_execOutGoingPorts[branchNb].end();iter++,i++)
    {
      Any *val=(Any *)(*iter)->getValue();
      _execVals[i]->setEltAtRank(rank,val);
    }
}

int ForEachLoopGen::getFinishedId()
{
  if(!_passedData)
    return _splitterNode.getNumberOfElements();
  else
    return _passedData->getNumberOfElementsToDo();
}

void ForEachLoopGen::prepareSequenceValues(int sizeOfSamples)
{
  _execVals.resize(_outGoingPorts.size());
  vector<AnySplitOutputPort *>::iterator iter=_outGoingPorts.begin();
  for(int i=0;iter!=_outGoingPorts.end();iter++,i++)
    _execVals[i]=SequenceAny::New((*iter)->edGetType()->contentType(),sizeOfSamples);
}

void ForEachLoopGen::pushAllSequenceValues()
{
  vector<AnySplitOutputPort *>::iterator iter=_outGoingPorts.begin();
  int i=0;
  for(;iter!=_outGoingPorts.end();iter++,i++)
    (*iter)->put((const void *)_execVals[i]);
}

void ForEachLoopGen::createOutputOutOfScopeInterceptors(int branchNb)
{
  vector<AnySplitOutputPort *>::iterator iter=_outGoingPorts.begin();
  int i=0;
  for(;iter!=_outGoingPorts.end();iter++,i++)
    {
      DEBTRACE( (*iter)->getName() << " " << (*iter)->edGetType()->kind() );
      //AnyInputPort *interceptor=new AnyInputPort((*iter)->getName(),this,(*iter)->edGetType());
      OutPort *portOut=getDynOutPortByAbsName(branchNb,getOutPortName(((*iter)->getRepr())));
      DEBTRACE( portOut->getName() );
      TypeCode *tc((TypeCode *)(*iter)->edGetType()->contentType());
      AnyInputPort *interceptor=new AnyInputPort((*iter)->getName(),this,tc);
      portOut->addInPort(interceptor);
      _execOutGoingPorts[branchNb].push_back(interceptor);
    }
}

void ForEachLoopGen::checkLinkPossibility(OutPort *start, const std::list<ComposedNode *>& pointsOfViewStart,
                                       InPort *end, const std::list<ComposedNode *>& pointsOfViewEnd)
{
  DynParaLoop::checkLinkPossibility(start, pointsOfViewStart, end, pointsOfViewEnd);
  if(end->getNode() == &_splitterNode)
    throw Exception("Illegal link within a foreach loop: \
the 'SmplsCollection' port cannot be linked within the scope of the loop.");
  if(end == _nbOfBranches->getPort())
    throw Exception("Illegal link within a foreach loop: \
the 'nbBranches' port cannot be linked within the scope of the loop.");
}

std::list<OutputPort *> ForEachLoopGen::getLocalOutputPorts() const
{
  list<OutputPort *> ret;
  ret.push_back(getOutputPort(NAME_OF_SPLITTED_SEQ_OUT)); 
  return ret;
}

//! Dump the node state to a stream
/*!
 * \param os : the output stream
 */
void ForEachLoopGen::writeDot(std::ostream &os) const
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

//! Reset the state of the node and its children depending on the parameter level
void ForEachLoopGen::resetState(int level)
{
  if(level==0)return;
  DynParaLoop::resetState(level);
  _execCurrentId=0;
  cleanDynGraph();
}

std::string ForEachLoopGen::getProgress() const
{
  int nbElems(getNbOfElementsToBeProcessed());
  std::stringstream aProgress;
  if (nbElems > 0)
    aProgress << _currentIndex << "/" << nbElems;
  else
    aProgress << "0";
  return aProgress.str();
}

//! Get the progress weight for all elementary nodes
/*!
 * Only elementary nodes have weight. For each node in the loop, the weight done is multiplied
 * by the number of elements done and the weight total by the number total of elements
 */
list<ProgressWeight> ForEachLoopGen::getProgressWeight() const
{
  list<ProgressWeight> ret;
  list<Node *> setOfNode=edGetDirectDescendants();
  int elemDone=getCurrentIndex();
  int elemTotal=getNbOfElementsToBeProcessed();
  for(list<Node *>::const_iterator iter=setOfNode.begin();iter!=setOfNode.end();iter++)
    {
      list<ProgressWeight> myCurrentSet=(*iter)->getProgressWeight();
      for(list<ProgressWeight>::iterator iter=myCurrentSet.begin();iter!=myCurrentSet.end();iter++)
        {
          (*iter).weightDone=((*iter).weightTotal) * elemDone;
          (*iter).weightTotal*=elemTotal;
        }
      ret.insert(ret.end(),myCurrentSet.begin(),myCurrentSet.end());
    }
  return ret;
}

int ForEachLoopGen::getNbOfElementsToBeProcessed() const
{
  int nbOfElems(_splitterNode.getNumberOfElements());
  int nbBranches = _nbOfBranches->getNumberOfBranches(nbOfElems);
  return nbOfElems
         + (_initNode ? nbBranches:0)
         + (_finalizeNode ? nbBranches:0) ;
}

/*!
 * This method allows to retrieve the state of \a this during execution or after. This method works even if this is \b NOT complete, or during execution or after a failure in \a this.
 * The typical usage of this method is to retrieve the results of items that passed successfully to avoid to lose all of them if only one fails.
 * This method has one input \a execut and 3 outputs.
 *
 * \param [in] execut - The single input is for threadsafety reasons because this method can be called safely during the execution of \a this.
 * \param [out] outputs - For each output ports in \a this linked with nodes sharing the same father than \a this the passed results are stored.
 *                        All of the items in \a outputs have the same size.
 * \param [out] nameOfOutputs - The array with same size than \a outputs, that tells for each item in outputs the output port it refers to.
 * \return the list of ids among \c this->edGetSeqOfSamplesPort() that run successfully. The length of this returned array will be the length of all
 *         SequenceAny objects contained in \a outputs.
 *
 * \sa edGetSeqOfSamplesPort
 */
std::vector<unsigned int> ForEachLoopGen::getPassedResults(Executor *execut, std::vector<SequenceAny *>& outputs, std::vector<std::string>& nameOfOutputs) const
{
  YACS::BASES::AutoLocker<YACS::BASES::Mutex> alck(&(execut->getTheMutexForSchedulerUpdate()));
  if(_execVals.empty())
    return std::vector<unsigned int>();
  if(_execOutGoingPorts.empty())
    return std::vector<unsigned int>();
  std::size_t sz(_execVals.size());
  outputs.resize(sz);
  nameOfOutputs.resize(sz);
  const std::vector<AnyInputPort *>& ports(_execOutGoingPorts[0]);
  for(std::size_t i=0;i<sz;i++)
    {
      outputs[i]=_execVals[i]->removeUnsetItemsFromThis();
      nameOfOutputs[i]=ports[i]->getName();
    }
  return _execVals[0]->getSetItems();
}

/*!
 * This method is typically useful for post-mortem relaunch to avoid to recompute already passed cases. This method takes in input exactly the parameters retrieved by
 * getPassedResults method.
 */
void ForEachLoopGen::assignPassedResults(const std::vector<unsigned int>& passedIds, const std::vector<SequenceAny *>& passedOutputs, const std::vector<std::string>& nameOfOutputs)
{
  delete _passedData;
  _failedCounter=0;
  _passedData=new ForEachLoopPassedData(passedIds,passedOutputs,nameOfOutputs);
}

int ForEachLoopGen::getFEDeltaBetween(OutPort *start, InPort *end)
{
  Node *ns(start->getNode()),*ne(end->getNode());
  ComposedNode *co(getLowestCommonAncestor(ns,ne));
  int ret(0);
  Node *work(ns);
  while(work!=co)
    {
      ForEachLoopGen *isFE(dynamic_cast<ForEachLoopGen *>(work));
      if(isFE)
        ret++;
      work=work->getFather();
    }
  if(dynamic_cast<AnySplitOutputPort *>(start))
    ret--;
  return ret;
}

/*!
 * This method is used to obtain the values already processed by the ForEachLoop.
 * A new ForEachLoopPassedData object is returned. You have to delete it.
 */
ForEachLoopPassedData* ForEachLoopGen::getProcessedData()const
{
  std::vector<SequenceAny *> outputs;
  std::vector<std::string> nameOfOutputs;
  if(_execVals.empty() || _execOutGoingPorts.empty())
    return new ForEachLoopPassedData(std::vector<unsigned int>(), outputs, nameOfOutputs);
  std::size_t sz(_execVals.size());
  outputs.resize(sz);
  nameOfOutputs.resize(sz);
  const std::vector<AnyInputPort *>& ports(_execOutGoingPorts[0]);
  for(std::size_t i=0;i<sz;i++)
    {
      outputs[i]=_execVals[i]->removeUnsetItemsFromThis();
      nameOfOutputs[i]=ports[i]->getName();
    }
  return new ForEachLoopPassedData(_execVals[0]->getSetItems(), outputs, nameOfOutputs);
}

void ForEachLoopGen::setProcessedData(ForEachLoopPassedData* processedData)
{
  if(_passedData)
    delete _passedData;
  _passedData = processedData;
}

/*!
 * \param portName : "interceptorized" name of port.
 */
const YACS::ENGINE::TypeCode* ForEachLoopGen::getOutputPortType(const std::string& portName)const
{
  const YACS::ENGINE::TypeCode* ret=NULL;
  vector<AnySplitOutputPort *>::const_iterator it;
  for(it=_outGoingPorts.begin();it!=_outGoingPorts.end() && ret==NULL;it++)
  {
    std::string originalPortName(getPortName(*it));
    //InterceptorizeNameOfPort(originalPortName);
    DEBTRACE("ForEachLoopGen::getOutputPortType compare " << portName << " == " << originalPortName);
    if(originalPortName == portName)
    {
      ret = (*it)->edGetType()->contentType();
    }
  }
  return ret;
}

Node *ForEachLoop::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new ForEachLoop(*this,father,editionOnly);
}

void ForEachLoop::accept(Visitor *visitor)
{
  visitor->visitForEachLoop(this);
}

void ForEachLoopDyn::accept(Visitor *visitor)
{
  visitor->visitForEachLoopDyn(this);
}

Node *ForEachLoopDyn::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new ForEachLoopDyn(*this,father,editionOnly);
}
