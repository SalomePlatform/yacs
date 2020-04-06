// Copyright (C) 2020  CEA/DEN, EDF R&D
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

#include "WorkloadAlgorithm.hxx"
#include <set>
#include <map>
#include <list>

namespace WorkloadManager
{
/**
 * @todo write docs
 */
class DefaultAlgorithm : public WorkloadAlgorithm
{
public:
  void addTask(Task* t)override;
  void addResource(Resource* r)override;
  LaunchInfo chooseTask()override;
  void liberate(const LaunchInfo& info)override;
  bool empty()const override;

// ----------------------------- PRIVATE ----------------------------- //
private:
  class ResourceInfoForContainer
  {
  public:
    ResourceInfoForContainer(const Resource * r, const ContainerType* ctype);
    unsigned int maxContainers()const;
    unsigned int  alloc();
    void free(unsigned int index);
    unsigned int nbRunningContainers()const;
    bool isContainerRunning(unsigned int index)const;
  private:
    const ContainerType* _ctype;
    const Resource* _resource;
    std::set<unsigned int> _runningContainers;
    unsigned int _firstFreeContainer;
  };
  
  class ResourceLoadInfo
  {
  public:
    ResourceLoadInfo(const Resource * r);
    bool isSupported(const ContainerType* ctype)const;
    bool isAllocPossible(const ContainerType* ctype)const;
    float cost(const ContainerType* ctype)const;
    unsigned int alloc(const ContainerType* ctype);
    void free(const ContainerType* ctype, int index);
  private:
    const Resource* _resource;
    float _load;
    std::map<const ContainerType*, ResourceInfoForContainer> _ctypes;
  };
  
private:
  std::map<const Resource *, ResourceLoadInfo> _resources;
  std::list<Task*> _waitingTasks;
};
}
#endif // ALGORITHMIMPLEMENT_H
