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

class MyAutoThreadSaver
{
public:
  MyAutoThreadSaver():_save(PyEval_SaveThread()) { }
  ~MyAutoThreadSaver() { PyEval_RestoreThread(_save); }
private:
  PyThreadState *_save;
};


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

std::list< YACSEvalInputPort * > YACSEvalYFX::getFreeInputPorts() const
{
  return _pattern->getFreeInputPorts();
}

std::list< YACSEvalOutputPort * > YACSEvalYFX::getFreeOutputPorts() const
{
  return _pattern->getFreeOutputPorts();
}

void YACSEvalYFX::lockPortsForEvaluation(const std::list< YACSEvalOutputPort * >& outputsOfInterest)
{
  std::size_t sz(checkPortsForEvaluation(outputsOfInterest));
  _pattern->setOutPortsOfInterestForEvaluation(sz,outputsOfInterest);
  _pattern->generateGraph();
}

void YACSEvalYFX::unlockAll()
{
  std::list< YACSEvalInputPort * > allInputs(getFreeInputPorts());
  for(std::list< YACSEvalInputPort * >::const_iterator it=allInputs.begin();it!=allInputs.end();it++)
    (*it)->unlock();
  _pattern->resetOutputsOfInterest();
  _pattern->resetGeneratedGraph();
}

bool YACSEvalYFX::isLocked() const
{
  return _pattern->isLocked();
}

YACSEvalListOfResources *YACSEvalYFX::giveResources()
{
  return _pattern->giveResources();
}

bool YACSEvalYFX::run(YACSEvalSession *session, int& nbOfBranches)
{
  YACSEvalListOfResources *rss(giveResources());
  if(!rss->isInteractive())
    throw YACS::Exception("YACSEvalYFX::run : not implemented yet for non interactive !");
  YACSEvalSession *mySession(session);
  YACS::AutoCppPtr<YACSEvalSession> loc;
  if(!session)
    {
      throw YACS::Exception("YACSEvalYFX::run : input session in null !");
      /*loc=new YACSEvalSession;
      mySession=loc;*/
    }
  rss->apply();
  nbOfBranches=_pattern->assignNbOfBranches();
  mySession->launch();
  YACS::ENGINE::Executor exe;
  //
  {
    MyAutoThreadSaver locker;
    exe.RunW(getUndergroundGeneratedGraph());
  }
  return getUndergroundGeneratedGraph()->getState()==YACS::DONE;
}

std::vector<YACSEvalSeqAny *> YACSEvalYFX::getResults() const
{
  return _pattern->getResults();
}

YACS::ENGINE::Proc *YACSEvalYFX::getUndergroundGeneratedGraph() const
{
  return _pattern->getUndergroundGeneratedGraph();
}

YACSEvalYFX::YACSEvalYFX(YACS::ENGINE::Proc *scheme, bool ownScheme):_pattern(0)
{
  _pattern=YACSEvalYFXPattern::FindPatternFrom(scheme,ownScheme);
}

std::size_t YACSEvalYFX::checkPortsForEvaluation(const std::list< YACSEvalOutputPort * >& outputs) const
{
  std::list< YACSEvalInputPort * > allInputs(getFreeInputPorts());
  std::list< YACSEvalOutputPort * > allOutputs(getFreeOutputPorts());
  std::size_t sz(std::numeric_limits<std::size_t>::max());
  for(std::list< YACSEvalInputPort * >::const_iterator it=allInputs.begin();it!=allInputs.end();it++)
    {
      std::size_t mySz;
      if(!(*it)->isOKForLock() && !(*it)->hasSequenceOfValuesToEval(mySz))
        {
          std::ostringstream oss; oss << "YACSEvalYFX::checkPortsForEvaluation : input port with name \"" << (*it)->getName() << "\" is not set properly !";
          throw YACS::Exception(oss.str());
        }
      if((*it)->hasSequenceOfValuesToEval(mySz))
        {
          if(sz==std::numeric_limits<std::size_t>::max())
            sz=mySz;
          else
            {
              if(sz!=mySz)
                {
                  std::ostringstream oss; oss << "YACSEvalYFX::checkPortsForEvaluation : input port with name \"" << (*it)->getName() << "\" is declared as to be evaluated on array ! But size of array is not the same than the others !";
                  throw YACS::Exception(oss.str());
                }
            }
        }
    }
  for(std::list< YACSEvalOutputPort * >::const_iterator it=outputs.begin();it!=outputs.end();it++)
    if(std::find(allOutputs.begin(),allOutputs.end(),*it)==allOutputs.end())
      throw YACS::Exception("YACSEvalYFX::lockPortsForEvaluation : one of output is not part of this !");
  std::set< YACSEvalOutputPort * > soutputs(outputs.begin(),outputs.end());
  if(soutputs.size()!=outputs.size())
    throw YACS::Exception("YACSEvalYFX::lockPortsForEvaluation : each elt in outputs must appear once !");
  for(std::list< YACSEvalInputPort * >::const_iterator it=allInputs.begin();it!=allInputs.end();it++)
    (*it)->lock();
  return sz;
}

YACSEvalYFX::~YACSEvalYFX()
{
  delete _pattern;
}
