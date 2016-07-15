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

#ifndef __YACSEVALRESOURCE_HXX__
#define __YACSEVALRESOURCE_HXX__

#include "YACSEvalYFXExport.hxx"

#include <map>
#include <string>
#include <vector>

#ifdef WIN32
#define NOMINMAX
#endif

namespace YACS
{
  namespace ENGINE
  {
    class Container;
    class DeploymentTree;
  }
}

class YACSEvalNonConstLocker
{
public:
  YACSEVALYFX_EXPORT YACSEvalNonConstLocker():_isLocked(false) { }
  YACSEVALYFX_EXPORT YACSEvalNonConstLocker(const YACSEvalNonConstLocker& other):_isLocked(false) { }
  YACSEVALYFX_EXPORT void lock() { _isLocked=true; }
  YACSEVALYFX_EXPORT bool isLocked() const { return _isLocked; }
  YACSEVALYFX_EXPORT void checkNonLocked() const;
  YACSEVALYFX_EXPORT void checkLocked() const;
private:
  bool _isLocked;
};

class ParserResourcesType;
class YACSEvalResource;

class YACSEvalVirtualYACSContainer : public YACSEvalNonConstLocker
{
public:
  YACSEvalVirtualYACSContainer(YACSEvalResource *gf, YACS::ENGINE::Container *cont);
  YACSEVALYFX_EXPORT ~YACSEvalVirtualYACSContainer();
  YACSEVALYFX_EXPORT std::string getChosenMachine() const { return _chosenHost; }
  YACSEVALYFX_EXPORT void setWantedMachine(const std::string& machine);
  YACSEVALYFX_EXPORT std::vector<std::string> listOfPropertyKeys() const;
  YACSEVALYFX_EXPORT std::string getValueOfKey(const char *key) const;
  YACSEVALYFX_EXPORT void setProperty(const std::string& key, const std::string &value);
  YACSEVALYFX_EXPORT void resetOverloadedProps() { checkNonLocked(); _overloadedPropertyMap.clear(); }
  YACSEVALYFX_EXPORT std::string getName() const;
  void apply(bool interactiveStatus);
public:
  YACSEvalVirtualYACSContainer();
  void set(YACSEvalResource *gf, YACS::ENGINE::Container *cont);
  void aggregate(ParserResourcesType& entry) const;
  void setMachineNoCheck(const std::string& machine) { checkNonLocked(); _chosenHost=machine; }
  std::string findDefault(bool isInteractive) const;
private:
  std::string getHostName() const { return getValueOfKey(HOSTNAME_KEY); }
  unsigned int getMem() const { return getValueOfKeyUInt(MEM_KEY); }
  unsigned int getNbNodes() const { return getValueOfKeyUInt(NB_NODE_KEY); }
  unsigned int getCPUFreq() const { return getValueOfKeyUInt(CPU_CLOCK_KEY); }
  unsigned int getNbProcPerNode() const { return getValueOfKeyUInt(NB_PROC_PER_NODE_KEY); }
  unsigned int getValueOfKeyUInt(const char *key) const;
  std::map<std::string,std::string> listOfPropertiesInYACSContainer() const;
  void checkNotNullYACSContainer() const;
public:
  static const char CONTAINER_NAME_KEY[];
  static const char CPU_CLOCK_KEY[];
  static const char HOSTNAME_KEY[];
  static const char MEM_KEY[];
  static const char NB_NODE_KEY[];
  static const char NB_PROC_PER_NODE_KEY[];
  static const char NB_RESOURCE_PROCS_KEY[];
  static const char POLICY_KEY[];
  static const char OS_KEY[];
private:
  std::string _chosenHost;
  //! list of properties that overloads.
  std::map<std::string,std::string> _overloadedPropertyMap;
  //! property map at the origin.
  std::map<std::string,std::string> _propertyMap;
  YACSEvalResource *_gf;
  YACS::ENGINE::Container *_cont;
};

class YACSEvalListOfResources;

