//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
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

const char OptimizerLoop::NAME_OF_FILENAME_INPUT[]="FileNameInitAlg";

const char OptimizerLoop::NAME_OF_OUT_POOL_INPUT[]="retPortForOutPool";

OptimizerAlgStandardized::OptimizerAlgStandardized(Pool *pool, OptimizerAlgBase *alg)
  : OptimizerAlgSync(pool),_algBehind(alg),_threadInCaseOfNotEvent(0)
{
  if(_algBehind)
    _algBehind->incrRef();
}

OptimizerAlgStandardized::~OptimizerAlgStandardized()
{
  if(_algBehind)
    _algBehind->decrRef();
}

TypeCode *OptimizerAlgStandardized::getTCForIn() const
{
  return _algBehind->getTCForIn();
}

TypeCode *OptimizerAlgStandardized::getTCForOut() const
{
  return _algBehind->getTCForOut();
}

void OptimizerAlgStandardized::setAlgPointer(OptimizerAlgBaseFactory algFactory)
{
  if(_algBehind)
    _algBehind->decrRef();
  if(algFactory)
    _algBehind=algFactory(_pool);
  else
    _algBehind=0;
}

void OptimizerAlgStandardized::parseFileToInit(const std::string& fileName)
{
  _algBehind->parseFileToInit(fileName);
}

void OptimizerAlgStandardized::initialize(const Any *input) throw(YACS::Exception)
{
  _algBehind->initialize(input);
}

void OptimizerAlgStandardized::takeDecision()
{
  switch(_algBehind->getType())
    {
    case EVENT_ORIENTED:
      {
        ((OptimizerAlgSync *) _algBehind)->takeDecision();
        break;
      }
    case NOT_EVENT_ORIENTED:
      {   
        _condition.notifyOneSync();
        break;
      }
    default:
      throw Exception("Unrecognized type of algorithm. Only 2 types are available : EVENT_ORIENTED or NOT_EVENT_ORIENTED.");
    }
}

void OptimizerAlgStandardized::finish()
{
  _algBehind->finish();
}

void OptimizerAlgStandardized::start()
{
  switch(_algBehind->getType())
    {
    case EVENT_ORIENTED:
      {
        ((OptimizerAlgSync *) _algBehind)->start();
        break;
      }
    case NOT_EVENT_ORIENTED:
      {
        void **stackForNewTh= new void* [2];
        stackForNewTh[0]=(void *) ((OptimizerAlgASync *)(_algBehind));//In case of virtual inheritance
        stackForNewTh[1]=(void *) &_condition;
        _threadInCaseOfNotEvent=new ::YACS::BASES::Thread(threadFctForAsync,stackForNewTh);
        _condition.waitForAWait();
        break;
      }
    default:
      throw Exception("Unrecognized type of algorithm. Only 2 types are available : EVENT_ORIENTED or NOT_EVENT_ORIENTED.");
    }
}

void *OptimizerAlgStandardized::threadFctForAsync(void* ownStack)
{
  void **ownStackCst=(void **)ownStack;
  OptimizerAlgASync *alg=(OptimizerAlgASync *)ownStackCst[0];
  ::YACS::BASES::DrivenCondition *cond=(::YACS::BASES::DrivenCondition *)ownStackCst[1];
  delete [] ownStackCst;
  alg->startToTakeDecision(cond);
  return 0;
}

FakeNodeForOptimizerLoop::FakeNodeForOptimizerLoop(OptimizerLoop *loop, bool normal, unsigned reason):ElementaryNode(NAME),_loop(loop),
                                                                                                      _normal(normal),
                                                                                                      _reason(reason)
{
  _state=YACS::TOACTIVATE;
  _father=_loop->getFather();
}

FakeNodeForOptimizerLoop::FakeNodeForOptimizerLoop(const FakeNodeForOptimizerLoop& other):ElementaryNode(other),_loop(0),
                                                                                          _normal(false)
{ 
}

