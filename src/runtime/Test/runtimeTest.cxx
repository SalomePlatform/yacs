// --- include from engine first, to avoid redifinition warning _POSIX_C_SOURCE

#include "Bloc.hxx"
#include "ElementaryNode.hxx"
#include "Loop.hxx"
#include "Switch.hxx"
#include "RuntimeSALOME.hxx"
#include "CppNode.hxx"
#include "PythonNode.hxx"
#include "CORBANode.hxx"
#include "XMLNode.hxx"
#include "TypeConversions.hxx"
#include "CORBACORBAConv.hxx"
#include "PythonCORBAConv.hxx"
#include "CORBAPythonConv.hxx"
#include "CORBAXMLConv.hxx"

#include "runtimeTest.hxx"

#include <omniORB4/CORBA.h>

#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <map>

using namespace YACS::ENGINE;
using namespace YACS;
using namespace std;

#define _DEVDEBUG_
#ifdef _DEVDEBUG_
#define MYDEBTRACE {std::cerr << __FILE__ << " [" << __LINE__ << "] : ";}
#define DEBTRACE(msg) {MYDEBTRACE; std::cerr<<msg<<std::endl<<std::flush;}
#else
#define MYDEBTRACE
#define DEBTRACE(msg)
#endif
    

void RuntimeTest::setUp()
{
}

void RuntimeTest::tearDown()
{
}

