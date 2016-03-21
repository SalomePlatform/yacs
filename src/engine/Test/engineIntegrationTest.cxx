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

#include "engineIntegrationTest.hxx"
#include "RuntimeForEngineIntegrationTest.hxx"
#include "ComponentInstanceTest.hxx"
#include "OutputDataStreamPort.hxx"
#include "InputDataStreamPort.hxx"
#include "ContainerTest.hxx"
#include "OptimizerLoop.hxx"
#include "ForEachLoop.hxx"
#include "WhileLoop.hxx"
#include "TypeCode.hxx"
#include "Executor.hxx"
#include "LinkInfo.hxx"
#include "ForLoop.hxx"
#include "ToyNode.hxx"
#include "Switch.hxx"
#include "Bloc.hxx"
#include "Proc.hxx"

#include <iostream>
#include <string.h>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

#define DBL_PRECISION_COMPARE 1.e-12

using namespace YACS::ENGINE;

void EngineIntegrationTest::setUp()
{
  RuntimeForEngineIntegrationTest::setRuntime();
}

void EngineIntegrationTest::tearDown()
{
}

//Simplest test to test basic mechanisms like Data, initialisation of ports.
void EngineIntegrationTest::testBloc1()
{
  ToyNode *n1=new ToyNode("T1");
  InputPort *i8=n1->edAddInputPort("o",Runtime::_tc_double);
  InputPort *i9=n1->edAddInputPort("p",Runtime::_tc_double);
  Bloc *graph=new Bloc("toto");
  graph->edAddChild(n1);
  OutputPort *o1=n1->edAddOutputPort("b",Runtime::_tc_double);
  i9->edInit(5.67);
  i8->edInit(2.78);
  Executor exe;
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)o1)->get()->getDoubleValue(),8.45,DBL_PRECISION_COMPARE );
  OutputPort *o2=n1->edAddOutputPort("c",Runtime::_tc_double);
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)o1)->get()->getDoubleValue(),4.225,DBL_PRECISION_COMPARE );
  Executor exe2;
  Bloc *clonedGraph=(Bloc *)graph->clone(0);
  delete graph;
  exe2.RunW(clonedGraph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)clonedGraph->getOutputPort("T1.b"))->get()->getDoubleValue(),4.225,DBL_PRECISION_COMPARE );
  delete clonedGraph;
}

//Only one level(hierarchy) simple graph. Tests DF and CF links.
void EngineIntegrationTest::testBloc2()
{
  ToyNode *n1=new ToyNode("T1");
  ToyNode *n2=new ToyNode("T2");
  ToyNode *n3=new ToyNode("T3");
  ToyNode *n4=new ToyNode("T4");
  Bloc *graph=new Bloc("Global");
  graph->edAddChild(n1); graph->edAddChild(n2); graph->edAddChild(n3); graph->edAddChild(n4);
  InputPort *i1_a=n1->edAddInputPort("a",Runtime::_tc_double); InputPort *i1_b=n1->edAddInputPort("b",Runtime::_tc_double); OutputPort *o1_j=n1->edAddOutputPort("j",Runtime::_tc_double); OutputPort *o1_k=n1->edAddOutputPort("k",Runtime::_tc_double);
  InputPort *i2_c=n2->edAddInputPort("c",Runtime::_tc_double); InputPort *i2_d=n2->edAddInputPort("d",Runtime::_tc_double); OutputPort *o2_f=n2->edAddOutputPort("f",Runtime::_tc_double);
  InputPort *i3_e=n3->edAddInputPort("e",Runtime::_tc_double); OutputPort *o3_g=n3->edAddOutputPort("g",Runtime::_tc_double); OutputPort *o3_h=n3->edAddOutputPort("h",Runtime::_tc_double);
  InputPort *i4_l=n4->edAddInputPort("l",Runtime::_tc_double); InputPort *i4_m=n4->edAddInputPort("m",Runtime::_tc_double); OutputPort *o4_i=n4->edAddOutputPort("i",Runtime::_tc_double);
  //Retrieving gates
  InGate *iN1=n1->getInGate(); InGate *iN2=n2->getInGate(); InGate *iN3=n3->getInGate(); InGate *iN4=n4->getInGate();
  OutGate *oN1=n1->getOutGate(); OutGate *oN2=n2->getOutGate(); OutGate *oN3=n3->getOutGate(); OutGate *oN4=n4->getOutGate();
  i1_a->edInit(1.2);
  i1_b->edInit(3.4);
  i2_d->edInit(7.);
  //DF                                                                      //CF
  graph->edAddLink(o1_j,i2_c);                                              graph->edAddLink(oN1,iN2);
  graph->edAddLink(o1_k,i3_e);                                              graph->edAddLink(oN1,iN3);
  graph->edAddLink(o2_f,i4_l);                                              graph->edAddLink(oN2,iN4);
  graph->edAddLink(o3_g,i4_m);                                              graph->edAddLink(oN3,iN4);
  Executor exe;
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)o4_i)->get()->getDoubleValue(),10.45,DBL_PRECISION_COMPARE );
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)o4_i)->get()->getDoubleValue(),10.45,DBL_PRECISION_COMPARE );
  Executor exe2;
  Bloc *clonedGraph=(Bloc *)graph->clone(0);
  exe2.RunW(clonedGraph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)clonedGraph->getOutputPort("T4.i"))->get()->getDoubleValue(),10.45,DBL_PRECISION_COMPARE );
  delete clonedGraph;
  bool testOfCycleSucceed=false;
  try
    { graph->edAddLink(oN4,iN1); }
  catch(YACS::Exception &e)
    { if(strcmp(e.what(),"Cycle has been detected")==0)
      testOfCycleSucceed=true; }
  CPPUNIT_ASSERT(testOfCycleSucceed);
  delete graph;
}

//test multi level graphs
void EngineIntegrationTest::testBloc3()
{
  Bloc *toto=new Bloc("toto");
  Bloc *tata=new Bloc("tata");
  Bloc *titi=new Bloc("titi");
  ToyNode *n2=new ToyNode("T2"); ToyNode *n3=new ToyNode("T3"); ToyNode *n4=new ToyNode("T4"); ToyNode *n5=new ToyNode("T5"); ToyNode *n6=new ToyNode("T6");
  ToyNode *n7=new ToyNode("T7"); ToyNode *n8=new ToyNode("T8");
  toto->edAddChild(titi); titi->edAddChild(tata); titi->edAddChild(n3); toto->edAddChild(n2); tata->edAddChild(n4); tata->edAddChild(n5); tata->edAddChild(n6);
  titi->edAddChild(n7); titi->edAddChild(n8);
  CPPUNIT_ASSERT( toto->edAddCFLink(n2,titi) );
  CPPUNIT_ASSERT( !toto->edAddCFLink(n2,titi) );
  toto->edAddCFLink(n3,tata); toto->edAddCFLink(n5,n4); titi->edAddCFLink(n7,n8); titi->edAddCFLink(tata,n8);
  //
  InputPort *i2_a=n2->edAddInputPort("a",Runtime::_tc_double); OutputPort *o2_a=n2->edAddOutputPort("a",Runtime::_tc_double); OutputPort *o2_b=n2->edAddOutputPort("b",Runtime::_tc_double);
  InputPort *i3_a=n3->edAddInputPort("a",Runtime::_tc_double); OutputPort *o3_a=n3->edAddOutputPort("a",Runtime::_tc_double); OutputPort *o3_b=n3->edAddOutputPort("b",Runtime::_tc_double);
  InputPort *i4_a=n4->edAddInputPort("a",Runtime::_tc_double); InputPort *i4_b=n4->edAddInputPort("b",Runtime::_tc_double); OutputPort *o4_a=n4->edAddOutputPort("a",Runtime::_tc_double); OutputPort *o4_b=n4->edAddOutputPort("b",Runtime::_tc_double);
  InputPort *i5_a=n5->edAddInputPort("a",Runtime::_tc_double); InputPort *i5_b=n5->edAddInputPort("b",Runtime::_tc_double); OutputPort *o5_a=n5->edAddOutputPort("a",Runtime::_tc_double);
  InputPort *i6_a=n6->edAddInputPort("a",Runtime::_tc_double); InputPort *i6_b=n6->edAddInputPort("b",Runtime::_tc_double); OutputPort *o6_a=n6->edAddOutputPort("a",Runtime::_tc_double);
  InputPort *i7_a=n7->edAddInputPort("a",Runtime::_tc_double); OutputPort *o7_a=n7->edAddOutputPort("a",Runtime::_tc_double);
  InputPort *i8_a=n8->edAddInputPort("a",Runtime::_tc_double); InputPort *i8_b=n8->edAddInputPort("b",Runtime::_tc_double); OutputPort *o8_a=n8->edAddOutputPort("a",Runtime::_tc_double);
  //
  toto->edAddLink(o2_a,i3_a);  toto->edAddLink(o3_a,i5_a); toto->edAddLink(o2_b,i5_b); toto->edAddLink(o2_a,i4_b);
  toto->edAddLink(o3_b,i6_a); toto->edAddLink(o6_a,i8_a); toto->edAddLink(o7_a,i8_b);
  //
  i2_a->edInit(1.2);
  i6_b->edInit(7.);
  i4_a->edInit(7.);
  i7_a->edInit(3.);
  CPPUNIT_ASSERT_EQUAL( 1, toto->getNumberOfCFLinks() ); CPPUNIT_ASSERT_EQUAL( 3, titi->getNumberOfCFLinks() );
  CPPUNIT_ASSERT_EQUAL( 1, tata->getNumberOfCFLinks() );
  Executor exe;
  exe.RunW(toto);
  CPPUNIT_ASSERT_EQUAL(toto->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(titi->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(tata->getState(),YACS::DONE);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)o6_a)->get()->getDoubleValue(),7.3,DBL_PRECISION_COMPARE );
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)o4_a)->get()->getDoubleValue(),3.8,DBL_PRECISION_COMPARE );
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)o8_a)->get()->getDoubleValue(),10.3,DBL_PRECISION_COMPARE );
  exe.RunW(toto);
  CPPUNIT_ASSERT_EQUAL(toto->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(titi->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(tata->getState(),YACS::DONE);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)o6_a)->get()->getDoubleValue(),7.3,DBL_PRECISION_COMPARE );
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)o4_a)->get()->getDoubleValue(),3.8,DBL_PRECISION_COMPARE );
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)o8_a)->get()->getDoubleValue(),10.3,DBL_PRECISION_COMPARE );
  Bloc *clonedGraph=(Bloc *)toto->clone(0);
  Bloc *titiCloned=(Bloc *)titi->clone(0);//to check internal links only cpy
  CPPUNIT_ASSERT_EQUAL(4,(int)titiCloned->getSetOfInternalLinks().size());
  CPPUNIT_ASSERT_EQUAL(7,(int)clonedGraph->getSetOfInternalLinks().size());
  delete titiCloned;
  titi->edRemoveChild(n7);
  delete n7;
  CPPUNIT_ASSERT_EQUAL( toto->getNumberOfCFLinks(),1 ); CPPUNIT_ASSERT_EQUAL( titi->getNumberOfCFLinks(),2 );
  CPPUNIT_ASSERT_EQUAL( tata->getNumberOfCFLinks(),1 );
  titi->edRemoveChild(tata);
  delete tata;
  CPPUNIT_ASSERT_EQUAL( 1, toto->getNumberOfCFLinks() ); CPPUNIT_ASSERT_EQUAL( 0, titi->getNumberOfCFLinks() );
  delete toto;
  Executor exe2;
  exe2.RunW(clonedGraph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)clonedGraph->getOutputPort("titi.tata.T6.a"))->get()->getDoubleValue(),7.3,DBL_PRECISION_COMPARE );
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)clonedGraph->getOutputPort("titi.tata.T4.a"))->get()->getDoubleValue(),3.8,DBL_PRECISION_COMPARE );
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)clonedGraph->getOutputPort("titi.T8.a"))->get()->getDoubleValue(),10.3,DBL_PRECISION_COMPARE );
  delete clonedGraph;
}

/*!
 * Test for cross hierarchy detection when defining a hierarchy.
 */
void EngineIntegrationTest::testBloc4()
{
  Bloc *graph=new Bloc("Graph");
  Bloc *g1=new Bloc("g1");
  Bloc *g2=new Bloc("g2");
  
  graph->edAddChild(g1);
  g1->edAddChild(g2);
  CPPUNIT_ASSERT_THROW(g2->edAddChild(graph),YACS::Exception);
  delete graph;
}

void EngineIntegrationTest::testForLoop1()
{
  TypeCode *tc_double    = Runtime::_tc_double;
  TypeCode *tc_int       = Runtime::_tc_int;
  LinkInfo info(LinkInfo::ALL_DONT_STOP);
  ToyNode *n1=new ToyNode("T1");
  InputPort *i11=n1->edAddInputPort("i11",tc_double);
  i11->edInit(3.14);
  InputPort *i12=n1->edAddInputPort("i12",tc_double);
  i12->edInit(2.78);
  InputPort *i13=n1->edAddInputPort("i13",tc_double);
  i13->edInit(7.177);
  OutputPort *o1=n1->edAddOutputPort("o1",tc_double);
  OutputPort *o2=n1->edGetNbOfInputsOutputPort();
  Bloc *graph=new Bloc("Graph");
  graph->edAddChild(n1);
  Bloc *titi=new Bloc("titi");
  ForLoop *loop=new ForLoop("toto");
  ToyNode *n2=new ToyNode("T2");
  titi->edAddChild(n2);
  InputPort *i21=n2->edAddInputPort("i1",tc_double);
  InputPort *i22=n2->edAddInputPort("i2",tc_double);
  i22->edInit(2.1);
  OutputPort *o21=n2->edAddOutputPort("o1",tc_double);
  loop->edSetNode(titi);
  graph->edAddChild(loop);
  InputPort *iNbTimes=loop->edGetNbOfTimesInputPort();
  graph->edAddLink(o2,iNbTimes);
  graph->edAddLink(o1,i21);
  graph->edAddCFLink(n1,loop);
  graph->edAddLink(o21,i21);
  graph->checkConsistency(info);
  DEBTRACE(info.areWarningsOrErrors());
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  Executor exe;
  exe.RunW(graph);
  DEBTRACE(((OutputToyPort*)n2->getOutputPort("o1"))->get()->getDoubleValue());
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)n2->getOutputPort("o1"))->get()->getDoubleValue(),19.397, DBL_PRECISION_COMPARE);
  DEBTRACE(loop->getNbOfTurns());
  CPPUNIT_ASSERT_EQUAL(loop->getNbOfTurns(),3);
  DEBTRACE(loop->getState());
  CPPUNIT_ASSERT_EQUAL(loop->getState(),YACS::DONE);
  DEBTRACE(graph->getState());
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  exe.RunW(graph);
  DEBTRACE(((OutputToyPort*)n2->getOutputPort("o1"))->get()->getDoubleValue());
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)n2->getOutputPort("o1"))->get()->getDoubleValue(),19.397, DBL_PRECISION_COMPARE);
  DEBTRACE(loop->getNbOfTurns());
  CPPUNIT_ASSERT_EQUAL(loop->getNbOfTurns(),3);
  DEBTRACE(loop->getState());
  CPPUNIT_ASSERT_EQUAL(loop->getState(),YACS::DONE);
  DEBTRACE(graph->getState());
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  Bloc *clonedGraph=(Bloc *)graph->clone(0);
  delete graph;
  Executor exe2;
  clonedGraph->checkConsistency(info);
  DEBTRACE(info.areWarningsOrErrors());
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe2.RunW(clonedGraph);
  DEBTRACE(((OutputToyPort*)clonedGraph->getOutputPort("toto.titi.T2.o1"))->get()->getDoubleValue());
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)clonedGraph->getOutputPort("toto.titi.T2.o1"))->get()->getDoubleValue(),19.397, DBL_PRECISION_COMPARE);
  DEBTRACE(((Loop *)clonedGraph->getChildByName("toto"))->getNbOfTurns());
  CPPUNIT_ASSERT_EQUAL(((Loop *)clonedGraph->getChildByName("toto"))->getNbOfTurns(),3);
  DEBTRACE(clonedGraph->getChildByName("toto")->getState());
  CPPUNIT_ASSERT_EQUAL(clonedGraph->getChildByName("toto")->getState(),YACS::DONE);
  DEBTRACE(clonedGraph->getState());
  CPPUNIT_ASSERT_EQUAL(clonedGraph->getState(),YACS::DONE);
  delete clonedGraph;
}

void EngineIntegrationTest::testForLoop2()
{
  TypeCode *tc_double    = Runtime::_tc_double;
  TypeCode *tc_int       = Runtime::_tc_int;
  LinkInfo info(LinkInfo::ALL_DONT_STOP);
  ToyNode *n1=new ToyNode("T1");
  InputPort *i11=n1->edAddInputPort("i11",tc_double);
  i11->edInit(3.14);
  InputPort *i12=n1->edAddInputPort("i12",tc_double);
  i12->edInit(2.78);
  InputPort *i13=n1->edAddInputPort("i13",tc_double);
  i13->edInit(7.177);
  OutputPort *o1=n1->edAddOutputPort("o1",tc_double);
  OutputPort *o2=n1->edGetNbOfInputsOutputPort();
  Bloc *graph=new Bloc("Graph");
  graph->edAddChild(n1);
  ForLoop *loop=new ForLoop("toto");
  ToyNode *n2=new ToyNode("T2");
  InputPort *i21=n2->edAddInputPort("i1",tc_double);
  InputPort *i22=n2->edAddInputPort("i2",tc_double);
  i22->edInit(2.1);
  OutputPort *o21=n2->edAddOutputPort("o1",tc_double);
  loop->edSetNode(n2);
  graph->edAddChild(loop);
  InputPort *iNbTimes=loop->edGetNbOfTimesInputPort();
  graph->edAddLink(o2,iNbTimes);
  graph->edAddLink(o1,i21);
  graph->edAddCFLink(n1,loop);
  graph->edAddLink(o21,i21);
  graph->checkConsistency(info);
  DEBTRACE(info.areWarningsOrErrors());
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  Executor exe;
  exe.RunW(graph);
  DEBTRACE(((OutputToyPort*)n2->getOutputPort("o1"))->get()->getDoubleValue());
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)n2->getOutputPort("o1"))->get()->getDoubleValue(),19.397, DBL_PRECISION_COMPARE);
  DEBTRACE(loop->getNbOfTurns());
  CPPUNIT_ASSERT_EQUAL(loop->getNbOfTurns(),3);
  DEBTRACE(loop->getState());
  CPPUNIT_ASSERT_EQUAL(loop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)n2->getOutputPort("o1"))->get()->getDoubleValue(),19.397, DBL_PRECISION_COMPARE);
  CPPUNIT_ASSERT_EQUAL(loop->getNbOfTurns(),3);
  CPPUNIT_ASSERT_EQUAL(loop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  delete graph;
}

//Test on loop when 0 turn of forloop is done
void EngineIntegrationTest::testForLoop3()
{
  TypeCode *tc_double    = Runtime::_tc_double;
  TypeCode *tc_int       = Runtime::_tc_int;
  LinkInfo info(LinkInfo::ALL_DONT_STOP);
  ToyNode *n1=new ToyNode("T1");
  InputPort *i11=n1->edAddInputPort("i11",tc_double);
  i11->edInit(3.14);
  InputPort *i12=n1->edAddInputPort("i12",tc_double);
  i12->edInit(2.78);
  OutputPort *o11=n1->edAddOutputPort("o1",tc_double);
  OutputPort *o12=n1->edGetNbOfInputsOutputPort();
  ToyNode *n2=new ToyNode("T2");
  InputPort *i21=n2->edAddInputPort("i1",tc_double);
  InputPort *i22=n2->edAddInputPort("i2",tc_double);
  i22->edInit(2.1);
  OutputPort *o21=n2->edAddOutputPort("o1",tc_double);
  ToyNode *n3=new ToyNode("T3");
  InputPort *i31=n3->edAddInputPort("i1",tc_double);
  OutputPort *o31=n3->edAddOutputPort("o1",tc_double);
  ForLoop *loop=new ForLoop("toto");
  loop->edSetNode(n2);
  Bloc *graph=new Bloc("Graph");
  graph->edAddChild(loop);
  graph->edAddChild(n1);
  graph->edAddChild(n3);
  graph->edAddCFLink(n1,loop);
  graph->edAddCFLink(loop,n3);
  graph->edAddLink(o11,i21);
  graph->edAddLink(o12,loop->edGetNbOfTimesInputPort());
  graph->edAddLink(o21,i21);
  graph->edAddLink(o21,i31);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  Executor exe;
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)graph->getOutputPort("T3.o1"))->get()->getDoubleValue(),10.12, DBL_PRECISION_COMPARE);
  CPPUNIT_ASSERT_EQUAL(loop->getNbOfTurns(),2);
  CPPUNIT_ASSERT_EQUAL(loop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n1->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n3->getState(),YACS::DONE);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)graph->getOutputPort("T3.o1"))->get()->getDoubleValue(),10.12, DBL_PRECISION_COMPARE);
  CPPUNIT_ASSERT_EQUAL(loop->getNbOfTurns(),2);
  CPPUNIT_ASSERT_EQUAL(loop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n1->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n3->getState(),YACS::DONE);
  //Let's go to test 0 cycle whithout any back dependancies : normal behaviour
  n1->edRemovePort(i11);
  n1->edRemovePort(i12);
  graph->edRemoveLink(o21,i31);
  graph->edAddLink(o11,i31);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe.RunW(graph);
  CPPUNIT_ASSERT_EQUAL(loop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(loop->getNbOfTurns(),0);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n1->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n3->getState(),YACS::DONE);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe.RunW(graph);
  CPPUNIT_ASSERT_EQUAL(loop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(loop->getNbOfTurns(),0);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n1->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n3->getState(),YACS::DONE);
  Bloc *clonedGraph=(Bloc *)graph->clone(0);
  clonedGraph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  Executor exe2;
  exe2.RunW(clonedGraph);
  CPPUNIT_ASSERT_EQUAL(((Loop *)clonedGraph->getChildByName("toto"))->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(((Loop *)clonedGraph->getChildByName("toto"))->getNbOfTurns(),0);
  CPPUNIT_ASSERT_EQUAL(clonedGraph->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(clonedGraph->getChildByName("T1")->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(clonedGraph->getChildByName("T3")->getState(),YACS::DONE);
  delete clonedGraph;
  //
  graph->edRemoveLink(o11,i31);
  graph->edAddLink(o21,i31);//this link obliges that loops run at least 1 time : which is not the case
  exe.RunW(graph);
  CPPUNIT_ASSERT_EQUAL(loop->getState(),YACS::ERROR);
  CPPUNIT_ASSERT_EQUAL(loop->getNbOfTurns(),0);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::FAILED);
  CPPUNIT_ASSERT_EQUAL(n1->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n3->getState(),YACS::FAILED);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe.RunW(graph);
  CPPUNIT_ASSERT_EQUAL(loop->getState(),YACS::ERROR);
  CPPUNIT_ASSERT_EQUAL(loop->getNbOfTurns(),0);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::FAILED);
  CPPUNIT_ASSERT_EQUAL(n1->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n3->getState(),YACS::FAILED);
  clonedGraph=(Bloc *)graph->clone(0);
  clonedGraph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe2.RunW(clonedGraph);
  CPPUNIT_ASSERT_EQUAL(((Loop *)clonedGraph->getChildByName("toto"))->getState(),YACS::ERROR);
  CPPUNIT_ASSERT_EQUAL(((Loop *)clonedGraph->getChildByName("toto"))->getNbOfTurns(),0);
  CPPUNIT_ASSERT_EQUAL(clonedGraph->getState(),YACS::FAILED);
  CPPUNIT_ASSERT_EQUAL(clonedGraph->getChildByName("T1")->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(clonedGraph->getChildByName("T3")->getState(),YACS::FAILED);
  delete clonedGraph;
  
  //retrieves back state and retest
  graph->edRemoveLink(o21,i31);
  graph->edAddLink(o11,i31);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe.RunW(graph);
  CPPUNIT_ASSERT_EQUAL(loop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(loop->getNbOfTurns(),0);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n1->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n3->getState(),YACS::DONE);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe.RunW(graph);
  CPPUNIT_ASSERT_EQUAL(loop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(loop->getNbOfTurns(),0);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n1->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n3->getState(),YACS::DONE);
  //Retrieves initial graph and test again
  graph->edRemoveLink(o11,i31);
  graph->edAddLink(o21,i31);
  i11=n1->edAddInputPort("i11",tc_double);
  i11->edInit(3.14);
  i12=n1->edAddInputPort("i12",tc_double);
  i12->edInit(2.78);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)graph->getOutputPort("T3.o1"))->get()->getDoubleValue(),10.12, DBL_PRECISION_COMPARE);
  CPPUNIT_ASSERT_EQUAL(loop->getNbOfTurns(),2);
  CPPUNIT_ASSERT_EQUAL(loop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n1->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n3->getState(),YACS::DONE);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)graph->getOutputPort("T3.o1"))->get()->getDoubleValue(),10.12, DBL_PRECISION_COMPARE);
  CPPUNIT_ASSERT_EQUAL(loop->getNbOfTurns(),2);
  CPPUNIT_ASSERT_EQUAL(loop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n1->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n3->getState(),YACS::DONE);
  delete graph;
}

