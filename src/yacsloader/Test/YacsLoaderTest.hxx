
#ifndef _YACSLOADERTEST_HXX_
#define _YACSLOADERTEST_HXX_

#include <cppunit/extensions/HelperMacros.h>
#include <string>
#include <map>

namespace YACS
{
  class YacsLoaderTest: public CppUnit::TestFixture
  {
    CPPUNIT_TEST_SUITE( YacsLoaderTest );
    CPPUNIT_TEST(aschema);
    CPPUNIT_TEST(bschema);
    CPPUNIT_TEST(cschema);
    CPPUNIT_TEST(dschema);
    CPPUNIT_TEST(eschema);
    CPPUNIT_TEST(fschema);
    CPPUNIT_TEST(oschema);
    CPPUNIT_TEST(pschema);
    CPPUNIT_TEST(schema);
    CPPUNIT_TEST(schema2);
    CPPUNIT_TEST(forloop1);
    CPPUNIT_TEST(forloop2);
    CPPUNIT_TEST(forloop3);
    CPPUNIT_TEST(forloop4);
    CPPUNIT_TEST(forloop5);
    CPPUNIT_TEST(forloop6);
    CPPUNIT_TEST(forloop7);
    CPPUNIT_TEST(switch1);
    CPPUNIT_TEST(switch2);
    CPPUNIT_TEST(switch3);
    CPPUNIT_TEST(switch4);
    CPPUNIT_TEST(switch5);
    CPPUNIT_TEST(switch6);
    CPPUNIT_TEST(switch7);
    CPPUNIT_TEST(switch8);
    CPPUNIT_TEST(switch9);
    CPPUNIT_TEST(whiles);
    CPPUNIT_TEST(forwhile1);
    CPPUNIT_TEST(blocs);
    CPPUNIT_TEST(refcnt);
    CPPUNIT_TEST(foreachs);
    CPPUNIT_TEST(sinlines);
    CPPUNIT_TEST(bools);
    CPPUNIT_TEST(integers);
    CPPUNIT_TEST(doubles);
    CPPUNIT_TEST(strings);
    CPPUNIT_TEST(objrefs);
    CPPUNIT_TEST(structs);
    CPPUNIT_TEST(cpps);
    CPPUNIT_TEST_SUITE_END();
      
  public:
      
    void setUp();
    void tearDown();
      
    void aschema();
    void bschema();
    void cschema();
    void dschema();
    void eschema();
    void fschema();
    void oschema();
    void pschema();
    void schema();
    void schema2();
    void forloop1();
    void forloop2();
    void forloop3();
    void forloop4();
    void forloop5();
    void forloop6();
    void forloop7();
    void switch1();
    void switch2();
    void switch3();
    void switch4();
    void switch5();
    void switch6();
    void switch7();
    void switch8();
    void switch9();
    void whiles();
    void forwhile1();
    void blocs();
    void refcnt();
    void foreachs();
    void sinlines();
    void bools();
    void integers();
    void doubles();
    void strings();
    void objrefs();
    void structs();
    void cpps();
     
  protected:
      
  private:

  };

}

#endif
