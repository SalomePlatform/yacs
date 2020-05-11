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
#include "DefaultAlgorithm.hxx"
#include "Task.hxx"
#include <stdexcept>
#include <limits>
#include <algorithm>

namespace WorkloadManager
{
void DefaultAlgorithm::addTask(Task* t)
{
  // put the tasks which need more cores in front.
  float newNeedCores = t->type().neededCores;
  if(_waitingTasks.empty())
    _waitingTasks.push_back(t);
  else if(_waitingTasks.back()->type().neededCores >= newNeedCores)
    _waitingTasks.push_back(t);
  else
  {
    std::list<Task*>::iterator it = _waitingTasks.begin();
    while(it != _waitingTasks.end() && (*it)->type().neededCores >= newNeedCores)
      it++;
    _waitingTasks.insert(it, t);
  }
}

bool DefaultAlgorithm::empty()const
{
  return _waitingTasks.empty();
}

void DefaultAlgorithm::addResource(const Resource& r)
{
  _resources.emplace_back(r);
}

WorkloadAlgorithm::LaunchInfo DefaultAlgorithm::chooseTask()
{
  LaunchInfo result;
  std::list<Task*>::iterator chosenTaskIt;
  for( std::list<Task*>::iterator itTask = _waitingTasks.begin();
      !result.taskFound && itTask != _waitingTasks.end();
      itTask ++)
  {
    const ContainerType& ctype = (*itTask)->type();
    if(ctype.ignoreResources)
      result.taskFound = true;
    else
    {
      std::list<ResourceLoadInfo>::iterator best_resource;
      best_resource = _resources.end();
      float best_cost = std::numeric_limits<float>::max();
      bool isSupported = false;
      for(auto itResource = _resources.begin();
          itResource != _resources.end();
          itResource++)
        if(itResource->isSupported(ctype)
            && (*itTask)->isAccepted(itResource->resource()))
        {
          if(itResource->isAllocPossible(ctype))
          {
            float thisCost = itResource->cost(ctype);
            if( best_cost > thisCost)
            {
              best_cost = thisCost;
              best_resource = itResource;
            }
          }
        }
      if(best_resource != _resources.end())
      {
        result.taskFound = true;
        result.worker.resource = best_resource->resource();
        result.worker.index = best_resource->alloc(ctype);
      }
      else if(!isSupported)
      {
        // TODO: This task can never be run by any available resource.
      }
    }
    if(result.taskFound)
    {
      chosenTaskIt = itTask;
      result.task = (*itTask);
      result.worker.type = ctype;
    }
  }
  if(result.taskFound)
    _waitingTasks.erase(chosenTaskIt);
  return result;
}

void DefaultAlgorithm::liberate(const LaunchInfo& info)
{
  const ContainerType& ctype = info.worker.type;
  if(!ctype.ignoreResources)
  {
    const Resource& r = info.worker.resource;
    unsigned int index = info.worker.index;
    std::list<ResourceLoadInfo>::iterator it = std::find(_resources.begin(),
                                                        _resources.end(),
                                                        r);
    it->free(ctype, index); // we are sure to find it
  }
}

// ResourceInfoForContainer

DefaultAlgorithm::ResourceInfoForContainer::ResourceInfoForContainer
                                (const Resource& r, const ContainerType& ctype)
: _ctype(ctype)
, _resource(r)
, _runningContainers()
, _firstFreeContainer(0)
{
}

unsigned int DefaultAlgorithm::ResourceInfoForContainer::maxContainers()const
{
  return float(_resource.nbCores) / _ctype.neededCores;
}

unsigned int  DefaultAlgorithm::ResourceInfoForContainer::alloc()
{
  unsigned int result = _firstFreeContainer;
  _runningContainers.insert(result);
  _firstFreeContainer++;
  while(isContainerRunning(_firstFreeContainer))
    _firstFreeContainer++;
  return result;
}

void DefaultAlgorithm::ResourceInfoForContainer::free(unsigned int index)
{
  _runningContainers.erase(index);
  if(index < _firstFreeContainer)
    _firstFreeContainer = index;
}

unsigned int DefaultAlgorithm::ResourceInfoForContainer::nbRunningContainers()const
{
  return _runningContainers.size();
}

bool DefaultAlgorithm::ResourceInfoForContainer::isContainerRunning
                                (unsigned int index)const
{
  return _runningContainers.find(index)!=_runningContainers.end();
}

// ResourceLoadInfo

DefaultAlgorithm::ResourceLoadInfo::ResourceLoadInfo(const Resource& r)
: _resource(r)
, _load(0.0)
, _loadCost(0.0)
, _ctypes()
{
}

bool DefaultAlgorithm::ResourceLoadInfo::isSupported
                                (const ContainerType& ctype)const
{
  return ctype.neededCores <= _resource.nbCores ;
}
                                          
bool DefaultAlgorithm::ResourceLoadInfo::isAllocPossible
                                (const ContainerType& ctype)const
{
  return ctype.neededCores + _load <= _resource.nbCores;
}

float DefaultAlgorithm::ResourceLoadInfo::cost
                                (const ContainerType& ctype)const
{
  return _loadCost * 100.0 / float(_resource.nbCores);
}

unsigned int DefaultAlgorithm::ResourceLoadInfo::alloc
                                (const ContainerType& ctype)
{
  std::list<ResourceInfoForContainer>::iterator it = std::find(_ctypes.begin(),
                                                               _ctypes.end(),
                                                               ctype);
  // add the type if not found
  if(it == _ctypes.end())
  {
    _ctypes.emplace_back(_resource, ctype);
    it = _ctypes.end();
    it--;
  }
  _load += ctype.neededCores;
  if(ctype.neededCores == 0)
    _loadCost += COST_FOR_0_CORE_TASKS;
  else
    _loadCost += ctype.neededCores;
  return it->alloc();
}

void DefaultAlgorithm::ResourceLoadInfo::free
                                (const ContainerType& ctype, int index)
{
  _load -= ctype.neededCores;
  if(ctype.neededCores == 0)
    _loadCost -= COST_FOR_0_CORE_TASKS;
  else
    _loadCost -= ctype.neededCores;
  std::list<ResourceInfoForContainer>::iterator it = std::find(_ctypes.begin(),
                                                               _ctypes.end(),
                                                               ctype);
  it->free(index);
}

}
