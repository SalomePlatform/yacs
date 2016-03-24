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

#include "Executor.hxx"
#include "Task.hxx"
#include "AutoLocker.hxx"
#include "Scheduler.hxx"
#include "Dispatcher.hxx"
#include "Container.hxx"
#include "HomogeneousPoolContainer.hxx"
#include "ComponentInstance.hxx"

#include "VisitorSaveState.hxx"
#include "ServiceNode.hxx"
#include "ComposedNode.hxx"

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#ifndef WIN32
#include <sys/time.h>
#include <unistd.h>
#endif

#include <cstdlib>
#include <algorithm>

#ifdef WIN32
#define usleep(A) _sleep(A/1000)
#if !defined(S_ISCHR) || !defined(S_ISREG)
#  ifndef S_IFMT
#    ifdef _S_IFMT
#      define S_IFMT _S_IFMT
#      define S_IFCHR _S_IFCHR
#      define S_IFREG _S_IFREG
#    else
#    ifdef __S_IFMT
#      define S_IFMT __S_IFMT
#      define S_IFCHR __S_IFCHR
#      define S_IFREG __S_IFREG
#    endif
#    endif
#  endif
#  define S_ISCHR(mode) (((mode) & S_IFMT) == S_IFCHR)
#  define S_ISREG(mode) (((mode) & S_IFMT) == S_IFREG)
#endif
#endif

using namespace YACS::ENGINE;
using namespace std;

using YACS::BASES::Mutex;
using YACS::BASES::Thread;
using YACS::BASES::Semaphore;

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

int Executor::_maxThreads(1000);
size_t Executor::_threadStackSize(262144); // Default thread stack size is 256 kB == 2**18 because threads launched by YACS are lightweight

Executor::Executor():_nbOfConcurrentThreads(0), _semForMaxThreads(_maxThreads),_keepGoingOnFail(false),_DPLScopeSensitive(false)
{
  _root=0;
  _toContinue = true;
  _isOKToEnd = false;
  _stopOnErrorRequested = false;
  _dumpOnErrorRequested = false;
  _errorDetected = false;
  _isRunningunderExternalControl=false;
  _executorState = YACS::NOTYETINITIALIZED;
  _execMode = YACS::CONTINUE;
  _semThreadCnt = _maxThreads;
  DEBTRACE("Executor initialized with max threads = " << _maxThreads);
}

Executor::~Executor()
{
  for(list<Thread *>::iterator iter=_groupOfAllThreadsCreated.begin();iter!=_groupOfAllThreadsCreated.end();iter++)
    delete *iter;
}

//! Execute a graph waiting for completion
/*!
 *  \param graph : schema to execute
 *  \param debug : display the graph with dot if debug == 1
 *  \param fromScratch : if true the graph is reinitialized
 *
 *  Calls Scheduler::getNextTasks and Scheduler::selectRunnableTasks to select tasks to execute
 *  
 *  Calls Executor::launchTask to execute a selected Task.
 *
 *  Completion when graph is finished (Scheduler::isFinished)
 */

void Executor::RunA(Scheduler *graph,int debug, bool fromScratch)
{
  DEBTRACE("Executor::RunW debug: " << debug << " fromScratch: " << fromScratch);
  _mainSched=graph;
  _root = dynamic_cast<ComposedNode *>(_mainSched);
  if (!_root) throw Exception("Executor::Run, Internal Error!");
  bool isMore;
  int i=0;
  if(debug>1)_displayDot(graph);
  if (fromScratch)
    {
      graph->init();
      graph->exUpdateState();
    }
  if(debug>1)_displayDot(graph);
  vector<Task *> tasks;
  vector<Task *>::iterator iter;
  _toContinue=true;
  _execMode = YACS::CONTINUE;
  _isWaitingEventsFromRunningTasks = false;
  _numberOfRunningTasks = 0;
  _runningTasks.clear();
  _numberOfEndedTasks=0;
  while(_toContinue)
    {
      sleepWhileNoEventsFromAnyRunningTask();

      if(debug>2)_displayDot(graph);

      {//Critical section
        YACS::BASES::AutoLocker<YACS::BASES::Mutex> alck(&_mutexForSchedulerUpdate);
        tasks=graph->getNextTasks(isMore);
        graph->selectRunnableTasks(tasks);
      }//End of critical section

      if(debug>2)_displayDot(graph);

      for(iter=tasks.begin();iter!=tasks.end();iter++)
        loadTask(*iter,this);

      if(debug>1)_displayDot(graph);

      launchTasks(tasks);

      if(debug>1)_displayDot(graph);

      {//Critical section
        YACS::BASES::AutoLocker<YACS::BASES::Mutex> alck(&_mutexForSchedulerUpdate);
        _toContinue=!graph->isFinished();
      }//End of critical section
      DEBTRACE("_toContinue: " << _toContinue);

      if(debug>0)_displayDot(graph);

      i++;
    }
}

