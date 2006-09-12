#include "Data.hxx"
//
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/ui/text/TestRunner.h>
#include <sstream>
//

using namespace YACS::ENGINE;

class DataTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( DataTest );
  CPPUNIT_TEST( test1 );
  CPPUNIT_TEST( test2 );
  CPPUNIT_TEST( test3 );
  CPPUNIT_TEST( test4 );
  CPPUNIT_TEST_EXCEPTION( test5, YACS::ENGINE::ConversionException);
  CPPUNIT_TEST( test6 );
  CPPUNIT_TEST( testStaticCheckType );
  CPPUNIT_TEST_SUITE_END();
public: 
  void tearDown();
  void test1();
  void test2();
  void test3();
  void test4();
  void test5();
  void test6();
  void testStaticCheckType();
};

void DataTest::tearDown()
{
}

void DataTest::test1()
{
  Data d1(YACS::Double),d2(YACS::Int),d3(YACS::Bool);
  d1=3.14; d3=d2=d1;
  CPPUNIT_ASSERT( d1.edGetRepresentation() == "41_3.14" );
  CPPUNIT_ASSERT( d2.edGetRepresentation() == "42_3" );
  CPPUNIT_ASSERT( d3.edGetRepresentation() == "45_1" );
}

void DataTest::test2()
{
  Data d1("2.78",YACS::Double);
  d1.edSetNewType(YACS::Int);
  CPPUNIT_ASSERT( d1.edGetRepresentation() == "42_2" );
  d1.edSetNewType(YACS::Bool);
  CPPUNIT_ASSERT( d1.edGetRepresentation() == "45_1" );
  d1.edInitToType(YACS::String);
  CPPUNIT_ASSERT( d1.edGetRepresentation() == "44_NULL" );
}

void DataTest::test3()
{
  Data d1("2.78",YACS::Double);
  CPPUNIT_ASSERT( d1.edGetRepresentation() == "41_2.78" );
  d1.exSetContent("4.14e3");
  CPPUNIT_ASSERT( d1.edGetRepresentation() == "41_4140" );
  d1.exSetContent("-4.9995");
  CPPUNIT_ASSERT( d1.edGetRepresentation() == "41_-4.9995" );
}

void DataTest::test4()
{
  Data d1(6.279),d2(67), d3(false), d4(std::string("coucou"));
  CPPUNIT_ASSERT( d1.edGetRepresentation() == "41_6.279" );
  CPPUNIT_ASSERT( d2.edGetRepresentation() == "42_67" );
  CPPUNIT_ASSERT( Data::edGetTypeInPrintableForm(d1.edGetType()) == "Double" );
  CPPUNIT_ASSERT( d3.edGetRepresentation() == "45_0" );
  CPPUNIT_ASSERT( Data::edGetTypeInPrintableForm(d3.edGetType()) == "Bool" );
  CPPUNIT_ASSERT( d4.edGetRepresentation() == "44_coucou" );
  CPPUNIT_ASSERT( Data::edGetTypeInPrintableForm(d4.edGetType()) == "String" );
}

void DataTest::test5()
{
  Data d1("2.78",YACS::Double);
  d1=std::string("coucou");
}

void DataTest::test6()
{
  Data d1("2.78",YACS::Double);
  Data d2("3.14",YACS::Double);
  CPPUNIT_ASSERT( d1.edGetRepresentation() == "41_2.78" );
  CPPUNIT_ASSERT( d2.edGetRepresentation() == "41_3.14" );
  d1=d2;
  Data d3(d1);
  Data d4=d1;
  CPPUNIT_ASSERT( d1.edGetRepresentation() == "41_3.14" );
  CPPUNIT_ASSERT( d3.edGetRepresentation() == "41_3.14" );
  CPPUNIT_ASSERT( d4.edGetRepresentation() == "41_3.14" );
  d3.edInitToType(YACS::Int);
  CPPUNIT_ASSERT( d3.empty() );
  CPPUNIT_ASSERT( Data::edGetTypeInPrintableForm(d3.edGetType()) == "Int" );
}

void DataTest::testStaticCheckType()
{
  Data d1("2.78",YACS::Double);
  Data d2("3.14",YACS::Double);
  CPPUNIT_ASSERT( d1.isStaticallyCompatibleWith(d2) );
  d1.edInitToType(YACS::Int);
  CPPUNIT_ASSERT( d1.isStaticallyCompatibleWith(d2) );
  d1.edInitToType(YACS::Bool);
  CPPUNIT_ASSERT( !d1.isStaticallyCompatibleWith(d2) );
}

int main()
{
  CppUnit::TextUi::TestRunner runner;
  runner.addTest( DataTest::suite() );
  runner.run();
  return 0;
}
