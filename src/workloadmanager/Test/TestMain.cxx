// Copyright (C) 2020  EDF R&D
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
//

#include <cppunit/TestFixture.h>
#include <stdexcept>

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cppunit/extensions/HelperMacros.h>

#include <string>
#include <sstream>

#include <chrono>
#include <ctime>
#include <thread>

#include "../WorkloadManager.hxx"
#include "../DefaultAlgorithm.hxx"

constexpr bool ACTIVATE_DEBUG_LOG = false;
template<typename... Ts>
void DEBUG_LOG(Ts... args)
{
  if(! ACTIVATE_DEBUG_LOG)
    return;
  if(sizeof...(Ts) == 0)
    return;
  std::ostringstream message;
  // TODO:  C++17 solution: ( (message << args), ...);
  // since initializer lists guarantee sequencing, this can be used to
  // call a function on each element of a pack, in order:
  int dummy[] = { (message << args, 0)...};
  message << std::endl;
  std::cerr << message.str();
}

class MyTask;
class AbstractChecker
{
public:
  virtual void check(const WorkloadManager::RunInfo& c, MyTask* t)=0;
};

template <std::size_t size_R, std::size_t size_T>
class Checker : public AbstractChecker
{
public:
  Checker();
  void check(const WorkloadManager::RunInfo& c, MyTask* t)override;
  void globalCheck();
  void reset();

  WorkloadManager::Resource resources[size_R];
  WorkloadManager::ContainerType types[size_T];
private:
  std::mutex _mutex;
  int _maxContainersForResource[size_R][size_T];
};

class MyTask : public WorkloadManager::Task
{
public:
  const WorkloadManager::ContainerType& type()const override {return *_type;}
  void run(const WorkloadManager::RunInfo& c)override
  {
    _check->check(c, this);

    DEBUG_LOG("Running task ", _id, " on ", c.resource.name, "-", c.type.name,
              "-", c.index);
    std::this_thread::sleep_for(std::chrono::seconds(_sleep));
    DEBUG_LOG("Finish task ", _id);
  }

  void reset(int id,
             const WorkloadManager::ContainerType* type,
             int sleep,
             AbstractChecker * check
            )
  {
    _id = id;
    _type = type;
    _sleep = sleep;
    _check = check;
  }
private:
  int _id = 0;
  const WorkloadManager::ContainerType* _type = nullptr;
  int _sleep = 0;
  AbstractChecker * _check;
};

template <std::size_t size_R, std::size_t size_T>
Checker<size_R, size_T>::Checker()
{
  for(std::size_t i=0; i < size_R; i ++)
  {
    resources[i].id = i;
    std::ostringstream name;
    name << "r" << i;
    resources[i].name = name.str();
  }

  for(std::size_t i=0; i < size_T; i ++)
  {
    types[i].id = i;
    std::ostringstream name;
    name << "t" << i;
    types[i].name = name.str();
  }

  for(std::size_t i=0; i < size_R; i++)
    for(std::size_t j=0; j < size_T; j++)
      _maxContainersForResource[i][j] = 0;
}

template <std::size_t size_R, std::size_t size_T>
void Checker<size_R, size_T>::check(const WorkloadManager::RunInfo& c,
                                    MyTask* t)
{
  std::unique_lock<std::mutex> lock(_mutex);
  int& max = _maxContainersForResource[c.resource.id][c.type.id];
  if( max < c.index)
    max = c.index;
}

template <std::size_t size_R, std::size_t size_T>
void Checker<size_R, size_T>::globalCheck()
{
  for(std::size_t i=0; i < size_R; i++)
  {
    float global_max = 0;
    for(std::size_t j=0; j < size_T; j++)
    {
      int max = _maxContainersForResource[i][j];
      DEBUG_LOG(resources[i].name, ", ", types[j].name,
                " max simultaneous runs:", max+1);
      CPPUNIT_ASSERT( (max+1) * types[j].neededCores <= resources[i].nbCores );
      global_max += types[j].neededCores * float(max+1);
    }
    DEBUG_LOG(resources[i].name, " max cores added for evry type: ", global_max);
    // This assertion may be false if there are more resources than needed.
    CPPUNIT_ASSERT(global_max >= resources[i].nbCores); // cores fully used
  }
}

template <std::size_t size_R, std::size_t size_T>
void Checker<size_R, size_T>::reset()
{
  for(std::size_t i=0; i < size_R; i++)
    for(std::size_t j=0; j < size_T; j++)
      _maxContainersForResource[i][j] = 0;
}

class MyTest: public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(MyTest);
  CPPUNIT_TEST(atest);
  CPPUNIT_TEST(btest);
  CPPUNIT_TEST_SUITE_END();
public:
  void atest();
  void btest(); // ignore resources
};

/**
 * General test with 150 tasks of 3 types:
 *   - 50 tasks which need 4 cores for 2s each
 *   - 50 tasks which need 1 core for 1s each
 *   - 50 tasks which need no core but take 2s each
 * We use 2 resources: 10 cores and 18 cores
 * We verify the global time of execution.
 */