Node *FakeNodeForOptimizerLoop::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new FakeNodeForOptimizerLoop(*this);
}

void FakeNodeForOptimizerLoop::exForwardFailed()
{
  _loop->exForwardFailed();
  FakeNodeForOptimizerLoop *normallyThis=_loop->_nodeForSpecialCases;
  _loop->_nodeForSpecialCases=0;
  delete normallyThis;
}

void FakeNodeForOptimizerLoop::exForwardFinished()
{
  _loop->exForwardFinished();
  FakeNodeForOptimizerLoop *normallyThis=_loop->_nodeForSpecialCases;
  _loop->_nodeForSpecialCases=0;
  delete normallyThis;
}

void FakeNodeForOptimizerLoop::execute()
{
  if(!_normal)
    {
      string what;
      if(_reason==ALG_WITHOUT_START_CASES)
        what="Alg initialization of optimizerNode with name "; what+=_loop->getName(); what+=" returns no new case(s) to launch";
      throw Exception(what);
    }
}

void FakeNodeForOptimizerLoop::aborted()
{
  _loop->_state=YACS::ERROR;
}

void FakeNodeForOptimizerLoop::finished()
{
  
}

/*! \class YACS::ENGINE::OptimizerLoop
 *  \brief class to build optimization loops
 *
 * \ingroup Nodes
 */

OptimizerLoop::OptimizerLoop(const std::string& name, const std::string& algLibWthOutExt,
                             const std::string& symbolNameToOptimizerAlgBaseInstanceFactory,
                             bool algInitOnFile) throw(YACS::Exception)
  try : DynParaLoop(name,Runtime::_tc_string),_loader(algLibWthOutExt),_algInitOnFile(algInitOnFile),
        _portForInitFile(NAME_OF_FILENAME_INPUT,this,Runtime::_tc_string),
        _alg(new OptimizerAlgStandardized(&_myPool,0)),_convergenceReachedWithOtherCalc(false),
        _retPortForOutPool(NAME_OF_OUT_POOL_INPUT,this,Runtime::_tc_string),
        _nodeForSpecialCases(0)
{
  setAlgorithm(algLibWthOutExt,symbolNameToOptimizerAlgBaseInstanceFactory);
}
catch(Exception& e)
{
}

OptimizerLoop::OptimizerLoop(const OptimizerLoop& other, ComposedNode *father, bool editionOnly):
  DynParaLoop(other,father,editionOnly),_algInitOnFile(other._algInitOnFile),_loader(other._loader.getLibNameWithoutExt()),_convergenceReachedWithOtherCalc(false),
  _alg(new OptimizerAlgStandardized(&_myPool,0)),_portForInitFile(other._portForInitFile,this),_retPortForOutPool(other._retPortForOutPool,this),_nodeForSpecialCases(0)
{
  setAlgorithm(other._loader.getLibNameWithoutExt(),other._symbol,false);
}

OptimizerLoop::~OptimizerLoop()
{
  _alg->decrRef();
  cleanDynGraph();
  cleanInterceptors();
}

Node *OptimizerLoop::simpleClone(ComposedNode *father, bool editionOnly) const
{
  return new OptimizerLoop(*this,father,editionOnly);
}

void OptimizerLoop::init(bool start)
{
  DynParaLoop::init(start);
  _portForInitFile.exInit(start);
  _convergenceReachedWithOtherCalc=false;
  cleanDynGraph();
  cleanInterceptors();
}

