// Copyright (C) 2012-2016  CEA/DEN, EDF R&D
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
// Author : Anthony Geay (EDF R&D)

#include "YACSEvalYFXPattern.hxx"
#include "YACSEvalResource.hxx"
#include "YACSEvalSeqAny.hxx"
#include "YACSEvalSession.hxx"
#include "YACSEvalObserver.hxx"
#include "YACSEvalSessionInternal.hxx"
#include "YACSEvalAutoPtr.hxx"
#include "YACSEvalExecParams.hxx"

#include "ElementaryNode.hxx"
#include "RuntimeSALOME.hxx"
#include "Dispatcher.hxx"
#include "Executor.hxx"
#include "InputPort.hxx"
#include "LinkInfo.hxx"
#include "TypeCode.hxx"
#include "Proc.hxx"
#include "Dispatcher.hxx"

#include "PythonPorts.hxx"
#include "ForEachLoop.hxx"
#include "PythonNode.hxx"
#include "InlineNode.hxx"
#include "ServiceNode.hxx"
#include "PyStdout.hxx"
#include "AutoGIL.hxx"

#include "ResourcesManager.hxx"

#include <map>
#include <limits>
#include <numeric>
#include <sstream>
#include <iterator>

////
#include <stdlib.h>

const char YACSEvalYFXPattern::ST_OK[]="ALL_OK";

const char YACSEvalYFXPattern::ST_FAILED[]="SOME_SAMPLES_FAILED_AND_ALL_OF_THEM_FAILED_DETERMINISTICALLY";

const char YACSEvalYFXPattern::ST_ERROR[]="SOME_SAMPLES_FAILED_BUT_IMPOSSIBLE_TO_CONCLUDE_ON_THEM";

const std::size_t YACSEvalYFXPattern::MAX_LGTH_OF_INP_DUMP=10000;

const char YACSEvalYFXGraphGen::DFT_PROC_NAME[]="YFX";

const char YACSEvalYFXGraphGen::FIRST_FE_SUBNODE_NAME[]="Bloc";

const char YACSEvalYFXGraphGen::GATHER_NODE_NAME[]="__gather__";

class MyAutoThreadSaver
{
public:
  MyAutoThreadSaver(bool isToSave):_isToSave(isToSave),_save(0)
  {
    if(_isToSave)
      {
        PyThreadState *save(PyThreadState_Swap(NULL));// safe call of PyEval_SaveThread()
        if(save)
          {
            _save=save;
            PyEval_ReleaseLock();
          }
      }
  }
  ~MyAutoThreadSaver() { if(_isToSave) if(_save) { PyEval_AcquireLock(); PyThreadState_Swap(_save); /*safe call of PyEval_RestoreThread*/ } }
private:
  bool _isToSave;
  PyThreadState *_save;
};

std::vector< YACSEvalInputPort *> YACSEvalYFXPattern::getFreeInputPorts() const
{
  std::size_t sz(_inputs.size());
  std::vector< YACSEvalInputPort *> ret;
  std::vector< YACSEvalInputPort >::const_iterator it(_inputs.begin());
  for(std::size_t i=0;i<sz;i++,it++)
    ret.push_back(const_cast<YACSEvalInputPort *>(&(*it)));
  return ret;
}

std::vector< YACSEvalOutputPort *> YACSEvalYFXPattern::getFreeOutputPorts() const
{
  std::size_t sz(_outputs.size());
  std::vector< YACSEvalOutputPort *> ret;
  std::vector< YACSEvalOutputPort >::const_iterator it(_outputs.begin());
  for(std::size_t i=0;i<sz;i++,it++)
    ret.push_back(const_cast<YACSEvalOutputPort *>(&(*it)));
  return ret;
}

YACSEvalYFXPattern *YACSEvalYFXPattern::FindPatternFrom(YACSEvalYFX *boss, YACS::ENGINE::Proc *scheme, bool ownScheme)
{
  if(!scheme)
    throw YACS::Exception("YACSEvalYFXPattern::FindPatternFrom : input scheme must be not null !");
  {
      YACS::ENGINE::ComposedNode *zeRunNode(0);
      bool isMatchingRunOnlyPattern(YACSEvalYFXRunOnlyPattern::IsMatching(scheme,zeRunNode));
      if(isMatchingRunOnlyPattern)
        return new YACSEvalYFXRunOnlyPattern(boss,scheme,ownScheme,zeRunNode);
  }
  throw YACS::Exception("YACSEvalYFXPattern::FindPatternFrom : no pattern found for the input scheme !");
}

bool YACSEvalYFXPattern::isAlreadyComputedResources() const
{
  return _res!=0;
}

void YACSEvalYFXPattern::checkNonAlreadyComputedResources() const
{
  if(isAlreadyComputedResources())
    throw YACS::Exception("checkNonAlreadyComputedResources : instance of computed resources already computed !");
}

void YACSEvalYFXPattern::checkAlreadyComputedResources() const
{
  if(!isAlreadyComputedResources())
    throw YACS::Exception("checkAlreadyComputedResources : instance of computed resources not already computed !");
}

void YACSEvalYFXPattern::checkLocked() const
{
  if(!isLocked())
    throw YACS::Exception("YACSEvalYFXPattern::checkLocked : Pattern is not locked !");
}

void YACSEvalYFXPattern::checkNonLocked() const
{
  if(isLocked())
    throw YACS::Exception("YACSEvalYFXPattern::checkNonLocked : Pattern is locked !");
}

void YACSEvalYFXPattern::CheckNodeIsOK(YACS::ENGINE::ComposedNode *node)
{
  /*YACS::ENGINE::LinkInfo info(YACS::ENGINE::LinkInfo::WARN_ONLY_DONT_STOP);
  try
  {
      node->checkConsistency(info);
  }
  catch(YACS::Exception& e)
  {
  }
  if(info.getNumberOfErrLinks(YACS::ENGINE::E_ALL)!=0)
    throw YACS::Exception("YACSEvalYFXPattern::CheckNodeIsOK : found node is not OK !");
  std::list<YACS::ENGINE::ElementaryNode *> allNodes(node->getRecursiveConstituents());
  for(std::list<YACS::ENGINE::ElementaryNode *>::const_iterator it=allNodes.begin();it!=allNodes.end();it++)
    {
      YACS::ENGINE::ServiceNode *node0(dynamic_cast<YACS::ENGINE::ServiceNode *>(*it));
      YACS::ENGINE::InlineNode *node1(dynamic_cast<YACS::ENGINE::InlineNode *>(*it));
      if(node0)
        {
          YACS::ENGINE::Container *cont(node0->getContainer());
          YACS::ENGINE::ComponentInstance *comp(node0->getComponent());
          if(!cont || !comp)
            {
              std::ostringstream oss; oss << "YACSEvalYFXPattern::CheckNodeIsOK : ServiceNode called \"" << node0->getName() << "\" is not correctly defined !";
              throw YACS::Exception(oss.str());
            }
        }
      if(node1)
        {
          YACS::ENGINE::Container *cont(node1->getContainer());
          if(!cont && node1->getExecutionMode()==YACS::ENGINE::InlineNode::REMOTE_STR)
            {
              std::ostringstream oss; oss << "YACSEvalYFXPattern::CheckNodeIsOK : InlineNode called \"" << node1->getName() << "\" is not correctly defined !";
              throw YACS::Exception(oss.str());
            }
        }
    }*/
}