//! Execute a graph with breakpoints or step by step
/*!
 *  To be launch in a thread (main thread controls the progression).
 *  \param graph : schema to execute
 *  \param debug : display the graph with dot if debug >0
 *  \param fromScratch : if false, state from a previous partial exection is already loaded
 *
 *  Calls Scheduler::getNextTasks and Scheduler::selectRunnableTasks to select tasks to execute
 *  
 *  Calls Executor::checkBreakPoints to verify if a pause is requested 
 * 
 *  Calls Executor::launchTask to execute a selected Task
 *
 *  Completion when graph is finished (Scheduler::isFinished)
 *
 *  States of execution:
 *  - YACS::NOTYETINITIALIZED
 *  - YACS::INITIALISED
 *  - YACS::RUNNING            (to next breakpoint or step)
 *  - YACS::WAITINGTASKS       (a breakpoint or step as been reached, but there are still running tasks)
 *  - YACS::PAUSED             (a breakpoint or step as been reached, no more running tasks)
 *  - YACS::FINISHED           (no more ready tasks, nore running tasks)
 *  - YACS::STOPPED            (stopped by user before end)
 *
 *  Modes of Execution:
 *  - YACS::CONTINUE           (normal run without breakpoints)
 *  - YACS::STEPBYSTEP         (pause at each loop)
 *  - YACS::STOPBEFORENODES    (pause when a node is reached)
 *
 *  A breakpoint is defined by a node name. The breakpoint is reached when the node becomes ready.
 *  Step by Step means execution node by node or group of node by group of nodes.
 *  At a given step, the user decides to launch all the ready nodes or only a subset
 *  (Caution: some nodes must run in parallel). 
 *  The next event (end of task) may give a new set of ready nodes, and define a new step.
 *
 *  The graph execution may be controled by a pilot which sends requests. Requests are asynchronous.
 *  Requests are taken into account only on certain states, otherwise return the status IgnoredRequest.
 *  - Executor::getCurrentExecMode
 *  - Executor::getExecutorState
 *  - Executor::setExecMode             : change the execution mode for next loop
 *  - Executor::setListOfBreakPoints    : must be set before setting YACS::STOPBEFORENODES
 *  - Executor::getTasksToLoad          : when paused or waiting tasks, get the list of next tasks
 *  - Executor::setStepsToExecute       : define a subset of the list given by Executor::getTasksToLoad
 *  - Executor::resumeCurrentBreakPoint : when paused or waiting tasks, resumes execution
 *  - Executor::isNotFinished
 *  - Executor::stopExecution           : stop execution asap, i.e. set STEPBYSTEP and wait PAUSED
 *  - Executor::saveState               : dump the current state of execution in an xml file
 *  - Executor::loadState               : Not yet implemented
 *  - Executor::getNbOfThreads
 *  - Executor::displayDot
 *  - Executor::setStopOnError          : ask to stop execution if a node is found in ERROR state
 *
 *  If the pilot wants to wait the state YACS::PAUSED or YACS::WAITINGTASKS, synchronisation is obtained with:
 *  - Executor::waitPause
 *
 *  TO BE VALIDATED:
 *  - Pilot may connect to executor during execution, or deconnect.
 *  - Several Pilots may be connected at the same time (for observation...)
 * 
 */

void Executor::RunB(Scheduler *graph,int debug, bool fromScratch)
{
  DEBTRACE("Executor::RunB debug: "<< graph->getName() <<" "<< debug<<" fromScratch: "<<fromScratch);

  { // --- Critical section
    YACS::BASES::AutoLocker<YACS::BASES::Mutex> alck(&_mutexForSchedulerUpdate);
    _mainSched = graph;
    _root = dynamic_cast<ComposedNode *>(_mainSched);
    if (!_root) throw Exception("Executor::Run, Internal Error!");
    _executorState = YACS::NOTYETINITIALIZED;
    sendEvent("executor");
    _toContinue=true;
    _isOKToEnd = false;
    _errorDetected = false;
    _isWaitingEventsFromRunningTasks = false;
    _numberOfRunningTasks = 0;
    _runningTasks.clear();
    _numberOfEndedTasks = 0;
    string tracefile = "traceExec_";
    tracefile += _mainSched->getName();
    _trace.open(tracefile.c_str());
#ifdef WIN32
   _start = timeGetTime();
#else
    gettimeofday(&_start, NULL);
#endif

  } // --- End of critical section

  if (debug > 1) _displayDot(graph);

  if (fromScratch)
    {
      try
        {
          graph->init();
          graph->exUpdateState();
        }
      catch(Exception& ex)
        {
          DEBTRACE("exception: "<< (ex.what()));
          _executorState = YACS::FINISHED;
          sendEvent("executor");
          throw;
        }
    }
  _executorState = YACS::INITIALISED;
  sendEvent("executor");

  if (debug > 1) _displayDot(graph);

  vector<Task *>::iterator iter;
  bool isMore;
  int problemCount=0;
  int numberAllTasks;

  _executorState = YACS::RUNNING;
  sendEvent("executor");
  while (_toContinue)
    {
      DEBTRACE("--- executor main loop");
      sleepWhileNoEventsFromAnyRunningTask();
      DEBTRACE("--- events...");
      if (debug > 2) _displayDot(graph);
      { // --- Critical section
        YACS::BASES::AutoLocker<YACS::BASES::Mutex> alck(&_mutexForSchedulerUpdate);
        _tasks=graph->getNextTasks(isMore);
        graph->selectRunnableTasks(_tasks);
        FilterTasksConsideringContainers(_tasks);
        numberAllTasks=_numberOfRunningTasks+_tasks.size();
      } // --- End of critical section
      if (debug > 2) _displayDot(graph);
      if (_executorState == YACS::RUNNING)
        {
          if (checkBreakPoints()) break; // end of thread requested, OK to exit at once;
          if (debug > 0) _displayDot(graph);
          DEBTRACE("---");
          //loadTasks(_tasks);
          loadParallelTasks(_tasks,this);
          if (debug > 1) _displayDot(graph);
          DEBTRACE("---");
          launchTasks(_tasks);
          DEBTRACE("---");
        }
      if (debug > 1) _displayDot(graph);
      { // --- Critical section
        DEBTRACE("---");
        YACS::BASES::AutoLocker<YACS::BASES::Mutex> alck(&_mutexForSchedulerUpdate);
        //It is possible that the graph is finished but it remains running tasks (it's an error but we must take it into account)
        if(_numberOfRunningTasks == 0)
          _toContinue = !graph->isFinished();

        DEBTRACE("_numberOfRunningTasks: " << _numberOfRunningTasks);
        DEBTRACE("_numberOfEndedTasks: " << _numberOfEndedTasks);
        DEBTRACE("_toContinue: " << _toContinue);
        if(_toContinue && numberAllTasks==0)
          {
            //Problem : no running tasks and no task to launch ??
            problemCount++;
            std::cerr << "Problem in Executor : no running tasks and no task to launch ?? problemCount=" << problemCount << std::endl;
            //Pause to give a chance to interrupt
            usleep(1000);
            if(problemCount > 25)
              {
                // Too much problems encountered : stop execution
                _toContinue=false;
              }
          }

        if (! _toContinue)
          {
            _executorState = YACS::FINISHED;
            sendEvent("executor");
            _condForPilot.notify_all();
          }
      } // --- End of critical section
      if (debug > 0) _displayDot(graph);
      DEBTRACE("_toContinue: " << _toContinue);
    }

  DEBTRACE("End of main Loop");

  { // --- Critical section
    YACS::BASES::AutoLocker<YACS::BASES::Mutex> alck(&_mutexForSchedulerUpdate);
    if ( _toContinue) // --- break while(): request to stop detected on checkBreakPoints()
      {
        DEBTRACE("stop requested: End soon");
        _executorState = YACS::STOPPED;
        _toContinue = false;
        sendEvent("executor");
      }
  } // --- End of critical section
  if ( _dumpOnErrorRequested && _errorDetected)
    {
      saveState(_dumpErrorFile);
    }
  {
    YACS::BASES::AutoLocker<YACS::BASES::Mutex> alck(&_mutexForTrace);
    _trace.close();
  }
  DEBTRACE("End of RunB thread");  
}

