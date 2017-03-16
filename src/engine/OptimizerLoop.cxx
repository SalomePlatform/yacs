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

#include "OptimizerLoop.hxx"
#include "OutputPort.hxx"
#include "Visitor.hxx"

#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

const char FakeNodeForOptimizerLoop::NAME[]="thisIsAFakeNode";

const int OptimizerLoop::NOT_RUNNING_BRANCH_ID=-1973012217;
const int OptimizerLoop::NOT_INITIALIZED_BRANCH_ID=-1973;

const char OptimizerLoop::NAME_OF_ALGO_INIT_PORT[] = "algoInit";
const char OptimizerLoop::NAME_OF_OUT_POOL_INPUT[] = "evalResults";
const char OptimizerLoop::NAME_OF_ALGO_RESULT_PORT[] = "algoResults";


FakeNodeForOptimizerLoop::FakeNodeForOptimizerLoop(OptimizerLoop *loop, bool normal, std::string message)
  : ElementaryNode(NAME), _loop(loop), _normal(normal), _message(message)
{
  _state=YACS::TOACTIVATE;
  _father=_loop->getFather();
}

FakeNodeForOptimizerLoop::FakeNodeForOptimizerLoop(const FakeNodeForOptimizerLoop& other)
  : ElementaryNode(other), _loop(0), _normal(other._normal), _message(other._message)
{ 
}

Node *FakeNodeForOptimizerLoop::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new FakeNodeForOptimizerLoop(*this);
}

void FakeNodeForOptimizerLoop::exForwardFailed()
{
  _loop->exForwardFailed();
}

void FakeNodeForOptimizerLoop::exForwardFinished()
{
  _loop->exForwardFinished();
}

void FakeNodeForOptimizerLoop::execute()
{
  DEBTRACE("FakeNodeForOptimizerLoop::execute: " << _message)
  if (!_normal) {
    _loop->_errorDetails = _message;
    throw Exception(_message);
  }
  else
  {
    _loop->_algoResultPort.put(_loop->_alg->getAlgoResultProxy());
  }
}

void FakeNodeForOptimizerLoop::aborted()
{
  _loop->setState(YACS::ERROR);
}

void FakeNodeForOptimizerLoop::finished()
{
  _loop->setState(YACS::DONE);
}

/*! \class YACS::ENGINE::OptimizerLoop
 *  \brief class to build optimization loops
 *
 * \ingroup ComposedNodes
 */

OptimizerLoop::OptimizerLoop(const std::string& name, const std::string& algLibWthOutExt,
                             const std::string& symbolNameToOptimizerAlgBaseInstanceFactory,
                             bool algInitOnFile,bool initAlgo, Proc * procForTypes):
        DynParaLoop(name,Runtime::_tc_string),_algInitOnFile(algInitOnFile),_alglib(algLibWthOutExt),
        _algoInitPort(NAME_OF_ALGO_INIT_PORT, this, Runtime::_tc_string, true),
        _loader(NULL),_alg(0),_convergenceReachedWithOtherCalc(false),
        _retPortForOutPool(NAME_OF_OUT_POOL_INPUT,this,Runtime::_tc_string),
        _nodeForSpecialCases(0), _algoResultPort(NAME_OF_ALGO_RESULT_PORT, this, Runtime::_tc_string)
{
  //We need this because calling a virtual method in a constructor does not call the most derived method but the method of the class
  //A derived class must take care to manage that 
  if(initAlgo)
    setAlgorithm(algLibWthOutExt,symbolNameToOptimizerAlgBaseInstanceFactory, procForTypes);
}

OptimizerLoop::OptimizerLoop(const OptimizerLoop& other, ComposedNode *father, bool editionOnly):
  DynParaLoop(other,father,editionOnly),_algInitOnFile(other._algInitOnFile),_alglib(other._alglib),
  _convergenceReachedWithOtherCalc(false),_loader(NULL),_alg(0),_algoInitPort(other._algoInitPort,this),
  _retPortForOutPool(other._retPortForOutPool,this),_nodeForSpecialCases(0),
  _algoResultPort(other._algoResultPort, this)
{
  //Don't call setAlgorithm here because it will be called several times if the class is derived. Call it in simpleClone for cloning
  
  // Create the links to evalResults port
  set<OutPort *> fromPortsToReproduce=other._retPortForOutPool.edSetOutPort();
  for(set<OutPort *>::iterator iter=fromPortsToReproduce.begin();iter!=fromPortsToReproduce.end();iter++)
    edAddLink(getOutPort(other.getPortName(*iter)),&_retPortForOutPool);
}

