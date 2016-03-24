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

// --- include from engine first, to avoid redifinition warning _POSIX_C_SOURCE
//
#include "TypeConversions.hxx"
#include "Bloc.hxx"
#include "ElementaryNode.hxx"
#include "Loop.hxx"
#include "ForLoop.hxx"
#include "Switch.hxx"
#include "CppNode.hxx"
#include "PythonNode.hxx"
#include "InlineNode.hxx"
#include "ServiceNode.hxx"
#include "XMLNode.hxx"
#include "PythonPorts.hxx"
#include "XMLPorts.hxx"
#include "CORBAPorts.hxx"
#include "CppPorts.hxx"
#include "CppComponent.hxx"
#include "Executor.hxx"

#include "runtimeTest.hxx"

#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <cctype>
#include <cstdlib>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace YACS;
using namespace std;

// --- init typecodes

TypeCode *RuntimeTest::_tc_double    = new TypeCode(Double);
TypeCode *RuntimeTest::_tc_int       = new TypeCode(Int);
TypeCode *RuntimeTest::_tc_string    = new TypeCode(String);
TypeCode *RuntimeTest::_tc           = TypeCode::interfaceTc("id","name");
//TypeCode *RuntimeTest::_tc_obj    = TypeCode::interfaceTc("eo::Obj","IDL:eo/Obj:1.0");
TypeCode *RuntimeTest::_tc_obj       = TypeCode::interfaceTc("IDL:eo/Obj:1.0","Obj");
TypeCode *RuntimeTest::_tc_seqdble   = TypeCode::sequenceTc("eo:seqdouble","seqdouble",RuntimeTest::_tc_double);
TypeCode *RuntimeTest::_tc_seqstr    = TypeCode::sequenceTc("eo:seqstring","seqstring",RuntimeTest::_tc_string);
TypeCode *RuntimeTest::_tc_seqlong   = TypeCode::sequenceTc("eo:seqlong","seqlong",RuntimeTest::_tc_int);
TypeCode *RuntimeTest::_tc_seqobj    = TypeCode::sequenceTc("eo:seqobj","seqobj",RuntimeTest::_tc_obj);
TypeCode *RuntimeTest::_tc_seqseqdble= TypeCode::sequenceTc("eo:seqseqdble","seqseqdble",RuntimeTest::_tc_seqdble);
TypeCode *RuntimeTest::_tc_seqseqobj = TypeCode::sequenceTc("eo:seqseqobj","seqseqobj",RuntimeTest::_tc_seqobj);

list<TypeCodeObjref *> RuntimeTest::_ltc;
TypeCode *RuntimeTest::_tc_C;
TypeCode *RuntimeTest::_tc_seqC;

map<string, Node*> RuntimeTest::_nodeMap;
map<string, Bloc*> RuntimeTest::_blocMap;
int RuntimeTest::_inode = 0;
int RuntimeTest::_ibloc = 0;
Runtime *RuntimeTest::_myRuntime = 0;
bool RuntimeTest::endTests = false;

void RuntimeTest::setUp()
{
  if (_ltc.size() == 0)
    {
      _ltc.push_back((TypeCodeObjref *)_tc_obj);
      _tc_C         = TypeCode::interfaceTc("IDL:eo/C:1.0","C",_ltc);
      _tc_seqC      = TypeCode::sequenceTc("eo:seqC","seqC",_tc_C);
    }
}


#define cRef(x) cerr << "_tc" << #x << " : " << _tc ## x->getRefCnt() << endl

void RuntimeTest::tearDown()
{
}


void RuntimeTest::initRuntimeTypeCode()
{
  // --- init runtime
  std::cerr << std::endl;
  RuntimeSALOME::setRuntime();
  _myRuntime = getRuntime();
  
  // --- init typecodes
  
  DEBTRACE( " " << _tc->id() << " " << _tc->name() );
  DEBTRACE("int is a int: ");                CPPUNIT_ASSERT( _tc_int->isA(_tc_int));
  DEBTRACE("seqdble is not a seqlong: ");    CPPUNIT_ASSERT(!_tc_seqdble->isA(_tc_seqlong));
  DEBTRACE("seqdble is a seqdble: ");        CPPUNIT_ASSERT( _tc_seqdble->isA(_tc_seqdble));
  DEBTRACE("seqlong is not a seqdble: ");    CPPUNIT_ASSERT(!_tc_seqlong->isA(_tc_seqdble));
  DEBTRACE("C is a Obj: ");                  CPPUNIT_ASSERT( _tc_C->isA(_tc_obj));
  DEBTRACE("Obj is not a C: " );             CPPUNIT_ASSERT(!_tc_obj->isA(_tc_C));
  DEBTRACE("seqC is a seqObj: ");            CPPUNIT_ASSERT( _tc_seqC->isA(_tc_seqobj));
  DEBTRACE( "seqObj is not a seqC: ");       CPPUNIT_ASSERT(!_tc_seqobj->isA(_tc_seqC));
}

void RuntimeTest::createPythonNodes()
{
  // --- Nodes 0 a 4 : Python

  for (int i=0; i<5; i++)
    {
      ostringstream ss;
      ss << "Node_" << _inode++;
      string s = ss.str();
      ElementaryNode* node = _myRuntime->createScriptNode("",s);
      _nodeMap[s] = node;
      InputPort  *i1  = node->edAddInputPort("id1",  _tc_double);
      InputPort  *i2  = node->edAddInputPort("ii2",  _tc_int);
      InputPort  *i3  = node->edAddInputPort("is3",  _tc_string);
      InputPort  *i4  = node->edAddInputPort("io4",  _tc_obj);
      InputPort  *i5  = node->edAddInputPort("isd5", _tc_seqdble);
      InputPort  *i6  = node->edAddInputPort("isl6", _tc_seqlong);
      InputPort  *i7  = node->edAddInputPort("iso7", _tc_seqobj);
      InputPort  *i8  = node->edAddInputPort("issd8",_tc_seqseqdble);
      InputPort  *i9  = node->edAddInputPort("isso9",_tc_seqseqobj);
      InputPort  *i10 = node->edAddInputPort("iC10", _tc_C);
      InputPort  *i11 = node->edAddInputPort("isC11",_tc_seqC);

      OutputPort  *o1  = node->edAddOutputPort("od1",  _tc_double);
      OutputPort  *o2  = node->edAddOutputPort("oi2",  _tc_int);
      OutputPort  *o3  = node->edAddOutputPort("os3",  _tc_string);
      OutputPort  *o4  = node->edAddOutputPort("oo4",  _tc_obj);
      OutputPort  *o5  = node->edAddOutputPort("osd5", _tc_seqdble);
      OutputPort  *o6  = node->edAddOutputPort("osl6", _tc_seqlong);
      OutputPort  *o7  = node->edAddOutputPort("oso7", _tc_seqobj);
      OutputPort  *o8  = node->edAddOutputPort("ossd8",_tc_seqseqdble);
      OutputPort  *o9  = node->edAddOutputPort("osso9",_tc_seqseqobj);
      OutputPort  *o10 = node->edAddOutputPort("oC10", _tc_C);
      OutputPort  *o11 = node->edAddOutputPort("osC11",_tc_seqC);
      CPPUNIT_ASSERT_EQUAL(node->getImplementation(),string("Python"));
      CPPUNIT_ASSERT_EQUAL(node->getNumberOfInputPorts(),11);
      CPPUNIT_ASSERT_EQUAL(node->getNumberOfOutputPorts(),11);
      CPPUNIT_ASSERT_EQUAL(node->getInPortName(i10),string("iC10"));
      CPPUNIT_ASSERT_EQUAL(node->getOutPortName(o10),string("oC10"));
      CPPUNIT_ASSERT_EQUAL(node->getInputPort("iC10"),i10);
      CPPUNIT_ASSERT_EQUAL(node->getOutputPort("oC10"),o10);
    }
}