void YACSEvalYFXPattern::registerObserver(YACSEvalObserver *observer)
{
  if(_observer==observer)
    return ;
  if(_observer)
    _observer->decrRef();
  _observer=observer;
  if(_observer)
    _observer->incrRef();
}

YACSEvalYFXPattern::YACSEvalYFXPattern(YACSEvalYFX *boss, YACS::ENGINE::Proc *scheme, bool ownScheme):_boss(boss),_scheme(scheme),_ownScheme(ownScheme),_parallelizeStatus(true),_rm(new ResourcesManager_cpp),_res(0),_observer(0)
{
}

YACS::ENGINE::TypeCode *YACSEvalYFXPattern::CreateSeqTypeCodeFrom(YACS::ENGINE::Proc *scheme, const std::string& zeType)
{
  std::ostringstream oss; oss << "list[" << zeType << "]";
  YACS::ENGINE::TypeCode *tc(scheme->getTypeCode(zeType));
  return scheme->createSequenceTc(oss.str(),oss.str(),tc);
}

void YACSEvalYFXPattern::setResources(YACSEvalListOfResources *res)
{
  checkNonAlreadyComputedResources();
  if(res!=_res)
    delete _res;
  _res=res;
}

void YACSEvalYFXPattern::resetResources()
{
  delete _res;
  _res=0;
}

YACSEvalSeqAny *YACSEvalYFXPattern::BuildValueInPort(YACS::ENGINE::InputPyPort *port)
{
  if(!port)
    throw YACS::Exception("YACSEvalYFXPattern::GetValueInPort : null input port !");
  PyObject *obj(port->getPyObj());
  YACS::ENGINE::TypeCode *tc(port->edGetType());
  YACS::ENGINE::TypeCodeSeq *tcc(dynamic_cast<YACS::ENGINE::TypeCodeSeq *>(tc));
  if(!tcc)
    {
      std::ostringstream oss; oss << "YACSEvalYFXPattern::GetValueInPort : internal error for tc of input \"" << port->getName() << "\"";
      throw YACS::Exception(oss.str());
    }
  const YACS::ENGINE::TypeCode *tcct(tcc->contentType());
  if(!PyList_Check(obj))
    throw YACS::Exception("YACSEvalYFXPattern::GetValueInPort : internal error 2 !");
  std::size_t sz(PyList_Size(obj));
  if(tcct->kind()==YACS::ENGINE::Double)
    {
      std::vector<double> eltCpp(sz);
      for(std::size_t i=0;i<sz;i++)
        {
          PyObject *elt(PyList_GetItem(obj,i));
          eltCpp[i]=PyFloat_AsDouble(elt);
        }
      YACS::AutoCppPtr<YACSEvalSeqAnyDouble> elt(new YACSEvalSeqAnyDouble(eltCpp));
      return elt.dettach();
    }
  else if(tcct->kind()==YACS::ENGINE::Int)
    {
      std::vector<int> eltCpp(sz);
      for(std::size_t i=0;i<sz;i++)
        {
          PyObject *elt(PyList_GetItem(obj,i));
          eltCpp[i]=PyInt_AsLong(elt);
        }
      YACS::AutoCppPtr<YACSEvalSeqAnyInt> elt(new YACSEvalSeqAnyInt(eltCpp));
      return elt.dettach();
    }
  else
    throw YACS::Exception("YACSEvalYFXPattern::GetValueInPort : not implemented yet for other than Double and Int !");
}

YACSEvalSeqAny *YACSEvalYFXPattern::BuildValueFromEngineFrmt(YACS::ENGINE::SequenceAny *data)
{
  unsigned int sz(data->size());
  std::vector<double> eltCpp(sz);
  for(unsigned int ii=0;ii<sz;ii++)
    {
      YACS::ENGINE::AnyPtr elt((*data)[ii]);
      YACS::ENGINE::Any *eltPtr((YACS::ENGINE::Any *)elt);
      YACS::ENGINE::AtomAny *eltPtr2(dynamic_cast<YACS::ENGINE::AtomAny *>(eltPtr));
      if(!eltPtr2)
        {
          std::ostringstream oss; oss << "YACSEvalYFXPattern::BuildValueFromEngineFrmt : error at pos #" << ii << " ! It is not an AtomAny !";
          throw YACS::Exception(oss.str());
        }
      eltCpp[ii]=eltPtr2->getDoubleValue();
    }
  return new YACSEvalSeqAnyDouble(eltCpp);
}

void YACSEvalYFXPattern::cleanScheme()
{
  if(_ownScheme)
    delete _scheme;
  _scheme=0;
}

YACSEvalYFXPattern::~YACSEvalYFXPattern()
{
  if(_observer)
    _observer->decrRef();
  delete _rm;
  delete _res;
}

/////////////////////

class YACSEvalYFXRunOnlyPatternInternalObserver : public YACS::ENGINE::Observer
{
public:
  YACSEvalYFXRunOnlyPatternInternalObserver(YACSEvalYFXRunOnlyPattern *boss):_boss(boss) { if(!_boss) throw YACS::Exception("YACSEvalYFXRunOnlyPatternInternalObserver constructor : null boss not supported :)"); }
  void notifyObserver2(YACS::ENGINE::Node *object, const std::string& event, void *something);
private:
  YACSEvalYFXRunOnlyPattern *_boss;
};

void YACSEvalYFXRunOnlyPatternInternalObserver::notifyObserver2(YACS::ENGINE::Node *object, const std::string& event, void *something)
{
  YACS::ENGINE::ForEachLoop *object2(_boss->getUndergroundForEach());
  YACSEvalObserver *obs(_boss->getObserver());
  if(!obs)
    return ;
  if(event=="progress_ok" && object2==object)
    {
      int *casted(reinterpret_cast<int *>(something));
      obs->notifySampleOK(_boss->getBoss(),*casted);
      return ;
    }
  if(event=="progress_ko" && object2==object)
    {
      int *casted(reinterpret_cast<int *>(something));
      obs->notifySampleKO(_boss->getBoss(),*casted);
      return ;
    }
}

