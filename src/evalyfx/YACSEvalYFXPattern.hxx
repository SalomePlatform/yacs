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
    class SequenceAny;
  }
}

class YACSEvalYFX;
class YACSEvalSession;
class YACSEvalObserver;
class YACSEvalExecParams;
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
  YACSEvalListOfResources *getResourcesInternal() const { return _res; }
  virtual void setOutPortsOfInterestForEvaluation(const std::vector<YACSEvalOutputPort *>& outputs) = 0;
  virtual void resetOutputsOfInterest() = 0;
  virtual void generateGraph() = 0;
  virtual void resetGeneratedGraph() = 0;
  virtual void assignRandomVarsInputs() = 0;
  virtual int assignNbOfBranches() = 0;
  virtual bool isLocked() const = 0;
  virtual YACSEvalListOfResources *giveResources() = 0;
  virtual YACS::ENGINE::Proc *getUndergroundGeneratedGraph() const = 0;
  virtual std::string getErrorDetailsInCaseOfFailure() const = 0;
  virtual std::string getStatusOfRunStr() const = 0;
  virtual std::vector<YACSEvalSeqAny *> getResults() const = 0;
  virtual std::vector<YACSEvalSeqAny *> getResultsInCaseOfFailure(std::vector<unsigned int>& passedIds) const = 0;
  virtual bool go(const YACSEvalExecParams& params, YACSEvalSession *session) const = 0;
public://for _gen
  const std::vector< YACSEvalInputPort >& getInputs() const { return _inputs; }
  static YACS::ENGINE::TypeCode *CreateSeqTypeCodeFrom(YACS::ENGINE::Proc *scheme, const std::string& zeType);
  static YACSEvalSeqAny *BuildValueInPort(YACS::ENGINE::InputPyPort *port);
protected:
  YACSEvalYFXPattern(YACSEvalYFX *boss, YACS::ENGINE::Proc *scheme, bool ownScheme);
  void setResources(YACSEvalListOfResources *res);
  void resetResources();
  ResourcesManager_cpp *getCatalogInAppli() const { return _rm; }
  static YACSEvalSeqAny *BuildValueFromEngineFrmt(YACS::ENGINE::SequenceAny *data);
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
public:
  static const char ST_OK[]; // execution goes to the end without any trouble -> results can be exploited without any problem with getResults and getResultsInCaseOfFailure.
  static const char ST_FAILED[]; // execution has reached some failed evaluation (normal errors due to incapacity of one node to evaluate) -> results can be exploited without any problem with getResultsInCaseOfFailure
  static const char ST_ERROR[]; // execution has encountered hard errors (SIGSEGV in a server, internal error in YACS) -> results can be exploited with getResultsInCaseOfFailure but you can't make hypothesis for other ids not in passedIds.
  static const std::size_t MAX_LGTH_OF_INP_DUMP;
};

class YACSEvalYFXGraphGen;

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
  std::string getErrorDetailsInCaseOfFailure() const;
  std::string getStatusOfRunStr() const;
  std::vector<YACSEvalSeqAny *> getResults() const;
  std::vector<YACSEvalSeqAny *> getResultsInCaseOfFailure(std::vector<unsigned int>& passedIds) const;
  bool go(const YACSEvalExecParams& params, YACSEvalSession *session) const;
  //
  YACS::ENGINE::ForEachLoop *getUndergroundForEach() const;
  static bool IsMatching(YACS::ENGINE::Proc *scheme, YACS::ENGINE::ComposedNode *& runNode);
private:
  void emitStart() const;
  void buildInputPorts();
  void buildOutputPorts();
  YACSEvalYFXGraphGen *getGenerator() const;
public://for _gen
  const std::vector<YACSEvalOutputPort *>& getOutputsOfInterest() const { return _outputsOfInterest; }
  YACS::ENGINE::ComposedNode *getRunNode() const { return _runNode; }
private:
  bool _lockedStatus;
  YACS::ENGINE::ComposedNode *_runNode;
  std::vector<YACSEvalOutputPort *> _outputsOfInterest;
  YACSEvalYFXGraphGen *_gen;
  YACSEvalYFXRunOnlyPatternInternalObserver *_obs;
};

class YACSEvalYFXGraphGen
{
protected:
  YACSEvalYFXGraphGen(YACSEvalYFXRunOnlyPattern *boss);
  YACSEvalYFXRunOnlyPattern *getBoss() const { return _boss; }
public:
  virtual ~YACSEvalYFXGraphGen();
  virtual void generateGraph() = 0;
  virtual bool go(const YACSEvalExecParams& params, YACSEvalSession *session) const = 0;
  virtual std::vector<YACSEvalSeqAny *> getResults() const = 0;
  bool isLocked() const;
  int assignNbOfBranches();
  void resetGeneratedGraph();
  YACS::ENGINE::Proc *getUndergroundGeneratedGraph() const { return _generatedGraph; }
  YACS::ENGINE::ForEachLoop *getUndergroundForEach() const { return _FEInGeneratedGraph; }
private:
  YACSEvalYFXRunOnlyPattern *_boss;
protected:
  YACS::ENGINE::Proc *_generatedGraph;
  YACS::ENGINE::ForEachLoop *_FEInGeneratedGraph;
public:
  static const char DFT_PROC_NAME[];
  static const char FIRST_FE_SUBNODE_NAME[];
  static const char GATHER_NODE_NAME[];
};

class YACSEvalYFXGraphGenInteractive : public YACSEvalYFXGraphGen
{
public:
  YACSEvalYFXGraphGenInteractive(YACSEvalYFXRunOnlyPattern *boss):YACSEvalYFXGraphGen(boss) { }
  void generateGraph();
  bool go(const YACSEvalExecParams& params, YACSEvalSession *session) const;
  std::vector<YACSEvalSeqAny *> getResults() const;
};

class YACSEvalYFXGraphGenCluster : public YACSEvalYFXGraphGen
{
public:
  YACSEvalYFXGraphGenCluster(YACSEvalYFXRunOnlyPattern *boss):YACSEvalYFXGraphGen(boss) { }
  void generateGraph();
  bool go(const YACSEvalExecParams& params, YACSEvalSession *session) const;
  std::vector<YACSEvalSeqAny *> getResults() const;
private:
  std::string _locSchemaFile;
  std::string _jobName;
  mutable int _jobid;
  mutable std::string _errors;
  mutable std::vector< std::vector<double> > _res;
};

#endif
