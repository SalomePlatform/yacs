#include "Node.hxx"
#include "Data.hxx"
#include "InGate.hxx"
#include "OutGate.hxx"
#include "InputPort.hxx"
#include "OutputPort.hxx"
//
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/ui/text/TestRunner.h>
#include <sstream>
//

using namespace YACS::ENGINE;

class PortsTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( PortsTest );
  CPPUNIT_TEST( testControlFlow );
  CPPUNIT_TEST( testDataFlow1 );
  CPPUNIT_TEST_EXCEPTION( testDataFlowTypeCheck, YACS::ENGINE::ConversionException);
  CPPUNIT_TEST_SUITE_END();
public: 
  void tearDown();
  void testControlFlow();
  void testDataFlow1();
  void testDataFlowTypeCheck();
};

void PortsTest::tearDown()
{
}

void PortsTest::testControlFlow()
{
  Node *n1=0,*n2=0,*n3=0,*n4=0;
  OutGate *o1=new OutGate(n1);
  OutGate *o2=new OutGate(n2);
  OutGate *o3=new OutGate(n3);
  OutGate *o4=new OutGate(n4);
  InGate *i1=new InGate(n1);
  InGate *i2=new InGate(n2);
  InGate *i3=new InGate(n3);
  InGate *i4=new InGate(n4);
  // Simuling edition
  o1->edAddInGate(i2); o1->edAddInGate(i3);
  o2->edAddInGate(i2); o2->edAddInGate(i4);
  o3->edAddInGate(i2); o3->edAddInGate(i4); o3->edAddInGate(i3);
  o4->edAddInGate(i2); o4->edAddInGate(i4);
  // Simuling execution
  CPPUNIT_ASSERT( i1->exIsReady() ); CPPUNIT_ASSERT( !i2->exIsReady() ); CPPUNIT_ASSERT( !i3->exIsReady() ); CPPUNIT_ASSERT( !i4->exIsReady() );
  o1->exNotifyDone();
  CPPUNIT_ASSERT( i1->exIsReady() ); CPPUNIT_ASSERT( !i2->exIsReady() ); CPPUNIT_ASSERT( !i3->exIsReady() ); CPPUNIT_ASSERT( !i4->exIsReady() );
  o2->exNotifyDone();
  CPPUNIT_ASSERT( i1->exIsReady() ); CPPUNIT_ASSERT( !i2->exIsReady() ); CPPUNIT_ASSERT( !i3->exIsReady() ); CPPUNIT_ASSERT( !i4->exIsReady() );
  o3->exNotifyDone();
  CPPUNIT_ASSERT( i1->exIsReady() ); CPPUNIT_ASSERT( !i2->exIsReady() ); CPPUNIT_ASSERT( i3->exIsReady() ); CPPUNIT_ASSERT( !i4->exIsReady() );
  o4->exNotifyDone();
  CPPUNIT_ASSERT( i1->exIsReady() ); CPPUNIT_ASSERT( i2->exIsReady() ); CPPUNIT_ASSERT( i3->exIsReady() ); CPPUNIT_ASSERT( i4->exIsReady() );
  //
  delete o1; delete o2; delete o3; delete o4;
  delete i1; delete i2; delete i3; delete i4;
}

void PortsTest::testDataFlow1()
{
  Node *n1=0,*n2=0,*n3=0,*n4=0;
  OutputPort *o1=new OutputPort("o1",n1,YACS::Double);
  OutputPort *o3=new OutputPort("o3",n3,YACS::Int);
  InputPort *i1=new InputPort("i1",n1,YACS::Double);
  InputPort *i2=new InputPort("i2",n2,YACS::Int);
  InputPort *i4=new InputPort("i4",n4,YACS::Bool);
  // Simuling edition
  o1->edAddInputPort(i1); o1->edAddInputPort(i2);
  o3->edAddInputPort(i1); o3->edAddInputPort(i2); o3->edAddInputPort(i4);
  CPPUNIT_ASSERT( o1->foGet().edGetRepresentation() == "41_NULL" ); CPPUNIT_ASSERT( o3->foGet().edGetRepresentation() == "42_NULL" );
  CPPUNIT_ASSERT( i1->exGet().edGetRepresentation() == "41_NULL" ); CPPUNIT_ASSERT( i2->exGet().edGetRepresentation() == "42_NULL" ); CPPUNIT_ASSERT( i4->exGet().edGetRepresentation() == "45_NULL" );
  // Simuling execution
  o1->exPut(3.14);
  CPPUNIT_ASSERT( i1->exGet().edGetRepresentation() == "41_3.14" ); CPPUNIT_ASSERT( i2->exGet().edGetRepresentation() == "42_3" ); CPPUNIT_ASSERT( i4->exGet().edGetRepresentation() == "45_NULL" );
  o3->exPut(325);
  CPPUNIT_ASSERT( i1->exGet().edGetRepresentation() == "41_325" ); CPPUNIT_ASSERT( i2->exGet().edGetRepresentation() == "42_325" ); CPPUNIT_ASSERT( i4->exGet().edGetRepresentation() == "45_1" );
  o1->exPut(-2.78);
  CPPUNIT_ASSERT( i1->exGet().edGetRepresentation() == "41_-2.78" ); CPPUNIT_ASSERT( i2->exGet().edGetRepresentation() == "42_-2" ); CPPUNIT_ASSERT( i4->exGet().edGetRepresentation() == "45_1" );
  //
  delete o1; delete o3;
  delete i1; delete i2; delete i4;
}

void PortsTest::testDataFlowTypeCheck()
{
  Node *n1=0;
  OutputPort o1("o1",n1,YACS::Double);
  InputPort i1("i1",n1,YACS::Bool);
  o1.edAddInputPort(&i1);//Should throw exception
}

int main()
{
  CppUnit::TextUi::TestRunner runner;
  runner.addTest( PortsTest::suite() );
  runner.run();
  return 0;
}