YACS::ExecutionMode Executor::getCurrentExecMode()
{
  _isRunningunderExternalControl=true;
  return _execMode;
}


YACS::ExecutorState Executor::getExecutorState()
{
  _isRunningunderExternalControl=true;
  return _executorState;
}


bool Executor::isNotFinished()
{
  _isRunningunderExternalControl=true;
  return _toContinue;
}

//! ask to stop execution on the first node found in error
/*!
 * \param dumpRequested   produce a state dump when an error is found
 * \param xmlFile         name of file used for state dump
 */

void Executor::setStopOnError(bool dumpRequested, std::string xmlFile)
{
  { // --- Critical section
    YACS::BASES::AutoLocker<YACS::BASES::Mutex> alck(&_mutexForSchedulerUpdate);
    _dumpErrorFile=xmlFile;
    _stopOnErrorRequested=true;
    _dumpOnErrorRequested = dumpRequested;
    if (dumpRequested && xmlFile.empty())
      throw YACS::Exception("dump on error requested and no filename given for dump");
    DEBTRACE("_dumpErrorFile " << _dumpErrorFile << " " << _dumpOnErrorRequested);
  } // --- End of critical section
}

//! ask to do not stop execution on nodes found in error
/*!
 */

void Executor::unsetStopOnError()
{
  { // --- Critical section
    YACS::BASES::AutoLocker<YACS::BASES::Mutex> alck(&_mutexForSchedulerUpdate);
    _stopOnErrorRequested=false;
  } // --- End of critical section
}

//! Dynamically set the current mode of execution
/*!
 * The mode can be Continue, step by step, or stop before execution of a node
 * defined in a list of breakpoints.
 */

void Executor::setExecMode(YACS::ExecutionMode mode)
{
  DEBTRACE("Executor::setExecMode(YACS::ExecutionMode mode) " << mode);
  { // --- Critical section
    YACS::BASES::AutoLocker<YACS::BASES::Mutex> alck(&_mutexForSchedulerUpdate);
    _isRunningunderExternalControl=true;
    _execMode = mode;
  } // --- End of critical section
}

//! wake up executor when in pause
/*!
 * When Executor is in state paused or waiting for task completion, the thread
 * running loop RunB waits on condition _condForStepByStep.
 * Thread RunB is waken up.
 * \return true when actually wakes up executor
 */

bool Executor::resumeCurrentBreakPoint()
{
  DEBTRACE("Executor::resumeCurrentBreakPoint()");
  bool ret = false;
  //bool doDump = false;
  { // --- Critical section
    YACS::BASES::AutoLocker<YACS::BASES::Mutex> alck(&_mutexForSchedulerUpdate);
    _isRunningunderExternalControl=true;
    DEBTRACE("_executorState: " << _executorState);
    switch (_executorState)
      {
      case YACS::WAITINGTASKS:
      case YACS::PAUSED:
        {
          _condForStepByStep.notify_all();
          _executorState = YACS::RUNNING;
          sendEvent("executor");
          ret = true;
          //if (_dumpOnErrorRequested && _errorDetected) doDump =true;
          break;
        }
      case YACS::FINISHED:
      case YACS::STOPPED:
        {
          //if (_dumpOnErrorRequested && _errorDetected) doDump =true;
          DEBTRACE("Graph Execution finished or stopped !");
          break;
        }
      default :
        {
          // debug: no easy way to verify if main loop is acutally waiting on condition
        }
      }
    DEBTRACE("---");
    //if (doDump) saveState(_dumpErrorFile);
  } // --- End of critical section
  return ret;
}


//! define a list of nodes names as breakpoints in the graph


void Executor::setListOfBreakPoints(std::list<std::string> listOfBreakPoints)
{
  DEBTRACE("Executor::setListOfBreakPoints(std::list<std::string> listOfBreakPoints)");
  { // --- Critical section
    YACS::BASES::AutoLocker<YACS::BASES::Mutex> alck(&_mutexForSchedulerUpdate);
    _isRunningunderExternalControl=true;
    _listOfBreakPoints = listOfBreakPoints;
  } // --- End of critical section
}


//! Get the list of tasks to load, to define a subset to execute in step by step mode
/*!
 *  If the executor is not in mode YACS::WAITINGTASKS nor YACS::PAUSED, the list is empty.
 *  Use Executor::waitPause to wait.
 */
