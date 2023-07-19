// Copyright (C) 2006-2023  CEA, EDF
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
#include <Python.h>
#include <fstream>
#include <string>
#include <sstream>

#include "Py2yacsTest.hxx"
#include "py2yacs.hxx"
#include "Proc.hxx"
#include "Executor.hxx"
#include "PythonCppUtils.hxx"

#include "RuntimeSALOME.hxx"
#include "PythonPorts.hxx"
#include "InputPort.hxx"
//#include "parsers.hxx"

void Py2yacsTest::setUp()
{
  YACS::ENGINE::RuntimeSALOME::setRuntime();
  //YACS::ENGINE::getRuntime()->init();
}

void Py2yacsTest::tearDown()
{
  //YACS::ENGINE::getRuntime()->fini();
}

static
void verifyCorrectPycode(const char * code_py, const char * function_name,
                         int nbInputPorts, const char** input_ports,
                         int nbOutputPorts, const char** output_ports)
{
  std::cerr << std::endl;
  std::cerr << "-----------------------------------------------" << std::endl;
  std::cerr << code_py << std::endl;
  std::cerr << "-----------------------------------------------" << std::endl;
  std::cerr << "Function :" << function_name << std::endl;
  Py2yacs parser;
  try
  {
    parser.load(code_py);
  }
  catch(Py2yacsException& e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch(std::exception& e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch(...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }
  YACS::ENGINE::Proc* p = parser.createProc(function_name);
  CPPUNIT_ASSERT( p != NULL);
  YACS::ENGINE::Node* n = p->getChildByShortName("default_name");
  CPPUNIT_ASSERT( n != NULL);
  CPPUNIT_ASSERT( n->getNumberOfInputPorts() == nbInputPorts);
  for(int i = 0; i < nbInputPorts; i++)
    CPPUNIT_ASSERT( n->getInputPort(input_ports[i]) != NULL);
  CPPUNIT_ASSERT( n->getNumberOfOutputPorts() == nbOutputPorts);
  for(int i = 0; i < nbOutputPorts; i++)
    CPPUNIT_ASSERT( n->getOutputPort(output_ports[i]) != NULL);
  delete p;
}

static
void verifyWrongPycode(const char* code_py, const char* function_name,
                       const char* error_message)
{
  Py2yacs parser;
  try
  {
    parser.load(code_py);
    YACS::ENGINE::Proc* p = parser.createProc(function_name);
    CPPUNIT_FAIL("Exception expected and no exception occured.");
  }
  catch(Py2yacsException& e)
  {
    std::string what = e.what();
    std::cerr << std::endl;
    std::cerr << "-----------------------------------------------" << std::endl;
    std::cerr << code_py << std::endl;
    std::cerr << "-----------------------------------------------" << std::endl;
    std::cerr << "Function :" << function_name << std::endl;
    std::cerr << "==========EXCEPTION TEXT=======================" << std::endl;
    std::cerr << what << std::endl;
    std::cerr << "===============================================" << std::endl;
    CPPUNIT_ASSERT(what.find(error_message) != std::string::npos);
  }
  catch(std::exception& e)
  {
    CPPUNIT_FAIL(e.what());
  }
}

static
void verifyWrongParser(const char* parser_module, const char* parser_function,
                       const char* error_message)
{
  const char* code_py = "def f():\n"
                        "  return\n";
  Py2yacs parser(parser_module, parser_function);
  try
  {
    parser.load(code_py);
    CPPUNIT_FAIL("Exception expected and no exception occured.");
  }
  catch(Py2yacsException& e)
  {
    std::string what = e.what();
    std::cerr << std::endl;
    std::cerr << "==========EXCEPTION TEXT=======================" << std::endl;
    std::cerr << what << std::endl;
    std::cerr << "===============================================" << std::endl;
    CPPUNIT_ASSERT(what.find(error_message) != std::string::npos);
  }
  catch(std::exception& e)
  {
    CPPUNIT_FAIL(e.what());
  }
}

void Py2yacsTest::t1()
{
  const char * code_py = "def f1(a, b, c):\n"
                         "  y = a*b + c\n"
                         "  return y\n";
  const char* input_ports[] = {"a", "b", "c"};
  const char* output_ports[] = {"y"};
  verifyCorrectPycode(code_py, "f1", 3, input_ports, 1, output_ports);
}

void Py2yacsTest::t2()
{
  const char * code_py = "def f1(a, b, c):\n"
                         "  x = a + b + c\n"
                         "  y = a*b + c\n"
                         "  z = a*b*c\n"
                         "  return x, y, z\n";
  const char* input_ports[] = {"a", "b", "c"};
  const char* output_ports[] = {"x", "y", "z"};
  verifyCorrectPycode(code_py, "f1", 3, input_ports, 3, output_ports);
}

void Py2yacsTest::t3()
{
  const char * code_py = "def f1(a, b, c):\n"
                         "  print(a)\n"
                         "  print(b)\n"
                         "  print(c)\n";
  const char* input_ports[] = {"a", "b", "c"};
  const char** output_ports;
  verifyCorrectPycode(code_py, "f1", 3, input_ports, 0, output_ports);
}

void Py2yacsTest::t4()
{
  const char * code_py = "def f1():\n"
                         "  print('toto')\n"
                         "  return\n";
  const char* input_ports[] = {"a", "b", "c"};
  const char** output_ports;
  verifyCorrectPycode(code_py, "f1", 0, input_ports, 0, output_ports);
}

void Py2yacsTest::t5()
{
  const char * code_py = "def f1(a):\n"
                         "  x = -a\n"
                         "  if a > 0:\n"
                         "    return a\n"
                         "  else:\n"
                         "    return x\n"
                         "class ignoremoi:\n"
                         "  def ignoreF(t):\n"
                         "    return t+1\n"
                         "def f2(v):\n"
                         "  ret = f1(v)\n"
                         "  return ret\n";
  const char* input_ports[] = {"v"};
  const char* output_ports[] = {"ret"};
  verifyCorrectPycode(code_py, "f2", 1, input_ports, 1, output_ports);
}

void Py2yacsTest::no_multiple_returns()
{
  const char * code_py = "def f(a):\n"
                         "  x = -a\n"
                         "  if a > 0:\n"
                         "    return a\n"
                         "  else:\n"
                         "    return x\n";
  verifyWrongPycode(code_py, "f", "multiple returns.");
}

void Py2yacsTest::unaccepted_statement()
{
  const char * code_py = "def f(a):\n"
                         "  return a\n"
                         "x=5\n";
  verifyWrongPycode(code_py, "f", "not accepted statement");
}

/* // This is now accepted
void Py2yacsTest::unaccepted_statement2()
{
  const char * code_py = "def f(a):\n"
                         "  return a\n"
                         "if __name__ == '__main__':"
                         "  print('toto')\n";
  verifyWrongPycode(code_py, "f", "not accepted statement");
}*/

void Py2yacsTest::unaccepted_return()
{
  const char * code_py = "def f(a):\n"
                         "  return 7\n";
  verifyWrongPycode(code_py, "f", "invalid type returned");
}

void Py2yacsTest::unaccepted_return2()
{
  const char * code_py = "def f1(a):\n"
                         "  return 7\n"
                         "def f2(x):\n"
                         "  return f1(x)\n";
  verifyWrongPycode(code_py, "f2", "invalid type returned");
}

void Py2yacsTest::syntaxError()
{
  const char * code_py = "bla bla bla\n"
                         "  return f1(x)\n";
  verifyWrongPycode(code_py, "f2", "invalid syntax");
}

void Py2yacsTest::badFunctionName()
{
  const char * code_py = "def f1(a):\n"
                         "  x=7\n"
                         "  return x\n"
                         "def f2(x):\n"
                         "  y=8\n"
                         "  return y\n";
  verifyWrongPycode(code_py, "nonexistant", "Function not found:");
}

void Py2yacsTest::schemaExec()
{
  const char * code_py = "def f(a):\n"
                         "  x = a\n"
                         "  return x\n";
  Py2yacs parser;
  try
  {
    parser.load(code_py);
  }
  catch(Py2yacsException& e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch(...)
  {
    CPPUNIT_FAIL("Unknown exception");
  }
  YACS::ENGINE::Proc* p = parser.createProc("f");
  CPPUNIT_ASSERT( p != NULL);
  YACS::ENGINE::Node* n = p->getChildByShortName("default_name");
  CPPUNIT_ASSERT( n != NULL);
  CPPUNIT_ASSERT( n->getInputPort("a") != NULL);
  CPPUNIT_ASSERT( n->getOutputPort("x") != NULL);
  // run the schema
  n->getInputPort("a")->edInit(42.);
  YACS::ENGINE::Executor executor;
  executor.RunW(p, 0);
  // verify the output port value
  YACS::ENGINE::OutputPyPort* var = dynamic_cast<YACS::ENGINE::OutputPyPort*>(n->getOutputPort("x"));
  CPPUNIT_ASSERT(var);
  double x = -2.0;
  PyObject* pyVal = var->get();
  {
    AutoGIL agil;
    CPPUNIT_ASSERT(pyVal);
    x = PyFloat_AsDouble(pyVal);
    CPPUNIT_ASSERT( PyErr_Occurred()==nullptr );// check pyVal is interpretable as float
  }
  CPPUNIT_ASSERT_DOUBLES_EQUAL(42., x, 1.E-12);
  p->shutdown(10); // kill all the containers
}

// Test the behaviour when there is an error in the python parser
void Py2yacsTest::parserErrors()
{
  verifyWrongParser("bad_parsers", "p1", "0 positional arguments");
  verifyWrongParser("bad_parsers", "p2", "'str' object has no attribute 'name'");
  verifyWrongParser("bad_parsers", "p3", "should be a python list");
  verifyWrongParser("bad_parsers", "p4", "unsupported operand type");
  verifyWrongParser("bad_parsers", "f", "Cannot find the parsing function");
  verifyWrongParser("err_py2yacs_invalid", "get_properties", "invalid syntax");
  verifyWrongParser("no_file", "f", "Failed to load");
  verifyWrongParser("bad_parsers", "p5", "is not a string");
  verifyWrongParser("bad_parsers", "p6", "is not a string");
  verifyWrongParser("bad_parsers", "p7", "is not a string");
  verifyWrongParser("bad_parsers", "p8", "Attribute 'name' should be a string.");
  verifyWrongParser("bad_parsers", "p9", "Not a python list");
  verifyWrongParser("bad_parsers", "p10", "is not a string");
}

void Py2yacsTest::globalVerification()
{
  std::ifstream file_stream("exemple_py2yacs.py");
  if(!file_stream)
    return;

  std::stringstream buffer;
  buffer << file_stream.rdbuf();
  Py2yacs parser;
  parser.load(buffer.str());
  std::list<FunctionProperties>::const_iterator it_fp;
  const std::list<FunctionProperties>& functions = parser.getFunctionProperties();
  for(it_fp=functions.begin();it_fp!=functions.end();it_fp++)
  {
    std::cerr << "Function :" << it_fp->_name << std::endl;
    std::list<std::string>::const_iterator it;
    std::cerr << "Input ports :" ;
    for(it=it_fp->_input_ports.begin();it!=it_fp->_input_ports.end();it++)
      std::cerr << *it << ",";
    std::cerr << std::endl;
    std::cerr << "Output ports :" ;
    for(it=it_fp->_output_ports.begin();it!=it_fp->_output_ports.end();it++)
      std::cerr << *it << ",";
    std::cerr << std::endl;
    std::cerr << "Imports :" ;
    for(it=it_fp->_imports.begin();it!=it_fp->_imports.end();it++)
      std::cerr << *it << ",";
    std::cerr << std::endl;
    std::cerr << "Errors :" ;
    for(it=it_fp->_errors.begin();it!=it_fp->_errors.end();it++)
      std::cerr << *it << std::endl;
    std::cerr << std::endl;
    std::cerr << "-----------------------------------------" << std::endl;
  }

}
