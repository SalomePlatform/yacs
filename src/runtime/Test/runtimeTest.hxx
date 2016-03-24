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

#ifndef _RUNTIMETEST_HXX_
#define _RUNTIMETEST_HXX_

#include "RuntimeSALOME.hxx"
#include "TypeCode.hxx"

#ifdef USE_CPPUNIT
#include <cppunit/extensions/HelperMacros.h>
#else
#define CPPUNIT_TEST_SUITE(x)
#define CPPUNIT_TEST(x)
#define CPPUNIT_TEST_SUITE_END(x)
#define CPPUNIT_ASSERT(x) std::cerr << #x << " : " << (x) << std::endl
#define CPPUNIT_ASSERT_EQUAL(x,y) std::cerr << "Test " << #x << " == " << #y << " : " << ((x) == (y)) << std::endl;
#define CPPUNIT_ASSERT_DOUBLES_EQUAL(x,y,z)
#define CPPUNIT_ASSERT_THROW(x,y) \
	try { x; } \
	catch (y &e) { cerr << e.what() << endl; }
#endif

#include <string>
#include <map>

namespace YACS
{
  class RuntimeTest
#ifdef USE_CPPUNIT
                     : public CppUnit::TestFixture
#endif
  {
    CPPUNIT_TEST_SUITE( RuntimeTest );
    CPPUNIT_TEST(initRuntimeTypeCode );
    CPPUNIT_TEST(createPythonNodes );
    CPPUNIT_TEST(createCORBANodes );
    CPPUNIT_TEST(createBloc );
    CPPUNIT_TEST(createRecursiveBlocs );
    CPPUNIT_TEST(createDataLinks );
    CPPUNIT_TEST(createPythonNodesWithScript );
    CPPUNIT_TEST(createCORBANodesWithMethod );
    CPPUNIT_TEST(createXMLNodes );
    CPPUNIT_TEST(createBloc2 );
    CPPUNIT_TEST(createDataLinksPythonPython );
    CPPUNIT_TEST(createDataLinksPythonCORBA );
    CPPUNIT_TEST(createDataLinksCORBACORBA );
    CPPUNIT_TEST(createDataLinksCORBAPython );
    CPPUNIT_TEST(createDataLinksXML );
    CPPUNIT_TEST(manualInitInputPort );
    CPPUNIT_TEST(manualExecuteNoThread );
    CPPUNIT_TEST(manualGetOutputs );
    CPPUNIT_TEST(createCppNodes );
    CPPUNIT_TEST(convertPorts );
    CPPUNIT_TEST(executeCppNode );
    CPPUNIT_TEST(createGraphWithCppNodes );
    CPPUNIT_TEST(classTeardown );
    CPPUNIT_TEST_SUITE_END();
    
  public:
      
    void setUp();
    void tearDown();
      
    void initRuntimeTypeCode();
    void createPythonNodes();
    void createCORBANodes();
    void createBloc();
    void createRecursiveBlocs();
    void createDataLinks();
    void createPythonNodesWithScript();
    void createCORBANodesWithMethod();
    void createXMLNodes();
    void createBloc2();
    void createDataLinksPythonPython();
    void createDataLinksPythonCORBA();
    void createDataLinksCORBACORBA();
    void createDataLinksCORBAPython();
    void createDataLinksXML();
    void manualInitInputPort();
    void manualExecuteNoThread();
    void manualGetOutputs();
    void createCppNodes();
    void executeCppNode();
    void createGraphWithCppNodes();
    void convertPorts();
     
    void classTeardown();
    
  protected:
    static std::map<std::string, YACS::ENGINE::Node*> _nodeMap;
    static std::map<std::string, YACS::ENGINE::Bloc*> _blocMap;
    static std::list<YACS::ENGINE::TypeCodeObjref *> _ltc;
    static int _inode;
    static int _ibloc;
    static YACS::ENGINE::Runtime *_myRuntime;

    static  YACS::ENGINE::TypeCode *_tc_double;
    static  YACS::ENGINE::TypeCode *_tc_int;
    static  YACS::ENGINE::TypeCode *_tc_string;
    static  YACS::ENGINE::TypeCode *_tc;
    static  YACS::ENGINE::TypeCode *_tc_obj;
    static  YACS::ENGINE::TypeCode *_tc_seqdble;
    static  YACS::ENGINE::TypeCode *_tc_seqstr;
    static  YACS::ENGINE::TypeCode *_tc_seqlong;
    static  YACS::ENGINE::TypeCode *_tc_seqobj;
    static  YACS::ENGINE::TypeCode *_tc_seqseqdble;
    static  YACS::ENGINE::TypeCode *_tc_seqseqobj;
    static  YACS::ENGINE::TypeCode *_tc_C;
    static  YACS::ENGINE::TypeCode *_tc_seqC;
    
    static bool endTests;
      
  private:

  };

}

#endif
