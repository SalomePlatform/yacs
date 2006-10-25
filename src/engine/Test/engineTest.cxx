
// --- include from engine first, to avoid redifinition warning _POSIX_C_SOURCE

#include "Bloc.hxx"
#include "ElementaryNode.hxx"
#include "Loop.hxx"
#include "Switch.hxx"
#include "Runtime.hxx"

#include "engineTest.hxx"

#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>

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

map<string, Node*> EngineTest::_nodeMap; 
map<string, ComposedNode*> EngineTest::_compoMap;

// --- init typecodes

TypeCode *EngineTest::_tc_bool   = new TypeCode(Bool);
TypeCode *EngineTest::_tc_int    = new TypeCode(Int);
TypeCode *EngineTest::_tc_double = new TypeCode(Double);
    

void EngineTest::setUp()
{
}

void EngineTest::tearDown()
{
}

void EngineTest::checkGetRuntime()
{
  CPPUNIT_ASSERT_THROW(Runtime *myrun = getRuntime(), YACS::Exception); 
  Runtime::setRuntime();
  Runtime *myrun1 = getRuntime();
  CPPUNIT_ASSERT(myrun1);
  Runtime *myrun2 = getRuntime();
  CPPUNIT_ASSERT_EQUAL(myrun1, myrun2);
  }

void EngineTest::checkInGateOutGate()
{
  string nodeName = "Node1";
  ElementaryNode* node1 = new TestElemNode(nodeName);
  _nodeMap[nodeName] = node1;

  nodeName = "Node2";
  ElementaryNode* node2 = new TestElemNode(nodeName);
  _nodeMap[nodeName] = node2;

  DEBTRACE(" --- check InGate OK after node creation" );
  {
    InGate *node1Ingate = node1->getInGate();
    CPPUNIT_ASSERT(node1Ingate);
    string name=node1Ingate->getNameOfTypeOfCurrentInstance();
    string expected="InGate";
    CPPUNIT_ASSERT_EQUAL(name,expected);
  }

  DEBTRACE(" --- check OutGate OK after node creation" );
  {
    OutGate *node1Outgate = node1->getOutGate();
    CPPUNIT_ASSERT(node1Outgate);
    string name=node1Outgate->getNameOfTypeOfCurrentInstance();
    string expected="OutGate";
    CPPUNIT_ASSERT_EQUAL(name,expected);
  }
}

void EngineTest::checkNodePortNumber()
{
  ElementaryNode* node1 = (ElementaryNode*) _nodeMap["Node1"];

  DEBTRACE(" --- check number of ports = 0 after node creation" );
  CPPUNIT_ASSERT(node1->getNumberOfInputPorts() == 0);
  CPPUNIT_ASSERT(node1->getNumberOfOutputPorts() == 0);

  InputPort  *in1 = node1->edAddInputPort("ib1",_tc_bool);
  InputPort  *in2 = node1->edAddInputPort("ii1",_tc_int);
  InputPort  *in3 = node1->edAddInputPort("ii2",_tc_int);
  InputPort  *in4 = node1->edAddInputPort("id1",_tc_double);

  OutputPort *ou1 = node1->edAddOutputPort("ob1",_tc_bool);
  OutputPort *ou2 = node1->edAddOutputPort("oi1",_tc_int);
  OutputPort *ou3 = node1->edAddOutputPort("od1",_tc_double);

  DEBTRACE(" --- check number of ports after ports creation" );
//   DEBTRACE("     node1->getNumberOfInputPorts(): "
// 	   << node1->getNumberOfInputPorts());
//   DEBTRACE("     node1->getNumberOfOutputPorts(): "
// 	   << node1->getNumberOfOutputPorts());
  CPPUNIT_ASSERT(node1->getNumberOfInputPorts() == 4);
  CPPUNIT_ASSERT(node1->getNumberOfOutputPorts() == 3);
}

void EngineTest::checkPortTypeName()
{
  ElementaryNode* node1 = (ElementaryNode*) _nodeMap["Node1"];

  DEBTRACE(" --- check InputPort name OK" );
  {
    string name=node1->getInputPort("ib1")->NAME;
    string expected="InputPort";
    CPPUNIT_ASSERT_EQUAL(name,expected);
  }

  DEBTRACE(" --- check OutputPort name OK" );
  {
    string name=node1->getOutputPort("ob1")->NAME;
    string expected="OutputPort";
    CPPUNIT_ASSERT_EQUAL(name,expected);
  }
}