void RuntimeTest::createCORBANodes()
{
  // --- Nodes 5 a 9 : CORBA

  for (int i=5; i<10; i++)
    {
      ostringstream ss;
      ss << "Node_" << _inode++;
      string s = ss.str();
      ElementaryNode* node = _myRuntime->createRefNode("",s);
      _nodeMap[s] = node;
      InputPort  *i1  = node->edAddInputPort("id1",  _tc_double);
      InputPort  *i2  = node->edAddInputPort("ii2",  _tc_int);
      InputPort  *i3  = node->edAddInputPort("is3",  _tc_string);
      InputPort  *i4  = node->edAddInputPort("io4",  _tc_obj);
      InputPort  *i5  = node->edAddInputPort("isd5", _tc_seqdble);
      InputPort  *i6  = node->edAddInputPort("isl6", _tc_seqlong);
      InputPort  *i7  = node->edAddInputPort("iso7", _tc_seqobj);
      InputPort  *i8  = node->edAddInputPort("issd8",_tc_seqseqdble);
      InputPort  *i9  = node->edAddInputPort("isso9",_tc_seqseqobj);
      InputPort  *i10 = node->edAddInputPort("iC10", _tc_C);
      InputPort  *i11 = node->edAddInputPort("isC11",_tc_seqC);

      OutputPort  *o1  = node->edAddOutputPort("od1",  _tc_double);
      OutputPort  *o2  = node->edAddOutputPort("oi2",  _tc_int);
      OutputPort  *o3  = node->edAddOutputPort("os3",  _tc_string);
      OutputPort  *o4  = node->edAddOutputPort("oo4",  _tc_obj);
      OutputPort  *o5  = node->edAddOutputPort("osd5", _tc_seqdble);
      OutputPort  *o6  = node->edAddOutputPort("osl6", _tc_seqlong);
      OutputPort  *o7  = node->edAddOutputPort("oso7", _tc_seqobj);
      OutputPort  *o8  = node->edAddOutputPort("ossd8",_tc_seqseqdble);
      OutputPort  *o9  = node->edAddOutputPort("osso9",_tc_seqseqobj);
      OutputPort  *o10 = node->edAddOutputPort("oC10", _tc_C);
      OutputPort  *o11 = node->edAddOutputPort("osC11",_tc_seqC);
      CPPUNIT_ASSERT_EQUAL(node->getImplementation(),string("CORBA"));
      CPPUNIT_ASSERT_EQUAL(node->getNumberOfInputPorts(),11);
      CPPUNIT_ASSERT_EQUAL(node->getNumberOfOutputPorts(),11);
      CPPUNIT_ASSERT_EQUAL(node->getInPortName(i10),string("iC10"));
      CPPUNIT_ASSERT_EQUAL(node->getOutPortName(o10),string("oC10"));
      CPPUNIT_ASSERT_EQUAL(node->getInputPort("iC10"),i10);
      CPPUNIT_ASSERT_EQUAL(node->getOutputPort("oC10"),o10);
    }
}


void RuntimeTest::createBloc()
{
  DEBTRACE(" --- create bloc, add two nodes, check constituants" );

  // --- Bloc_0 with Node_0 and Node_1
  {
    ostringstream ss;
    ss << "Bloc_" << _ibloc++;
    string s = ss.str();
    Bloc* bloc = new Bloc(s);
    _nodeMap[s] = bloc;
    _blocMap[s] = bloc;
    bloc->edAddChild(_nodeMap["Node_0"]);
    bloc->edAddChild(_nodeMap["Node_1"]);
    {
      list<ElementaryNode *> setelem = bloc->getRecursiveConstituents();
      CPPUNIT_ASSERT(setelem.size() == 2);
    }
  }

  // --- Bloc_1 with Node_2
  {
    ostringstream ss;
    ss << "Bloc_" << _ibloc++;
    string s = ss.str();
    Bloc* bloc = new Bloc(s);
    _nodeMap[s] = bloc;
    _blocMap[s] = bloc;
    bloc->edAddChild(_nodeMap["Node_2"]);
  }

  DEBTRACE(" --- add a node already used in the bloc does nothing (return false)" );
  CPPUNIT_ASSERT( ! _blocMap["Bloc_0"]->edAddChild(_nodeMap["Node_1"]));

  DEBTRACE(" --- add a node already used elsewhere raises exception " );
  CPPUNIT_ASSERT_THROW(_blocMap["Bloc_1"]->edAddChild(_nodeMap["Node_1"]),
                       YACS::Exception);
}


void RuntimeTest::createRecursiveBlocs()
{
  DEBTRACE(" --- recursive blocs, check constituants" );

  // --- Bloc_2 with Bloc_0, Bloc_1 and Node_3
  {
    ostringstream ss;
    ss << "Bloc_" << _ibloc++;
    string s = ss.str();
    Bloc* bloc = new Bloc(s);
    _nodeMap[s] = bloc;
    _blocMap[s] = bloc;
    bloc->edAddChild(_nodeMap["Bloc_0"]);  // 2 elementary nodes 
    bloc->edAddChild(_nodeMap["Bloc_1"]);  // 1 elementary node
    bloc->edAddChild(_nodeMap["Node_3"]);  // 1 elementary node
  }

  {
    list<ElementaryNode *> setelem = _blocMap["Bloc_2"]->getRecursiveConstituents();
    CPPUNIT_ASSERT(setelem.size() == 4);
    for (list<ElementaryNode*>::iterator it=setelem.begin(); it!=setelem.end(); it++)
      {
        DEBTRACE("     elem name = " << (*it)->getName());
      }
  }
}


void RuntimeTest::createDataLinks()
{
  DEBTRACE(" --- create and delete data links" );

  CPPUNIT_ASSERT(_blocMap["Bloc_0"]->getNumberOfInputPorts() == 22);
  CPPUNIT_ASSERT_THROW(_blocMap["Bloc_0"]->edAddLink(_nodeMap["Node_0"]->getOutputPort("od1"),
                                                     _nodeMap["Node_1"]->getInputPort("is3")),
                       YACS::ENGINE::ConversionException);
  CPPUNIT_ASSERT(_blocMap["Bloc_0"]->getNumberOfInputPorts() == 22);

  _blocMap["Bloc_0"]->edAddLink(_nodeMap["Node_0"]->getOutputPort("oi2"),
                                _nodeMap["Node_1"]->getInputPort("ii2"));
  CPPUNIT_ASSERT(_blocMap["Bloc_0"]->getNumberOfInputPorts() == 22);

  _blocMap["Bloc_0"]->edRemoveLink(_nodeMap["Node_0"]->getOutputPort("oi2"),
                                   _nodeMap["Node_1"]->getInputPort("ii2"));
  CPPUNIT_ASSERT(_blocMap["Bloc_0"]->getNumberOfInputPorts() == 22);
}


