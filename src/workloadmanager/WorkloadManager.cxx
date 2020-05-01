// Copyright (C) 2020  CEA/DEN, EDF R&D
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
#include "WorkloadManager.hxx"
#include "Task.hxx"

namespace WorkloadManager
{
  WorkloadManager::WorkloadManager(WorkloadAlgorithm& algo)
  : _runningTasks()
  , _finishedTasks()
  , _nextIndex(0)
  , _data_mutex()
  , _startCondition()
  , _endCondition()
  , _stop(false)
  , _otherThreads()
  , _algo(algo)
  {
  }
  
  WorkloadManager::~WorkloadManager()
  {
    stop();
  }
  
  void WorkloadManager::addResource(const Resource& r)
  {
    std::unique_lock<std::mutex> lock(_data_mutex);
    _algo.addResource(r);
    _startCondition.notify_one();
  }
  
  void WorkloadManager::addTask(Task* t)
  {
    std::unique_lock<std::mutex> lock(_data_mutex);
    _algo.addTask(t);
    _startCondition.notify_one();
  }

  void WorkloadManager::start()
  {
    {
      std::unique_lock<std::mutex> lock(_data_mutex);
      _stop = false;
    }
    _otherThreads.emplace_back(std::async([this]
      {
        runTasks();
      }));
    _otherThreads.emplace_back(std::async([this]
      {
        endTasks();
      }));
  }

  void WorkloadManager::stop()
  {
    {
      std::unique_lock<std::mutex> lock(_data_mutex);
      _stop = true;
    }
    _startCondition.notify_one();
    _endCondition.notify_one();
   for(std::future<void>& th : _otherThreads)
     th.wait();
  }

  void WorkloadManager::runTasks()
  {
    bool threadStop = false;
    while(!threadStop)
    {
      std::unique_lock<std::mutex> lock(_data_mutex);
      _startCondition.wait(lock, [this] {return !_algo.empty() || _stop;});
      RunningInfo taskInfo;
      while(chooseTaskToRun(taskInfo))
      {
        _runningTasks.emplace(taskInfo.id, std::async([this, taskInfo]
          {
            runOneTask(taskInfo);
          }));
      }
      threadStop = _stop && _algo.empty();
    }
  }

  void WorkloadManager::runOneTask(const RunningInfo& taskInfo)
  {
    taskInfo.info.task->run(taskInfo.info.worker);

    {
      std::unique_lock<std::mutex> lock(_data_mutex);
      _finishedTasks.push(taskInfo);
      _endCondition.notify_one();
    }
  }

  void WorkloadManager::endTasks()
  {
    bool threadStop = false;
    while(!threadStop)
    {
      std::unique_lock<std::mutex> lock(_data_mutex);
      _endCondition.wait(lock, [this]
                            {
                              return !_finishedTasks.empty() ||
                              (_stop && _runningTasks.empty() && _algo.empty());
                            });
      while(!_finishedTasks.empty())
      {
        RunningInfo taskInfo = _finishedTasks.front();
        _finishedTasks.pop();
        _runningTasks[taskInfo.id].wait();
        _runningTasks.erase(taskInfo.id);
        _algo.liberate(taskInfo.info);
      }
      threadStop = _stop && _runningTasks.empty() && _algo.empty();
      _startCondition.notify_one();
    }
  }

  bool WorkloadManager::chooseTaskToRun(RunningInfo& taskInfo)
  {
    // We are already under the lock
    taskInfo.id = _nextIndex;
    taskInfo.info = _algo.chooseTask();
    if(taskInfo.info.taskFound)
      _nextIndex ++;
    return taskInfo.info.taskFound;
  }

}
