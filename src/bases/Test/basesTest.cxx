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

#include "basesTest.hxx"

#include <iostream>
#include <sstream>

using namespace YACS::BASES;
using namespace YACS;
using namespace std;

//#define _DEVDEBUG_
#include "YacsTrace.hxx"
    
Mutex BasesTest::_m;
Semaphore BasesTest::_s1;
Semaphore BasesTest::_s2;
Condition BasesTest::_cond;
ostringstream BasesTest::_glob;
int BasesTest::_var=7;

const int BasesTest::THREAD_NUM=5;

const int BasesTest::LOOPS=4;

const int BasesTest::MAX_RESOURCE=7;

int BasesTest::_value=0;
int BasesTest::_waiting=0;
int BasesTest::_resources=BasesTest::MAX_RESOURCE;
int BasesTest::_ownedResources[THREAD_NUM];

void *BasesTest::th1_1(void *st)
{
  char myName=*((char *) st);
  _s1.wait();
  _glob<< myName << _var;
  _var+=2;
  _s1.post();
  _s2.post();
  return 0;
}

void *BasesTest::th1_2(void *st)
{
  char myName=*((char *) st);
  _s2.wait();
  _glob<< myName << _var;
  _s2.post();
  return 0;
}

void *BasesTest::th1_3(void *st)
{
  char myName=*((char *) st);
  _glob<< myName << _var++;
  _s1.post();
  return 0;
}

void *BasesTest::th2_1(void *)
{
  int i, tmp;
  int rc = 0;      
  for (i=0; i<LOOPS; ++i)
    {
      _m.lock();
      tmp = _value;
      tmp = tmp+1;
      Thread::sleep(1000);
      _value = tmp;
      _m.unLock();
      Thread::sleep(100000);
    }
  return 0;
}

void *BasesTest::myFunc(void *n)
{
  int id = *((int*) n);

  for (int i=3; i>0; i--)
    {
      DEBTRACE("thread " << id << " asks for " << i << " resources");
      get_resources(id, i);
      DEBTRACE("thread " << id << " has got " << i << " resources");
      CPPUNIT_ASSERT(count_resources() == BasesTest::MAX_RESOURCE);
      Thread::sleep(10000);
      CPPUNIT_ASSERT(count_resources() == BasesTest::MAX_RESOURCE);
      DEBTRACE("thread " << id << " frees " << i << " resources");
      free_resources(id, i);
    }
  return 0;
 }

void BasesTest::get_resources(int id, int amount)
{
  _m.lock();
  while (_resources < amount)
    {
      _waiting++;
      _cond.wait(_m); // _m is unlocked during the wait
    }
  _resources -= amount;
  _ownedResources[id] = amount;
  _m.unLock();
}

void BasesTest::free_resources(int id, int amount)
{
  _m.lock();
  _resources += amount;
  _ownedResources[id] = 0;
  if (_waiting > 0)
    {
      _waiting = 0;
      _cond.notify_all();
    }
  _m.unLock();
}

int BasesTest::count_resources()
{
  int resources = 0;
  int totOwned = 0;
  {
    _m.lock();
    resources = _resources;
    for (int i=0; i<THREAD_NUM; i++)
      {
        totOwned += _ownedResources[i];
      }
    _m.unLock();
  }
  int total = resources + totOwned;
  DEBTRACE("resources:: owned by threads: " << totOwned << " remaining: " << resources << " total: " << total);
  return total;
}

void BasesTest::setUp()
{
}

void BasesTest::tearDown()
{
}


void BasesTest::test1()
{
  char t1Name='A'; char t2Name='B'; char t3Name='C';
  Thread t1(th1_1,&t1Name);
  Thread t2(th1_2,&t2Name);
  Thread t3(th1_3,&t3Name);
  t1.join();
  t2.join();
  t3.join();
  CPPUNIT_ASSERT( _glob.str() == "C7A8B10" );
}

void BasesTest::test2()
{
  int i;
  Thread **ths=new Thread *[THREAD_NUM];
  for (i=0; i<THREAD_NUM; ++i)
    {
      ths[i]=new Thread(th2_1,0);
    }
  for (i=0; i<THREAD_NUM; ++i)
    {
      ths[i]->join();
      delete ths[i];
    }
  delete [] ths;
  CPPUNIT_ASSERT( _value == THREAD_NUM*LOOPS );
}

void BasesTest::test3()
{
  int i;
  void *ret;
  Thread **ths=new Thread *[THREAD_NUM];

  int id[THREAD_NUM];
  for (i=0; i<THREAD_NUM; i++)
    {
      id[i] = i;
      _ownedResources[i] = 0;
    }
  
  for (i=0; i<THREAD_NUM; i++)
    {      
      ths[i] = new Thread(myFunc, &id[i]);
      CPPUNIT_ASSERT(count_resources() == BasesTest::MAX_RESOURCE);
    }

  for (i=0; i<THREAD_NUM; i++)
    {
      ths[i]->join();
      delete ths[i];
      DEBTRACE("thread " << id[i] << " is finished");
      CPPUNIT_ASSERT(count_resources() == BasesTest::MAX_RESOURCE);
    }
}

typedef double (*FctPt)(double);

void BasesTest::testDL()
{
  DynLibLoader *loader=new DynLibLoader("libYACSDLTest");
  void *symb=loader->getHandleOnSymbolWithName("myYacsFct");
  FctPt f=(FctPt) symb;
  double res=f(1.7);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( 20.23, res, 1e-13);
  res=f(2.3);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( 37.03, res, 1e-13);
  delete loader;
}
