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

#include "yacsconfig.h"
#include "RuntimeSALOME.hxx"
#include "PythonPorts.hxx"
#include "CORBAPorts.hxx"
#include "YacsLoaderTest.hxx"
#include "parsers.hxx"
#include "Proc.hxx"
#include "Exception.hxx"
#include "Executor.hxx"
#include "parsers.hxx"

#include <iostream>
#include <fstream>
#include <stdlib.h>
#ifdef WIN32
#include <io.h>
#define F_OK 0
#define access _access
#else
#include <unistd.h>
#endif

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace YACS;
using namespace std;

int driverTest(Proc* &p, const char* schema)
{
  DEBTRACE("+++++++++++++++++++ BEGIN test " << schema);
  RuntimeSALOME::setRuntime();

  YACSLoader loader;
  Executor executor;
  
  try
    {
      p=loader.load(schema);
      DEBTRACE("Proc *p = " << p);
      std::ofstream f("toto");
      p->writeDot(f);
      f.close();
      DEBTRACE("+++++++++++++++++++ BEGIN execution " << schema);
      executor.RunW(p,0);
      DEBTRACE("+++++++++++++++++++   END execution " << schema);
      std::ofstream g("titi");
      p->writeDot(g);
      g.close();
      DEBTRACE("+++++++++++++++++++ END test " << schema);
      return 0;
    }
  catch (YACS::Exception& e)
    {
      DEBTRACE("YACS exception caught: ");
      DEBTRACE(e.what());
      DEBTRACE("+++++++++++++++++++ END test in error " << schema);
      return 1;
    }
  catch (const std::ios_base::failure&)
    {
      DEBTRACE("io failure");
      DEBTRACE("+++++++++++++++++++ END test in error " << schema);
      return 1;
    }
  catch(CORBA::SystemException& ex)
    {
      DEBTRACE("Caught a CORBA::SystemException.");
      CORBA::Any tmp;
      tmp <<= ex;
      CORBA::TypeCode_var tc = tmp.type();
      const char *p = tc->name();
      if ( *p != '\0' )
        {
          DEBTRACE(p);
        }
      else
        {
          DEBTRACE(tc->id());
        }
      DEBTRACE("+++++++++++++++++++ END test in error " << schema);
      return 1;
    }
  catch(omniORB::fatalException& fe)
    {
      DEBTRACE("Caught omniORB::fatalException:" );
      DEBTRACE("  file: " << fe.file());
      DEBTRACE("  line: " << fe.line());
      DEBTRACE("  mesg: " << fe.errmsg());
      DEBTRACE("+++++++++++++++++++ END test in error " << schema);
      return 1;
    }
  catch(...)
    {
      DEBTRACE("Caught unknown exception.");
      DEBTRACE("+++++++++++++++++++ END test in error " << schema);
      return 1;
    }
}

void YacsLoaderTest::setUp()
{
}

void YacsLoaderTest::tearDown()
{
}

void YacsLoaderTest::aschema()
{
  Proc *p = 0;
  int ret = driverTest(p, "samples/aschema.xml");
  CPPUNIT_ASSERT(ret == 0);
  CPPUNIT_ASSERT(p != 0);
  if (p)
    {
      CORBA::Double dval = 0;
      char *text = (char*)"";
      CPPUNIT_ASSERT(p->nodeMap["c0.c1.n1"]);
      *((OutputCorbaPort*)p->nodeMap["c0.c1.n1"]->getOutputPort("p1"))->getAny() >>= dval;
      PyObject *data = ((OutputPyPort*)p->nodeMap["node32"]->getOutputPort("p1"))->get();
      double val = PyFloat_AsDouble(data);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(22., dval, 1.E-12);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(20., val, 1.E-12);
      delete p;
    }
}

void YacsLoaderTest::bschema()
{
  Proc *p = 0;
  int ret = driverTest(p, "samples/bschema.xml");
  CPPUNIT_ASSERT(ret == 0);
  DEBTRACE("Proc *p = " << p);
  CPPUNIT_ASSERT(p != 0);
  if (p)
    {
      CORBA::Double dval = 0;
      const char *text = "";
      *((OutputCorbaPort*)p->nodeMap["node1"]->getOutputPort("p1"))->getAny() >>= dval;
      *((OutputCorbaPort*)p->nodeMap["node2"]->getOutputPort("p1"))->getAny() >>= text;
      CPPUNIT_ASSERT_DOUBLES_EQUAL(24., dval, 1.E-12);
      CPPUNIT_ASSERT_EQUAL(string("coucou"), string(text));
      delete p;
    }
}

