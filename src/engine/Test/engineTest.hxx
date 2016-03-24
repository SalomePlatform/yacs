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

#ifndef _ENGINETEST_HXX_
#define _ENGINETEST_HXX_

#include "ComposedNode.hxx"
#include "TypeCode.hxx"

#include <cppunit/extensions/HelperMacros.h>
#include <string>
#include <map>

namespace YACS
{
  class EngineTest: public CppUnit::TestFixture
  {
    CPPUNIT_TEST_SUITE( EngineTest );
    CPPUNIT_TEST(checkGetRuntime );
    CPPUNIT_TEST(checkAny1);
    CPPUNIT_TEST(checkSharedPtr);
    CPPUNIT_TEST(checkAny2);
    CPPUNIT_TEST(checkAny3);
    CPPUNIT_TEST(checkAny4);
    CPPUNIT_TEST(checkAny5);
    CPPUNIT_TEST(checkAny6);
    CPPUNIT_TEST(checkAny7);
    CPPUNIT_TEST(checkAny8);
    CPPUNIT_TEST(checkInGateOutGate );
    CPPUNIT_TEST(checkNodePortNumber );
    CPPUNIT_TEST(checkPortTypeName );
    CPPUNIT_TEST(checkDuplicatePortName );
    CPPUNIT_TEST(checkRemovePort );
    CPPUNIT_TEST(checkAddNodesToBloc );
    CPPUNIT_TEST(checkAddingTwiceSameNodeInSameBloc );
    CPPUNIT_TEST(checkAddingTwiceSameNodeInTwoBlocs );
    CPPUNIT_TEST(checkRecursiveBlocs_NumberOfNodes );
    CPPUNIT_TEST(checkRecursiveBlocs_NumberOfPorts );
    CPPUNIT_TEST(checkPortNameInBloc );
    CPPUNIT_TEST(checkGetNameOfPortNotInBloc );
    CPPUNIT_TEST(checkRemoveNode );
    CPPUNIT_TEST(RecursiveBlocs_multipleRecursion );
    CPPUNIT_TEST(RecursiveBlocs_removeNodes );
    CPPUNIT_TEST(cleanUp);
    CPPUNIT_TEST(checkLogger);
    CPPUNIT_TEST(checkGraphAnalyser0);
    CPPUNIT_TEST(checkGraphAnalyser1);
    CPPUNIT_TEST(checkGraphAnalyser2);
    CPPUNIT_TEST(checkGraphAnalyser3);
    CPPUNIT_TEST(checkGraphAnalyser4);
    CPPUNIT_TEST_SUITE_END();
      
  public:
      
    void setUp();
    void tearDown();
    void cleanUp();

    void checkGetRuntime();
    void checkAny1();
    void checkSharedPtr();
    void checkAny2();
    void checkAny3();
    void checkAny4();
    void checkAny5();
    void checkAny6();
    void checkAny7();
    void checkAny8();
    void checkInGateOutGate();
    void checkNodePortNumber();
    void checkPortTypeName();
    void checkDuplicatePortName();
    void checkRemovePort();
    void checkAddNodesToBloc();
    void checkAddingTwiceSameNodeInSameBloc();
    void checkAddingTwiceSameNodeInTwoBlocs();
    void checkRecursiveBlocs_NumberOfNodes();
    void checkRecursiveBlocs_NumberOfPorts();
    void checkPortNameInBloc();
    void checkGetNameOfPortNotInBloc();
    void checkRemoveNode();
    void RecursiveBlocs_multipleRecursion();
    void RecursiveBlocs_removeNodes();
    void checkLogger();
    void checkGraphAnalyser0();
    void checkGraphAnalyser1();
    void checkGraphAnalyser2();
    void checkGraphAnalyser3();
    void checkGraphAnalyser4();
  protected:
    static std::map<std::string, YACS::ENGINE::Node*> _nodeMap; 
    static std::map<std::string, YACS::ENGINE::ComposedNode*> _compoMap;

    static  YACS::ENGINE::TypeCode *_tc_bool;
    static  YACS::ENGINE::TypeCode *_tc_int;
    static  YACS::ENGINE::TypeCode *_tc_double;
  private:

  };

}

#endif