/////////////////////

YACSEvalYFXRunOnlyPattern::YACSEvalYFXRunOnlyPattern(YACSEvalYFX *boss, YACS::ENGINE::Proc *scheme, bool ownScheme, YACS::ENGINE::ComposedNode *runNode):YACSEvalYFXPattern(boss,scheme,ownScheme),_lockedStatus(false),_runNode(runNode),_gen(0),_obs(new YACSEvalYFXRunOnlyPatternInternalObserver(this))
{
  if(!_runNode)
    throw YACS::Exception("YACSEvalYFXRunOnlyPattern : internal run node must be not null !");
  buildInputPorts();
  buildOutputPorts();
}

YACSEvalYFXRunOnlyPattern::~YACSEvalYFXRunOnlyPattern()
{
  delete _obs;
  delete _gen;
}

void YACSEvalYFXRunOnlyPattern::setOutPortsOfInterestForEvaluation(const std::vector<YACSEvalOutputPort *>& outputsOfInterest)
{
  checkNonLocked();
  _outputsOfInterest=outputsOfInterest;
  _lockedStatus=true;
}

void YACSEvalYFXRunOnlyPattern::resetOutputsOfInterest()
{
  checkLocked();
  _outputsOfInterest.clear();
  _lockedStatus=false;
}

void YACSEvalYFXRunOnlyPattern::generateGraph()
{
  delete _gen;
  if(getResourcesInternal()->isInteractive())
    _gen=new YACSEvalYFXGraphGenInteractive(this);
  else
    _gen=new YACSEvalYFXGraphGenCluster(this);
  _gen->generateGraph();
}

void YACSEvalYFXRunOnlyPattern::resetGeneratedGraph()
{
  if(_gen)
    _gen->resetGeneratedGraph();
}

int YACSEvalYFXRunOnlyPattern::assignNbOfBranches()
{
  checkAlreadyComputedResources();
  if(!_gen)
    throw YACS::Exception("YACSEvalYFXRunOnlyPattern::assignNbOfBranches : generator is NULL ! Please invoke generateGraph before !");
  return _gen->assignNbOfBranches();
}

void YACSEvalYFXRunOnlyPattern::assignRandomVarsInputs()
{
  std::size_t sz(std::numeric_limits<std::size_t>::max());
  for(std::vector< YACSEvalInputPort >::const_iterator it=_inputs.begin();it!=_inputs.end();it++)
    if((*it).isRandomVar())
      {
        std::size_t locSize((*it).initializeUndergroundWithSeq());
        if(sz==std::numeric_limits<std::size_t>::max())
          sz=locSize;
        else
          if(sz!=locSize)
            throw YACS::Exception("YACSEvalYFXRunOnlyPattern::assignRandomVarsInputs : length of sequences in random vars must be the same !");
      }
}

bool YACSEvalYFXRunOnlyPattern::isLocked() const
{
  return _lockedStatus;
}

YACSEvalListOfResources *YACSEvalYFXRunOnlyPattern::giveResources()
{
  checkLocked();
  if(!isAlreadyComputedResources())
    {
      YACS::ENGINE::DeploymentTree dt(_runNode->getDeploymentTree());
      _runNode->removeRecursivelyRedundantCL();
      YACSEvalListOfResources *res(new YACSEvalListOfResources(_runNode->getMaxLevelOfParallelism(),getCatalogInAppli(),dt));
      setResources(res);
    }
  return getResourcesInternal();
}

YACS::ENGINE::Proc *YACSEvalYFXRunOnlyPattern::getUndergroundGeneratedGraph() const
{
  return getGenerator()->getUndergroundGeneratedGraph();
}

std::string YACSEvalYFXRunOnlyPattern::getErrorDetailsInCaseOfFailure() const
{
  std::string st(getStatusOfRunStr());//test if a run has occurred.
  if(st==ST_OK)
    throw YACS::Exception("YACSEvalYFXRunOnlyPattern::getErrorDetailsInCaseOfFailure : The execution of scheme has been carried out to the end without any problem !");
  // All the problem can only comes from foreach -> scan it
  YACS::ENGINE::ForEachLoop *fe(getUndergroundForEach());
  YACS::ENGINE::NodeStateNameMap nsm;
  unsigned nbB(fe->getNumberOfBranchesCreatedDyn());
  std::ostringstream oss;
  for(unsigned j=0;j<nbB;j++)
    {
      YACS::ENGINE::Node *nn(fe->getChildByNameExec(YACSEvalYFXGraphGen::FIRST_FE_SUBNODE_NAME,j));
      YACS::ENGINE::Bloc *nnc(dynamic_cast<YACS::ENGINE::Bloc *>(nn));
      if(!nnc)
        throw YACS::Exception("YACSEvalYFXRunOnlyPattern::getErrorDetailsInCaseOfFailure : internal error 1 ! The direct son of main foreach is expected to be a Bloc !");
      if(nnc->getState()==YACS::DONE)
        continue;
      std::list< YACS::ENGINE::ElementaryNode *> fec(nnc->getRecursiveConstituents());
      for(std::list< YACS::ENGINE::ElementaryNode *>::reverse_iterator it1=fec.rbegin();it1!=fec.rend();it1++)
        {
          YACS::StatesForNode st0((*it1)->getState());
          if(st0!=YACS::DONE)
            {
              oss << "NODE = " << nnc->getChildName(*it1) << std::endl;
              oss << "STATUS = " << nsm[st0] << std::endl;
              oss << "BRANCH ID = " << j << std::endl;
              std::list<YACS::ENGINE::InputPort *> inps((*it1)->getSetOfInputPort());
              for(std::list<YACS::ENGINE::InputPort *>::const_iterator it2=inps.begin();it2!=inps.end();it2++)
                {
                  std::string d((*it2)->getHumanRepr());
                  if(d.size()>10000)
                    d=d.substr(0,MAX_LGTH_OF_INP_DUMP);
                  oss << "INPUT \"" << (*it2)->getName() << "\" = " << d << std::endl;
                }
              oss << "DETAILS = " << std::endl;
              oss << (*it1)->getErrorDetails();
            }
        }
    }
  return oss.str();
}

