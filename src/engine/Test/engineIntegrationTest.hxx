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

#ifndef __ENGINEINTEGRATIONTEST_HXX__
#define __ENGINEINTEGRATIONTEST_HXX__

#include <cppunit/extensions/HelperMacros.h>
#include <list>

namespace YACS
{
  namespace ENGINE
  {
    class Task;

    class EngineIntegrationTest : public CppUnit::TestFixture
    {
      CPPUNIT_TEST_SUITE( EngineIntegrationTest );
      CPPUNIT_TEST( testBloc1 );
      CPPUNIT_TEST( testBloc2 );
      CPPUNIT_TEST( testBloc3 );
      CPPUNIT_TEST( testBloc4 );
      CPPUNIT_TEST( testForLoop1 );
      CPPUNIT_TEST( testForLoop2 );
      CPPUNIT_TEST( testForLoop3 );
      CPPUNIT_TEST( testForLoop4 );
      CPPUNIT_TEST( testForLoop5 );
      CPPUNIT_TEST( testWhileLoop1 );
      CPPUNIT_TEST( testWhileLoop2 );
      CPPUNIT_TEST( testSwitch );
      CPPUNIT_TEST( testSwitch2 );
      CPPUNIT_TEST( testSwitch3 );
      CPPUNIT_TEST( testEdInitOnLoops );
      CPPUNIT_TEST( testLinkUpdate1 );
      CPPUNIT_TEST( testLinkUpdate1DS );
      CPPUNIT_TEST( testLinkUpdate2 );
      CPPUNIT_TEST( testLinkUpdate2DS );
      CPPUNIT_TEST( testLinkUpdate3 );
      CPPUNIT_TEST( testLinkUpdate4 );
      CPPUNIT_TEST( testInterLoopDFLink );
      CPPUNIT_TEST( deathTestForLinks );
      CPPUNIT_TEST( testForEachLoop1 );
      CPPUNIT_TEST( testForEachLoop2 );
      CPPUNIT_TEST( testForEachLoop3 );
      CPPUNIT_TEST( testForEachLoop4 );
      CPPUNIT_TEST( testForEachLoop5 );
      CPPUNIT_TEST( testForOptimizerLoop1 );
      CPPUNIT_TEST( testForOptimizerLoop2 );
      CPPUNIT_TEST( testForOptimizerLoop3 );
      CPPUNIT_TEST( testForDeployment1 );
      CPPUNIT_TEST( testForDeployment2 );
      CPPUNIT_TEST( testForCheckConsistency1 );
      CPPUNIT_TEST( testForCheckConsistency2 );
      CPPUNIT_TEST( testForCheckConsistency3 );
      CPPUNIT_TEST( testForCheckConsistency4 );
      CPPUNIT_TEST( testRemoveRuntime );
      CPPUNIT_TEST_SUITE_END();
    public:
      void setUp();
      void tearDown();
      void testBloc1();
      void testBloc2();
      void testBloc3();
      void testBloc4();
      void testSwitch();
      void testSwitch2();
      void testSwitch3();
      void testForLoop1();
      void testForLoop2();
      void testForLoop3();
      void testForLoop4();
      void testForLoop5();
      void testWhileLoop1();
      void testWhileLoop2();
      void testEdInitOnLoops();
      void testLinkUpdate1();
      void testLinkUpdate1DS();
      void testLinkUpdate2();
      void testLinkUpdate2DS();
      void testLinkUpdate3();
      void testLinkUpdate4();
      void testInterLoopDFLink();
      void deathTestForLinks();
      void testForEachLoop1();
      void testForEachLoop2();
      void testForEachLoop3();
      void testForEachLoop4();
      void testForEachLoop5();
      void testForOptimizerLoop1();
      void testForOptimizerLoop2();
      void testForOptimizerLoop3();
      void testForDeployment1();
      void testForDeployment2();
      void testForCheckConsistency1();
      void testForCheckConsistency2();
      void testForCheckConsistency3();
      void testForCheckConsistency4();
      void testRemoveRuntime();
    protected:
      template<class T>
      static void checkListsEqual(const std::list<T>& setToTest1, const std::list<T>& setToTest2);
      template<class T>
      static void checkSetsEqual(const std::set<T>& setToTest1, const std::set<T>& setToTest2);
      template<class T>
      static void checkSetsNotEqual(const std::set<T *>& setToTest1, const std::set<T *>&setToTest2);
    };

    template<class T>
    void EngineIntegrationTest::checkListsEqual(const std::list<T>& setToTest1, const std::list<T>& setToTest2)
    {
      typename std::list<T>::iterator iter1=setToTest1.begin();
      typename std::list<T>::iterator iter2=setToTest2.begin();
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Lists can't be equal : size different", (int)setToTest1.size(), (int)setToTest2.size());
      for(;iter1!=setToTest1.end();iter1++,iter2++)
        CPPUNIT_ASSERT_MESSAGE("Lists can't be equal : value different", *iter1==*iter2);
    }

    template<class T>
    void EngineIntegrationTest::checkSetsEqual(const std::set<T>& setToTest1, const std::set<T>& setToTest2)
    {
      typename std::set<T>::const_iterator iter1=setToTest1.begin();
      typename std::set<T>::const_iterator iter2=setToTest2.begin();
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Sets can't be equal : size different", (int)setToTest1.size(), (int)setToTest2.size());
      for(;iter1!=setToTest1.end();iter1++,iter2++)
        CPPUNIT_ASSERT_MESSAGE("Sets can't be equal : value different", *iter1==*iter2);
    }
    
    template<class T>
    void EngineIntegrationTest::checkSetsNotEqual(const std::set<T *>& setToTest1, const std::set<T *>&setToTest2)
    {
      typename std::set<T *>::const_iterator iter1=setToTest1.begin();
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Sets can't be equal : size different", (int)setToTest1.size(), (int)setToTest2.size());
      for(;iter1!=setToTest1.end();iter1++)
        CPPUNIT_ASSERT_MESSAGE("Elements is in set : not expected.",setToTest2.find(*iter1)==setToTest2.end());
    }
  }
}

#endif
