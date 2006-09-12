#ifndef __SCHEDULER_HXX__
#define __SCHEDULER_HXX__

#include <vector>
#include "define.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class Task;
    
    class Scheduler
    {
    public:
      virtual void init() = 0;
      virtual bool isFinished() = 0;
      virtual std::vector<Task *> getNextTasks(bool& isMore) = 0;
      virtual void selectRunnableTasks(std::vector<Task *>& tasks) = 0;
      virtual void notifyFrom(const Task *sender, YACS::Event event) = 0;
    };
  }
}

#endif