std::list<std::string> Executor::getTasksToLoad()
{
  DEBTRACE("Executor::getTasksToLoad()");
  list<string> listOfNodesToLoad;
  listOfNodesToLoad.clear();
  { // --- Critical section
    YACS::BASES::AutoLocker<YACS::BASES::Mutex> alck(&_mutexForSchedulerUpdate);
    _isRunningunderExternalControl=true;
    switch (_executorState)
      {
      case YACS::WAITINGTASKS:
      case YACS::PAUSED:
        {
          listOfNodesToLoad = _listOfTasksToLoad;
          break;
        }
      case YACS::NOTYETINITIALIZED:
      case YACS::INITIALISED:
      case YACS::RUNNING:
      case YACS::FINISHED:
      case YACS::STOPPED:
      default:
        {
          break;
        }
      }
  } // --- End of critical section
  return listOfNodesToLoad;
}


//! Define a subset of task to execute in step by step mode
/*!
 * Behaviour is unpredictable if the list is not a subset of the list given by Executor::getTasksToLoad
 * in the current step.
 * If some nodes must run in parallel, they must stay together in the list.
 */

bool Executor::setStepsToExecute(std::list<std::string> listToExecute)
{
  DEBTRACE("Executor::setStepsToExecute(std::list<std::string> listToExecute)");
  bool ret = true;
  vector<Task *>::iterator iter;
  vector<Task *> restrictedTasks;
  { // --- Critical section
    YACS::BASES::AutoLocker<YACS::BASES::Mutex> alck(&_mutexForSchedulerUpdate);
    _isRunningunderExternalControl=true;
    switch (_executorState)
      {
      case YACS::WAITINGTASKS:
      case YACS::PAUSED:
        {
          for (iter=_tasksSave.begin(); iter!=_tasksSave.end(); iter++)
            {
              string readyNode = _mainSched->getTaskName(*iter);
              if (find(listToExecute.begin(), listToExecute.end(), readyNode)
                  != listToExecute.end())
                {
                  restrictedTasks.push_back(*iter);
                  DEBTRACE("node to execute " << readyNode);
                }
            }
          _tasks.clear();
          for (iter=restrictedTasks.begin(); iter!=restrictedTasks.end(); iter++)
            {
              _tasks.push_back(*iter);
            }
          break;
        }
      case YACS::NOTYETINITIALIZED:
      case YACS::INITIALISED:
      case YACS::RUNNING:
      case YACS::FINISHED:
      case YACS::STOPPED:
      default:
        {
          break;
        }
      }
    } // --- End of critical section

  _tasks.clear();
  for (iter=restrictedTasks.begin(); iter!=restrictedTasks.end(); iter++)
    {
      _tasks.push_back(*iter);
    }
  for (iter=_tasks.begin(); iter!=_tasks.end(); iter++)
    {
      string readyNode = _mainSched->getTaskName(*iter);
      DEBTRACE("selected node to execute " << readyNode);
    }

  return ret;
}

//! suspend pilot execution until Executor is in pause or waiting tasks completion mode.
/*!
 *  Do nothing if execution is finished or in pause.
 *  Wait first step if Executor is running or in initialization.
 */

void Executor::waitPause()
{
  DEBTRACE("Executor::waitPause()" << _executorState);
  { // --- Critical section
    YACS::BASES::AutoLocker<YACS::BASES::Mutex> alck(&_mutexForSchedulerUpdate);
    _isRunningunderExternalControl=true;
    switch (_executorState)
      {
      default:
      case YACS::STOPPED:
      case YACS::FINISHED:
      case YACS::WAITINGTASKS:
      case YACS::PAUSED:
        {
          break;
        }
      case YACS::NOTYETINITIALIZED:
      case YACS::INITIALISED:
      case YACS::RUNNING:
        {
          _condForPilot.wait(_mutexForSchedulerUpdate); // wait until executor is PAUSED or WAITINGTASKS
          break;
        }
      }
  } // --- End of critical section
  DEBTRACE("---");
}

//! stops the execution as soon as possible 

void Executor::stopExecution()
{
  setExecMode(YACS::STEPBYSTEP);
  //waitPause();
  _isOKToEnd = true;
  resumeCurrentBreakPoint();
}

//! save the current state of execution in an xml file

bool Executor::saveState(const std::string& xmlFile)
{
  DEBTRACE("Executor::saveState() in " << xmlFile);
  bool result = false;
  try {
    YACS::ENGINE::VisitorSaveState vst(_root);
    vst.openFileDump(xmlFile.c_str());
    _root->accept(&vst);
    vst.closeFileDump();
    result = true;
  }
  catch(Exception& ex) {
    std::cerr << ex.what() << std::endl;
  }
  return result;
}

//! not yet implemented

bool Executor::loadState()
{
  DEBTRACE("Executor::loadState()");
  _isRunningunderExternalControl=true;
  return true;
}


static int isfile(const char *filename) 
{
  struct stat buf;
  if (stat(filename, &buf) != 0)
    return 0;
  if (!S_ISREG(buf.st_mode))
    return 0;
  return 1;
}

//! Display the graph state as a dot display, public method

void Executor::displayDot(Scheduler *graph)
{
  _isRunningunderExternalControl=true;
  _displayDot(graph);
}

//! Display the graph state as a dot display
/*!
 *  \param graph  : the node to display
 */

void Executor::_displayDot(Scheduler *graph)
{
   std::ofstream g("titi");
   ((ComposedNode*)graph)->writeDot(g);
   g.close();
   const char displayScript[]="display.sh";
   if(isfile(displayScript))
     system("sh display.sh");
   else
     system("dot -Tpng titi|display -delay 5");
}