void YacsLoaderTest::cschema()
{
  Proc *p = 0;
  int ret = driverTest(p, "samples/cschema.xml");
  CPPUNIT_ASSERT(ret == 0);
  DEBTRACE("Proc *p = " << p);
  CPPUNIT_ASSERT(p != 0);
#ifdef SALOME_KERNEL
  if (p)
    {
      if(getenv("PYHELLO_ROOT_DIR"))
        {
          std::string hellodir(getenv("PYHELLO_ROOT_DIR"));
          hellodir=hellodir+"/share/salome/resources/pyhello";
          if(access(hellodir.c_str(),F_OK) == 0)
            {
              CORBA::Double dval = 0;
              const char *text = "";
              *((OutputCorbaPort*)p->nodeMap["node1"]->getOutputPort("p1"))->getAny() >>= text;
              CPPUNIT_ASSERT_EQUAL(string("Hello coucou!"), string(text) );
              text = "";
              *((OutputCorbaPort*)p->nodeMap["node2"]->getOutputPort("p1"))->getAny() >>= text;
              CPPUNIT_ASSERT_EQUAL(string("Hello Hello coucou!!"), string(text) );
            }
        }
      delete p;
    }
#endif
}

void YacsLoaderTest::dschema()
{
  Proc *p = 0;
  int ret = driverTest(p, "samples/dschema.xml");
  CPPUNIT_ASSERT(ret == 0);
  DEBTRACE("Proc *p = " << p);
  CPPUNIT_ASSERT(p != 0);
#ifdef SALOME_KERNEL
  if (p)
    {
      if(getenv("PYHELLO_ROOT_DIR"))
        {
          std::string hellodir(getenv("PYHELLO_ROOT_DIR"));
          hellodir=hellodir+"/share/salome/resources/pyhello";
          if(access(hellodir.c_str(),F_OK) == 0)
            {
              CORBA::Double dval = 0;
              const char *text = "";
              *((OutputCorbaPort*)p->nodeMap["node1"]->getOutputPort("p1"))->getAny() >>= text;
              CPPUNIT_ASSERT_EQUAL(string("Hello coucou!"), string(text) );
              text = "";
              *((OutputCorbaPort*)p->nodeMap["node2"]->getOutputPort("p1"))->getAny() >>= text;
              CPPUNIT_ASSERT_EQUAL(string("Hello Hello coucou!!"), string(text) );
              text = "";
              *((OutputCorbaPort*)p->nodeMap["node3"]->getOutputPort("p1"))->getAny() >>= text;
              CPPUNIT_ASSERT_EQUAL( string("Hello Hello coucou!!"), string(text));
            }
        }
      delete p;
    }
#endif
}

void YacsLoaderTest::eschema()
{
  Proc *p = 0;
  int ret = driverTest(p, "samples/eschema.xml");
  CPPUNIT_ASSERT(ret == 0);
  DEBTRACE("Proc *p = " << p);
  CPPUNIT_ASSERT(p != 0);
  if (p)
    {
      PyObject *data = ((OutputPyPort*)p->nodeMap["node2"]->getOutputPort("p1"))->get();
      char *text = PyString_AsString(data);
      CPPUNIT_ASSERT_EQUAL(string("coucoucoucoucoucoucoucou"), string(text));
      delete p;
    }
}

void YacsLoaderTest::fschema()
{
  Proc *p = 0;
  int ret = driverTest(p, "samples/fschema.xml");
  CPPUNIT_ASSERT(ret == 0);
  DEBTRACE("Proc *p = " << p);
  CPPUNIT_ASSERT(p != 0);
  if (p)
    {
      PyObject *data = ((OutputPyPort*)p->nodeMap["node2"]->getOutputPort("p1"))->get();
      char *text = PyString_AsString(data);;
      CPPUNIT_ASSERT_EQUAL(string("coucoucoucoucoucoucoucou"), string(text) );
      delete p;
    }
}

void YacsLoaderTest::oschema()
{
  Proc *p = 0;
  int ret = driverTest(p, "samples/oschema.xml");
  CPPUNIT_ASSERT(ret == 1);
}

