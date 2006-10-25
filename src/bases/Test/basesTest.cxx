
#include "basesTest.hxx"

#include <sstream>

using namespace YACS::BASES;
using namespace YACS;
using namespace std;
    
Mutex BasesTest::_m;
Semaphore BasesTest::_s1;
Semaphore BasesTest::_s2;
ostringstream BasesTest::_glob;
int BasesTest::_var=7;

const int BasesTest::THREAD_NUM=5;

const int BasesTest::LOOPS=4;

int BasesTest::_value=0;

void *BasesTest::th1_1(void *st)
{
  char myName=*((char *) st);
  _s1.wait();
  _glob<< myName << _var;
  _var+=2;
  _s1.post();
  _s2.post();
}

void *BasesTest::th1_2(void *st)
{
  char myName=*((char *) st);
  _s2.wait();
  _glob<< myName << _var;
  _s2.post();
}

void *BasesTest::th1_3(void *st)
{
  char myName=*((char *) st);
  _glob<< myName << _var++;
  _s1.post();
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
      _m.unlock();
      Thread::sleep(100000);
    }
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
    ths[i]=new Thread(th2_1,0);

  for (i=0; i<THREAD_NUM; ++i)
    {
      ths[i]->join();
      delete ths[i];
    }
  delete [] ths;
  CPPUNIT_ASSERT( _value == THREAD_NUM*LOOPS );
}