void RuntimeTest::createPythonNodesWithScript()
{
  DEBTRACE(" --- create python nodes with scripts" );

  // --- Node 10 : Python

  {
    ostringstream ss;
    ss << "Node_" << _inode++;
    string s = ss.str();
    ElementaryNode* node = _myRuntime->createScriptNode("",s);
    _nodeMap[s] = node;
    ((InlineNode*) node)->setScript("a=a+1\n");
    InputPort  *i1  = node->edAddInputPort("a",  _tc_double);
    OutputPort *o1  = node->edAddOutputPort("a", _tc_double);
    CPPUNIT_ASSERT_EQUAL(node->getImplementation(),string("Python"));
    CPPUNIT_ASSERT_EQUAL(node->getNumberOfInputPorts(),1);
    CPPUNIT_ASSERT_EQUAL(node->getNumberOfOutputPorts(),1);
  }
 
  // --- Node 11 : Python

  {
    ostringstream ss;
    ss << "Node_" << _inode++;
    string s = ss.str();
    ElementaryNode* node = _myRuntime->createScriptNode("",s);
    _nodeMap[s] = node;
    ((InlineNode*) node)->setScript("a=b+1\n"
                                     "c=2*a\n"
                                     "i=10\n"
                                     "s='aaaaaaa'\n"
                                     "seqdble=[1.5,2.4]\n"
                                     "lngvec=[1,2]\n"
                                     "dblevecvec=[[1.5,2.4],[6.7,7.8]]\n"
                                     );
    InputPort  *i1  = node->edAddInputPort("b",            _tc_double);
    OutputPort *o1  = node->edAddOutputPort("c",           _tc_double);
    OutputPort *o2  = node->edAddOutputPort("i",           _tc_int);
    OutputPort *o3  = node->edAddOutputPort("s",           _tc_string);
    OutputPort *o4  = node->edAddOutputPort("seqdble",     _tc_seqdble);
    OutputPort *o5  = node->edAddOutputPort("dblevecvec",  _tc_seqseqdble);
    OutputPort *o6  = node->edAddOutputPort("lngvec",      _tc_seqlong);
  }

  // --- Node 12 : Python

  {
    ostringstream ss;
    ss << "Node_" << _inode++;
    string s = ss.str();
    ElementaryNode* node = _myRuntime->createScriptNode("",s);
    _nodeMap[s] = node;
    ((InlineNode*) node)->setScript("a=dble+1\n"
                                     "dble=2*a\n"
                                     );
    InputPort  *i1  = node->edAddInputPort("dble",  _tc_double);
    OutputPort *o1  = node->edAddOutputPort("dble", _tc_double);
  }
 
  // --- Node 13 : Python

  {
    ostringstream ss;
    ss << "Node_" << _inode++;
    string s = ss.str();
    ElementaryNode* node = _myRuntime->createScriptNode("",s);
    _nodeMap[s] = node;
    ((InlineNode*) node)->setScript("print 'node 13'\n"
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

    InputPort  *i1  = node->edAddInputPort("dble",  _tc_double);
    InputPort  *i2  = node->edAddInputPort("lng",   _tc_int);
    InputPort  *i3  = node->edAddInputPort("s",     _tc_string);
    InputPort  *i4  = node->edAddInputPort("ob",    _tc_obj);
    OutputPort *o1  = node->edAddOutputPort("dble",       _tc_double);
    OutputPort *o2  = node->edAddOutputPort("s",          _tc_string);
    OutputPort *o3  = node->edAddOutputPort("lng",        _tc_int);
    OutputPort *o4  = node->edAddOutputPort("ob",         _tc_obj);
    OutputPort *o5  = node->edAddOutputPort("seqstr",     _tc_seqstr);
    OutputPort *o6  = node->edAddOutputPort("seqobj",     _tc_seqobj);
    OutputPort *o7  = node->edAddOutputPort("seqseqobj",  _tc_seqseqobj);
    CPPUNIT_ASSERT_EQUAL(((InputPyPort *)i1)->getPyObj(), Py_None);
    CPPUNIT_ASSERT_EQUAL(((InputPyPort *)i2)->getPyObj(), Py_None);
    CPPUNIT_ASSERT_EQUAL(((InputPyPort *)i3)->getPyObj(), Py_None);
    CPPUNIT_ASSERT_EQUAL(((InputPyPort *)i4)->getPyObj(), Py_None);
    CPPUNIT_ASSERT_EQUAL(((OutputPyPort *)o1)->get(), Py_None);
    CPPUNIT_ASSERT_EQUAL(((OutputPyPort *)o2)->get(), Py_None);
    CPPUNIT_ASSERT_EQUAL(((OutputPyPort *)o3)->get(), Py_None);
    CPPUNIT_ASSERT_EQUAL(((OutputPyPort *)o4)->get(), Py_None);
    CPPUNIT_ASSERT_EQUAL(((OutputPyPort *)o5)->get(), Py_None);
    CPPUNIT_ASSERT_EQUAL(((OutputPyPort *)o6)->get(), Py_None);
    CPPUNIT_ASSERT_EQUAL(((OutputPyPort *)o7)->get(), Py_None);
  }
 
  // --- Node 14 : Python

  {
    ostringstream ss;
    ss << "Node_" << _inode++;
    string s = ss.str();
    ElementaryNode* node = _myRuntime->createScriptNode("",s);
    _nodeMap[s] = node;
    ((InlineNode*) node)->setScript("print li\n"
                                     "print 'lili=',lili\n"
                                     "print 'lstr=',lstr\n"
                                     "print 'lobj=',lobj\n"
                                     "print 'llobj=',llobj\n"
                                     "print 'objc=',objc\n"
                                     "li=2*li\n"
                                     );
    InputPort  *i1  = node->edAddInputPort("li",    _tc_seqdble);
    InputPort  *i2  = node->edAddInputPort("lili",  _tc_seqseqdble);
    InputPort  *i3  = node->edAddInputPort("lstr",  _tc_seqstr);
    InputPort  *i4  = node->edAddInputPort("lobj",  _tc_seqobj);
    InputPort  *i5  = node->edAddInputPort("llobj", _tc_seqseqobj);
    InputPort  *i6  = node->edAddInputPort("objc",  _tc_C);
    OutputPort *o1  = node->edAddOutputPort("li",   _tc_seqdble);
    OutputPort *o2  = node->edAddOutputPort("objc", _tc_C);
    CPPUNIT_ASSERT_EQUAL(((InputPyPort *)i1)->getPyObj(), Py_None);
    CPPUNIT_ASSERT_EQUAL(((InputPyPort *)i2)->getPyObj(), Py_None);
    CPPUNIT_ASSERT_EQUAL(((InputPyPort *)i3)->getPyObj(), Py_None);
    CPPUNIT_ASSERT_EQUAL(((InputPyPort *)i4)->getPyObj(), Py_None);
    CPPUNIT_ASSERT_EQUAL(((InputPyPort *)i5)->getPyObj(), Py_None);
    CPPUNIT_ASSERT_EQUAL(((InputPyPort *)i6)->getPyObj(), Py_None);
    CPPUNIT_ASSERT_EQUAL(((OutputPyPort *)o1)->get(), Py_None);
    CPPUNIT_ASSERT_EQUAL(((OutputPyPort *)o2)->get(), Py_None);
  }
 
  // --- Node 15 : Python

  {
    ostringstream ss;
    ss << "Node_" << _inode++;
    string s = ss.str();
    ElementaryNode* node = _myRuntime->createScriptNode("",s);
    _nodeMap[s] = node;
    ((InlineNode*) node)->setScript("print li\n"
                                     "li=[2*e for e in li]\n"
                                     "print 'obj=',obj\n"
                                     "print li\n"
                                     "print lngvec\n"
                                     "print dblevec\n"
                                     );
    InputPort  *i1  = node->edAddInputPort("li",      _tc_seqdble);
    InputPort  *i2  = node->edAddInputPort("obj",     _tc_obj);
    InputPort  *i3  = node->edAddInputPort("lngvec",  _tc_seqlong);
    InputPort  *i4  = node->edAddInputPort("dblevec", _tc_seqdble);
    OutputPort *o1  = node->edAddOutputPort("li",     _tc_seqdble);
    CPPUNIT_ASSERT_EQUAL(node->getImplementation(),string("Python"));
    CPPUNIT_ASSERT_EQUAL(node->getNumberOfInputPorts(),4);
    CPPUNIT_ASSERT_EQUAL(node->getNumberOfOutputPorts(),1);
  }
}

void RuntimeTest::createCORBANodesWithMethod()
{
  DEBTRACE(" --- create CORBA nodes" );

  // --- Node 16 : CORBA

  {
    ostringstream ss;
    ss << "Node_" << _inode++;
    string s = ss.str();
    ElementaryNode* node = _myRuntime->createRefNode("",s);
    _nodeMap[s] = node;
    ((ServiceNode *) node)->setRef("corbaname:rir:#test.my_context/Echo.Object");
    ((ServiceNode *) node)->setMethod("echoDouble");
    InputPort  *i1  = node->edAddInputPort("a",  _tc_double);
    OutputPort *o1  = node->edAddOutputPort("c", _tc_double);
    CPPUNIT_ASSERT_EQUAL(node->getImplementation(),string("CORBA"));
    CPPUNIT_ASSERT_EQUAL(node->getNumberOfInputPorts(),1);
    CPPUNIT_ASSERT_EQUAL(node->getNumberOfOutputPorts(),1);
    CPPUNIT_ASSERT_EQUAL(((InputCorbaPort *)i1)->getAny()->type()->kind(),CORBA::tk_null);
    CPPUNIT_ASSERT_EQUAL(((OutputCorbaPort *)o1)->getAny()->type()->kind(),CORBA::tk_null);
  }
 
  // --- Node 17 - 18 : CORBA

  for (int i =0; i <2; i++)
    {
      ostringstream ss;
      ss << "Node_" << _inode++;
      string s = ss.str();
      ElementaryNode* node = _myRuntime->createRefNode("",s);
      _nodeMap[s] = node;
      ((ServiceNode *) node)->setRef("corbaname:rir:#test.my_context/Echo.Object");
      ((ServiceNode *) node)->setMethod("echoDouble");
      InputPort  *i1  = node->edAddInputPort("b",  _tc_double);
      OutputPort *o1  = node->edAddOutputPort("c", _tc_double);
    }

  // --- Node 19 : CORBA

  {
    ostringstream ss;
    ss << "Node_" << _inode++;
    string s = ss.str();
    ElementaryNode* node = _myRuntime->createRefNode("",s);
    _nodeMap[s] = node;
    ((ServiceNode *) node)->setRef("corbaname:rir:#test.my_context/Echo.Object");
    ((ServiceNode *) node)->setMethod("createObj");
    InputPort  *i1  = node->edAddInputPort("long",  _tc_int);
    OutputPort *o1  = node->edAddOutputPort("obj",  _tc_obj);
    CPPUNIT_ASSERT_EQUAL(((InputCorbaPort *)i1)->getAny()->type()->kind(),CORBA::tk_null);
    CPPUNIT_ASSERT_EQUAL(((OutputCorbaPort *)o1)->getAny()->type()->kind(),CORBA::tk_null);
  }
 
  // --- Node 20, 21, 22 : CORBA

  for (int i =0; i <3; i++)
    {
      ostringstream ss;
      ss << "Node_" << _inode++;
      string s = ss.str();
      ElementaryNode* node = _myRuntime->createRefNode("",s);
      _nodeMap[s] = node;
      ((ServiceNode *) node)->setRef("corbaname:rir:#test.my_context/Echo.Object");
      ((ServiceNode *) node)->setMethod("echoAll");
      InputPort  *i1  = node->edAddInputPort("double",  _tc_double);
      InputPort  *i2  = node->edAddInputPort("long",    _tc_int);
      InputPort  *i3  = node->edAddInputPort("str",     _tc_string);
      InputPort  *i4  = node->edAddInputPort("obj",     _tc_obj);
      OutputPort *o1  = node->edAddOutputPort("double", _tc_double);
      OutputPort *o2  = node->edAddOutputPort("long",   _tc_int);
      OutputPort *o3  = node->edAddOutputPort("str",    _tc_string);
      OutputPort *o4  = node->edAddOutputPort("obj",    _tc_obj);
      CPPUNIT_ASSERT_EQUAL(((InputCorbaPort *)i3)->getAny()->type()->kind(),CORBA::tk_null);
      CPPUNIT_ASSERT_EQUAL(((InputCorbaPort *)i4)->getAny()->type()->kind(),CORBA::tk_null);
      CPPUNIT_ASSERT_EQUAL(((OutputCorbaPort *)o3)->getAny()->type()->kind(),CORBA::tk_null);
      CPPUNIT_ASSERT_EQUAL(((OutputCorbaPort *)o4)->getAny()->type()->kind(),CORBA::tk_null);
    }
 
  // --- Node 23 a 26 : CORBA

  for (int i =0; i <4; i++)
    {
      ostringstream ss;
      ss << "Node_" << _inode++;
      string s = ss.str();
      ElementaryNode* node = _myRuntime->createRefNode("",s);
      _nodeMap[s] = node;
      ((ServiceNode *) node)->setRef("corbaname:rir:#test.my_context/Echo.Object");
      ((ServiceNode *) node)->setMethod("echoDoubleVec");
      InputPort  *i1  = node->edAddInputPort("dblevec",  _tc_seqdble);
      OutputPort *o1  = node->edAddOutputPort("dblevec", _tc_seqdble);
      CPPUNIT_ASSERT_EQUAL(((InputCorbaPort *)i1)->getAny()->type()->kind(),CORBA::tk_null);
      CPPUNIT_ASSERT_EQUAL(((OutputCorbaPort *)o1)->getAny()->type()->kind(),CORBA::tk_null);
    }
 
  // --- Node 27 : CORBA

  {
    ostringstream ss;
    ss << "Node_" << _inode++;
    string s = ss.str();
    ElementaryNode* node = _myRuntime->createRefNode("",s);
    _nodeMap[s] = node;
    ((ServiceNode *) node)->setRef("corbaname:rir:#test.my_context/Echo.Object");
    ((ServiceNode *) node)->setMethod("echoStrVec");
    InputPort  *i1  = node->edAddInputPort("strvec",  _tc_seqstr);
    OutputPort *o1  = node->edAddOutputPort("strvec", _tc_seqstr);
    CPPUNIT_ASSERT_EQUAL(((InputCorbaPort *)i1)->getAny()->type()->kind(),CORBA::tk_null);
    CPPUNIT_ASSERT_EQUAL(((OutputCorbaPort *)o1)->getAny()->type()->kind(),CORBA::tk_null);
  }

  // --- Node 28 : CORBA

  {
    ostringstream ss;
    ss << "Node_" << _inode++;
    string s = ss.str();
    ElementaryNode* node = _myRuntime->createRefNode("",s);
    _nodeMap[s] = node;
    ((ServiceNode *) node)->setRef("corbaname:rir:#test.my_context/Echo.Object");
    ((ServiceNode *) node)->setMethod("echoObjectVec");
    InputPort  *i1  = node->edAddInputPort("objvec",  _tc_seqobj);
    OutputPort *o1  = node->edAddOutputPort("objvec", _tc_seqobj);
    CPPUNIT_ASSERT_EQUAL(((InputCorbaPort *)i1)->getAny()->type()->kind(),CORBA::tk_null);
    CPPUNIT_ASSERT_EQUAL(((OutputCorbaPort *)o1)->getAny()->type()->kind(),CORBA::tk_null);
  }

  // --- Node 29 : CORBA

  {
    ostringstream ss;
    ss << "Node_" << _inode++;
    string s = ss.str();
    ElementaryNode* node = _myRuntime->createRefNode("",s);
    _nodeMap[s] = node;
    ((ServiceNode *) node)->setRef("corbaname:rir:#test.my_context/Echo.Object");
    ((ServiceNode *) node)->setMethod("echoDoubleVecVec");
    InputPort  *i1  = node->edAddInputPort("dblevecvec",  _tc_seqseqdble);
    OutputPort *o1  = node->edAddOutputPort("dblevecvec", _tc_seqseqdble);
    CPPUNIT_ASSERT_EQUAL(((InputCorbaPort *)i1)->getAny()->type()->kind(),CORBA::tk_null);
    CPPUNIT_ASSERT_EQUAL(((OutputCorbaPort *)o1)->getAny()->type()->kind(),CORBA::tk_null);
  }
 
  // --- Node 30 : CORBA

  {
    ostringstream ss;
    ss << "Node_" << _inode++;
    string s = ss.str();
    ElementaryNode* node = _myRuntime->createRefNode("",s);
    _nodeMap[s] = node;
    ((ServiceNode *) node)->setRef("corbaname:rir:#test.my_context/Echo.Object");
    ((ServiceNode *) node)->setMethod("echoObjectVecVec");
    InputPort  *i1  = node->edAddInputPort("objvecvec",  _tc_seqseqobj);
    OutputPort *o1  = node->edAddOutputPort("objvecvec", _tc_seqseqobj);
    CPPUNIT_ASSERT_EQUAL(((InputCorbaPort *)i1)->getAny()->type()->kind(),CORBA::tk_null);
    CPPUNIT_ASSERT_EQUAL(((OutputCorbaPort *)o1)->getAny()->type()->kind(),CORBA::tk_null);
  }
 
  // --- Node 31 : CORBA

  {
    ostringstream ss;
    ss << "Node_" << _inode++;
    string s = ss.str();
    ElementaryNode* node = _myRuntime->createRefNode("",s);
    _nodeMap[s] = node;
    ((ServiceNode *) node)->setRef("corbaname:rir:#test.my_context/Echo.Object");
    ((ServiceNode *) node)->setMethod("echoIntVec");
    InputPort  *i1  = node->edAddInputPort("lngvec",  _tc_seqlong);
    OutputPort *o1  = node->edAddOutputPort("lngvec", _tc_seqlong);
    CPPUNIT_ASSERT_EQUAL(((InputCorbaPort *)i1)->getAny()->type()->kind(),CORBA::tk_null);
    CPPUNIT_ASSERT_EQUAL(((OutputCorbaPort *)o1)->getAny()->type()->kind(),CORBA::tk_null);
  }

  // --- Node 32 : CORBA

  {
    ostringstream ss;
    ss << "Node_" << _inode++;
    string s = ss.str();
    ElementaryNode* node = _myRuntime->createRefNode("",s);
    _nodeMap[s] = node;
    ((ServiceNode *) node)->setRef("corbaname:rir:#test.my_context/Echo.Object");
    ((ServiceNode *) node)->setMethod("createC");
    OutputPort *o1  = node->edAddOutputPort("objc", _tc_C);
    CPPUNIT_ASSERT_EQUAL(((OutputCorbaPort *)o1)->getAny()->type()->kind(),CORBA::tk_null);
  }

  // --- Node 33 : CORBA

  {
    ostringstream ss;
    ss << "Node_" << _inode++;
    string s = ss.str();
    ElementaryNode* node = _myRuntime->createRefNode("",s);
    _nodeMap[s] = node;
    ((ServiceNode *) node)->setRef("corbaname:rir:#test.my_context/Echo.Object");
    ((ServiceNode *) node)->setMethod("echoDouble");
    InputPort  *i1  = node->edAddInputPort("dble",  _tc_double);
    OutputPort *o1  = node->edAddOutputPort("dble", _tc_double);
  }

  // --- Node 34 : CORBA

  {
    ostringstream ss;
    ss << "Node_" << _inode++;
    string s = ss.str();
    ElementaryNode* node = _myRuntime->createRefNode("",s);
    _nodeMap[s] = node;
    ((ServiceNode *) node)->setRef("corbaname:rir:#test.my_context/Echo.Object");
    ((ServiceNode *) node)->setMethod("echoDoubleVec");
    InputPort  *i1  = node->edAddInputPort("dblevec",  _tc_seqdble);
    OutputPort *o1  = node->edAddOutputPort("dblevec", _tc_seqdble);
  }

  // --- Node 35 : CORBA

  {
    ostringstream ss;
    ss << "Node_" << _inode++;
    string s = ss.str();
    ElementaryNode* node = _myRuntime->createRefNode("",s);
    _nodeMap[s] = node;
    ((ServiceNode *) node)->setRef("corbaname:rir:#test.my_context/Echo.Object");
    ((ServiceNode *) node)->setMethod("echoDoubleVecVec");
    InputPort  *i1  = node->edAddInputPort("dblevecvec",  _tc_seqseqdble);
    OutputPort *o1  = node->edAddOutputPort("dblevecvec", _tc_seqseqdble);
  }
}


void RuntimeTest::createXMLNodes()
{
  DEBTRACE(" --- create XML nodes" );

  // --- Node 36 : XML

  {
    ostringstream ss;
    ss << "Node_" << _inode++;
    string s = ss.str();
    ElementaryNode* node = _myRuntime->createRefNode("xmlsh",s);
    _nodeMap[s] = node;
    ((ServiceNode *) node)->setRef("./xmlrun.sh");
    ((ServiceNode *) node)->setMethod("echo");
    InputPort  *i1  = node->edAddInputPort("dble",  _tc_double);
    InputPort  *i2  = node->edAddInputPort("dblevec",  _tc_seqdble);
    InputPort  *i3  = node->edAddInputPort("dblevecvec",  _tc_seqseqdble);
    OutputPort *o1  = node->edAddOutputPort("dble", _tc_double);
    OutputPort *o2  = node->edAddOutputPort("dblevec", _tc_seqdble);
    OutputPort *o3  = node->edAddOutputPort("dblevecvec", _tc_seqseqdble);
    CPPUNIT_ASSERT_EQUAL(node->getImplementation(),string("XML"));
    CPPUNIT_ASSERT_EQUAL(node->getNumberOfInputPorts(),3);
    CPPUNIT_ASSERT_EQUAL(node->getNumberOfOutputPorts(),3);
  }
}


void RuntimeTest::createBloc2()
{
  DEBTRACE(" --- create Bloc with all nodes" );
  
  // --- Bloc_3 with Node_10 and following

  {
    ostringstream ss;
    ss << "Bloc_" << _ibloc++;
    string s = ss.str();
    Bloc* bloc = new Bloc(s);
    _nodeMap[s] = bloc;
    _blocMap[s] = bloc;
    for (int i=10; i<_inode; i++)
      {
        ostringstream ssn;
        ssn << "Node_" << i;
        string sn = ssn.str();
        cerr << sn << endl;
        bloc->edAddChild(_nodeMap[sn]);
      }
    {
      list<ElementaryNode *> setelem = bloc->getRecursiveConstituents();
      CPPUNIT_ASSERT(setelem.size() == _inode - 10);
    }
  }
  set<InputPort *> unitialized =  _blocMap["Bloc_3"]->edGetSetOfUnitializedInputPort();
  DEBTRACE(unitialized.size());
  CPPUNIT_ASSERT(! _blocMap["Bloc_3"]->edAreAllInputPortInitialized() );
}


void RuntimeTest::createDataLinksPythonPython()
{
  DEBTRACE(" --- create data links, python to python" );


  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_10"]->getOutputPort("a"),
                                _nodeMap["Node_11"]->getInputPort("b"));


  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_10"]->getOutputPort("a"),
                                _nodeMap["Node_12"]->getInputPort("dble"));

  // --- Python sequence<double> -> Python sequence<double>
  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_14"]->getOutputPort("li"),
                                _nodeMap["Node_15"]->getInputPort("li"));

  // --- Python obj C (derived from Obj) -> Python obj Obj : accepted
  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_14"]->getOutputPort("objc"),
                                _nodeMap["Node_15"]->getInputPort("obj"));

  // --- Python Obj -> Python C (derived from Obj) : forbidden
  CPPUNIT_ASSERT_THROW(_blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_13"]->getOutputPort("ob"),
                                                     _nodeMap["Node_14"]->getInputPort("objc")),
                       YACS::ENGINE::ConversionException);

  CPPUNIT_ASSERT_THROW(_blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_11"]->getOutputPort("s"),
                                                     _nodeMap["Node_12"]->getInputPort("dble")),
                       YACS::ENGINE::ConversionException);

  CPPUNIT_ASSERT_THROW(_blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_11"]->getOutputPort("seqdble"),
                                                     _nodeMap["Node_12"]->getInputPort("dble")),
                       YACS::ENGINE::ConversionException);
}