//! Wait reactivation in modes Step By step or with BreakPoints
/*!
 *  Check mode of execution (set by main thread):
 *  - YACS::CONTINUE        : the graph execution continues.
 *  - YACS::STEPBYSTEP      : wait on condition (pilot thread, Executor::resumeCurrentBreakPoint)
 *  - YACS::STOPBEFORENODES : if there are ready nodes in a list of breakpoints,
 *                            wait on condition (pilot thread, Executor::resumeCurrentBreakPoint)
 *                            else continue the graph execution.
 *  \return true if end of executor thread is requested
 */

bool Executor::checkBreakPoints()
{
  DEBTRACE("Executor::checkBreakPoints()");
  vector<Task *>::iterator iter;
  bool endRequested = false;

  switch (_execMode)
    {
    case YACS::CONTINUE:
      {
        break;
      }
    case YACS::STOPBEFORENODES:
      {
        bool stop = false;
        { // --- Critical section
          YACS::BASES::AutoLocker<YACS::BASES::Mutex> alck(&_mutexForSchedulerUpdate);
          _tasksSave = _tasks;
          for (iter=_tasks.begin(); iter!=_tasks.end(); iter++)
            {
              string nodeToLoad = _mainSched->getTaskName(*iter);
              if (find(_listOfBreakPoints.begin(), _listOfBreakPoints.end(), nodeToLoad)
                  != _listOfBreakPoints.end())
                {
                  stop = true;
                  break;
                }
            }
          if (stop)
            {
              _listOfTasksToLoad.clear();
              for (iter=_tasks.begin(); iter!=_tasks.end(); iter++)
                {
                  string nodeToLoad = _mainSched->getTaskName(*iter);
                  _listOfTasksToLoad.push_back(nodeToLoad);
                }
              if (getNbOfThreads())
                _executorState = YACS::WAITINGTASKS; // will be paused after completion of running tasks
              else
                _executorState = YACS::PAUSED;
              sendEvent("executor");
              _condForPilot.notify_all();
            }
          if (stop && !_isOKToEnd) waitResume(); // wait until pilot calls resumeCurrentBreakPoint(), mutex released during wait 
          if (_isOKToEnd) endRequested = true;
        } // --- End of critical section
          if (stop) DEBTRACE("wake up from waitResume");
        break;
      }
    default:
    case YACS::STEPBYSTEP:
      {
        { // --- Critical section
          YACS::BASES::AutoLocker<YACS::BASES::Mutex> alck(&_mutexForSchedulerUpdate);
          _tasksSave = _tasks;
          _listOfTasksToLoad.clear();
          for (iter=_tasks.begin(); iter!=_tasks.end(); iter++)
            {
              string nodeToLoad = _mainSched->getTaskName(*iter);
              _listOfTasksToLoad.push_back(nodeToLoad);
            }
          if (getNbOfThreads())
            _executorState = YACS::WAITINGTASKS; // will be paused after completion of running tasks
          else
            _executorState = YACS::PAUSED;
          sendEvent("executor");
          _condForPilot.notify_all();
          if (!_isOKToEnd)
            waitResume(); // wait until pilot calls resumeCurrentBreakPoint(), mutex released during wait
                          // or, if no pilot, wait until no more running tasks (stop on error)
          if (_isOKToEnd) endRequested = true;
        } // --- End of critical section
        DEBTRACE("wake up from waitResume");
        break;
      }
    }
  DEBTRACE("endRequested: " << endRequested);
  return endRequested;
}


//! in modes Step By step or with BreakPoint, wait until pilot resumes the execution
/*!
 *  With the condition Mutex, the mutex is released atomically during the wait.
 *  Pilot calls Executor::resumeCurrentBreakPoint to resume execution.
 *  Must be called while mutex is locked.
 */

void Executor::waitResume()
{
  DEBTRACE("Executor::waitResume()");
  _condForStepByStep.wait(_mutexForSchedulerUpdate); // wait until pilot calls resumeCurrentBreakPoint()
  DEBTRACE("---");
}


//! Perform loading of a Task.
/*!
 *  \param task  : Task to load
 */

void Executor::loadTask(Task *task, const Executor *execInst)
{
  DEBTRACE("Executor::loadTask(Task *task)");
  if(task->getState() != YACS::TOLOAD)
    return;
  traceExec(task, "state:TOLOAD", ComputePlacement(task));
  {//Critical section
    YACS::BASES::AutoLocker<YACS::BASES::Mutex> alck(&_mutexForSchedulerUpdate);
    _mainSched->notifyFrom(task,YACS::START,execInst);
  }//End of critical section
  try
    {
      traceExec(task, "load", ComputePlacement(task));
      task->load();
      traceExec(task, "initService", ComputePlacement(task));
      task->initService();
    }
  catch(Exception& ex) 
    {
      std::cerr << ex.what() << std::endl;
      {//Critical section
        YACS::BASES::AutoLocker<YACS::BASES::Mutex> alck(&_mutexForSchedulerUpdate);
        task->aborted();
        _mainSched->notifyFrom(task,YACS::ABORT,execInst);
        traceExec(task, "state:"+Node::getStateName(task->getState()), ComputePlacement(task));
      }//End of critical section
    }
  catch(...) 
    {
      std::cerr << "Load failed" << std::endl;
      {//Critical section
        YACS::BASES::AutoLocker<YACS::BASES::Mutex> alck(&_mutexForSchedulerUpdate);
        task->aborted();
        _mainSched->notifyFrom(task,YACS::ABORT,execInst);
        traceExec(task, "state:"+Node::getStateName(task->getState()), ComputePlacement(task));
      }//End of critical section
    }
}

struct threadargs
{
  Task *task;
  Scheduler *sched;
  Executor *execInst;
};

void Executor::loadTasks(const std::vector<Task *>& tasks, const Executor *execInst)
{
  for(std::vector<Task *>::const_iterator iter = _tasks.begin(); iter != _tasks.end(); iter++)
    loadTask(*iter,execInst);
}