void RuntimeTest::test1()
{
  // --- init runtime
  
  RuntimeSALOME::setRuntime();
  
  Runtime *myRuntime = getRuntime();
  
  // --- init typecodes
  
  TypeCode *tc_double    = new TypeCode(Double);
  TypeCode *tc_int       = new TypeCode(Int);
  TypeCode *tc_string    = new TypeCode(String);
  TypeCode *tc           = TypeCode::interface_tc("id","name");
  TypeCode *tc_obj       = TypeCode::interface_tc("eo:Obj","Obj");
  DEBTRACE( " " << tc->id() << " " << tc->name() );
  TypeCode *tc_seqdble   = TypeCode::sequence_tc("eo:seqdouble","seqdouble",tc_double);
  TypeCode *tc_seqstr    = TypeCode::sequence_tc("eo:seqstring","seqstring",tc_string);
  TypeCode *tc_seqlong   = TypeCode::sequence_tc("eo:seqlong","seqlong",tc_int);
  TypeCode *tc_seqobj    = TypeCode::sequence_tc("eo:seqobj","seqobj",tc_obj);
  TypeCode *tc_seqseqdble= TypeCode::sequence_tc("eo:seqseqdble","seqseqdble",tc_seqdble);
  TypeCode *tc_seqseqobj = TypeCode::sequence_tc("eo:seqseqobj","seqseqobj",tc_seqobj);
  std::list<TypeCode_objref *> ltc;
  ltc.push_back((TypeCode_objref *)tc_obj);
  TypeCode *tc_C         = TypeCode::interface_tc("eo:C","C",ltc);
  TypeCode *tc_seqC      = TypeCode::sequence_tc("eo:seqC","seqC",tc_C);
  
  DEBTRACE("int is a int: ");                CPPUNIT_ASSERT( tc_int->is_a(tc_int));
  DEBTRACE("seqdble is not a seqlong: ");    CPPUNIT_ASSERT(!tc_seqdble->is_a(tc_seqlong));
  DEBTRACE("seqdble is a seqdble: ");        CPPUNIT_ASSERT( tc_seqdble->is_a(tc_seqdble));
  DEBTRACE("seqlong is not a seqdble: ");    CPPUNIT_ASSERT(!tc_seqlong->is_a(tc_seqdble));
  DEBTRACE("C is a Obj: ");                  CPPUNIT_ASSERT( tc_C->is_a(tc_obj));
  DEBTRACE("Obj is not a C: " );             CPPUNIT_ASSERT(!tc_obj->is_a(tc_C));
  DEBTRACE("seqC is a seqObj: ");            CPPUNIT_ASSERT( tc_seqC->is_a(tc_seqobj));
  DEBTRACE( "seqObj is not a seqC: ");       CPPUNIT_ASSERT(!tc_seqobj->is_a(tc_seqC));
 
  map<string, Node*> nodeMap;
  int inode = 0;

  // --- Nodes 0 a 4 : Python

  for (int i=0; i<5; i++)
    {
      ostringstream ss;
      ss << "Node_" << inode++;
      string s = ss.str();
      ElementaryNode* node = myRuntime->createNode("Python",s);
      nodeMap[s] = node;
      InputPort  *i1  = node->edAddInputPort("id1",  tc_double);
      InputPort  *i2  = node->edAddInputPort("ii2",  tc_int);
      InputPort  *i3  = node->edAddInputPort("is3",  tc_string);
      InputPort  *i4  = node->edAddInputPort("io4",  tc_obj);
      InputPort  *i5  = node->edAddInputPort("isd5", tc_seqdble);
      InputPort  *i6  = node->edAddInputPort("isl6", tc_seqlong);
      InputPort  *i7  = node->edAddInputPort("iso7", tc_seqobj);
      InputPort  *i8  = node->edAddInputPort("issd8",tc_seqseqdble);
      InputPort  *i9  = node->edAddInputPort("isso9",tc_seqseqobj);
      InputPort  *i10 = node->edAddInputPort("iC10", tc_C);
      InputPort  *i11 = node->edAddInputPort("isC11",tc_seqC);

      OutputPort  *o1  = node->edAddOutputPort("od1",  tc_double);
      OutputPort  *o2  = node->edAddOutputPort("oi2",  tc_int);
      OutputPort  *o3  = node->edAddOutputPort("os3",  tc_string);
      OutputPort  *o4  = node->edAddOutputPort("oo4",  tc_obj);
      OutputPort  *o5  = node->edAddOutputPort("osd5", tc_seqdble);
      OutputPort  *o6  = node->edAddOutputPort("osl6", tc_seqlong);
      OutputPort  *o7  = node->edAddOutputPort("oso7", tc_seqobj);
      OutputPort  *o8  = node->edAddOutputPort("ossd8",tc_seqseqdble);
      OutputPort  *o9  = node->edAddOutputPort("osso9",tc_seqseqobj);
      OutputPort  *o10 = node->edAddOutputPort("oC10", tc_C);
      OutputPort  *o11 = node->edAddOutputPort("osC11",tc_seqC);
    }

  // --- Nodes 5 a 9 : CORBA

  for (int i=5; i<10; i++)
    {
      ostringstream ss;
      ss << "Node_" << inode++;
      string s = ss.str();
      ElementaryNode* node = myRuntime->createNode("CORBA",s);
      nodeMap[s] = node;
      InputPort  *i1  = node->edAddInputPort("id1",  tc_double);
      InputPort  *i2  = node->edAddInputPort("ii2",  tc_int);
      InputPort  *i3  = node->edAddInputPort("is3",  tc_string);
      InputPort  *i4  = node->edAddInputPort("io4",  tc_obj);
      InputPort  *i5  = node->edAddInputPort("isd5", tc_seqdble);
      InputPort  *i6  = node->edAddInputPort("isl6", tc_seqlong);
      InputPort  *i7  = node->edAddInputPort("iso7", tc_seqobj);
      InputPort  *i8  = node->edAddInputPort("issd8",tc_seqseqdble);
      InputPort  *i9  = node->edAddInputPort("isso9",tc_seqseqobj);
      InputPort  *i10 = node->edAddInputPort("iC10", tc_C);
      InputPort  *i11 = node->edAddInputPort("isC11",tc_seqC);

      OutputPort  *o1  = node->edAddOutputPort("od1",  tc_double);
      OutputPort  *o2  = node->edAddOutputPort("oi2",  tc_int);
      OutputPort  *o3  = node->edAddOutputPort("os3",  tc_string);
      OutputPort  *o4  = node->edAddOutputPort("oo4",  tc_obj);
      OutputPort  *o5  = node->edAddOutputPort("osd5", tc_seqdble);
      OutputPort  *o6  = node->edAddOutputPort("osl6", tc_seqlong);
      OutputPort  *o7  = node->edAddOutputPort("oso7", tc_seqobj);
      OutputPort  *o8  = node->edAddOutputPort("ossd8",tc_seqseqdble);
      OutputPort  *o9  = node->edAddOutputPort("osso9",tc_seqseqobj);
      OutputPort  *o10 = node->edAddOutputPort("oC10", tc_C);
      OutputPort  *o11 = node->edAddOutputPort("osC11",tc_seqC);
    }

  DEBTRACE(" --- create bloc, add two nodes, check constituants" );

  map<string, Bloc*> blocMap;
  int ibloc = 0;

  // --- Bloc_0 with Node_0 and Node_1
  {
    ostringstream ss;
    ss << "Bloc_" << ibloc++;
    string s = ss.str();
    Bloc* bloc = new Bloc(s);
    nodeMap[s] = bloc;
    blocMap[s] = bloc;
    bloc->edAddChild(nodeMap["Node_0"]);
    bloc->edAddChild(nodeMap["Node_1"]);
    {
      set<ElementaryNode *> setelem = bloc->getRecursiveConstituents();
      CPPUNIT_ASSERT(setelem.size() == 2);
    }
  }

  // --- Bloc_1 with Node_2
  {
    ostringstream ss;
    ss << "Bloc_" << ibloc++;
    string s = ss.str();
    Bloc* bloc = new Bloc(s);
    nodeMap[s] = bloc;
    blocMap[s] = bloc;
    bloc->edAddChild(nodeMap["Node_2"]);
  }

  DEBTRACE(" --- add a node already used in the bloc does nothing (return false)" );
  CPPUNIT_ASSERT( ! blocMap["Bloc_0"]->edAddChild(nodeMap["Node_1"]));

  DEBTRACE(" --- add a node already used elsewhere raises exception " );
  CPPUNIT_ASSERT_THROW(blocMap["Bloc_1"]->edAddChild(nodeMap["Node_1"]),
 		       YACS::Exception);


  DEBTRACE(" --- recursive blocs, check constituants" );

  // --- Bloc_2 with Bloc_1 and Bloc_2
  {
    ostringstream ss;
    ss << "Bloc_" << ibloc++;
    string s = ss.str();
    Bloc* bloc = new Bloc(s);
    nodeMap[s] = bloc;
    blocMap[s] = bloc;
    bloc->edAddChild(nodeMap["Bloc_0"]);  // 2 elementary nodes 
    bloc->edAddChild(nodeMap["Bloc_1"]);  // 1 elementary node
    bloc->edAddChild(nodeMap["Node_3"]);   // 1 elementary node
  }

  {
    set<ElementaryNode *> setelem = blocMap["Bloc_2"]->getRecursiveConstituents();
    CPPUNIT_ASSERT(setelem.size() == 4);
    for (set<ElementaryNode*>::iterator it=setelem.begin(); it!=setelem.end(); it++)
      {
	DEBTRACE("     elem name = " << (*it)->getName());
      }
  }


  DEBTRACE(" --- create and delete data links" );

  CPPUNIT_ASSERT(blocMap["Bloc_0"]->getNumberOfInputPorts() == 22);
  CPPUNIT_ASSERT_THROW(blocMap["Bloc_0"]->edAddLink(nodeMap["Node_0"]->getOutputPort("od1"),
						    nodeMap["Node_1"]->getInputPort("is3")),
 		       YACS::ENGINE::ConversionException);
  CPPUNIT_ASSERT(blocMap["Bloc_0"]->getNumberOfInputPorts() == 22);

  blocMap["Bloc_0"]->edAddLink(nodeMap["Node_0"]->getOutputPort("oi2"),
			       nodeMap["Node_1"]->getInputPort("ii2"));
  CPPUNIT_ASSERT(blocMap["Bloc_0"]->getNumberOfInputPorts() == 21);

  blocMap["Bloc_0"]->edRemoveLink(nodeMap["Node_0"]->getOutputPort("oi2"),
				  nodeMap["Node_1"]->getInputPort("ii2"));
  CPPUNIT_ASSERT(blocMap["Bloc_0"]->getNumberOfInputPorts() == 22);


  DEBTRACE(" --- create python nodes with scripts" );

  // --- Node 10 : Python

  {
    ostringstream ss;
    ss << "Node_" << inode++;
    string s = ss.str();
    ElementaryNode* node = myRuntime->createNode("Python",s);
    nodeMap[s] = node;
    ((PythonNode*) node)->set_script("a=a+1\n");
    InputPort  *i1  = node->edAddInputPort("a",  tc_double);
    OutputPort *o1  = node->edAddOutputPort("a", tc_double);
  }
 
  // --- Node 11 : Python

  {
    ostringstream ss;
    ss << "Node_" << inode++;
    string s = ss.str();
    ElementaryNode* node = myRuntime->createNode("Python",s);
    nodeMap[s] = node;
    ((PythonNode*) node)->set_script("a=b+1\n"
				     "c=2*a\n"
				     "i=10\n"
				     "s='aaaaaaa'\n"
				     "seqdble=[1.5,2.4]\n"
				     "lngvec=[1,2]\n"
				     "dblevecvec=[[1.5,2.4],[6.7,7.8]]\n"
				     );
    InputPort  *i1  = node->edAddInputPort("b",            tc_double);
    OutputPort *o1  = node->edAddOutputPort("c",           tc_double);
    OutputPort *o2  = node->edAddOutputPort("i",           tc_int);
    OutputPort *o3  = node->edAddOutputPort("s",           tc_string);
    OutputPort *o4  = node->edAddOutputPort("seqdble",     tc_seqdble);
    OutputPort *o5  = node->edAddOutputPort("dblevecvec",  tc_seqseqdble);
    OutputPort *o6  = node->edAddOutputPort("lngvec",      tc_seqlong);
  }

  // --- Node 12 : Python

  {
    ostringstream ss;
    ss << "Node_" << inode++;
    string s = ss.str();
    ElementaryNode* node = myRuntime->createNode("Python",s);
    nodeMap[s] = node;
    ((PythonNode*) node)->set_script("a=dble+1\n"
				     "dble=2*a\n"
				     );
    InputPort  *i1  = node->edAddInputPort("dble",  tc_double);
    OutputPort *o1  = node->edAddOutputPort("dble", tc_double);
  }
 
  // --- Node 13 : Python

  {
    ostringstream ss;
    ss << "Node_" << inode++;
    string s = ss.str();
    ElementaryNode* node = myRuntime->createNode("Python",s);
    nodeMap[s] = node;
    ((PythonNode*) node)->set_script("print 'node 13'\n"
				     "import eo\n"
				     "print ob\n"
				     "o=ob._narrow(eo.Obj)\n"
				     "print o\n"
				     "print o.echoLong(13)\n"
				     "a=dble+1\n"
				     "dble=2*a\n"
				     "print lng\n"
				     "print '++++++++',s,'+++++++++++++'\n"
				     "ob=o\n"
				     "seqstr=['aaa','bbb']\n"
				     "seqobj=[o,o,o,o]\n"
				     "seqseqobj=[[o,o],[o,o]]\n"
				     );

    InputPort  *i1  = node->edAddInputPort("dble",  tc_double);
    InputPort  *i2  = node->edAddInputPort("lng",   tc_int);
    InputPort  *i3  = node->edAddInputPort("s",     tc_string);
    InputPort  *i4  = node->edAddInputPort("ob",    tc_obj);
    OutputPort *o1  = node->edAddOutputPort("dble",       tc_double);
    OutputPort *o2  = node->edAddOutputPort("s",          tc_string);
    OutputPort *o3  = node->edAddOutputPort("lng",        tc_int);
    OutputPort *o4  = node->edAddOutputPort("ob",         tc_obj);
    OutputPort *o5  = node->edAddOutputPort("seqstr",     tc_seqstr);
    OutputPort *o6  = node->edAddOutputPort("seqobj",     tc_seqobj);
    OutputPort *o7  = node->edAddOutputPort("seqseqobj",  tc_seqseqobj);
  }
 
  // --- Node 14 : Python

  {
    ostringstream ss;
    ss << "Node_" << inode++;
    string s = ss.str();
    ElementaryNode* node = myRuntime->createNode("Python",s);
    nodeMap[s] = node;
    ((PythonNode*) node)->set_script("print li\n"
				     "print 'lili=',lili\n"
				     "print 'lstr=',lstr\n"
				     "print 'lobj=',lobj\n"
				     "print 'llobj=',llobj\n"
				     "print 'objc=',objc\n"
				     "li=2*li\n"
				     );
    InputPort  *i1  = node->edAddInputPort("li",    tc_seqdble);
    InputPort  *i2  = node->edAddInputPort("lili",  tc_seqseqdble);
    InputPort  *i3  = node->edAddInputPort("lstr",  tc_seqstr);
    InputPort  *i4  = node->edAddInputPort("lobj",  tc_seqobj);
    InputPort  *i5  = node->edAddInputPort("llobj", tc_seqseqobj);
    InputPort  *i6  = node->edAddInputPort("objc",  tc_C);
    OutputPort *o1  = node->edAddOutputPort("li",   tc_seqdble);
    OutputPort *o2  = node->edAddOutputPort("objc", tc_C);
  }
 
  // --- Node 15 : Python

  {
    ostringstream ss;
    ss << "Node_" << inode++;
    string s = ss.str();
    ElementaryNode* node = myRuntime->createNode("Python",s);
    nodeMap[s] = node;
    ((PythonNode*) node)->set_script("print li\n"
				     "li=[2*e for e in li]\n"
				     "print 'obj=',obj\n"
				     "print li\n"
				     "print lngvec\n"
				     "print dblevec\n"
				     );
    InputPort  *i1  = node->edAddInputPort("li",      tc_seqdble);
    InputPort  *i2  = node->edAddInputPort("obj",     tc_obj);
    InputPort  *i3  = node->edAddInputPort("lngvec",  tc_seqlong);
    InputPort  *i4  = node->edAddInputPort("dblevec", tc_seqdble);
    OutputPort *o1  = node->edAddOutputPort("li",     tc_seqdble);
  }

  DEBTRACE(" --- create CORBA nodes" );

  // --- Node 16 : CORBA

  {
    ostringstream ss;
    ss << "Node_" << inode++;
    string s = ss.str();
    ElementaryNode* node = myRuntime->createNode("CORBA",s);
    nodeMap[s] = node;
    ((CORBANode *) node)->set_ref("corbaname:rir:#test.my_context/Echo.Object");
    ((CORBANode *) node)->set_method("echoDouble");
    InputPort  *i1  = node->edAddInputPort("a",  tc_double);
    OutputPort *o1  = node->edAddOutputPort("c", tc_double);
  }
 
  // --- Node 17 - 18 : CORBA

  for (int i =0; i <2; i++)
    {
      ostringstream ss;
      ss << "Node_" << inode++;
      string s = ss.str();
      ElementaryNode* node = myRuntime->createNode("CORBA",s);
      nodeMap[s] = node;
      ((CORBANode *) node)->set_ref("corbaname:rir:#test.my_context/Echo.Object");
      ((CORBANode *) node)->set_method("echoDouble");
      InputPort  *i1  = node->edAddInputPort("b",  tc_double);
      OutputPort *o1  = node->edAddOutputPort("c", tc_double);
    }

  // --- Node 19 : CORBA

  {
    ostringstream ss;
    ss << "Node_" << inode++;
    string s = ss.str();
    ElementaryNode* node = myRuntime->createNode("CORBA",s);
    nodeMap[s] = node;
    ((CORBANode *) node)->set_ref("corbaname:rir:#test.my_context/Echo.Object");
    ((CORBANode *) node)->set_method("createObj");
    InputPort  *i1  = node->edAddInputPort("long",  tc_int);
    OutputPort *o1  = node->edAddOutputPort("obj",  tc_obj);
  }
 
  // --- Node 20, 21, 22 : CORBA

  for (int i =0; i <3; i++)
    {
      ostringstream ss;
      ss << "Node_" << inode++;
      string s = ss.str();
      ElementaryNode* node = myRuntime->createNode("CORBA",s);
      nodeMap[s] = node;
      ((CORBANode *) node)->set_ref("corbaname:rir:#test.my_context/Echo.Object");
      ((CORBANode *) node)->set_method("echoAll");
      InputPort  *i1  = node->edAddInputPort("double",  tc_double);
      InputPort  *i2  = node->edAddInputPort("long",    tc_int);
      InputPort  *i3  = node->edAddInputPort("str",     tc_string);
      InputPort  *i4  = node->edAddInputPort("obj",     tc_obj);
      OutputPort *o1  = node->edAddOutputPort("double", tc_double);
      OutputPort *o2  = node->edAddOutputPort("long",   tc_int);
      OutputPort *o3  = node->edAddOutputPort("str",    tc_string);
      OutputPort *o4  = node->edAddOutputPort("obj",    tc_obj);
    }
 
  // --- Node 23 a 26 : CORBA

  for (int i =0; i <4; i++)
    {
      ostringstream ss;
      ss << "Node_" << inode++;
      string s = ss.str();
      ElementaryNode* node = myRuntime->createNode("CORBA",s);
      nodeMap[s] = node;
      ((CORBANode *) node)->set_ref("corbaname:rir:#test.my_context/Echo.Object");
      ((CORBANode *) node)->set_method("echoDoubleVec");
      InputPort  *i1  = node->edAddInputPort("dblevec",  tc_seqdble);
      OutputPort *o1  = node->edAddOutputPort("dblevec", tc_seqdble);
    }
 
  // --- Node 27 : CORBA

  {
    ostringstream ss;
    ss << "Node_" << inode++;
    string s = ss.str();
    ElementaryNode* node = myRuntime->createNode("CORBA",s);
    nodeMap[s] = node;
    ((CORBANode *) node)->set_ref("corbaname:rir:#test.my_context/Echo.Object");
    ((CORBANode *) node)->set_method("echoStrVec");
    InputPort  *i1  = node->edAddInputPort("strvec",  tc_seqstr);
    OutputPort *o1  = node->edAddOutputPort("strvec", tc_seqstr);
  }

  // --- Node 28 : CORBA

  {
    ostringstream ss;
    ss << "Node_" << inode++;
    string s = ss.str();
    ElementaryNode* node = myRuntime->createNode("CORBA",s);
    nodeMap[s] = node;
    ((CORBANode *) node)->set_ref("corbaname:rir:#test.my_context/Echo.Object");
    ((CORBANode *) node)->set_method("echoObjectVec");
    InputPort  *i1  = node->edAddInputPort("objvec",  tc_seqobj);
    OutputPort *o1  = node->edAddOutputPort("objvec", tc_seqobj);
  }

  // --- Node 29 : CORBA

  {
    ostringstream ss;
    ss << "Node_" << inode++;
    string s = ss.str();
    ElementaryNode* node = myRuntime->createNode("CORBA",s);
    nodeMap[s] = node;
    ((CORBANode *) node)->set_ref("corbaname:rir:#test.my_context/Echo.Object");
    ((CORBANode *) node)->set_method("echoDoubleVecVec");
    InputPort  *i1  = node->edAddInputPort("dblevecvec",  tc_seqseqdble);
    OutputPort *o1  = node->edAddOutputPort("dblevecvec", tc_seqseqdble);
  }
 
  // --- Node 30 : CORBA

  {
    ostringstream ss;
    ss << "Node_" << inode++;
    string s = ss.str();
    ElementaryNode* node = myRuntime->createNode("CORBA",s);
    nodeMap[s] = node;
    ((CORBANode *) node)->set_ref("corbaname:rir:#test.my_context/Echo.Object");
    ((CORBANode *) node)->set_method("echoObjectVecVec");
    InputPort  *i1  = node->edAddInputPort("objvecvec",  tc_seqseqobj);
    OutputPort *o1  = node->edAddOutputPort("objvecvec", tc_seqseqobj);
  }
 
  // --- Node 31 : CORBA

  {
    ostringstream ss;
    ss << "Node_" << inode++;
    string s = ss.str();
    ElementaryNode* node = myRuntime->createNode("CORBA",s);
    nodeMap[s] = node;
    ((CORBANode *) node)->set_ref("corbaname:rir:#test.my_context/Echo.Object");
    ((CORBANode *) node)->set_method("echoIntVec");
    InputPort  *i1  = node->edAddInputPort("lngvec",  tc_seqlong);
    OutputPort *o1  = node->edAddOutputPort("lngvec", tc_seqlong);
  }

  // --- Node 32 : CORBA

  {
    ostringstream ss;
    ss << "Node_" << inode++;
    string s = ss.str();
    ElementaryNode* node = myRuntime->createNode("CORBA",s);
    nodeMap[s] = node;
    ((CORBANode *) node)->set_ref("corbaname:rir:#test.my_context/Echo.Object");
    ((CORBANode *) node)->set_method("createC");
    OutputPort *o1  = node->edAddOutputPort("objc", tc_C);
  }

  // --- Node 33 : CORBA

  {
    ostringstream ss;
    ss << "Node_" << inode++;
    string s = ss.str();
    ElementaryNode* node = myRuntime->createNode("CORBA",s);
    nodeMap[s] = node;
    ((CORBANode *) node)->set_ref("corbaname:rir:#test.my_context/Echo.Object");
    ((CORBANode *) node)->set_method("echoDouble");
    InputPort  *i1  = node->edAddInputPort("dble",  tc_double);
    OutputPort *o1  = node->edAddOutputPort("dble", tc_double);
  }

  // --- Node 34 : CORBA

  {
    ostringstream ss;
    ss << "Node_" << inode++;
    string s = ss.str();
    ElementaryNode* node = myRuntime->createNode("CORBA",s);
    nodeMap[s] = node;
    ((CORBANode *) node)->set_ref("corbaname:rir:#test.my_context/Echo.Object");
    ((CORBANode *) node)->set_method("echoDoubleVec");
    InputPort  *i1  = node->edAddInputPort("dblevec",  tc_seqdble);
    OutputPort *o1  = node->edAddOutputPort("dblevec", tc_seqdble);
  }

  // --- Node 35 : CORBA

  {
    ostringstream ss;
    ss << "Node_" << inode++;
    string s = ss.str();
    ElementaryNode* node = myRuntime->createNode("CORBA",s);
    nodeMap[s] = node;
    ((CORBANode *) node)->set_ref("corbaname:rir:#test.my_context/Echo.Object");
    ((CORBANode *) node)->set_method("echoDoubleVecVec");
    InputPort  *i1  = node->edAddInputPort("dblevecvec",  tc_seqseqdble);
    OutputPort *o1  = node->edAddOutputPort("dblevecvec", tc_seqseqdble);
  }

  DEBTRACE(" --- create XML nodes" );

  // --- Node 36 : XML

  {
    ostringstream ss;
    ss << "Node_" << inode++;
    string s = ss.str();
    ElementaryNode* node = myRuntime->createNode("XML",s);
    nodeMap[s] = node;
    ((XmlNode *) node)->set_script("./xmlrun.sh");
    InputPort  *i1  = node->edAddInputPort("dble",  tc_double);
    InputPort  *i2  = node->edAddInputPort("dblevec",  tc_seqdble);
    InputPort  *i3  = node->edAddInputPort("dblevecvec",  tc_seqseqdble);
    OutputPort *o1  = node->edAddOutputPort("dble", tc_double);
    OutputPort *o2  = node->edAddOutputPort("dblevec", tc_seqdble);
    OutputPort *o3  = node->edAddOutputPort("dblevecvec", tc_seqseqdble);
  }

  DEBTRACE(" --- create Bloc with all nodes" );
  
  // --- Bloc_3 with Node_10 and following

  {
    ostringstream ss;
    ss << "Bloc_" << ibloc++;
    string s = ss.str();
    Bloc* bloc = new Bloc(s);
    nodeMap[s] = bloc;
    blocMap[s] = bloc;
    for (int i=10; i<inode; i++)
      {
	ostringstream ssn;
	ssn << "Node_" << i;
	string sn = ssn.str();
	cerr << sn << endl;
	bloc->edAddChild(nodeMap[sn]);
      }
  }

  DEBTRACE(" --- create data links, python to python" );


  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_10"]->getOutputPort("a"),
			       nodeMap["Node_11"]->getInputPort("b"));


  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_10"]->getOutputPort("a"),
			       nodeMap["Node_12"]->getInputPort("dble"));

  // Python sequence<double> -> Python sequence<double>
  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_14"]->getOutputPort("li"),
			       nodeMap["Node_15"]->getInputPort("li"));

  // Python obj C -> Python obj Obj : OK bon sens
  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_14"]->getOutputPort("objc"),
			       nodeMap["Node_15"]->getInputPort("obj"));

  // Python Obj -> Python C (dérivé de Obj) : interdit
  CPPUNIT_ASSERT_THROW(blocMap["Bloc_3"]->edAddLink(nodeMap["Node_13"]->getOutputPort("ob"),
						    nodeMap["Node_14"]->getInputPort("objc")),
 		       YACS::ENGINE::ConversionException);

  CPPUNIT_ASSERT_THROW(blocMap["Bloc_3"]->edAddLink(nodeMap["Node_11"]->getOutputPort("s"),
						    nodeMap["Node_12"]->getInputPort("dble")),
 		       YACS::ENGINE::ConversionException);

  CPPUNIT_ASSERT_THROW(blocMap["Bloc_3"]->edAddLink(nodeMap["Node_11"]->getOutputPort("seqdble"),
						    nodeMap["Node_12"]->getInputPort("dble")),
 		       YACS::ENGINE::ConversionException);

  DEBTRACE(" --- create data links, python to CORBA" );

  // double->double
  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_11"]->getOutputPort("c"),
			       nodeMap["Node_16"]->getInputPort("a"));

  // int->int
  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_11"]->getOutputPort("i"),
			       nodeMap["Node_19"]->getInputPort("long"));

  // str->str
  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_11"]->getOutputPort("s"),
			       nodeMap["Node_20"]->getInputPort("str"));

  // seq<long> -> seq<double>
  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_11"]->getOutputPort("lngvec"),
			       nodeMap["Node_23"]->getInputPort("dblevec"));

  // seq<long> -> seq<long>
  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_11"]->getOutputPort("lngvec"),
			       nodeMap["Node_31"]->getInputPort("lngvec"));

  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_13"]->getOutputPort("ob"),
			       nodeMap["Node_22"]->getInputPort("obj"));

  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_11"]->getOutputPort("seqdble"),
			       nodeMap["Node_26"]->getInputPort("dblevec"));

  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_13"]->getOutputPort("seqstr"),
			       nodeMap["Node_27"]->getInputPort("strvec"));

  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_13"]->getOutputPort("seqobj"),
			       nodeMap["Node_28"]->getInputPort("objvec"));

  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_11"]->getOutputPort("dblevecvec"),
			       nodeMap["Node_29"]->getInputPort("dblevecvec"));

  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_13"]->getOutputPort("seqseqobj"),
			       nodeMap["Node_30"]->getInputPort("objvecvec"));

  CPPUNIT_ASSERT_THROW(blocMap["Bloc_3"]->edAddLink(nodeMap["Node_11"]->getOutputPort("seqdble"),
						    nodeMap["Node_27"]->getInputPort("strvec")),
 		       YACS::ENGINE::ConversionException);

  CPPUNIT_ASSERT_THROW(blocMap["Bloc_3"]->edAddLink(nodeMap["Node_13"]->getOutputPort("ob"),
						    nodeMap["Node_22"]->getInputPort("str")),
 		       YACS::ENGINE::ConversionException);

  CPPUNIT_ASSERT_THROW(blocMap["Bloc_3"]->edAddLink(nodeMap["Node_13"]->getOutputPort("dble"),
						    nodeMap["Node_22"]->getInputPort("str")),
 		       YACS::ENGINE::ConversionException);

  CPPUNIT_ASSERT_THROW(blocMap["Bloc_3"]->edAddLink(nodeMap["Node_13"]->getOutputPort("lng"),
						    nodeMap["Node_22"]->getInputPort("str")),
 		       YACS::ENGINE::ConversionException);

  CPPUNIT_ASSERT_THROW(blocMap["Bloc_3"]->edAddLink(nodeMap["Node_13"]->getOutputPort("dble"),
						    nodeMap["Node_22"]->getInputPort("long")),
 		       YACS::ENGINE::ConversionException);

  CPPUNIT_ASSERT_THROW(blocMap["Bloc_3"]->edAddLink(nodeMap["Node_13"]->getOutputPort("dble"),
						    nodeMap["Node_24"]->getInputPort("dblevec")),
 		       YACS::ENGINE::ConversionException);

  CPPUNIT_ASSERT_THROW(blocMap["Bloc_3"]->edAddLink(nodeMap["Node_11"]->getOutputPort("dblevecvec"),
						    nodeMap["Node_24"]->getInputPort("dblevec")),
 		       YACS::ENGINE::ConversionException);

  CPPUNIT_ASSERT_THROW(blocMap["Bloc_3"]->edAddLink(nodeMap["Node_13"]->getOutputPort("seqstr"),
						    nodeMap["Node_24"]->getInputPort("dblevec")),
 		       YACS::ENGINE::ConversionException);

  DEBTRACE(" --- create data links, CORBA to CORBA" );

  // double->double
  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_16"]->getOutputPort("c"),
			       nodeMap["Node_17"]->getInputPort("b"));

  // double->double
  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_16"]->getOutputPort("c"),
			       nodeMap["Node_18"]->getInputPort("b"));

  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_19"]->getOutputPort("obj"),
			       nodeMap["Node_20"]->getInputPort("obj"));

  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_18"]->getOutputPort("c"),
			       nodeMap["Node_20"]->getInputPort("double"));

  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_20"]->getOutputPort("long"),
			       nodeMap["Node_21"]->getInputPort("double"));

  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_20"]->getOutputPort("long"),
			       nodeMap["Node_21"]->getInputPort("long"));

  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_20"]->getOutputPort("str"),
			       nodeMap["Node_21"]->getInputPort("str"));

  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_20"]->getOutputPort("obj"),
			       nodeMap["Node_21"]->getInputPort("obj"));

  // Corba sequence<double> -> Corba sequence<double>
  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_23"]->getOutputPort("dblevec"),
			       nodeMap["Node_24"]->getInputPort("dblevec"));

  // Corba sequence<double> -> Corba sequence<double>
  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_31"]->getOutputPort("lngvec"),
			       nodeMap["Node_25"]->getInputPort("dblevec"));


  DEBTRACE(" --- create data links, CORBA to Python" );


  // Corba C -> Python C (dérivé de Obj):OK
  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_32"]->getOutputPort("objc"),
			       nodeMap["Node_14"]->getInputPort("objc"));

  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_21"]->getOutputPort("double"),
			       nodeMap["Node_13"]->getInputPort("dble"));

  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_21"]->getOutputPort("long"),
			       nodeMap["Node_13"]->getInputPort("lng"));

  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_21"]->getOutputPort("str"),
			       nodeMap["Node_13"]->getInputPort("s"));

  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_21"]->getOutputPort("obj"),
			       nodeMap["Node_13"]->getInputPort("ob"));

  // Corba sequence<double> -> Python sequence<double>
  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_23"]->getOutputPort("dblevec"),
			       nodeMap["Node_14"]->getInputPort("li"));

  // Corba sequence<sequence<double>> -> Python sequence<sequence<double>>
  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_29"]->getOutputPort("dblevecvec"),
			       nodeMap["Node_14"]->getInputPort("lili"));

  // Corba sequence<object> -> Python sequence<object>
  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_28"]->getOutputPort("objvec"),
			       nodeMap["Node_14"]->getInputPort("lobj"));

  // Corba sequence<string> -> Python sequence<string>
  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_27"]->getOutputPort("strvec"),
			       nodeMap["Node_14"]->getInputPort("lstr"));

  // Corba sequence<object> -> Python sequence<object>
  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_30"]->getOutputPort("objvecvec"),
			       nodeMap["Node_14"]->getInputPort("llobj"));

  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_31"]->getOutputPort("lngvec"),
			       nodeMap["Node_15"]->getInputPort("lngvec"));

  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_31"]->getOutputPort("lngvec"),
			       nodeMap["Node_15"]->getInputPort("dblevec"));

  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_20"]->getOutputPort("long"),
			       nodeMap["Node_22"]->getInputPort("double"));

  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_20"]->getOutputPort("long"),
			       nodeMap["Node_22"]->getInputPort("long"));

  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_20"]->getOutputPort("str"),
			       nodeMap["Node_22"]->getInputPort("str"));
 
 DEBTRACE(" --- create data links, xml nodes" );

  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_16"]->getOutputPort("c"),
			       nodeMap["Node_36"]->getInputPort("dble"));

  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_26"]->getOutputPort("dblevec"),
			       nodeMap["Node_36"]->getInputPort("dblevec"));

  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_29"]->getOutputPort("dblevecvec"),
			       nodeMap["Node_36"]->getInputPort("dblevecvec"));

  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_36"]->getOutputPort("dble"),
			       nodeMap["Node_33"]->getInputPort("dble"));

  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_36"]->getOutputPort("dblevec"),
			       nodeMap["Node_34"]->getInputPort("dblevec"));

  blocMap["Bloc_3"]->edAddLink(nodeMap["Node_36"]->getOutputPort("dblevecvec"),
			       nodeMap["Node_35"]->getInputPort("dblevecvec"));

  DEBTRACE(" --- initialization" );

  PyObject *pyDouble = PyFloat_FromDouble(10.51);
  nodeMap["Node_10"]->getInputPort("a")->put(pyDouble);

  CORBA::Any a;
  a <<= (CORBA::Double) 3.14;
  
  CORBA::Any anyLong;
  anyLong <<= (CORBA::Long) 1;

  CORBA::Any anyLong2;
  anyLong2 <<= (CORBA::Long) 1;
  nodeMap["Node_20"]->getInputPort("long")->put(&anyLong2);
  
  CORBA::Any aString;
  aString <<= (const char *)"texte";
  nodeMap["Node_20"]->getInputPort("str")->put(&aString);


  DEBTRACE(" --- execution Python Node_10" );
  ((ElementaryNode*)nodeMap["Node_10"])->execute();

  DEBTRACE(" --- execution Python Node_11" );
  ((ElementaryNode*)nodeMap["Node_11"])->execute();

  DEBTRACE(" --- execution Python Node_12" );
  ((ElementaryNode*)nodeMap["Node_12"])->execute();

  DEBTRACE(" --- execution CORBA Node_16" );
  ((ElementaryNode*)nodeMap["Node_16"])->execute();

  DEBTRACE(" --- execution CORBA Node_17" );
  ((ElementaryNode*)nodeMap["Node_17"])->execute();

  DEBTRACE(" --- execution CORBA Node_18" );
  ((ElementaryNode*)nodeMap["Node_18"])->execute();

  DEBTRACE(" --- execution CORBA Node_19" );
  ((ElementaryNode*)nodeMap["Node_19"])->execute();

  DEBTRACE(" --- execution CORBA Node_20" );
  ((ElementaryNode*)nodeMap["Node_20"])->execute();

  DEBTRACE(" --- execution CORBA Node_21" );
  ((ElementaryNode*)nodeMap["Node_21"])->execute();

  DEBTRACE(" --- execution CORBA Node_29" );
  ((ElementaryNode*)nodeMap["Node_29"])->execute();

  DEBTRACE(" --- execution Python Node_13" );
  ((ElementaryNode*)nodeMap["Node_13"])->execute();

  DEBTRACE(" --- execution CORBA Node_22" );
  ((ElementaryNode*)nodeMap["Node_22"])->execute();

  DEBTRACE(" --- execution CORBA Node_23" );
  ((ElementaryNode*)nodeMap["Node_23"])->execute();

  DEBTRACE(" --- execution CORBA Node_24" );
  ((ElementaryNode*)nodeMap["Node_24"])->execute();

  DEBTRACE(" --- execution CORBA Node_27" );
  ((ElementaryNode*)nodeMap["Node_27"])->execute();

  DEBTRACE(" --- execution CORBA Node_28" );
  ((ElementaryNode*)nodeMap["Node_28"])->execute();

  DEBTRACE(" --- execution CORBA Node_30" );
  ((ElementaryNode*)nodeMap["Node_30"])->execute();

  DEBTRACE(" --- execution CORBA Node_32" );
  ((ElementaryNode*)nodeMap["Node_32"])->execute();

  DEBTRACE(" --- execution CORBA Node_26" );
  ((ElementaryNode*)nodeMap["Node_26"])->execute();

  DEBTRACE(" --- execution CORBA Node_31" );
  ((ElementaryNode*)nodeMap["Node_31"])->execute();

  DEBTRACE(" --- execution Python Node_14" );
  ((ElementaryNode*)nodeMap["Node_14"])->execute();

  DEBTRACE(" --- execution Python Node_15" );
  ((ElementaryNode*)nodeMap["Node_15"])->execute();

  DEBTRACE(" --- execution XML Node_36" );
  ((ElementaryNode*)nodeMap["Node_36"])->execute();

  DEBTRACE(" --- execution CORBA Node_33" );
  ((ElementaryNode*)nodeMap["Node_33"])->execute();

  DEBTRACE(" --- execution CORBA Node_34" );
  ((ElementaryNode*)nodeMap["Node_34"])->execute();

  DEBTRACE(" --- execution CORBA Node_35" );
  ((ElementaryNode*)nodeMap["Node_35"])->execute();

  DEBTRACE(" --- end of execution" );

  CORBA::Double l;
  CORBA::Any* x;

  PyObject *ob=((OutputPyPort*)nodeMap["Node_11"]->getOutputPort("c"))->get();
  DEBTRACE("ob refcnt: " << ob->ob_refcnt);
  PyObject_Print(ob,stdout,Py_PRINT_RAW);

  DEBTRACE("a: " << &a);
  DEBTRACE("a.value(): " << a.value());

  x= ((InputCorbaPort*)nodeMap["Node_16"]->getInputPort("a"))->getAny();
  DEBTRACE("CORBA Node_16 input a any: " << x);
  *x >>= l;
  DEBTRACE("CORBA Node_16 input a double: " << l);

  *((InputCorbaPort*)nodeMap["Node_17"]->getInputPort("b"))->getAny() >>= l;
  DEBTRACE("CORBA Node_17 input b double: " << l);

  *((InputCorbaPort*)nodeMap["Node_18"]->getInputPort("b"))->getAny() >>= l;
  DEBTRACE("CORBA Node_18 input a double: " << l);

  *((OutputCorbaPort*)nodeMap["Node_16"]->getOutputPort("c"))->getAny() >>= l;
  DEBTRACE("CORBA Node_16 output c double: " << l);

  *((OutputCorbaPort*)nodeMap["Node_17"]->getOutputPort("c"))->getAny() >>= l;
  DEBTRACE("CORBA Node_17 output c double: " << l);

  *((OutputCorbaPort*)nodeMap["Node_18"]->getOutputPort("c"))->getAny() >>= l;
  DEBTRACE("CORBA Node_18 output c double: " << l);

  DEBTRACE(" --- fini" );
}