void RuntimeTest::createDataLinksPythonCORBA()
{
  DEBTRACE(" --- create data links, python to CORBA" );

  // --- double->double
  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_11"]->getOutputPort("c"),
                                _nodeMap["Node_16"]->getInputPort("a"));

  // --- int->int
  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_11"]->getOutputPort("i"),
                                _nodeMap["Node_19"]->getInputPort("long"));

  // --- str->str
  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_11"]->getOutputPort("s"),
                                _nodeMap["Node_20"]->getInputPort("str"));
  cerr << "##### 3" << endl;

  // --- seq<long> -> seq<double>
  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_11"]->getOutputPort("lngvec"),
                                _nodeMap["Node_23"]->getInputPort("dblevec"));

  // --- seq<long> -> seq<long>
  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_11"]->getOutputPort("lngvec"),
                                _nodeMap["Node_31"]->getInputPort("lngvec"));

  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_13"]->getOutputPort("ob"),
                                _nodeMap["Node_22"]->getInputPort("obj"));

  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_11"]->getOutputPort("seqdble"),
                                _nodeMap["Node_26"]->getInputPort("dblevec"));

  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_13"]->getOutputPort("seqstr"),
                                _nodeMap["Node_27"]->getInputPort("strvec"));

  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_13"]->getOutputPort("seqobj"),
                                _nodeMap["Node_28"]->getInputPort("objvec"));

  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_11"]->getOutputPort("dblevecvec"),
                                _nodeMap["Node_29"]->getInputPort("dblevecvec"));

  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_13"]->getOutputPort("seqseqobj"),
                                _nodeMap["Node_30"]->getInputPort("objvecvec"));

  CPPUNIT_ASSERT_THROW(_blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_11"]->getOutputPort("seqdble"),
                                                     _nodeMap["Node_27"]->getInputPort("strvec")),
                       YACS::ENGINE::ConversionException);

  CPPUNIT_ASSERT_THROW(_blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_13"]->getOutputPort("ob"),
                                                     _nodeMap["Node_22"]->getInputPort("str")),
                       YACS::ENGINE::ConversionException);

  CPPUNIT_ASSERT_THROW(_blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_13"]->getOutputPort("dble"),
                                                     _nodeMap["Node_22"]->getInputPort("str")),
                       YACS::ENGINE::ConversionException);

  CPPUNIT_ASSERT_THROW(_blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_13"]->getOutputPort("lng"),
                                                     _nodeMap["Node_22"]->getInputPort("str")),
                       YACS::ENGINE::ConversionException);

  CPPUNIT_ASSERT_THROW(_blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_13"]->getOutputPort("dble"),
                                                     _nodeMap["Node_22"]->getInputPort("long")),
                       YACS::ENGINE::ConversionException);

  CPPUNIT_ASSERT_THROW(_blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_13"]->getOutputPort("dble"),
                                                     _nodeMap["Node_24"]->getInputPort("dblevec")),
                       YACS::ENGINE::ConversionException);

  CPPUNIT_ASSERT_THROW(_blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_11"]->getOutputPort("dblevecvec"),
                                                     _nodeMap["Node_24"]->getInputPort("dblevec")),
                       YACS::ENGINE::ConversionException);

  CPPUNIT_ASSERT_THROW(_blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_13"]->getOutputPort("seqstr"),
                                                     _nodeMap["Node_24"]->getInputPort("dblevec")),
                       YACS::ENGINE::ConversionException);
}


