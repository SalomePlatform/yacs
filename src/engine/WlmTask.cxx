// Copyright (C) 2006-2026  CEA, EDF
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

#include "WlmTask.hxx"
#include "Runtime.hxx"
#include "Container.hxx"

namespace YACS
{
namespace ENGINE
{
WlmTask::WlmTask(Executor& executor, YACS::ENGINE::Task* yacsTask)
: _type()
, _executor(executor)
, _yacsTask(yacsTask)
{
  Container * yacsContainer = yacsTask->getContainer();
  if(yacsContainer != nullptr && yacsTask->canAcceptImposedResource())
  {
    _type.ignoreResources = false;
    _type.name = yacsContainer->getName();
    std::string nb_procs_str = yacsContainer->getProperty("nb_parallel_procs");
    float needed_cores = 0.0;
    if(!nb_procs_str.empty())
      needed_cores = std::stof(nb_procs_str);
    _type.neededCores = needed_cores;
  }
  else
  {
    _type.ignoreResources = true;
    _type.name = "test";
    _type.neededCores = 0;
  }
  _type.id = 0;
}

const WorkloadManager::ContainerType& WlmTask::type()const
{
  return _type;
}

void WlmTask::run(const WorkloadManager::RunInfo& runInfo)
{
  if(runInfo.isOk)
  {
    _executor.loadTask(_yacsTask, runInfo);
    _executor.makeDatastreamConnections(_yacsTask);
    YACS::Event ev = _executor.runTask(_yacsTask);
    _executor.endTask(_yacsTask, ev);
  }
  else
  {
    _executor.failTask(_yacsTask, runInfo.error_message);
  }
  delete this; // provisoire
}

bool WlmTask::isAccepted(const WorkloadManager::Resource& r)
{
  Container * yacsContainer = _yacsTask->getContainer();
  std::string hostname = yacsContainer->getProperty("hostname");
  bool accept = true;
  if(!hostname.empty())
    accept = (hostname == r.name);
  return accept;
}

void WlmTask::loadResources(WorkloadManager::WorkloadManager& wm)
{
  Runtime *r(getRuntime());
  if(!r)
    throw YACS::Exception("loadResources : no runtime  !");
  std::vector< std::pair<std::string,int> > data(r->getCatalogOfComputeNodes());
  int id = 0;
  for(const std::pair<std::string,int>& res : data)
  {
    WorkloadManager::Resource newResource;
    newResource.name = res.first;
    newResource.id = id;
    id++;
    newResource.nbCores = res.second;
    wm.addResource(newResource);
  }
  wm.freezeResources();
}

}
}
