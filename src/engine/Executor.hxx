// Copyright (C) 2006-2024  CEA, EDF
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

#ifndef __EXECUTOR_HXX__
#define __EXECUTOR_HXX__

#include "YACSlibEngineExport.hxx"
#include "Mutex.hxx"
#include "Thread.hxx"
#include "Semaphore.hxx"
#include "Exception.hxx"
#include "define.hxx"

#ifdef WIN32
#include <windows.h>
#pragma comment(lib,"winmm.lib")
#endif
#include <list>
#include <vector>
#include <set>
#include <string>
#include <fstream>
#include <ctime>
#include <chrono>
#include <cstdint>

namespace WorkloadManager
{
  struct RunInfo;
}

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
    class YACSLIBENGINE_EXPORT Executor
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
      std::uint32_t _maxNbThreads = 10000;
      bool _toContinue;
      bool _isOKToEnd;
      bool _stopOnErrorRequested;
      bool _dumpOnErrorRequested;
      bool _errorDetected;
      bool _isRunningunderExternalControl;
      bool _isWaitingEventsFromRunningTasks;
      int _numberOfRunningTasks;
      std::set<Task *> _runningTasks; 
      int _numberOfEndedTasks;
      int _semThreadCnt;
      YACS::ExecutorState _executorState;
      YACS::ExecutionMode _execMode;
      std::list<std::string> _listOfBreakPoints;
      std::list<std::string> _listOfTasksToLoad;
      std::vector<Task *> _tasks; 
      std::vector<Task *> _tasksSave; 
      std::ofstream _trace;
      std::string _dumpErrorFile;
      bool _keepGoingOnFail;
      //! specifies if scope DynParaLoop is active or not. False by default.
      bool _DPLScopeSensitive;
      std::chrono::steady_clock::time_point _start;
    public:
      Executor();
      virtual ~Executor();
      void RunA(Scheduler *graph,int debug=0, bool fromScratch=true);
      void RunW(Scheduler *graph,int debug=0, bool fromScratch=true);
      void RunB(Scheduler *graph,int debug=0, bool fromScratch=true);
      void runWlm(Scheduler *graph,int debug=0, bool fromScratch=true);
      void setKeepGoingProperty(bool newVal) { _keepGoingOnFail=newVal; }
      bool getKeepGoingProperty() const { return _keepGoingOnFail; }
      void setDPLScopeSensitive(bool newVal) { _DPLScopeSensitive=newVal; }
      bool getDPLScopeSensitive() const { return _DPLScopeSensitive; }
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
      int getMaxNbOfThreads() const;
      void setMaxNbOfThreads(int maxNbThreads);
      int getNbOfThreads();
      int getNumberOfRunningTasks();
      void displayDot(Scheduler *graph);
      void setStopOnError(bool dumpRequested=false, std::string xmlFile="");
      void unsetStopOnError();
      void waitPause();
      bool suspendASAP();
      void resume(bool suspended);
      static int _maxThreads;
      static size_t _threadStackSize;
      YACS::BASES::Mutex& getTheMutexForSchedulerUpdate() { return _mutexForSchedulerUpdate; }
      ///// new executor !!!!!
      void loadTask(Task *task, const WorkloadManager::RunInfo& runInfo);
      YACS::Event runTask(Task *task);
      void makeDatastreamConnections(Task *task);
      void beginTask(Task *task);
      void endTask(Task *task, YACS::Event ev);
      void failTask(Task *task, const std::string& message);
      ////////////
    protected:
      bool checkBreakPoints();
      void waitResume();
      void loadTask(Task *task, const Executor *execInst);
      void loadParallelTasks(const std::vector<Task *>& tasks, const Executor *execInst);
      void launchTasks(const std::vector<Task*>& tasks);
      void launchTask(Task *task);
      void wakeUp();
      void sleepWhileNoEventsFromAnyRunningTask();
      void traceExec(Task *task, const std::string& message, const std::string& placement);
      void _displayDot(Scheduler *graph);
      virtual void sendEvent(const std::string& event);
      void filterTasksConsideringContainers(std::vector<Task *>& tsks);
      static std::string ComputePlacement(Task *zeTask);
    protected:
      static void *functionForTaskLoad(void *);
      static void *functionForTaskExecution(void *);
    };
  }
}

#endif