void RuntimeTest::createDataLinksCORBACORBA()
{
  DEBTRACE(" --- create data links, CORBA to CORBA" );

  // double->double
  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_16"]->getOutputPort("c"),
                                _nodeMap["Node_17"]->getInputPort("b"));

  // double->double
  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_16"]->getOutputPort("c"),
                                _nodeMap["Node_18"]->getInputPort("b"));

  CPPUNIT_ASSERT_THROW(_blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_19"]->getOutputPort("obj"),
                                                     _nodeMap["Node_21"]->getInputPort("double")),
                       YACS::ENGINE::ConversionException);

  CPPUNIT_ASSERT_THROW(_blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_23"]->getOutputPort("dblevec"),
                                                     _nodeMap["Node_31"]->getInputPort("lngvec")),
                       YACS::ENGINE::ConversionException);

  CPPUNIT_ASSERT_THROW(_blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_20"]->getOutputPort("str"),
                                                     _nodeMap["Node_27"]->getInputPort("strvec")),
                       YACS::ENGINE::ConversionException);

  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_19"]->getOutputPort("obj"),
                                _nodeMap["Node_20"]->getInputPort("obj"));

  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_18"]->getOutputPort("c"),
                                _nodeMap["Node_20"]->getInputPort("double"));

  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_20"]->getOutputPort("long"),
                                _nodeMap["Node_21"]->getInputPort("double"));

  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_20"]->getOutputPort("long"),
                                _nodeMap["Node_21"]->getInputPort("long"));

  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_20"]->getOutputPort("str"),
                                _nodeMap["Node_21"]->getInputPort("str"));

  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_20"]->getOutputPort("obj"),
                                _nodeMap["Node_21"]->getInputPort("obj"));

  // Corba sequence<double> -> Corba sequence<double>
  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_23"]->getOutputPort("dblevec"),
                                _nodeMap["Node_24"]->getInputPort("dblevec"));

  // Corba sequence<double> -> Corba sequence<double>
  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_31"]->getOutputPort("lngvec"),
                                _nodeMap["Node_25"]->getInputPort("dblevec"));

  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_20"]->getOutputPort("long"),
                                _nodeMap["Node_22"]->getInputPort("double"));

  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_20"]->getOutputPort("long"),
                                _nodeMap["Node_22"]->getInputPort("long"));

  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_20"]->getOutputPort("str"),
                                _nodeMap["Node_22"]->getInputPort("str"));
}