void YacsLoaderTest::pschema()
{
  Proc *p = 0;
  int ret = driverTest(p, "samples/pschema.xml");
  CPPUNIT_ASSERT(ret == 0);
  DEBTRACE("Proc *p = " << p);
  CPPUNIT_ASSERT(p != 0);
  if (p)
    {
      {
        CORBA::Double dval = 0;
        *((OutputCorbaPort*)p->nodeMap["node61"]->getOutputPort("p1"))->getAny() >>= dval;
        CPPUNIT_ASSERT_DOUBLES_EQUAL(25., dval, 1.E-12);
      }
      {
        CORBA::Double dval = 0;
        *((OutputCorbaPort*)p->nodeMap["node62"]->getOutputPort("p1"))->getAny() >>= dval;
        CPPUNIT_ASSERT_DOUBLES_EQUAL(25., dval, 1.E-12);
      }
      {
        CORBA::Double dval = 0;
        *((OutputCorbaPort*)p->nodeMap["node63"]->getOutputPort("p1"))->getAny() >>= dval;
        CPPUNIT_ASSERT_DOUBLES_EQUAL(25., dval, 1.E-12);
      }
      delete p;
    }
}

void YacsLoaderTest::schema()
{
  Proc *p = 0;
  int ret = driverTest(p, "samples/schema.xml");
  CPPUNIT_ASSERT(ret == 1);
}

void YacsLoaderTest::schema2()
{
  Proc *p = 0;
  int ret = driverTest(p, "samples/schema2.xml");
  CPPUNIT_ASSERT(ret == 0);
  DEBTRACE("Proc *p = " << p);
  CPPUNIT_ASSERT(p != 0);
  if (p)
    {
      {
        CORBA::Double dval = 0;
        *((OutputCorbaPort*)p->nodeMap["node61"]->getOutputPort("p1"))->getAny() >>= dval;
        CPPUNIT_ASSERT_DOUBLES_EQUAL(25., dval, 1.E-12);
      }
      {
        CORBA::Double dval = 0;
        *((OutputCorbaPort*)p->nodeMap["node62"]->getOutputPort("p1"))->getAny() >>= dval;
        CPPUNIT_ASSERT_DOUBLES_EQUAL(25., dval, 1.E-12);
      }
      {
        CORBA::Double dval = 0;
        *((OutputCorbaPort*)p->nodeMap["node63"]->getOutputPort("p1"))->getAny() >>= dval;
        CPPUNIT_ASSERT_DOUBLES_EQUAL(25., dval, 1.E-12);
      }
      try
        {
          delete p;
        }
      catch (YACS::Exception& e)
        {
          DEBTRACE("YACS exception caught: ");
          DEBTRACE(e.what());
        }
      catch (const std::ios_base::failure&)
        {
          DEBTRACE("io failure");
        }
      catch(CORBA::SystemException& ex)
        {
          DEBTRACE("Caught a CORBA::SystemException.");
          CORBA::Any tmp;
          tmp <<= ex;
          CORBA::TypeCode_var tc = tmp.type();
          const char *p = tc->name();
          if ( *p != '\0' )
            {
              DEBTRACE(p);
            }
          else
            {
              DEBTRACE(tc->id());
            }
        }
      catch(omniORB::fatalException& fe)
        {
          DEBTRACE("Caught omniORB::fatalException:" );
          DEBTRACE("  file: " << fe.file());
          DEBTRACE("  line: " << fe.line());
          DEBTRACE("  mesg: " << fe.errmsg());
        }
      catch(...)
        {
          DEBTRACE("unknown exception");
        }
    }
}

void YacsLoaderTest::forloop1()
{
  Proc *p = 0;
  int ret = driverTest(p, "samples/forloop1.xml");
  CPPUNIT_ASSERT(ret == 0);
  DEBTRACE("Proc *p = " << p);
  CPPUNIT_ASSERT(p != 0);
  if (p)
    {
      PyObject *data = ((OutputPyPort*)p->nodeMap["b1.node2"]->getOutputPort("p1"))->get();
      double val = PyFloat_AsDouble(data);;
      CPPUNIT_ASSERT_DOUBLES_EQUAL(33., val, 1.E-12);
      delete p;
    }
}

void YacsLoaderTest::forloop2()
{
  Proc *p = 0;
  int ret = driverTest(p, "samples/forloop2.xml");
  CPPUNIT_ASSERT(ret == 0);
}

