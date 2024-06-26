// Copyright (C) 2020-2024  CEA, EDF
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
#ifndef ALGORITHMIMPLEMENT_H
#define ALGORITHMIMPLEMENT_H

#include "YACSlibWorkloadmanagerExport.hxx"
#include "WorkloadAlgorithm.hxx"
#include <set>
#include <map>
#include <list>

namespace WorkloadManager
{
/**
 * @todo write docs
 */
class YACSLIBWLM_EXPORT DefaultAlgorithm : public WorkloadAlgorithm
{
public:
  void addTask(Task* t)override;
  void addResource(const Resource& r)override;
  LaunchInfo chooseTask()override;
  void liberate(const LaunchInfo& info)override;
  bool empty()const override;
  void freezeResources() override { _resourcesFrozen = true;}

// ----------------------------- PRIVATE ----------------------------- //
private:
  class ResourceInfoForContainer
  {
  public:
    ResourceInfoForContainer(const Resource& r, const ContainerType& ctype);
    unsigned int maxContainers()const;
    unsigned int  alloc();
    void free(unsigned int index);
    unsigned int nbRunningContainers()const;
    bool isContainerRunning(unsigned int index)const;
    bool operator<(const ResourceInfoForContainer& other)const
    { return _ctype < other._ctype;}
    bool operator==(const ContainerType& other)const
    { return _ctype == other;}
    const ContainerType& type()const { return _ctype;}
  private:
    ContainerType _ctype;
    const Resource& _resource; // same ref as ResourceLoadInfo
    std::set<unsigned int> _runningContainers; // 0 to max possible containers on this resource
    unsigned int _firstFreeContainer;
  };

  class ResourceLoadInfo
  {
  public:
    ResourceLoadInfo(const Resource& r);
    bool isSupported(const ContainerType& ctype)const;
    bool isAllocPossible(const ContainerType& ctype)const;
    float cost(const ContainerType& ctype)const;
    unsigned int alloc(const ContainerType& ctype);
    void free(const ContainerType& ctype, int index);
    bool operator<(const ResourceLoadInfo& other)const
    { return _resource < other._resource;}
    bool operator==(const Resource& other)const
    { return _resource == other;}
    const Resource& resource()const { return _resource;}
    float COST_FOR_0_CORE_TASKS = 1.0 / 4096.0 ;
  private:
    Resource _resource;
    float _load;
    float _loadCost;
    std::list<ResourceInfoForContainer> _ctypes;
  };

private:
  std::list<ResourceLoadInfo> _resources;
  std::list<Task*> _waitingTasks;
  bool _resourcesFrozen = false;
};
}
#endif // ALGORITHMIMPLEMENT_H
