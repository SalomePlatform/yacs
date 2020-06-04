// Copyright (C) 2006-2020  CEA/DEN, EDF R&D
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
#ifndef __WLM_TASK_HXX__
#define __WLM_TASK_HXX__
#include "Executor.hxx"
#include "Task.hxx"
#include "workloadmanager/WorkloadManager.hxx"

namespace YACS
{
namespace ENGINE
{
class YACSLIBENGINE_EXPORT WlmTask : public WorkloadManager::Task
{
public:
  WlmTask(Executor& executor, YACS::ENGINE::Task* yacsTask);
  const WorkloadManager::ContainerType& type()const override;
  void run(const WorkloadManager::RunInfo& runInfo)override;
  bool isAccepted(const WorkloadManager::Resource& r)override;
  
  static void loadResources(WorkloadManager::WorkloadManager& wm);
private:
  WorkloadManager::ContainerType _type;
  Executor& _executor;
  YACS::ENGINE::Task * _yacsTask;
};

}
}

#endif //__WLM_TASK_HXX__