void OptimizerLoop::exUpdateState()
{
  if(_state == YACS::DISABLED)
    return;
  if(_inGate.exIsReady())
    {
      _state=YACS::TOACTIVATE;
      //internal graph update
      int i;
      int nbOfBr=_nbOfBranches.getIntValue();
      if(nbOfBr==0)
        {
          delete _nodeForSpecialCases;
          _nodeForSpecialCases=new FakeNodeForOptimizerLoop(this,getAllOutPortsLeavingCurrentScope().empty(),FakeNodeForOptimizerLoop::NO_BRANCHES);
          return;
        }
      
      if(_portForInitFile.isEmpty())
        {
          delete _nodeForSpecialCases;
          _nodeForSpecialCases=new FakeNodeForOptimizerLoop(this,getAllOutPortsLeavingCurrentScope().empty(),FakeNodeForOptimizerLoop::NO_ALG_INITIALIZATION);
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
      for(i=0;i<nbOfBr;i++)
        {
          _execIds[i]=NOT_INITIALIZED_BRANCH_ID;
          _execNodes[i]=_node->clone(this,false);
          if(_initNode)
            _execInitNodes[i]=_initNode->clone(this,false);
          prepareInputsFromOutOfScope(i);
        }
      initInterceptors(nbOfBr);
      _alg->parseFileToInit(_portForInitFile.getValue()->getStringValue());
      _alg->start();
      int id;
      unsigned char priority;
      Any *val=_myPool.getNextSampleWithHighestPriority(id,priority);
      if(!val)
        {
          delete _nodeForSpecialCases;
          _nodeForSpecialCases=new FakeNodeForOptimizerLoop(this,getAllOutPortsLeavingCurrentScope().empty(),FakeNodeForOptimizerLoop::ALG_WITHOUT_START_CASES);
          return;
        }
      launchMaxOfSamples(true);
    }
}

int OptimizerLoop::getNumberOfInputPorts() const
{
  return DynParaLoop::getNumberOfInputPorts()+2;
}

InputPort *OptimizerLoop::getInputPort(const std::string& name) const throw(YACS::Exception)
{
  if(name==NAME_OF_FILENAME_INPUT)
    return (InputPort *)&_portForInitFile;
  else if(name==NAME_OF_OUT_POOL_INPUT)
    return (InputPort *)&_retPortForOutPool;
  else
    return DynParaLoop::getInputPort(name);
}

std::list<InputPort *> OptimizerLoop::getSetOfInputPort() const
{
  list<InputPort *> ret=DynParaLoop::getSetOfInputPort();
  ret.push_back((InputPort *)&_portForInitFile);
  ret.push_back((InputPort *)&_retPortForOutPool);
  return ret;
}

std::list<InputPort *> OptimizerLoop::getLocalInputPorts() const
{
  list<InputPort *> ret=DynParaLoop::getLocalInputPorts();
  ret.push_back((InputPort *)&_portForInitFile);
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
      for(vector<Node *>::iterator iter=_execNodes.begin();iter!=_execNodes.end();iter++)
        (*iter)->getReadyTasks(tasks);
      for(vector<Node *>::iterator iter2=_execInitNodes.begin();iter2!=_execInitNodes.end();iter2++)
        (*iter2)->getReadyTasks(tasks);
    }
    return;
  
}

YACS::Event OptimizerLoop::updateStateOnFinishedEventFrom(Node *node)
{
  unsigned int id;
  switch(getIdentityOfNotifyerNode(node,id))
    {
    case INIT_NODE:
      _execNodes[id]->exUpdateState();
      _nbOfEltConsumed++;
      break;
    case WORK_NODE:
      if(_state==YACS::DONE)//This case happend when alg has reached its convergence whereas other calculations still compute.
        {
          if(isFullyLazy())
            _condForCompletenessB4Relaunch.wait();
          return YACS::NOEVENT;
        }
      _myPool.putOutSampleAt(_execIds[id],_interceptorsForOutPool[id]->getValue());
      _myPool.setCurrentId(_execIds[id]);
      _alg->takeDecision();
      _myPool.destroyCurrentCase();
      if(_myPool.empty())
        {
          pushValueOutOfScopeForCase(id);
          _execIds[id]=NOT_RUNNING_BRANCH_ID;
          if(!isFullyLazy())// This case happens when the hand is returned to continue, whereas some other are working in parallel for nothing.
            _convergenceReachedWithOtherCalc=true;
          _state=YACS::DONE;
          return YACS::FINISH;
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
              exForwardFailed();
              _state=YACS::INTERNALERR;
              return YACS::FINISH;
            }
          return YACS::NOEVENT;
        }
      launchMaxOfSamples(false);
    }
  return YACS::NOEVENT;
}

