
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