std::string YACSEvalYFXRunOnlyPattern::getStatusOfRunStr() const
{
  YACS::StatesForNode st(getUndergroundGeneratedGraph()->getState());
  switch(st)
    {
    case YACS::READY:
    case YACS::TOLOAD:
    case YACS::LOADED:
    case YACS::TOACTIVATE:
    case YACS::ACTIVATED:
    case YACS::SUSPENDED:
    case YACS::PAUSE:
    case YACS::DISABLED:
    case YACS::DESACTIVATED:
      {
        std::ostringstream oss; oss << "YACSEvalYFXRunOnlyPattern::getStatusOfRunStr : Unexpected state \"" << YACS::ENGINE::Node::getStateName(st) << "\" ! Did you invoke run ?";
        throw YACS::Exception(oss.str());
      }
    case YACS::LOADFAILED:
    case YACS::EXECFAILED:
    case YACS::ERROR:
    case YACS::INTERNALERR:
      return std::string(ST_ERROR);
    case YACS::FAILED:
      return std::string(ST_FAILED);
    case YACS::DONE:
      return std::string(ST_OK);
    default:
      {
        std::ostringstream oss; oss << "YACSEvalYFXRunOnlyPattern::getStatusOfRunStr : unrecognized and managed state \"" << YACS::ENGINE::Node::getStateName(st) << "\" !";
        throw YACS::Exception(oss.str());
      }
    }
}

std::vector<YACSEvalSeqAny *> YACSEvalYFXRunOnlyPattern::getResults() const
{
  return _gen->getResults();
}

/*!
 * This method works if run succeeded (true return) and also if graph has failed. Graph failed means soft error of evaluation due to error in evaluation (example 1/0 or a normal throw from one node)
 * If a more serious error occured (SIGSEGV of a server or internal error in YACS engine, cluster error, loose of connection...) this method will throw an exception to warn the caller that the results may be 
 */
std::vector<YACSEvalSeqAny *> YACSEvalYFXRunOnlyPattern::getResultsInCaseOfFailure(std::vector<unsigned int>& passedIds) const
{
  YACS::StatesForNode st(getUndergroundGeneratedGraph()->getState());
  if(st==YACS::DONE)
    {
      passedIds.clear();
      std::vector<YACSEvalSeqAny *> ret(getResults());
      if(!ret.empty())
        {
          if(!ret[0])
            throw YACS::Exception("YACSEvalYFXRunOnlyPattern::getResultsInCaseOfFailure : internal error ! The returned vector has a null pointer at pos #0 !");
          std::size_t sz(ret[0]->size());
          passedIds.resize(sz);
          for(std::size_t i=0;i<sz;i++)
            passedIds[i]=i;
        }
      return ret;
    }
  getStatusOfRunStr();// To check that the status is recognized.
  std::list<YACS::ENGINE::Node *> lns(getUndergroundGeneratedGraph()->edGetDirectDescendants());
  YACS::ENGINE::ForEachLoop *fe(getUndergroundForEach());
  //
  YACS::ENGINE::Executor exe;
  std::vector<YACS::ENGINE::SequenceAny *> outputs;
  std::vector<std::string> nameOfOutputs;
  passedIds=fe->getPassedResults(&exe,outputs,nameOfOutputs);//<- the key invokation is here.
  std::size_t sz(passedIds.size()),ii(0);
  std::vector<YACSEvalSeqAny *> ret(_outputsOfInterest.size());
  for(std::vector<YACSEvalOutputPort *>::const_iterator it1=_outputsOfInterest.begin();it1!=_outputsOfInterest.end();it1++,ii++)
    {
      YACS::ENGINE::OutputPort *p((*it1)->getUndergroundPtr());
      std::string st(_runNode->getOutPortName(p));
      std::ostringstream oss; oss << YACSEvalYFXGraphGen::FIRST_FE_SUBNODE_NAME << '.' << _runNode->getName() << '.' << st;
      st=oss.str();
      YACS::ENGINE::ForEachLoop::InterceptorizeNameOfPort(st);
      std::vector<std::string>::iterator it2(std::find(nameOfOutputs.begin(),nameOfOutputs.end(),st));
      if(it2==nameOfOutputs.end())
        {
          std::ostringstream oss; oss << "YACSEvalYFXRunOnlyPattern::getResultsInCaseOfFailure : internal error 3 ! Unable to locate interceptor with name " << st << " ! Possibilities are : ";
          std::copy(nameOfOutputs.begin(),nameOfOutputs.end(),std::ostream_iterator<std::string>(oss," "));
          oss << " !";
          throw YACS::Exception(oss.str());
        }
      std::size_t pos(std::distance(nameOfOutputs.begin(),it2));
      ret[ii]=BuildValueFromEngineFrmt(outputs[pos]);
    }
  return ret;
}

void YACSEvalYFXRunOnlyPattern::emitStart() const
{
  YACSEvalObserver *obs(getObserver());
  if(!obs)
    return ;
  obs->startComputation(getBoss());
}

bool YACSEvalYFXRunOnlyPattern::go(const YACSEvalExecParams& params, YACSEvalSession *session) const
{
  emitStart();
  YACS::ENGINE::Dispatcher *disp(YACS::ENGINE::Dispatcher::getDispatcher());
  disp->addObserver(_obs,getUndergroundForEach(),"progress_ok");
  disp->addObserver(_obs,getUndergroundForEach(),"progress_ko");
  bool ret(getGenerator()->go(params,session));
  disp->removeObserver(_obs,getUndergroundForEach(),"progress_ok");
  disp->removeObserver(_obs,getUndergroundForEach(),"progress_ko");
  return ret;
}

YACS::ENGINE::ForEachLoop *YACSEvalYFXRunOnlyPattern::getUndergroundForEach() const
{
  return getGenerator()->getUndergroundForEach();
}

bool YACSEvalYFXRunOnlyPattern::IsMatching(YACS::ENGINE::Proc *scheme, YACS::ENGINE::ComposedNode *& runNode)
{
  std::list<YACS::ENGINE::Node *> nodes(scheme->getChildren());
  if(nodes.empty())
    return false;
  bool areAllElementary(true);
  for(std::list<YACS::ENGINE::Node *>::const_iterator it=nodes.begin();it!=nodes.end() && areAllElementary;it++)
    if(!dynamic_cast<YACS::ENGINE::ElementaryNode *>(*it))
      areAllElementary=false;
  if(areAllElementary)
    {
      if(scheme)
        CheckNodeIsOK(scheme);
      runNode=scheme;
      return true;
    }
  if(nodes.size()!=1)
    return false;
  YACS::ENGINE::ComposedNode *candidate(dynamic_cast<YACS::ENGINE::ComposedNode *>(nodes.front()));
  runNode=candidate;
  if(candidate)
    CheckNodeIsOK(candidate);
  return candidate!=0;
}

