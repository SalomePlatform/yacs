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

#ifndef _BASESTEST_HXX_
#define _BASESTEST_HXX_

#include <cppunit/extensions/HelperMacros.h>
#include "Mutex.hxx"
#include "Thread.hxx"
#include "Semaphore.hxx"
#include "DynLibLoader.hxx"

namespace YACS
{
  class BasesTest: public CppUnit::TestFixture
  {
    CPPUNIT_TEST_SUITE( BasesTest );
    CPPUNIT_TEST( test1 );
    CPPUNIT_TEST( test2 );
    CPPUNIT_TEST( test3 );
    CPPUNIT_TEST( testDL );
    CPPUNIT_TEST_SUITE_END();
      
  public:
      
    void setUp();
    void tearDown();
      
    void test1();
    void test2();
    void test3();
    void testDL();

  protected:
      
  private:
    static void *th1_1(void *);
    static void *th1_2(void *);
    static void *th1_3(void *);
    static void *th2_1(void *);

    static void *myFunc(void *);
    static void get_resources(int id, int amount);
    static void free_resources(int id, int amount);
    static int count_resources();
      
    static int _var;
    static std::ostringstream _glob;
    static YACS::BASES::Mutex _m;
    static YACS::BASES::Semaphore _s1;
    static YACS::BASES::Semaphore _s2;
    static YACS::BASES::Condition _cond;
      
    static const int THREAD_NUM;
    static const int LOOPS;
    static const int MAX_RESOURCE;
    static int _value;
    static int _waiting;
    static int _resources;
    static int _ownedResources[];

  };

}

#endif
