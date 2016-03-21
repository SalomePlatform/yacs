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

// ----------------------------------------------------------------------------
//
%define SALOMEDOCSTRING
"Implementation of nodes for SALOME platform."
%enddef

%module(directors="1",docstring=SALOMEDOCSTRING) SALOMERuntime

//work around SWIG bug #1863647
#if SWIG_VERSION >= 0x010336
#define SwigPyIterator SALOMERuntime_PySwigIterator
#else
#define PySwigIterator SALOMERuntime_PySwigIterator
#endif

%feature("autodoc", "1");

%include engtypemaps.i

#ifdef DOXYGEN_IS_OK
%include docruntime.i
#endif

// ----------------------------------------------------------------------------

%{
#include "SalomeContainer.hxx"
#include "SalomeHPContainer.hxx"
#include "RuntimeSALOME.hxx"
#include "SALOMEDispatcher.hxx"
#include "SalomeProc.hxx"
#include "PythonNode.hxx"
#include "PythonPorts.hxx"
#include "PresetNode.hxx"
#include "PresetPorts.hxx"
#include "CORBANode.hxx"
#include "CORBAPorts.hxx"
#include "StudyNodes.hxx"
#include "StudyPorts.hxx"
#include "TypeConversions.hxx"
#include "TypeCode.hxx"
#include "VisitorSaveSalomeSchema.hxx"
#include "SalomeOptimizerLoop.hxx"
#include "DistributedPythonNode.hxx"
#include "PyOptimizerAlg.hxx"
#include "PyStdout.hxx"
#include <sstream>
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

%include <YACSRuntimeSALOMEExport.hxx>
%include "SalomeContainer.hxx"
%include "SalomeHPContainer.hxx"
%include "RuntimeSALOME.hxx"
%include "SALOMEDispatcher.hxx"
%include "SalomeProc.hxx"
%include "PythonNode.hxx"
%include "PythonPorts.hxx"
%include "XMLPorts.hxx"
%include "PresetNode.hxx"
%include "PresetPorts.hxx"
%include "CORBANode.hxx"
%include "CORBAPorts.hxx"
%include "StudyNodes.hxx"
%include "StudyPorts.hxx"
%include "SalomeOptimizerLoop.hxx"
%include "DistributedPythonNode.hxx"

%extend YACS::ENGINE::OutputPresetPort
{
  void setDataPy(PyObject *ob)
  {
    std::string sss = convertPyObjectXml(self->edGetType(),ob);
    self->setData(sss);
  }
}


// Define the methods that can (default) or cannot (nodirector) be overloaded in Python
%feature("director") YACS::ENGINE::PyOptimizerAlgBase;
%feature("nodirector") YACS::ENGINE::PyOptimizerAlgBase::getTCForInProxy;
%feature("nodirector") YACS::ENGINE::PyOptimizerAlgBase::getTCForOutProxy;
%feature("nodirector") YACS::ENGINE::PyOptimizerAlgBase::initializeProxy;
%feature("nodirector") YACS::ENGINE::PyOptimizerAlgBase::startProxy;
%feature("nodirector") YACS::ENGINE::PyOptimizerAlgBase::takeDecisionProxy;
%feature("nodirector") YACS::ENGINE::PyOptimizerAlgBase::finishProxy;
%feature("nodirector") YACS::ENGINE::PyOptimizerAlgBase::hasError;
%feature("nodirector") YACS::ENGINE::PyOptimizerAlgBase::getError;
%feature("nodirector") YACS::ENGINE::PyOptimizerAlgBase::setError;
%feature("nodirector") YACS::ENGINE::PyOptimizerAlgBase::getTCForAlgoInitProxy;
%feature("nodirector") YACS::ENGINE::PyOptimizerAlgBase::getTCForAlgoResultProxy;
%feature("nodirector") YACS::ENGINE::PyOptimizerAlgBase::getAlgoResultProxy;
%feature("nodirector") YACS::ENGINE::PyOptimizerAlgBase::setProc;

%feature("director") YACS::ENGINE::PyOptimizerAlgASync;
%feature("nodirector") YACS::ENGINE::PyOptimizerAlgASync::getTCForInProxy;
%feature("nodirector") YACS::ENGINE::PyOptimizerAlgASync::getTCForOutProxy;
%feature("nodirector") YACS::ENGINE::PyOptimizerAlgASync::initializeProxy;
%feature("nodirector") YACS::ENGINE::PyOptimizerAlgASync::startProxy;
%feature("nodirector") YACS::ENGINE::PyOptimizerAlgASync::takeDecisionProxy;
%feature("nodirector") YACS::ENGINE::PyOptimizerAlgASync::finishProxy;
%feature("nodirector") YACS::ENGINE::PyOptimizerAlgASync::hasError;
%feature("nodirector") YACS::ENGINE::PyOptimizerAlgASync::getError;
%feature("nodirector") YACS::ENGINE::PyOptimizerAlgASync::setError;
%feature("nodirector") YACS::ENGINE::PyOptimizerAlgASync::signalMasterAndWait;
%feature("nodirector") YACS::ENGINE::PyOptimizerAlgASync::signalSlaveAndWait;
%feature("nodirector") YACS::ENGINE::PyOptimizerAlgASync::start;
%feature("nodirector") YACS::ENGINE::PyOptimizerAlgASync::takeDecision;
%feature("nodirector") YACS::ENGINE::PyOptimizerAlgASync::run;
%feature("nodirector") YACS::ENGINE::PyOptimizerAlgASync::getTCForAlgoInitProxy;
%feature("nodirector") YACS::ENGINE::PyOptimizerAlgASync::getTCForAlgoResultProxy;
%feature("nodirector") YACS::ENGINE::PyOptimizerAlgASync::getAlgoResultProxy;
%feature("nodirector") YACS::ENGINE::PyOptimizerAlgASync::setProc;

%feature("director:except") {
    if ($error != NULL) {
        std::string errorMessage;
        PyObject* new_stderr = YACS::ENGINE::newPyStdOut(errorMessage);
        PySys_SetObject((char*)"stderr", new_stderr);
        PyErr_Print();
        PySys_SetObject((char*)"stderr", PySys_GetObject((char*)"__stderr__"));
        Py_DECREF(new_stderr);
        throw YACS::Exception(errorMessage);
    }
}

// Warning: as PyOptimizerAlgSync is only a typedef in PyOptimizerAlg.hxx, we have to
// directly rename PyOptimizerAlgBase in OptimizerAlgSync. If someday PyOptimizerAlgSync
// becomes a real derived class, this will have to be changed.
%rename(OptimizerAlgSync) YACS::ENGINE::PyOptimizerAlgBase;
%rename(OptimizerAlgASync) YACS::ENGINE::PyOptimizerAlgASync;
%include "PyOptimizerAlg.hxx"
