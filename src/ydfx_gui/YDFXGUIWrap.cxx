// Copyright (C) 2016  CEA/DEN, EDF R&D
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

#include "YDFXGUIWrap.hxx"

#include "YACSEvalYFX.hxx"
#include "YACSEvalPort.hxx"
#include "YACSEvalSeqAny.hxx"

#include <limits>

std::vector< YACSEvalInputPort * > YACSEvalYFXWrap::getFreeInputPorts() const
{
  return _efx->getFreeInputPorts();
}

std::vector< YACSEvalOutputPort * > YACSEvalYFXWrap::getFreeOutputPorts() const
{
  return _efx->getFreeOutputPorts();
}

YACSEvalExecParams *YACSEvalYFXWrap::getParams() const
{
  return _efx->getParams();
}

bool YACSEvalYFXWrap::isLocked() const
{
  return _efx->isLocked();
}

YACSEvalListOfResources *YACSEvalYFXWrap::giveResources()
{
  return _efx->giveResources();
}

class AutoRunningDeclarator
{
public:
  AutoRunningDeclarator(YACSEvalYFXWrap *wrap):_wrap(wrap) { _wrap->setRunningStatus(true); }
  ~AutoRunningDeclarator() { _wrap->setRunningStatus(false); }
private:
  YACSEvalYFXWrap *_wrap;
};

bool YACSEvalYFXWrap::run(YACSEvalSession *session, int& nbOfBranches)
{
  AutoRunningDeclarator ard(this);
  return _efx->run(session,nbOfBranches);
}

void YACSEvalYFXWrap::registerObserver(YACSEvalObserver *observer)
{
  _efx->registerObserver(observer);
}

void YACSEvalYFXWrap::unlockAll()
{
  _efx->unlockAll();
  emit lockSignal(false);
}

void YACSEvalYFXWrap::lockPortsForEvaluation()
{
  bool lockedOrNot(isLocked());
  std::vector< YACSEvalInputPort * > inps(getFreeInputPorts()),inps2;
  foreach(YACSEvalInputPort *inp,inps)
    {
      if(inp->isRandomVar())
        inps2.push_back(inp);
    }
  std::vector< YACSEvalOutputPort * > outps(getFreeOutputPorts()),outps2;
  foreach(YACSEvalOutputPort *outp,outps)
    {
      if(outp->isQOfInterest())
        outps2.push_back(outp);
    }
  if(lockedOrNot)
    _efx->unlockAll();
  _efx->lockPortsForEvaluation(inps2,outps2);
  _efx->giveResources();//do not remove this line to generate resource info
  if(!lockedOrNot)
    emit lockSignal(true);
}

int YACSEvalYFXWrap::getNbOfItems() const
{
  std::vector< YACSEvalInputPort * > inps(getFreeInputPorts());
  int nbOfRandom(0),refSz(std::numeric_limits<int>::max());
  foreach(YACSEvalInputPort *inp,inps)
    {
      if(!inp->isRandomVar())
        continue;
      YACSEvalSeqAny *seq(inp->getSequenceOfValuesToEval());
      if(!seq)
        return 0;
      if(nbOfRandom==0)
        refSz=seq->size();
      if(refSz!=seq->size())
        return 0;
      nbOfRandom++;
    }
  if(nbOfRandom==0)
    return 0;
  return refSz;
}

void lockPortsForEvaluation(const std::vector< YACSEvalInputPort * >& inputsOfInterest, const std::vector< YACSEvalOutputPort * >& outputsOfInterest);

YACSEvalYFXWrap::~YACSEvalYFXWrap()
{
  delete _efx;
}

void YACSEvalYFXWrap::setRunningStatus(bool status)
{
  if(_isRunning!=status)
    emit runSignal(status);
  _isRunning=status;
}

void YACSEvalYFXWrap::updateSequencesStatus()
{
  int nbOfVals;
  bool newStatus(computeSequencesStatus(nbOfVals));
  if(_isSeqOfValsSet!=newStatus)
    {
      _isSeqOfValsSet=newStatus;
      emit sequencesAreSetSignal(_isSeqOfValsSet);
    }
}

bool YACSEvalYFXWrap::computeSequencesStatus(int& nbOfVals)
{
  std::vector< YACSEvalInputPort * > inputs(_efx->getFreeInputPorts());
  std::size_t sz(0),nbOfRandomVars(0);
  foreach(YACSEvalInputPort *input,inputs)
    {
      if(!input->isRandomVar())
        continue;
      nbOfRandomVars++;
      if(!input->hasSequenceOfValuesToEval())
        return false;
      YACSEvalSeqAny *seq(input->getSequenceOfValuesToEval());
      sz=seq->size();
    }
  if(nbOfRandomVars==0 || sz==0)
    return false;
  foreach(YACSEvalInputPort *input,inputs)
    {
      if(!input->isRandomVar())
        continue;
      if(sz!=input->getSequenceOfValuesToEval()->size())
        return false;
    }
  nbOfVals=sz;
  return true;
}