void Executor::loadParallelTasks(const std::vector<Task *>& tasks, const Executor *execInst)
{
  std::vector<Thread> ths(tasks.size());
  std::size_t ithread(0);
  for(std::vector<Task *>::const_iterator iter = _tasks.begin(); iter != _tasks.end(); iter++, ithread++)
    {
      DEBTRACE("Executor::loadParallelTasks(Task *task)");
      struct threadargs *args(new threadargs);
      args->task = (*iter);
      args->sched = _mainSched;
      args->execInst = this;
      ths[ithread].go(functionForTaskLoad, args, _threadStackSize);
    }
  for(ithread=0;ithread<tasks.size();ithread++)
    ths[ithread].join();
}

//! Execute a list of tasks possibly connected through datastream links
/*!
 *  \param tasks  : a list of tasks to execute
 *
 */
void Executor::launchTasks(const std::vector<Task *>& tasks)
{
  //First phase, make datastream connections
  for(vector<Task *>::const_iterator iter=tasks.begin();iter!=tasks.end();iter++)
    {
      YACS::StatesForNode state=(*iter)->getState();
      if(state != YACS::TOLOAD && state != YACS::TORECONNECT)continue;
      try
        {
          (*iter)->connectService();
          traceExec(*iter, "connectService",ComputePlacement(*iter));
          {//Critical section
            YACS::BASES::AutoLocker<YACS::BASES::Mutex> alck(&_mutexForSchedulerUpdate);
            (*iter)->connected();
          }//End of critical section
        }
      catch(Exception& ex) 
        {
          std::cerr << ex.what() << std::endl;
          try
            {
              (*iter)->disconnectService();
              traceExec(*iter, "disconnectService",ComputePlacement(*iter));
            }
          catch(...) 
            {
              // Disconnect has failed
              traceExec(*iter, "disconnectService failed, ABORT",ComputePlacement(*iter));
            }
          {//Critical section
            YACS::BASES::AutoLocker<YACS::BASES::Mutex> alck(&_mutexForSchedulerUpdate);
            (*iter)->aborted();
            _mainSched->notifyFrom(*iter,YACS::ABORT,this);
          }//End of critical section
        }
      catch(...) 
        {
          std::cerr << "Problem in connectService" << std::endl;
          try
            {
              (*iter)->disconnectService();
              traceExec(*iter, "disconnectService",ComputePlacement(*iter));
            }
          catch(...) 
            {
              // Disconnect has failed
              traceExec(*iter, "disconnectService failed, ABORT",ComputePlacement(*iter));
            }
          {//Critical section
            YACS::BASES::AutoLocker<YACS::BASES::Mutex> alck(&_mutexForSchedulerUpdate);
            (*iter)->aborted();
            _mainSched->notifyFrom(*iter,YACS::ABORT,this);
          }//End of critical section
        }
      if((*iter)->getState() == YACS::ERROR)
        {
          //try to put all coupled tasks in error
          std::set<Task*> coupledSet;
          (*iter)->getCoupledTasks(coupledSet);
          for (std::set<Task*>::iterator it = coupledSet.begin(); it != coupledSet.end(); ++it)
            {
              Task* t=*it;
              if(t == *iter)continue;
              if(t->getState() == YACS::ERROR)continue;
              try
                {
                  t->disconnectService();
                  traceExec(t, "disconnectService",ComputePlacement(*iter));
                }
              catch(...)
                {
                  // Disconnect has failed
                  traceExec(t, "disconnectService failed, ABORT",ComputePlacement(*iter));
                }
              {//Critical section
                YACS::BASES::AutoLocker<YACS::BASES::Mutex> alck(&_mutexForSchedulerUpdate);
                t->aborted();
                _mainSched->notifyFrom(t,YACS::ABORT,this);
              }//End of critical section
              traceExec(t, "state:"+Node::getStateName(t->getState()),ComputePlacement(*iter));
            }
        }
      traceExec(*iter, "state:"+Node::getStateName((*iter)->getState()),ComputePlacement(*iter));
    }

  //Second phase, execute each task in a thread
  for(vector<Task *>::const_iterator iter=tasks.begin();iter!=tasks.end();iter++)
    {
      launchTask(*iter);
    }
}

//! Execute a Task in a thread
/*!
 *  \param task  : Task to execute
 *
 *  Calls Scheduler::notifyFrom of main node (_mainSched) to notify start
 *
 *  Calls Executor::functionForTaskExecution in Thread
 */

void Executor::launchTask(Task *task)
{
  DEBTRACE("Executor::launchTask(Task *task)");
  struct threadargs *args;
  if(task->getState() != YACS::TOACTIVATE)return;

  DEBTRACE("before _semForMaxThreads.wait " << _semThreadCnt);
  if(_semThreadCnt == 0)
    {
      // --- Critical section
      YACS::BASES::AutoLocker<YACS::BASES::Mutex> alck(&_mutexForSchedulerUpdate);
      //check if we have enough threads to run
      std::set<Task*> tmpSet=_runningTasks;
      std::set<Task*>::iterator it = tmpSet.begin();
      std::string status="running";
      std::set<Task*> coupledSet;
      while( it != tmpSet.end() )
        {
          Task* tt=*it;
          coupledSet.clear();
          tt->getCoupledTasks(coupledSet);
          status="running";
          for (std::set<Task*>::iterator iter = coupledSet.begin(); iter != coupledSet.end(); ++iter)
            {
              if((*iter)->getState() == YACS::TOACTIVATE)status="toactivate";
              tmpSet.erase(*iter);
            }
          if(status=="running")break;
          it = tmpSet.begin();
        }

      if(status=="toactivate")
        {
          std::cerr << "WARNING: maybe you need more threads to run your schema (current value="<< _maxThreads << ")" << std::endl;
          std::cerr << "If it is the case, set the YACS_MAX_THREADS environment variable to a bigger value (export YACS_MAX_THREADS=xxx)" << std::endl;
        }
      // --- End of critical section
    }

  _semForMaxThreads.wait();
  _semThreadCnt -= 1;

  args= new threadargs;
  args->task = task;
  args->sched = _mainSched;
  args->execInst = this;

  traceExec(task, "launch",ComputePlacement(task));

  { // --- Critical section
    YACS::BASES::AutoLocker<YACS::BASES::Mutex> alck(&_mutexForSchedulerUpdate);
    _numberOfRunningTasks++;
    _runningTasks.insert(task);
    task->begin(); //change state to ACTIVATED
  } // --- End of critical section
  Thread(functionForTaskExecution, args, _threadStackSize);
}