void EngineIntegrationTest::testForLoop4()
{
  LinkInfo info(LinkInfo::ALL_DONT_STOP);
  Bloc *graph=new Bloc("Graph");
  ToyNode *n1=new ToyNode("T1"); graph->edAddChild(n1);
  ForLoop *loop=new ForLoop("loop"); graph->edAddChild(loop); graph->edAddCFLink(n1,loop);
  ToyNode *n2=new ToyNode("T2"); loop->edSetNode(n2);
  n1->edAddInputPort("i11",Runtime::_tc_double)->edInit(3.);
  n1->edAddInputPort("i12",Runtime::_tc_double)->edInit(4.);
  n1->edAddInputPort("i13",Runtime::_tc_double)->edInit(5.);
  OutputPort *o11=n1->edAddOutputPort("o11",Runtime::_tc_double);
  InputPort *i21=n2->edAddInputPort("i21",Runtime::_tc_double);
  n2->edAddInputPort("i22",Runtime::_tc_double)->edInit(2.);
  OutputPort *o21=n2->edAddOutputPort("o21",Runtime::_tc_double);
  graph->edAddLink(o11,i21);
  graph->edAddLink(o21,i21);
  graph->edAddLink(n1->edGetNbOfInputsOutputPort(),loop->edGetNbOfTimesInputPort());
  Bloc *graph2=(Bloc *)graph->clone(0);
  loop->edGetNbOfTimesInputPort()->edInit(0);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  Executor exe;
  exe.RunW(graph);
  CPPUNIT_ASSERT_EQUAL(loop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(loop->getNbOfTurns(),3);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)graph->getOutputPort("loop.T2.o21"))->get()->getDoubleValue(),18., DBL_PRECISION_COMPARE);
  exe.RunW(graph);
  CPPUNIT_ASSERT_EQUAL(loop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(loop->getNbOfTurns(),3);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)graph->getOutputPort("loop.T2.o21"))->get()->getDoubleValue(),18., DBL_PRECISION_COMPARE);
  Bloc *blocInt=new Bloc("blocInt");
  graph->edRemoveChild(loop);
  blocInt->edAddChild(loop);
  graph->edAddChild(blocInt);
  graph->edAddCFLink(n1,blocInt);
  graph->edAddLink(o11,i21);
  graph->edAddLink(n1->edGetNbOfInputsOutputPort(),loop->edGetNbOfTimesInputPort());
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe.RunW(graph);
  CPPUNIT_ASSERT_EQUAL(loop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(3,loop->getNbOfTurns());
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)graph->getOutputPort("blocInt.loop.T2.o21"))->get()->getDoubleValue(),18., DBL_PRECISION_COMPARE);
  exe.RunW(graph);
  CPPUNIT_ASSERT_EQUAL(loop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(3,loop->getNbOfTurns());
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)graph->getOutputPort("blocInt.loop.T2.o21"))->get()->getDoubleValue(),18., DBL_PRECISION_COMPARE);
  graph->edRemoveLink(n1->edGetNbOfInputsOutputPort(),loop->edGetNbOfTimesInputPort());
  exe.RunW(graph);
  CPPUNIT_ASSERT_EQUAL(loop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(0,loop->getNbOfTurns());
  exe.RunW(graph);
  CPPUNIT_ASSERT_EQUAL(loop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(0,loop->getNbOfTurns());
  graph2->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe.RunW(graph2);
  ForLoop *loop2=(ForLoop *)graph2->getChildByName("loop");
  CPPUNIT_ASSERT_EQUAL(loop2->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(graph2->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(loop2->getNbOfTurns(),3);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)graph2->getOutputPort("loop.T2.o21"))->get()->getDoubleValue(),18., DBL_PRECISION_COMPARE);
  exe.RunW(graph2);
  CPPUNIT_ASSERT_EQUAL(loop2->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(graph2->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(loop2->getNbOfTurns(),3);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)graph2->getOutputPort("loop.T2.o21"))->get()->getDoubleValue(),18., DBL_PRECISION_COMPARE);
  Bloc *blocInt2=new Bloc("blocInt");
  n1=(ToyNode *)graph2->getChildByName("T1");
  graph2->edRemoveChild(loop2);
  blocInt2->edAddChild(loop2);
  graph2->edAddChild(blocInt2);
  graph2->edAddCFLink(n1,blocInt2);
  o11=graph2->getOutputPort("T1.o11");
  i21=graph2->getInputPort("blocInt.loop.T2.i21");
  graph2->edAddLink(o11,i21);
  graph2->edAddLink(n1->edGetNbOfInputsOutputPort(),loop2->edGetNbOfTimesInputPort());
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe.RunW(graph);
  graph2->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe.RunW(graph2);
  CPPUNIT_ASSERT_EQUAL(loop2->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(graph2->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(3,loop2->getNbOfTurns());
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)graph2->getOutputPort("blocInt.loop.T2.o21"))->get()->getDoubleValue(),18., DBL_PRECISION_COMPARE);
  exe.RunW(graph2);
  CPPUNIT_ASSERT_EQUAL(loop2->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(graph2->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(3,loop2->getNbOfTurns());
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)graph2->getOutputPort("blocInt.loop.T2.o21"))->get()->getDoubleValue(),18., DBL_PRECISION_COMPARE);
  delete graph2;
  delete graph;
}

//multi loop inclusion
void EngineIntegrationTest::testForLoop5()
{
  LinkInfo info(LinkInfo::ALL_DONT_STOP);
  Bloc *graph=new Bloc("Graph");
  ToyNode *n1=new ToyNode("T1"); graph->edAddChild(n1);
  Bloc *b1=new Bloc("b1"); graph->edAddChild(b1); graph->edAddCFLink(n1,b1);
  ForLoop *loop2=new ForLoop("loop2"); b1->edAddChild(loop2);
  Bloc *b3=new Bloc("b3"); loop2->edSetNode(b3);
  ForLoop *loop4=new ForLoop("loop4"); b3->edAddChild(loop4);
  Bloc *b5=new Bloc("b5"); loop4->edSetNode(b5);
  ToyNode *n2=new ToyNode("T2"); b5->edAddChild(n2);
  n1->edAddInputPort("i11",Runtime::_tc_double)->edInit(3.);
  n1->edAddInputPort("i12",Runtime::_tc_double)->edInit(4.);
  n1->edAddInputPort("i13",Runtime::_tc_double)->edInit(5.);
  OutputPort *o11=n1->edAddOutputPort("o11",Runtime::_tc_double);
  graph->edAddLink(n1->edGetNbOfInputsOutputPort(),loop2->edGetNbOfTimesInputPort());
  loop4->edGetNbOfTimesInputPort()->edInit(2);
  n2->edAddInputPort("i22",Runtime::_tc_double)->edInit(7.);
  InputPort *i21=n2->edAddInputPort("i21",Runtime::_tc_double);
  OutputPort *o21=n2->edAddOutputPort("o21",Runtime::_tc_double);
  graph->edAddLink(o21,i21);
  graph->edAddLink(o11,i21);
  Executor exe;
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe.RunW(graph);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)loop2->getOutputPort("b3.loop4.b5.T2.o21"))->get()->getDoubleValue(),54., DBL_PRECISION_COMPARE);
  CPPUNIT_ASSERT_EQUAL(3,loop2->getNbOfTurns());
  CPPUNIT_ASSERT_EQUAL(2,loop4->getNbOfTurns());
  exe.RunW(graph);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)loop2->getOutputPort("b3.loop4.b5.T2.o21"))->get()->getDoubleValue(),54., DBL_PRECISION_COMPARE);
  CPPUNIT_ASSERT_EQUAL(3,loop2->getNbOfTurns());
  CPPUNIT_ASSERT_EQUAL(2,loop4->getNbOfTurns());
  delete graph;
}

void EngineIntegrationTest::testWhileLoop1()
{
  TypeCode *tc_double    = Runtime::_tc_double;
  TypeCode *tc_int       = Runtime::_tc_int;
  LinkInfo info(LinkInfo::ALL_DONT_STOP);
  ToyNode *n1=new ToyNode("T1");
  InputPort *i11=n1->edAddInputPort("i11",tc_double);
  i11->edInit(3.14);
  InputPort *i12=n1->edAddInputPort("i12",tc_double);
  i12->edInit(2.78);
  InputPort *i13=n1->edAddInputPort("i13",tc_double);
  i13->edInit(7.177);
  OutputPort *o1=n1->edAddOutputPort("o1",tc_double);
  LimitNode *l1=new LimitNode("L1");
  OutputPort *ol1s=l1->getSwitchPort();
  l1->setLimit(25.);
  Bloc *graph=new Bloc("Graph");
  //
  Bloc *inLoop=new Bloc("inLoop");
  ToyNode *n2=new ToyNode("T2");
  InputPort *i21=n2->edAddInputPort("i1",tc_double);
  InputPort *i22=n2->edAddInputPort("i2",tc_double);
  OutputPort *o21=n2->edAddOutputPort("o1",tc_double);
  i22->edInit(22.1006);
  LimitNode *l2=new LimitNode("L2");
  OutputPort *ol2s=l2->getSwitchPort();
  l2->setLimit(100.);
  inLoop->edAddChild(n2);
  inLoop->edAddChild(l2);
  WhileLoop *whileLoop=new WhileLoop("MyWhile");
  whileLoop->edSetNode(inLoop);
  inLoop->edAddCFLink(n2,l2);
  inLoop->edAddLink(o21,l2->getEntry());
  inLoop->edAddLink(l2->getCounterPort(),i21);
  //
  graph->edAddChild(n1);
  graph->edAddChild(l1);
  graph->edAddChild(whileLoop);
  graph->edAddCFLink(n1,l1);
  graph->edAddCFLink(l1,whileLoop);
  graph->edAddCFLink(n1,whileLoop);
  graph->edAddLink(l1->getSwitchPort(),whileLoop->edGetConditionPort());
  graph->edAddLink(l2->getSwitchPort(),whileLoop->edGetConditionPort());
  graph->edAddLink(o1,i21);
  graph->edAddLink(o1,l1->getEntry());
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  CPPUNIT_ASSERT(1==info.getNumberOfInfoLinks(I_CF_USELESS));
  graph->edRemoveCFLink(n1,whileLoop);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  CPPUNIT_ASSERT(0==info.getNumberOfInfoLinks(I_CF_USELESS));
 
  Executor exe;
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputLimitPort*)l2->getCounterPort())->get()->getDoubleValue(),207.0922, DBL_PRECISION_COMPARE);
  CPPUNIT_ASSERT_EQUAL(whileLoop->getNbOfTurns(),3);
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputLimitPort*)l2->getCounterPort())->get()->getDoubleValue(),207.0922, DBL_PRECISION_COMPARE);
  CPPUNIT_ASSERT_EQUAL(whileLoop->getNbOfTurns(),3);
  delete graph;
}

void EngineIntegrationTest::testSwitch()
{
  RuntimeForEngineIntegrationTest::setRuntime();
  Runtime *myRuntime = getRuntime();
  LinkInfo info(LinkInfo::ALL_DONT_STOP);
  TypeCode *tc_double    = Runtime::_tc_double;
  TypeCode *tc_int       = Runtime::_tc_int;
  Bloc *graph=new Bloc("Graph");

  ToyNode *n1=new ToyNode("T1");
  InputPort *i11=n1->edAddInputPort("i11",tc_double);
  i11->edInit(17.);
  OutputPort *o1=n1->edAddOutputPort("o1",tc_double);
  graph->edAddChild(n1);
  
  Switch *mySwitch=new Switch("mySwitch");
  graph->edAddChild(mySwitch);
  graph->edAddCFLink(n1,mySwitch);

  ToyNode *n2=new ToyNode("T2");
  InputPort *i21=n2->edAddInputPort("i21",tc_double);
  InputPort *i22=n2->edAddInputPort("i22",tc_double);
  OutputPort *o21=n2->edAddOutputPort("o21",tc_double);
  i22->edInit(1.);
  mySwitch->edSetNode(0,n2);
  graph->edAddLink(o1,i21);
  graph->edAddLink(n1->edGetNbOfInputsOutputPort(),mySwitch->edGetConditionPort());
  ToyNode *n3=new ToyNode("T3");
  InputPort *i31=n3->edAddInputPort("i31",tc_double);
  InputPort *i32=n3->edAddInputPort("i32",tc_double);
  OutputPort *o31=n3->edAddOutputPort("o31",tc_double);
  i32->edInit(2.);
  mySwitch->edSetNode(1,n3);
  graph->edAddLink(o1,i31);
  try
    {
      graph->edAddLink(o31,i21);
      CPPUNIT_ASSERT(0);
    }
  catch(Exception& e)
    {
      CPPUNIT_ASSERT(string(e.what())=="Switch::checkLinkPossibility : A link between 2 different cases of a same Switch requested -> Impossible");
    }
  ToyNode *n4=new ToyNode("T4");
  InputPort *i41=n4->edAddInputPort("i41",tc_double);
  InputPort *i42=n4->edAddInputPort("i42",tc_double);
  OutputPort *o41=n4->edAddOutputPort("o41",tc_double);
  i42->edInit(3.);
  mySwitch->edSetNode(2,n4);
  graph->edAddLink(o1,i41);
  Executor exe;
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(((OutputToyPort*)o31)->get()->getDoubleValue(),19., DBL_PRECISION_COMPARE);
  CPPUNIT_ASSERT_EQUAL(((OutputToyPort*)o21)->get(),(Any *)0);
  CPPUNIT_ASSERT_EQUAL(((OutputToyPort*)o41)->get(),(Any *)0);
  CPPUNIT_ASSERT_EQUAL(mySwitch->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  InputPort *i12=n1->edAddInputPort("i12",tc_double);
  i12->edInit(17.);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(((OutputToyPort*)o41)->get()->getDoubleValue(),37., DBL_PRECISION_COMPARE);
  CPPUNIT_ASSERT_EQUAL(((OutputToyPort*)o21)->get(),(Any *)0);
  CPPUNIT_ASSERT_EQUAL(((OutputToyPort*)o31)->get(),(Any *)0);
  CPPUNIT_ASSERT_EQUAL(mySwitch->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  delete graph;
}

void EngineIntegrationTest::testSwitch2()
{
  TypeCode *tc_double    = Runtime::_tc_double;
  TypeCode *tc_int       = Runtime::_tc_int;
  LinkInfo info(LinkInfo::ALL_DONT_STOP);
  Bloc *graph=new Bloc("Graph");

  ToyNode *n1=new ToyNode("T1");
  InputPort *i11=n1->edAddInputPort("i11",tc_double);
  i11->edInit(17.);
  OutputPort *o1=n1->edAddOutputPort("o1",tc_double);
  graph->edAddChild(n1);
  
  Switch *mySwitch=new Switch("mySwitch");
  graph->edAddChild(mySwitch);
  graph->edAddCFLink(n1,mySwitch);

  ToyNode *n2=new ToyNode("T2");
  InputPort *i21=n2->edAddInputPort("i21",tc_double);
  InputPort *i22=n2->edAddInputPort("i22",tc_double);
  OutputPort *o21=n2->edAddOutputPort("o21",tc_double);
  i22->edInit(1.);
  mySwitch->edSetNode(0,n2);
  graph->edAddLink(o1,i21);
  graph->edAddLink(n1->edGetNbOfInputsOutputPort(),mySwitch->edGetConditionPort());
  ToyNode *n3=new ToyNode("T3");
  InputPort *i31=n3->edAddInputPort("i31",tc_double);
  InputPort *i32=n3->edAddInputPort("i32",tc_double);
  OutputPort *o31=n3->edAddOutputPort("o31",tc_double);
  i32->edInit(2.);
  mySwitch->edSetNode(1,n3);
  graph->edAddLink(o1,i31);
  ToyNode *n4=new ToyNode("T4");
  InputPort *i41=n4->edAddInputPort("i41",tc_double);
  InputPort *i42=n4->edAddInputPort("i42",tc_double);
  OutputPort *o41=n4->edAddOutputPort("o41",tc_double);
  i42->edInit(3.);
  mySwitch->edSetNode(2,n4);
  graph->edAddLink(o1,i41);
  LimitNode *l1=new LimitNode("L1");
  l1->getEntry()->edInit(13.);
  l1->setLimit(100.);
  graph->edAddChild(l1);
  //
  Bloc *inLoop=new Bloc("inLoop");
  ToyNode *nn2=new ToyNode("TT2");
  inLoop->edAddChild(nn2);
  InputPort *ii21=nn2->edAddInputPort("ii1",tc_double);
  ii21->edInit(7.);
  InputPort *ii22=nn2->edAddInputPort("ii2",tc_double);
  OutputPort *oo21=nn2->edAddOutputPort("oo1",tc_double);
  LimitNode *l2=new LimitNode("L2");
  OutputPort *ool2s=l2->getSwitchPort();
  l2->setLimit(100.);
  inLoop->edAddChild(nn2);
  inLoop->edAddChild(l2);
  WhileLoop *whileLoop=new WhileLoop("MyWhile");
  whileLoop->edSetNode(inLoop);
  graph->edAddChild(whileLoop);
  graph->edAddCFLink(l1,whileLoop);
  graph->edAddLink(o21,ii22);
  graph->edAddLink(o31,ii22);
  graph->edAddLink(o41,ii22);
  inLoop->edAddCFLink(nn2,l2);
  graph->edAddCFLink(mySwitch,whileLoop);
  inLoop->edAddLink(oo21,l2->getEntry());
  inLoop->edAddLink(l2->getCounterPort(),ii21);
  graph->edAddLink(l1->getSwitchPort(),whileLoop->edGetConditionPort());
  graph->edAddLink(l2->getSwitchPort(),whileLoop->edGetConditionPort());
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  graph->edRemoveLink(o31,ii22);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT( 1==info.getNumberOfErrLinks(E_ALL) );
  CPPUNIT_ASSERT( 1==info.getNumberOfErrLinks(E_UNCOMPLETE_SW) );
  CPPUNIT_ASSERT(info.getErrRepr()=="For link to ii2 of node TT2 the cases of switch node named mySwitch do not define links for following cases ids :1 \n");
  graph->edRemoveLink(o41,ii22);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT( 1==info.getNumberOfErrLinks(E_ALL) );
  CPPUNIT_ASSERT( 1==info.getNumberOfErrLinks(E_UNCOMPLETE_SW) );
  CPPUNIT_ASSERT(info.getErrRepr()=="For link to ii2 of node TT2 the cases of switch node named mySwitch do not define links for following cases ids :1 2 \n");
  graph->edAddLink(o31,ii22);
  graph->edAddLink(o41,ii22);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  Executor exe;
  DEBTRACE("Run graph");
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(((OutputToyPort*)l2->getCounterPort())->get()->getDoubleValue(),161., DBL_PRECISION_COMPARE);
  CPPUNIT_ASSERT_EQUAL(3,whileLoop->getNbOfTurns());
  CPPUNIT_ASSERT_EQUAL(whileLoop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  DEBTRACE("Run graph");
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(((OutputToyPort*)l2->getCounterPort())->get()->getDoubleValue(),161., DBL_PRECISION_COMPARE);
  CPPUNIT_ASSERT_EQUAL(3,whileLoop->getNbOfTurns());
  CPPUNIT_ASSERT_EQUAL(whileLoop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  //default
  ToyNode *n5=new ToyNode("T5");
  InputPort *i51=n5->edAddInputPort("i51",tc_double);
  InputPort *i52=n5->edAddInputPort("i52",tc_double);
  OutputPort *o51=n5->edAddOutputPort("o51",tc_double);
  i52->edInit(4.);
  mySwitch->edSetDefaultNode(n5);
  graph->edAddLink(o1,i51);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT( 1==info.getNumberOfErrLinks(E_ALL) );
  CPPUNIT_ASSERT( 1==info.getNumberOfErrLinks(E_UNCOMPLETE_SW) );
  CPPUNIT_ASSERT(info.getErrRepr()=="For link to ii2 of node TT2 the cases of switch node named mySwitch do not define links for following cases ids :default \n");
  graph->edAddLink(o51,ii22);
  DEBTRACE("Run graph");
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(((OutputToyPort*)l2->getCounterPort())->get()->getDoubleValue(),161., DBL_PRECISION_COMPARE);
  CPPUNIT_ASSERT_EQUAL(3,whileLoop->getNbOfTurns());
  CPPUNIT_ASSERT_EQUAL(whileLoop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  DEBTRACE("Run graph");
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(((OutputToyPort*)l2->getCounterPort())->get()->getDoubleValue(),161., DBL_PRECISION_COMPARE);
  CPPUNIT_ASSERT_EQUAL(3,whileLoop->getNbOfTurns());
  CPPUNIT_ASSERT_EQUAL(whileLoop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  try
    {
      mySwitch->edReleaseCase(4);
      CPPUNIT_ASSERT(0);
    }
  catch(Exception& e)
    {
      CPPUNIT_ASSERT(std::string(e.what())=="Switch::edReleaseCase : the case # 4 is not set yet.");
    }
  mySwitch->edReleaseCase(1);
  DEBTRACE("Run graph");
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(((OutputToyPort*)l2->getCounterPort())->get()->getDoubleValue(),175., DBL_PRECISION_COMPARE);
  CPPUNIT_ASSERT_EQUAL(3,whileLoop->getNbOfTurns());
  CPPUNIT_ASSERT_EQUAL(whileLoop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  DEBTRACE("Run graph");
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(((OutputToyPort*)l2->getCounterPort())->get()->getDoubleValue(),175., DBL_PRECISION_COMPARE);
  CPPUNIT_ASSERT_EQUAL(3,whileLoop->getNbOfTurns());
  CPPUNIT_ASSERT_EQUAL(whileLoop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  //now test when unexpected value is recieved and no default node specifies
  mySwitch->edReleaseDefaultNode();
  DEBTRACE("Run graph");
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe.RunW(graph);
  CPPUNIT_ASSERT_EQUAL(whileLoop->getState(),YACS::FAILED);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::FAILED);
  CPPUNIT_ASSERT_EQUAL(mySwitch->getState(),YACS::ERROR);
  DEBTRACE("Run graph");
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe.RunW(graph);
  CPPUNIT_ASSERT_EQUAL(whileLoop->getState(),YACS::FAILED);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::FAILED);
  CPPUNIT_ASSERT_EQUAL(mySwitch->getState(),YACS::ERROR);
  //retrieving back state
  mySwitch->edSetDefaultNode(n5);
  graph->edAddLink(o1,i51);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT( 1==info.getNumberOfErrLinks(E_ALL) );
  CPPUNIT_ASSERT( 1==info.getNumberOfErrLinks(E_UNCOMPLETE_SW) );
  CPPUNIT_ASSERT(info.getErrRepr()=="For link to ii2 of node TT2 the cases of switch node named mySwitch do not define links for following cases ids :default \n");
  graph->edAddLink(o51,ii22);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  DEBTRACE("Run graph");
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(((OutputToyPort*)l2->getCounterPort())->get()->getDoubleValue(),175., DBL_PRECISION_COMPARE);
  CPPUNIT_ASSERT_EQUAL(3,whileLoop->getNbOfTurns());
  CPPUNIT_ASSERT_EQUAL(whileLoop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  DEBTRACE("Run graph");
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(((OutputToyPort*)l2->getCounterPort())->get()->getDoubleValue(),175., DBL_PRECISION_COMPARE);
  CPPUNIT_ASSERT_EQUAL(3,whileLoop->getNbOfTurns());
  CPPUNIT_ASSERT_EQUAL(whileLoop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  //and finally retrieving initial state
  mySwitch->edSetNode(1,n3);
  graph->edAddLink(o1,i31);
  graph->edAddLink(o31,ii22);
  DEBTRACE("Run graph");
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(((OutputToyPort*)l2->getCounterPort())->get()->getDoubleValue(),161., DBL_PRECISION_COMPARE);
  CPPUNIT_ASSERT_EQUAL(3,whileLoop->getNbOfTurns());
  CPPUNIT_ASSERT_EQUAL(whileLoop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  DEBTRACE("Run graph");
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(((OutputToyPort*)l2->getCounterPort())->get()->getDoubleValue(),161., DBL_PRECISION_COMPARE);
  CPPUNIT_ASSERT_EQUAL(3,whileLoop->getNbOfTurns());
  CPPUNIT_ASSERT_EQUAL(whileLoop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  Bloc *clonedGraph=(Bloc *)graph->clone(0);
  delete graph;
  Executor exe2;
  clonedGraph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe2.RunW(clonedGraph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(((OutputToyPort*)clonedGraph->getOutPort("MyWhile.inLoop.L2.SwitchPort2"))->get()->getDoubleValue(),161., DBL_PRECISION_COMPARE);
  CPPUNIT_ASSERT_EQUAL(3,((WhileLoop *)clonedGraph->getChildByName("MyWhile"))->getNbOfTurns());
  CPPUNIT_ASSERT_EQUAL(((WhileLoop *)clonedGraph->getChildByName("MyWhile"))->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(clonedGraph->getState(),YACS::DONE);
  delete clonedGraph;
}

/**
 * multi switch inclusion - link update
 */
void EngineIntegrationTest::testSwitch3()
{
  LinkInfo info(LinkInfo::ALL_DONT_STOP);
  Bloc *graph=new Bloc("Graph");
  Switch *mySwitch1=new Switch("mySwitch1");
  graph->edAddChild(mySwitch1);
  Switch *mySwitch2=new Switch("mySwitch2");
  mySwitch1->edSetNode(2,mySwitch2);
  ToyNode *n1=new ToyNode("T1"); Bloc *blocForT1=new Bloc("blocForT1"); blocForT1->edAddChild(n1);
  ToyNode *n2=new ToyNode("T2"); Bloc *blocForT2=new Bloc("blocForT2"); blocForT2->edAddChild(n2); mySwitch1->edSetNode(3,blocForT2);graph->edAddChild(blocForT1); n2->edAddInputPort("i22",Runtime::_tc_double)->edInit(2.);
  ToyNode *n3=new ToyNode("T3"); mySwitch2->edSetNode(2,n3); n3->edAddInputPort("i32",Runtime::_tc_double)->edInit(3.);
  ToyNode *n4=new ToyNode("T4"); Bloc *blocForT4=new Bloc("blocForT4"); blocForT4->edAddChild(n4); mySwitch2->edSetNode(5,blocForT4); n4->edAddInputPort("i42",Runtime::_tc_double)->edInit(4.);
  ToyNode *n5=new ToyNode("T5"); mySwitch2->edSetNode(8,n5); n5->edAddInputPort("i52",Runtime::_tc_double)->edInit(5.);
  ToyNode *n6=new ToyNode("T6"); graph->edAddChild(n6);
  graph->edAddCFLink(blocForT1,mySwitch1);
  graph->edAddCFLink(mySwitch1,n6);
  mySwitch2->getInputPort("select")->edInit(5);
  mySwitch1->getInputPort("select")->edInit(2);
  InputPort *i11=n1->edAddInputPort("i11",Runtime::_tc_double); OutputPort *o11=n1->edAddOutputPort("o11",Runtime::_tc_double);
  InputPort *i21=n2->edAddInputPort("i21",Runtime::_tc_double); OutputPort *o21=n2->edAddOutputPort("o21",Runtime::_tc_double);
  InputPort *i31=n3->edAddInputPort("i31",Runtime::_tc_double); OutputPort *o31=n3->edAddOutputPort("o31",Runtime::_tc_double);
  InputPort *i41=n4->edAddInputPort("i41",Runtime::_tc_double); OutputPort *o41=n4->edAddOutputPort("o41",Runtime::_tc_double);
  InputPort *i51=n5->edAddInputPort("i51",Runtime::_tc_double); OutputPort *o51=n5->edAddOutputPort("o51",Runtime::_tc_double);
  InputPort *i61=n6->edAddInputPort("i61",Runtime::_tc_double); OutputPort *o61=n6->edAddOutputPort("o61",Runtime::_tc_double);
  i11->edInit(17.);
  graph->edAddLink(o11,i21); graph->edAddLink(o11,i31); graph->edAddLink(o11,i41); graph->edAddLink(o11,i51);
  graph->edAddLink(o21,i61);
  graph->edAddLink(o31,i61);
  graph->edAddLink(o41,i61);
  graph->edAddLink(o51,i61);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  CPPUNIT_ASSERT_EQUAL(1,n6->getInGate()->getNumberOfBackLinks());
  CPPUNIT_ASSERT_EQUAL(1,i61->edGetNumberOfLinks());// <-- important
  CPPUNIT_ASSERT_EQUAL(4,o11->edGetNumberOfOutLinks()); 
  CPPUNIT_ASSERT_EQUAL(1,i21->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,i31->edGetNumberOfLinks());
  graph->edRemoveLink(o21,i61);
  CPPUNIT_ASSERT_EQUAL(1,i61->edGetNumberOfLinks());// <-- important
  CPPUNIT_ASSERT_EQUAL(0,o21->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i31->edGetNumberOfLinks());
  graph->edAddLink(o21,i61);
  graph->edRemoveLink(o31,i61);
  graph->edRemoveLink(o41,i61);
  graph->edRemoveLink(o51,i61);
  CPPUNIT_ASSERT_EQUAL(1,i61->edGetNumberOfLinks());// <-- important
  graph->edRemoveLink(o21,i61);
  CPPUNIT_ASSERT_EQUAL(0,i61->edGetNumberOfLinks());// <-- important
  graph->edAddLink(o21,i61);
  CPPUNIT_ASSERT_EQUAL(1,i61->edGetNumberOfLinks());
  graph->edAddLink(o41,i61);
  CPPUNIT_ASSERT_EQUAL(1,i61->edGetNumberOfLinks());
  graph->checkConsistency(info);
  CPPUNIT_ASSERT( 1==info.getNumberOfErrLinks(E_ALL) );
  CPPUNIT_ASSERT( 1==info.getNumberOfErrLinks(E_UNCOMPLETE_SW) );
  CPPUNIT_ASSERT(info.getErrRepr()=="For link to i61 of node T6 the cases of switch node named mySwitch2 do not define links for following cases ids :2 8 \n");
  graph->edAddLink(o31,i61);
  graph->edAddLink(o51,i61);
  CPPUNIT_ASSERT_EQUAL(1,i61->edGetNumberOfLinks());
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  Executor exe;
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)graph->getOutputPort("T6.o61"))->get()->getDoubleValue(),21., DBL_PRECISION_COMPARE);
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)graph->getOutputPort("T6.o61"))->get()->getDoubleValue(),21., DBL_PRECISION_COMPARE);
  mySwitch1->getInputPort("select")->edInit(3);
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)graph->getOutputPort("T6.o61"))->get()->getDoubleValue(),19., DBL_PRECISION_COMPARE);
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)graph->getOutputPort("T6.o61"))->get()->getDoubleValue(),19., DBL_PRECISION_COMPARE);
  delete graph;
}

