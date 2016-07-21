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

#include "YACSEvalYFX.hxx"
#include "YACSEvalYFXPattern.hxx"
#include "YACSEvalAutoPtr.hxx"
#include "YACSEvalResource.hxx"
#include "YACSEvalSession.hxx"
#include "YACSEvalPort.hxx"
#include "RuntimeSALOME.hxx"
#include "Executor.hxx"
#include "Proc.hxx"
#include "Exception.hxx"
#include "parsers.hxx"

#include <algorithm>
#include <sstream>
#include <limits>
#include <set>

#include <Python.h>

YACSEvalYFX *YACSEvalYFX::BuildFromFile(const std::string& xmlOfScheme)
{
  YACS::ENGINE::RuntimeSALOME::setRuntime();
  YACS::YACSLoader l;
  YACS::ENGINE::Proc *scheme(l.load(xmlOfScheme.c_str()));
  return new YACSEvalYFX(scheme,true);
}

YACSEvalYFX *YACSEvalYFX::BuildFromScheme(YACS::ENGINE::Proc *scheme)
{
  return new YACSEvalYFX(scheme,false);
}

std::vector< YACSEvalInputPort * > YACSEvalYFX::getFreeInputPorts() const
{
  return _pattern->getFreeInputPorts();
}

std::vector< YACSEvalOutputPort * > YACSEvalYFX::getFreeOutputPorts() const
{
  return _pattern->getFreeOutputPorts();
}

void YACSEvalYFX::lockPortsForEvaluation(const std::vector< YACSEvalInputPort * >& inputsOfInterest, const std::vector< YACSEvalOutputPort * >& outputsOfInterest)
{
  checkPortsForEvaluation(inputsOfInterest,outputsOfInterest);
  _pattern->setOutPortsOfInterestForEvaluation(outputsOfInterest);
}

void YACSEvalYFX::unlockAll()
{
  std::vector< YACSEvalInputPort * > allInputs(getFreeInputPorts());
  for(std::vector< YACSEvalInputPort * >::const_iterator it=allInputs.begin();it!=allInputs.end();it++)
    (*it)->unlock();
  _pattern->resetOutputsOfInterest();
  _pattern->resetGeneratedGraph();
}

bool YACSEvalYFX::isLocked() const
{
  return _pattern->isLocked();
}

/*!
 * Caller is not responsible from returned pointer
 */
YACSEvalListOfResources *YACSEvalYFX::giveResources()
{
  return _pattern->giveResources();
}

bool YACSEvalYFX::run(YACSEvalSession *session, int& nbOfBranches)
{
  _pattern->generateGraph();
  if(!session)
    {
      throw YACS::Exception("YACSEvalYFX::run : input session in null !");
    }
  session->launch();
  YACSEvalListOfResources *rss(giveResources());
  rss->checkOKForRun();
  _pattern->assignRandomVarsInputs();
  rss->apply();
  nbOfBranches=_pattern->assignNbOfBranches();
  return _pattern->go(_params,session);
}

void YACSEvalYFX::registerObserver(YACSEvalObserver *observer)
{
  if(!_pattern)
    throw YACS::Exception("YACSEvalYFX::registerObserver : no pattern !");
  _pattern->registerObserver(observer);
}

YACSEvalObserver *YACSEvalYFX::getObserver()
{
  if(!_pattern)
    throw YACS::Exception("YACSEvalYFX::getObserver : no pattern !");
  return _pattern->getObserver();
}

std::string YACSEvalYFX::getErrorDetailsInCaseOfFailure() const
{
  return _pattern->getErrorDetailsInCaseOfFailure();
}

std::string YACSEvalYFX::getStatusOfRunStr() const
{
  return _pattern->getStatusOfRunStr();
}

std::vector<YACSEvalSeqAny *> YACSEvalYFX::getResults() const
{
  return _pattern->getResults();
}

std::vector<YACSEvalSeqAny *> YACSEvalYFX::getResultsInCaseOfFailure(std::vector<unsigned int>& passedIds) const
{
  return _pattern->getResultsInCaseOfFailure(passedIds);
}

YACS::ENGINE::Proc *YACSEvalYFX::getUndergroundGeneratedGraph() const
{
  return _pattern->getUndergroundGeneratedGraph();
}

void YACSEvalYFX::setParallelizeStatus(bool newVal)
{
  _pattern->setParallelizeStatus(newVal);
}

bool YACSEvalYFX::getParallelizeStatus() const
{
  return _pattern->getParallelizeStatus();
}

YACSEvalYFX::YACSEvalYFX(YACS::ENGINE::Proc *scheme, bool ownScheme):_pattern(0)
{
  _pattern=YACSEvalYFXPattern::FindPatternFrom(this,scheme,ownScheme);
}

void YACSEvalYFX::checkPortsForEvaluation(const std::vector< YACSEvalInputPort * >& inputs, const std::vector< YACSEvalOutputPort * >& outputs) const
{
  std::set<YACSEvalInputPort * > inputs2(inputs.begin(),inputs.end());
  std::vector< YACSEvalInputPort * > allInputs(getFreeInputPorts());
  std::vector< YACSEvalOutputPort * > allOutputs(getFreeOutputPorts());
  for(std::vector< YACSEvalInputPort * >::const_iterator it=allInputs.begin();it!=allInputs.end();it++)
    {
      if(inputs2.find(*it)==inputs2.end())
        {
          if(!(*it)->isOKForLock())
            {
              std::ostringstream oss; oss << "YACSEvalYFX::checkPortsForEvaluation : input port with name \"" << (*it)->getName() << "\" is not set properly !";
              throw YACS::Exception(oss.str());
            }
        }
    }
  for(std::vector< YACSEvalOutputPort * >::const_iterator it=outputs.begin();it!=outputs.end();it++)
    if(std::find(allOutputs.begin(),allOutputs.end(),*it)==allOutputs.end())
      throw YACS::Exception("YACSEvalYFX::lockPortsForEvaluation : one of output is not part of this !");
  std::set< YACSEvalOutputPort * > soutputs(outputs.begin(),outputs.end());
  if(soutputs.size()!=outputs.size())
    throw YACS::Exception("YACSEvalYFX::lockPortsForEvaluation : each elt in outputs must appear once !");
  for(std::vector< YACSEvalInputPort * >::const_iterator it=allInputs.begin();it!=allInputs.end();it++)
    {
      (*it)->declareRandomnessStatus(inputs2.find(*it)!=inputs2.end());
      (*it)->lock();
    }
}

YACSEvalYFX::~YACSEvalYFX()
{
  delete _pattern;
}
