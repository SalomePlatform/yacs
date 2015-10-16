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

#ifndef __YACSEVALYFXPATTERN_HXX__
#define __YACSEVALYFXPATTERN_HXX__

#include "YACSEvalPort.hxx"

#include <vector>

#ifdef WIN32
#define NOMINMAX
#endif

namespace YACS
{
  namespace ENGINE
  {
    class Proc;
    class TypeCode;
    class ForEachLoop;
    class ComposedNode;
    class InputPyPort;
  }
}

class YACSEvalYFX;
class YACSEvalObserver;
class YACSEvalListOfResources;
class ResourcesManager_cpp;
class YACSEvalYFXRunOnlyPatternInternalObserver;

class YACSEvalYFXPattern
{
public:
  virtual ~YACSEvalYFXPattern();
  std::vector< YACSEvalInputPort *> getFreeInputPorts() const;
  std::vector< YACSEvalOutputPort *> getFreeOutputPorts() const;
  static YACSEvalYFXPattern *FindPatternFrom(YACSEvalYFX *boss, YACS::ENGINE::Proc *scheme, bool ownScheme);
  void setParallelizeStatus(bool newVal) { _parallelizeStatus=newVal; }
  bool getParallelizeStatus() const { return _parallelizeStatus; }
  bool isAlreadyComputedResources() const;
  void checkNonAlreadyComputedResources() const;
  void checkAlreadyComputedResources() const;
  void checkLocked() const;
  void checkNonLocked() const;
  static void CheckNodeIsOK(YACS::ENGINE::ComposedNode *node);
  void registerObserver(YACSEvalObserver *observer);
  YACSEvalObserver *getObserver() const { return _observer; }
  YACSEvalYFX *getBoss() const { return _boss; }
  virtual void setOutPortsOfInterestForEvaluation(const std::vector<YACSEvalOutputPort *>& outputs) = 0;
  virtual void resetOutputsOfInterest() = 0;
  virtual void generateGraph() = 0;
  virtual void resetGeneratedGraph() = 0;
  virtual void assignRandomVarsInputs() = 0;
  virtual int assignNbOfBranches() = 0;
  virtual bool isLocked() const = 0;
  virtual YACSEvalListOfResources *giveResources() = 0;
  virtual YACS::ENGINE::Proc *getUndergroundGeneratedGraph() const = 0;
  virtual std::vector<YACSEvalSeqAny *> getResults() const = 0;
  virtual void emitStart() const = 0;
public:
  static const char DFT_PROC_NAME[];
protected:
  YACSEvalYFXPattern(YACSEvalYFX *boss, YACS::ENGINE::Proc *scheme, bool ownScheme);
  YACS::ENGINE::TypeCode *createSeqTypeCodeFrom(YACS::ENGINE::Proc *scheme, const std::string& zeType);
  void setResources(YACSEvalListOfResources *res);
  void resetResources();
  YACSEvalListOfResources *getResourcesInternal() const { return _res; }
  ResourcesManager_cpp *getCatalogInAppli() const { return _rm; }
  static YACSEvalSeqAny *BuildValueInPort(YACS::ENGINE::InputPyPort *port);
private:
  void cleanScheme();
private:
  YACSEvalYFX *_boss;
  bool _ownScheme;
  bool _parallelizeStatus;
  YACS::ENGINE::Proc *_scheme;
  ResourcesManager_cpp *_rm;
  YACSEvalListOfResources *_res;
  mutable YACSEvalObserver *_observer;
protected:
  std::vector< YACSEvalInputPort > _inputs;
  std::vector< YACSEvalOutputPort > _outputs;
};

class YACSEvalYFXRunOnlyPattern : public YACSEvalYFXPattern
{
public:
  YACSEvalYFXRunOnlyPattern(YACSEvalYFX *boss, YACS::ENGINE::Proc *scheme, bool ownScheme, YACS::ENGINE::ComposedNode *runNode);
  ~YACSEvalYFXRunOnlyPattern();
  void setOutPortsOfInterestForEvaluation(const std::vector<YACSEvalOutputPort *>& outputsOfInterest);
  void resetOutputsOfInterest();
  void generateGraph();
  void resetGeneratedGraph();
  int assignNbOfBranches();
  void assignRandomVarsInputs();
  bool isLocked() const;
  YACSEvalListOfResources *giveResources();
  YACS::ENGINE::Proc *getUndergroundGeneratedGraph() const;
  std::vector<YACSEvalSeqAny *> getResults() const;
  void emitStart() const;
  //
  YACS::ENGINE::ForEachLoop *getUndergroundForEach() const { return _FEInGeneratedGraph; }
  static bool IsMatching(YACS::ENGINE::Proc *scheme, YACS::ENGINE::ComposedNode *& runNode);
public:
  static const char GATHER_NODE_NAME[];
private:
  void buildInputPorts();
  void buildOutputPorts();
private:
  YACS::ENGINE::ComposedNode *_runNode;
  std::vector<YACSEvalOutputPort *> _outputsOfInterest;
  YACS::ENGINE::Proc *_generatedGraph;
  YACS::ENGINE::ForEachLoop *_FEInGeneratedGraph;
  YACSEvalYFXRunOnlyPatternInternalObserver *_obs;
};

#endif
