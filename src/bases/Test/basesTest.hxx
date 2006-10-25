
#ifndef _BASESTEST_HXX_
#define _BASESTEST_HXX_

#include <cppunit/extensions/HelperMacros.h>
#include "Mutex.hxx"
#include "Thread.hxx"
#include "Semaphore.hxx"

namespace YACS
{
  class BasesTest: public CppUnit::TestFixture
  {
    CPPUNIT_TEST_SUITE( BasesTest );
    CPPUNIT_TEST(test1 );
    CPPUNIT_TEST(test2 );
    CPPUNIT_TEST_SUITE_END();
      
  public:
      
    void setUp();
    void tearDown();
      
    void test1();
    void test2();
      
  protected:
      
  private:
    static void *th1_1(void *);
    static void *th1_2(void *);
    static void *th1_3(void *);
    static void *th2_1(void *);
      
    static int _var;
    static std::ostringstream _glob;
    static YACS::BASES::Mutex _m;
    static YACS::BASES::Semaphore _s1;
    static YACS::BASES::Semaphore _s2;
      
    static const int THREAD_NUM;
    static const int LOOPS;
    static int _value;

  };

}

#endif
