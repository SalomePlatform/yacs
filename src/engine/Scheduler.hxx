// Copyright (C) 2006-2016  CEA/DEN, EDF R&D
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

#ifndef __SCHEDULER_HXX__
#define __SCHEDULER_HXX__

#include "DeploymentTree.hxx"
#include "define.hxx"

#include <string>
#include <vector>

namespace YACS
{
  namespace ENGINE
  {
    class Task;
    class Executor;
    
    class Scheduler
    {
    public:
      virtual void init(bool start=true) = 0;
      virtual bool isFinished() = 0;
      virtual void exUpdateState() = 0;
      virtual std::string getName() const = 0;
      virtual std::string getTaskName(Task *task) const = 0;
      virtual std::vector<Task *> getNextTasks(bool& isMore) = 0;
      virtual void selectRunnableTasks(std::vector<Task *>& tasks) = 0;
      virtual void notifyFrom(const Task *sender, YACS::Event event, const Executor *execInst) = 0;
      //Placement methods
      virtual DeploymentTree getDeploymentTree() const = 0;
      virtual bool isPlacementPredictableB4Run() const = 0;
      virtual bool isMultiplicitySpecified(unsigned& value) const = 0;
      virtual void forceMultiplicity(unsigned value) = 0;
      virtual ~Scheduler();
    };
  }
}

#endif
