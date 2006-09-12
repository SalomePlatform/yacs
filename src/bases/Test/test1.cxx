#include "Mutex.hxx"
#include "Thread.hxx"
#include "Semaphore.hxx"
//
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/ui/text/TestRunner.h>
#include <sstream>
//

using namespace YACS::BASES;

class ThreadMechanismTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( ThreadMechanismTest );
  CPPUNIT_TEST( test1 );
  CPPUNIT_TEST( test2 );
  CPPUNIT_TEST_SUITE_END();
public: 
  void setUp();
  void tearDown();
  void test1();
  void test2();
private:
  static void *th1_1(void *);
  static void *th1_2(void *);
  static void *th1_3(void *);
  static void *th2_1(void *);
private:
  static int _var;
  static std::ostringstream _glob;
  static Mutex _m;
  static Semaphore _s1;
  static Semaphore _s2;
  //
  static const int THREAD_NUM;
  static const int LOOPS;
  static int _value;
};

Mutex ThreadMechanismTest::_m;

Semaphore ThreadMechanismTest::_s1;

Semaphore ThreadMechanismTest::_s2;

std::ostringstream ThreadMechanismTest::_glob;

int ThreadMechanismTest::_var=7;

const int ThreadMechanismTest::THREAD_NUM=5;

const int ThreadMechanismTest::LOOPS=4;

int ThreadMechanismTest::_value=0;

void ThreadMechanismTest::setUp()
{
}

void ThreadMechanismTest::tearDown()
{
}

void ThreadMechanismTest::test1()
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

void ThreadMechanismTest::test2()
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

void *ThreadMechanismTest::th1_1(void *st)
{
  char myName=*((char *) st);
  _s1.wait();
  _glob<< myName << _var;
  _var+=2;
  _s1.post();
  _s2.post();
}

void *ThreadMechanismTest::th1_2(void *st)
{
  char myName=*((char *) st);
  _s2.wait();
  _glob<< myName << _var;
  _s2.post();
}

void *ThreadMechanismTest::th1_3(void *st)
{
  char myName=*((char *) st);
  _glob<< myName << _var++;
  _s1.post();
}

void *ThreadMechanismTest::th2_1(void *)
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

int main()
{
  CppUnit::TextUi::TestRunner runner;
  runner.addTest( ThreadMechanismTest::suite() );
  runner.run();
  return 0;
}
