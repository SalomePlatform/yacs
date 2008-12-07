//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#ifndef __EXECUTOR_HXX__
#define __EXECUTOR_HXX__

#include "Mutex.hxx"
#include "Thread.hxx"
#include "Semaphore.hxx"
#include "Exception.hxx"
#include "define.hxx"

#include <list>
#include <vector>
#include <string>
#include <fstream>

namespace YACS
{
  namespace ENGINE
  {
    class Scheduler;
    class ComposedNode;
    class Task;
    
/*! \brief Threaded Executor 
 *
 * \ingroup Executors
 *
 *
 */
    class Executor
    {
    protected:
      Scheduler *_mainSched;
      ComposedNode *_root;
      int _nbOfConcurrentThreads;
      YACS::BASES::Mutex _mutexForNbOfConcurrentThreads;
      YACS::BASES::Condition _condForNewTasksToPerform;
      YACS::BASES::Semaphore _semForMaxThreads;
      YACS::BASES::Condition _condForStepByStep;
      YACS::BASES::Condition _condForPilot;
      YACS::BASES::Mutex _mutexForSchedulerUpdate;
      YACS::BASES::Mutex _mutexForTrace;
      bool _toContinue;
      bool _isOKToEnd;
      bool _stopOnErrorRequested;
      bool _dumpOnErrorRequested;
      bool _errorDetected;
      bool _isRunningunderExternalControl;
      bool _isWaitingEventsFromRunningTasks;
      int _numberOfRunningTasks;
      int _numberOfEndedTasks;
      int _semThreadCnt;
      YACS::ExecutorState _executorState;
      YACS::ExecutionMode _execMode;
      std::list<std::string> _listOfBreakPoints;
      std::list<std::string> _listOfTasksToLoad;
      std::vector<Task *> _tasks; 
      std::vector<Task *> _tasksSave; 
      std::list< YACS::BASES::Thread * > _groupOfAllThreadsCreated;
      std::ofstream _trace;
      std::string _dumpErrorFile;
    public:
      Executor();
      ~Executor();
      void RunA(Scheduler *graph,int debug=0, bool fromScratch=true);
      void RunW(Scheduler *graph,int debug=0, bool fromScratch=true) { RunB(graph, debug, fromScratch); }
      void RunB(Scheduler *graph,int debug=0, bool fromScratch=true);
      YACS::ExecutionMode getCurrentExecMode();
      YACS::ExecutorState getExecutorState();
      void setExecMode(YACS::ExecutionMode mode);
      void setListOfBreakPoints(std::list<std::string> listOfBreakPoints);
      std::list<std::string> getTasksToLoad();
      bool setStepsToExecute(std::list<std::string> listToExecute);
      bool resumeCurrentBreakPoint();
      bool isNotFinished();
      void stopExecution();
      bool saveState(const std::string& xmlFile);
      bool loadState();
      int getNbOfThreads();
      void displayDot(Scheduler *graph);
      void setStopOnError(bool dumpRequested=false, std::string xmlFile="");
      void unsetStopOnError();
      void waitPause();
      static int _maxThreads;
    protected:
      bool checkBreakPoints();
      void waitResume();
      void loadTask(Task *task);
      void launchTasks(std::vector<Task*>& tasks);
      void launchTask(Task *task);
      void wakeUp();
      void sleepWhileNoEventsFromAnyRunningTask();
      void notifyEndOfThread(YACS::BASES::Thread *thread);
      void traceExec(Task *task, const std::string& message);
      void _displayDot(Scheduler *graph);
      virtual void sendEvent(const std::string& event);
    protected:
      static void *functionForTaskExecution(void *);
    };
  }
}

#endif