/////////////////////////////

void YacsLoaderTest::forloop3()
{
  Proc *p = 0;
  int ret = driverTest(p, "samples/forloop3.xml");
  CPPUNIT_ASSERT(ret == 0);
  DEBTRACE("Proc *p = " << p);
  CPPUNIT_ASSERT(p != 0);
  if (p)
    {
      delete p;
    }
}

void YacsLoaderTest::forloop4()
{
  Proc *p = 0;
  int ret = driverTest(p, "samples/forloop4.xml");
  CPPUNIT_ASSERT(ret == 0);
  DEBTRACE("Proc *p = " << p);
  CPPUNIT_ASSERT(p != 0);
  if (p)
    {
      delete p;
    }
}

void YacsLoaderTest::forloop5()
{
  Proc *p = 0;
  int ret = driverTest(p, "samples/forloop5.xml");
  CPPUNIT_ASSERT(ret == 0);
  DEBTRACE("Proc *p = " << p);
  CPPUNIT_ASSERT(p != 0);
  if (p)
    {
      delete p;
    }
}

void YacsLoaderTest::forloop6()
{
  Proc *p = 0;
  int ret = driverTest(p, "samples/forloop6.xml");
  CPPUNIT_ASSERT(ret == 0);
  DEBTRACE("Proc *p = " << p);
  CPPUNIT_ASSERT(p != 0);
  if (p)
    {
      delete p;
    }
}


void YacsLoaderTest::forloop7()
{
  Proc *p = 0;
  int ret = driverTest(p, "samples/forloop7.xml");
  CPPUNIT_ASSERT(ret == 0);
  DEBTRACE("Proc *p = " << p);
  CPPUNIT_ASSERT(p != 0);
  if (p)
    {
      delete p;
    }
}

void YacsLoaderTest::switch1()
{
  Proc *p = 0;
  int ret = driverTest(p, "samples/switch1.xml");
  CPPUNIT_ASSERT(ret == 0);
  DEBTRACE("Proc *p = " << p);
  CPPUNIT_ASSERT(p != 0);
  if (p)
    {
      delete p;
    }
}

void YacsLoaderTest::switch2()
{
  Proc *p = 0;
  int ret = driverTest(p, "samples/switch2.xml");
  CPPUNIT_ASSERT(ret == 0);
  DEBTRACE("Proc *p = " << p);
  CPPUNIT_ASSERT(p != 0);
  if (p)
    {
      delete p;
    }
}

void YacsLoaderTest::switch3()
{
  Proc *p = 0;
  int ret = driverTest(p, "samples/switch3.xml");
  CPPUNIT_ASSERT(ret == 0);
  DEBTRACE("Proc *p = " << p);
  CPPUNIT_ASSERT(p != 0);
  if (p)
    {
      delete p;
    }
}

void YacsLoaderTest::switch4()
{
  Proc *p = 0;
  int ret = driverTest(p, "samples/switch4.xml");
  CPPUNIT_ASSERT(ret == 0);
  DEBTRACE("Proc *p = " << p);
  CPPUNIT_ASSERT(p != 0);
  if (p)
    {
      delete p;
    }
}

void YacsLoaderTest::switch5()
{
  Proc *p = 0;
  int ret = driverTest(p, "samples/switch5.xml");
  CPPUNIT_ASSERT(ret == 0);
  DEBTRACE("Proc *p = " << p);
  CPPUNIT_ASSERT(p != 0);
  if (p)
    {
      delete p;
    }
}

void YacsLoaderTest::switch6()
{
  Proc *p = 0;
  int ret = driverTest(p, "samples/switch6.xml");
  CPPUNIT_ASSERT(ret == 0);
  DEBTRACE("Proc *p = " << p);
  CPPUNIT_ASSERT(p != 0);
  if (p)
    {
      delete p;
    }
}

void YacsLoaderTest::switch7()
{
  Proc *p = 0;
  int ret = driverTest(p, "samples/switch7.xml");
  CPPUNIT_ASSERT(ret == 0);
  DEBTRACE("Proc *p = " << p);
  CPPUNIT_ASSERT(p != 0);
  if (p)
    {
      delete p;
    }
}

