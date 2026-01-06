// Copyright (C) 2006-2026  CEA, EDF
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
#ifndef _Py2yacsTest_hxx_
#define _Py2yacsTest_hxx_

#include <cppunit/extensions/HelperMacros.h>

class Py2yacsTest: public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(Py2yacsTest);
  CPPUNIT_TEST(t1);
  CPPUNIT_TEST(t2);
  CPPUNIT_TEST(t3);
  CPPUNIT_TEST(t4);
  CPPUNIT_TEST(t5);
  CPPUNIT_TEST(no_multiple_returns);
  CPPUNIT_TEST(unaccepted_statement);
  //CPPUNIT_TEST(unaccepted_statement2);
  CPPUNIT_TEST(unaccepted_return);
  CPPUNIT_TEST(unaccepted_return2);
  CPPUNIT_TEST(syntaxError);
  CPPUNIT_TEST(badFunctionName);
  CPPUNIT_TEST(schemaExec);
  CPPUNIT_TEST(parserErrors);
  CPPUNIT_TEST(globalVerification);
  CPPUNIT_TEST_SUITE_END();
  
  public:
    void setUp();
    void tearDown();
    
    void t1();
    void t2();
    void t3();
    void t4();
    void t5();
    void no_multiple_returns();
    void unaccepted_statement();
    //void unaccepted_statement2();
    void unaccepted_return();
    void unaccepted_return2();
    void syntaxError();
    void badFunctionName();
    void schemaExec();
    void parserErrors();
    void globalVerification();
};

#endif
