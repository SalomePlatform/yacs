#include "Executor.hxx"
#include "Task.hxx"
#include "Scheduler.hxx"
#include <pthread.h>
#include <iostream>

using namespace YACS::ENGINE;

using YACS::BASES::Mutex;
using YACS::BASES::Thread;
using YACS::BASES::Semaphore;

Executor::Executor():_nbOfConcurrentThreads(0)//,_cond(PTHREAD_COND_INITIALIZER)
{
}

Executor::~Executor()
{
  for(std::list<Thread *>::iterator iter=_groupOfAllThreadsCreated.begin();iter!=_groupOfAllThreadsCreated.end();iter++)
    delete *iter;
}

void Executor::RunW(Scheduler *graph)
{
  _mainSched=graph;
  bool isMore;
  int i=0;
  graph->init();
  std::vector<Task *> tasks;
  std::vector<Task *>::iterator iter;
  bool toContinue=true;
  wakeUp();
  while(toContinue)
    {
      sleepWhileNoEventsFromAnyRunningTask();
      {//Critical section
	_mutexForSchedulerUpdate.lock();
	tasks=graph->getNextTasks(isMore);
	graph->selectRunnableTasks(tasks);
	_mutexForSchedulerUpdate.unlock();
      }//End of critical section
      for(iter=tasks.begin();iter!=tasks.end();iter++)
	launchTask(*iter);
      {//Critical section
	_mutexForSchedulerUpdate.lock();
	toContinue=!graph->isFinished();
	_mutexForSchedulerUpdate.unlock();
      }//End of critical section
      i++;
    }
}

void Executor::launchTask(Task *task)
{
  void **args=new void *[3];
  _mutexForNbOfConcurrentThreads.lock();
  _groupOfAllThreadsCreated.push_back(0);
  std::list<Thread *>::iterator iter=_groupOfAllThreadsCreated.end();
  iter--;
  _mutexForNbOfConcurrentThreads.unlock();
  args[0]=(void *)task;
  args[1]=(void *)_mainSched;
  args[2]=(void *)this;
  {//Critical section
    _mutexForSchedulerUpdate.lock();
    task->begin();
    _mainSched->notifyFrom(task,YACS::START);
    _mutexForSchedulerUpdate.unlock();
  }//End of critical section
  _mutexForNbOfConcurrentThreads.lock();
  //functionForTaskExecution(args);//MultiThreaded=NO
  //  *iter=
  new Thread(functionForTaskExecution,args);//MultiThreaded=YES
  _mutexForNbOfConcurrentThreads.unlock();
}

void Executor::sleepWhileNoEventsFromAnyRunningTask()
{
  _semForNewTasksToPerform.wait();
}

void Executor::notifyEndOfThread(YACS::BASES::Thread *thread)
{
  /*_mutexForNbOfConcurrentThreads.lock();
  _groupOfAllThreadsCreated.remove(thread);
  delete thread;
  _mutexForNbOfConcurrentThreads.unlock();*/
}

void Executor::wakeUp()
{
  int val=_semForNewTasksToPerform.getValue();
  if(!val)
    _semForNewTasksToPerform.post();
}

int Executor::getNbOfThreads()
{
  int ret;
  _mutexForNbOfConcurrentThreads.lock();
  ret=_groupOfAllThreadsCreated.size();
  _mutexForNbOfConcurrentThreads.unlock();
  return ret;
}

void *Executor::functionForTaskExecution(void *arg)
{
  void **argT=(void **)arg;
  Task *task=(Task *)argT[0];
  Scheduler *sched=(Scheduler *)argT[1];
  Executor *execInst=(Executor *)argT[2];
  delete [] argT;
  task->execute();
  {//Critical section
    execInst->_mutexForSchedulerUpdate.lock();
    task->finished();
    sched->notifyFrom(task,YACS::FINISH);
    execInst->_mutexForSchedulerUpdate.unlock();
  }//End of critical section
  execInst->wakeUp();
  execInst->notifyEndOfThread(0);
  return 0;
}
