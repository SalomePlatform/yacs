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
#include <Python.h>
#include <sstream>
#include "py2yacs.hxx"
#include "RuntimeSALOME.hxx"
#include "Proc.hxx"
#include "InlineNode.hxx"
#include "AutoGIL.hxx"

Py2yacsException::Py2yacsException(const std::string& what)
: std::exception(),
  _what(what)
{
}

Py2yacsException::~Py2yacsException()throw ()
{
}

const char * Py2yacsException::what() const throw ()
{
  return _what.c_str();
}


Py2yacs::Py2yacs()
: _python_parser_module("py2yacs"),
  _python_parser_function("get_properties"),
  _functions(),
  _global_errors(),
  _python_code()
{
}

Py2yacs::Py2yacs(const std::string& python_parser_module,
        const std::string& python_parser_function)
: _python_parser_module(python_parser_module),
  _python_parser_function(python_parser_function),
  _functions(),
  _global_errors(),
  _python_code()
{
}

const std::list<std::string>& Py2yacs::getGlobalErrors() const
{
  return _global_errors;
}

const std::list<FunctionProperties>& Py2yacs::getFunctionProperties()const
{
  return _functions;
}

// Copy a python list of strings to a c++ list of strings.
// Return an error string. An empty string means no error.
static
std::string copyList(PyObject *pyList, std::list<std::string>& cppList)
{
  std::string error;
  if(not PyList_Check(pyList))
  {
    error = "Not a python list.\n";
    //throw Py2yacsException("Not a python list.");
  }
  else
  {
    int n = PyList_Size(pyList);
    for(int i=0; i<n; i++)
    {
      PyObject *elem = PyList_GetItem(pyList,i);
      if(not PyBytes_Check(elem))
      {
        std::stringstream message;
        message << "List element number " << i << " is not a string.\n";
        error += message.str();
        // throw Py2yacsException(message.str());
      }
      else
      {
        const char * portName = PyBytes_AsString(elem);
        cppList.push_back(portName);
      }
    }
  }
  return error;
}

static
std::string getPyErrorText()
{
  std::string result="";
  if (PyErr_Occurred())
  {
    PyObject *ptype, *pvalue, *ptraceback;
    PyObject *pystr, *module_name, *pyth_module, *pyth_func;
    PyErr_Fetch(&ptype, &pvalue, &ptraceback);
    pystr = PyObject_Str(pvalue);
    result = PyBytes_AsString(pystr);
    result += "\n";
    Py_DECREF(pystr);
    
    /* See if we can get a full traceback */
    if(ptraceback)
    {
      module_name = PyBytes_FromString("traceback");
      pyth_module = PyImport_Import(module_name);
      Py_DECREF(module_name);
      if (pyth_module)
      {
        pyth_func = PyObject_GetAttrString(pyth_module, "format_exception");
        if (pyth_func && PyCallable_Check(pyth_func))
        {
          PyObject *pyList;
          pyList = PyObject_CallFunctionObjArgs(pyth_func, ptype, pvalue, ptraceback, NULL);
          if(pyList)
          {
            int n = PyList_Size(pyList);
            for(int i=0; i<n; i++)
            {
              pystr = PyList_GetItem(pyList,i);
              result += PyBytes_AsString(pystr);
            }
            Py_DECREF(pyList);
          }
        }
        Py_XDECREF(pyth_func);
        Py_DECREF(pyth_module);
      }
    }
    Py_XDECREF(ptype);
    Py_XDECREF(pvalue);
    Py_XDECREF(ptraceback);
  }
  return result;
}

static
PyObject* checkAndGetAttribute(PyObject *p,
                               const char* attribute,
                               std::string& error)
{
  PyObject *pAttribute = PyObject_GetAttrString(p, attribute);
  if(not pAttribute)
  {
    error += "Attribute '";
    error += attribute;
    error += "' not found in the returned value of the parsing function.\n";
    error += getPyErrorText();
  }
  return pAttribute;
}