void OptimizerLoop::checkNoCyclePassingThrough(Node *node) throw(YACS::Exception)
{
}

void OptimizerLoop::buildDelegateOf(InPort * & port, OutPort *initialStart, const std::list<ComposedNode *>& pointsOfView)
{
  DynParaLoop::buildDelegateOf(port,initialStart,pointsOfView);
  if(port==&_retPortForOutPool)
    throw Exception("OptimizerLoop::buildDelegateOf : uncorrect OptimizerLoop link : out pool port must be linked within the scope of OptimizerLoop node it belongs to.");
}

void OptimizerLoop::buildDelegateOf(std::pair<OutPort *, OutPort *>& port, InPort *finalTarget, const std::list<ComposedNode *>& pointsOfView)
{
  DynParaLoop::buildDelegateOf(port,finalTarget,pointsOfView);
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

void OptimizerLoop::cleanInterceptors()
{
  //the destruction of interceptors whereas some running nodes can push value on them can lead to SIG SEGV.
  if(!_execNodes.empty())
    {
      if(_convergenceReachedWithOtherCalc)
        {
          cout << "Waiting completion of last other useless cases." << endl;
          _condForCompletenessB4Relaunch.waitForAWait();
        }
    }
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
  for(val=_myPool.getNextSampleWithHighestPriority(id,priority);!isFullyBusy(i) && val;val=_myPool.getNextSampleWithHighestPriority(id,priority))
    {
      if(_execIds[i] == NOT_INITIALIZED_BRANCH_ID)
        first=true; // node is not initialized (first pass)
      else
        first=false; // node is initialized (second pass)
      _execIds[i]=id;
      _myPool.markIdAsInUse(id);
      if(_initNode)
        {
          if(!_initNodeUpdated[i])
            {
              putValueOnBranch(val,i,first);
              _execInitNodes[i]->exUpdateState();
              _initNodeUpdated[i]=true;
            }
          else
            {
              putValueOnBranch(val,i,first);
              _execNodes[i]->exUpdateState();
              _nbOfEltConsumed++;
            }
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
void OptimizerLoop::setAlgorithm(const std::string& alglib, const std::string& symbol, bool checkLinks)
{
  if(checkLinks)
    {
      if(_splittedPort.edGetNumberOfOutLinks() != 0)
        throw Exception("The OptimizerLoop node must be disconnected before setting the algorithm");
      if(_retPortForOutPool.edGetNumberOfLinks() != 0)
        throw Exception("The OptimizerLoop node must be disconnected before setting the algorithm");
    }

  _symbol=symbol;
  _loader=YACS::BASES::DynLibLoader(alglib);

  OptimizerAlgBaseFactory algFactory=0;
  if(alglib!="" && _symbol!="")
    algFactory=(OptimizerAlgBaseFactory)_loader.getHandleOnSymbolWithName(_symbol);
  _alg->setAlgPointer(algFactory);
  if(!algFactory)
    return;
  _splittedPort.edSetType(_alg->getTCForIn());
  _retPortForOutPool.edSetType(_alg->getTCForOut());
}

//! Return the name of the algorithm library
/*!
 *
 */
std::string OptimizerLoop::getAlgLib() const
{
  return _loader.getLibNameWithoutExt();
}

//! Check validity for the node.
/*!
 *  Throw an exception if the node is not valid
 */
void OptimizerLoop::checkBasicConsistency() const throw(YACS::Exception)
{
  DEBTRACE("OptimizerLoop::checkBasicConsistency");
  DynParaLoop::checkBasicConsistency();
  if(!_alg->getAlg())
    throw Exception("Problem during library loading.");
}