//! wait until a running task ends

void Executor::sleepWhileNoEventsFromAnyRunningTask()
{
  DEBTRACE("Executor::sleepWhileNoEventsFromAnyRunningTask()");
//   _semForNewTasksToPerform.wait(); //----utiliser pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
  YACS::BASES::AutoLocker<YACS::BASES::Mutex> alck(&_mutexForSchedulerUpdate);
  if (_numberOfRunningTasks > 0 && _numberOfEndedTasks==0)
    {
      _isWaitingEventsFromRunningTasks = true;
      _condForNewTasksToPerform.wait(_mutexForSchedulerUpdate); // mutex released during wait
    }
  _numberOfEndedTasks=0;
  DEBTRACE("---");
}

//! not implemented

void Executor::notifyEndOfThread(YACS::BASES::Thread *thread)
{
  /*_mutexForNbOfConcurrentThreads.lock();
  _groupOfAllThreadsCreated.remove(thread);
  delete thread;
  _mutexForNbOfConcurrentThreads.unlock();*/
}


//! must be used protected by _mutexForSchedulerUpdate!

void Executor::wakeUp()
{
  DEBTRACE("Executor::wakeUp() " << _isWaitingEventsFromRunningTasks);
  if (_isWaitingEventsFromRunningTasks)
    {
      _isWaitingEventsFromRunningTasks = false;
      _condForNewTasksToPerform.notify_all();
    }
  else
    _numberOfEndedTasks++;
}

//! number of running tasks

int Executor::getNbOfThreads()
{
  int ret;
  YACS::BASES::AutoLocker<YACS::BASES::Mutex> alck(&_mutexForNbOfConcurrentThreads);
  _isRunningunderExternalControl=true;
  ret = _groupOfAllThreadsCreated.size();
  return ret;
}

/*!
 * This thread is NOT supposed to be detached !
 */
void *Executor::functionForTaskLoad(void *arg)
{
  DEBTRACE("Executor::functionForTaskLoad(void *arg)");
  struct threadargs *args = (struct threadargs *) arg;
  Task *task=args->task;
  Scheduler *sched=args->sched;
  Executor *execInst=args->execInst;
  delete args;
  execInst->loadTask(task,execInst);// no throw of this method - all throw are catched !
  return 0;
}

//! Function to perform execution of a task in a thread
/*!
 *  \param arg  : 3 elements (a Task, a Scheduler, an Executor)
 *
 *  Calls Task::execute
 *
 *  Calls Task::finished when the task is finished
 *
 *  Calls (notify with event YACS::FINISH) Scheduler::notifyFrom when the task is finished
 *
 *  Calls Executor::wakeUp and Executor::notifyEndOfThread
 */

void *Executor::functionForTaskExecution(void *arg)
{
  DEBTRACE("Executor::functionForTaskExecution(void *arg)");

  struct threadargs *args = (struct threadargs *) arg;
  Task *task=args->task;
  Scheduler *sched=args->sched;
  Executor *execInst=args->execInst;
  delete args;
  execInst->traceExec(task, "state:"+Node::getStateName(task->getState()),ComputePlacement(task));

  Thread::detach();

  // Execute task

  if(execInst->getDPLScopeSensitive())
    {
      Node *node(dynamic_cast<Node *>(task));
      ComposedNode *gfn(dynamic_cast<ComposedNode *>(sched));
      if(node!=0 && gfn!=0)
        node->applyDPLScope(gfn);
    }

  YACS::Event ev=YACS::FINISH;
  try
    {
      execInst->traceExec(task, "start execution",ComputePlacement(task));
      task->execute();
      execInst->traceExec(task, "end execution OK",ComputePlacement(task));
    }
  catch(Exception& ex)
    {
      std::cerr << "YACS Exception during execute" << std::endl;
      std::cerr << ex.what() << std::endl;
      ev=YACS::ABORT;
      string message = "end execution ABORT, ";
      message += ex.what();
      execInst->traceExec(task, message,ComputePlacement(task));
    }
  catch(...) 
    {
      // Execution has failed
      std::cerr << "Execution has failed: unknown reason" << std::endl;
      ev=YACS::ABORT;
      execInst->traceExec(task, "end execution ABORT, unknown reason",ComputePlacement(task));
    }

  // Disconnect task
  try
    {
      DEBTRACE("task->disconnectService()");
      task->disconnectService();
      execInst->traceExec(task, "disconnectService",ComputePlacement(task));
    }
  catch(...) 
    {
      // Disconnect has failed
      std::cerr << "disconnect has failed" << std::endl;
      ev=YACS::ABORT;
      execInst->traceExec(task, "disconnectService failed, ABORT",ComputePlacement(task));
    }
  //

  std::string placement(ComputePlacement(task));

  // container management for HomogeneousPoolOfContainer

  HomogeneousPoolContainer *contC(dynamic_cast<HomogeneousPoolContainer *>(task->getContainer()));
  if(contC)
    {
      YACS::BASES::AutoLocker<Container> alckCont(contC);
      contC->release(task);
    }

  DEBTRACE("End task->execute()");
  { // --- Critical section
    YACS::BASES::AutoLocker<YACS::BASES::Mutex> alck(&execInst->_mutexForSchedulerUpdate);
    try
      {
        if (ev == YACS::FINISH) task->finished();
        if (ev == YACS::ABORT)
          {
            execInst->_errorDetected = true;
            if (execInst->_stopOnErrorRequested)
              {
                execInst->_execMode = YACS::STEPBYSTEP;
                execInst->_isOKToEnd = true;
              }
            task->aborted();
          }
        execInst->traceExec(task, "state:"+Node::getStateName(task->getState()),placement);
        sched->notifyFrom(task,ev,execInst);
      }
    catch(Exception& ex)
      {
        //notify has failed : it is supposed to have set state
        //so no need to do anything
        std::cerr << "Error during notification" << std::endl;
        std::cerr << ex.what() << std::endl;
      }
    catch(...)
      {
        //notify has failed : it is supposed to have set state
        //so no need to do anything
        std::cerr << "Notification failed" << std::endl;
      }
    execInst->_numberOfRunningTasks--;
    execInst->_runningTasks.erase(task);
    DEBTRACE("_numberOfRunningTasks: " << execInst->_numberOfRunningTasks 
             << " _execMode: " << execInst->_execMode
             << " _executorState: " << execInst->_executorState);
    if ((execInst->_numberOfRunningTasks == 0) && (execInst->_execMode != YACS::CONTINUE)) // no more running tasks
      {
        if (execInst->_executorState == YACS::WAITINGTASKS)
          {
            execInst->_executorState = YACS::PAUSED;
            execInst->sendEvent("executor");
            execInst->_condForPilot.notify_all();
            if (execInst->_errorDetected &&
                execInst->_stopOnErrorRequested &&
                !execInst->_isRunningunderExternalControl)
              execInst->_condForStepByStep.notify_all(); // exec thread may be on waitResume
          }
      }
    DEBTRACE("before _semForMaxThreads.post " << execInst->_semThreadCnt);
    execInst->_semForMaxThreads.post();
    execInst->_semThreadCnt += 1;
    DEBTRACE("after _semForMaxThreads.post " << execInst->_semThreadCnt);
    if (execInst->_executorState != YACS::PAUSED)  execInst->wakeUp();

  } // --- End of critical section (change state)

  //execInst->notifyEndOfThread(0);
  Thread::exit(0);
  return 0;
}

