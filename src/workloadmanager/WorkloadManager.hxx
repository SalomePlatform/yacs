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
#ifndef WORKLOADMANAGER_H
#define WORKLOADMANAGER_H
#include <mutex>
#include <future>
#include <condition_variable> // notifications
#include <map>
#include <queue>
#include <list>
#include "Task.hxx"
#include "WorkloadAlgorithm.hxx"

namespace WorkloadManager
{
  class WorkloadManager
  {
  public:
    WorkloadManager(WorkloadAlgorithm& algo);
    ~WorkloadManager();
    void addTask(Task* t);
    void addResource(Resource* r);
    void start(); //! start execution
    void stop(); //! stop execution

  private:
    typedef unsigned long TaskId;
    struct RunningInfo
    {
      TaskId id;
      WorkloadAlgorithm::LaunchInfo info;
    };
    std::map<TaskId, std::future<void> > _runningTasks;
    std::queue<RunningInfo> _finishedTasks;
    TaskId _nextIndex;
    std::mutex _data_mutex;
    std::condition_variable _startCondition; // start tasks thread notification
    std::condition_variable _endCondition; // end tasks thread notification
    bool _stop;
    std::list< std::future<void> > _otherThreads;
    WorkloadAlgorithm& _algo;

    void runTasks();
    void endTasks();
    void runOneTask(const RunningInfo& taskInfo);
    // choose a task and block a resource
    bool chooseTaskToRun(RunningInfo& taskInfo);
  };
}
#endif // WORKLOADMANAGER_H
