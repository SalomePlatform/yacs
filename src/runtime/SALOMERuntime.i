// ----------------------------------------------------------------------------
%define SALOMEDOCSTRING
"SALOMERuntime docstring
Implementation of nodes for SALOME platform."
%enddef

%module(docstring=SALOMEDOCSTRING) SALOMERuntime

%feature("autodoc", "1");
%include std_string.i

// ----------------------------------------------------------------------------

%{
#include "RuntimeSALOME.hxx"
#include "SALOMEDispatcher.hxx"
  
#include <iostream>
#include <string>
#include <sstream>
#include <cassert>
#include <stdexcept>

omniORBpyAPI* api;

using namespace YACS::ENGINE;

%}

// ----------------------------------------------------------------------------

%init
%{
  // init section

  PyObject* omnipy = PyImport_ImportModule((char*)"_omnipy");
  if (!omnipy)
  {
    PyErr_SetString(PyExc_ImportError,(char*)"Cannot import _omnipy");
    return;
  }
  PyObject* pyapi = PyObject_GetAttrString(omnipy, (char*)"API");
  api = (omniORBpyAPI*)PyCObject_AsVoidPtr(pyapi);
  Py_DECREF(pyapi);
%}

// ----------------------------------------------------------------------------

%typemap(python,out) YACSGui_ORB::Observer_ptr
{
  $result = api->cxxObjRefToPyObjRef($1, 1);
}

%typemap(python,in) YACSGui_ORB::Observer_ptr
{
  try
  {
     CORBA::Object_ptr obj = api->pyObjRefToCxxObjRef($input,1);
     $1 = YACSGui_ORB::Observer::_narrow(obj);
  }
  catch (...)
  {
     PyErr_SetString(PyExc_RuntimeError, "not a valid CORBA object ptr");
  }
}

// ----------------------------------------------------------------------------
/*
 * Ownership section
 */
/*
 * End of Ownership section
 */

// ----------------------------------------------------------------------------

%import "Runtime.hxx"
%import "Dispatcher.hxx"
%include "RuntimeSALOME.hxx"
%include "SALOMEDispatcher.hxx"

