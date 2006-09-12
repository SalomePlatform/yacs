#ifndef __EXECUTOR_HXX__
#define __EXECUTOR_HXX__

#include "Mutex.hxx"
#include "Thread.hxx"
#include "Semaphore.hxx"

#include <list>

namespace YACS
{
  namespace ENGINE
  {
    class Scheduler;
    class Task;
    
    class Executor
    {
    protected:
      Scheduler *_mainSched;
      int _nbOfConcurrentThreads;
      YACS::BASES::Mutex _mutexForNbOfConcurrentThreads;
      YACS::BASES::Semaphore _semForNewTasksToPerform;
      YACS::BASES::Mutex _mutexForSchedulerUpdate;
      pthread_cond_t _cond;
      std::list< YACS::BASES::Thread * > _groupOfAllThreadsCreated;
    public:
      Executor();
      ~Executor();
      void RunW(Scheduler *graph);
      int getNbOfThreads();
    protected:
      void launchTask(Task *task);
      void wakeUp();
      void sleepWhileNoEventsFromAnyRunningTask();
      void notifyEndOfThread(YACS::BASES::Thread *thread);
    protected:
      static void *functionForTaskExecution(void *);
    };
  }
}

#endif
