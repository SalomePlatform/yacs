// Copyright (C) 2012-2015  CEA/DEN, EDF R&D
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
#include "YACSEvalAutoPtr.hxx"

#include "ElementaryNode.hxx"
#include "RuntimeSALOME.hxx"
#include "InputPort.hxx"
#include "LinkInfo.hxx"
#include "TypeCode.hxx"
#include "Proc.hxx"

#include "PythonPorts.hxx"
#include "ForEachLoop.hxx"
#include "PythonNode.hxx"
#include "InlineNode.hxx"
#include "ServiceNode.hxx"

#include "ResourcesManager.hxx"

#include <map>
#include <numeric>
#include <sstream>

const char YACSEvalYFXPattern::DFT_PROC_NAME[]="YFX";

const char YACSEvalYFXRunOnlyPattern::GATHER_NODE_NAME[]="__gather__";

std::list< YACSEvalInputPort *> YACSEvalYFXPattern::getFreeInputPorts() const
{
  std::size_t sz(_inputs.size());
  std::list< YACSEvalInputPort *> ret;
  std::list< YACSEvalInputPort >::const_iterator it(_inputs.begin());
  for(std::size_t i=0;i<sz;i++,it++)
    ret.push_back(const_cast<YACSEvalInputPort *>(&(*it)));
  return ret;
}

std::list< YACSEvalOutputPort *> YACSEvalYFXPattern::getFreeOutputPorts() const
{
  std::size_t sz(_outputs.size());
  std::list< YACSEvalOutputPort *> ret;
  std::list< YACSEvalOutputPort >::const_iterator it(_outputs.begin());
  for(std::size_t i=0;i<sz;i++,it++)
    ret.push_back(const_cast<YACSEvalOutputPort *>(&(*it)));
  return ret;
}