class YACSEvalResource : public YACSEvalNonConstLocker
{
public:
  YACSEVALYFX_EXPORT virtual ~YACSEvalResource();
  YACSEVALYFX_EXPORT std::vector<std::string> getAllChosenMachines() const;
  YACSEVALYFX_EXPORT std::vector<std::string> getAllFittingMachines() const;
  YACSEVALYFX_EXPORT void setWantedMachine(const std::string& machine);
  YACSEVALYFX_EXPORT std::size_t size() const { return _containers.size(); }
  YACSEVALYFX_EXPORT YACSEvalVirtualYACSContainer *at(std::size_t i) const;
  YACSEVALYFX_EXPORT void apply(bool interactiveStatus);
public:
  void fitWithCurrentCatalogAbs();
  void aggregate(ParserResourcesType& entry) const;
  YACSEvalListOfResources *getGodFather() const { return _gf; }
  void notifyWantedMachine(YACSEvalVirtualYACSContainer *sender, const std::string& oldMachine, const std::string& newMachine);
  void setMachineNoCheck(const std::string& machine);
protected:
  YACSEvalResource(YACSEvalListOfResources *gf, const std::vector< YACS::ENGINE::Container * >& conts);
protected:
  YACSEvalListOfResources *_gf;
  std::vector< YACSEvalVirtualYACSContainer > _containers;
};

class ResourcesManager_cpp;

class YACSEvalParamsForCluster
{
public:
  YACSEvalParamsForCluster():_exclusiveness(false),_nbOfProcs(1) { }
  bool getExclusiveness() const { return _exclusiveness; }
  YACSEVALYFX_EXPORT void setExclusiveness(bool newStatus);
  std::string getRemoteWorkingDir() const { return _remoteWorkingDir; }
  void setRemoteWorkingDir(const std::string& remoteWorkingDir) { _remoteWorkingDir=remoteWorkingDir; }
  std::string getLocalWorkingDir() const { return _localWorkingDir; }
  void setLocalWorkingDir(const std::string& localWorkingDir) { _localWorkingDir=localWorkingDir; }
  std::string getWCKey() const { return _wcKey; }
  void setWCKey(const std::string& wcKey) { _wcKey=wcKey; }
  unsigned int getNbProcs() const { return _nbOfProcs; }
  void setNbProcs(unsigned int nbProcs) { _nbOfProcs=nbProcs; }
  void setMaxDuration(const std::string& maxDuration) { _maxDuration=maxDuration; }
  std::string getMaxDuration() const { return _maxDuration; }
  YACSEVALYFX_EXPORT void checkConsistency() const;
private:
  bool _exclusiveness;
  std::string _remoteWorkingDir;
  std::string _localWorkingDir;
  std::string _wcKey;
  std::string _maxDuration;
  unsigned int _nbOfProcs;
};

class YACSEvalListOfResources : public YACSEvalNonConstLocker
{
public:
  YACSEvalListOfResources(int maxLevOfPara, ResourcesManager_cpp *rm, const YACS::ENGINE::DeploymentTree& dt);
  YACSEVALYFX_EXPORT std::vector<std::string> getAllChosenMachines() const;
  YACSEVALYFX_EXPORT std::vector<std::string> getAllFittingMachines() const;
  YACSEVALYFX_EXPORT void setWantedMachine(const std::string& machine);
  YACSEVALYFX_EXPORT std::size_t size() const { return _resources.size(); }
  YACSEVALYFX_EXPORT YACSEvalResource *at(std::size_t i) const;
  YACSEVALYFX_EXPORT bool isInteractive() const;
  YACSEVALYFX_EXPORT unsigned int getNumberOfProcsDeclared() const;
  YACSEVALYFX_EXPORT void checkOKForRun() const;
  YACSEVALYFX_EXPORT YACSEvalParamsForCluster& getAddParamsForCluster() { return _paramsInCaseOfCluster; }
  YACSEVALYFX_EXPORT const YACSEvalParamsForCluster& getAddParamsForCluster() const { return _paramsInCaseOfCluster; }
  void apply();
  YACSEVALYFX_EXPORT ~YACSEvalListOfResources();
public:// info only
  YACSEVALYFX_EXPORT bool isMachineInteractive(const std::string& machine) const;
public:
  ResourcesManager_cpp *getCatalogEntry() const { return _rm; }
  YACS::ENGINE::DeploymentTree *getDeploymentTree() const { return _dt; }
  int getMaxLevelOfParallelism() const { return _maxLevOfPara; }
  std::vector<std::string> getFittingResources(ParserResourcesType *request) const;
  void notifyWantedMachine(YACSEvalVirtualYACSContainer *sender, const std::string& oldMachine, const std::string& newMachine);
  bool hasRightInteractiveStatus(const std::string& machineToTest, bool isInteractive) const;
private:
  void fitWithCurrentCatalog();
  unsigned int getNumberOfProcOfResource(const std::string& machine) const;
private:
  ResourcesManager_cpp *_rm;
  int _maxLevOfPara;
  std::vector<YACSEvalResource *> _resources;
  YACS::ENGINE::DeploymentTree *_dt;
  YACSEvalParamsForCluster _paramsInCaseOfCluster;
};

#endif