void YACSEvalYFXRunOnlyPattern::buildInputPorts()
{
  _inputs.clear();
  std::list< YACS::ENGINE::InputPort *> allInputPorts(_runNode->getSetOfInputPort());
  std::vector<std::string> allNames;
  for(std::list< YACS::ENGINE::InputPort *>::const_iterator it=allInputPorts.begin();it!=allInputPorts.end();it++)
    {
      YACS::ENGINE::InputPort *elt(*it);
      if(!elt)
        throw YACS::Exception("YACSEvalYFXRunOnlyPattern::buildInputPorts : presence of null input !");
      std::set<YACS::ENGINE::OutPort *> bls(elt->edSetOutPort());
      if(bls.empty())
        {
          if(YACSEvalPort::IsInputPortPublishable(elt))
            {
              std::string inpName(elt->getName());
              if(inpName.empty())
                throw YACS::Exception("YACSEvalYFXRunOnlyPattern::buildInputPorts : an input has empty name ! Should not !");
              _inputs.push_back(YACSEvalInputPort(elt));
              if(std::find(allNames.begin(),allNames.end(),inpName)!=allNames.end())
                {
                  std::ostringstream oss; oss << "YACSEvalYFXRunOnlyPattern::buildInputPorts : input name \"" << inpName << "\" appears more than once !";
                  throw YACS::Exception(oss.str());
                }
              allNames.push_back(inpName);
            }
        }
    }
}

void YACSEvalYFXRunOnlyPattern::buildOutputPorts()
{
  _outputs.clear();
  std::list< YACS::ENGINE::OutputPort *> allOutputPorts(_runNode->getSetOfOutputPort());
  std::vector<std::string> allNames;
  for(std::list< YACS::ENGINE::OutputPort *>::const_iterator it=allOutputPorts.begin();it!=allOutputPorts.end();it++)
    {
      YACS::ENGINE::OutputPort *elt(*it);
      if(YACSEvalPort::IsOutputPortPublishable(elt))
        {
          if(!elt)
            throw YACS::Exception("YACSEvalYFXRunOnlyPattern::buildOutputPorts : presence of null output !");
          std::string outpName(elt->getName());
          if(outpName.empty())
            throw YACS::Exception("YACSEvalYFXRunOnlyPattern::buildOutputPorts : an output has empty name ! Should not !");
          if(std::find(allNames.begin(),allNames.end(),outpName)!=allNames.end())
            {
              std::ostringstream oss; oss << "YACSEvalYFXRunOnlyPattern::buildOutputPorts : output name \"" << outpName << "\" appears more than once !";
              throw YACS::Exception(oss.str());
            }
          _outputs.push_back(YACSEvalOutputPort(*it));
        }
    }
}

YACSEvalYFXGraphGen *YACSEvalYFXRunOnlyPattern::getGenerator() const
{
  if(!_gen)
    throw YACS::Exception("getGenerator : generator is NULL !");
  return _gen;
}

/////////////////////

YACSEvalYFXGraphGen::YACSEvalYFXGraphGen(YACSEvalYFXRunOnlyPattern *boss):_boss(boss),_generatedGraph(0),_FEInGeneratedGraph(0)
{
  if(!_boss)
    throw YACS::Exception("YACSEvalYFXGraphGen constructor : boss is NULL !");
}

YACSEvalYFXGraphGen::~YACSEvalYFXGraphGen()
{
  //delete _generatedGraph;// -> TODO : AGY why ?
}

void YACSEvalYFXGraphGen::resetGeneratedGraph()
{
  delete _generatedGraph;
  _generatedGraph=0; _FEInGeneratedGraph=0;
}

bool YACSEvalYFXGraphGen::isLocked() const
{
  return _generatedGraph!=0;
}

int YACSEvalYFXGraphGen::assignNbOfBranches()
{
  if(!_generatedGraph)
    throw YACS::Exception("YACSEvalYFXGraphGen::assignNbOfBranches : the generated graph has not been created !");
  std::list<YACS::ENGINE::Node *> nodes(_generatedGraph->getChildren());
  YACS::ENGINE::ForEachLoop *zeMainNode(0);
  for(std::list<YACS::ENGINE::Node *>::const_iterator it=nodes.begin();it!=nodes.end();it++)
    {
      YACS::ENGINE::ForEachLoop *isZeMainNode(dynamic_cast<YACS::ENGINE::ForEachLoop *>(*it));
      if(isZeMainNode)
        {
          if(!zeMainNode)
            zeMainNode=isZeMainNode;
          else
            throw YACS::Exception("YACSEvalYFXGraphGen::assignNbOfBranches : internal error 1 !");
        }
    }
  if(!zeMainNode)
    throw YACS::Exception("YACSEvalYFXGraphGen::assignNbOfBranches : internal error 2 !");
  unsigned int nbProcsDeclared(getBoss()->getResourcesInternal()->getNumberOfProcsDeclared());
  nbProcsDeclared=std::max(nbProcsDeclared,4u);
  int nbOfBranch=1;
  if(getBoss()->getParallelizeStatus())
    {
      nbOfBranch=(nbProcsDeclared/getBoss()->getResourcesInternal()->getMaxLevelOfParallelism());
      nbOfBranch=std::max(nbOfBranch,1);
    }
  YACS::ENGINE::InputPort *zeInputToSet(zeMainNode->edGetNbOfBranchesPort());
  YACS::ENGINE::AnyInputPort *zeInputToSetC(dynamic_cast<YACS::ENGINE::AnyInputPort *>(zeInputToSet));
  if(!zeInputToSetC)
    throw YACS::Exception("YACSEvalYFXGraphGen::assignNbOfBranches : internal error 3 !");
  YACS::ENGINE::Any *a(YACS::ENGINE::AtomAny::New(nbOfBranch));
  zeInputToSetC->put(a);
  zeInputToSetC->exSaveInit();
  a->decrRef();
  return nbOfBranch;
}

