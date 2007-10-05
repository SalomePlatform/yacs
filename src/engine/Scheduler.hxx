
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
      virtual void notifyFrom(const Task *sender, YACS::Event event) = 0;
      //Placement methods
      virtual DeploymentTree getDeploymentTree() const = 0;
      virtual bool isPlacementPredictableB4Run() const = 0;
      virtual bool isMultiplicitySpecified(unsigned& value) const = 0;
      virtual void forceMultiplicity(unsigned value) = 0;
    };
  }
}

#endif