void EngineTest::checkDuplicatePortName()
{
  ElementaryNode* node1 = (ElementaryNode*) _nodeMap["Node1"];
  DEBTRACE(" --- check duplicated name throws exception" );
  CPPUNIT_ASSERT_THROW(InputPort *in5=node1->edAddInputPort("ii2",_tc_int),
		       YACS::Exception); 
}

void EngineTest::checkRemovePort()
{
  ElementaryNode* node1 = (ElementaryNode*) _nodeMap["Node1"];
  ElementaryNode* node2 = (ElementaryNode*) _nodeMap["Node2"];

    DEBTRACE(" --- check remove port" );
  {
    node1->edRemovePort(node1->getInputPort("ib1"));
    CPPUNIT_ASSERT(node1->getNumberOfInputPorts() == 3);
    CPPUNIT_ASSERT(node1->getNumberOfOutputPorts() == 3);
  }

  DEBTRACE(" --- check remove wrong port throws exception" )
  {
    CPPUNIT_ASSERT_THROW(node1->edRemovePort(node1->getInputPort("ib1")),
 			 YACS::Exception);
  }
}

void EngineTest::checkAddNodesToBloc()
{
  DEBTRACE(" --- delete node; // ====== NOT OK : done by bloc" );

  for (int i=0; i<10; i++)
    {
      ostringstream ss;
      ss << "Node_" << i;
      string s = ss.str();
      ElementaryNode* node = new TestElemNode(s);
      _nodeMap[s] = node;
      InputPort  *i1 = node->edAddInputPort("ib1",_tc_bool);
      InputPort  *i2 = node->edAddInputPort("ii1",_tc_int);
      InputPort  *i3 = node->edAddInputPort("ii2",_tc_int);
      InputPort  *i4 = node->edAddInputPort("id1",_tc_double);
      OutputPort *o1 = node->edAddOutputPort("ob1",_tc_bool);
      OutputPort *o2 = node->edAddOutputPort("oi1",_tc_int);
      OutputPort *o3 = node->edAddOutputPort("od1",_tc_double);
    }

  DEBTRACE(" --- create bloc, add two nodes, check constituants" );

  Bloc* bloc1 = new Bloc("bloc1");
  _nodeMap["bloc1"] = bloc1;
  _compoMap["bloc1"] = bloc1;
  bloc1->edAddChild(_nodeMap["Node_1"]);
  bloc1->edAddChild(_nodeMap["Node_2"]);
  {
    set<ElementaryNode *> setelem = bloc1->getRecursiveConstituents();
    CPPUNIT_ASSERT(setelem.size() == 2);
  }
}

void EngineTest::checkAddingTwiceSameNodeInSameBloc()
{
  DEBTRACE(" --- add the same node two times does nothing: return false" );

  CPPUNIT_ASSERT(! ((Bloc*)_compoMap["bloc1"])->edAddChild(_nodeMap["Node_1"]));
}

void EngineTest::checkAddingTwiceSameNodeInTwoBlocs()
{
  DEBTRACE(" --- add a node already used elsewhere raises exception" );

  Bloc* bloc2 = new Bloc("bloc2");
  _nodeMap["bloc2"] = bloc2;
  _compoMap["bloc2"] = bloc2;
  bloc2->edAddChild(_nodeMap["Node_3"]);

  CPPUNIT_ASSERT_THROW(bloc2->edAddChild(_nodeMap["Node_1"]),
 		       YACS::Exception);
}

void EngineTest::checkRecursiveBlocs_NumberOfNodes()
{
  Bloc *bloc1 = (Bloc*)_compoMap["bloc1"];
  Bloc *bloc2 = (Bloc*)_compoMap["bloc2"];

  DEBTRACE(" --- recursive blocs, check constituants" );

  Bloc* bloc3 = new Bloc("bloc3");
  _nodeMap["bloc3"] = bloc3;
  _compoMap["bloc3"] = bloc3;
  bloc3->edAddChild((bloc1));  // 2 elementary nodes 
  bloc3->edAddChild((bloc2));  // 1 elementary node
  bloc3->edAddChild(_nodeMap["Node_4"]);   // 1 elementary node
  {
    set<ElementaryNode *> setelem = bloc3->getRecursiveConstituents();
    CPPUNIT_ASSERT(setelem.size() == 4);
    for (set<ElementaryNode*>::iterator it=setelem.begin(); it!=setelem.end(); it++)
      {
	DEBTRACE("     elem name = " << (*it)->getName());
      }
  }
}