void Py2yacs::load(const std::string& python_code)
{
    PyObject *pModule, *pDict, *pFunc;
    PyObject *pArgs, *pValue;
    int i;
    std::string errorMessage="";
    _python_code = python_code;
    _functions.clear();
    _global_errors.clear();
    
    // Py_Initialize();
    YACS::ENGINE::AutoGIL agil;
    pValue = PyBytes_FromString(_python_parser_module.c_str());
    pModule = PyImport_Import(pValue);
    Py_DECREF(pValue);

    if (not pModule)
    {
      errorMessage  = getPyErrorText();
      errorMessage += "\nFailed to load ";
      errorMessage += _python_parser_module;
      errorMessage += ".\n";
    }
    else
    {
      pFunc = PyObject_GetAttrString(pModule, _python_parser_function.c_str());

      if (pFunc && PyCallable_Check(pFunc))
      {
        pArgs = PyTuple_New(1);
        pValue = PyBytes_FromString(python_code.c_str());
        PyTuple_SetItem(pArgs, 0, pValue);
        
        pValue = PyObject_CallObject(pFunc, pArgs);
        Py_DECREF(pArgs);
        if (not pValue)
            errorMessage = getPyErrorText();
        else
        {
          if (not PyTuple_Check(pValue))
          {
            errorMessage += "Parsing function should return a tuple of two string lists.\n";
          }
          int n = PyTuple_Size(pValue);
          if(n != 2)
          {
            errorMessage += "Parsing function should return two string lists.\n";
          }
          PyObject *pyList = PyTuple_GetItem(pValue, 0);
          if(not PyList_Check(pyList))
          {
            errorMessage += "The first returned value of the parsing function"
                            " should be a python list.\n";
          }
          else
          {
            n = PyList_Size(pyList);
            for(int i=0; i<n; i++)
            {
              PyObject *fpy = PyList_GetItem(pyList,i);
              PyObject *pAttribute;
              
              if(pAttribute = checkAndGetAttribute(fpy, "name", errorMessage))
              {
                if(not PyBytes_Check(pAttribute))
                {
                  errorMessage += "Attribute 'name' should be a string.\n";
                  Py_DECREF(pAttribute);
                }
                else
                {
                  _functions.push_back(FunctionProperties());
                  FunctionProperties& fcpp = _functions.back();
                  fcpp._name=PyBytes_AsString(pAttribute);
                  Py_DECREF(pAttribute);
                  
                  if(pAttribute = checkAndGetAttribute(fpy, "inputs", errorMessage))
                    errorMessage += copyList(pAttribute, fcpp._input_ports);
                  Py_XDECREF(pAttribute);
                  
                  if(pAttribute = checkAndGetAttribute(fpy, "outputs", errorMessage))
                    // None value means no return statement in the function
                    if(pAttribute != Py_None)
                      errorMessage += copyList(pAttribute,fcpp._output_ports);
                  Py_XDECREF(pAttribute);
                    
                  if(pAttribute = checkAndGetAttribute(fpy, "errors", errorMessage))
                    errorMessage += copyList(pAttribute, fcpp._errors);
                  Py_XDECREF(pAttribute);
                  
                  if(pAttribute = checkAndGetAttribute(fpy, "imports", errorMessage))
                    errorMessage += copyList(pAttribute, fcpp._imports);
                  Py_XDECREF(pAttribute);
                }
              }
            }
          }
          errorMessage += copyList(PyTuple_GetItem(pValue, 1), _global_errors);
          Py_DECREF(pValue);
        }
      }
      else
      {
        errorMessage  = getPyErrorText();
        errorMessage += "\nCannot find the parsing function '";
        errorMessage += _python_parser_function;
        errorMessage += "' in python module '";
        errorMessage += _python_parser_module;
        errorMessage += "'.\n";
      }
      Py_XDECREF(pFunc);
      Py_DECREF(pModule);
    }
    
    if(not errorMessage.empty())
      throw Py2yacsException(errorMessage);
    // Py_Finalize();
}

void Py2yacs::save(const std::string& file_path,
                   const std::string& python_function)const
{
  YACS::ENGINE::Proc* schema = createProc(python_function);
  schema->saveSchema(file_path);
  delete schema;
}

YACS::ENGINE::Proc* Py2yacs::createProc(const std::string& python_function)const
{
  if(not _global_errors.empty())
  {
    std::string error_message = "The python script contains errors.\n";
    std::list<std::string>::const_iterator it;
    for(it = _global_errors.begin(); it != _global_errors.end(); it++)
      error_message += (*it) + "\n";
    throw Py2yacsException(error_message);
  }
  
  // find function properties
  std::list<FunctionProperties>::const_iterator fn_prop = _functions.begin();
  while(fn_prop != _functions.end() and fn_prop->_name != python_function)
    fn_prop++;
  
  if(fn_prop == _functions.end())
  {
    throw Py2yacsException(std::string("Function not found: ")+python_function);
  }
  
  if(not fn_prop->_errors.empty())
  {
    std::string error_message = "Function contains errors.\n";
    std::list<std::string>::const_iterator it;
    for(it = fn_prop->_errors.begin(); it != fn_prop->_errors.end(); it++)
      error_message += (*it) + "\n";
    throw Py2yacsException(error_message);
  }
  
  // add the call to the function at the end of the script
  std::stringstream fn_call;
  fn_call << std::endl;
  std::list<std::string>::const_iterator it;
  bool first = true;
  for(it=fn_prop->_output_ports.begin();
      it!=fn_prop->_output_ports.end();
      it++)
  {
    if (not first)
      fn_call << ",";
    first = false;
    fn_call << *it;
  }
  fn_call << "=" << python_function << "(";
  first = true;
  for(it = fn_prop->_input_ports.begin();
      it != fn_prop->_input_ports.end();
      it++)
  {
    if (not first)
      fn_call << ",";
    first = false;
    fn_call << *it;
  }
  fn_call << ")" << std::endl;
  std::string node_body = _python_code + fn_call.str();
  
  YACS::ENGINE::Proc* schema;
  YACS::ENGINE::RuntimeSALOME::setRuntime();
  YACS::ENGINE::RuntimeSALOME* runtime = YACS::ENGINE::getSALOMERuntime();
  
  // build the YACS schema
  const char * node_name = "default_name";
  schema = runtime->createProc("Schema");
  YACS::ENGINE::InlineNode* node = runtime->createScriptNode("", node_name);
  schema->edAddChild(node);
  node->setScript(node_body);
  YACS::ENGINE::TypeCode *tc_double = runtime->getTypeCode("double");
  
  for(it = fn_prop->_input_ports.begin();
      it != fn_prop->_input_ports.end();
      it++)
    node->edAddInputPort(*it, tc_double);
  
  for(it = fn_prop->_output_ports.begin();
      it != fn_prop->_output_ports.end();
      it++)
    node->edAddOutputPort(*it, tc_double);
  
  return schema;
}