OptimizerLoop::~OptimizerLoop()
{
  if(_alg)
    _alg->decrRef();
  cleanDynGraph();
  cleanInterceptors();
  delete _loader;
  delete _nodeForSpecialCases;
}

Node *OptimizerLoop::simpleClone(ComposedNode *father, bool editionOnly) const
{
  OptimizerLoop* ol=new OptimizerLoop(*this,father,editionOnly);
  // TODO: Remove this const_cast (find a better design to get the type codes from the original node)
  Proc * procForTypes = ol->getProc();
  if (procForTypes == NULL) {
    const Proc * origProc = getProc();
    procForTypes = const_cast<Proc *>(origProc);
  }
  ol->setAlgorithm(_alglib, _symbol, false, procForTypes);
  return ol;
}

void OptimizerLoop::init(bool start)
{
  DynParaLoop::init(start);
  _algoInitPort.exInit(start);
  _retPortForOutPool.exInit(start);
  _algoResultPort.exInit();
  _convergenceReachedWithOtherCalc=false;
  cleanDynGraph();
  cleanInterceptors();
}

void OptimizerLoop::exUpdateState()
{
  if(_state == YACS::DISABLED)
    return;
  delete _nodeForSpecialCases;
  _nodeForSpecialCases = NULL;
  try
    {
      if(_inGate.exIsReady())
        {
          setState(YACS::TOACTIVATE);
          // Force termination in case the previous algorithm did not finish properly (manual stop)
          _alg->finishProxy();
          _myPool.destroyAll();

          // Initialize and launch the algorithm
          _alg->initializeProxy(_algoInitPort.getValue());
          if (_alg->hasError()) {
            string error = _alg->getError();
            _alg->finishProxy();
            throw Exception(error);
          }

          //internal graph update
          int i;
          int nbOfBr=_nbOfBranches.getIntValue();
          _alg->setNbOfBranches(nbOfBr);

          _alg->startProxy();
          if (_alg->hasError()) {
            string error = _alg->getError();
            _alg->finishProxy();
            throw Exception(error);
          }

          if(nbOfBr==0)
            {
              // A number of branches of 0 is acceptable if there are no output ports
              // leaving OptimizerLoop
              bool normal = getAllOutPortsLeavingCurrentScope().empty();
              _nodeForSpecialCases = new FakeNodeForOptimizerLoop(this, normal,
                  "OptimizerLoop has no branch to run the internal node(s)");
              return;
            }
          _execNodes.resize(nbOfBr);
          _execIds.resize(nbOfBr);
          if(_initNode)
            {
              _execInitNodes.resize(nbOfBr);
              _initNodeUpdated.resize(nbOfBr);
              for(i=0;i<nbOfBr;i++)
                _initNodeUpdated[i]=false;
            }
          _initializingCounter = 0;
          if (_finalizeNode)
            _execFinalizeNodes.resize(nbOfBr);
          vector<Node *> origNodes;
          origNodes.push_back(_initNode);
          origNodes.push_back(_node);
          origNodes.push_back(_finalizeNode);
          for(i=0;i<nbOfBr;i++)
            {
              _execIds[i]=NOT_INITIALIZED_BRANCH_ID;
              vector<Node *> clonedNodes = cloneAndPlaceNodesCoherently(origNodes);
              if(_initNode)
                _execInitNodes[i] = clonedNodes[0];
              _execNodes[i] = clonedNodes[1];
              if(_finalizeNode)
                _execFinalizeNodes[i] = clonedNodes[2];
              prepareInputsFromOutOfScope(i);
            }
          initInterceptors(nbOfBr);
          int id;
          unsigned char priority;
          Any *val=_myPool.getNextSampleWithHighestPriority(id,priority);
          if(!val)
            {
              // It is acceptable to have no sample to launch if there are no output ports
              // leaving OptimizerLoop
              std::set<OutPort *> setOutPort = getAllOutPortsLeavingCurrentScope();
              // Special in the special
              // We do not check algoResult
              setOutPort.erase(&_algoResultPort);
              bool normal = setOutPort.empty();
              _nodeForSpecialCases = new FakeNodeForOptimizerLoop(this, normal,
                  string("The algorithm of OptimizerLoop with name ") + _name +
                  " returns no sample to launch");
              return;
            }
          launchMaxOfSamples(true);
        }
    }
  catch (const exception & e)
    {
      _nodeForSpecialCases = new FakeNodeForOptimizerLoop(this, false,
          string("An error happened in the control algorithm of OptimizerLoop \"") + _name +
          "\": " + e.what());
    }
}