void YacsLoaderTest::switch8()
{
  Proc *p = 0;
  int ret = driverTest(p, "samples/switch8.xml");
  CPPUNIT_ASSERT(ret == 0);
  DEBTRACE("Proc *p = " << p);
  CPPUNIT_ASSERT(p != 0);
  if (p)
    {
      delete p;
    }
}

void YacsLoaderTest::switch9()
{
  Proc *p = 0;
  int ret = driverTest(p, "samples/switch9.xml");
  CPPUNIT_ASSERT(ret == 0);
  DEBTRACE("Proc *p = " << p);
  CPPUNIT_ASSERT(p != 0);
  if (p)
    {
      delete p;
    }
}

void YacsLoaderTest::whiles()
{
  Proc *p = 0;
  int ret = driverTest(p, "samples/while1.xml");
  CPPUNIT_ASSERT(ret == 0);
  DEBTRACE("Proc *p = " << p);
  CPPUNIT_ASSERT(p != 0);
  if (p)
    {
      delete p;
    }
  ret = driverTest(p, "samples/while2.xml");
  CPPUNIT_ASSERT(ret == 0);
  ret = driverTest(p, "samples/while3.xml");
  CPPUNIT_ASSERT(ret == 0);
}

void YacsLoaderTest::forwhile1()
{
  Proc *p = 0;
  int ret = driverTest(p, "samples/forwhile1.xml");
  CPPUNIT_ASSERT(ret == 0);
  DEBTRACE("Proc *p = " << p);
  CPPUNIT_ASSERT(p != 0);
  if (p)
    {
      delete p;
    }
}

void YacsLoaderTest::blocs()
{
  Proc *p = 0;
  int ret;
  ret = driverTest(p, "samples/bloc1.xml");
  CPPUNIT_ASSERT(ret == 0);
  CPPUNIT_ASSERT(p->getEffectiveState() == YACS::DONE );
  ret = driverTest(p, "samples/bloc2.xml");
  CPPUNIT_ASSERT(ret == 0);
  CPPUNIT_ASSERT(p->getEffectiveState() == YACS::DONE );
  ret = driverTest(p, "samples/bloc3.xml");
  CPPUNIT_ASSERT(ret == 0);
  CPPUNIT_ASSERT(p->getEffectiveState() != YACS::DONE );
  ret = driverTest(p, "samples/bloc4.xml");
  CPPUNIT_ASSERT(ret == 0);
  CPPUNIT_ASSERT(p->getEffectiveState() == YACS::DONE );
}

void YacsLoaderTest::refcnt()
{
  Proc *p = 0;
  int ret;
  PyObject *data;
  ret = driverTest(p, "samples/refcnt1.xml");
  CPPUNIT_ASSERT(ret == 0);
  data = ((OutputPyPort*)p->nodeMap["b1.b.node1"]->getOutputPort("p1"))->get();
  CPPUNIT_ASSERT(data->ob_refcnt==13);
  ret = driverTest(p, "samples/refcnt2.xml");
  CPPUNIT_ASSERT(ret == 0);
  data = ((OutputPyPort*)p->nodeMap["b1.b.node1"]->getOutputPort("p1"))->get();
  CPPUNIT_ASSERT(data->ob_refcnt==19);
}

