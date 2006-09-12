#ifndef __TESTBLOC_HXX__
#define __TESTBLOC_HXX__

#include "Bloc.hxx"
#include "Data.hxx"
#include "ToyNode.hxx"
#include "Executor.hxx"
#include "Exception.hxx"
#include "InputPort.hxx"
#include "OutputPort.hxx"
//
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/ui/text/TestRunner.h>
#include <sstream>
#include <string.h>
//

#define DBL_PRECISION_COMPARE 1e-15

using namespace YACS::ENGINE;

class BlocTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( BlocTest );
  CPPUNIT_TEST( test1 );
  CPPUNIT_TEST( test2 );
  CPPUNIT_TEST( test3 );
  CPPUNIT_TEST_SUITE_END();
public: 
  void tearDown();
  void test1();
  void test2();
  void test3();
};

void BlocTest::tearDown()
{
}

//Simplest test to test basic mechanisms like Data, initialisation of ports.
void BlocTest::test1()
{
  ToyNode *n1=new ToyNode("T1");
  InputPort *i8=n1->edAddInputPort("o");
  InputPort *i9=n1->edAddInputPort("p");
  Bloc *graph=new Bloc("toto");
  graph->edAddChild(n1);
  OutputPort *o1=n1->edAddOutputPort("b");
  Data v1(5.67);
  i9->edInit(v1);
  v1=2.78;
  i8->edInit(v1);
  Executor exe;
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( o1->foGet().exGet<double>(),8.45,DBL_PRECISION_COMPARE );
  OutputPort *o2=n1->edAddOutputPort("c");
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( o1->foGet().exGet<double>(),4.225,DBL_PRECISION_COMPARE );
  delete graph;
}