void MyTest::atest()
{
  constexpr std::size_t resourcesNumber = 2;
  constexpr std::size_t typesNumber = 3;
  Checker<resourcesNumber, typesNumber> check;
  check.resources[0].nbCores = 10;
  check.resources[1].nbCores = 18;
  check.types[0].neededCores = 4.0;
  check.types[1].neededCores = 1.0;
  check.types[2].neededCores = 0.0; // tasks to be run with no cost

  for(std::size_t i=0; i < resourcesNumber; i ++)
    DEBUG_LOG(check.resources[i].name, " has ", check.resources[i].nbCores,
              " cores.");
  for(std::size_t i=0; i < typesNumber; i ++)
    DEBUG_LOG(check.types[i].name, " needs ", check.types[i].neededCores,
              " cores.");

  constexpr std::size_t tasksNumber = 150;
  MyTask tasks[tasksNumber];
  for(int type_id = 0; type_id < typesNumber; type_id++)
    for(int j = type_id * tasksNumber / typesNumber;
        j < (type_id + 1) * tasksNumber / typesNumber;
        j++)
        //            id,  ContainerType,       sleep (1|2s)
        tasks[j].reset(j, &check.types[type_id], 2-type_id%2, &check);

  DEBUG_LOG("Number of tasks: ", tasksNumber);
  for(int type_id = 0; type_id < typesNumber; type_id++)
    DEBUG_LOG("Tasks from ", type_id * tasksNumber / typesNumber, 
              " to ", (type_id + 1) * tasksNumber / typesNumber,
              " are of type ", check.types[type_id].name);

  WorkloadManager::DefaultAlgorithm algo;
  WorkloadManager::WorkloadManager wlm(algo);
  for(std::size_t i=0; i < resourcesNumber; i ++)
    wlm.addResource(check.resources[i]);

  // Add 4 core tasks first
  check.reset();
  for(std::size_t i = 0; i < tasksNumber; i++)
    wlm.addTask(&tasks[i]);
  std::chrono::steady_clock::time_point start_time;
  start_time = std::chrono::steady_clock::now();
  wlm.start(); // tasks can be added before start.
  wlm.stop();
  std::chrono::steady_clock::time_point end_time;
  end_time = std::chrono::steady_clock::now();
  std::chrono::seconds duration;
  duration = std::chrono::duration_cast<std::chrono::seconds>
             (end_time - start_time);
  std::chrono::seconds maxExpectedDuration(22);
  CPPUNIT_ASSERT( duration < maxExpectedDuration );
  DEBUG_LOG("Test step duration : ", duration.count(), "s");
  check.globalCheck();

  // Add 1 core tasks first
  check.reset();
  // WARNING: std::size_t is always >= 0
  for(int i = tasksNumber-1; i >= 0; i--)
    wlm.addTask(&tasks[i]);
  start_time = std::chrono::steady_clock::now();
  wlm.start(); // tasks can be added before start.
  wlm.stop();
  end_time = std::chrono::steady_clock::now();
  duration = std::chrono::duration_cast<std::chrono::seconds>
             (end_time - start_time);
  CPPUNIT_ASSERT( duration < maxExpectedDuration );
  DEBUG_LOG("Test step duration : ", duration.count(), "s");
  check.globalCheck();

  // Add 1 core tasks first & start before addTask
  check.reset();
  start_time = std::chrono::steady_clock::now();
  wlm.start();
  for(int i = tasksNumber-1; i >= 0; i--)
    wlm.addTask(&tasks[i]);
  wlm.stop();
  end_time = std::chrono::steady_clock::now();
  duration = std::chrono::duration_cast<std::chrono::seconds>
             (end_time - start_time);
  CPPUNIT_ASSERT( duration < maxExpectedDuration );
  DEBUG_LOG("Test step duration : ", duration.count(), "s");
  check.globalCheck();

}

/**
 * Test the case of tasks which need no resources and can be run whithout
 * waiting.
 */
void MyTest::btest()
{
  Checker<1, 1> check;
  WorkloadManager::ContainerType ctype;
  ctype.ignoreResources = true;
  constexpr std::size_t tasksNumber = 20;
  MyTask tasks[tasksNumber];
  for(std::size_t i = 0; i < tasksNumber; i++)
    tasks[i].reset(i, &ctype, 1, &check);
  WorkloadManager::DefaultAlgorithm algo;
  WorkloadManager::WorkloadManager wlm(algo);
  // no resource needed
  std::chrono::steady_clock::time_point start_time;
  std::chrono::steady_clock::time_point end_time;
  std::chrono::seconds duration;
  start_time = std::chrono::steady_clock::now();
  wlm.start();
  for(std::size_t i = 0; i < tasksNumber; i++)
    wlm.addTask(&tasks[i]);
  wlm.stop();
  end_time = std::chrono::steady_clock::now();
  duration = std::chrono::duration_cast<std::chrono::seconds>
             (end_time - start_time);
  std::chrono::seconds maxExpectedDuration(2);
  CPPUNIT_ASSERT( duration <= maxExpectedDuration);
}

CPPUNIT_TEST_SUITE_REGISTRATION(MyTest);

#include "BasicMainTest.hxx"