void EngineIntegrationTest::testWhileLoop2()//Test of 0 turn of loop
{
  //first test without any link to condition port on start of loop that is to say WhileLoop is considered has while and NOT dowhile.
  LinkInfo info(LinkInfo::ALL_DONT_STOP);
  TypeCode *tc_double    = Runtime::_tc_double;
  TypeCode *tc_int       = Runtime::_tc_int;
  ToyNode *n1=new ToyNode("T1");
  InputPort *i11=n1->edAddInputPort("i11",tc_double);
  i11->edInit(3.14);
  InputPort *i12=n1->edAddInputPort("i12",tc_double);
  i12->edInit(2.78);
  OutputPort *o11=n1->edAddOutputPort("o1",tc_double);
  ToyNode *n2=new ToyNode("T2");
  InputPort *i21=n2->edAddInputPort("i1",tc_double);
  InputPort *i22=n2->edAddInputPort("i2",tc_double);
  i22->edInit(22.1);
  OutputPort *o21=n2->edAddOutputPort("o1",tc_double);
  ToyNode *n3=new ToyNode("T3");
  InputPort *i31=n3->edAddInputPort("i1",tc_double);
  OutputPort *o31=n3->edAddOutputPort("o1",tc_double);
  LimitNode *l2=new LimitNode("L2");
  OutputPort *ol2s=l2->getSwitchPort();
  l2->setLimit(100.);
  Bloc *titi=new Bloc("titi");
  titi->edAddChild(n2);
  titi->edAddChild(l2);
  WhileLoop *loop=new WhileLoop("toto");
  loop->edSetNode(titi);
  Bloc *graph=new Bloc("Graph");
  graph->edAddChild(loop);
  graph->edAddChild(n1);
  graph->edAddChild(n3);
  graph->edAddCFLink(n1,loop);
  graph->edAddCFLink(loop,n3);
  titi->edAddCFLink(n2,l2);
  graph->edAddLink(o11,i21);
  graph->edAddLink(o21,l2->getEntry());
  graph->edAddLink(ol2s,loop->edGetConditionPort());
  graph->edAddLink(o21,i21);
  graph->edAddLink(o21,i31);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  Executor exe;
  DEBTRACE("Run graph");
  exe.RunW(graph);
  DEBTRACE("After Run graph");
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)graph->getOutputPort("T3.o1"))->get()->getDoubleValue(),72.22, DBL_PRECISION_COMPARE);
  CPPUNIT_ASSERT_EQUAL(loop->getNbOfTurns(),3);
  CPPUNIT_ASSERT_EQUAL(loop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n1->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n2->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n3->getState(),YACS::DONE);
  DEBTRACE("Run graph");
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)graph->getOutputPort("T3.o1"))->get()->getDoubleValue(), 72.22, DBL_PRECISION_COMPARE);
  CPPUNIT_ASSERT_EQUAL(loop->getNbOfTurns(),3);
  CPPUNIT_ASSERT_EQUAL(loop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n1->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n2->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n3->getState(),YACS::DONE);
  //Link on conditionPort of while same behaviour as ahead
  LimitNode *l1=new LimitNode("L1");
  graph->edAddChild(l1);
  OutputPort *ol1s=l1->getSwitchPort();
  l1->setLimit(100.);
  l1->getEntry()->edInit(12.);
  graph->edAddCFLink(l1,loop);
  graph->edAddLink(ol1s,loop->edGetConditionPort());
  DEBTRACE("Run graph");
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)graph->getOutputPort("T3.o1"))->get()->getDoubleValue(),72.22, DBL_PRECISION_COMPARE);
  CPPUNIT_ASSERT_EQUAL(loop->getNbOfTurns(),3);
  CPPUNIT_ASSERT_EQUAL(loop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n1->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n2->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n3->getState(),YACS::DONE);
  DEBTRACE("Run graph");
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)graph->getOutputPort("T3.o1"))->get()->getDoubleValue(),72.22, DBL_PRECISION_COMPARE);
  CPPUNIT_ASSERT_EQUAL(loop->getNbOfTurns(),3);
  CPPUNIT_ASSERT_EQUAL(loop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n1->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n2->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n3->getState(),YACS::DONE);
  //Now no turn in while loop...
  l1->getEntry()->edInit(120.);
  DEBTRACE("Run graph");
  exe.RunW(graph);
  CPPUNIT_ASSERT_EQUAL(loop->getState(),YACS::ERROR);
  CPPUNIT_ASSERT_EQUAL(loop->getNbOfTurns(),0);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::FAILED);
  CPPUNIT_ASSERT_EQUAL(n1->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(l1->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n3->getState(),YACS::FAILED);
  DEBTRACE("Run graph");
  exe.RunW(graph);
  CPPUNIT_ASSERT_EQUAL(loop->getState(),YACS::ERROR);
  CPPUNIT_ASSERT_EQUAL(loop->getNbOfTurns(),0);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::FAILED);
  CPPUNIT_ASSERT_EQUAL(n1->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(l1->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n3->getState(),YACS::FAILED);
  //like before
  l1->getEntry()->edInit(12.);
  DEBTRACE("Run graph");
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)graph->getOutputPort("T3.o1"))->get()->getDoubleValue(),72.22, DBL_PRECISION_COMPARE);
  CPPUNIT_ASSERT_EQUAL(loop->getNbOfTurns(),3);
  CPPUNIT_ASSERT_EQUAL(loop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n1->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n2->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n3->getState(),YACS::DONE);
  DEBTRACE("Run graph");
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)graph->getOutputPort("T3.o1"))->get()->getDoubleValue(),72.22, DBL_PRECISION_COMPARE);
  CPPUNIT_ASSERT_EQUAL(loop->getNbOfTurns(),3);
  CPPUNIT_ASSERT_EQUAL(loop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n1->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n2->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n3->getState(),YACS::DONE);
  //now 0 turn of loop but no back dependancies
  l1->getEntry()->edInit(125.);
  graph->edRemoveLink(o21,i31);
  graph->edAddLink(o11,i31);//this
  DEBTRACE("Run graph");
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)graph->getOutputPort("T3.o1"))->get()->getDoubleValue(),5.92, DBL_PRECISION_COMPARE);
  CPPUNIT_ASSERT_EQUAL(loop->getNbOfTurns(),0);
  CPPUNIT_ASSERT_EQUAL(loop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n1->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n3->getState(),YACS::DONE);
  DEBTRACE("Run graph");
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)graph->getOutputPort("T3.o1"))->get()->getDoubleValue(),5.92, DBL_PRECISION_COMPARE);
  CPPUNIT_ASSERT_EQUAL(loop->getNbOfTurns(),0);
  CPPUNIT_ASSERT_EQUAL(loop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n1->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n3->getState(),YACS::DONE);
  //
  graph->edRemoveChild(l1);
  delete l1;
  graph->getInputPort("toto.condition")->edInit(false);
  DEBTRACE("Run graph");
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)graph->getOutputPort("T3.o1"))->get()->getDoubleValue(),5.92, DBL_PRECISION_COMPARE);
  CPPUNIT_ASSERT_EQUAL(loop->getNbOfTurns(),0);
  CPPUNIT_ASSERT_EQUAL(loop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n1->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n3->getState(),YACS::DONE);
  DEBTRACE("Run graph");
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)graph->getOutputPort("T3.o1"))->get()->getDoubleValue(),5.92, DBL_PRECISION_COMPARE);
  CPPUNIT_ASSERT_EQUAL(loop->getNbOfTurns(),0);
  CPPUNIT_ASSERT_EQUAL(loop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n1->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n3->getState(),YACS::DONE);
  //
  graph->getInputPort("toto.condition")->edInit(true);
  DEBTRACE("Run graph");
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)graph->getOutputPort("T3.o1"))->get()->getDoubleValue(),5.92, DBL_PRECISION_COMPARE);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)l2->getCounterPort())->get()->getDoubleValue(),150.36, DBL_PRECISION_COMPARE);
  CPPUNIT_ASSERT_EQUAL(loop->getNbOfTurns(),3);
  CPPUNIT_ASSERT_EQUAL(loop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(graph->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n1->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n3->getState(),YACS::DONE);
  delete graph;
}

void EngineIntegrationTest::testEdInitOnLoops()
{
  ForLoop *loop=new ForLoop("totoloop");
  Bloc *titi=new Bloc("titi");
  ToyNode *n2=new ToyNode("T2");
  titi->edAddChild(n2);
  loop->edSetNode(titi);
  InputPort *i21=n2->edAddInputPort("i1",Runtime::_tc_double);
  InputPort *i22=n2->edAddInputPort("i2",Runtime::_tc_double);
  i21->edInit(2.1);
  i22->edInit(4.3);
  OutputPort *o21=n2->edAddOutputPort("o1",Runtime::_tc_double);
  titi->edAddLink(o21,i21);
  loop->edGetNbOfTimesInputPort()->edInit(4);
  Executor exe;
  exe.RunW(loop);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)loop->getOutputPort("titi.T2.o1"))->get()->getDoubleValue(),19.3, DBL_PRECISION_COMPARE);
  CPPUNIT_ASSERT_EQUAL(loop->getNbOfTurns(),4);
  CPPUNIT_ASSERT_EQUAL(loop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n2->getState(),YACS::DONE);
  exe.RunW(loop);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)loop->getOutputPort("titi.T2.o1"))->get()->getDoubleValue(),19.3, DBL_PRECISION_COMPARE);
  CPPUNIT_ASSERT_EQUAL(loop->getNbOfTurns(),4);
  CPPUNIT_ASSERT_EQUAL(loop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n2->getState(),YACS::DONE);
  loop->edGetNbOfTimesInputPort()->edInit(5);
  exe.RunW(loop);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( ((OutputToyPort*)loop->getOutputPort("titi.T2.o1"))->get()->getDoubleValue(),23.6, DBL_PRECISION_COMPARE);
  CPPUNIT_ASSERT_EQUAL(loop->getNbOfTurns(),5);
  CPPUNIT_ASSERT_EQUAL(loop->getState(),YACS::DONE);
  CPPUNIT_ASSERT_EQUAL(n2->getState(),YACS::DONE);
  loop->edGetNbOfTimesInputPort()->edInit(0);
  exe.RunW(loop);
  CPPUNIT_ASSERT_EQUAL(loop->getNbOfTurns(),0);
  CPPUNIT_ASSERT_EQUAL(loop->getState(),YACS::DONE);
  exe.RunW(loop);
  CPPUNIT_ASSERT_EQUAL(loop->getNbOfTurns(),0);
  CPPUNIT_ASSERT_EQUAL(loop->getState(),YACS::DONE);
  delete loop;
}

/**
 * This test peers at link deletion forwarding for switch
 */