void EngineTest::checkRecursiveBlocs_NumberOfPorts()
{
  Bloc *bloc3 = (Bloc*)_compoMap["bloc3"];

  DEBTRACE(" --- recursive blocs, check input & output ports // COMPLETER" );
  CPPUNIT_ASSERT(bloc3->getNumberOfInputPorts() == 4*4);
  DEBTRACE("     number of input ports: " << bloc3->getNumberOfInputPorts());
  DEBTRACE("     number of output ports: " << bloc3->getNumberOfOutputPorts());
  {
    set<InputPort *> inset = bloc3->getSetOfInputPort();
    set<OutputPort *> outset = bloc3->getSetOfOutputPort();
    for (set<InputPort *>::iterator it=inset.begin(); it!=inset.end(); it++)
      {
	DEBTRACE("     input port name = " << bloc3->getInputPortName(*it));
      }
    for (set<OutputPort *>::iterator it=outset.begin(); it!=outset.end(); it++)
      {
	DEBTRACE("     output port name = " << (*it)->getName());
      }
  }
}

void EngineTest::checkPortNameInBloc()
{

  DEBTRACE(" --- recursive blocs, check port names" );
  
  InputPort *inport = _nodeMap["Node_1"]->getInputPort("id1");
  CPPUNIT_ASSERT(_nodeMap["bloc3"]->getInputPortName(inport) == "bloc1.Node_1.id1");
}

void EngineTest::checkGetNameOfPortNotInBloc()
{
  InputPort *inport = _nodeMap["Node_5"]->getInputPort("id1");
  CPPUNIT_ASSERT_THROW(string name = _nodeMap["bloc3"]->getInputPortName(inport),
		       YACS::Exception);
}

void EngineTest::RecursiveBlocs_multipleRecursion()
{
  {
    Bloc* bloc = new Bloc("bloc4");
    _nodeMap["bloc4"] = bloc;
    _compoMap["bloc4"] = bloc;
    bloc->edAddChild(_nodeMap["Node_5"]);
    bloc->edAddChild(_nodeMap["Node_6"]);
  }

  {
    Bloc* bloc = new Bloc("bloc5");
    _nodeMap["bloc5"] = bloc;
    _compoMap["bloc5"] = bloc;
    bloc->edAddChild(_nodeMap["Node_7"]);
    bloc->edAddChild(_nodeMap["Node_8"]);
  }

  {
    Bloc* bloc = new Bloc("bloc6");
    _nodeMap["bloc6"] = bloc;
    _compoMap["bloc6"] = bloc;
    bloc->edAddChild(_nodeMap["bloc4"]);
    bloc->edAddChild(_nodeMap["bloc5"]);
  }

  {   
    Bloc* bloc = new Bloc("bloc7");
    _nodeMap["bloc7"] = bloc;
    _compoMap["bloc7"] = bloc;
    bloc->edAddChild(_nodeMap["bloc3"]);
    bloc->edAddChild(_nodeMap["bloc6"]);
  }

  {   
    Bloc* bloc = new Bloc("graphe");
    _nodeMap["graphe"] = bloc;
    _compoMap["graphe"] = bloc;
    bloc->edAddChild(_nodeMap["bloc7"]);
    bloc->edAddChild(_nodeMap["Node_9"]);
  }

  {
    set<ElementaryNode *> setelem = _nodeMap["graphe"]->getRecursiveConstituents();
    CPPUNIT_ASSERT(setelem.size() == 9);
    for (set<ElementaryNode*>::iterator it=setelem.begin(); it!=setelem.end(); it++)
      {
	DEBTRACE("     elem name = " << (*it)->getName());
      }
  }

  {
    set<InputPort *> inset = _nodeMap["bloc7"]->getSetOfInputPort();
    set<OutputPort *> outset = _nodeMap["bloc7"]->getSetOfOutputPort();
    for (set<InputPort *>::iterator it=inset.begin(); it!=inset.end(); it++)
      {
	DEBTRACE("     input port name for bloc7  = " << _nodeMap["bloc7"]->getInputPortName(*it));
	DEBTRACE("     input port name for graphe = " << _nodeMap["graphe"]->getInputPortName(*it));
      }
    for (set<OutputPort *>::iterator it=outset.begin(); it!=outset.end(); it++)
      {
	DEBTRACE("     output port name = " << (*it)->getName());
      }
  }
}
