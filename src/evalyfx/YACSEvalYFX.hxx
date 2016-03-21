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

#ifndef __YACSEVALYFX_HXX__
#define __YACSEVALYFX_HXX__

#include "YACSEvalYFXExport.hxx"
#include "YACSEvalExecParams.hxx"

#include <string>
#include <vector>

namespace YACS
{
  namespace ENGINE
  {
    class Proc;
  }
}

class YACSEvalObserver;
class YACSEvalSeqAny;
class YACSEvalSession;
class YACSEvalYFXPattern;
class YACSEvalInputPort;
class YACSEvalOutputPort;
class YACSEvalListOfResources;

class YACSEvalYFX
{
public:
  YACSEVALYFX_EXPORT static YACSEvalYFX *BuildFromFile(const std::string& xmlOfScheme);
  YACSEVALYFX_EXPORT static YACSEvalYFX *BuildFromScheme(YACS::ENGINE::Proc *scheme);
  YACSEVALYFX_EXPORT YACSEvalExecParams *getParams() const { return &_params; }
  YACSEVALYFX_EXPORT std::vector< YACSEvalInputPort * > getFreeInputPorts() const;
  YACSEVALYFX_EXPORT std::vector< YACSEvalOutputPort * > getFreeOutputPorts() const;
  YACSEVALYFX_EXPORT void lockPortsForEvaluation(const std::vector< YACSEvalInputPort * >& inputsOfInterest, const std::vector< YACSEvalOutputPort * >& outputsOfInterest);
  YACSEVALYFX_EXPORT void unlockAll();
  YACSEVALYFX_EXPORT bool isLocked() const;
  YACSEVALYFX_EXPORT YACSEvalListOfResources *giveResources();
  YACSEVALYFX_EXPORT bool run(YACSEvalSession *session, int& nbOfBranches);
  YACSEVALYFX_EXPORT void registerObserver(YACSEvalObserver *observer);
  YACSEVALYFX_EXPORT YACSEvalObserver *getObserver();
  YACSEVALYFX_EXPORT std::string getErrorDetailsInCaseOfFailure() const;
  YACSEVALYFX_EXPORT std::string getStatusOfRunStr() const;
  YACSEVALYFX_EXPORT std::vector<YACSEvalSeqAny *> getResults() const;
  YACSEVALYFX_EXPORT std::vector<YACSEvalSeqAny *> getResultsInCaseOfFailure(std::vector<unsigned int>& passedIds) const;
  //
  YACSEVALYFX_EXPORT YACS::ENGINE::Proc *getUndergroundGeneratedGraph() const;
  YACSEVALYFX_EXPORT void setParallelizeStatus(bool newVal);
  YACSEVALYFX_EXPORT bool getParallelizeStatus() const;
  YACSEVALYFX_EXPORT ~YACSEvalYFX();
private:
  YACSEvalYFX(YACS::ENGINE::Proc *scheme, bool ownScheme);
  void checkPortsForEvaluation(const std::vector< YACSEvalInputPort * >& inputs, const std::vector< YACSEvalOutputPort * >& outputs) const;
private:
  YACSEvalYFXPattern *_pattern;
  mutable YACSEvalExecParams _params;
};

#endif