void EngineIntegrationTest::testLinkUpdate1()
{
  Bloc *graph=new Bloc("Graph");
  LinkInfo info(LinkInfo::ALL_DONT_STOP);
  ToyNode *n1=new ToyNode("T1");
  InputPort *i11=n1->edAddInputPort("i11",Runtime::_tc_double);
  i11->edInit(17.);
  OutputPort *o1=n1->edAddOutputPort("o1",Runtime::_tc_double);
  graph->edAddChild(n1);
  
  Switch *mySwitch=new Switch("mySwitch");
  graph->edAddChild(mySwitch);
  graph->edAddCFLink(n1,mySwitch);

  ToyNode *n2=new ToyNode("T2");
  InputPort *i21=n2->edAddInputPort("i21",Runtime::_tc_double);
  InputPort *i22=n2->edAddInputPort("i22",Runtime::_tc_double);
  OutputPort *o21=n2->edAddOutputPort("o21",Runtime::_tc_double);
  OutputPort *o22=n2->edAddOutputPort("o22",Runtime::_tc_double);
  i22->edInit(1.);
  mySwitch->edSetNode(0,n2);
  graph->edAddLink(o1,i21);
  graph->edAddLink(n1->edGetNbOfInputsOutputPort(),mySwitch->edGetConditionPort());
  ToyNode *n3=new ToyNode("T3");
  Bloc *Bn3=new Bloc("Bn3");
  Bn3->edAddChild(n3);
  InputPort *i31=n3->edAddInputPort("i31",Runtime::_tc_double);
  InputPort *i32=n3->edAddInputPort("i32",Runtime::_tc_double);
  OutputPort *o31=n3->edAddOutputPort("o31",Runtime::_tc_double);
  OutputPort *o32=n3->edAddOutputPort("o32",Runtime::_tc_double);
  i32->edInit(2.);
  mySwitch->edSetNode(1,Bn3);
  graph->edAddLink(o1,i31);
  //
  ToyNode *n4=new ToyNode("T4");
  InputPort *i41=n4->edAddInputPort("i41",Runtime::_tc_double);
  InputPort *i42=n4->edAddInputPort("i42",Runtime::_tc_double);
  OutputPort *o41=n4->edAddOutputPort("o41",Runtime::_tc_double);
  i42->edInit(3.14);
  graph->edAddChild(n4);
  graph->edAddCFLink(mySwitch,n4);
  graph->edAddLink(o21,i41);
  graph->edAddLink(o31,i41);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  CPPUNIT_ASSERT_EQUAL(1,o21->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,o31->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i41->edGetNumberOfLinks());// <-- important
  graph->edRemoveLink(o21,i41);
  CPPUNIT_ASSERT_EQUAL(0,o21->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,o31->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i41->edGetNumberOfLinks());// <-- important
  graph->edRemoveLink(o31,i41);//normally implies collector inputport deletion
  CPPUNIT_ASSERT_EQUAL(0,o21->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(0,o31->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(0,i41->edGetNumberOfLinks());// <-- important
  //Test on forward update for deletion
  graph->edAddLink(o21,i41);
  graph->edAddLink(o31,i41);
  CPPUNIT_ASSERT_EQUAL(1,o21->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,o31->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i41->edGetNumberOfLinks());
  n2->edRemovePort(o21);
  CPPUNIT_ASSERT_EQUAL(1,o31->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i41->edGetNumberOfLinks());
  n3->edRemovePort(o31);
  CPPUNIT_ASSERT_EQUAL(0,i41->edGetNumberOfLinks());// <-- important
  o21=n2->edAddOutputPort("o21",Runtime::_tc_double);
  o31=n3->edAddOutputPort("o31",Runtime::_tc_double);
  //
  graph->edAddLink(o21,i41);
  graph->edAddLink(o31,i41);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  CPPUNIT_ASSERT_EQUAL(1,o21->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,o31->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i41->edGetNumberOfLinks());
  ToyNode *n5=new ToyNode("T5");
  InputPort *i51=n5->edAddInputPort("i51",Runtime::_tc_double);
  InputPort *i52=n5->edAddInputPort("i52",Runtime::_tc_double);
  OutputPort *o51=n5->edAddOutputPort("o51",Runtime::_tc_double);
  OutputPort *o52=n5->edAddOutputPort("o52",Runtime::_tc_double);
  i52->edInit(7.);
  mySwitch->edSetNode(17,n5);
  graph->edAddLink(o1,i51);
  CPPUNIT_ASSERT_EQUAL(1,o21->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,o31->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(0,o51->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i41->edGetNumberOfLinks());
  graph->edAddLink(o51,i41);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  CPPUNIT_ASSERT_EQUAL(1,o21->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,o31->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,o51->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i41->edGetNumberOfLinks());
  graph->edRemoveLink(o31,i41);
  CPPUNIT_ASSERT_EQUAL(1,o21->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(0,o31->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,o51->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i41->edGetNumberOfLinks());
  graph->edAddLink(o31,i41);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  graph->edAddLink(o22,i42);
  graph->edAddLink(o32,i42);
  graph->edAddLink(o52,i42);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  CPPUNIT_ASSERT_EQUAL(1,o21->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,o31->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,o51->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i41->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,o22->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,o32->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,o52->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i42->edGetNumberOfLinks());
  ToyNode *n6=new ToyNode("T6");
  InputPort *i61=n6->edAddInputPort("i61",Runtime::_tc_double);
  InputPort *i62=n6->edAddInputPort("i62",Runtime::_tc_double);
  OutputPort *o61=n6->edAddOutputPort("o61",Runtime::_tc_double);
  graph->edAddChild(n6);
  graph->edAddCFLink(mySwitch,n6);
  CPPUNIT_ASSERT_EQUAL(1,n6->getInGate()->getNumberOfBackLinks());
  CPPUNIT_ASSERT_EQUAL(0,i62->edGetNumberOfLinks());
  graph->edAddLink(o22,i62);
  CPPUNIT_ASSERT_EQUAL(1,i62->edGetNumberOfLinks());
  graph->edAddLink(o32,i62);
  graph->edAddLink(o52,i62);
  CPPUNIT_ASSERT_EQUAL(2,o22->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(2,o32->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(2,o52->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i42->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,i62->edGetNumberOfLinks());
  graph->edRemoveChild(n6);//normally implies collector inputport deletion
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  CPPUNIT_ASSERT_EQUAL(1,o22->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,o32->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,o52->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i42->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(0,i62->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(0,n6->getInGate()->getNumberOfBackLinks());
  graph->edAddChild(n6);
  graph->edAddCFLink(mySwitch,n6);
  CPPUNIT_ASSERT_EQUAL(1,n6->getInGate()->getNumberOfBackLinks());
  graph->edAddLink(o22,i62);
  CPPUNIT_ASSERT_EQUAL(1,i62->edGetNumberOfLinks());
  graph->edAddLink(o32,i62);
  graph->edAddLink(o52,i62);
  CPPUNIT_ASSERT_EQUAL(2,o22->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(2,o32->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(2,o52->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i42->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,i62->edGetNumberOfLinks());
  n6->edRemovePort(i61);
  CPPUNIT_ASSERT_EQUAL(1,n6->getInGate()->getNumberOfBackLinks());
  CPPUNIT_ASSERT_EQUAL(2,o22->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(2,o32->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(2,o52->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i42->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,i62->edGetNumberOfLinks());
  n6->edRemovePort(i62);//normally implies collector inputport deletion
  CPPUNIT_ASSERT_EQUAL(1,n6->getInGate()->getNumberOfBackLinks());
  CPPUNIT_ASSERT_EQUAL(1,o22->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,o32->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,o52->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i42->edGetNumberOfLinks());
  graph->edRemoveLink(o32,i42);
  CPPUNIT_ASSERT_EQUAL(1,n6->getInGate()->getNumberOfBackLinks());
  CPPUNIT_ASSERT_EQUAL(1,o22->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(0,o32->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,o52->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i42->edGetNumberOfLinks());
  n4->edRemovePort(i42);//normally implies collector inputport deletion
  CPPUNIT_ASSERT_EQUAL(0,o22->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(0,o32->edGetNumberOfOutLinks());
  //few forward link updates
  CPPUNIT_ASSERT_EQUAL(1,i21->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,i31->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,i51->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(3,o1->edGetNumberOfOutLinks());
  n1->edRemovePort(o1);
  CPPUNIT_ASSERT_EQUAL(0,i21->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(0,i31->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(0,i51->edGetNumberOfLinks());
  delete graph;
}

/**
 * Idem testLinkUpdate1 but for DS.
 */
void EngineIntegrationTest::testLinkUpdate1DS()
{
  Bloc *graph=new Bloc("Graph");
  LinkInfo info(LinkInfo::ALL_DONT_STOP);
  ToyNode *n1=new ToyNode("T1");
  InputDataStreamPort *i11=n1->edAddInputDataStreamPort("i11",Runtime::_tc_double);
  OutputDataStreamPort *o1=n1->edAddOutputDataStreamPort("o1",Runtime::_tc_double);
  graph->edAddChild(n1);
  
  Switch *mySwitch=new Switch("mySwitch");
  graph->edAddChild(mySwitch);
  graph->edAddCFLink(n1,mySwitch);

  ToyNode *n2=new ToyNode("T2");
  InputDataStreamPort *i21=n2->edAddInputDataStreamPort("i21",Runtime::_tc_double);
  InputDataStreamPort *i22=n2->edAddInputDataStreamPort("i22",Runtime::_tc_double);
  OutputDataStreamPort *o21=n2->edAddOutputDataStreamPort("o21",Runtime::_tc_double);
  OutputDataStreamPort *o22=n2->edAddOutputDataStreamPort("o22",Runtime::_tc_double);
  mySwitch->edSetNode(0,n2);
  graph->edAddLink(o1,i21);
  graph->edAddLink(n1->edGetNbOfInputsOutputPort(),mySwitch->edGetConditionPort());
  ToyNode *n3=new ToyNode("T3");
  Bloc *Bn3=new Bloc("Bn3");
  Bn3->edAddChild(n3);
  InputDataStreamPort *i31=n3->edAddInputDataStreamPort("i31",Runtime::_tc_double);
  InputDataStreamPort *i32=n3->edAddInputDataStreamPort("i32",Runtime::_tc_double);
  OutputDataStreamPort *o31=n3->edAddOutputDataStreamPort("o31",Runtime::_tc_double);
  OutputDataStreamPort *o32=n3->edAddOutputDataStreamPort("o32",Runtime::_tc_double);
  mySwitch->edSetNode(1,Bn3);
  graph->edAddLink(o1,i31);
  //
  ToyNode *n4=new ToyNode("T4");
  InputDataStreamPort *i41=n4->edAddInputDataStreamPort("i41",Runtime::_tc_double);
  InputDataStreamPort *i42=n4->edAddInputDataStreamPort("i42",Runtime::_tc_double);
  OutputDataStreamPort *o41=n4->edAddOutputDataStreamPort("o41",Runtime::_tc_double);
  graph->edAddChild(n4);
  graph->edAddCFLink(mySwitch,n4);
  graph->edAddLink(o21,i41);
  graph->edAddLink(o31,i41);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  CPPUNIT_ASSERT_EQUAL(1,o21->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,o31->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i41->edGetNumberOfLinks());// <-- important
  graph->edRemoveLink(o21,i41);
  CPPUNIT_ASSERT_EQUAL(0,o21->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,o31->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i41->edGetNumberOfLinks());// <-- important
  graph->edRemoveLink(o31,i41);//normally implies collector inputport deletion
  CPPUNIT_ASSERT_EQUAL(0,o21->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(0,o31->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(0,i41->edGetNumberOfLinks());// <-- important
  //Test on forward update for deletion
  graph->edAddLink(o21,i41);
  graph->edAddLink(o31,i41);
  CPPUNIT_ASSERT_EQUAL(1,o21->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,o31->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i41->edGetNumberOfLinks());
  n2->edRemovePort(o21);
  CPPUNIT_ASSERT_EQUAL(1,o31->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i41->edGetNumberOfLinks());
  n3->edRemovePort(o31);
  CPPUNIT_ASSERT_EQUAL(0,i41->edGetNumberOfLinks());// <-- important
  o21=n2->edAddOutputDataStreamPort("o21",Runtime::_tc_double);
  o31=n3->edAddOutputDataStreamPort("o31",Runtime::_tc_double);
  //
  graph->edAddLink(o21,i41);
  graph->edAddLink(o31,i41);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  CPPUNIT_ASSERT_EQUAL(1,o21->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,o31->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i41->edGetNumberOfLinks());
  ToyNode *n5=new ToyNode("T5");
  InputDataStreamPort *i51=n5->edAddInputDataStreamPort("i51",Runtime::_tc_double);
  InputDataStreamPort *i52=n5->edAddInputDataStreamPort("i52",Runtime::_tc_double);
  OutputDataStreamPort *o51=n5->edAddOutputDataStreamPort("o51",Runtime::_tc_double);
  OutputDataStreamPort *o52=n5->edAddOutputDataStreamPort("o52",Runtime::_tc_double);
  mySwitch->edSetNode(17,n5);
  graph->edAddLink(o1,i51);
  CPPUNIT_ASSERT_EQUAL(1,o21->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,o31->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(0,o51->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i41->edGetNumberOfLinks());
  graph->edAddLink(o51,i41);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  CPPUNIT_ASSERT_EQUAL(1,o21->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,o31->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,o51->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i41->edGetNumberOfLinks());
  graph->edRemoveLink(o31,i41);
  CPPUNIT_ASSERT_EQUAL(1,o21->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(0,o31->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,o51->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i41->edGetNumberOfLinks());
  graph->edAddLink(o31,i41);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  graph->edAddLink(o22,i42);
  graph->edAddLink(o32,i42);
  graph->edAddLink(o52,i42);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  CPPUNIT_ASSERT_EQUAL(1,o21->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,o31->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,o51->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i41->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,o22->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,o32->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,o52->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i42->edGetNumberOfLinks());
  ToyNode *n6=new ToyNode("T6");
  InputDataStreamPort *i61=n6->edAddInputDataStreamPort("i61",Runtime::_tc_double);
  InputDataStreamPort *i62=n6->edAddInputDataStreamPort("i62",Runtime::_tc_double);
  OutputDataStreamPort *o61=n6->edAddOutputDataStreamPort("o61",Runtime::_tc_double);
  graph->edAddChild(n6);
  graph->edAddCFLink(mySwitch,n6);
  CPPUNIT_ASSERT_EQUAL(1,n6->getInGate()->getNumberOfBackLinks());
  CPPUNIT_ASSERT_EQUAL(0,i62->edGetNumberOfLinks());
  graph->edAddLink(o22,i62);
  CPPUNIT_ASSERT_EQUAL(1,i62->edGetNumberOfLinks());
  graph->edAddLink(o32,i62);
  graph->edAddLink(o52,i62);
  CPPUNIT_ASSERT_EQUAL(2,o22->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(2,o32->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(2,o52->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i42->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,i62->edGetNumberOfLinks());
  graph->edRemoveChild(n6);//normally implies collector inputport deletion
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  CPPUNIT_ASSERT_EQUAL(1,o22->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,o32->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,o52->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i42->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(0,i62->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(0,n6->getInGate()->getNumberOfBackLinks());
  graph->edAddChild(n6);
  graph->edAddCFLink(mySwitch,n6);
  CPPUNIT_ASSERT_EQUAL(1,n6->getInGate()->getNumberOfBackLinks());
  graph->edAddLink(o22,i62);
  CPPUNIT_ASSERT_EQUAL(1,i62->edGetNumberOfLinks());
  graph->edAddLink(o32,i62);
  graph->edAddLink(o52,i62);
  CPPUNIT_ASSERT_EQUAL(2,o22->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(2,o32->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(2,o52->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i42->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,i62->edGetNumberOfLinks());
  n6->edRemovePort(i61);
  CPPUNIT_ASSERT_EQUAL(1,n6->getInGate()->getNumberOfBackLinks());
  CPPUNIT_ASSERT_EQUAL(2,o22->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(2,o32->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(2,o52->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i42->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,i62->edGetNumberOfLinks());
  n6->edRemovePort(i62);//normally implies collector inputport deletion
  CPPUNIT_ASSERT_EQUAL(1,n6->getInGate()->getNumberOfBackLinks());
  CPPUNIT_ASSERT_EQUAL(1,o22->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,o32->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,o52->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i42->edGetNumberOfLinks());
  graph->edRemoveLink(o32,i42);
  CPPUNIT_ASSERT_EQUAL(1,n6->getInGate()->getNumberOfBackLinks());
  CPPUNIT_ASSERT_EQUAL(1,o22->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(0,o32->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,o52->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i42->edGetNumberOfLinks());
  n4->edRemovePort(i42);//normally implies collector inputport deletion
  CPPUNIT_ASSERT_EQUAL(0,o22->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(0,o32->edGetNumberOfOutLinks());
  //few forward link updates
  CPPUNIT_ASSERT_EQUAL(1,i21->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,i31->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,i51->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(3,o1->edGetNumberOfOutLinks());
  n1->edRemovePort(o1);
  CPPUNIT_ASSERT_EQUAL(0,i21->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(0,i31->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(0,i51->edGetNumberOfLinks());
  delete graph;
}

// Less complex than testLinkUpdate1 only for Blocs update
void EngineIntegrationTest::testLinkUpdate2()
{
  Bloc *graph=new Bloc("Graph");

  ToyNode *n1=new ToyNode("T1");
  InputPort *i11=n1->edAddInputPort("i11",Runtime::_tc_double);
  InputPort *i12=n1->edAddInputPort("i12",Runtime::_tc_double);
  InputPort *i13=n1->edAddInputPort("i13",Runtime::_tc_double);
  InputPort *i14=n1->edAddInputPort("i14",Runtime::_tc_double);
  OutputPort *o11=n1->edAddOutputPort("o11",Runtime::_tc_double);
  OutputPort *o12=n1->edAddOutputPort("o12",Runtime::_tc_double);
  OutputPort *o13=n1->edAddOutputPort("o13",Runtime::_tc_double);
  graph->edAddChild(n1);
  //
  Bloc *toto=new Bloc("toto");
  graph->edAddChild(toto);
  graph->edAddCFLink(n1,toto);
  ToyNode *n2=new ToyNode("T2");
  toto->edAddChild(n2);
  InputPort *i21=n2->edAddInputPort("i21",Runtime::_tc_double);
  InputPort *i22=n2->edAddInputPort("i22",Runtime::_tc_double);
  InputPort *i23=n2->edAddInputPort("i23",Runtime::_tc_double);
  OutputPort *o21=n2->edAddOutputPort("o21",Runtime::_tc_double);
  OutputPort *o22=n2->edAddOutputPort("o22",Runtime::_tc_double);
  OutputPort *o23=n2->edAddOutputPort("o23",Runtime::_tc_double);
  ToyNode *n3=new ToyNode("T3");
  toto->edAddChild(n3);
  InputPort *i31=n3->edAddInputPort("i31",Runtime::_tc_double);
  InputPort *i32=n3->edAddInputPort("i32",Runtime::_tc_double);
  InputPort *i33=n3->edAddInputPort("i33",Runtime::_tc_double);
  InputPort *i34=n3->edAddInputPort("i34",Runtime::_tc_double);
  OutputPort *o31=n3->edAddOutputPort("o31",Runtime::_tc_double);
  OutputPort *o32=n3->edAddOutputPort("o32",Runtime::_tc_double);
  OutputPort *o33=n3->edAddOutputPort("o33",Runtime::_tc_double);
  Bloc *totoSub=new Bloc("totoSub");
  toto->edAddChild(totoSub);
  toto->edAddCFLink(n2,totoSub);
  toto->edAddCFLink(totoSub,n3);
  ToyNode *n4=new ToyNode("T4");
  totoSub->edAddChild(n4);
  InputPort *i41=n4->edAddInputPort("i41",Runtime::_tc_double);
  InputPort *i42=n4->edAddInputPort("i42",Runtime::_tc_double);
  InputPort *i43=n4->edAddInputPort("i43",Runtime::_tc_double);
  OutputPort *o41=n4->edAddOutputPort("o41",Runtime::_tc_double);
  OutputPort *o44=n4->edAddOutputPort("o44",Runtime::_tc_double);
  OutputPort *o43=n4->edAddOutputPort("o43",Runtime::_tc_double);
  ToyNode *n5=new ToyNode("T5");
  totoSub->edAddChild(n5);
  totoSub->edAddCFLink(n4,n5);
  InputPort *i51=n5->edAddInputPort("i51",Runtime::_tc_double);
  InputPort *i52=n5->edAddInputPort("i52",Runtime::_tc_double);
  InputPort *i53=n5->edAddInputPort("i53",Runtime::_tc_double);
  InputPort *i54=n5->edAddInputPort("i54",Runtime::_tc_double);
  OutputPort *o51=n5->edAddOutputPort("o51",Runtime::_tc_double);
  OutputPort *o52=n5->edAddOutputPort("o52",Runtime::_tc_double);
  OutputPort *o53=n5->edAddOutputPort("o53",Runtime::_tc_double);
  ToyNode *n6=new ToyNode("T6");
  totoSub->edAddChild(n6);
  totoSub->edAddCFLink(n4,n6);
  InputPort *i61=n6->edAddInputPort("i61",Runtime::_tc_double);
  InputPort *i62=n6->edAddInputPort("i62",Runtime::_tc_double);
  InputPort *i63=n6->edAddInputPort("i63",Runtime::_tc_double);
  OutputPort *o61=n6->edAddOutputPort("o61",Runtime::_tc_double);
  OutputPort *o62=n6->edAddOutputPort("o62",Runtime::_tc_double);
  ToyNode *n7=new ToyNode("T7");
  totoSub->edAddChild(n7);
  totoSub->edAddCFLink(n6,n7);
  totoSub->edAddCFLink(n5,n7);
  InputPort *i71=n7->edAddInputPort("i71",Runtime::_tc_double);
  InputPort *i72=n7->edAddInputPort("i72",Runtime::_tc_double);
  InputPort *i73=n7->edAddInputPort("i73",Runtime::_tc_double);
  OutputPort *o71=n7->edAddOutputPort("o71",Runtime::_tc_double);
  OutputPort *o72=n7->edAddOutputPort("o72",Runtime::_tc_double);
  //
  Bloc *titi=new Bloc("titi");
  ToyNode *n8=new ToyNode("T8");
  graph->edAddChild(titi);
  titi->edAddChild(n8);
  InputPort *i81=n8->edAddInputPort("i81",Runtime::_tc_double);
  InputPort *i82=n8->edAddInputPort("i82",Runtime::_tc_double);
  InputPort *i83=n8->edAddInputPort("i83",Runtime::_tc_double);
  OutputPort *o81=n8->edAddOutputPort("o81",Runtime::_tc_double);
  OutputPort *o82=n8->edAddOutputPort("o82",Runtime::_tc_double);
  OutputDataStreamPort *o83=n8->edAddOutputDataStreamPort("o83",Runtime::_tc_double);
  graph->edAddCFLink(toto,titi);
  ToyNode *n9=new ToyNode("T9");
  graph->edAddChild(n9);
  graph->edAddCFLink(toto,n9);
  InputPort *i91=n9->edAddInputPort("i91",Runtime::_tc_double);
  InputPort *i92=n9->edAddInputPort("i92",Runtime::_tc_double);
  InputPort *i93=n9->edAddInputPort("i93",Runtime::_tc_double);
  OutputPort *o91=n9->edAddOutputPort("o91",Runtime::_tc_double);
  OutputPort *o92=n9->edAddOutputPort("o92",Runtime::_tc_double);
  // Let's test links updates ...
  graph->edAddLink(o61,i81);
  graph->edAddLink(o61,i72);
  graph->edAddLink(o61,i92);
  graph->edAddLink(o61,i33);
  graph->edAddLink(o12,i21); graph->edAddLink(o12,i31); graph->edAddLink(o12,i71); graph->edAddLink(o12,i82); graph->edAddLink(o12,i91);
  CPPUNIT_ASSERT_EQUAL(5,o12->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i21->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,i31->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,i71->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,i82->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,i91->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(0,i32->edGetNumberOfLinks());
  //Test on link throw
  try
    {
      graph->edAddLink(o83,i51);
      CPPUNIT_ASSERT(0);
    }
  catch(Exception& e)
    {
      CPPUNIT_ASSERT(string(e.what())=="ComposedNode::checkLinkPossibility : Request for cross protocol link impossible.");
    }
  //
  CPPUNIT_ASSERT_EQUAL(4,o61->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i81->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,i72->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,i92->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,i33->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(0,i32->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(2,(int)toto->getSetOfLinksLeavingCurrentScope().size());
  CPPUNIT_ASSERT_EQUAL(3,(int)totoSub->getSetOfLinksLeavingCurrentScope().size());
  CPPUNIT_ASSERT_EQUAL(1,n3->getInGate()->getNumberOfBackLinks());
  CPPUNIT_ASSERT_EQUAL(1,titi->getInGate()->getNumberOfBackLinks());
  CPPUNIT_ASSERT_EQUAL(0,n8->getInGate()->getNumberOfBackLinks());
  graph->edRemoveChild(toto);
  CPPUNIT_ASSERT_EQUAL(2,o12->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(0,i21->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(0,i31->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(0,i71->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,i82->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,i91->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(0,i32->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,n3->getInGate()->getNumberOfBackLinks());
  CPPUNIT_ASSERT_EQUAL(0,titi->getInGate()->getNumberOfBackLinks());
  CPPUNIT_ASSERT_EQUAL(0,n8->getInGate()->getNumberOfBackLinks());
  CPPUNIT_ASSERT_EQUAL(2,o61->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(0,i81->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,i72->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(0,i92->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,i33->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(0,i32->edGetNumberOfLinks());
  toto->edRemoveChild(totoSub);
  CPPUNIT_ASSERT_EQUAL(0,n3->getInGate()->getNumberOfBackLinks());
  CPPUNIT_ASSERT_EQUAL(0,titi->getInGate()->getNumberOfBackLinks());
  CPPUNIT_ASSERT_EQUAL(0,n8->getInGate()->getNumberOfBackLinks());
  CPPUNIT_ASSERT_EQUAL(1,o61->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(0,i81->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,i72->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(0,i92->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(0,i33->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(0,i32->edGetNumberOfLinks());
  delete toto;
  delete totoSub;
  //
  delete graph;
}

/** 
 *idem testLinkUpdate2 but with DS
 */
void EngineIntegrationTest::testLinkUpdate2DS()
{
  Bloc *graph=new Bloc("Graph");

  ToyNode *n1=new ToyNode("T1");
  InputDataStreamPort *i11=n1->edAddInputDataStreamPort("i11",Runtime::_tc_double);
  InputDataStreamPort *i12=n1->edAddInputDataStreamPort("i12",Runtime::_tc_double);
  InputDataStreamPort *i13=n1->edAddInputDataStreamPort("i13",Runtime::_tc_double);
  InputDataStreamPort *i14=n1->edAddInputDataStreamPort("i14",Runtime::_tc_double);
  OutputDataStreamPort *o11=n1->edAddOutputDataStreamPort("o11",Runtime::_tc_double);
  OutputDataStreamPort *o12=n1->edAddOutputDataStreamPort("o12",Runtime::_tc_double);
  OutputDataStreamPort *o13=n1->edAddOutputDataStreamPort("o13",Runtime::_tc_double);
  graph->edAddChild(n1);
  //
  Bloc *toto=new Bloc("toto");
  graph->edAddChild(toto);
  graph->edAddCFLink(n1,toto);
  ToyNode *n2=new ToyNode("T2");
  toto->edAddChild(n2);
  InputDataStreamPort *i21=n2->edAddInputDataStreamPort("i21",Runtime::_tc_double);
  InputDataStreamPort *i22=n2->edAddInputDataStreamPort("i22",Runtime::_tc_double);
  InputDataStreamPort *i23=n2->edAddInputDataStreamPort("i23",Runtime::_tc_double);
  OutputDataStreamPort *o21=n2->edAddOutputDataStreamPort("o21",Runtime::_tc_double);
  OutputDataStreamPort *o22=n2->edAddOutputDataStreamPort("o22",Runtime::_tc_double);
  OutputDataStreamPort *o23=n2->edAddOutputDataStreamPort("o23",Runtime::_tc_double);
  ToyNode *n3=new ToyNode("T3");
  toto->edAddChild(n3);
  InputDataStreamPort *i31=n3->edAddInputDataStreamPort("i31",Runtime::_tc_double);
  InputDataStreamPort *i32=n3->edAddInputDataStreamPort("i32",Runtime::_tc_double);
  InputDataStreamPort *i33=n3->edAddInputDataStreamPort("i33",Runtime::_tc_double);
  InputDataStreamPort *i34=n3->edAddInputDataStreamPort("i34",Runtime::_tc_double);
  OutputDataStreamPort *o31=n3->edAddOutputDataStreamPort("o31",Runtime::_tc_double);
  OutputDataStreamPort *o32=n3->edAddOutputDataStreamPort("o32",Runtime::_tc_double);
  OutputDataStreamPort *o33=n3->edAddOutputDataStreamPort("o33",Runtime::_tc_double);
  Bloc *totoSub=new Bloc("totoSub");
  toto->edAddChild(totoSub);
  toto->edAddCFLink(n2,totoSub);
  toto->edAddCFLink(totoSub,n3);
  ToyNode *n4=new ToyNode("T4");
  totoSub->edAddChild(n4);
  InputDataStreamPort *i41=n4->edAddInputDataStreamPort("i41",Runtime::_tc_double);
  InputDataStreamPort *i42=n4->edAddInputDataStreamPort("i42",Runtime::_tc_double);
  InputDataStreamPort *i43=n4->edAddInputDataStreamPort("i43",Runtime::_tc_double);
  OutputDataStreamPort *o41=n4->edAddOutputDataStreamPort("o41",Runtime::_tc_double);
  OutputDataStreamPort *o44=n4->edAddOutputDataStreamPort("o44",Runtime::_tc_double);
  OutputDataStreamPort *o43=n4->edAddOutputDataStreamPort("o43",Runtime::_tc_double);
  ToyNode *n5=new ToyNode("T5");
  totoSub->edAddChild(n5);
  totoSub->edAddCFLink(n4,n5);
  InputDataStreamPort *i51=n5->edAddInputDataStreamPort("i51",Runtime::_tc_double);
  InputDataStreamPort *i52=n5->edAddInputDataStreamPort("i52",Runtime::_tc_double);
  InputDataStreamPort *i53=n5->edAddInputDataStreamPort("i53",Runtime::_tc_double);
  InputDataStreamPort *i54=n5->edAddInputDataStreamPort("i54",Runtime::_tc_double);
  OutputDataStreamPort *o51=n5->edAddOutputDataStreamPort("o51",Runtime::_tc_double);
  OutputDataStreamPort *o52=n5->edAddOutputDataStreamPort("o52",Runtime::_tc_double);
  OutputDataStreamPort *o53=n5->edAddOutputDataStreamPort("o53",Runtime::_tc_double);
  ToyNode *n6=new ToyNode("T6");
  totoSub->edAddChild(n6);
  totoSub->edAddCFLink(n4,n6);
  InputDataStreamPort *i61=n6->edAddInputDataStreamPort("i61",Runtime::_tc_double);
  InputDataStreamPort *i62=n6->edAddInputDataStreamPort("i62",Runtime::_tc_double);
  InputDataStreamPort *i63=n6->edAddInputDataStreamPort("i63",Runtime::_tc_double);
  OutputDataStreamPort *o61=n6->edAddOutputDataStreamPort("o61",Runtime::_tc_double);
  OutputDataStreamPort *o62=n6->edAddOutputDataStreamPort("o62",Runtime::_tc_double);
  ToyNode *n7=new ToyNode("T7");
  totoSub->edAddChild(n7);
  totoSub->edAddCFLink(n6,n7);
  totoSub->edAddCFLink(n5,n7);
  InputDataStreamPort *i71=n7->edAddInputDataStreamPort("i71",Runtime::_tc_double);
  InputDataStreamPort *i72=n7->edAddInputDataStreamPort("i72",Runtime::_tc_double);
  InputDataStreamPort *i73=n7->edAddInputDataStreamPort("i73",Runtime::_tc_double);
  OutputDataStreamPort *o71=n7->edAddOutputDataStreamPort("o71",Runtime::_tc_double);
  OutputDataStreamPort *o72=n7->edAddOutputDataStreamPort("o72",Runtime::_tc_double);
  //
  Bloc *titi=new Bloc("titi");
  ToyNode *n8=new ToyNode("T8");
  graph->edAddChild(titi);
  titi->edAddChild(n8);
  InputDataStreamPort *i81=n8->edAddInputDataStreamPort("i81",Runtime::_tc_double);
  InputDataStreamPort *i82=n8->edAddInputDataStreamPort("i82",Runtime::_tc_double);
  InputDataStreamPort *i83=n8->edAddInputDataStreamPort("i83",Runtime::_tc_double);
  OutputDataStreamPort *o81=n8->edAddOutputDataStreamPort("o81",Runtime::_tc_double);
  OutputDataStreamPort *o82=n8->edAddOutputDataStreamPort("o82",Runtime::_tc_double);
  graph->edAddCFLink(toto,titi);
  ToyNode *n9=new ToyNode("T9");
  graph->edAddChild(n9);
  graph->edAddCFLink(toto,n9);
  InputDataStreamPort *i91=n9->edAddInputDataStreamPort("i91",Runtime::_tc_double);
  InputDataStreamPort *i92=n9->edAddInputDataStreamPort("i92",Runtime::_tc_double);
  InputDataStreamPort *i93=n9->edAddInputDataStreamPort("i93",Runtime::_tc_double);
  OutputDataStreamPort *o91=n9->edAddOutputDataStreamPort("o91",Runtime::_tc_double);
  OutputDataStreamPort *o92=n9->edAddOutputDataStreamPort("o92",Runtime::_tc_double);
  // Let's test links updates ...
  graph->edAddLink(o61,i81);
  graph->edAddLink(o61,i72);
  graph->edAddLink(o61,i92);
  graph->edAddLink(o61,i33);
  graph->edAddLink(o12,i21); graph->edAddLink(o12,i31); graph->edAddLink(o12,i71); graph->edAddLink(o12,i82); graph->edAddLink(o12,i91);
  CPPUNIT_ASSERT_EQUAL(5,o12->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i21->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,i31->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,i71->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,i82->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,i91->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(0,i32->edGetNumberOfLinks());
  //
  CPPUNIT_ASSERT_EQUAL(4,o61->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i81->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,i72->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,i92->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,i33->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(0,i32->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(2,(int)toto->getSetOfLinksLeavingCurrentScope().size());
  CPPUNIT_ASSERT_EQUAL(3,(int)totoSub->getSetOfLinksLeavingCurrentScope().size());
  CPPUNIT_ASSERT_EQUAL(1,n3->getInGate()->getNumberOfBackLinks());
  CPPUNIT_ASSERT_EQUAL(1,titi->getInGate()->getNumberOfBackLinks());
  CPPUNIT_ASSERT_EQUAL(0,n8->getInGate()->getNumberOfBackLinks());
  graph->edRemoveChild(toto);
  CPPUNIT_ASSERT_EQUAL(2,o12->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(0,i21->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(0,i31->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(0,i71->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,i82->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,i91->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(0,i32->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,n3->getInGate()->getNumberOfBackLinks());
  CPPUNIT_ASSERT_EQUAL(0,titi->getInGate()->getNumberOfBackLinks());
  CPPUNIT_ASSERT_EQUAL(0,n8->getInGate()->getNumberOfBackLinks());
  CPPUNIT_ASSERT_EQUAL(2,o61->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(0,i81->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,i72->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(0,i92->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,i33->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(0,i32->edGetNumberOfLinks());
  toto->edRemoveChild(totoSub);
  CPPUNIT_ASSERT_EQUAL(0,n3->getInGate()->getNumberOfBackLinks());
  CPPUNIT_ASSERT_EQUAL(0,titi->getInGate()->getNumberOfBackLinks());
  CPPUNIT_ASSERT_EQUAL(0,n8->getInGate()->getNumberOfBackLinks());
  CPPUNIT_ASSERT_EQUAL(1,o61->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(0,i81->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,i72->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(0,i92->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(0,i33->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(0,i32->edGetNumberOfLinks());
  delete toto;
  delete totoSub;
  //
  delete graph;
}

void EngineIntegrationTest::testLinkUpdate3()
{
  Bloc *graph=new Bloc("Graph");
  WhileLoop *loop2=new WhileLoop("loop2");
  graph->edAddChild(loop2);
  LimitNode *n1=new LimitNode("T1"); graph->edAddChild(n1);
  OutputPort *o1s=n1->getSwitchPort();
  graph->edAddCFLink(n1,loop2);
  LimitNode *n2=new LimitNode("T2"); loop2->edSetNode(n2);
  OutputPort *o2s=n2->getSwitchPort();
  graph->edAddLink(o1s,loop2->edGetConditionPort());
  graph->edAddLink(o2s,loop2->edGetConditionPort());
  graph->edRemoveLink(o2s,loop2->edGetConditionPort());
  graph->edRemoveLink(o1s,loop2->edGetConditionPort());
  delete graph;
}

void EngineIntegrationTest::testLinkUpdate4()
{
  Bloc *graph1=new Bloc("graph1");
  Bloc *graph2=new Bloc("graph2");
  graph1->edAddChild(graph2);
  ForEachLoop *forEach=new ForEachLoop("myFE",Runtime::_tc_double);
  graph2->edAddChild(forEach);
  ToyNode *n1=new ToyNode("T1");
  OutputPort *o11=n1->edAddOutputPort("o11",Runtime::_tc_double);
  Seq2ToyNode *n2=new Seq2ToyNode("n2");
  graph2->edAddChild(n2);
  graph2->edAddCFLink(forEach,n2);
  forEach->edSetNode(n1);
  Seq2ToyNode *n3=new Seq2ToyNode("n3");
  graph1->edAddChild(n3);
  graph1->edAddCFLink(graph2,n3);
  //
  CPPUNIT_ASSERT(graph1->edAddLink(o11,n2->edGetInValue1()));
  CPPUNIT_ASSERT(!graph1->edAddLink(o11,n2->edGetInValue1()));
  CPPUNIT_ASSERT(graph1->edAddLink(o11,n3->edGetInValue1()));
  CPPUNIT_ASSERT(!graph1->edAddLink(o11,n3->edGetInValue1()));
  graph1->edRemoveLink(o11,n2->edGetInValue1());
  graph1->edRemoveLink(o11,n3->edGetInValue1());
  //
  graph1->edRemoveChild(graph2);
  delete graph2;
  delete graph1;
}

/*!
 * test of links between 2 loops to simulate coupling.
 */
void EngineIntegrationTest::testInterLoopDFLink()
{
  Bloc *graph=new Bloc("Graph");
  ForLoop *loop1=new ForLoop("loop1"); graph->edAddChild(loop1);
  ForLoop *loop2=new ForLoop("loop2"); graph->edAddChild(loop2);
  Bloc *b4N1=new Bloc("b4N1"); loop1->edSetNode(b4N1); ToyNode *n1=new ToyNode("T1"); b4N1->edAddChild(n1);
  ToyNode *n2=new ToyNode("T2"); loop2->edSetNode(n2);
  InputPort *i11=n1->edAddInputPort("i11",Runtime::_tc_double);
  InputPort *i21=n2->edAddInputPort("i21",Runtime::_tc_double);
  OutputPort *o11=n1->edAddOutputPort("o11",Runtime::_tc_double);
  OutputPort *o21=n2->edAddOutputPort("o21",Runtime::_tc_double);
  CPPUNIT_ASSERT( graph->edAddLink(o11,i21) );
  CPPUNIT_ASSERT_EQUAL(1,i21->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,o11->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT( !graph->edAddLink(o11,i21) );
  CPPUNIT_ASSERT_EQUAL(1,i21->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,o11->edGetNumberOfOutLinks());
  set<InPort *> setI1=o11->edSetInPort();
  CPPUNIT_ASSERT(*(setI1.begin()) == i21);
  set<OutPort *> setO1=i21->edSetOutPort();
  CPPUNIT_ASSERT(*(setO1.begin()) == o11);
  graph->edRemoveLink(o11,i21);
  CPPUNIT_ASSERT_EQUAL(0,i21->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(0,o11->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT( graph->edAddLink(o11,i21) );
  CPPUNIT_ASSERT( !graph->edAddLink(o11,i21) );
  CPPUNIT_ASSERT_EQUAL(1,i21->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,o11->edGetNumberOfOutLinks());
  InputDataStreamPort *i22=n2->edAddInputDataStreamPort("i22",Runtime::_tc_double);
  CPPUNIT_ASSERT( graph->edAddLink(o11,i22) );//DF-DS link
  CPPUNIT_ASSERT_EQUAL(1,i21->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(2,o11->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i22->edGetNumberOfLinks());
  graph->edRemoveLink(o11,i21);
  CPPUNIT_ASSERT_EQUAL(0,i21->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,o11->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i22->edGetNumberOfLinks());
  graph->edRemoveLink(o11,i22);
  CPPUNIT_ASSERT_EQUAL(0,i21->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(0,o11->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(0,i22->edGetNumberOfLinks());
  CPPUNIT_ASSERT( graph->edAddLink(o11,i21) );
  CPPUNIT_ASSERT_EQUAL(1,i21->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,o11->edGetNumberOfOutLinks());
  setI1=o11->edSetInPort();
  CPPUNIT_ASSERT(*(setI1.begin()) == i21);
  setO1=i21->edSetOutPort();
  CPPUNIT_ASSERT(*(setO1.begin()) == o11);
  CPPUNIT_ASSERT( graph->edAddLink(o11,i22) );
  CPPUNIT_ASSERT_EQUAL(1,i21->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(2,o11->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i22->edGetNumberOfLinks());
  ElementaryNode *n=(ElementaryNode *)loop2->edRemoveNode();
  CPPUNIT_ASSERT_EQUAL(0,o11->edGetNumberOfOutLinks());
  //
  loop2->edSetNode(n);
  CPPUNIT_ASSERT(n==n2);
  OutputDataStreamPort *o22=n->edAddOutputDataStreamPort("o22",Runtime::_tc_double);
  CPPUNIT_ASSERT( graph->edAddLink(o22,i11) );//DS-DF Link
  CPPUNIT_ASSERT_EQUAL(0,o11->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,o22->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i11->edGetNumberOfLinks());
  graph->edRemoveLink(o22,i11);
  CPPUNIT_ASSERT_EQUAL(0,o11->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(0,o22->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(0,i11->edGetNumberOfLinks());
  CPPUNIT_ASSERT( graph->edAddLink(o22,i11) );
  CPPUNIT_ASSERT_EQUAL(0,o11->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,o22->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i11->edGetNumberOfLinks());
  InputDataStreamPort *i13=n1->edAddInputDataStreamPort("i13",Runtime::_tc_double);
  CPPUNIT_ASSERT( graph->edAddLink(o22,i13) );//interloop DS-DS Link
  CPPUNIT_ASSERT_EQUAL(0,o11->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(2,o22->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i11->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(1,i13->edGetNumberOfLinks());
  graph->edRemoveLink(o22,i13);
  CPPUNIT_ASSERT_EQUAL(0,o11->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,o22->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i11->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(0,i13->edGetNumberOfLinks());
  graph->edRemoveLink(o22,i11);
  CPPUNIT_ASSERT_EQUAL(0,o11->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(0,o22->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(0,i11->edGetNumberOfLinks());
  CPPUNIT_ASSERT_EQUAL(0,i13->edGetNumberOfLinks());
  delete graph;
}

/*!
 * test of links between 2 loops to simulate coupling.
 */
void EngineIntegrationTest::deathTestForLinks()
{
  Bloc *graph=new Bloc("Graph");
  Bloc *b1=new Bloc("b1"); Bloc *b2=new Bloc("b2"); graph->edAddChild(b1); graph->edAddChild(b2);
  ForLoop *loop1=new ForLoop("loop1"); b1->edAddChild(loop1);
  ForLoop *loop2=new ForLoop("loop2"); b2->edAddChild(loop2);
  Bloc *b11=new Bloc("b11"); loop1->edSetNode(b11);
  Bloc *b21=new Bloc("b21"); loop2->edSetNode(b21);
  ToyNode *n22=new ToyNode("T22"); b21->edAddChild(n22);
  Switch *sw12=new Switch("sw12"); b11->edAddChild(sw12);
  Bloc *b13=new Bloc("b13"); sw12->edSetNode(7,b13);
  Switch *sw14=new Switch("sw14"); b13->edAddChild(sw14);
  Bloc *b15=new Bloc("b15"); sw14->edSetNode(9,b15);
  ToyNode *n16=new ToyNode("T16"); b15->edAddChild(n16);
  //Let's link
  OutputPort *o16_1=n16->edAddOutputPort("o16_1",Runtime::_tc_double);
  InputPort *i22_1=n22->edAddInputPort("i22_1",Runtime::_tc_double);
  CPPUNIT_ASSERT( graph->edAddLink(o16_1,i22_1) );
  CPPUNIT_ASSERT_EQUAL(1,o16_1->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i22_1->edGetNumberOfLinks());
  graph->edRemoveLink(o16_1,i22_1);
  CPPUNIT_ASSERT_EQUAL(0,o16_1->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(0,i22_1->edGetNumberOfLinks());
  CPPUNIT_ASSERT( graph->edAddLink(o16_1,i22_1) );
  CPPUNIT_ASSERT( !graph->edAddLink(o16_1,i22_1) );
  CPPUNIT_ASSERT_EQUAL(1,o16_1->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i22_1->edGetNumberOfLinks());
  set<InPort *> setI1=o16_1->edSetInPort();
  CPPUNIT_ASSERT(*(setI1.begin()) == i22_1);
  vector<DataPort *> vec=o16_1->calculateHistoryOfLinkWith(i22_1);
  string path;
  for(vector<DataPort *>::iterator iter=vec.begin();iter!=vec.end();iter++)
    { path+=(*iter)->getNameOfTypeOfCurrentInstance(); path+=" * "; }
  CPPUNIT_ASSERT ( path == "OutputPort * OutputPort * OutputPort * OutputDataStreamPort * InputDataStreamPort * InputPort * ");
  graph->edRemoveLink(o16_1,i22_1);
  CPPUNIT_ASSERT_EQUAL(0,o16_1->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(0,i22_1->edGetNumberOfLinks());
  CPPUNIT_ASSERT( graph->edAddLink(o16_1,i22_1) );
  CPPUNIT_ASSERT( !graph->edAddLink(o16_1,i22_1) );
  CPPUNIT_ASSERT_EQUAL(1,o16_1->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(1,i22_1->edGetNumberOfLinks());
  path="";
  vec=o16_1->calculateHistoryOfLinkWith(i22_1);
  for(vector<DataPort *>::iterator iter=vec.begin();iter!=vec.end();iter++)
    { path+=(*iter)->getNameOfTypeOfCurrentInstance(); path+=" * "; }
  CPPUNIT_ASSERT ( path == "OutputPort * OutputPort * OutputPort * OutputDataStreamPort * InputDataStreamPort * InputPort * ");
  graph->edRemoveLink(o16_1,i22_1);
  CPPUNIT_ASSERT_EQUAL(0,o16_1->edGetNumberOfOutLinks());
  CPPUNIT_ASSERT_EQUAL(0,i22_1->edGetNumberOfLinks());
  delete graph;
}

void EngineIntegrationTest::testForEachLoop1()
{
  LinkInfo info(LinkInfo::ALL_DONT_STOP);
  Bloc *graph=new Bloc("graph");
  ForEachLoop *forEach=new ForEachLoop("myFE",Runtime::_tc_double);
  graph->edAddChild(forEach);
  ToyNode *n1=new ToyNode("T1");
  InputPort *i11=n1->edAddInputPort("i11",Runtime::_tc_double); i11->edInit(1.3);
  InputPort *i12=n1->edAddInputPort("i12",Runtime::_tc_double); i12->edInit(3.4);
  InputPort *i13=n1->edAddInputPort("i13",Runtime::_tc_double); i13->edInit(5.6);
  OutputPort *o11=n1->edAddOutputPort("o11",Runtime::_tc_double);
  graph->edAddChild(n1);
  graph->edAddCFLink(n1,forEach);
  SeqToyNode *n2=new SeqToyNode("T2");
  graph->edAddChild(n2);
  graph->edAddCFLink(n2,forEach);
  graph->edAddLink(n1->edGetNbOfInputsOutputPort(),forEach->edGetNbOfBranchesPort());
  graph->edAddLink(n2->edGetSeqOut(),forEach->edGetSeqOfSamplesPort());
  n2->edGetInIntValue()->edInit(5);
  ToyNode *n3=new ToyNode("T3");
  InputPort *i31=n3->edAddInputPort("i31",Runtime::_tc_double); i31->edInit(2.);
  InputPort *i32=n3->edAddInputPort("i32",Runtime::_tc_double);
  OutputPort *o31=n3->edAddOutputPort("o31",Runtime::_tc_double);
  forEach->edSetNode(n3);
  Seq2ToyNode *n4=new Seq2ToyNode("sequencer");
  graph->edAddChild(n4);
  graph->edAddCFLink(forEach,n4);
  graph->edAddLink(forEach->edGetSamplePort(),i32);
  graph->edAddLink(o31,n4->edGetInValue1());
  graph->edAddLink(n2->edGetSeqOut(),n4->edGetInValue2());
  int tab[]={12,14,16,18,20};
  vector<int> tabv(tab,tab+5);
  SequenceAnyPtr tmp=SequenceAny::New(tabv);//expected sequence
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  Executor exe;
  exe.RunW(graph);
  CPPUNIT_ASSERT_EQUAL(3,(int)forEach->getNumberOfBranchesCreatedDyn());
  Any *val=n4->edGetSeqOut()->get();
  CPPUNIT_ASSERT( *val==*tmp );
  exe.RunW(graph);
  CPPUNIT_ASSERT_EQUAL(3,(int)forEach->getNumberOfBranchesCreatedDyn());
  n1->edRemovePort(i13);
  exe.RunW(graph);
  val=n4->edGetSeqOut()->get();
  CPPUNIT_ASSERT( *val==*tmp );
  CPPUNIT_ASSERT_EQUAL(2,(int)forEach->getNumberOfBranchesCreatedDyn());
  exe.RunW(graph);
  val=n4->edGetSeqOut()->get();
  CPPUNIT_ASSERT( *val==*tmp );
  CPPUNIT_ASSERT_EQUAL(2,(int)forEach->getNumberOfBranchesCreatedDyn());
  Bloc *graph2=(Bloc *)graph->clone(0);
  delete graph;
  graph2->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe.RunW(graph2);
  CPPUNIT_ASSERT_EQUAL(2,(int)((ForEachLoop *)graph2->getChildByName("myFE"))->getNumberOfBranchesCreatedDyn());
  exe.RunW(graph2);
  CPPUNIT_ASSERT_EQUAL(2,(int)((ForEachLoop *)graph2->getChildByName("myFE"))->getNumberOfBranchesCreatedDyn());
  n4=(Seq2ToyNode *)graph2->getChildByName("sequencer");
  val=n4->edGetSeqOut()->get();
  CPPUNIT_ASSERT( *val==*tmp );
  delete graph2;
}

void EngineIntegrationTest::testForEachLoop2()
{
  LinkInfo info(LinkInfo::ALL_DONT_STOP);
  Bloc *graph=new Bloc("graph");
  ForEachLoop *forEach=new ForEachLoop("myFE",Runtime::_tc_double);
  graph->edAddChild(forEach);
  ToyNode *n1=new ToyNode("T1");
  InputPort *i11=n1->edAddInputPort("i11",Runtime::_tc_double); i11->edInit(1.3);
  InputPort *i12=n1->edAddInputPort("i12",Runtime::_tc_double); i12->edInit(3.4);
  InputPort *i13=n1->edAddInputPort("i13",Runtime::_tc_double); i13->edInit(5.6);
  OutputPort *o11=n1->edAddOutputPort("o11",Runtime::_tc_double);
  graph->edAddChild(n1);
  graph->edAddCFLink(n1,forEach);
  SeqToyNode *n2=new SeqToyNode("T2");
  graph->edAddChild(n2);
  graph->edAddCFLink(n2,forEach);
  graph->edAddLink(n1->edGetNbOfInputsOutputPort(),forEach->edGetNbOfBranchesPort());
  graph->edAddLink(n2->edGetSeqOut(),forEach->edGetSeqOfSamplesPort());
  n2->edGetInIntValue()->edInit(5);
  Bloc *blocToShakeBaby=new Bloc("blocToShakeBaby");
  ToyNode *n3=new ToyNode("T3");
  blocToShakeBaby->edAddChild(n3);
  InputPort *i31=n3->edAddInputPort("i31",Runtime::_tc_double); i31->edInit(2.);
  InputPort *i32=n3->edAddInputPort("i32",Runtime::_tc_double);
  OutputPort *o31=n3->edAddOutputPort("o31",Runtime::_tc_double);
  forEach->edSetNode(blocToShakeBaby);
  Seq2ToyNode *n4=new Seq2ToyNode("sequencer");
  graph->edAddChild(n4);
  graph->edAddCFLink(forEach,n4);
  graph->edAddLink(forEach->edGetSamplePort(),i32);
  graph->edAddLink(o31,n4->edGetInValue1());
  graph->edAddLink(n2->edGetSeqOut(),n4->edGetInValue2());
  CPPUNIT_ASSERT(graph->getOutputPort("myFE.blocToShakeBaby.T3.o31"));
  int tab[]={12,14,16,18,20};
  vector<int> tabv(tab,tab+5);
  SequenceAnyPtr tmp=SequenceAny::New(tabv);//expected sequence
  Executor exe;
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe.RunW(graph);
  CPPUNIT_ASSERT_EQUAL(3,(int)forEach->getNumberOfBranchesCreatedDyn());
  Any *val=n4->edGetSeqOut()->get();
  CPPUNIT_ASSERT( *val==*tmp );
  exe.RunW(graph);
  CPPUNIT_ASSERT_EQUAL(3,(int)forEach->getNumberOfBranchesCreatedDyn());
  n1->edRemovePort(i13);
  exe.RunW(graph);
  val=n4->edGetSeqOut()->get();
  CPPUNIT_ASSERT( *val==*tmp );
  CPPUNIT_ASSERT_EQUAL(2,(int)forEach->getNumberOfBranchesCreatedDyn());
  exe.RunW(graph);
  val=n4->edGetSeqOut()->get();
  CPPUNIT_ASSERT( *val==*tmp );
  CPPUNIT_ASSERT_EQUAL(2,(int)forEach->getNumberOfBranchesCreatedDyn());
  Bloc *graph2=(Bloc *)graph->clone(0);
  delete graph;
  graph2->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe.RunW(graph2);
  CPPUNIT_ASSERT_EQUAL(2,(int)((ForEachLoop *)graph2->getChildByName("myFE"))->getNumberOfBranchesCreatedDyn());
  exe.RunW(graph2);
  CPPUNIT_ASSERT_EQUAL(2,(int)((ForEachLoop *)graph2->getChildByName("myFE"))->getNumberOfBranchesCreatedDyn());
  n4=(Seq2ToyNode *)graph2->getChildByName("sequencer");
  val=n4->edGetSeqOut()->get();
  CPPUNIT_ASSERT( *val==*tmp );
  delete graph2;
}

//Multi inclusion of ForEach
void EngineIntegrationTest::testForEachLoop3()
{
  LinkInfo info(LinkInfo::ALL_DONT_STOP);
  Bloc *graph=new Bloc("graph");
  ForEachLoop *forEach1=new ForEachLoop("myFE1",Runtime::_tc_double);
  TypeCodeSeq *tc1=new TypeCodeSeq("","",Runtime::_tc_double);
  ForEachLoop *forEach2=new ForEachLoop("myFE2",tc1);
  SequenceAnyPtr tmpI=SequenceAny::New(tc1,3);//value as input
  tc1->decrRef();
  tc1=new TypeCodeSeq("","",Runtime::_tc_int);
  SequenceAnyPtr tmpO=SequenceAny::New(tc1,3);//value expected
  tc1->decrRef();
  graph->edAddChild(forEach2);
  forEach2->edSetNode(forEach1);
  ToyNode *n1=new ToyNode("T1");
  InputPort *i11=n1->edAddInputPort("i11",Runtime::_tc_double); i11->edInit(9.);
  InputPort *i12=n1->edAddInputPort("i12",Runtime::_tc_double);
  OutputPort *o11=n1->edAddOutputPort("o11",Runtime::_tc_double);
  OutputPort *o12=n1->edAddOutputPort("o12",Runtime::_tc_double);
  OutputPort *o13=n1->edAddOutputPort("o13",Runtime::_tc_double);
  forEach1->edSetNode(n1);
  graph->edAddLink(forEach2->edGetSamplePort(),forEach1->edGetSeqOfSamplesPort());
  graph->edAddLink(forEach1->edGetSamplePort(),i12);
  Seq3ToyNode *n2=new Seq3ToyNode("T2");
  graph->edAddChild(n2);
  graph->edAddLink(o11,n2->edGetInValue1());
  graph->edAddLink(o13,n2->edGetInValue2());
  graph->edAddCFLink(forEach2,n2);
  forEach1->edGetNbOfBranchesPort()->edInit(5);
  forEach2->edGetNbOfBranchesPort()->edInit(10);
  //Preparing input matrix
  double tab1[]={3.,6.,9.,12.,15.};
  double tab2[]={18.,21.,24.,27.,30.};
  double tab3[]={33.,36.,39.,42.,42.};
  vector<double> tabv1(tab1,tab1+5);
  vector<double> tabv2(tab2,tab2+5);
  vector<double> tabv3(tab3,tab3+5);
  SequenceAnyPtr tmp=SequenceAny::New(tabv1);
  tmpI->setEltAtRank(0,tmp);
  tmp=SequenceAny::New(tabv2);
  tmpI->setEltAtRank(1,tmp);
  tmp=SequenceAny::New(tabv3);
  tmpI->setEltAtRank(2,tmp);
  forEach2->edGetSeqOfSamplesPort()->edInit((Any *)tmpI);
  //preparing expected matrix
  int tab1I[]={8,10,12,14,16};
  int tab2I[]={18,20,22,24,26};
  int tab3I[]={28,30,32,34,34};
  vector<int> tabvI1(tab1I,tab1I+5);
  vector<int> tabvI2(tab2I,tab2I+5);
  vector<int> tabvI3(tab3I,tab3I+5);
  tmp=SequenceAny::New(tabvI1);
  tmpO->setEltAtRank(0,tmp);
  tmp=SequenceAny::New(tabvI2);
  tmpO->setEltAtRank(1,tmp);
  tmp=SequenceAny::New(tabvI3);
  tmpO->setEltAtRank(2,tmp);
  Executor exe;
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe.RunW(graph);
  CPPUNIT_ASSERT_EQUAL(3,(int)forEach2->getNumberOfBranchesCreatedDyn());
  Any *val=n2->edGetSeqOut()->get();
  CPPUNIT_ASSERT( *val==*tmpO );
  exe.RunW(graph);
  CPPUNIT_ASSERT_EQUAL(3,(int)forEach2->getNumberOfBranchesCreatedDyn());
  val=n2->edGetSeqOut()->get();
  CPPUNIT_ASSERT( *val==*tmpO );
  ForEachLoop *clone=(ForEachLoop *)forEach2->clone(0);
  try
    {
      clone->getNumberOfBranchesCreatedDyn();
      CPPUNIT_ASSERT(false);
    }
  catch(Exception& e)
    {
      CPPUNIT_ASSERT( string(e.what())=="ForEachLoop::getNumberOfBranches : No branches created dynamically ! - ForEachLoop needs to run or to be runned to call getNumberOfBranches");
    }
  Bloc *graphCloned=(Bloc *)graph->clone(0);
  delete graph;
  clone->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  graphCloned->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe.RunW(clone);
  CPPUNIT_ASSERT_EQUAL(3,(int)clone->getNumberOfBranchesCreatedDyn());
  exe.RunW(clone);
  CPPUNIT_ASSERT_EQUAL(3,(int)clone->getNumberOfBranchesCreatedDyn());
  exe.RunW(graphCloned);
  CPPUNIT_ASSERT_EQUAL(3,(int)((ForEachLoop *)graphCloned->getChildByName("myFE2"))->getNumberOfBranchesCreatedDyn());
  val=((Seq3ToyNode *) graphCloned->getChildByName("T2"))->edGetSeqOut()->get();
  CPPUNIT_ASSERT( *val==*tmpO );
  exe.RunW(graphCloned);
  CPPUNIT_ASSERT_EQUAL(3,(int)((ForEachLoop *)graphCloned->getChildByName("myFE2"))->getNumberOfBranchesCreatedDyn());
  val=((Seq3ToyNode *) graphCloned->getChildByName("T2"))->edGetSeqOut()->get();
  CPPUNIT_ASSERT( *val==*tmpO );
  delete graphCloned;
  delete clone;
}

void EngineIntegrationTest::testForEachLoop4()
{
  LinkInfo info(LinkInfo::ALL_DONT_STOP);
  Bloc *graph=new Bloc("graph");
  ForEachLoop *forEach1=new ForEachLoop("myFE1",Runtime::_tc_int);
  Switch *sw=new Switch("Sw1");
  forEach1->edSetNode(sw);
  graph->edAddChild(forEach1);
  SeqToyNode *n0=new SeqToyNode("T0");
  graph->edAddChild(n0);
  n0->edGetInIntValue()->edInit(5);
  ToyNode *n1=new ToyNode("T1");
  InputPort *i11=n1->edAddInputPort("i11",Runtime::_tc_double); i11->edInit(9.);
  InputPort *i12=n1->edAddInputPort("i12",Runtime::_tc_double); i12->edInit(8.);
  OutputPort *o11=n1->edAddOutputPort("o11",Runtime::_tc_double);
  graph->edAddChild(n1);
  Seq2ToyNode *n2=new Seq2ToyNode("T2");
  graph->edAddChild(n2);
  graph->edAddCFLink(n0,forEach1);
  graph->edAddCFLink(n1,forEach1);
  graph->edAddCFLink(forEach1,n2);
  ToyNode *n3=new ToyNode("T3");
  InputPort *i31=n3->edAddInputPort("i31",Runtime::_tc_double);
  InputPort *i32=n3->edAddInputPort("i32",Runtime::_tc_double); i32->edInit(5.);
  OutputPort *o31=n3->edAddOutputPort("o31",Runtime::_tc_double);
  sw->edSetNode(2,n3);
  ToyNode *n4=new ToyNode("T4");
  InputPort *i41=n4->edAddInputPort("i41",Runtime::_tc_double); i41->edInit(5.);
  InputPort *i42=n4->edAddInputPort("i42",Runtime::_tc_double); 
  InputPort *i43=n4->edAddInputPort("i43",Runtime::_tc_double); i43->edInit(7.);
  OutputPort *o41=n4->edAddOutputPort("o41",Runtime::_tc_double);
  OutputPort *o42=n4->edAddOutputPort("o42",Runtime::_tc_double);
  sw->edSetDefaultNode(n4);
  graph->edAddLink(o11,i31);
  graph->edAddLink(forEach1->edGetSamplePort(),sw->edGetConditionPort());
  graph->edAddLink(o31,n2->edGetInValue1());
  graph->edRemoveLink(o31,n2->edGetInValue1());
  graph->edAddLink(o31,n2->edGetInValue1());
  graph->edAddLink(o31,n2->edGetInValue2());//
  graph->edAddLink(o11,i42);
  graph->edAddLink(o41,n2->edGetInValue2());
  graph->edRemoveLink(o41,n2->edGetInValue2());
  graph->edAddLink(o41,n2->edGetInValue2());
  graph->edAddLink(o42,n2->edGetInValue1());
  int tabI[]={1,2,2,45,2,5,6,2};
  vector<int> tabvI(tabI,tabI+8);
  SequenceAnyPtr tmp=SequenceAny::New(tabvI);
  forEach1->edGetSeqOfSamplesPort()->edInit((Any *)tmp);
  //graph->edAddLink(n0->edGetSeqOut(),forEach1->edGetSeqOfSamplesPort());
  graph->edAddLink(n0->edGetSeqOut(),n2->edGetInValue2());
  Executor exe;
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(1==info.getNumberOfErrLinks(E_NEVER_SET_INPUTPORT));
 
  try
    {
      exe.RunW(graph);
      CPPUNIT_ASSERT(false);
    }
  catch(Exception& e)
    {
      CPPUNIT_ASSERT(string(e.what())=="InputPort::checkBasicConsistency : Port nbBranches of node with name myFE1 neither initialized nor linked back");
    }
  forEach1->edGetNbOfBranchesPort()->edInit(7);
  //expected vals
  int tabI2[]={29,44,44,29,44,29,29,44};
  vector<int> tabvI2(tabI2,tabI2+8);
  SequenceAnyPtr tmp2=SequenceAny::New(tabvI2);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe.RunW(graph);
  Any *val=n2->edGetSeqOut()->get();
  CPPUNIT_ASSERT( *val==*tmp2 );
  CPPUNIT_ASSERT_EQUAL(7,(int)forEach1->getNumberOfBranchesCreatedDyn());
  exe.RunW(graph);
  val=n2->edGetSeqOut()->get();
  CPPUNIT_ASSERT( *val==*tmp2 );
  CPPUNIT_ASSERT_EQUAL(7,(int)forEach1->getNumberOfBranchesCreatedDyn());
  Bloc *graphCloned=(Bloc *)graph->clone(0);
  delete graph;
  graphCloned->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe.RunW(graphCloned);
  val=((Seq2ToyNode *)graphCloned->getChildByName("T2"))->edGetSeqOut()->get();
  delete graphCloned;
}

/*!
  Test to check possibility to linked several times SamplePort OutputPort of a ForEachLoop node.
 */
void EngineIntegrationTest::testForEachLoop5()
{
  LinkInfo info(LinkInfo::ALL_DONT_STOP);
  Bloc *graph=new Bloc("graph");
  ForEachLoop *forEach=new ForEachLoop("myFE",Runtime::_tc_double);
  graph->edAddChild(forEach);
  ToyNode *n1=new ToyNode("T1");
  InputPort *i11=n1->edAddInputPort("i11",Runtime::_tc_double); i11->edInit(1.3);
  InputPort *i12=n1->edAddInputPort("i12",Runtime::_tc_double); i12->edInit(3.4);
  InputPort *i13=n1->edAddInputPort("i13",Runtime::_tc_double); i13->edInit(5.6);
  OutputPort *o11=n1->edAddOutputPort("o11",Runtime::_tc_double);
  graph->edAddChild(n1);
  graph->edAddCFLink(n1,forEach);
  SeqToyNode *n2=new SeqToyNode("T2");
  graph->edAddChild(n2);
  graph->edAddCFLink(n2,forEach);
  graph->edAddLink(n1->edGetNbOfInputsOutputPort(),forEach->edGetNbOfBranchesPort());
  graph->edAddLink(n2->edGetSeqOut(),forEach->edGetSeqOfSamplesPort());
  n2->edGetInIntValue()->edInit(5);
  ToyNode *n3=new ToyNode("T3");
  InputPort *i31=n3->edAddInputPort("i31",Runtime::_tc_double); 
  InputPort *i32=n3->edAddInputPort("i32",Runtime::_tc_double);
  OutputPort *o31=n3->edAddOutputPort("o31",Runtime::_tc_double);
  forEach->edSetNode(n3);
  Seq2ToyNode *n4=new Seq2ToyNode("sequencer");
  graph->edAddChild(n4);
  graph->edAddCFLink(forEach,n4);
  graph->edAddLink(forEach->edGetSamplePort(),i31);
  graph->edAddLink(forEach->edGetSamplePort(),i32);
  graph->edAddLink(o31,n4->edGetInValue1());
  graph->edAddLink(n2->edGetSeqOut(),n4->edGetInValue2());
  int tab[]={15,18,21,24,27};
  vector<int> tabv(tab,tab+5);
  SequenceAnyPtr tmp=SequenceAny::New(tabv);//expected sequence
  Executor exe;
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe.RunW(graph);
  CPPUNIT_ASSERT_EQUAL(3,(int)forEach->getNumberOfBranchesCreatedDyn());
  Any *val=n4->edGetSeqOut()->get();
  CPPUNIT_ASSERT( *val==*tmp );
  exe.RunW(graph);
  CPPUNIT_ASSERT_EQUAL(3,(int)forEach->getNumberOfBranchesCreatedDyn());
  n1->edRemovePort(i13);
  exe.RunW(graph);
  val=n4->edGetSeqOut()->get();
  CPPUNIT_ASSERT( *val==*tmp );
  CPPUNIT_ASSERT_EQUAL(2,(int)forEach->getNumberOfBranchesCreatedDyn());
  exe.RunW(graph);
  val=n4->edGetSeqOut()->get();
  CPPUNIT_ASSERT( *val==*tmp );
  CPPUNIT_ASSERT_EQUAL(2,(int)forEach->getNumberOfBranchesCreatedDyn());
  Bloc *graph2=(Bloc *)graph->clone(0);
  delete graph;
  exe.RunW(graph2);
  CPPUNIT_ASSERT_EQUAL(2,(int)((ForEachLoop *)graph2->getChildByName("myFE"))->getNumberOfBranchesCreatedDyn());
  exe.RunW(graph2);
  CPPUNIT_ASSERT_EQUAL(2,(int)((ForEachLoop *)graph2->getChildByName("myFE"))->getNumberOfBranchesCreatedDyn());
  n4=(Seq2ToyNode *)graph2->getChildByName("sequencer");
  val=n4->edGetSeqOut()->get();
  CPPUNIT_ASSERT( *val==*tmp );
  delete graph2;
}


/*!
 * Here a test for OptimizerLoop with an evenemential (or synchronous) algorithm
 */
void EngineIntegrationTest::testForOptimizerLoop1()
{
  LinkInfo info(LinkInfo::ALL_DONT_STOP);
  Bloc *graph=new Bloc("Global");
  OptimizerLoop *opt=new OptimizerLoop("myOptWthAlgSync","libPluginOptEvTest1","PluginOptEvTest1Factory",true);
  graph->edAddChild(opt);
  ToyNode *n1=new ToyNode("T1");
  ToyNode *n2=new ToyNode("T2");
  graph->edAddChild(n2);
  graph->edAddCFLink(opt,n2);
  opt->edSetNode(n1);
  InputPort *i1=n1->edAddInputPort("i1",Runtime::_tc_double);
  OutputPort *o1=n1->edAddOutputPort("o1",Runtime::_tc_double);
  graph->edAddLink(opt->edGetSamplePort(),i1);
  InputPort *i2=n2->edAddInputPort("i2",opt->edGetAlgoResultPort()->edGetType());
  OutputPort *o2_1=n2->edAddOutputPort("o1",Runtime::_tc_double);
  graph->edAddLink(opt->edGetAlgoResultPort(),i2);
  graph->edAddLink(n1->edGetNbOfInputsOutputPort(),opt->edGetPortForOutPool());
  opt->edGetNbOfBranchesPort()->edInit(2);
  opt->edGetAlgoInitPort()->edInit("toto");
  Executor exe;
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(45.6,((OutputToyPort*)o2_1)->get()->getDoubleValue(),DBL_PRECISION_COMPARE );
  CPPUNIT_ASSERT(8==(int)opt->getNumberOfEltsConsumed() || 7==(int)opt->getNumberOfEltsConsumed());
  CPPUNIT_ASSERT_EQUAL(2,(int)((DynParaLoop *)(opt))->getNumberOfBranchesCreatedDyn());
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(45.6,((OutputToyPort*)o2_1)->get()->getDoubleValue(),DBL_PRECISION_COMPARE );
  CPPUNIT_ASSERT(8==(int)opt->getNumberOfEltsConsumed() || 7==(int)opt->getNumberOfEltsConsumed());
  CPPUNIT_ASSERT_EQUAL(2,(int)((DynParaLoop *)(opt))->getNumberOfBranchesCreatedDyn());
  Bloc *clone=(Bloc *)graph->clone(0);
  delete graph;
  clone->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe.RunW(clone);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(45.6,((OutputToyPort*)(clone->getOutPort("T2.o1")))->get()->getDoubleValue(),DBL_PRECISION_COMPARE );
  CPPUNIT_ASSERT(8==(int)((OptimizerLoop *)(clone->getChildByName("myOptWthAlgSync")))->getNumberOfEltsConsumed() || 7==(int)((OptimizerLoop *)(clone->getChildByName("myOptWthAlgSync")))->getNumberOfEltsConsumed());
  CPPUNIT_ASSERT_EQUAL(2,(int)((OptimizerLoop *)(clone->getChildByName("myOptWthAlgSync")))->getNumberOfBranchesCreatedDyn());
  exe.RunW(clone);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(45.6,((OutputToyPort*)(clone->getOutPort("T2.o1")))->get()->getDoubleValue(),DBL_PRECISION_COMPARE );
  CPPUNIT_ASSERT(8==(int)((OptimizerLoop *)(clone->getChildByName("myOptWthAlgSync")))->getNumberOfEltsConsumed() || 7==(int)((OptimizerLoop *)(clone->getChildByName("myOptWthAlgSync")))->getNumberOfEltsConsumed());
  CPPUNIT_ASSERT_EQUAL(2,(int)((OptimizerLoop *)(clone->getChildByName("myOptWthAlgSync")))->getNumberOfBranchesCreatedDyn());
  delete clone;
}


/*!
 * Idem testForOptimizerLoop1 but with intermediate bloc inside OptermizerLoop.
 */
void EngineIntegrationTest::testForOptimizerLoop2()
{
  LinkInfo info(LinkInfo::ALL_DONT_STOP);
  Bloc *graph=new Bloc("Global");
  OptimizerLoop *opt=new OptimizerLoop("myOptWthAlgSync","libPluginOptEvTest1","PluginOptEvTest1Factory",true);
  graph->edAddChild(opt);
  ToyNode *n1=new ToyNode("T1");
  ToyNode *n2=new ToyNode("T2");
  Bloc *bloc=new Bloc("Bloc");
  graph->edAddChild(n2);
  graph->edAddCFLink(opt,n2);
  opt->edSetNode(bloc);
  bloc->edAddChild(n1);
  InputPort *i1=n1->edAddInputPort("i1",Runtime::_tc_double);
  OutputPort *o1=n1->edAddOutputPort("o1",Runtime::_tc_double);
  graph->edAddLink(opt->edGetSamplePort(),i1);
  InputPort *i2=n2->edAddInputPort("i2",opt->edGetAlgoResultPort()->edGetType());
  OutputPort *o2_1=n2->edAddOutputPort("o1",Runtime::_tc_double);
  graph->edAddLink(opt->edGetAlgoResultPort(),i2);
  graph->edAddLink(n1->edGetNbOfInputsOutputPort(),opt->edGetPortForOutPool());
  opt->edGetNbOfBranchesPort()->edInit(2);
  opt->edGetAlgoInitPort()->edInit("toto");
  Executor exe;
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(45.6,((OutputToyPort*)o2_1)->get()->getDoubleValue(),DBL_PRECISION_COMPARE );
  CPPUNIT_ASSERT(8==(int)opt->getNumberOfEltsConsumed() || 7==(int)opt->getNumberOfEltsConsumed());
  CPPUNIT_ASSERT_EQUAL(2,(int)((DynParaLoop *)(opt))->getNumberOfBranchesCreatedDyn());
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(45.6,((OutputToyPort*)o2_1)->get()->getDoubleValue(),DBL_PRECISION_COMPARE );
  CPPUNIT_ASSERT(8==(int)opt->getNumberOfEltsConsumed() || 7==(int)opt->getNumberOfEltsConsumed());
  CPPUNIT_ASSERT_EQUAL(2,(int)((DynParaLoop *)(opt))->getNumberOfBranchesCreatedDyn());
  Bloc *clone=(Bloc *)graph->clone(0);
  delete graph;
  clone->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  exe.RunW(clone);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(45.6,((OutputToyPort*)(clone->getOutPort("T2.o1")))->get()->getDoubleValue(),DBL_PRECISION_COMPARE );
  CPPUNIT_ASSERT(8==(int)((OptimizerLoop *)(clone->getChildByName("myOptWthAlgSync")))->getNumberOfEltsConsumed() || 7==(int)((OptimizerLoop *)(clone->getChildByName("myOptWthAlgSync")))->getNumberOfEltsConsumed());
  CPPUNIT_ASSERT_EQUAL(2,(int)((OptimizerLoop *)(clone->getChildByName("myOptWthAlgSync")))->getNumberOfBranchesCreatedDyn());
  exe.RunW(clone);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(45.6,((OutputToyPort*)(clone->getOutPort("T2.o1")))->get()->getDoubleValue(),DBL_PRECISION_COMPARE );
  CPPUNIT_ASSERT(8==(int)((OptimizerLoop *)(clone->getChildByName("myOptWthAlgSync")))->getNumberOfEltsConsumed() || 7==(int)((OptimizerLoop *)(clone->getChildByName("myOptWthAlgSync")))->getNumberOfEltsConsumed());
  CPPUNIT_ASSERT_EQUAL(2,(int)((OptimizerLoop *)(clone->getChildByName("myOptWthAlgSync")))->getNumberOfBranchesCreatedDyn());
  delete clone;
}

/*!
 * Test of illegal links within an OptimizerLoop.
 */
void EngineIntegrationTest::testForOptimizerLoop3()
{
  LinkInfo info(LinkInfo::ALL_DONT_STOP);
  Bloc *graph=new Bloc("Global");
  OptimizerLoop *opt=new OptimizerLoop("myOptWthAlgSync","libPluginOptEvTest1","PluginOptEvTest1Factory",true);
  graph->edAddChild(opt);
  ToyNode *n1=new ToyNode("T1");
  ToyNode *n2=new ToyNode("T2");
  Bloc *bloc=new Bloc("Bloc");
  graph->edAddChild(n2);
  graph->edAddCFLink(opt,n2);
  opt->edSetNode(bloc);
  bloc->edAddChild(n1);
  InputPort *i1=n1->edAddInputPort("i1",Runtime::_tc_double);
  OutputPort *o1=n1->edAddOutputPort("o1",Runtime::_tc_double);
  InputPort *i2=n2->edAddInputPort("i2",opt->edGetAlgoResultPort()->edGetType());
  try
  {
    graph->edAddLink(opt->edGetAlgoResultPort(),i1);
    CPPUNIT_FAIL("Construction of an illegal link: the result port linked to a node inside the OptimizerLoop.");
  }
  catch(Exception& e)
  {
  }

  try
  {
    graph->edAddLink(o1,i2);
    CPPUNIT_FAIL("Construction of an illegal link: a node inside the loop linked to a node outside the OptimizerLoop.");
  }
  catch(Exception& e)
  {
  }

  delete graph;
}

/*!
 * Test to check that in for compil-time known connectivity of a graph, deployment calculation is OK. 
 */
void EngineIntegrationTest::testForDeployment1()
{
  // First test of NON ServiceNodes deployment calculation
  ToyNode *n1=new ToyNode("T1");
  ToyNode *n2=new ToyNode("T2");
  Bloc *graph=new Bloc("Global");
  graph->edAddChild(n1); graph->edAddChild(n2);
  DeploymentTree tree=graph->getDeploymentTree();
  CPPUNIT_ASSERT(tree.isNull());//No placement info
  graph->edRemoveChild(n2);
  delete n2;
  tree=graph->getDeploymentTree();
  CPPUNIT_ASSERT(tree.isNull());//No placement info to.
  CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfCTDefContainer()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefContainer());
  CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfCTDefComponentInstances()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefComponentInstances());
  CPPUNIT_ASSERT(!tree.presenceOfDefaultContainer());
  //
  ToyNode1S *n1S=new ToyNode1S("T1S"); graph->edAddChild(n1S);
  tree=graph->getDeploymentTree();
  CPPUNIT_ASSERT(!tree.isNull());//Here placement info.
  CPPUNIT_ASSERT_EQUAL( 1, (int) tree.getFreeDeployableTasks().size());
  CPPUNIT_ASSERT_EQUAL((Task *)n1S, tree.getFreeDeployableTasks()[0]);
  CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfCTDefContainer()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefContainer());
  CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfCTDefComponentInstances()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefComponentInstances());
  CPPUNIT_ASSERT(!tree.presenceOfDefaultContainer());
  //
  Bloc *b1=new Bloc("B1");
  ToyNode1S *n2S=new ToyNode1S("T2S"); b1->edAddChild(n2S); ToyNode1S *n3S=new ToyNode1S("T3S"); b1->edAddChild(n3S); ToyNode2S *n4S=new ToyNode2S("T4S");
  ToyNode2S *n5S=new ToyNode2S("T5S"); b1->edAddChild(n5S);
  graph->edAddChild(b1); graph->edAddChild(n4S);
  tree=graph->getDeploymentTree();
  CPPUNIT_ASSERT(!tree.isNull());//Here placement info.
  CPPUNIT_ASSERT_EQUAL( 5, (int) tree.getFreeDeployableTasks().size());
  CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfCTDefContainer()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefContainer());
  CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfCTDefComponentInstances()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefComponentInstances());
  CPPUNIT_ASSERT(!tree.presenceOfDefaultContainer());
  //Ok now let's associate some components...
  ComponentInstanceTest1 *comp1=new ComponentInstanceTest1("FLICA");
  n1S->setComponent(comp1);
  tree=graph->getDeploymentTree();
  CPPUNIT_ASSERT(!tree.isNull());//Here placement info.
  CPPUNIT_ASSERT_EQUAL( 4, (int) tree.getFreeDeployableTasks().size());
  CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfCTDefContainer()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefContainer());
  CPPUNIT_ASSERT_EQUAL(1, (int) tree.getNumberOfCTDefComponentInstances()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefComponentInstances());
  CPPUNIT_ASSERT(tree.presenceOfDefaultContainer());
  n3S->setComponent(comp1);
  tree=graph->getDeploymentTree();
  CPPUNIT_ASSERT(!tree.isNull());//Here placement info.
  CPPUNIT_ASSERT_EQUAL( 3, (int) tree.getFreeDeployableTasks().size());
  CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfCTDefContainer()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefContainer());
  CPPUNIT_ASSERT_EQUAL(1, (int) tree.getNumberOfCTDefComponentInstances()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefComponentInstances());
  CPPUNIT_ASSERT(tree.presenceOfDefaultContainer());
  vector<Task *> ta=tree.getTasksLinkedToComponent(comp1);
  set<Task *> setToTest1(ta.begin(),ta.end()); set<Task *> setToTest2; setToTest2.insert(n1S); setToTest2.insert(n3S);
  checkSetsEqual(setToTest1,setToTest2);
  ComponentInstanceTest1 *comp2=new ComponentInstanceTest1("CRONOS");
  n2S->setComponent(comp2);
  CPPUNIT_ASSERT_THROW(n4S->setComponent(comp2),YACS::Exception);//incompatibility between ComponentInstanceTest1 and ToyNode2S
  tree=graph->getDeploymentTree();
  CPPUNIT_ASSERT_EQUAL( 2, (int) tree.getFreeDeployableTasks().size());
  CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfCTDefContainer()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefContainer());
  CPPUNIT_ASSERT_EQUAL(2, (int) tree.getNumberOfCTDefComponentInstances()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefComponentInstances());
  CPPUNIT_ASSERT(tree.presenceOfDefaultContainer());
  ComponentInstanceTest2 *comp3=new ComponentInstanceTest2("PYDK");
  n4S->setComponent(comp3);
  n5S->setComponent(comp3);
  tree=graph->getDeploymentTree();
  CPPUNIT_ASSERT_EQUAL( 0, (int) tree.getFreeDeployableTasks().size());
  CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfCTDefContainer()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefContainer());
  CPPUNIT_ASSERT_EQUAL(3, (int) tree.getNumberOfCTDefComponentInstances()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefComponentInstances());
  CPPUNIT_ASSERT(tree.presenceOfDefaultContainer());
  // Ok let's play with containers now
  ContainerTest *cont1=new ContainerTest;
  comp1->setContainer(cont1);
  tree=graph->getDeploymentTree();
  CPPUNIT_ASSERT_EQUAL( 0, (int) tree.getFreeDeployableTasks().size());
  CPPUNIT_ASSERT_EQUAL(1, (int) tree.getNumberOfCTDefContainer()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefContainer());
  CPPUNIT_ASSERT_EQUAL(3, (int) tree.getNumberOfCTDefComponentInstances()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefComponentInstances());
  CPPUNIT_ASSERT(tree.presenceOfDefaultContainer());
  ContainerTest *cont2=new ContainerTest;
  comp2->setContainer(cont2);
  tree=graph->getDeploymentTree();
  CPPUNIT_ASSERT_EQUAL( 0, (int) tree.getFreeDeployableTasks().size());
  CPPUNIT_ASSERT_EQUAL(2, (int) tree.getNumberOfCTDefContainer()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefContainer());
  CPPUNIT_ASSERT_EQUAL(3, (int) tree.getNumberOfCTDefComponentInstances()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefComponentInstances());
  CPPUNIT_ASSERT(tree.presenceOfDefaultContainer());
  comp2->setContainer(cont1);
  tree=graph->getDeploymentTree();
  CPPUNIT_ASSERT_EQUAL( 0, (int) tree.getFreeDeployableTasks().size());
  CPPUNIT_ASSERT_EQUAL(1, (int) tree.getNumberOfCTDefContainer()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefContainer());
  CPPUNIT_ASSERT_EQUAL(3, (int) tree.getNumberOfCTDefComponentInstances()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefComponentInstances());
  CPPUNIT_ASSERT(tree.presenceOfDefaultContainer());
  ContainerTest2 *cont3=new ContainerTest2;
  CPPUNIT_ASSERT_THROW(comp3->setContainer(cont2),YACS::Exception);
  comp3->setContainer(cont3);
  tree=graph->getDeploymentTree();
  vector<Container *> tb=tree.getAllCTDefContainers(); set<Container *> tbs(tb.begin(),tb.end()); set<Container *> tb2;  tb2.insert(cont1); tb2.insert(cont3);
  checkSetsEqual(tbs,tb2);
  vector<ComponentInstance *> tc=tree.getComponentsLinkedToContainer(cont1); set<ComponentInstance *> tcs(tc.begin(),tc.end());
  set<ComponentInstance *> tc2; tc2.insert(comp1); tc2.insert(comp2);
  checkSetsEqual(tcs,tc2);
  CPPUNIT_ASSERT_EQUAL( 0, (int) tree.getFreeDeployableTasks().size());
  CPPUNIT_ASSERT_EQUAL(2, (int) tree.getNumberOfCTDefContainer()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefContainer());
  CPPUNIT_ASSERT_EQUAL(3, (int) tree.getNumberOfCTDefComponentInstances()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefComponentInstances());
  CPPUNIT_ASSERT(!tree.presenceOfDefaultContainer());
  ta=tree.getTasksLinkedToComponent(comp1);
  //Ok now let's see behaviour on cloning.
  Bloc *cl=(Bloc *)graph->clone(0);
  tree=cl->getDeploymentTree();
  CPPUNIT_ASSERT_EQUAL( 0, (int) tree.getFreeDeployableTasks().size());
  CPPUNIT_ASSERT_EQUAL(2, (int) tree.getNumberOfCTDefContainer()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefContainer());
  CPPUNIT_ASSERT_EQUAL(3, (int) tree.getNumberOfCTDefComponentInstances()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefComponentInstances());
  CPPUNIT_ASSERT(!tree.presenceOfDefaultContainer());
  tb=tree.getAllCTDefContainers(); tbs.clear(); tbs.insert(tb.begin(),tb.end());
  checkSetsNotEqual(tbs,tb2);
  delete cl;
  //
  cont1->attachOnCloning(); cont3->attachOnCloning();
  cl=(Bloc *)graph->clone(0);
  tree=cl->getDeploymentTree();
  CPPUNIT_ASSERT_EQUAL( 0, (int) tree.getFreeDeployableTasks().size());
  CPPUNIT_ASSERT_EQUAL(2, (int) tree.getNumberOfCTDefContainer()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefContainer());
  CPPUNIT_ASSERT_EQUAL(3, (int) tree.getNumberOfCTDefComponentInstances()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefComponentInstances());
  CPPUNIT_ASSERT(!tree.presenceOfDefaultContainer());
  tb=tree.getAllCTDefContainers(); tbs.clear(); tbs.insert(tb.begin(),tb.end());
  checkSetsEqual(tbs,tb2);
  delete cl;
  //
  cont1->dettachOnCloning();
  cl=(Bloc *)graph->clone(0);
  tree=cl->getDeploymentTree();
  CPPUNIT_ASSERT_EQUAL( 0, (int) tree.getFreeDeployableTasks().size());
  CPPUNIT_ASSERT_EQUAL(2, (int) tree.getNumberOfCTDefContainer()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefContainer());
  CPPUNIT_ASSERT_EQUAL(3, (int) tree.getNumberOfCTDefComponentInstances()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefComponentInstances());
  CPPUNIT_ASSERT(!tree.presenceOfDefaultContainer());
  tb=tree.getAllCTDefContainers(); tbs.clear(); tbs.insert(tb.begin(),tb.end());
  CPPUNIT_ASSERT(tbs.find(cont1)==tbs.end());
  CPPUNIT_ASSERT(!(tbs.find(cont3)==tbs.end()));
  delete cl;
  cont1->attachOnCloning();
  comp1->attachOnCloning();
  comp2->attachOnCloning();
  cl=(Bloc *)graph->clone(0);
  tree=cl->getDeploymentTree();
  CPPUNIT_ASSERT_EQUAL( 0, (int) tree.getFreeDeployableTasks().size());
  CPPUNIT_ASSERT_EQUAL(2, (int) tree.getNumberOfCTDefContainer()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefContainer());
  CPPUNIT_ASSERT_EQUAL(3, (int) tree.getNumberOfCTDefComponentInstances()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefComponentInstances());
  CPPUNIT_ASSERT(!tree.presenceOfDefaultContainer());
  tc=tree.getComponentsLinkedToContainer(cont1); tcs.clear(); tcs.insert(tc.begin(),tc.end());
  checkSetsEqual(tcs,tc2);
  delete cl;
  comp1->dettachOnCloning();
  comp2->dettachOnCloning();
  cl=(Bloc *)graph->clone(0);
  tree=cl->getDeploymentTree();
  CPPUNIT_ASSERT_EQUAL( 0, (int) tree.getFreeDeployableTasks().size());
  CPPUNIT_ASSERT_EQUAL(2, (int) tree.getNumberOfCTDefContainer()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefContainer());
  CPPUNIT_ASSERT_EQUAL(3, (int) tree.getNumberOfCTDefComponentInstances()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefComponentInstances());
  CPPUNIT_ASSERT(!tree.presenceOfDefaultContainer());
  tc=tree.getComponentsLinkedToContainer(cont1); tcs.clear(); tcs.insert(tc.begin(),tc.end());
  checkSetsNotEqual(tcs,tc2);
  delete cl;
  comp1->attachOnCloning();
  cl=(Bloc *)graph->clone(0);
  tree=cl->getDeploymentTree();
  CPPUNIT_ASSERT_EQUAL( 0, (int) tree.getFreeDeployableTasks().size());
  CPPUNIT_ASSERT_EQUAL(2, (int) tree.getNumberOfCTDefContainer()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefContainer());
  CPPUNIT_ASSERT_EQUAL(3, (int) tree.getNumberOfCTDefComponentInstances()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefComponentInstances());
  CPPUNIT_ASSERT(!tree.presenceOfDefaultContainer());
  tc=tree.getComponentsLinkedToContainer(cont1); tcs.clear(); tcs.insert(tc.begin(),tc.end());
  CPPUNIT_ASSERT(tcs.find(comp1)!=tcs.end());
  CPPUNIT_ASSERT(tcs.find(comp2)==tcs.end());
  delete cl;
  //Final clean up
  comp3->decrRef();
  comp2->decrRef();
  comp1->decrRef();
  cont1->decrRef();
  cont2->decrRef();
  cont3->decrRef();
  delete graph;
}

/*!
 * Test to check that in for compil-time known connectivity of a graph, deployment calculation is OK. 
 */
void EngineIntegrationTest::testForDeployment2()
{
  Bloc *graph=new Bloc("graph");
  ToyNode1S *n1S=new ToyNode1S("T1S"); graph->edAddChild(n1S);
  ForEachLoop *fe1=new ForEachLoop("fe1",Runtime::_tc_double);
  Bloc *b1=new Bloc("B1");
  fe1->edSetNode(b1);
  ToyNode1S *n2S=new ToyNode1S("T2S"); b1->edAddChild(n2S); ToyNode1S *n3S=new ToyNode1S("T3S"); b1->edAddChild(n3S); ToyNode2S *n4S=new ToyNode2S("T4S");
  ToyNode2S *n5S=new ToyNode2S("T5S"); b1->edAddChild(n5S);
  graph->edAddChild(fe1); graph->edAddChild(n4S);
  DeploymentTree tree=graph->getDeploymentTree();
  CPPUNIT_ASSERT(!tree.isNull());//Here placement info.
  CPPUNIT_ASSERT_EQUAL( 5, (int) tree.getFreeDeployableTasks().size());
  CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfCTDefContainer()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefContainer());
  CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfCTDefComponentInstances()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefComponentInstances());
  CPPUNIT_ASSERT(!tree.presenceOfDefaultContainer());
  //Check that compatibility is checked...
  ComponentInstanceTest1 *comp1=new ComponentInstanceTest1("FLICA");
  /*n1S->setComponent(comp1);
  CPPUNIT_ASSERT_THROW(n4S->setComponent(comp1),YACS::Exception);//Impossible not compatible between ToyNode1S and ToyNode2S
  CPPUNIT_ASSERT_THROW(n2S->setComponent(comp1),YACS::Exception);//Failure here to but due to incompatibility of scopes n2S is in a ForEachLoop and not n1S and comp1 is NOT attached here.*/
  n1S->setComponent(comp1);
  comp1->attachOnCloning();
  n2S->setComponent(comp1);//here it's ok because comp1 is attached...
  tree=graph->getDeploymentTree();
  CPPUNIT_ASSERT(!tree.isNull());
  CPPUNIT_ASSERT_EQUAL( 3, (int) tree.getFreeDeployableTasks().size());
  CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfCTDefContainer()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefContainer());
  CPPUNIT_ASSERT_EQUAL(1, (int) tree.getNumberOfCTDefComponentInstances()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefComponentInstances());
  CPPUNIT_ASSERT(tree.presenceOfDefaultContainer());
  //Check that RTO (runtime only) components are detected
  ComponentInstanceTest1 *comp2=new ComponentInstanceTest1("DKCORE");
  n3S->setComponent(comp2);
  tree=graph->getDeploymentTree();
  CPPUNIT_ASSERT(!tree.isNull());
  CPPUNIT_ASSERT_EQUAL( 2, (int) tree.getFreeDeployableTasks().size());
  CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfCTDefContainer()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefContainer());
  CPPUNIT_ASSERT_EQUAL(1, (int) tree.getNumberOfCTDefComponentInstances()); CPPUNIT_ASSERT_EQUAL(1, (int) tree.getNumberOfRTODefComponentInstances());
  ComponentInstanceTest2 *comp3=new ComponentInstanceTest2("DKCORE");
  n4S->setComponent(comp3);
  //CPPUNIT_ASSERT_THROW(n5S->setComponent(comp3),YACS::Exception);//For fun just like 19 lines before.
  comp3->attachOnCloning();
  n5S->setComponent(comp3);//ok
  tree=graph->getDeploymentTree();
  CPPUNIT_ASSERT(!tree.isNull());
  CPPUNIT_ASSERT_EQUAL( 0, (int) tree.getFreeDeployableTasks().size());
  CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfCTDefContainer()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefContainer());
  CPPUNIT_ASSERT_EQUAL(2, (int) tree.getNumberOfCTDefComponentInstances()); CPPUNIT_ASSERT_EQUAL(1, (int) tree.getNumberOfRTODefComponentInstances());
  CPPUNIT_ASSERT(tree.presenceOfDefaultContainer());
  ContainerTest *cont1=new ContainerTest;
  comp1->setContainer(cont1);
  tree=graph->getDeploymentTree();
  CPPUNIT_ASSERT_EQUAL( 0, (int) tree.getFreeDeployableTasks().size());
  CPPUNIT_ASSERT_EQUAL(1, (int) tree.getNumberOfCTDefContainer()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefContainer());
  CPPUNIT_ASSERT_EQUAL(2, (int) tree.getNumberOfCTDefComponentInstances()); CPPUNIT_ASSERT_EQUAL(1, (int) tree.getNumberOfRTODefComponentInstances());
  CPPUNIT_ASSERT(tree.presenceOfDefaultContainer());
  ContainerTest2 *cont3=new ContainerTest2;
  comp3->setContainer(cont3);
  tree=graph->getDeploymentTree();
  CPPUNIT_ASSERT_EQUAL( 0, (int) tree.getFreeDeployableTasks().size());
  CPPUNIT_ASSERT_EQUAL(2, (int) tree.getNumberOfCTDefContainer()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefContainer());
  CPPUNIT_ASSERT_EQUAL(2, (int) tree.getNumberOfCTDefComponentInstances()); CPPUNIT_ASSERT_EQUAL(1, (int) tree.getNumberOfRTODefComponentInstances());
  CPPUNIT_ASSERT(tree.presenceOfDefaultContainer());
  ContainerTest *cont2=new ContainerTest;
  comp2->setContainer(cont2);
  tree=graph->getDeploymentTree();
  CPPUNIT_ASSERT_EQUAL( 0, (int) tree.getFreeDeployableTasks().size());
  CPPUNIT_ASSERT_EQUAL(2, (int) tree.getNumberOfCTDefContainer()); CPPUNIT_ASSERT_EQUAL(1, (int) tree.getNumberOfRTODefContainer());
  CPPUNIT_ASSERT_EQUAL(2, (int) tree.getNumberOfCTDefComponentInstances()); CPPUNIT_ASSERT_EQUAL(1, (int) tree.getNumberOfRTODefComponentInstances());
  CPPUNIT_ASSERT(!tree.presenceOfDefaultContainer());
  vector<Container *> conts=tree.getAllCTDefContainers(); set<Container *> contsS(conts.begin(),conts.end());
  set<Container *> expectedContSet; expectedContSet.insert(cont1); expectedContSet.insert(cont3);
  checkSetsEqual(expectedContSet,contsS);
  conts=tree.getAllRTODefContainers(); contsS.clear(); contsS.insert(conts.begin(),conts.end());
  expectedContSet.clear(); expectedContSet.insert(cont2);
  checkSetsEqual(expectedContSet,contsS);
  cont2->attachOnCloning();
  tree=graph->getDeploymentTree();
  CPPUNIT_ASSERT_EQUAL( 0, (int) tree.getFreeDeployableTasks().size());
  tree.getNumberOfCTDefContainer();
  CPPUNIT_ASSERT_EQUAL(3, (int) tree.getNumberOfCTDefContainer()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefContainer());
  CPPUNIT_ASSERT_EQUAL(2, (int) tree.getNumberOfCTDefComponentInstances()); CPPUNIT_ASSERT_EQUAL(1, (int) tree.getNumberOfRTODefComponentInstances());
  CPPUNIT_ASSERT(!tree.presenceOfDefaultContainer());
  comp2->attachOnCloning();
  tree.getNumberOfCTDefContainer();
  CPPUNIT_ASSERT_EQUAL(3, (int) tree.getNumberOfCTDefContainer()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefContainer());
  CPPUNIT_ASSERT_EQUAL(3, (int) tree.getNumberOfCTDefComponentInstances()); CPPUNIT_ASSERT_EQUAL(0, (int) tree.getNumberOfRTODefComponentInstances());
  CPPUNIT_ASSERT(!tree.presenceOfDefaultContainer());
  //clean up
  cont1->decrRef();
  cont2->decrRef();
  cont3->decrRef();
  comp1->decrRef();
  comp2->decrRef();
  comp3->decrRef();
  delete graph;
}

/*!
 * Checking firstley fondamentals tools for check consistency. Essentially CF dependancy by visiting CF Graph.
 * Secondly check that data link at one level is correctly dealed.
 */
void EngineIntegrationTest::testForCheckConsistency1()
{
  LinkInfo info(LinkInfo::ALL_DONT_STOP);
  ToyNode *n1=new ToyNode("T1"); ToyNode *n5=new ToyNode("T5"); ToyNode *n9=new ToyNode("T9");
  ToyNode *n2=new ToyNode("T2"); ToyNode *n6=new ToyNode("T6"); ToyNode *n10=new ToyNode("T10");
  ToyNode *n3=new ToyNode("T3"); ToyNode *n7=new ToyNode("T7"); ToyNode *n11=new ToyNode("T11");
  ToyNode *n4=new ToyNode("T4"); ToyNode *n8=new ToyNode("T8"); ToyNode *n12=new ToyNode("T12");
  Bloc *graph=new Bloc("Global");
  graph->edAddChild(n1); graph->edAddChild(n2); graph->edAddChild(n3); graph->edAddChild(n4); graph->edAddChild(n5); graph->edAddChild(n6);
  graph->edAddChild(n7); graph->edAddChild(n8); graph->edAddChild(n9); graph->edAddChild(n10); graph->edAddChild(n11); graph->edAddChild(n12);
  graph->edAddCFLink(n1,n2); graph->edAddCFLink(n1,n4); graph->edAddCFLink(n1,n3); graph->edAddCFLink(n3,n10); graph->edAddCFLink(n4,n6);
  graph->edAddCFLink(n2,n10); graph->edAddCFLink(n6,n7); graph->edAddCFLink(n5,n8); graph->edAddCFLink(n9,n6); graph->edAddCFLink(n3,n5);
  graph->edAddCFLink(n5,n6); graph->edAddCFLink(n10,n7); graph->edAddCFLink(n10,n11); graph->edAddCFLink(n11,n12);
  graph->edAddCFLink(n12,n5);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT_EQUAL(2,(int)info.getNumberOfInfoLinks(I_CF_USELESS)); CPPUNIT_ASSERT_EQUAL(0,(int)info.getNumberOfWarnLinksGrp(W_ALL));
  CPPUNIT_ASSERT_EQUAL(0,(int)info.getNumberOfErrLinks(E_ALL));
  set< pair<Node *, Node *> > s=info.getInfoUselessLinks();
  set< pair<Node *, Node *> > s2;  s2.insert(pair<Node*,Node*>(n3,n5)); s2.insert(pair<Node*,Node*>(n10,n7));
  checkSetsEqual< pair<Node *, Node *> >(s,s2);
  set<Node *> s3; set<Node *> setExpected; setExpected.insert(n2); setExpected.insert(n4); setExpected.insert(n3); setExpected.insert(n10);
  setExpected.insert(n11); setExpected.insert(n12); setExpected.insert(n5); setExpected.insert(n6); setExpected.insert(n7); setExpected.insert(n8);
  map<Node *, set<Node *> > accelStr;
  graph->findAllNodesStartingFrom<true>(n1,s3,accelStr,info); accelStr.clear();
  checkSetsEqual(setExpected,s3); s3.clear(); setExpected.clear();
  graph->findAllNodesStartingFrom<true>(n7,s3,accelStr,info); accelStr.clear();
  checkSetsEqual(setExpected,s3); s3.clear(); setExpected.clear();
  graph->findAllNodesStartingFrom<false>(n1,s3,accelStr,info); accelStr.clear();
  checkSetsEqual(setExpected,s3); s3.clear(); setExpected.clear();
  list< vector<Node *> > vec;
  graph->findAllPathsStartingFrom<false>(n7, vec,accelStr);
  graph->findAllNodesStartingFrom<false>(n7,s3,accelStr,info); accelStr.clear();//setExpected=graph->edGetDirectDescendants(); setExpected.erase(n8); setExpected.erase(n7);
  //checkSetsEqual(setExpected,s3); 
  s3.clear(); setExpected.clear();
  //Testing good reinitialisation
  setExpected.insert(n2); setExpected.insert(n4); setExpected.insert(n3); setExpected.insert(n10);
  setExpected.insert(n11); setExpected.insert(n12); setExpected.insert(n5); setExpected.insert(n6); setExpected.insert(n7); setExpected.insert(n8);
  graph->findAllNodesStartingFrom<true>(n1,s3,accelStr,info); accelStr.clear();
  checkSetsEqual(setExpected,s3); s3.clear(); setExpected.clear();
  graph->findAllNodesStartingFrom<false>(n1,s3,accelStr,info); accelStr.clear();
  checkSetsEqual(setExpected,s3); s3.clear(); setExpected.clear();
  //End test good reinitialisation
  graph->checkConsistency(info);
  CPPUNIT_ASSERT_EQUAL(2,(int)info.getNumberOfInfoLinks(I_CF_USELESS)); CPPUNIT_ASSERT_EQUAL(0,(int)info.getNumberOfWarnLinksGrp(W_ALL));
  CPPUNIT_ASSERT_EQUAL(0,(int)info.getNumberOfErrLinks(E_ALL));
  s=info.getInfoUselessLinks();
  s2.clear();  s2.insert(pair<Node*,Node*>(n3,n5)); s2.insert(pair<Node*,Node*>(n10,n7));
  checkSetsEqual< pair<Node *, Node *> >(s,s2);
  //n3->n5 and n10->n7 have been detected to be useless... try to remove them and relaunch : normally no useless links should be detected
  graph->edRemoveCFLink(n3,n5); graph->edRemoveCFLink(n10,n7);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT_EQUAL(0,(int)info.getNumberOfInfoLinks(I_CF_USELESS)); CPPUNIT_ASSERT_EQUAL(0,(int)info.getNumberOfWarnLinksGrp(W_ALL));
  CPPUNIT_ASSERT_EQUAL(0,(int)info.getNumberOfErrLinks(E_ALL));
  //Playing with check at one level of scope.
  InputPort *i11_1=n11->edAddInputPort("i1",Runtime::_tc_double);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT_EQUAL(0,(int)info.getNumberOfInfoLinks(I_ALL)); CPPUNIT_ASSERT_EQUAL(0,(int)info.getNumberOfWarnLinksGrp(W_ALL));
  CPPUNIT_ASSERT_EQUAL(1,(int)info.getNumberOfErrLinks(E_ALL)); CPPUNIT_ASSERT_EQUAL(1,(int)info.getNumberOfErrLinks(E_ALL));
  CPPUNIT_ASSERT_EQUAL(1,(int)info.getNumberOfErrLinks(E_NEVER_SET_INPUTPORT));
  pair<OutPort *, InPort *> p1=info.getErrLink(0,E_NEVER_SET_INPUTPORT); pair<OutPort *, InPort *> p2( reinterpret_cast<OutPort *>(0), i11_1 );
  CPPUNIT_ASSERT(p1==p2);
  //
  i11_1->edInit(3.14);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT_EQUAL(0,(int)info.getNumberOfInfoLinks(I_ALL)); CPPUNIT_ASSERT_EQUAL(0,(int)info.getNumberOfWarnLinksGrp(W_ALL));
  CPPUNIT_ASSERT_EQUAL(0,(int)info.getNumberOfErrLinks(E_ALL));
  i11_1->edRemoveManInit();
  graph->checkConsistency(info);
  CPPUNIT_ASSERT_EQUAL(0,(int)info.getNumberOfInfoLinks(I_ALL)); CPPUNIT_ASSERT_EQUAL(0,(int)info.getNumberOfWarnLinksGrp(W_ALL));
  CPPUNIT_ASSERT_EQUAL(1,(int)info.getNumberOfErrLinks(E_ALL)); CPPUNIT_ASSERT_EQUAL(1,(int)info.getNumberOfErrLinks(E_ALL));
  CPPUNIT_ASSERT_EQUAL(1,(int)info.getNumberOfErrLinks(E_NEVER_SET_INPUTPORT));
  //only back defined
  OutputPort *o11_1=n11->edAddOutputPort("o11_1",Runtime::_tc_double);
  graph->edAddLink(o11_1,i11_1);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT_EQUAL(1,(int)info.getNumberOfInfoLinks(I_ALL)); CPPUNIT_ASSERT_EQUAL(1,(int)info.getNumberOfInfoLinks(I_BACK)); CPPUNIT_ASSERT_EQUAL(0,(int)info.getNumberOfWarnLinksGrp(W_ALL));
  CPPUNIT_ASSERT_EQUAL(1,(int)info.getNumberOfErrLinks(E_ALL)); CPPUNIT_ASSERT_EQUAL(1,(int)info.getNumberOfErrLinks(E_ONLY_BACKWARD_DEFINED));
  graph->edRemoveLink(o11_1,i11_1);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT_EQUAL(0,(int)info.getNumberOfInfoLinks(I_ALL)); CPPUNIT_ASSERT_EQUAL(0,(int)info.getNumberOfWarnLinksGrp(W_ALL));
  CPPUNIT_ASSERT_EQUAL(1,(int)info.getNumberOfErrLinks(E_ALL)); CPPUNIT_ASSERT_EQUAL(1,(int)info.getNumberOfErrLinks(E_ALL));
  CPPUNIT_ASSERT_EQUAL(1,(int)info.getNumberOfErrLinks(E_NEVER_SET_INPUTPORT));
  //
  OutputPort *o10_1=n10->edAddOutputPort("o10_1",Runtime::_tc_double);
  graph->edAddLink(o10_1,i11_1);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT_EQUAL(0,(int)info.getNumberOfInfoLinks(I_ALL)); CPPUNIT_ASSERT_EQUAL(0,(int)info.getNumberOfWarnLinksGrp(W_ALL));
  CPPUNIT_ASSERT_EQUAL(0,(int)info.getNumberOfErrLinks(E_ALL));
  OutputPort *o2_1=n2->edAddOutputPort("o2_1",Runtime::_tc_double);
  graph->edAddLink(o2_1,i11_1);
  // useless
  graph->checkConsistency(info);
  CPPUNIT_ASSERT_EQUAL(1,(int)info.getNumberOfInfoLinks(I_ALL)); CPPUNIT_ASSERT_EQUAL(1,(int)info.getNumberOfInfoLinks(I_USELESS)); CPPUNIT_ASSERT_EQUAL(0,(int)info.getNumberOfWarnLinksGrp(W_ALL));
  CPPUNIT_ASSERT_EQUAL(0,(int)info.getNumberOfErrLinks(E_ALL));
  // many useless
  OutputPort *o1_1=n1->edAddOutputPort("o1_1",Runtime::_tc_double);
  graph->edAddLink(o1_1,i11_1);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT_EQUAL(2,(int)info.getNumberOfInfoLinks(I_ALL)); CPPUNIT_ASSERT_EQUAL(2,(int)info.getNumberOfInfoLinks(I_USELESS)); CPPUNIT_ASSERT_EQUAL(0,(int)info.getNumberOfWarnLinksGrp(W_ALL));
  CPPUNIT_ASSERT_EQUAL(0,(int)info.getNumberOfErrLinks(E_ALL));
  // collapse and useless ; useless
  OutputPort *o3_1=n3->edAddOutputPort("o3_1",Runtime::_tc_double);
  graph->edAddLink(o3_1,i11_1);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT_EQUAL(1,(int)info.getNumberOfInfoLinks(I_ALL)); CPPUNIT_ASSERT_EQUAL(1,(int)info.getNumberOfInfoLinks(I_USELESS)); CPPUNIT_ASSERT_EQUAL(1,(int)info.getNumberOfWarnLinksGrp(W_ALL));
  CPPUNIT_ASSERT_EQUAL(0,(int)info.getNumberOfErrLinks(E_ALL)); CPPUNIT_ASSERT_EQUAL(1,(int)info.getNumberOfWarnLinksGrp(W_COLLAPSE_AND_USELESS));
  // collapse ; useless
  graph->edRemoveLink(o10_1,i11_1);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT_EQUAL(1,(int)info.getNumberOfInfoLinks(I_ALL)); CPPUNIT_ASSERT_EQUAL(1,(int)info.getNumberOfInfoLinks(I_USELESS)); CPPUNIT_ASSERT_EQUAL(1,(int)info.getNumberOfWarnLinksGrp(W_ALL));
  CPPUNIT_ASSERT_EQUAL(0,(int)info.getNumberOfErrLinks(E_ALL)); CPPUNIT_ASSERT_EQUAL(1,(int)info.getNumberOfWarnLinksGrp(W_COLLAPSE));
  // collapse ; useless ; unpredictable'
  OutputPort *o4_1=n4->edAddOutputPort("o4_1",Runtime::_tc_double);
  graph->edAddLink(o4_1,i11_1);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT_EQUAL(1,(int)info.getNumberOfInfoLinks(I_ALL)); CPPUNIT_ASSERT_EQUAL(1,(int)info.getNumberOfInfoLinks(I_USELESS)); CPPUNIT_ASSERT_EQUAL(1,(int)info.getNumberOfWarnLinksGrp(W_ALL));
  CPPUNIT_ASSERT_EQUAL(1,(int)info.getNumberOfErrLinks(E_ALL)); CPPUNIT_ASSERT_EQUAL(1,(int)info.getNumberOfErrLinks(E_UNPREDICTABLE_FED)); CPPUNIT_ASSERT_EQUAL(1,(int)info.getNumberOfWarnLinksGrp(W_COLLAPSE));
  CPPUNIT_ASSERT( (pair<OutPort*,InPort *>(o4_1,i11_1)==info.getErrLink(0,E_UNPREDICTABLE_FED)) );
  // restart all
  graph->edRemoveLink(o3_1,i11_1); graph->edRemoveLink(o4_1,i11_1); graph->edRemoveLink(o1_1,i11_1);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT_EQUAL(0,(int)info.getNumberOfInfoLinks(I_ALL)); CPPUNIT_ASSERT_EQUAL(0,(int)info.getNumberOfWarnLinksGrp(W_ALL)); CPPUNIT_ASSERT_EQUAL(0,(int)info.getNumberOfErrLinks(E_ALL));
  // back link 
  OutputPort *o5_1=n5->edAddOutputPort("o5_1",Runtime::_tc_double); graph->edAddLink(o5_1,i11_1);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT_EQUAL(1,(int)info.getNumberOfInfoLinks(I_ALL)); CPPUNIT_ASSERT_EQUAL(0,(int)info.getNumberOfWarnLinksGrp(W_ALL)); CPPUNIT_ASSERT_EQUAL(0,(int)info.getNumberOfErrLinks(E_ALL));
  CPPUNIT_ASSERT_EQUAL(1,(int)info.getNumberOfInfoLinks(I_BACK));
  CPPUNIT_ASSERT( (pair<OutPort*,InPort *>(o5_1,i11_1)==info.getInfoLink(0,I_BACK)) );
  // back - useless link
  OutputPort *o12_1=n12->edAddOutputPort("o12_1",Runtime::_tc_double); graph->edAddLink(o12_1,i11_1);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT_EQUAL(2,(int)info.getNumberOfInfoLinks(I_ALL)); CPPUNIT_ASSERT_EQUAL(0,(int)info.getNumberOfWarnLinksGrp(W_ALL)); CPPUNIT_ASSERT_EQUAL(0,(int)info.getNumberOfErrLinks(E_ALL));
  CPPUNIT_ASSERT_EQUAL(1,(int)info.getNumberOfInfoLinks(I_BACK)); CPPUNIT_ASSERT_EQUAL(1,(int)info.getNumberOfInfoLinks(I_BACK_USELESS));
  CPPUNIT_ASSERT( (pair<OutPort*,InPort *>(o5_1,i11_1)==info.getInfoLink(0,I_BACK)) );
  CPPUNIT_ASSERT( (pair<OutPort*,InPort *>(o12_1,i11_1)==info.getInfoLink(0,I_BACK_USELESS)) );
  graph->edAddLink(o11_1,i11_1);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT_EQUAL(3,(int)info.getNumberOfInfoLinks(I_ALL)); CPPUNIT_ASSERT_EQUAL(0,(int)info.getNumberOfWarnLinksGrp(W_ALL)); CPPUNIT_ASSERT_EQUAL(0,(int)info.getNumberOfErrLinks(E_ALL));
  CPPUNIT_ASSERT_EQUAL(1,(int)info.getNumberOfInfoLinks(I_BACK)); CPPUNIT_ASSERT_EQUAL(2,(int)info.getNumberOfInfoLinks(I_BACK_USELESS));
  delete graph;
}

void EngineIntegrationTest::testForCheckConsistency2()
{
  LinkInfo info(LinkInfo::ALL_DONT_STOP);
  Bloc *graph=new Bloc("Global");
  ToyNode *n1=new ToyNode("T1"); ToyNode *n5=new ToyNode("T5"); ToyNode *n9=new ToyNode("T9");
  ToyNode *n2=new ToyNode("T2"); ToyNode *n6=new ToyNode("T6"); ToyNode *n10=new ToyNode("T10");
  ToyNode *n3=new ToyNode("T3"); ToyNode *n7=new ToyNode("T7"); ToyNode *n11=new ToyNode("T11");
  ToyNode *n4=new ToyNode("T4"); ToyNode *n8=new ToyNode("T8"); ToyNode *n12=new ToyNode("T12");
  graph->edAddChild(n1); graph->edAddChild(n2); graph->edAddChild(n3); graph->edAddChild(n4); graph->edAddChild(n5); graph->edAddChild(n6);
  graph->edAddChild(n7); graph->edAddChild(n8); graph->edAddChild(n9); graph->edAddChild(n10); graph->edAddChild(n11); graph->edAddChild(n12);
  graph->edAddCFLink(n1,n10); 
  graph->edAddCFLink(n10,n8); graph->edAddCFLink(n8,n7); graph->edAddCFLink(n10,n7); graph->edAddCFLink(n1,n3);
  graph->checkConsistency(info);
  set< pair<Node *, Node *> > s=info.getInfoUselessLinks(); set< pair<Node *, Node *> > s2;  s2.insert(pair<Node*,Node*>(n10,n7));
  checkSetsEqual< pair<Node *, Node *> >(s,s2);
  set<Node *> s3; set<Node *> setExpected; map<Node *, set<Node *> > accelStr;
  graph->findAllNodesStartingFrom<true>(n1,s3,accelStr,info); accelStr.clear();
  delete graph;
}

/*!
 * Testing forwarding of check consistency.
 */
void EngineIntegrationTest::testForCheckConsistency3()
{
  LinkInfo info(LinkInfo::ALL_DONT_STOP);
  ToyNode *n1=new ToyNode("T1"); ToyNode *n2=new ToyNode("T2"); ToyNode *n3=new ToyNode("T3");
  Bloc *graph=new Bloc("Global");
  Bloc *g1=new Bloc("G1"); Bloc *g2=new Bloc("G2");
  graph->edAddChild(g1);
  graph->edAddChild(g2);
  g1->edAddChild(n1);
  g2->edAddChild(n2);
  g2->edAddChild(n3);
  g2->edAddCFLink(n2,n3);
  graph->edAddCFLink(g1,g2);
  InputPort *i1_3=n3->edAddInputPort("i1_3",Runtime::_tc_double);
  OutputPort *o1_1=n1->edAddOutputPort("o1_1",Runtime::_tc_double);
  OutputPort *o2_1=n2->edAddOutputPort("o1_2",Runtime::_tc_double);
  graph->edAddLink(o1_1,i1_3);
  graph->edAddLink(o2_1,i1_3);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  CPPUNIT_ASSERT(1==info.getNumberOfInfoLinks(I_ALL)); CPPUNIT_ASSERT(1==info.getNumberOfInfoLinks(I_USELESS));
  pair<OutPort *, InPort *> p=info.getInfoLink(0,I_USELESS);
  CPPUNIT_ASSERT(p.first==o1_1); CPPUNIT_ASSERT(p.second==i1_3);
  //now substituting g2 and graph in superv graph (by keeping same address)
  graph->edRemoveChild(g1);
  graph->edRemoveChild(g2);
  g2->edRemoveChild(n2);
  g2->edRemoveChild(n3);
  g2->edAddChild(g1); g2->edAddChild(graph);
  graph->edAddChild(n2); graph->edAddChild(n3);
  graph->edAddCFLink(n2,n3);
  g2->edAddCFLink(g1,graph);
  g2->edAddLink(o1_1,i1_3);
  g2->edAddLink(o2_1,i1_3);
  g2->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  CPPUNIT_ASSERT(1==info.getNumberOfInfoLinks(I_ALL)); CPPUNIT_ASSERT(1==info.getNumberOfInfoLinks(I_USELESS));
  p=info.getInfoLink(0,I_USELESS);
  CPPUNIT_ASSERT(p.first==o1_1); CPPUNIT_ASSERT(p.second==i1_3);
  g2->edRemoveLink(o1_1,i1_3);
  g2->checkConsistency(info);
  CPPUNIT_ASSERT(0==info.getNumberOfInfoLinks(I_ALL));
  //Now back-links
  InputPort *i1_1=n1->edAddInputPort("i1_1",Runtime::_tc_double);
  InputPort *i1_2=n2->edAddInputPort("i1_2",Runtime::_tc_double);
  OutputPort *o1_3=n3->edAddOutputPort("o1_3",Runtime::_tc_double);
  g2->edAddLink(o1_3,i1_1);
  g2->edAddLink(o1_3,i1_2);
  g2->checkConsistency(info);
  CPPUNIT_ASSERT(2==info.getNumberOfErrLinks(E_ALL)); CPPUNIT_ASSERT(2==info.getNumberOfErrLinks(E_ONLY_BACKWARD_DEFINED));
  i1_1->edInit(0.); i1_2->edInit(0.);
  g2->checkConsistency(info);
  CPPUNIT_ASSERT(2==info.getNumberOfInfoLinks(I_ALL)); CPPUNIT_ASSERT(2==info.getNumberOfInfoLinks(I_BACK));
  delete g2;
}

void EngineIntegrationTest::testForCheckConsistency4()
{
  LinkInfo info(LinkInfo::ALL_DONT_STOP);
  ToyNode *n1=new ToyNode("T1"); ToyNode *n2=new ToyNode("T2"); ToyNode *n3=new ToyNode("T3"); ToyNode *n4=new ToyNode("T4");
  Bloc *graph=new Bloc("Global");
  InputPort *i1_4=n4->edAddInputPort("i1_4",Runtime::_tc_double);
  OutputPort *o1_1=n1->edAddOutputPort("o1_1",Runtime::_tc_double);
  OutputPort *o1_2=n2->edAddOutputPort("o1_2",Runtime::_tc_double);
  OutputPort *o1_3=n3->edAddOutputPort("o1_3",Runtime::_tc_double);
  Switch *mySwitch=new Switch("mySwitch");
  mySwitch->edSetNode(0,n1);
  mySwitch->edSetNode(7,n2);
  graph->edAddChild(n3);
  graph->edAddChild(n4);
  graph->edAddChild(mySwitch);
  graph->edAddCFLink(mySwitch,n4);
  graph->edAddCFLink(n3,n4);
  graph->edAddLink(o1_1,i1_4);
  graph->edAddLink(o1_2,i1_4);
  mySwitch->edGetConditionPort()->edInit(7);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(!info.areWarningsOrErrors());
  graph->edAddLink(o1_3,i1_4);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(1==info.getNumberOfWarnLinksGrp(W_ALL));
  graph->edRemoveLink(o1_1,i1_4);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(1==info.getNumberOfWarnLinksGrp(W_ALL));
  CPPUNIT_ASSERT(1==info.getNumberOfErrLinks(E_ALL));
  CPPUNIT_ASSERT(1==info.getNumberOfErrLinks(E_UNCOMPLETE_SW));
  CPPUNIT_ASSERT(info.getErrRepr()=="For link to i1_4 of node T4 the cases of switch node named mySwitch do not define links for following cases ids :0 \n");
  graph->edRemoveChild(mySwitch);
  Bloc *bloc4Fun=new Bloc("bloc4Fun");
  graph->edAddChild(bloc4Fun);
  bloc4Fun->edAddChild(mySwitch);
  graph->edAddCFLink(bloc4Fun,n4);
  graph->edAddLink(o1_1,i1_4);
  graph->edAddLink(o1_2,i1_4);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(1==info.getNumberOfWarnLinksGrp(W_ALL));
  graph->edRemoveLink(o1_1,i1_4);
  graph->checkConsistency(info);
  CPPUNIT_ASSERT(1==info.getNumberOfWarnLinksGrp(W_ALL));
  CPPUNIT_ASSERT(1==info.getNumberOfErrLinks(E_ALL));
  CPPUNIT_ASSERT(1==info.getNumberOfErrLinks(E_UNCOMPLETE_SW));
  delete graph;
}

void EngineIntegrationTest::testRemoveRuntime()
{
  Runtime* r=YACS::ENGINE::getRuntime();
  delete r;
}