void YacsLoaderTest::foreachs()
{
  Proc *p = 0;
  int ret;
  ret = driverTest(p, "samples/foreach1.xml");
  CPPUNIT_ASSERT_MESSAGE("Schema: foreach1.xml", ret == 0);
  CPPUNIT_ASSERT_MESSAGE("Schema: foreach1.xml", p->getEffectiveState() == YACS::DONE);
  delete p;
  ret = driverTest(p, "samples/foreach2.xml");
  CPPUNIT_ASSERT_MESSAGE("Schema: foreach2.xml", ret == 0);
  CPPUNIT_ASSERT_MESSAGE("Schema: foreach2.xml", p->getEffectiveState() == YACS::DONE );
  delete p;
  ret = driverTest(p, "samples/foreach3.xml");
  CPPUNIT_ASSERT_MESSAGE("Schema: foreach3.xml", ret == 1);
  delete p;
  ret = driverTest(p, "samples/foreach4.xml");
  CPPUNIT_ASSERT_MESSAGE("Schema: foreach4.xml", ret == 0);
  CPPUNIT_ASSERT_MESSAGE("Schema: foreach4.xml", p->getEffectiveState() == YACS::DONE );
  delete p;
  ret = driverTest(p, "samples/foreach5.xml");
  CPPUNIT_ASSERT_MESSAGE("Schema: foreach5.xml", ret == 0);
  CPPUNIT_ASSERT_MESSAGE("Schema: foreach5.xml", p->getEffectiveState() == YACS::DONE );
  delete p;
  ret = driverTest(p, "samples/foreach6.xml");
  CPPUNIT_ASSERT_MESSAGE("Schema: foreach6.xml", ret == 0);
  CPPUNIT_ASSERT_MESSAGE("Schema: foreach6.xml", p->getEffectiveState() == YACS::DONE );
  delete p;
  ret = driverTest(p, "samples/foreach8.xml");
  CPPUNIT_ASSERT_MESSAGE("Schema: foreach8.xml", ret == 0);
  CPPUNIT_ASSERT_MESSAGE("Schema: foreach8.xml", p->getEffectiveState() == YACS::DONE );
  delete p;
  ret = driverTest(p, "samples/foreach_init2fin.xml");
  CPPUNIT_ASSERT_MESSAGE("Schema: foreach_init2fin.xml", ret == 0);
  CPPUNIT_ASSERT_MESSAGE("Schema: foreach_init2fin.xml", p->getEffectiveState() == YACS::DONE );
  delete p;
  ret = driverTest(p, "samples/foreach_init2work.xml");
  CPPUNIT_ASSERT_MESSAGE("Schema: foreach_init2work.xml", ret == 0);
  CPPUNIT_ASSERT_MESSAGE("Schema: foreach_init2work.xml", p->getEffectiveState() == YACS::DONE );
  CPPUNIT_ASSERT_EQUAL(p->getOutputPort("PostProc.r")->getAsString(), std::string("108"));
  delete p;
  if(getenv("GEOM_ROOT_DIR"))
    {
      std::string geomdir(getenv("GEOM_ROOT_DIR"));
      geomdir=geomdir+"/share/salome/resources/geom";
      if(access(geomdir.c_str(),F_OK) == 0)
      {
        ret = driverTest(p, "samples/foreach7.xml"); //needs GEOM_Superv component
        CPPUNIT_ASSERT_MESSAGE("Schema: foreach7.xml", ret == 0);
        CPPUNIT_ASSERT_MESSAGE("Schema: foreach7.xml", p->getEffectiveState() == YACS::DONE );
        delete p;
      }
    }
}

void YacsLoaderTest::sinlines()
{
  Proc *p = 0;
  int ret;
  ret = driverTest(p, "samples/sinline1.xml");
  CPPUNIT_ASSERT(ret == 0);
  CPPUNIT_ASSERT(p->getEffectiveState() == YACS::DONE );
  ret = driverTest(p, "samples/sinline2.xml");
  CPPUNIT_ASSERT(ret == 0);
  CPPUNIT_ASSERT(p->getEffectiveState() == YACS::DONE );
  ret = driverTest(p, "samples/sinline3.xml");
  CPPUNIT_ASSERT(ret == 0);
  CPPUNIT_ASSERT(p->getEffectiveState() == YACS::DONE );
  ret = driverTest(p, "samples/sinline4.xml");
  CPPUNIT_ASSERT(ret == 0);
  CPPUNIT_ASSERT(p->getEffectiveState() == YACS::DONE );
  ret = driverTest(p, "samples/sinline5.xml");
  CPPUNIT_ASSERT(ret == 0);
  CPPUNIT_ASSERT(p->getEffectiveState() == YACS::DONE );
}