YACSEvalYFXPattern *YACSEvalYFXPattern::FindPatternFrom(YACS::ENGINE::Proc *scheme, bool ownScheme)
{
  if(!scheme)
    throw YACS::Exception("YACSEvalYFXPattern::FindPatternFrom : input scheme must be not null !");
  {
      YACS::ENGINE::ComposedNode *zeRunNode(0);
      bool isMatchingRunOnlyPattern(YACSEvalYFXRunOnlyPattern::IsMatching(scheme,zeRunNode));
      if(isMatchingRunOnlyPattern)
        return new YACSEvalYFXRunOnlyPattern(scheme,ownScheme,zeRunNode);
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

YACSEvalYFXPattern::YACSEvalYFXPattern(YACS::ENGINE::Proc *scheme, bool ownScheme):_scheme(scheme),_ownScheme(ownScheme),_rm(new ResourcesManager_cpp),_res(0)
{
}

YACS::ENGINE::TypeCode *YACSEvalYFXPattern::createSeqTypeCodeFrom(YACS::ENGINE::Proc *scheme, const std::string& zeType)
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

void YACSEvalYFXPattern::cleanScheme()
{
  if(_ownScheme)
    delete _scheme;
  _scheme=0;
}

YACSEvalYFXPattern::~YACSEvalYFXPattern()
{
  delete _rm;
  delete _res;
}

YACSEvalYFXRunOnlyPattern::YACSEvalYFXRunOnlyPattern(YACS::ENGINE::Proc *scheme, bool ownScheme, YACS::ENGINE::ComposedNode *runNode):YACSEvalYFXPattern(scheme,ownScheme),_runNode(runNode),_commonSz(0),_generatedGraph(0)
{
  if(!_runNode)
    throw YACS::Exception("YACSEvalYFXRunOnlyPattern : internal run node must be not null !");
  buildInputPorts();
  buildOutputPorts();
}

void YACSEvalYFXRunOnlyPattern::setOutPortsOfInterestForEvaluation(std::size_t commonSize, const std::list<YACSEvalOutputPort *>& outputsOfInterest)
{
  checkNonLocked();
  _commonSz=commonSize;
  _outputsOfInterest=outputsOfInterest;
}

void YACSEvalYFXRunOnlyPattern::resetOutputsOfInterest()
{
  checkLocked();
  _commonSz=0;
  _outputsOfInterest.clear();
}

void YACSEvalYFXRunOnlyPattern::generateGraph()
{
  static const char LISTPYOBJ_STR[]="list[pyobj]";
  if(_commonSz==0 || _outputsOfInterest.empty())
    return ;
  YACS::ENGINE::RuntimeSALOME::setRuntime();
  YACS::ENGINE::RuntimeSALOME *r(YACS::ENGINE::getSALOMERuntime());
  _generatedGraph=r->createProc(DFT_PROC_NAME);
  YACS::ENGINE::TypeCode *pyobjTC(_generatedGraph->createInterfaceTc("python:obj:1.0","pyobj",std::list<YACS::ENGINE::TypeCodeObjref *>()));
  std::ostringstream oss; oss << "Loop_" << _runNode->getName();
  _generatedGraph->createType(YACSEvalAnyDouble::TYPE_REPR,"double");
  _generatedGraph->createType(YACSEvalAnyInt::TYPE_REPR,"int");
  //
  YACS::ENGINE::InlineNode *n0(r->createScriptNode(YACS::ENGINE::PythonNode::KIND,"__initializer__"));
  _generatedGraph->edAddChild(n0);
  YACS::ENGINE::TypeCode *listPyobjTC(_generatedGraph->createSequenceTc(LISTPYOBJ_STR,LISTPYOBJ_STR,pyobjTC));
  YACS::ENGINE::OutputPort *sender(n0->edAddOutputPort("sender",listPyobjTC));
  std::ostringstream var0;
  for(std::list< YACSEvalInputPort >::const_iterator it=_inputs.begin();it!=_inputs.end();it++)
    {
      std::size_t dummy;
      if((*it).hasSequenceOfValuesToEval(dummy))
        {
          var0 << (*it).getName() << ",";
          YACS::ENGINE::TypeCode *tc(createSeqTypeCodeFrom(_generatedGraph,(*it).getTypeOfData()));
          YACS::ENGINE::InputPort *inp(n0->edAddInputPort((*it).getName(),tc));
          YACS::ENGINE::InputPyPort *inpc(dynamic_cast<YACS::ENGINE::InputPyPort *>(inp));
          if(!inpc)
            throw YACS::Exception("YACSEvalYFXRunOnlyPattern::generateGraph : internal error 1 !");
          (*it).initializeUndergroundWithSeq(inpc);
        }
    }
  std::ostringstream n0Script; n0Script << "sender=zip(" << var0.str() << ")\n";
  n0->setScript(n0Script.str());
  //
  YACS::ENGINE::ForEachLoop *n1(r->createForEachLoop(oss.str(),pyobjTC));
  _generatedGraph->edAddChild(n1);
  _generatedGraph->edAddCFLink(n0,n1);
  _generatedGraph->edAddDFLink(sender,n1->edGetSeqOfSamplesPort());
  YACS::ENGINE::InlineNode *n2(r->createScriptNode(YACS::ENGINE::PythonNode::KIND,GATHER_NODE_NAME));
  _generatedGraph->edAddChild(n2);
  _generatedGraph->edAddCFLink(n1,n2);
  //
  YACS::ENGINE::Bloc *n10(r->createBloc("Bloc"));
  n1->edAddChild(n10);
  YACS::ENGINE::InlineNode *n100(r->createScriptNode(YACS::ENGINE::PythonNode::KIND,"__dispatch__"));
  YACS::ENGINE::Node *n101(_runNode->cloneWithoutCompAndContDeepCpy(0,true));
  n10->edAddChild(n100);
  n10->edAddChild(n101);
  YACS::ENGINE::InputPort *dispatchIn(n100->edAddInputPort("i0",pyobjTC));
  n10->edAddCFLink(n100,n101);
  n1->edAddDFLink(n1->edGetSamplePort(),dispatchIn);
  std::ostringstream var1;
  for(std::list< YACSEvalInputPort >::const_iterator it=_inputs.begin();it!=_inputs.end();it++)
    {
      std::size_t dummy;
      if((*it).hasSequenceOfValuesToEval(dummy))
        {
          var1 << (*it).getName() << ",";
          YACS::ENGINE::OutputPort *myOut(n100->edAddOutputPort((*it).getName(),_generatedGraph->getTypeCode((*it).getTypeOfData())));
          std::string tmpPortName(_runNode->getInPortName((*it).getUndergroundPtr()));
          YACS::ENGINE::InputPort *myIn(n101->getInputPort(tmpPortName));
          n10->edAddDFLink(myOut,myIn);
        }
    }
  std::ostringstream n100Script;  n100Script << var1.str() << "=i0\n";
  n100->setScript(n100Script.str());
  for(std::list< YACSEvalOutputPort * >::const_iterator it=_outputsOfInterest.begin();it!=_outputsOfInterest.end();it++)
    {
      YACS::ENGINE::TypeCode *tc(createSeqTypeCodeFrom(_generatedGraph,(*it)->getTypeOfData()));
      YACS::ENGINE::InputPort *myIn(n2->edAddInputPort((*it)->getName(),tc));
      std::string tmpPortName(_runNode->getOutPortName((*it)->getUndergroundPtr()));
      YACS::ENGINE::OutputPort *myOut(n101->getOutputPort(tmpPortName));
      _generatedGraph->edAddDFLink(myOut,myIn);
    }
  _generatedGraph->updateContainersAndComponents();
}

void YACSEvalYFXRunOnlyPattern::resetGeneratedGraph()
{
  delete _generatedGraph;
  _generatedGraph=0;
  resetResources();
}

int YACSEvalYFXRunOnlyPattern::assignNbOfBranches()
{
  checkAlreadyComputedResources();
  if(!_generatedGraph)
    throw YACS::Exception("YACSEvalYFXRunOnlyPattern::assignNbOfBranches : the generated graph has not been created !");
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
            throw YACS::Exception("YACSEvalYFXRunOnlyPattern::assignNbOfBranches : internal error 1 !");
        }
    }
  if(!zeMainNode)
    throw YACS::Exception("YACSEvalYFXRunOnlyPattern::assignNbOfBranches : internal error 2 !");
  unsigned int nbProcsDeclared(getResourcesInternal()->getNumberOfProcsDeclared());
  nbProcsDeclared=std::max(nbProcsDeclared,4u);
  int nbOfBranch(nbProcsDeclared/getResourcesInternal()->getMaxLevelOfParallelism());
  nbOfBranch=std::max(nbOfBranch,1);
  YACS::ENGINE::InputPort *zeInputToSet(zeMainNode->edGetNbOfBranchesPort());
  YACS::ENGINE::AnyInputPort *zeInputToSetC(dynamic_cast<YACS::ENGINE::AnyInputPort *>(zeInputToSet));
  if(!zeInputToSetC)
    throw YACS::Exception("YACSEvalYFXRunOnlyPattern::assignNbOfBranches : internal error 3 !");
  YACS::ENGINE::Any *a(YACS::ENGINE::AtomAny::New(nbOfBranch));
  zeInputToSetC->put(a);
  zeInputToSetC->exSaveInit();
  a->decrRef();
  return nbOfBranch;
}

