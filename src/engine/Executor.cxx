#include "Executor.hxx"
#include "Task.hxx"
#include "Scheduler.hxx"
#include "Dispatcher.hxx"

#include "VisitorSaveState.hxx"
#include "ComposedNode.hxx"

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <cassert>

using namespace YACS::ENGINE;
using namespace std;

using YACS::BASES::Mutex;
using YACS::BASES::Thread;
using YACS::BASES::Semaphore;

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

Executor::Executor():_nbOfConcurrentThreads(0), _semForMaxThreads(50)
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
  _semThreadCnt = 50;
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
  while(_toContinue)
    {
      sleepWhileNoEventsFromAnyRunningTask();

      if(debug>2)_displayDot(graph);

      {//Critical section
        _mutexForSchedulerUpdate.lock();
        tasks=graph->getNextTasks(isMore);
        graph->selectRunnableTasks(tasks);
        _mutexForSchedulerUpdate.unlock();
      }//End of critical section

      if(debug>2)_displayDot(graph);

      for(iter=tasks.begin();iter!=tasks.end();iter++)
        loadTask(*iter);

      if(debug>1)_displayDot(graph);

      launchTasks(tasks);

      if(debug>1)_displayDot(graph);

      {//Critical section
        _mutexForSchedulerUpdate.lock();
        _toContinue=!graph->isFinished();
        _mutexForSchedulerUpdate.unlock();
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
 *  \param fromscratch : if false, state from a previous partial exection is already loaded
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
    _mutexForSchedulerUpdate.lock();
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
    string tracefile = "traceExec_";
    tracefile += _mainSched->getName();
    _trace.open(tracefile.c_str());
    _mutexForSchedulerUpdate.unlock();
  } // --- End of critical section

  if (debug > 1) _displayDot(graph);

  if (fromScratch)
    {
      try
        {
          graph->init();
          graph->exUpdateState();
        }
      catch(Exception)
        {
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
        _mutexForSchedulerUpdate.lock();
        _tasks=graph->getNextTasks(isMore);
        numberAllTasks=_numberOfRunningTasks+_tasks.size();
        graph->selectRunnableTasks(_tasks);
        _mutexForSchedulerUpdate.unlock();
      } // --- End of critical section
      if (debug > 2) _displayDot(graph);
      if (_executorState == YACS::RUNNING)
        {
          if (checkBreakPoints()) break; // end of thread requested, OK to exit at once;
          if (debug > 0) _displayDot(graph);
          DEBTRACE("---");
          for (iter = _tasks.begin(); iter != _tasks.end(); iter++)
            loadTask(*iter);
          if (debug > 1) _displayDot(graph);
          DEBTRACE("---");
          launchTasks(_tasks);
          DEBTRACE("---");
        }
      if (debug > 1) _displayDot(graph);
      { // --- Critical section
        DEBTRACE("---");
        _mutexForSchedulerUpdate.lock();
        _toContinue = !graph->isFinished();

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
        _mutexForSchedulerUpdate.unlock();
      } // --- End of critical section
      if (debug > 0) _displayDot(graph);
      DEBTRACE("_toContinue: " << _toContinue);
    }

  DEBTRACE("End of main Loop");

  { // --- Critical section
    _mutexForSchedulerUpdate.lock();
    if ( _toContinue) // --- break while(): request to stop detected on checkBreakPoints()
      {
        DEBTRACE("stop requested: End soon");
        _executorState = YACS::STOPPED;
        _toContinue = false;
        sendEvent("executor");
      }
    _mutexForSchedulerUpdate.unlock();
  } // --- End of critical section
  if ( _dumpOnErrorRequested && _errorDetected)
    {
      saveState(_dumpErrorFile);
    }
  _trace.close();
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
    _mutexForSchedulerUpdate.lock();
    _dumpErrorFile=xmlFile;
    _stopOnErrorRequested=true;
    _dumpOnErrorRequested = dumpRequested;
    if (dumpRequested && xmlFile.empty())
      throw YACS::Exception("dump on error requested and no filename given for dump");
    _mutexForSchedulerUpdate.unlock();
    DEBTRACE("_dumpErrorFile " << _dumpErrorFile << " " << _dumpOnErrorRequested);
  } // --- End of critical section
}

//! ask to do not stop execution on nodes found in error
/*!
 */