void YacsLoaderTest::bools()
{
  Proc *p = 0;
  int ret;
  ret = driverTest(p, "samples/bool1.xml");
  CPPUNIT_ASSERT(ret == 0);
  CPPUNIT_ASSERT(p->getEffectiveState() == YACS::DONE );
}
void YacsLoaderTest::integers()
{
  Proc *p = 0;
  int ret;
  ret = driverTest(p, "samples/integer1.xml");
  CPPUNIT_ASSERT(ret == 0);
  CPPUNIT_ASSERT(p->getEffectiveState() == YACS::DONE );
}
void YacsLoaderTest::doubles()
{
  Proc *p = 0;
  int ret;
  ret = driverTest(p, "samples/double1.xml");
  CPPUNIT_ASSERT(ret == 0);
  CPPUNIT_ASSERT(p->getEffectiveState() == YACS::DONE );
}
void YacsLoaderTest::strings()
{
  Proc *p = 0;
  int ret;
  ret = driverTest(p, "samples/string1.xml");
  CPPUNIT_ASSERT(ret == 0);
  CPPUNIT_ASSERT(p->getEffectiveState() == YACS::DONE );
}
void YacsLoaderTest::objrefs()
{
  Proc *p = 0;
  int ret;
  ret = driverTest(p, "samples/objref1.xml");
  CPPUNIT_ASSERT(ret == 0);
  CPPUNIT_ASSERT(p->getEffectiveState() == YACS::DONE );
}
void YacsLoaderTest::structs()
{
  Proc *p = 0;
  int ret;
  ret = driverTest(p, "samples/struct1.xml");
  CPPUNIT_ASSERT(ret == 0);
  CPPUNIT_ASSERT(p->getEffectiveState() == YACS::DONE );
  delete p;

  ret = driverTest(p, "samples/struct2.xml");
  CPPUNIT_ASSERT(ret == 0);
  CPPUNIT_ASSERT(p->getEffectiveState() == YACS::DONE );
  delete p;
}

void YacsLoaderTest::cpps()
{
  Proc *p = 0;
  int ret;
  ret = driverTest(p, "samples/cpp1.xml");
  CPPUNIT_ASSERT(ret == 0);
  CPPUNIT_ASSERT(p->getEffectiveState() == YACS::DONE );
  delete p;
}

void YacsLoaderTest::datanodes()
{
  Proc *p = 0;
  int ret;
  ret = driverTest(p, "samples/datanode0.xml");
  CPPUNIT_ASSERT(ret == 0);
  CPPUNIT_ASSERT(p->getEffectiveState() == YACS::DONE );
  delete p;
}

void YacsLoaderTest::optimizers()
{
  Proc *p = 0;
  int ret;
  ret = driverTest(p, "samples/optimizer1.xml");
  CPPUNIT_ASSERT(ret == 0);
  CPPUNIT_ASSERT(p->getEffectiveState() == YACS::DONE );
  delete p;

  ret = driverTest(p, "samples/optimizer2.xml");
  CPPUNIT_ASSERT(ret == 0);
  CPPUNIT_ASSERT(p->getEffectiveState() == YACS::DONE );
  delete p;

  ret = driverTest(p, "samples/optimizer_sync_cpp.xml");
  CPPUNIT_ASSERT(ret == 0);
  CPPUNIT_ASSERT(p->getEffectiveState() == YACS::DONE );
  delete p;

  ret = driverTest(p, "samples/optimizer_async_cpp.xml");
  CPPUNIT_ASSERT(ret == 0);
  CPPUNIT_ASSERT(p->getEffectiveState() == YACS::DONE );
  delete p;

  ret = driverTest(p, "samples/optimizer_sync_py.xml");
  CPPUNIT_ASSERT(ret == 0);
  CPPUNIT_ASSERT(p->getEffectiveState() == YACS::DONE );
  delete p;

  ret = driverTest(p, "samples/optimizer_async_py.xml");
  CPPUNIT_ASSERT(ret == 0);
  CPPUNIT_ASSERT(p->getEffectiveState() == YACS::DONE );
  delete p;

  ret = driverTest(p, "samples/optimizer_retro.xml");
  CPPUNIT_ASSERT(ret == 0);
  CPPUNIT_ASSERT(p->getEffectiveState() == YACS::DONE );
  delete p;
}

void YacsLoaderTest::pyremotes()
{
  Proc *p = 0;
  int ret;
  ret = driverTest(p, "samples/pyremote1.xml");
  CPPUNIT_ASSERT(ret == 0);
  CPPUNIT_ASSERT(p->getEffectiveState() == YACS::DONE );
  ret = driverTest(p, "samples/pyremote2.xml");
  CPPUNIT_ASSERT(ret == 0);
  CPPUNIT_ASSERT(p->getEffectiveState() == YACS::DONE );
  ret = driverTest(p, "samples/pyremote3.xml");
  CPPUNIT_ASSERT(ret == 0);
  CPPUNIT_ASSERT(p->getEffectiveState() == YACS::DONE );
  ret = driverTest(p, "samples/pyremote4.xml");
  CPPUNIT_ASSERT(ret == 0);
  CPPUNIT_ASSERT(p->getEffectiveState() == YACS::DONE );
}