bool YACSEvalYFXRunOnlyPattern::isLocked() const
{
  return _generatedGraph!=0;
}

YACSEvalListOfResources *YACSEvalYFXRunOnlyPattern::giveResources()
{
  checkLocked();
  if(!isAlreadyComputedResources())
    {
      YACS::ENGINE::DeploymentTree dt(_runNode->getDeploymentTree());
      YACSEvalListOfResources *res(new YACSEvalListOfResources(_runNode->getMaxLevelOfParallelism(),getCatalogInAppli(),dt));
      setResources(res);
    }
  return getResourcesInternal();
}

YACS::ENGINE::Proc *YACSEvalYFXRunOnlyPattern::getUndergroundGeneratedGraph() const
{
  return _generatedGraph;
}

std::vector<YACSEvalSeqAny *> YACSEvalYFXRunOnlyPattern::getResults() const
{
  if(_generatedGraph->getState()!=YACS::DONE)
    throw YACS::Exception("YACSEvalYFXRunOnlyPattern::getResults : the execution did not finished correctly ! getResults should not be called !");
  std::vector<YACSEvalSeqAny *> ret(_outputsOfInterest.size());
  YACS::ENGINE::Node *node(_generatedGraph->getChildByName(GATHER_NODE_NAME));
  YACS::ENGINE::PythonNode *nodeC(dynamic_cast<YACS::ENGINE::PythonNode *>(node));
  if(!nodeC)
    throw YACS::Exception("YACSEvalYFXRunOnlyPattern::getResults : internal error !");
  std::size_t ii(0);
  for(std::list< YACSEvalOutputPort * >::const_iterator it=_outputsOfInterest.begin();it!=_outputsOfInterest.end();it++,ii++)
    {
      YACS::ENGINE::InPort *input(nodeC->getInPort((*it)->getName()));
      YACS::ENGINE::InputPyPort *inputC(dynamic_cast<YACS::ENGINE::InputPyPort *>(input));
      if(!inputC)
        {
          std::ostringstream oss; oss << "YACSEvalYFXRunOnlyPattern::getResults : internal error for input \"" << (*it)->getName() << "\"";
          throw YACS::Exception(oss.str());
        }
      ret[ii]=BuildValueInPort(inputC);
    }
  return ret;
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

void YACSEvalYFXRunOnlyPattern::buildOutputPorts()
{
  _outputs.clear();
  std::list< YACS::ENGINE::OutputPort *> allOutputPorts(_runNode->getSetOfOutputPort());
  std::vector<std::string> allNames;
  for(std::list< YACS::ENGINE::OutputPort *>::const_iterator it=allOutputPorts.begin();it!=allOutputPorts.end();it++)
    {
      YACS::ENGINE::OutputPort *elt(*it);
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

