// ----------------------------------------------------------------------------
%define SALOMEDOCSTRING
"Implementation of nodes for SALOME platform."
%enddef

%module(docstring=SALOMEDOCSTRING) SALOMERuntime

//work around SWIG bug #1863647
#define PySwigIterator SALOMERuntime_PySwigIterator

%feature("autodoc", "1");

%include engtypemaps.i

// ----------------------------------------------------------------------------

%{
#include "RuntimeSALOME.hxx"
#include "SALOMEDispatcher.hxx"
#include "SalomeProc.hxx"
#include "PythonNode.hxx"
#include "PythonPorts.hxx"
#include "CORBANode.hxx"
#include "CORBAPorts.hxx"
#include "TypeConversions.hxx"
#include "TypeCode.hxx"
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
  api = (omniORBPYAPI*)PyCObject_AsVoidPtr(pyapi);
  Py_DECREF(pyapi);
%}

// ----------------------------------------------------------------------------

#ifdef SWIGPYTHON
%typemap(out) YACS_ORB::Observer_ptr
{
  $result = api->cxxObjRefToPyObjRef($1, 1);
}

%typemap(in) YACS_ORB::Observer_ptr
{
  try
  {
     CORBA::Object_ptr obj = api->pyObjRefToCxxObjRef($input,1);
     $1 = YACS_ORB::Observer::_narrow(obj);
  }
  catch (...)
  {
     PyErr_SetString(PyExc_RuntimeError, "not a valid CORBA object ptr");
     return NULL;
  }
}

%typemap(out) YACS::ENGINE::PyObj * "Py_INCREF($1); $result = $1;";

#endif

// ----------------------------------------------------------------------------

%import "pilot.i"

%rename(getSALOMERuntime) YACS::ENGINE::getSALOMERuntime; // to suppress a 503 warning
%ignore omniORBpyAPI;

%include "RuntimeSALOME.hxx"
%include "SALOMEDispatcher.hxx"
%include "SalomeProc.hxx"
%include "PythonNode.hxx"
%include "PythonPorts.hxx"
%include "CORBANode.hxx"
%include "CORBAPorts.hxx"

%extend YACS::ENGINE::InputCorbaPort
{
  PyObject * getPyObj()
  {
    CORBA::TypeCode_var tc=self->getAny()->type();
    if (!tc->equivalent(CORBA::_tc_null))
      return convertCorbaPyObject(self->edGetType(),self->getAny());
    else
      {
        Py_INCREF(Py_None);
        return Py_None;
      }
  }
}
%extend YACS::ENGINE::OutputCorbaPort
{
  PyObject * getPyObj()
  {
    CORBA::TypeCode_var tc=self->getAny()->type();
    if (!tc->equivalent(CORBA::_tc_null))
      return convertCorbaPyObject(self->edGetType(),self->getAny());
    else
      {
        Py_INCREF(Py_None);
        return Py_None;
      }
  }
}