void YACSEvalYFXGraphGenInteractive::generateGraph()
{
  if(_generatedGraph)
    { delete _generatedGraph; _generatedGraph=0; _FEInGeneratedGraph=0; }
  static const char LISTPYOBJ_STR[]="list[pyobj]";
  if(getBoss()->getOutputsOfInterest().empty())
    return ;
  YACS::ENGINE::RuntimeSALOME::setRuntime();
  YACS::ENGINE::RuntimeSALOME *r(YACS::ENGINE::getSALOMERuntime());
  _generatedGraph=r->createProc(DFT_PROC_NAME);
  YACS::ENGINE::TypeCode *pyobjTC(_generatedGraph->createInterfaceTc("python:obj:1.0","pyobj",std::list<YACS::ENGINE::TypeCodeObjref *>()));
  std::ostringstream oss; oss << "Loop_" << getBoss()->getRunNode()->getName();
  _generatedGraph->createType(YACSEvalAnyDouble::TYPE_REPR,"double");
  _generatedGraph->createType(YACSEvalAnyInt::TYPE_REPR,"int");
  //
  YACS::ENGINE::InlineNode *n0(r->createScriptNode(YACS::ENGINE::PythonNode::KIND,"__initializer__"));
  _generatedGraph->edAddChild(n0);
  YACS::ENGINE::TypeCode *listPyobjTC(_generatedGraph->createSequenceTc(LISTPYOBJ_STR,LISTPYOBJ_STR,pyobjTC));
  YACS::ENGINE::OutputPort *sender(n0->edAddOutputPort("sender",listPyobjTC));
  std::ostringstream var0;
  const std::vector< YACSEvalInputPort >& inputs(getBoss()->getInputs());
  for(std::vector< YACSEvalInputPort >::const_iterator it=inputs.begin();it!=inputs.end();it++)
    {
      if((*it).isRandomVar())
        {
          var0 << (*it).getName() << ",";
          YACS::ENGINE::TypeCode *tc(YACSEvalYFXPattern::CreateSeqTypeCodeFrom(_generatedGraph,(*it).getTypeOfData()));
          YACS::ENGINE::InputPort *inp(n0->edAddInputPort((*it).getName(),tc));
          YACS::ENGINE::InputPyPort *inpc(dynamic_cast<YACS::ENGINE::InputPyPort *>(inp));
          if(!inpc)
            throw YACS::Exception("YACSEvalYFXRunOnlyPattern::generateGraph : internal error 1 !");
          (*it).setUndergroundPortToBeSet(inpc);
        }
    }
  std::ostringstream n0Script; n0Script << "sender=zip(" << var0.str() << ")\n";
  n0->setScript(n0Script.str());
  //
  YACS::ENGINE::ForEachLoop *n1(r->createForEachLoop(oss.str(),pyobjTC));
  _FEInGeneratedGraph=n1;
  _generatedGraph->edAddChild(n1);
  _generatedGraph->edAddCFLink(n0,n1);
  _generatedGraph->edAddDFLink(sender,n1->edGetSeqOfSamplesPort());
  YACS::ENGINE::InlineNode *n2(r->createScriptNode(YACS::ENGINE::PythonNode::KIND,GATHER_NODE_NAME));
  _generatedGraph->edAddChild(n2);
  _generatedGraph->edAddCFLink(n1,n2);
  //
  YACS::ENGINE::Bloc *n10(r->createBloc(FIRST_FE_SUBNODE_NAME));
  n1->edAddChild(n10);
  YACS::ENGINE::InlineNode *n100(r->createScriptNode(YACS::ENGINE::PythonNode::KIND,"__dispatch__"));
  YACS::ENGINE::ComposedNode *runNode(getBoss()->getRunNode());
  YACS::ENGINE::Node *n101(runNode->cloneWithoutCompAndContDeepCpy(0,true));
  n10->edAddChild(n100);
  n10->edAddChild(n101);
  YACS::ENGINE::InputPort *dispatchIn(n100->edAddInputPort("i0",pyobjTC));
  n10->edAddCFLink(n100,n101);
  n1->edAddDFLink(n1->edGetSamplePort(),dispatchIn);
  std::ostringstream var1;
  for(std::vector< YACSEvalInputPort >::const_iterator it=inputs.begin();it!=inputs.end();it++)
    {
      if((*it).isRandomVar())
        {
          var1 << (*it).getName() << ",";
          YACS::ENGINE::OutputPort *myOut(n100->edAddOutputPort((*it).getName(),_generatedGraph->getTypeCode((*it).getTypeOfData())));
          std::string tmpPortName(runNode->getInPortName((*it).getUndergroundPtr()));
          YACS::ENGINE::InputPort *myIn(n101->getInputPort(tmpPortName));
          n10->edAddDFLink(myOut,myIn);
        }
    }
  std::ostringstream n100Script;  n100Script << var1.str() << "=i0\n";
  n100->setScript(n100Script.str());
  const std::vector<YACSEvalOutputPort *>& outputsOfInt(getBoss()->getOutputsOfInterest());
  for(std::vector< YACSEvalOutputPort * >::const_iterator it=outputsOfInt.begin();it!=outputsOfInt.end();it++)
    {
      YACS::ENGINE::TypeCode *tc(YACSEvalYFXPattern::CreateSeqTypeCodeFrom(_generatedGraph,(*it)->getTypeOfData()));
      YACS::ENGINE::InputPort *myIn(n2->edAddInputPort((*it)->getName(),tc));
      std::string tmpPortName(runNode->getOutPortName((*it)->getUndergroundPtr()));
      YACS::ENGINE::OutputPort *myOut(n101->getOutputPort(tmpPortName));
      _generatedGraph->edAddDFLink(myOut,myIn);
    }
  _generatedGraph->updateContainersAndComponents();
}

bool YACSEvalYFXGraphGenInteractive::go(const YACSEvalExecParams& params, YACSEvalSession *session) const
{
  YACS::ENGINE::Executor exe;
  exe.setKeepGoingProperty(!params.getStopASAPAfterErrorStatus());
  {
    MyAutoThreadSaver locker(!session->isAlreadyPyThreadSaved());
    exe.RunW(getUndergroundGeneratedGraph());
  }
  return getUndergroundGeneratedGraph()->getState()==YACS::DONE;
}

std::vector<YACSEvalSeqAny *> YACSEvalYFXGraphGenInteractive::getResults() const
{
  if(getUndergroundGeneratedGraph()->getState()!=YACS::DONE)
    throw YACS::Exception("YACSEvalYFXRunOnlyPattern::getResults : the execution did not finished correctly ! getResults should not be called !");
  const std::vector<YACSEvalOutputPort *>& outputsOfInt(getBoss()->getOutputsOfInterest()); 
  std::vector<YACSEvalSeqAny *> ret(outputsOfInt.size());
  YACS::ENGINE::Node *node(getUndergroundGeneratedGraph()->getChildByName(YACSEvalYFXGraphGen::GATHER_NODE_NAME));
  YACS::ENGINE::PythonNode *nodeC(dynamic_cast<YACS::ENGINE::PythonNode *>(node));
  if(!nodeC)
    throw YACS::Exception("YACSEvalYFXRunOnlyPattern::getResults : internal error !");
  std::size_t ii(0);
  for(std::vector< YACSEvalOutputPort * >::const_iterator it=outputsOfInt.begin();it!=outputsOfInt.end();it++,ii++)
    {
      YACS::ENGINE::InPort *input(nodeC->getInPort((*it)->getName()));
      YACS::ENGINE::InputPyPort *inputC(dynamic_cast<YACS::ENGINE::InputPyPort *>(input));
      if(!inputC)
        {
          std::ostringstream oss; oss << "YACSEvalYFXRunOnlyPattern::getResults : internal error for input \"" << (*it)->getName() << "\"";
          throw YACS::Exception(oss.str());
        }
      ret[ii]=YACSEvalYFXPattern::BuildValueInPort(inputC);
    }
  return ret;
}

