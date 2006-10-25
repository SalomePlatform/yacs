
#ifndef _RUNTIMETEST_HXX_
#define _RUNTIMETEST_HXX_

#include <cppunit/extensions/HelperMacros.h>

namespace YACS
{
  class RuntimeTest: public CppUnit::TestFixture
  {
    CPPUNIT_TEST_SUITE( RuntimeTest );
    CPPUNIT_TEST(test1 );
    CPPUNIT_TEST_SUITE_END();
      
  public:
      
    void setUp();
    void tearDown();
      
    void test1();
      
  protected:
      
  private:

  };

}

#endif