void RuntimeTest::createDataLinksCORBAPython()
{
  DEBTRACE(" --- create data links, CORBA to Python" );

  CPPUNIT_ASSERT_THROW(_blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_20"]->getOutputPort("double"),
                                                     _nodeMap["Node_13"]->getInputPort("lng")),
                       YACS::ENGINE::ConversionException);

  CPPUNIT_ASSERT_THROW(_blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_27"]->getOutputPort("strvec"),
                                                     _nodeMap["Node_13"]->getInputPort("s")),
                       YACS::ENGINE::ConversionException);


  // Corba C -> Python C (derived from Obj):OK
  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_32"]->getOutputPort("objc"),
                                _nodeMap["Node_14"]->getInputPort("objc"));

  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_21"]->getOutputPort("double"),
                                _nodeMap["Node_13"]->getInputPort("dble"));

  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_21"]->getOutputPort("long"),
                                _nodeMap["Node_13"]->getInputPort("lng"));

  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_21"]->getOutputPort("str"),
                                _nodeMap["Node_13"]->getInputPort("s"));

  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_21"]->getOutputPort("obj"),
                                _nodeMap["Node_13"]->getInputPort("ob"));

  // --- Corba sequence<double> -> Python sequence<double>
  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_23"]->getOutputPort("dblevec"),
                                _nodeMap["Node_14"]->getInputPort("li"));

  // --- Corba sequence<sequence<double>> -> Python sequence<sequence<double>>
  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_29"]->getOutputPort("dblevecvec"),
                                _nodeMap["Node_14"]->getInputPort("lili"));

  // --- Corba sequence<object> -> Python sequence<object>
  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_28"]->getOutputPort("objvec"),
                                _nodeMap["Node_14"]->getInputPort("lobj"));

  // --- Corba sequence<string> -> Python sequence<string>
  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_27"]->getOutputPort("strvec"),
                                _nodeMap["Node_14"]->getInputPort("lstr"));

  // --- Corba sequence<object> -> Python sequence<object>
  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_30"]->getOutputPort("objvecvec"),
                                _nodeMap["Node_14"]->getInputPort("llobj"));

  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_31"]->getOutputPort("lngvec"),
                                _nodeMap["Node_15"]->getInputPort("lngvec"));

  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_31"]->getOutputPort("lngvec"),
                                _nodeMap["Node_15"]->getInputPort("dblevec"));
}


void RuntimeTest::createDataLinksXML()
{
  DEBTRACE(" --- create data links, xml nodes" );

  CPPUNIT_ASSERT_THROW(_blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_16"]->getOutputPort("c"),
                                                     _nodeMap["Node_36"]->getInputPort("dblevec")),
                       YACS::ENGINE::ConversionException);

  CPPUNIT_ASSERT_THROW(_blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_26"]->getOutputPort("dblevec"),
                                                     _nodeMap["Node_36"]->getInputPort("dble")),
                       YACS::ENGINE::ConversionException);

  CPPUNIT_ASSERT_THROW(_blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_36"]->getOutputPort("dblevec"),
                                                     _nodeMap["Node_33"]->getInputPort("dble")),
                       YACS::ENGINE::ConversionException);

  CPPUNIT_ASSERT_THROW(_blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_36"]->getOutputPort("dble"),
                                                     _nodeMap["Node_34"]->getInputPort("dblevec")),
                       YACS::ENGINE::ConversionException);

  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_16"]->getOutputPort("c"),
                                _nodeMap["Node_36"]->getInputPort("dble"));

  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_26"]->getOutputPort("dblevec"),
                                _nodeMap["Node_36"]->getInputPort("dblevec"));

  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_29"]->getOutputPort("dblevecvec"),
                                _nodeMap["Node_36"]->getInputPort("dblevecvec"));

  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_36"]->getOutputPort("dble"),
                                _nodeMap["Node_33"]->getInputPort("dble"));

  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_36"]->getOutputPort("dblevec"),
                                _nodeMap["Node_34"]->getInputPort("dblevec"));

  _blocMap["Bloc_3"]->edAddLink(_nodeMap["Node_36"]->getOutputPort("dblevecvec"),
                                _nodeMap["Node_35"]->getInputPort("dblevecvec"));
}


void RuntimeTest::manualInitInputPort()
{
  DEBTRACE(" --- InputPort initialization" );
  
  {
    CPPUNIT_ASSERT(! _blocMap["Bloc_3"]->edAreAllInputPortInitialized() );
    set<InputPort *> unitialized =  _blocMap["Bloc_3"]->edGetSetOfUnitializedInputPort();
    DEBTRACE(unitialized.size());
    for (set<InputPort *>::const_iterator iter = unitialized.begin(); iter != unitialized.end(); iter++)
      {
        DEBTRACE(_blocMap["Bloc_3"]->getInPortName(*iter));
      }
  }
  _nodeMap["Node_10"]->getInputPort("a")->edInit(10.51);

//   CORBA::Any anyLong;
//   anyLong <<= (CORBA::Long) 1;
  DEBTRACE("---");
  _nodeMap["Node_20"]->getInputPort("long")->edInit(1);
  DEBTRACE("---");
  
  // --- manual set of a linked input port: no use under normal conditions
  //     (value will be replaced by output value before activation and read)

  CORBA::Any aString;
  aString <<= (const char *)"texte";
  _nodeMap["Node_20"]->getInputPort("str")->put(&aString);
//  _nodeMap["Node_20"]->getInputPort("str")->edInit("texte");
  DEBTRACE("---");

  {
    set<InputPort *> unitialized =  _blocMap["Bloc_3"]->edGetSetOfUnitializedInputPort();
    DEBTRACE(unitialized.size());
    CPPUNIT_ASSERT( _blocMap["Bloc_3"]->edAreAllInputPortInitialized() );
  }

  {
    double d=10.51;
    int l;
    PyObject *pyob;
    CORBA::Any *any;
    Any * a;
    DEBTRACE("Python input port double");
    InputPort* inport=_nodeMap["Node_10"]->getInputPort("a");

    DEBTRACE("Initialize port with C++ double value");
    a = AtomAny::New(d);
    inport->edInit("Cpp",a);
    a->decrRef();
    pyob=((InputPyPort*)inport)->getPyObj();
    DEBTRACE(pyob->ob_refcnt);
    CPPUNIT_ASSERT(PyFloat_AS_DOUBLE(pyob) == d);

    DEBTRACE("Initialize port with XML double value");
    inport->edInit("XML","<value><double>10.51</double></value>");
    pyob=((InputPyPort*)inport)->getPyObj();
    DEBTRACE(pyob->ob_refcnt);
    CPPUNIT_ASSERT(PyFloat_AS_DOUBLE(pyob) == d);

    DEBTRACE("Initialize port with XML int value");
    inport->edInit("XML","<value><int>10</int></value>");
    pyob=((InputPyPort*)inport)->getPyObj();
    DEBTRACE(pyob->ob_refcnt);
    CPPUNIT_ASSERT(PyFloat_AS_DOUBLE(pyob) == 10.);

    DEBTRACE("Initialize port with Python double value");
    PyGILState_STATE gstate = PyGILState_Ensure();
    inport->edInit("Python",PyFloat_FromDouble(d));
    PyGILState_Release(gstate);
    pyob=((InputPyPort*)inport)->getPyObj();
    DEBTRACE(pyob->ob_refcnt);
    CPPUNIT_ASSERT(PyFloat_AS_DOUBLE(pyob) == d);

    DEBTRACE("Python input port seq<double>");
    inport=_nodeMap["Node_14"]->getInputPort("li");
    DEBTRACE("Initialize port with XML seq<double> value");
    inport->edInit("XML","<value><array><data>\
            <value><double>1.5</double></value>\
            <value><double>2.4</double></value>\
            </data></array></value>");
    pyob=((InputPyPort*)inport)->getPyObj();
    DEBTRACE(pyob->ob_refcnt);

    DEBTRACE("Initialize port with XML seq<int> value");
    inport->edInit("XML","<value><array><data>\
            <value><int>15</int></value>\
            <value><int>24</int></value>\
            </data></array></value>");
    pyob=((InputPyPort*)inport)->getPyObj();
    DEBTRACE(pyob->ob_refcnt);

    DEBTRACE("CORBA input port int");
    inport=_nodeMap["Node_20"]->getInputPort("long");
    DEBTRACE("Initialize port with XML int value");
    inport->edInit("XML","<value><int>10</int></value>");
    any=((InputCorbaPort*)inport)->getAny();
    CORBA::Long LL;
    *any >>= LL;
    l = LL;
    DEBTRACE("l = " << l);
    CPPUNIT_ASSERT(l == 10);

    DEBTRACE("CORBA input port double");
    inport=_nodeMap["Node_17"]->getInputPort("b");
    DEBTRACE("Initialize port with XML double value");
    inport->edInit("XML","<value><double>10.51</double></value>");
    any=((InputCorbaPort*)inport)->getAny();
    *any >>= d;
    CPPUNIT_ASSERT_DOUBLES_EQUAL(10.51,d, 1e-12);

    DEBTRACE("CORBA input port seq<double>");
    inport=_nodeMap["Node_24"]->getInputPort("dblevec");
    DEBTRACE("Initialize port with XML seq<double> value");
    inport->edInit("XML","<value><array><data>\
            <value><double>1.5</double></value>\
            <value><double>2.4</double></value>\
            </data></array></value>");

  }
}