////////////////////

void YACSEvalYFXGraphGenCluster::generateGraph()
{
  YACS::ENGINE::AutoGIL agil;
  if(_generatedGraph)
    { delete _generatedGraph; _generatedGraph=0; _FEInGeneratedGraph=0; }
  //
  const char EFXGenFileName[]="EFXGenFileName";
  const char EFXGenContent[]="import getpass,datetime,os\nn=datetime.datetime.now()\nreturn os.path.join(os.path.sep,\"tmp\",\"EvalYFX_%s_%s_%s.xml\"%(getpass.getuser(),n.strftime(\"%d%m%y\"),n.strftime(\"%H%M%S\")))";
  const char EFXGenContent2[]="import getpass,datetime\nn=datetime.datetime.now()\nreturn \"EvalYFX_%s_%s_%s\"%(getpass.getuser(),n.strftime(\"%d%m%y\"),n.strftime(\"%H%M%S\"))";
  //
  YACS::ENGINE::AutoPyRef func(YACS::ENGINE::evalPy(EFXGenFileName,EFXGenContent));
  YACS::ENGINE::AutoPyRef val(YACS::ENGINE::evalFuncPyWithNoParams(func));
  _locSchemaFile=PyString_AsString(val);
  func=YACS::ENGINE::evalPy(EFXGenFileName,EFXGenContent2);
  val=YACS::ENGINE::evalFuncPyWithNoParams(func);
  _jobName=PyString_AsString(val);
  //
  static const char LISTPYOBJ_STR[]="list[pyobj]";
  if(getBoss()->getOutputsOfInterest().empty())
    return ;
  YACS::ENGINE::RuntimeSALOME::setRuntime();
  YACS::ENGINE::RuntimeSALOME *r(YACS::ENGINE::getSALOMERuntime());
  _generatedGraph=r->createProc(DFT_PROC_NAME);
  YACS::ENGINE::TypeCode *pyobjTC(_generatedGraph->createInterfaceTc("python:obj:1.0","pyobj",std::list<YACS::ENGINE::TypeCodeObjref *>()));
  std::ostringstream oss; oss << "Loop_" << getBoss()->getRunNode()->getName();
  _generatedGraph->createType(YACSEvalAnyDouble::TYPE_REPR,"double");
  _generatedGraph->createType(YACSEvalAnyInt::TYPE_REPR,"int");
  //
  YACS::ENGINE::InlineNode *n0(r->createScriptNode(YACS::ENGINE::PythonNode::KIND,"__initializer__"));
  _generatedGraph->edAddChild(n0);
  YACS::ENGINE::TypeCode *listPyobjTC(_generatedGraph->createSequenceTc(LISTPYOBJ_STR,LISTPYOBJ_STR,pyobjTC));
  YACS::ENGINE::OutputPort *sender(n0->edAddOutputPort("sender",listPyobjTC));
  std::ostringstream var0;
  const std::vector< YACSEvalInputPort >& inputs(getBoss()->getInputs());
  for(std::vector< YACSEvalInputPort >::const_iterator it=inputs.begin();it!=inputs.end();it++)
    {
      if((*it).isRandomVar())
        {
          var0 << (*it).getName() << ",";
          YACS::ENGINE::TypeCode *tc(YACSEvalYFXPattern::CreateSeqTypeCodeFrom(_generatedGraph,(*it).getTypeOfData()));
          YACS::ENGINE::InputPort *inp(n0->edAddInputPort((*it).getName(),tc));
          YACS::ENGINE::InputPyPort *inpc(dynamic_cast<YACS::ENGINE::InputPyPort *>(inp));
          if(!inpc)
            throw YACS::Exception("YACSEvalYFXRunOnlyPattern::generateGraph : internal error 1 !");
          (*it).setUndergroundPortToBeSet(inpc);
        }
    }
  std::ostringstream n0Script; n0Script << "sender=zip(" << var0.str() << ")\n";
  n0->setScript(n0Script.str());
  //
  YACS::ENGINE::ForEachLoop *n1(r->createForEachLoop(oss.str(),pyobjTC));
  _FEInGeneratedGraph=n1;
  _generatedGraph->edAddChild(n1);
  _generatedGraph->edAddCFLink(n0,n1);
  _generatedGraph->edAddDFLink(sender,n1->edGetSeqOfSamplesPort());
  YACS::ENGINE::InlineNode *n2(r->createScriptNode(YACS::ENGINE::PythonNode::KIND,GATHER_NODE_NAME));
  _generatedGraph->edAddChild(n2);
  _generatedGraph->edAddCFLink(n1,n2);
  //
  YACS::ENGINE::Bloc *n10(r->createBloc(FIRST_FE_SUBNODE_NAME));
  n1->edAddChild(n10);
  YACS::ENGINE::InlineNode *n100(r->createScriptNode(YACS::ENGINE::PythonNode::KIND,"__dispatch__"));
  YACS::ENGINE::ComposedNode *runNode(getBoss()->getRunNode());
  YACS::ENGINE::Node *n101(runNode->cloneWithoutCompAndContDeepCpy(0,true));
  n10->edAddChild(n100);
  n10->edAddChild(n101);
  YACS::ENGINE::InputPort *dispatchIn(n100->edAddInputPort("i0",pyobjTC));
  n10->edAddCFLink(n100,n101);
  n1->edAddDFLink(n1->edGetSamplePort(),dispatchIn);
  std::ostringstream var1;
  for(std::vector< YACSEvalInputPort >::const_iterator it=inputs.begin();it!=inputs.end();it++)
    {
      if((*it).isRandomVar())
        {
          var1 << (*it).getName() << ",";
          YACS::ENGINE::OutputPort *myOut(n100->edAddOutputPort((*it).getName(),_generatedGraph->getTypeCode((*it).getTypeOfData())));
          std::string tmpPortName(runNode->getInPortName((*it).getUndergroundPtr()));
          YACS::ENGINE::InputPort *myIn(n101->getInputPort(tmpPortName));
          n10->edAddDFLink(myOut,myIn);
        }
    }
  std::ostringstream n100Script;  n100Script << var1.str() << "=i0\n";
  n100->setScript(n100Script.str());
  const std::vector<YACSEvalOutputPort *>& outputsOfInt(getBoss()->getOutputsOfInterest());
  std::ostringstream n2Script; n2Script << "zeRes=[";
  for(std::vector< YACSEvalOutputPort * >::const_iterator it=outputsOfInt.begin();it!=outputsOfInt.end();it++)
    {
      YACS::ENGINE::TypeCode *tc(YACSEvalYFXPattern::CreateSeqTypeCodeFrom(_generatedGraph,(*it)->getTypeOfData()));
      YACS::ENGINE::InputPort *myIn(n2->edAddInputPort((*it)->getName(),tc));
      n2Script << (*it)->getName() << ", ";
      std::string tmpPortName(runNode->getOutPortName((*it)->getUndergroundPtr()));
      YACS::ENGINE::OutputPort *myOut(n101->getOutputPort(tmpPortName));
      _generatedGraph->edAddDFLink(myOut,myIn);
    }
  n2Script << "]\nf=file(\"" << _jobName << "\",\"w\") ; f.write(str(zeRes)) ; del f";
  n2->setScript(n2Script.str());
  _generatedGraph->updateContainersAndComponents();
}