//Only one level(hierarchy) simple graph. Tests DF and CF links.
void BlocTest::test2()
{
  ToyNode *n1=new ToyNode("T1");
  ToyNode *n2=new ToyNode("T2",50);
  ToyNode *n3=new ToyNode("T3",20);
  ToyNode *n4=new ToyNode("T4");
  Bloc *graph=new Bloc("Global");
  graph->edAddChild(n1); graph->edAddChild(n2); graph->edAddChild(n3); graph->edAddChild(n4);
  InputPort *i1_a=n1->edAddInputPort("a"); InputPort *i1_b=n1->edAddInputPort("b"); OutputPort *o1_j=n1->edAddOutputPort("j"); OutputPort *o1_k=n1->edAddOutputPort("k");
  InputPort *i2_c=n2->edAddInputPort("c"); InputPort *i2_d=n2->edAddInputPort("d"); OutputPort *o2_f=n2->edAddOutputPort("f");
  InputPort *i3_e=n3->edAddInputPort("e"); OutputPort *o3_g=n3->edAddOutputPort("g"); OutputPort *o3_h=n3->edAddOutputPort("h");
  InputPort *i4_l=n4->edAddInputPort("l"); InputPort *i4_m=n4->edAddInputPort("m"); OutputPort *o4_i=n4->edAddOutputPort("i");
  //Retrieving gates
  InGate *iN1=n1->getInGate(); InGate *iN2=n2->getInGate(); InGate *iN3=n3->getInGate(); InGate *iN4=n4->getInGate();
  OutGate *oN1=n1->getOutGate(); OutGate *oN2=n2->getOutGate(); OutGate *oN3=n3->getOutGate(); OutGate *oN4=n4->getOutGate();
  Data v1(1.2);
  i1_a->edInit(v1);
  v1=3.4;
  i1_b->edInit(v1);
  v1=7;
  i2_d->edInit(v1);
  //DF                                                                      //CF
  graph->edAddLink(o1_j,i2_c);                                              graph->edAddLink(oN1,iN2);
  graph->edAddLink(o1_k,i3_e);                                              graph->edAddLink(oN1,iN3);
  graph->edAddLink(o2_f,i4_l);                                              graph->edAddLink(oN2,iN4);
  graph->edAddLink(o3_g,i4_m);                                              graph->edAddLink(oN3,iN4);
  Executor exe;
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( o4_i->foGet().exGet<double>(),10.45,DBL_PRECISION_COMPARE );
  n2->setTime(0); n3->setTime(0);
  exe.RunW(graph);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( o4_i->foGet().exGet<double>(),10.45,DBL_PRECISION_COMPARE );
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
void BlocTest::test3()
{
  Bloc *toto=new Bloc("toto");
  Bloc *tata=new Bloc("tata");
  Bloc *titi=new Bloc("titi");
  ToyNode *n2=new ToyNode("T2"); ToyNode *n3=new ToyNode("T3"); ToyNode *n4=new ToyNode("T4"); ToyNode *n5=new ToyNode("T5"); ToyNode *n6=new ToyNode("T6");
  ToyNode *n7=new ToyNode("T7",20); ToyNode *n8=new ToyNode("T8");
  toto->edAddChild(titi); titi->edAddChild(tata); titi->edAddChild(n3); toto->edAddChild(n2); tata->edAddChild(n4); tata->edAddChild(n5); tata->edAddChild(n6);
  titi->edAddChild(n7); titi->edAddChild(n8);
  toto->edAddCFLink(n2,titi); toto->edAddCFLink(n3,tata); toto->edAddCFLink(n5,n4); titi->edAddCFLink(n7,n8); titi->edAddCFLink(tata,n8);
  //
  InputPort *i2_a=n2->edAddInputPort("a"); OutputPort *o2_a=n2->edAddOutputPort("a"); OutputPort *o2_b=n2->edAddOutputPort("b");
  InputPort *i3_a=n3->edAddInputPort("a"); OutputPort *o3_a=n3->edAddOutputPort("a"); OutputPort *o3_b=n3->edAddOutputPort("b");
  InputPort *i4_a=n4->edAddInputPort("a"); InputPort *i4_b=n4->edAddInputPort("b"); OutputPort *o4_a=n4->edAddOutputPort("a"); OutputPort *o4_b=n4->edAddOutputPort("b");
  InputPort *i5_a=n5->edAddInputPort("a"); InputPort *i5_b=n5->edAddInputPort("b"); OutputPort *o5_a=n5->edAddOutputPort("a");
  InputPort *i6_a=n6->edAddInputPort("a"); InputPort *i6_b=n6->edAddInputPort("b"); OutputPort *o6_a=n6->edAddOutputPort("a");
  InputPort *i7_a=n7->edAddInputPort("a"); OutputPort *o7_a=n7->edAddOutputPort("a");
  InputPort *i8_a=n8->edAddInputPort("a"); InputPort *i8_b=n8->edAddInputPort("b"); OutputPort *o8_a=n8->edAddOutputPort("a");
  //
  toto->edAddLink(o2_a,i3_a);  toto->edAddLink(o3_a,i5_a); toto->edAddLink(o2_b,i5_b); toto->edAddLink(o2_a,i4_b);
  toto->edAddLink(o3_b,i6_a); toto->edAddLink(o6_a,i8_a); toto->edAddLink(o7_a,i8_b);
  //
  Data v1(1.2);
  i2_a->edInit(v1);
  v1=7;
  i6_b->edInit(v1);
  i4_a->edInit(v1);
  v1=3;
  i7_a->edInit(v1);
  CPPUNIT_ASSERT( toto->getNumberOfCFLinks()== 1 ); CPPUNIT_ASSERT( titi->getNumberOfCFLinks()== 3 ); CPPUNIT_ASSERT( tata->getNumberOfCFLinks()== 1 );
  Executor exe;
  exe.RunW(toto);
  CPPUNIT_ASSERT_DOUBLES_EQUAL( o6_a->foGet().exGet<double>(),7.3,DBL_PRECISION_COMPARE );
  CPPUNIT_ASSERT_DOUBLES_EQUAL( o4_a->foGet().exGet<double>(),3.8,DBL_PRECISION_COMPARE );
  CPPUNIT_ASSERT_DOUBLES_EQUAL( o8_a->foGet().exGet<double>(),10.3,DBL_PRECISION_COMPARE );
  delete toto;
}

int main()
{
  CppUnit::TextUi::TestRunner runner;
  runner.addTest( BlocTest::suite() );
  runner.run();
  return 0;
}


#endif