void Executor::traceExec(Task *task, const std::string& message, const std::string& placement)
{
  string nodeName = _mainSched->getTaskName(task);
  Container *cont = task->getContainer();
  string containerName = "---";
  if (cont)
    containerName = cont->getName();

#ifdef WIN32
  DWORD now = timeGetTime();
  double elapse = (now - _start)/1000.0;
#else
  timeval now;
  gettimeofday(&now, NULL);
  double elapse = (now.tv_sec - _start.tv_sec) + double(now.tv_usec - _start.tv_usec)/1000000.0;
#endif
  {
    YACS::BASES::AutoLocker<YACS::BASES::Mutex> alck(&_mutexForTrace);
    _trace << elapse << " " << containerName << " " << placement << " " << nodeName << " " << message << endl;
    _trace << flush;
  }
}

//! emit notification to all observers registered with  the dispatcher 
/*!
 * The dispatcher is unique and can be obtained by getDispatcher()
 */
void Executor::sendEvent(const std::string& event)
{
  Dispatcher* disp=Dispatcher::getDispatcher();
  YASSERT(disp);
  YASSERT(_root);
  disp->dispatch(_root,event);
}

/*!
 * This method takes in input a list of tasks and selects from that lists a part of it considering only the containers.
 * If tasks have no container instance subclass of HomogeneousPoolContainer this method will let the \a tsks untouched.
 *
 * \param [in,out] tsks - list of tasks to be
 */
void Executor::FilterTasksConsideringContainers(std::vector<Task *>& tsks)
{
  std::map<HomogeneousPoolContainer *, std::vector<Task *> > m;
  for(std::vector<Task *>::const_iterator it=tsks.begin();it!=tsks.end();it++)
    {
      Task *cur(*it);
      if(!cur)
        continue;
      Container *cont(cur->getContainer());
      if(!cont)
        {
          m[(HomogeneousPoolContainer *)NULL].push_back(cur);
          continue;
        }
      HomogeneousPoolContainer *contC(dynamic_cast<HomogeneousPoolContainer *>(cont));
      if(!contC)
        {
          m[(HomogeneousPoolContainer *)NULL].push_back(cur);
          continue;
        }
      m[contC].push_back(cur);
    }
  //
  std::vector<Task *> ret;
  for(std::map<HomogeneousPoolContainer *, std::vector<Task *> >::const_iterator it=m.begin();it!=m.end();it++)
    {
      HomogeneousPoolContainer *curhpc((*it).first);
      const std::vector<Task *>& curtsks((*it).second);
      if(!curhpc)
        {
          ret.insert(ret.end(),curtsks.begin(),curtsks.end());
        }
      else
        {
          // start of critical section for container curhpc
          YACS::BASES::AutoLocker<Container> alckForCont(curhpc);
          std::vector<const Task *> vecOfTaskSharingSameHPContToBeRunSimutaneously;
          std::size_t sz(curhpc->getNumberOfFreePlace());
          std::vector<Task *>::const_iterator it2(curtsks.begin());
          for(std::size_t i=0;i<sz && it2!=curtsks.end();i++,it2++)
            {
              vecOfTaskSharingSameHPContToBeRunSimutaneously.push_back(*it2);
              ret.push_back(*it2);
            }
          curhpc->allocateFor(vecOfTaskSharingSameHPContToBeRunSimutaneously);
          //end of critical section
        }
    }
  //
  tsks=ret;
}

std::string Executor::ComputePlacement(Task *zeTask)
{
  std::string placement("---");
  if(!zeTask)
    return placement;
  if(zeTask->getContainer())
    placement=zeTask->getContainer()->getFullPlacementId(zeTask);
  return placement;
}