void RuntimeTest::manualExecuteNoThread()
{
  DEBTRACE(" --- execution Python Node_10" );
  ((ElementaryNode*)_nodeMap["Node_10"])->load();
  ((ElementaryNode*)_nodeMap["Node_10"])->execute();
  //  CPPUNIT_ASSERT_DOUBLES_EQUAL(10.51, (ElementaryNode*)_nodeMap["Node_10"])

  DEBTRACE(" --- execution Python Node_11" );
  ((ElementaryNode*)_nodeMap["Node_11"])->load();
  ((ElementaryNode*)_nodeMap["Node_11"])->execute();

  DEBTRACE(" --- execution Python Node_12" );
  ((ElementaryNode*)_nodeMap["Node_12"])->load();
  ((ElementaryNode*)_nodeMap["Node_12"])->execute();

  DEBTRACE(" --- execution CORBA Node_16" );
  ((ElementaryNode*)_nodeMap["Node_16"])->load();
  ((ElementaryNode*)_nodeMap["Node_16"])->execute();

  DEBTRACE(" --- execution CORBA Node_17" );
  ((ElementaryNode*)_nodeMap["Node_17"])->load();
  ((ElementaryNode*)_nodeMap["Node_17"])->execute();

  DEBTRACE(" --- execution CORBA Node_18" );
  ((ElementaryNode*)_nodeMap["Node_18"])->load();
  ((ElementaryNode*)_nodeMap["Node_18"])->execute();

  DEBTRACE(" --- execution CORBA Node_19" );
  ((ElementaryNode*)_nodeMap["Node_19"])->load();
  ((ElementaryNode*)_nodeMap["Node_19"])->execute();

  DEBTRACE(" --- execution CORBA Node_20" );
  ((ElementaryNode*)_nodeMap["Node_20"])->load();
  ((ElementaryNode*)_nodeMap["Node_20"])->execute();

  DEBTRACE(" --- execution CORBA Node_21" );
  ((ElementaryNode*)_nodeMap["Node_21"])->load();
  ((ElementaryNode*)_nodeMap["Node_21"])->execute();

  DEBTRACE(" --- execution CORBA Node_29" );
  ((ElementaryNode*)_nodeMap["Node_29"])->load();
  ((ElementaryNode*)_nodeMap["Node_29"])->execute();

  DEBTRACE(" --- execution Python Node_13" );
  ((ElementaryNode*)_nodeMap["Node_13"])->load();
  ((ElementaryNode*)_nodeMap["Node_13"])->execute();

  DEBTRACE(" --- execution CORBA Node_22" );
  ((ElementaryNode*)_nodeMap["Node_22"])->load();
  ((ElementaryNode*)_nodeMap["Node_22"])->execute();

  DEBTRACE(" --- execution CORBA Node_23" );
  ((ElementaryNode*)_nodeMap["Node_23"])->load();
  ((ElementaryNode*)_nodeMap["Node_23"])->execute();

  DEBTRACE(" --- execution CORBA Node_24" );
  ((ElementaryNode*)_nodeMap["Node_24"])->load();
  ((ElementaryNode*)_nodeMap["Node_24"])->execute();

  DEBTRACE(" --- execution CORBA Node_27" );
  ((ElementaryNode*)_nodeMap["Node_27"])->load();
  ((ElementaryNode*)_nodeMap["Node_27"])->execute();

  DEBTRACE(" --- execution CORBA Node_28" );
  ((ElementaryNode*)_nodeMap["Node_28"])->load();
  ((ElementaryNode*)_nodeMap["Node_28"])->execute();

  DEBTRACE(" --- execution CORBA Node_30" );
  ((ElementaryNode*)_nodeMap["Node_30"])->load();
  ((ElementaryNode*)_nodeMap["Node_30"])->execute();

  DEBTRACE(" --- execution CORBA Node_32" );
  ((ElementaryNode*)_nodeMap["Node_32"])->load();
  ((ElementaryNode*)_nodeMap["Node_32"])->execute();

  DEBTRACE(" --- execution CORBA Node_26" );
  ((ElementaryNode*)_nodeMap["Node_26"])->load();
  ((ElementaryNode*)_nodeMap["Node_26"])->execute();

  DEBTRACE(" --- execution CORBA Node_31" );
  ((ElementaryNode*)_nodeMap["Node_31"])->load();
  ((ElementaryNode*)_nodeMap["Node_31"])->execute();

  DEBTRACE(" --- execution Python Node_14" );
  ((ElementaryNode*)_nodeMap["Node_14"])->load();
  ((ElementaryNode*)_nodeMap["Node_14"])->execute();

  DEBTRACE(" --- execution Python Node_15" );
  ((ElementaryNode*)_nodeMap["Node_15"])->load();
  ((ElementaryNode*)_nodeMap["Node_15"])->execute();

  DEBTRACE(" --- execution XML Node_36" );
  ((ElementaryNode*)_nodeMap["Node_36"])->load();
  ((ElementaryNode*)_nodeMap["Node_36"])->execute();

  DEBTRACE(" --- execution CORBA Node_33" );
  ((ElementaryNode*)_nodeMap["Node_33"])->load();
  ((ElementaryNode*)_nodeMap["Node_33"])->execute();

  DEBTRACE(" --- execution CORBA Node_34" );
  ((ElementaryNode*)_nodeMap["Node_34"])->load();
  ((ElementaryNode*)_nodeMap["Node_34"])->execute();

  DEBTRACE(" --- execution CORBA Node_35" );
  ((ElementaryNode*)_nodeMap["Node_35"])->load();
  ((ElementaryNode*)_nodeMap["Node_35"])->execute();

  DEBTRACE(" --- end of execution" );
}


void RuntimeTest::manualGetOutputs()
{
  CORBA::Double l;
  CORBA::Any* x;

  PyObject *ob=((OutputPyPort*)_nodeMap["Node_11"]->getOutputPort("c"))->get();
  DEBTRACE("ob refcnt: " << ob->ob_refcnt);
  std::cerr << "Output port Node_11.c: ";
  PyGILState_STATE gstate = PyGILState_Ensure();
  PyObject_Print(ob,stderr,Py_PRINT_RAW);
  PyGILState_Release(gstate);
  std::cerr << std::endl;

  //   DEBTRACE("a: " << &a);
  //   DEBTRACE("a.value(): " << a.value());

  x= ((InputCorbaPort*)_nodeMap["Node_16"]->getInputPort("a"))->getAny();
  DEBTRACE("CORBA Node_16 input a any: " << x);
  *x >>= l;
  DEBTRACE("CORBA Node_16 input a double: " << l);

  *((InputCorbaPort*)_nodeMap["Node_17"]->getInputPort("b"))->getAny() >>= l;
  DEBTRACE("CORBA Node_17 input b double: " << l);

  *((InputCorbaPort*)_nodeMap["Node_18"]->getInputPort("b"))->getAny() >>= l;
  DEBTRACE("CORBA Node_18 input a double: " << l);

  *((OutputCorbaPort*)_nodeMap["Node_16"]->getOutputPort("c"))->getAny() >>= l;
  DEBTRACE("CORBA Node_16 output c double: " << l);

  *((OutputCorbaPort*)_nodeMap["Node_17"]->getOutputPort("c"))->getAny() >>= l;
  DEBTRACE("CORBA Node_17 output c double: " << l);

  *((OutputCorbaPort*)_nodeMap["Node_18"]->getOutputPort("c"))->getAny() >>= l;
  DEBTRACE("CORBA Node_18 output c double: " << l);

  DEBTRACE(" --- fini" );
}

void RuntimeTest::createCppNodes()
{
    
  string s = "Node_Cpp";
  ElementaryNode* node = _myRuntime->createCompoNode("Cpp",s);
  _nodeMap[s] = node;
  InputPort  *i1  = node->edAddInputPort("id1",  _tc_double);
  InputPort  *i2  = node->edAddInputPort("ii2",  _tc_int);

  OutputPort  *o3  = node->edAddOutputPort("os3",  _tc_string);
  
  CPPUNIT_ASSERT_EQUAL(node->getImplementation(),string("Cpp"));
  CPPUNIT_ASSERT_EQUAL(node->getNumberOfInputPorts(),2);
  CPPUNIT_ASSERT_EQUAL(node->getNumberOfOutputPorts(),1);
  CPPUNIT_ASSERT_EQUAL(node->getInPortName(i1),string("id1"));
  CPPUNIT_ASSERT_EQUAL(node->getOutPortName(o3),string("os3"));
   
  CPPUNIT_ASSERT_EQUAL(node->getInputPort("id1"),i1);
  CPPUNIT_ASSERT_EQUAL(node->getOutputPort("os3"),o3);
  
  delete node;
}