bool YACSEvalYFXGraphGenCluster::go(const YACSEvalExecParams& params, YACSEvalSession *session) const
{
  YACS::ENGINE::AutoGIL agil;
  getUndergroundGeneratedGraph()->saveSchema(_locSchemaFile);
  YACSEvalListOfResources *rss(getBoss()->getResourcesInternal());
  const YACSEvalParamsForCluster& cli(rss->getAddParamsForCluster());
  std::vector<std::string> machines(rss->getAllChosenMachines());
  if(machines.size()!=1)
    throw YACS::Exception("YACSEvalYFXGraphGenCluster::go : internal error ! In batch mode and not exactly one machine !");
  Engines::SalomeLauncher_var sl(session->getInternal()->goFetchingSalomeLauncherInNS());
  Engines::ResourceParameters rr;
  rr.name=CORBA::string_dup(machines[0].c_str());
  rr.hostname=CORBA::string_dup("");
  rr.can_launch_batch_jobs=true;
  rr.can_run_containers=true;
  rr.OS=CORBA::string_dup("Linux");
  rr.componentList.length(0);
  rr.nb_proc=rss->getNumberOfProcsDeclared();// <- important
  rr.mem_mb=1024;
  rr.cpu_clock=1000;
  rr.nb_node=1;// useless only nb_proc used.
  rr.nb_proc_per_node=1;// useless only nb_proc used.
  rr.policy=CORBA::string_dup("cycl");
  rr.resList.length(0);
  Engines::JobParameters jp;
  jp.job_name=CORBA::string_dup(_jobName.c_str());
  jp.job_type=CORBA::string_dup("yacs_file");
  jp.job_file=CORBA::string_dup(_locSchemaFile.c_str());
  jp.env_file=CORBA::string_dup("");
  jp.in_files.length(0);
  jp.out_files.length(1);
  jp.out_files[0]=CORBA::string_dup(_jobName.c_str());
  jp.work_directory=CORBA::string_dup(cli.getRemoteWorkingDir().c_str());
  jp.local_directory=CORBA::string_dup(cli.getLocalWorkingDir().c_str());
  jp.result_directory=CORBA::string_dup(cli.getLocalWorkingDir().c_str());
  jp.maximum_duration=CORBA::string_dup(cli.getMaxDuration().c_str());
  jp.resource_required=rr;
  jp.queue=CORBA::string_dup("");
  jp.exclusive=false;
  jp.mem_per_cpu=rr.mem_mb;
  jp.wckey=CORBA::string_dup(cli.getWCKey().c_str());
  jp.extra_params=CORBA::string_dup("");
  jp.specific_parameters.length(0);
  jp.launcher_file=CORBA::string_dup("");
  jp.launcher_args=CORBA::string_dup("");
  _jobid=sl->createJob(jp);
  sl->launchJob(_jobid);
  bool ret(false);
  while(true)
    {
      PyRun_SimpleString("import time ; time.sleep(10)");
      char *state(sl->getJobState(_jobid));//"CREATED", "IN_PROCESS", "QUEUED", "RUNNING", "PAUSED", "FINISHED" or "FAILED"
      std::string sstate(state);
      CORBA::string_free(state);
      if(sstate=="FINISHED" || sstate=="FAILED")
        {
          ret=sstate=="FINISHED";
          break;
        }
    }
  sl->getJobResults(_jobid,cli.getLocalWorkingDir().c_str());
  //
  try
    {
      std::ostringstream oss; oss << "import os" << std::endl << "p=os.path.join(\"" << cli.getLocalWorkingDir() << "\",\"" << _jobName  << "\")" << std::endl;
      oss << "if not os.path.exists(p):\n  return None\n";
      oss << "f=file(p,\"r\")" << std::endl;
      oss << "return eval(f.read())";
      std::string zeInput(oss.str());
      YACS::ENGINE::AutoPyRef func(YACS::ENGINE::evalPy("fetch",zeInput));
      YACS::ENGINE::AutoPyRef val(YACS::ENGINE::evalFuncPyWithNoParams(func));
      if(!PyList_Check(val))
        throw YACS::Exception("Fetched file does not contain a list !");
      Py_ssize_t sz(PyList_Size(val));
      _res.resize(sz);
      for(Py_ssize_t i=0;i<sz;i++)
        {
          std::vector<double>& res0(_res[i]);
          PyObject *elt0(PyList_GetItem(val,i));
          if(!PyList_Check(elt0))
            throw YACS::Exception("Fetched file does contain a list of list !");
          Py_ssize_t sz0(PyList_Size(elt0)); res0.resize(sz0);
          for(Py_ssize_t j=0;j<sz0;j++)
            {
              PyObject *elt1(PyList_GetItem(elt0,j));
              res0[j]=PyFloat_AsDouble(elt1);
            }
        }
      // cleanup
      std::ostringstream oss1; oss1 << "import os" << std::endl << "p=os.path.join(\"" << cli.getLocalWorkingDir() << "\",\"" << _jobName  << "\") ; os.remove(p)" << std::endl;
      std::string s1(oss1.str());
      PyRun_SimpleString(s1.c_str());
      if(!params.getFetchRemoteDirForClusterStatus())
        {
          std::ostringstream oss2; oss2 << "import os,shutil" << std::endl << "p=os.path.join(\"" << cli.getLocalWorkingDir() << "\",\"logs\") ; shutil.rmtree(p)" << std::endl;
          std::string s2(oss2.str());
          PyRun_SimpleString(s2.c_str());
        }
    }
  catch(YACS::Exception& e)
    {
      _errors=e.what();
      return false;
    }
  //
  return ret;
}

std::vector<YACSEvalSeqAny *> YACSEvalYFXGraphGenCluster::getResults() const
{
  std::size_t sz(_res.size());
  std::vector<YACSEvalSeqAny *> ret(sz);
  for(std::size_t i=0;i<sz;i++)
    {
      YACS::AutoCppPtr<YACSEvalSeqAnyDouble> elt(new YACSEvalSeqAnyDouble(_res[i]));
      ret[i]=elt.dettach();
    }
  return ret;
}
