// Copyright (C) 2007-2016  CEA/DEN, EDF R&D
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
// Author : InckA


#include "YACSPMMLBasicsTest1.hxx"
#include "PMMLlib.hxx"

#include "yacsconfig.h"
#include "RuntimeSALOME.hxx"
#include "PythonPorts.hxx"
#include "CORBAPorts.hxx"
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


void YACSPMMLBasicsTest1::setUp()
{
    resourcesDir = "samples/";    
}

void YACSPMMLBasicsTest1::tearDown()
{
}


void YACSPMMLBasicsTest1::testYACSdriverLinearRegression()
{
    std::string xmlFilename = resourcesDir + "schemaLR2.xml";  
    Proc *p = 0;
    int ret = driverTest(p, xmlFilename.c_str() );
    CPPUNIT_ASSERT(ret == 0);
    DEBTRACE("Proc *p = " << p);
    CPPUNIT_ASSERT(p != 0);
    if (p)
    {        
        YACS::ENGINE::Node* node = p->nodeMap[string("PyGetRes")];
        YACS::ENGINE::OutputPort* outputPort = node->getOutputPort(string("res")); 
        string str =  outputPort->getAsString(); 
        CPPUNIT_ASSERT_EQUAL(str, string("True")); 
        delete p;         
    }  
}

void YACSPMMLBasicsTest1::testYACSdriverNeuralNetwork()
{
    std::string xmlFilename = resourcesDir + "schemaANN2.xml";  
    Proc *p = 0;
    int ret = driverTest(p, xmlFilename.c_str() );
    CPPUNIT_ASSERT(ret == 0);
    DEBTRACE("Proc *p = " << p);
    CPPUNIT_ASSERT(p != 0);    
    if (p)
    { 
        YACS::ENGINE::Node* node = p->nodeMap[string("PyGetRes")];
        YACS::ENGINE::OutputPort* outputPort = node->getOutputPort(string("res")); 
        string str =  outputPort->getAsString(); 
        CPPUNIT_ASSERT_EQUAL(str, string("True")); 
        delete p;    
    }
}  

void YACSPMMLBasicsTest1::testYACSdriver_LRANN()
{
    std::string xmlFilename = resourcesDir + "schemaANNLR2.xml";  
    Proc *p = 0;
    int ret = driverTest(p, xmlFilename.c_str() );
    CPPUNIT_ASSERT(ret == 0);
    DEBTRACE("Proc *p = " << p);
    CPPUNIT_ASSERT(p != 0);
    if (p) 
    { 
        YACS::ENGINE::Node* node = p->nodeMap[string("PyGetRes")];
        YACS::ENGINE::OutputPort* outputPort = node->getOutputPort(string("res")); 
        string str =  outputPort->getAsString(); 
        CPPUNIT_ASSERT_EQUAL(str, string("True")); 
        delete p;   
    }
}  

void YACSPMMLBasicsTest1::testYACSdriver_PmmlDoesNotExist()
{
    std::string xmlFilename = resourcesDir + "schemaPmmlDoesNotExist.xml";  
    Proc *p = 0;
    int ret = driverTest(p, xmlFilename.c_str() );
    CPPUNIT_ASSERT(ret == 0);
    DEBTRACE("Proc *p = " << p);
    CPPUNIT_ASSERT(p != 0);
    if (p)
    { 
        YACS::ENGINE::Node* node = p->nodeMap[string("PyGetRes")];
        YACS::ENGINE::OutputPort* outputPort = node->getOutputPort(string("res"));        
        string str =  outputPort->getAsString(); 
        CPPUNIT_ASSERT_EQUAL(str, string("None")); 
        delete p;     
    }
} 