void RuntimeTest::convertPorts()
{
  const char *type[] = { "Cpp", "CORBA", "Python", "XML" };
  int itype, jtype, ntypes = sizeof(type)/sizeof(const char *);

  string s0 = "Node_";
  CORBA::Any  cAny;
  PyObject  * pAny;
  Any       * vAny;
  std::string sAny;
  
  for (itype=0; itype<ntypes-1; itype++) 
    {
      string s = s0 + type[itype];
      
      cerr << endl;
      DEBTRACE("Node " << type[itype] << " creation");
      ElementaryNode* node;
      switch (itype) 
        {
        case 1:
          node = _myRuntime->createRefNode(type[itype],s);
          break;
        case 2: 
          node = _myRuntime->createScriptNode(type[itype], s);
          break;
        case 3:
          node = _myRuntime->createRefNode("xmlsh",s);
          break;
        default:
          node = _myRuntime->createCompoNode(type[itype], s);
          break;
        }
      InputPort  *inport  = node->edAddInputPort("id1",  _tc_double);
      CPPUNIT_ASSERT(NULL != node);
      for (jtype=0; jtype < ntypes; jtype++) 
        {
          double d0= itype * 10 + jtype, d1;
          InputPort* pwrap=_myRuntime->adapt(inport, type[jtype], _tc_double);

          const void *v;
          switch (jtype) 
            {
            case 0:
            case 4:
              v = vAny = AtomAny::New(d0);
              break;
            case 1:
              v = &cAny;
              cAny <<= (CORBA::Double) d0;
              break;
            case 2:
              v = pAny = PyFloat_FromDouble(d0);
              break;
            case 3:
              { 
                ostringstream os;
                os << "<value><double>" << d0 << "</double></value>";
                sAny = os.str();
                v = sAny.c_str();
              }
              break;
            default:
              v = NULL;
              break;
            }

            if ((itype == 2) && (jtype == 2)) 
              {
                std::cerr << "s = " << s << endl;
              }

            DEBTRACE("Put a " << type[jtype] << " double (" << d0 << ") in " << s);
            if (jtype == 2) 
              {
                PyGILState_STATE gstate = PyGILState_Ensure();
                pwrap->put(v);
                PyGILState_Release(gstate);
              }
            else
              pwrap->put(v);
            cerr << endl;

            switch (itype)
              {
              case 0:
              case 4: 
                {
                  Any * a=((InputCppPort*)inport)->getCppObj();
                  CPPUNIT_ASSERT(a->getType()->isA(_tc_double));
                  d1 = a->getDoubleValue();
                }
                break;
              case 1:
                {
                  CORBA::Any * a = ((InputCorbaPort*)inport)->getAny();
                  CPPUNIT_ASSERT(a->type()->equal(CORBA::_tc_double));
                  CORBA::Double d;
                  *a >>= d;
                  d1 = d;
                }
                break;
              case 2:
                {
                  PyObject *a = ((InputPyPort*)inport)->getPyObj();
                  CPPUNIT_ASSERT(PyFloat_Check(a));
                  d1 = PyFloat_AsDouble(a);
                }
                break;
              case 3:
                {
                  const char *a = ((InputXmlPort*)inport)->getXml();
                  const char *a1, *a2, *a3, *a_end;
                  a_end = a + strlen(a) - 1;
                  while (isspace(*a_end)) a_end--;
                  a1 = a;
                  a2 = a + strlen("<value><double>");
                  a3 = a_end - strlen("</double></value>") + 1;
                  CPPUNIT_ASSERT(!strncmp(a1, "<value><double>", strlen("<value><double>")));
                  CPPUNIT_ASSERT(!strncmp(a3, "</double></value>", strlen("</double></value>")));
                  char *err;
                  d1 = strtod(a2, &err);
                  CPPUNIT_ASSERT(err == a3);
                }
                break;
              }
          
            CPPUNIT_ASSERT_DOUBLES_EQUAL(d0, d1, 1e-12);
         
            switch (jtype) 
              {
              case 0:
              case 4:
                vAny->decrRef();
                break;
              case 2:
                Py_DECREF(pAny);
                break;
              default:
                break;
              }
          if (pwrap != inport) delete pwrap;
        }
      delete node;
    }
}

void myTestRun(int nIn, int nOut, Any **In, Any **Out) throw (YACS::Exception)
{
  double x, y;
  cerr << "myTestRun nIn = " << nIn << endl;
  cerr << "myTestRun nOut = " << nOut << endl;

  x = In[0]->getDoubleValue();

  if (x >= 0.0)
    {
      y = sqrt(x);
      Out[0] = AtomAny::New(y);
    }
  else 
    {
      throw YACS::Exception("myTestRun : input must be a positive or null real");
    }
}


void RuntimeTest::executeCppNode()
{
  cerr << endl << endl;
  
  Any *u, *v, *w;
  double du, dv, dw;
  
  DEBTRACE("execute a CppNode with an internal C++ implementation")
  ServiceNode * node = _myRuntime->createCompoNode("Cpp", "test");
  ((CppNode *) node)->setFunc(myTestRun);
  
  InputPort  *in  = node->edAddInputPort("in",  _tc_double);
  OutputPort  *out  = node->edAddOutputPort("out",  _tc_double);
  node->load();

  dv = 4.5;
  v = AtomAny::New(dv);
  in->edInit("Cpp",v);
  
  node->execute();
  
  w = ((OutputCppPort *) out)->get();
  dw = w->getDoubleValue();
  
  cerr << "sqrt(" << dv << ") = " << dw << endl;
  CPPUNIT_ASSERT_DOUBLES_EQUAL(dw, sqrt(dv), 1e-12);
  
  u = AtomAny::New((double) -3.5);
  in->edInit("Cpp",u);
  CPPUNIT_ASSERT_THROW(node->execute(), YACS::Exception);
  
  u->decrRef();
  v->decrRef();
  
  delete node;
  
  DEBTRACE("execute a CppNode with an external C++ implementation (in a dynamic library)");
  
  node = _myRuntime->createCompoNode("Cpp", "test");
  CppComponent * C = new CppComponent("TestComponent");
  node->setComponent(C);
  C->decrRef();
  node->setMethod("f");
  
  in  = node->edAddInputPort("in",  _tc_double);
  out  = node->edAddOutputPort("out",  _tc_double);
  node->load();

  dv = 4.5;
  v = AtomAny::New(dv);
  in->edInit("Cpp",v);
  
  node->execute();
  
  w = ((OutputCppPort *) out)->get();
  dw = w->getDoubleValue();
  
  cerr << "sqrt(" << dv << ") = " << dw << endl;
  CPPUNIT_ASSERT_DOUBLES_EQUAL(dw, sqrt(dv), 1e-12);
  
  u = AtomAny::New((double) -3.5);
  in->edInit("Cpp",u);
  CPPUNIT_ASSERT_THROW(node->execute(), YACS::Exception);
  
  u->decrRef();
  v->decrRef();
  
  delete node;
  
}

void RuntimeTest::createGraphWithCppNodes()
{
  ElementaryNode * n1, * n2;
  InputPort  *in1, *in2;
  OutputPort  *out1, *out2;

  n1 = _myRuntime->createCompoNode(CppNode::KIND, "test1");
  n2 = _myRuntime->createScriptNode(PythonNode::KIND, "test2");
   
  CppComponent *C = new CppComponent("TestComponent"); 
  ((CppNode *) n1)->setComponent(C);
  C->decrRef();
  ((CppNode *) n1)->setMethod("f");
  in1  = n1->edAddInputPort("i",  _tc_double);
  out1  = n1->edAddOutputPort("o",  _tc_double);
  
  ((InlineNode*) n2)->setScript("a=a+1\n");
  in2  = n2->edAddInputPort("a",  _tc_double);
  out2  = n2->edAddOutputPort("a",  _tc_double);
    
  Bloc * loopBody = _myRuntime->createBloc("LoopBody");
  loopBody->edAddChild(n1);
  loopBody->edAddChild(n2);
  loopBody->edAddLink(out1, in2);
  loopBody->edAddLink(out2, in1);
    
  ForLoop *loop=_myRuntime->createForLoop("Loop");
  loop->edSetNode(loopBody);
  InputPort *iNbTimes=loop->edGetNbOfTimesInputPort();
  iNbTimes->edInit(5);
    
  Bloc * graph = _myRuntime->createBloc("graph");
  graph->edAddChild(loop);
    
  DEBTRACE("n1->getInputPort(\"in\") = " << n1->getInputPort("i")->getName())
    
  in1->edInit(4.5);
  in2->edInit(0.0);
    
  Executor exe;
  exe.RunW(graph);
    
  DEBTRACE(out2->dump());
    
  delete graph;
}

void RuntimeTest::classTeardown()
{
  if (endTests) return;
  
  endTests = true;

  delete _blocMap["Bloc_3"];
  delete _blocMap["Bloc_2"];
  delete _nodeMap["Node_4"];
  delete _nodeMap["Node_5"];
  delete _nodeMap["Node_6"];
  delete _nodeMap["Node_7"];
  delete _nodeMap["Node_8"];
  delete _nodeMap["Node_9"];

  _tc_seqC->decrRef();
  _tc_C->decrRef();
  list<TypeCodeObjref *>::iterator i;
  for (i=_ltc.begin(); i != _ltc.end(); i++)
    (*i)->decrRef();

  _tc_seqseqobj->decrRef();
  _tc_seqobj->decrRef();


  _tc->decrRef();
  _tc_seqseqdble->decrRef();
  _tc_seqdble->decrRef();
  _tc_seqstr->decrRef();
  _tc_seqlong->decrRef();
  _tc_string->decrRef();

  _myRuntime->fini();
  delete _myRuntime;
}