void Executor::unsetStopOnError()
{
  { // --- Critical section
    _mutexForSchedulerUpdate.lock();
    _stopOnErrorRequested=false;
    _mutexForSchedulerUpdate.unlock();
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
    _mutexForSchedulerUpdate.lock();
    _isRunningunderExternalControl=true;
    _execMode = mode;
    _mutexForSchedulerUpdate.unlock();
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
    _mutexForSchedulerUpdate.lock();
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
    _mutexForSchedulerUpdate.unlock();
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
    _mutexForSchedulerUpdate.lock();
    _isRunningunderExternalControl=true;
    _listOfBreakPoints = listOfBreakPoints;
    _mutexForSchedulerUpdate.unlock();
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
    _mutexForSchedulerUpdate.lock();
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
    _mutexForSchedulerUpdate.unlock();
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
  bool ret = false;
  vector<Task *>::iterator iter;
  vector<Task *> restrictedTasks;
  { // --- Critical section
    _mutexForSchedulerUpdate.lock();
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
    _mutexForSchedulerUpdate.unlock();
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
    _mutexForSchedulerUpdate.lock();
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
    _mutexForSchedulerUpdate.unlock();
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
  YACS::ENGINE::VisitorSaveState vst(_root);
  vst.openFileDump(xmlFile.c_str());
  _root->accept(&vst);
  vst.closeFileDump();
}

//! not yet implemented

bool Executor::loadState()
{
  DEBTRACE("Executor::loadState()");
  _isRunningunderExternalControl=true;
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
          _mutexForSchedulerUpdate.lock();
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
          //_mutexForSchedulerUpdate.unlock(); 
          //} // --- End of critical section
          if (stop && !_isOKToEnd) waitResume(); // wait until pilot calls resumeCurrentBreakPoint(), mutex released during wait 
          if (_isOKToEnd) endRequested = true;
          _mutexForSchedulerUpdate.unlock();
        } // --- End of critical section
          if (stop) DEBTRACE("wake up from waitResume");
        break;
      }
    default:
    case YACS::STEPBYSTEP:
      {
        { // --- Critical section
          _mutexForSchedulerUpdate.lock();
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
          _mutexForSchedulerUpdate.unlock();
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

void Executor::loadTask(Task *task)
{
  DEBTRACE("Executor::loadTask(Task *task)");
  if(task->getState() != YACS::TOLOAD)return;
  {//Critical section
    _mutexForSchedulerUpdate.lock();
    task->loaded();
    _mainSched->notifyFrom(task,YACS::START);
    _mutexForSchedulerUpdate.unlock();
  }//End of critical section
  try
    {
      task->load();
    }
  catch(Exception& ex) 
    {
      std::cerr << ex.what() << std::endl;
      {//Critical section
        _mutexForSchedulerUpdate.lock();
        task->aborted();
        _mainSched->notifyFrom(task,YACS::ABORT);
        _mutexForSchedulerUpdate.unlock();
      }//End of critical section
    }
  catch(...) 
    {
      std::cerr << "Load failed" << std::endl;
      {//Critical section
        _mutexForSchedulerUpdate.lock();
        task->aborted();
        _mainSched->notifyFrom(task,YACS::ABORT);
        _mutexForSchedulerUpdate.unlock();
      }//End of critical section
    }
}


//! Execute a list of tasks possibly connected through datastream links
/*!
 *  \param tasks  : a list of tasks to execute
 *
 */
void Executor::launchTasks(std::vector<Task *>& tasks)
{
  vector<Task *>::iterator iter;
  //First phase, initialize the execution
  for(iter=tasks.begin();iter!=tasks.end();iter++)
    {
      if((*iter)->getState() != YACS::TOACTIVATE)continue;
      try
        {
          (*iter)->initService();
          traceExec(*iter, "initService");
        }
      catch(Exception& ex) 
        {
          std::cerr << ex.what() << std::endl;
          {//Critical section
            _mutexForSchedulerUpdate.lock();
            (*iter)->aborted();
            _mainSched->notifyFrom(*iter,YACS::ABORT);
            _mutexForSchedulerUpdate.unlock();
          }//End of critical section
        }
      catch(...) 
        {
          std::cerr << "Problem in initService" << std::endl;
          {//Critical section
            _mutexForSchedulerUpdate.lock();
            (*iter)->aborted();
            _mainSched->notifyFrom(*iter,YACS::ABORT);
            _mutexForSchedulerUpdate.unlock();
          }//End of critical section
        }
    }
  //Second phase, make datastream connections
  for(iter=tasks.begin();iter!=tasks.end();iter++)
    {
      if((*iter)->getState() != YACS::TOACTIVATE)continue;
      try
        {
          (*iter)->connectService();
          traceExec(*iter, "connectService");
        }
      catch(Exception& ex) 
        {
          std::cerr << ex.what() << std::endl;
          {//Critical section
            _mutexForSchedulerUpdate.lock();
            (*iter)->aborted();
            _mainSched->notifyFrom(*iter,YACS::ABORT);
            _mutexForSchedulerUpdate.unlock();
          }//End of critical section
        }
      catch(...) 
        {
          std::cerr << "Problem in connectService" << std::endl;
          {//Critical section
            _mutexForSchedulerUpdate.lock();
            (*iter)->aborted();
            _mainSched->notifyFrom(*iter,YACS::ABORT);
            _mutexForSchedulerUpdate.unlock();
          }//End of critical section
        }
    }
  //Third phase, execute each task in a thread
  for(iter=tasks.begin();iter!=tasks.end();iter++)
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
  if(task->getState() != YACS::TOACTIVATE)return;

  DEBTRACE("before _semForMaxThreads.wait " << _semThreadCnt);
  _semForMaxThreads.wait();
  _semThreadCnt -= 1;

  void **args=new void *[3];
  args[0]=(void *)task;
  args[1]=(void *)_mainSched;
  args[2]=(void *)this;
  traceExec(task, "launch");

  { // --- Critical section
    _mutexForSchedulerUpdate.lock();
    _numberOfRunningTasks++;
    task->begin(); //change state to ACTIVATED
    //no more need : done when loading
    //_mainSched->notifyFrom(task,YACS::START);
    _mutexForSchedulerUpdate.unlock();
  } // --- End of critical section
  Thread(functionForTaskExecution,args);
  //functionForTaskExecution(args);
}

//! wait until a running task ends

void Executor::sleepWhileNoEventsFromAnyRunningTask()
{
  DEBTRACE("Executor::sleepWhileNoEventsFromAnyRunningTask()");
//   _semForNewTasksToPerform.wait(); //----utiliser pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
  _mutexForSchedulerUpdate.lock();
  if (_numberOfRunningTasks > 0)
    {
      _isWaitingEventsFromRunningTasks = true;
      _condForNewTasksToPerform.wait(_mutexForSchedulerUpdate); // mutex released during wait
    }
  _mutexForSchedulerUpdate.unlock();
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
  DEBTRACE("Executor::wakeUp()");
  if (_isWaitingEventsFromRunningTasks)
    {
      _isWaitingEventsFromRunningTasks = false;
      _condForNewTasksToPerform.notify_all();
    }
}

//! number of running tasks

int Executor::getNbOfThreads()
{
  int ret;
  _mutexForNbOfConcurrentThreads.lock();
  _isRunningunderExternalControl=true;
  ret = _groupOfAllThreadsCreated.size();
  _mutexForNbOfConcurrentThreads.unlock();
  return ret;
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
  void **argT=(void **)arg;
  Task *task=(Task *)argT[0];
  Scheduler *sched=(Scheduler *)argT[1];
  Executor *execInst=(Executor *)argT[2];
  delete [] argT;
  Thread::detach();

  // Execute task

  YACS::Event ev=YACS::FINISH;
  try
    {
      execInst->traceExec(task, "start execution");
      task->execute();
      execInst->traceExec(task, "end execution OK");
    }
  catch(Exception& ex)
    {
      std::cerr << "YACS Exception during execute" << std::endl;
      std::cerr << ex.what() << std::endl;
      ev=YACS::ABORT;
      string message = "end execution ABORT, ";
      message += ex.what();
      execInst->traceExec(task, message);
    }
  catch(...) 
    {
      // Execution has failed
      std::cerr << "Execution has failed: unknown reason" << std::endl;
      ev=YACS::ABORT;
      execInst->traceExec(task, "end execution ABORT, unknown reason");
    }

  // Disconnect task
  try
    {
      task->disconnectService();
      execInst->traceExec(task, "disconnectService");
    }
  catch(...) 
    {
      // Disconnect has failed
      std::cerr << "disconnect has failed" << std::endl;
      ev=YACS::ABORT;
      execInst->traceExec(task, "disconnectService failed, ABORT");
    }

  DEBTRACE("End task->execute()");
  { // --- Critical section
    execInst->_mutexForSchedulerUpdate.lock();
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
        sched->notifyFrom(task,ev);
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

    execInst->_mutexForSchedulerUpdate.unlock();
  } // --- End of critical section (change state)

  //execInst->notifyEndOfThread(0);
  Thread::exit(0);
  return 0;
}

void Executor::traceExec(Task *task, const std::string& message)
{
  string nodeName = _mainSched->getTaskName(task);
  _mutexForTrace.lock();
  _trace << nodeName << " " << message << endl;
  _trace << flush;
  _mutexForTrace.unlock();
}

//! emit notification to all observers registered with  the dispatcher 
/*!
 * The dispatcher is unique and can be obtained by getDispatcher()
 */
void Executor::sendEvent(const std::string& event)
{
  Dispatcher* disp=Dispatcher::getDispatcher();
  assert(disp);
  assert(_root);
  disp->dispatch(_root,event);
}
