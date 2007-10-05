// ----------------------------------------------------------------------------
%define SALOMEDOCSTRING
"SALOMERuntime docstring
Implementation of nodes for SALOME platform."
%enddef

%module(docstring=SALOMEDOCSTRING) SALOMERuntime

%feature("autodoc", "0");
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
#include "utilities.h"

using namespace std;
using namespace YACS::ENGINE;

//--- from omniORBpy.h (not present on Debian Sarge packages)
//    (rename omniORBpyAPI in omniORBpy_API)
struct omniORBpy_API
{

  PyObject* (*cxxObjRefToPyObjRef)(const CORBA::Object_ptr cxx_obj,
				   CORBA::Boolean hold_lock);
  // Convert a C++ object reference to a Python object reference.
  // If <hold_lock> is true, caller holds the Python interpreter lock.

  CORBA::Object_ptr (*pyObjRefToCxxObjRef)(PyObject* py_obj,
					   CORBA::Boolean hold_lock);
  // Convert a Python object reference to a C++ object reference.
  // Raises BAD_PARAM if the Python object is not an object reference.
  // If <hold_lock> is true, caller holds the Python interpreter lock.


  omniORBpy_API();
  // Constructor for the singleton. Sets up the function pointers.
};

  omniORBpy_API* api;

%}


// ----------------------------------------------------------------------------


%init
%{
  // init section

  PyObject* omnipy = PyImport_ImportModule((char*)"_omnipy");
  if (!omnipy)
  {
    PyErr_SetString(PyExc_ImportError,
		    (char*)"Cannot import _omnipy");
    return;
  }
  PyObject* pyapi = PyObject_GetAttrString(omnipy, (char*)"API");
  api = (omniORBpy_API*)PyCObject_AsVoidPtr(pyapi);
  Py_DECREF(pyapi);
%}

// ----------------------------------------------------------------------------

%typemap(python,out) YACSGui_ORB::Observer_ptr
{
  MESSAGE("typemap out on CORBA object ptr");
  SCRUTE($1);
  $result = api->cxxObjRefToPyObjRef($1, 1);
  SCRUTE($result);
}

%typemap(python,in) YACSGui_ORB::Observer_ptr
{
  MESSAGE("typemap in on CORBA object ptr");
  try
  {
     CORBA::Object_ptr obj = api->pyObjRefToCxxObjRef($input,1);
     $1 = YACSGui_ORB::Observer::_narrow(obj);
     SCRUTE($1);
  }
  catch (...)
  {
     PyErr_SetString(PyExc_RuntimeError, "not a valid CORBA object ptr");
  }
}

// ----------------------------------------------------------------------------

namespace YACS
{
  namespace ENGINE
  {
    class RuntimeSALOME: public Runtime
    {
    public:
      static void setRuntime(bool ispyext=false); // singleton creation
      virtual ~RuntimeSALOME(); 
    protected:
      RuntimeSALOME();  // singleton
      RuntimeSALOME(bool ispyext);  // singleton
    };

    class SALOMEDispatcher: public Dispatcher
    {
    public:
      void addObserver(YACSGui_ORB::Observer_ptr observer,int numid, std::string event);
      static void setSALOMEDispatcher();
      static SALOMEDispatcher* getSALOMEDispatcher();
    };

  }
}