int OptimizerLoop::getNumberOfInputPorts() const
{
  return DynParaLoop::getNumberOfInputPorts()+2;
}

InputPort *OptimizerLoop::getInputPort(const std::string& name) const throw(YACS::Exception)
{
  if (name == NAME_OF_ALGO_INIT_PORT)
    return (InputPort *)&_algoInitPort;
  else if (name == NAME_OF_OUT_POOL_INPUT)
    return (InputPort *)&_retPortForOutPool;
  else
    return DynParaLoop::getInputPort(name);
}

std::list<InputPort *> OptimizerLoop::getSetOfInputPort() const
{
  list<InputPort *> ret=DynParaLoop::getSetOfInputPort();
  ret.push_back((InputPort *)&_algoInitPort);
  ret.push_back((InputPort *)&_retPortForOutPool);
  return ret;
}

std::list<InputPort *> OptimizerLoop::getLocalInputPorts() const
{
  list<InputPort *> ret=DynParaLoop::getLocalInputPorts();
  ret.push_back((InputPort *)&_algoInitPort);
  ret.push_back((InputPort *)&_retPortForOutPool);
  return ret;
}

void OptimizerLoop::selectRunnableTasks(std::vector<Task *>& tasks)
{
}

void OptimizerLoop::getReadyTasks(std::vector<Task *>& tasks)
{
  if(!_node)
    return;
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

YACS::Event OptimizerLoop::updateStateOnFinishedEventFrom(Node *node)
{
  if (getState() == YACS::FAILED)
    {
      // This happens when a valid computation on a branch finishes after an error on another branch.
      // In this case we just ignore the new result because the algorithm has already been terminated.
      return YACS::NOEVENT;
    }
  unsigned int id;
  switch(getIdentityOfNotifyerNode(node,id))
    {
    case INIT_NODE:
    {
      _execNodes[id]->exUpdateState();
      _nbOfEltConsumed++;
      _initializingCounter--;
      if (_initializingCounter == 0) _initNode->setState(DONE);
      break;
    }
    case WORK_NODE:
    {
      if(_convergenceReachedWithOtherCalc)
        { //This case happens when alg has reached its convergence whereas other calculations still compute
          _execIds[id]=NOT_RUNNING_BRANCH_ID;
          if(!isFullyLazy())
            return YACS::NOEVENT;
          else
            return finalize();
        }
      _myPool.putOutSampleAt(_execIds[id],_interceptorsForOutPool[id]->getValue());
      _myPool.setCurrentId(_execIds[id]);
      _alg->takeDecisionProxy();
      if (_alg->hasError()) {
        _errorDetails = string("An error happened in the control algorithm of optimizer loop: ") +
                        _alg->getError();
        _alg->finishProxy();
        setState(YACS::FAILED);
        return YACS::ABORT;
      }

      _myPool.destroyCurrentCase();
      if(_myPool.empty())
        {
          pushValueOutOfScopeForCase(id);
          _execIds[id]=NOT_RUNNING_BRANCH_ID;
          if(!isFullyLazy())
            {// This case happens when the hand is returned to continue, whereas some other are working in parallel for nothing.
              _convergenceReachedWithOtherCalc=true;
              return YACS::NOEVENT;
            }
          return finalize();
        }
      _execIds[id]=NOT_RUNNING_BRANCH_ID;
      int newId;
      unsigned char priority;
      Any *val=_myPool.getNextSampleWithHighestPriority(newId, priority);
      if(!val)
        {
          bool isFinished=true;
          for(int i=0;i<_execIds.size() && isFinished;i++)
            isFinished=(_execIds[i]==NOT_RUNNING_BRANCH_ID || _execIds[i]==NOT_INITIALIZED_BRANCH_ID);
          if(isFinished)
            {
              std::cerr <<"OptimizerLoop::updateStateOnFinishedEventFrom: Alg has not inserted more cases whereas last element has been calculated !" << std::endl;
              setState(YACS::ERROR);
              exForwardFailed();
              _alg->finishProxy();
              return YACS::FINISH;
            }
          return YACS::NOEVENT;
        }
      launchMaxOfSamples(false);
      break;
    }
    case FINALIZE_NODE:
    {
      _unfinishedCounter--;
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

YACS::Event OptimizerLoop::finalize()
{
  //update internal node (definition node) state
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
  _alg->finishProxy();
  _algoResultPort.put(_alg->getAlgoResultProxy());
  if (_finalizeNode == NULL)
    {
      // No finalize node, we just finish OptimizerLoop at the end of exec nodes execution
      setState(YACS::DONE);
      return YACS::FINISH;
    }
  else
    {
      // Run the finalize nodes, the OptimizerLoop will be done only when they all finish
      _unfinishedCounter = 0;  // This counter indicates how many branches are not finished
      for (int i=0 ; i<_nbOfBranches.getIntValue() ; i++)
        if (_execIds[i] == NOT_RUNNING_BRANCH_ID)
          {
            DEBTRACE("Launching finalize node for branch " << i)
            _execFinalizeNodes[i]->exUpdateState();
            _unfinishedCounter++;
          }
        else
          // There should not be any running branch at this point
          YASSERT(_execIds[i] == NOT_INITIALIZED_BRANCH_ID)
      return YACS::NOEVENT;
    }
}

//! Method used to notify the node that a child node has failed
/*!
 * Notify the slave thread of the error, update the current state and
 * return the change state
 *
 *  \param node : the child node that has failed
 *  \return the state change
 */
YACS::Event OptimizerLoop::updateStateOnFailedEventFrom(Node *node, const Executor *execInst)
{
  DEBTRACE("OptimizerLoop::updateStateOnFailedEventFrom " << node->getName());
  _alg->setError(string("Error during the execution of YACS node ") + node->getName() +
                 ": " + node->getErrorReport());
  _alg->finishProxy();
  _myPool.destroyAll();
  DEBTRACE("OptimizerLoop::updateStateOnFailedEventFrom: returned from error notification.");
  return DynParaLoop::updateStateOnFailedEventFrom(node,execInst);
}

void OptimizerLoop::checkNoCyclePassingThrough(Node *node) throw(YACS::Exception)
{
}

void OptimizerLoop::buildDelegateOf(InPort * & port, OutPort *initialStart, const std::list<ComposedNode *>& pointsOfView)
{
  DynParaLoop::buildDelegateOf(port,initialStart,pointsOfView);
  if(port==&_retPortForOutPool)
  {
    std::string linkName("(");
    linkName += initialStart->getName()+" to "+port->getName()+")";
    throw Exception(std::string("Illegal OptimizerLoop link: \
The 'evalResults' port must be linked within the scope of the loop.")
                    + linkName);
  }
}

void OptimizerLoop::buildDelegateOf(std::pair<OutPort *, OutPort *>& port, InPort *finalTarget, const std::list<ComposedNode *>& pointsOfView)
{
  DynParaLoop::buildDelegateOf(port,finalTarget,pointsOfView);
  if(port.first != &_algoResultPort)
  {
    std::string linkName("(");
    linkName += port.first->getName()+" to "+finalTarget->getName()+")";
    throw Exception(std::string("Illegal OptimizerLoop link: \
Only the algorithm result port can be linked to a port outside the scope of the loop.")
                    + linkName);
  }

  string typeOfPortInstance=(port.first)->getNameOfTypeOfCurrentInstance();
  if(typeOfPortInstance!=OutputPort::NAME)
    throw Exception("OptimizerLoop::buildDelegateOf : not implemented for DS because not specified ");
}

void OptimizerLoop::checkControlDependancy(OutPort *start, InPort *end, bool cross,
                                           std::map < ComposedNode *,  std::list < OutPort * >, SortHierarc >& fw,
                                           std::vector<OutPort *>& fwCross,
                                           std::map< ComposedNode *, std::list < OutPort *>, SortHierarc >& bw,
                                           LinkInfo& info) const
{
  if(end==&_retPortForOutPool)
    fw[(ComposedNode *)this].push_back(start);
  else
    DynParaLoop::checkControlDependancy(start,end,cross,fw,fwCross,bw,info);
}

void OptimizerLoop::checkCFLinks(const std::list<OutPort *>& starts, InputPort *end, unsigned char& alreadyFed, bool direction, LinkInfo& info) const
{
  if(end==&_retPortForOutPool)
    solveObviousOrDelegateCFLinks(starts,end,alreadyFed,direction,info);
  else
    DynParaLoop::checkCFLinks(starts,end,alreadyFed,direction,info);
}

void OptimizerLoop::checkLinkPossibility(OutPort *start, const std::list<ComposedNode *>& pointsOfViewStart,
                          InPort *end, const std::list<ComposedNode *>& pointsOfViewEnd) throw(YACS::Exception)
{
  DynParaLoop::checkLinkPossibility(start, pointsOfViewStart, end, pointsOfViewEnd);
  std::string linkName("(");
  linkName += start->getName()+" to "+end->getName()+")";

  // Yes, it should be possible to link back the result port to any input port of the loop.
  if(end == &_nbOfBranches || end == &_algoInitPort)
    if(start != &_algoResultPort)
      throw Exception(std::string("Illegal OptimizerLoop link.") + linkName);
    else
      return;

  if(start == &_algoResultPort)
    throw Exception(std::string("Illegal OptimizerLoop link: \
The 'algoResult' port can't be linked within the scope of the loop.") + linkName);
  
  if(end == &_retPortForOutPool && isInMyDescendance(start->getNode())!=_node)
    throw Exception(std::string("Illegal OptimizerLoop link: \
The 'evalResults' port can only be linked to the working node.") + linkName);
}

void OptimizerLoop::cleanInterceptors()
{
  // At this point all garanties taken let's clean all.
  map<InputPort *,vector<InputPort *> >::iterator iter=_interceptors.begin();
  for(;iter!=_interceptors.end();iter++)
    for(vector<InputPort *>::iterator iter2=(*iter).second.begin();iter2!=(*iter).second.end();iter2++)
      delete (*iter2);
  _interceptors.clear();
  for(vector<AnyInputPort *>::iterator iter3=_interceptorsForOutPool.begin();iter3!=_interceptorsForOutPool.end();iter3++)
    delete (*iter3);
  _interceptorsForOutPool.clear();
}

void OptimizerLoop::launchMaxOfSamples(bool first)
{
  int id;
  unsigned char priority;
  Any *val;
  unsigned i;
  for (val = _myPool.getNextSampleWithHighestPriority(id, priority);
       !isFullyBusy(i) && val;
       val = _myPool.getNextSampleWithHighestPriority(id, priority))
    {
      if(_execIds[i] == NOT_INITIALIZED_BRANCH_ID)
        first=true; // node is not initialized (first pass)
      else
        first=false; // node is initialized (second pass)
      _execIds[i]=id;
      _myPool.markIdAsInUse(id);
      if(_initNode && !_initNodeUpdated[i])
        {
          putValueOnBranch(val,i,first);
          _execInitNodes[i]->exUpdateState();
          _initNodeUpdated[i]=true;
          _initializingCounter++;
        }
      else
        {
          if(!first)
            _execNodes[i]->init(first);
          putValueOnBranch(val,i,first);
          _execNodes[i]->exUpdateState();
          _nbOfEltConsumed++;
        }
    }
}

bool OptimizerLoop::isFullyLazy() const
{
  bool isLazy=true;
  for(unsigned i=0;i<_execIds.size() && isLazy;i++)
    isLazy=(_execIds[i]==NOT_RUNNING_BRANCH_ID || _execIds[i]==NOT_INITIALIZED_BRANCH_ID);
  return isLazy;
}

/*!
 * Returns if a dynamic branch is available.
 * \param branchId Out param. Only usable if returned value is equal to \b false.
 */
bool OptimizerLoop::isFullyBusy(unsigned& branchId) const
{
  bool isFinished=true;
  unsigned i;
  for(i=0;i<_execIds.size() && isFinished;i++)
    isFinished=(_execIds[i]!=NOT_RUNNING_BRANCH_ID && _execIds[i]!=NOT_INITIALIZED_BRANCH_ID);
  if(!isFinished)
    branchId=i-1;
  return isFinished;
}

/*!
 * Perform initialization of interceptors. \b WARNING _execNodes have to be created before.
 */
void OptimizerLoop::initInterceptors(unsigned nbOfBr)
{
  //For all classical outputports leaving 'this'
  set<OutPort *> portsToIntercept=getAllOutPortsLeavingCurrentScope();
  portsToIntercept.erase(&_algoResultPort);
  for(set<OutPort *>::iterator iter=portsToIntercept.begin();iter!=portsToIntercept.end();iter++)
    {
      OutputPort *portC=(OutputPort *)(*iter);//Warrantied by OptimizerLoop::buildDelegateOf
      const set<InputPort *>& links=portC->getSetOfPhyLinks();
      for(set<InputPort *>::const_iterator iter2=links.begin();iter2!=links.end();iter2++)
        {
#ifdef NOCOVARIANT
          InputPort *reprCur=dynamic_cast<InputPort *>((*iter2)->getPublicRepresentant());
#else
          InputPort *reprCur=(*iter2)->getPublicRepresentant();
#endif
          if(!isInMyDescendance(reprCur->getNode()))
            {//here we've got an out of scope link : Let's intercept it
              if(_interceptors.find(reprCur)==_interceptors.end())
                {
                  _interceptors[reprCur].resize(nbOfBr);
                  for(unsigned i=0;i<nbOfBr;i++)
                    {
                      OutputPort *portExecC=(OutputPort *)_execNodes[i]->getOutputPort(_node->getOutPortName(portC));
                      InputPort *clone=reprCur->clone(0);
                      _interceptors[reprCur][i]=clone;
                      portExecC->edAddInputPort(clone);
                    }
                }
              else
                {
                  for(unsigned i=0;i<nbOfBr;i++)
                    {
                      OutputPort *portExecC=(OutputPort *)_execNodes[i]->getOutputPort(_node->getOutPortName(portC));
                      portExecC->edAddInputPort(_interceptors[reprCur][i]);
                    }
                }
            }
        }
    }
  // For out pool
  _interceptorsForOutPool.resize(nbOfBr);
  set< OutPort * > links=_retPortForOutPool.edSetOutPort();
  for(unsigned i=0;i<nbOfBr;i++)
    _interceptorsForOutPool[i]=(AnyInputPort *)_retPortForOutPool.clone(this);
  for(set<OutPort *>::iterator iter2=links.begin();iter2!=links.end();iter2++)
    for(unsigned j=0;j<nbOfBr;j++)
      {
        OutPort *portExec;
        Node *whatType=isInMyDescendance((*iter2)->getNode());
        if(whatType==_node)
          {
            portExec=_execNodes[j]->getOutPort(_node->getOutPortName(*iter2));
            portExec->addInPort(_interceptorsForOutPool[j]);
          }
        else if(whatType==_initNode && whatType!=0)//This case should never happend. Useless !
          {
            portExec=_execInitNodes[j]->getOutPort(_node->getOutPortName(*iter2));
            portExec->addInPort(_interceptorsForOutPool[j]);
          }
      }
}

/*!
 * Typically called when _alg has decided that convergence has been reached. In this case the links leaving the current scope are activated and filled
 * with value of the branch specified by 'branchId' that is the branch in which the convergence has been reached.
 */
void OptimizerLoop::pushValueOutOfScopeForCase(unsigned branchId)
{
  map<InputPort *, std::vector<InputPort *> >::iterator iter;
  for(iter=_interceptors.begin();iter!=_interceptors.end();iter++)
    (*iter).first->put((*iter).second[branchId]->get());
}

void OptimizerLoop::accept(Visitor *visitor)
{
  visitor->visitOptimizerLoop(this);
}

//! Set the algorithm library name and factory name (symbol in library) to create the algorithm and change it if the node is not connected
/*!
 *   throw an exception if the node is connected
 */
void OptimizerLoop::setAlgorithm(const std::string& alglib, const std::string& symbol,
                                 bool checkLinks, Proc * procForTypes)
{
  if(checkLinks)
    {
      if (_splittedPort.edGetNumberOfOutLinks() != 0 ||
          _retPortForOutPool.edGetNumberOfLinks() != 0 ||
          _algoInitPort.edGetNumberOfLinks() != 0 ||
          _algoResultPort.edGetNumberOfOutLinks() != 0)
        throw Exception("The OptimizerLoop node must be disconnected before setting the algorithm");
    }

  _symbol = symbol;
  _alglib = alglib;

  if (_alg) {
    _alg->decrRef();
    _alg = NULL;
  }

  loadAlgorithm();

  if(_alg)
    {
      _alg->setProc((procForTypes == NULL) ? getProc() : procForTypes);

      // Delete the values in the input ports if they were initialized
      _retPortForOutPool.put((Any *)NULL);
      _algoInitPort.put((Any *)NULL);

      // Change the type of the ports
      _splittedPort.edSetType(checkTypeCode(_alg->getTCForInProxy(), NAME_OF_SPLITTED_SEQ_OUT));
      _retPortForOutPool.edSetType(checkTypeCode(_alg->getTCForOutProxy(), NAME_OF_OUT_POOL_INPUT));
      _algoInitPort.edSetType(checkTypeCode(_alg->getTCForAlgoInitProxy(), NAME_OF_ALGO_INIT_PORT));
      _algoResultPort.edSetType(checkTypeCode(_alg->getTCForAlgoResultProxy(), NAME_OF_ALGO_RESULT_PORT));
    }

  modified();
}

TypeCode * OptimizerLoop::checkTypeCode(TypeCode * tc, const char * portName)
{
  if (tc == NULL) {
    ostringstream errorMsg;
    errorMsg << "The algorithm specified for OptimizerLoop node \"" << getName() <<
                "\" provided an invalid type for port \"" << portName << "\"";
    throw Exception(errorMsg.str());
  }
  return tc;
}

//! Load the algorithm from the dynamic library
/*!
 *
 */
void OptimizerLoop::loadAlgorithm()
{
  YASSERT(_alg == NULL)

  if (_loader != NULL) {
    delete _loader;
    _loader = NULL;
  }
  _loader = new YACS::BASES::DynLibLoader(_alglib);
  OptimizerAlgBaseFactory algFactory = NULL;

  if (_alglib != "" && _symbol != "")
    {
      try
        {
          _errorDetails = "";
          algFactory = (OptimizerAlgBaseFactory)_loader->getHandleOnSymbolWithName(_symbol);
        }
      catch (YACS::Exception& e)
        {
          _errorDetails = e.what();
          modified();
          throw;
        }
    }

  if (algFactory != NULL)
    _alg = algFactory(&_myPool);
}

//! Return the name of the algorithm library
/*!
 *
 */
std::string OptimizerLoop::getAlgLib() const
{
  return _alglib;
}

//! Check validity for the node.
/*!
 *  Throw an exception if the node is not valid
 */
void OptimizerLoop::checkBasicConsistency() const throw(YACS::Exception)
{
  DEBTRACE("OptimizerLoop::checkBasicConsistency");
  if (_alglib == "")
    throw Exception("No library specified for the OptimizerLoop control algorithm");
  if (_symbol == "")
    throw Exception("No symbol specified for the OptimizerLoop control algorithm");
  if(_alg == NULL)
    throw YACS::Exception("Problem during library loading: "+_errorDetails);

  DynParaLoop::checkBasicConsistency();
}

int OptimizerLoop::getNumberOfOutputPorts() const
{
  return DynParaLoop::getNumberOfOutputPorts() + 1;
}

std::list<OutputPort *> OptimizerLoop::getSetOfOutputPort() const
{
  list<OutputPort *> ret = DynParaLoop::getSetOfOutputPort();
  ret.push_back((OutputPort *)&_algoResultPort);
  return ret;
}

std::list<OutputPort *> OptimizerLoop::getLocalOutputPorts() const
{
  list<OutputPort *> ret = DynParaLoop::getLocalOutputPorts();
  ret.push_front((OutputPort *)&_algoResultPort);
  return ret;
}

OutPort * OptimizerLoop::getOutPort(const std::string& name) const throw(YACS::Exception)
{
  return (name == NAME_OF_ALGO_RESULT_PORT) ? (OutPort *)&_algoResultPort :
                                              DynParaLoop::getOutPort(name);
}


OutputPort * OptimizerLoop::getOutputPort(const std::string& name) const throw(YACS::Exception)
{
  return (name == NAME_OF_ALGO_RESULT_PORT) ? (OutputPort *)&_algoResultPort :
                                              DynParaLoop::getOutputPort(name);
}
