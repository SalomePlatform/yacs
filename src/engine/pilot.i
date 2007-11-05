// ----------------------------------------------------------------------------
%define DOCSTRING
"Pilot docstring
All is needed to create and execute a calculation schema."
%enddef

%module(directors="1",docstring=DOCSTRING) pilot

%feature("autodoc", "0");

%include std_except.i
%include std_string.i
%include std_map.i
%include std_list.i
%include std_vector.i
%include std_set.i

// ----------------------------------------------------------------------------

%{
#include "yacsconfig.h"

#ifdef OMNIORB
#include <omniORB4/CORBA.h>

//--- from omniORBpy.h (not present on Debian Sarge packages)
struct omniORBpyAPI
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

  PyObject* (*handleCxxSystemException)(const CORBA::SystemException& ex);
  // Sets the Python exception state to reflect the given C++ system
  // exception. Always returns NULL. The caller must hold the Python
  // interpreter lock.
};

omniORBpyAPI* api;

#define OMNIPY_CATCH_AND_HANDLE_SYSTEM_EXCEPTIONS \
catch (const CORBA::SystemException& ex) { \
  return api->handleCxxSystemException(ex); \
}
#else
#define OMNIPY_CATCH_AND_HANDLE_SYSTEM_EXCEPTIONS 
#endif

#include "Runtime.hxx"
#include "Logger.hxx"
#include "LinkInfo.hxx"
#include "Loop.hxx"
#include "WhileLoop.hxx"
#include "ForLoop.hxx"
#include "ForEachLoop.hxx"
#include "OptimizerLoop.hxx"
#include "Switch.hxx"
#include "Bloc.hxx"
#include "Proc.hxx"
#include "ElementaryNode.hxx"
#include "InlineNode.hxx"
#include "ServiceNode.hxx"
#include "ComponentInstance.hxx"
#include "Dispatcher.hxx"
#include "ServiceInlineNode.hxx"

#include "TypeCode.hxx"
#include "Catalog.hxx"
#include "ComponentDefinition.hxx"

#include "OutPort.hxx"
#include "InputPort.hxx"
#include "OutputPort.hxx"
#include "InputDataStreamPort.hxx"
#include "OutputDataStreamPort.hxx"

#include "ExecutorSwig.hxx"
#include "Executor.hxx"
#include "Task.hxx"
#include "Scheduler.hxx"
#include "VisitorSaveSchema.hxx"
#include "Container.hxx"

#include <pthread.h>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <map>
#include <list>

using namespace YACS::ENGINE;
using namespace std;

class InterpreterUnlocker
{
public:
  InterpreterUnlocker() 
    {
      _save = PyEval_SaveThread(); // allow Python threads to run
    }
  ~InterpreterUnlocker() 
    {
      PyEval_RestoreThread(_save); // restore the thread state
    }
private:
  PyThreadState *_save;
};

#define POINTER_OWN 1

namespace YACS
{
  namespace ENGINE
  {
    class PyObserver:public Observer
    {
    public:
      virtual void notifyObserver(Node* object,const std::string& event)
      {
        //YACS engine processing is pure C++ : need to take the GIL
        PyGILState_STATE gstate = PyGILState_Ensure();

        try
          {
            pynotify(object,event);
            if (PyErr_Occurred()) 
              {
                // print the exception and clear it
                PyErr_Print();
                //do not propagate the python exception (ignore it)
              }
          }
        catch (...)
          {
            //ignore this exception:probably Swig::DirectorException
            if (PyErr_Occurred()) 
                PyErr_Print();
          }
        PyGILState_Release(gstate);
      }
      virtual void pynotify(Node* object,const std::string& event)
      {
        //To inherit (Python class)
        std::cerr << "pynotify " << event << object << std::endl;
      }
    };

  }
}
    static PyObject* convertNode(YACS::ENGINE::Node* node)
    {
      PyObject* ob;
      //should use $descriptor(YACS::ENGINE::Bloc *) and so on but $descriptor is not defined
      //here. It is better to define a helper function to avoid code bloat
      //You must respect inheritance order in casting : Bloc before ComposedNode and so on
      if(dynamic_cast<YACS::ENGINE::Bloc *>(node))
        ob=SWIG_NewPointerObj((void*)node,SWIGTYPE_p_YACS__ENGINE__Bloc,0);
      else if(dynamic_cast<YACS::ENGINE::ForLoop *>(node))
        ob=SWIG_NewPointerObj((void*)node,SWIGTYPE_p_YACS__ENGINE__ForLoop,0);
      else if(dynamic_cast<YACS::ENGINE::WhileLoop *>(node))
        ob=SWIG_NewPointerObj((void*)node,SWIGTYPE_p_YACS__ENGINE__WhileLoop,0);
      else if(dynamic_cast<YACS::ENGINE::ForEachLoop *>(node))
        ob=SWIG_NewPointerObj((void*)node,SWIGTYPE_p_YACS__ENGINE__ForEachLoop,0);
      else if(dynamic_cast<YACS::ENGINE::Switch *>(node))
        ob=SWIG_NewPointerObj((void*)node,SWIGTYPE_p_YACS__ENGINE__Switch,0);
      else if(dynamic_cast<YACS::ENGINE::ComposedNode *>(node))
        ob=SWIG_NewPointerObj((void*)node,SWIGTYPE_p_YACS__ENGINE__ComposedNode,0);
      else if(dynamic_cast<YACS::ENGINE::InlineFuncNode *>(node))
        ob=SWIG_NewPointerObj((void*)node,SWIGTYPE_p_YACS__ENGINE__InlineFuncNode,0);
      else if(dynamic_cast<YACS::ENGINE::InlineNode *>(node))
        ob=SWIG_NewPointerObj((void*)node,SWIGTYPE_p_YACS__ENGINE__InlineNode,0);
      else if(dynamic_cast<YACS::ENGINE::ServiceInlineNode *>(node))
        ob=SWIG_NewPointerObj((void*)node,SWIGTYPE_p_YACS__ENGINE__ServiceInlineNode,0);
      else if(dynamic_cast<YACS::ENGINE::ServiceNode *>(node))
        ob=SWIG_NewPointerObj((void*)node,SWIGTYPE_p_YACS__ENGINE__ServiceNode,0);
      else if(dynamic_cast<YACS::ENGINE::ElementaryNode *>(node))
        ob=SWIG_NewPointerObj((void*)node,SWIGTYPE_p_YACS__ENGINE__ElementaryNode,0);
      else
        ob=SWIG_NewPointerObj((void*)node,SWIGTYPE_p_YACS__ENGINE__Node,0);
      return ob;
    }

%}

%init
%{
  // init section
#ifdef OMNIORB
  PyObject* omnipy = PyImport_ImportModule((char*)"_omnipy");
  if (!omnipy)
  {
    PyErr_SetString(PyExc_ImportError,(char*)"Cannot import _omnipy");
    return;
  }
  PyObject* pyapi = PyObject_GetAttrString(omnipy, (char*)"API");
  api = (omniORBpyAPI*)PyCObject_AsVoidPtr(pyapi);
  Py_DECREF(pyapi);
#endif
%}

%ignore operator=;

/*
 * Template section
 */
%template()              std::pair<std::string, YACS::ENGINE::TypeCode *>;
%template()              std::pair<std::string, YACS::ENGINE::Node *>;
%template()              std::pair<std::string, YACS::ENGINE::InlineNode *>;
%template()              std::pair<std::string, YACS::ENGINE::ServiceNode *>;
%template()              std::pair<YACS::ENGINE::OutPort *,YACS::ENGINE::InPort *>;
%template()              std::pair<YACS::ENGINE::InPort *,YACS::ENGINE::OutPort *>;
%template(TCmap)         std::map<std::string, YACS::ENGINE::TypeCode *>;
%template(NODEmap)       std::map<std::string, YACS::ENGINE::Node *>;
%template(INODEmap)      std::map<std::string, YACS::ENGINE::InlineNode *>;
%template(SNODEmap)      std::map<std::string, YACS::ENGINE::ServiceNode *>;
%template(strvec)        std::vector<std::string>;
%template(linksvec)      std::vector< std::pair<YACS::ENGINE::OutPort *,YACS::ENGINE::InPort *> >;
%template(linkvec)       std::vector< std::pair<YACS::ENGINE::InPort *,YACS::ENGINE::OutPort *> >;
%template(inlist)        std::list<YACS::ENGINE::InPort *>;
%template(outlist)       std::list<YACS::ENGINE::OutPort *>;
%template(inputlist)     std::list<YACS::ENGINE::InputPort *>;
%template(outputlist)    std::list<YACS::ENGINE::OutputPort *>;
%template(instreamlist)  std::list<YACS::ENGINE::InputDataStreamPort *>;
%template(outstreamlist) std::list<YACS::ENGINE::OutputDataStreamPort *>;

%template()              std::pair<std::string, YACS::ENGINE::CatalogLoader *>;
%template(loadermap)     std::map<std::string,YACS::ENGINE::CatalogLoader *>;
%template()              std::pair<std::string, YACS::ENGINE::ComposedNode *>;
%template(composedmap)   std::map<std::string,YACS::ENGINE::ComposedNode *>;
%template()              std::pair<std::string, YACS::ENGINE::ComponentDefinition *>;
%template(compomap)      std::map<std::string, YACS::ENGINE::ComponentDefinition *>;

#if SWIG_VERSION >= 0x010329
%template()        std::list<int>;
%template()        std::list<std::string>;
#else

%typemap(python,out) std::list<int>
{
  int i;
  std::list<int>::iterator iL;

  $result = PyList_New($1.size());
  for (i=0, iL=$1.begin(); iL!=$1.end(); i++, iL++)
    PyList_SetItem($result,i,PyLong_FromLong((*iL))); 
}

%typemap(python,out) std::list<std::string>
{
  int i;
  std::list<std::string>::iterator iL;

  $result = PyList_New($1.size());
  for (i=0, iL=$1.begin(); iL!=$1.end(); i++, iL++)
    PyList_SetItem($result,i,PyString_FromString((*iL).c_str())); 
}

%typemap(python,in) std::list<std::string>
{
  /* Check if input is a list */
  if (PyList_Check($input))
    {
      int size = PyList_Size($input);
      int i = 0;
      std::list<std::string> myList;
      $1 = myList;
      for (i = 0; i < size; i++)
        {
          PyObject *o = PyList_GetItem($input,i);
          if (PyString_Check(o))
            $1.push_back(std::string(PyString_AsString(PyList_GetItem($input,i))));
          else
            {
              PyErr_SetString(PyExc_TypeError,"list must contain strings");
              return NULL;
            }
        }
    }
  else
    {
      PyErr_SetString(PyExc_TypeError,"not a list");
      return NULL;
    }
}
#endif

%typemap(python,in) std::list<YACS::ENGINE::TypeCodeObjref*>
{
  // Check if input is a list 
  if (PyList_Check($input))
    {
      int size = PyList_Size($input);
      int i = 0;
      std::list<YACS::ENGINE::TypeCodeObjref*> myList;
      $1 = myList;
      for (i = 0; i < size; i++)
        {
          PyObject *o = PyList_GetItem($input,i);
          YACS::ENGINE::TypeCode* temp;
          if ((SWIG_ConvertPtr(o, (void **) &temp, $descriptor(YACS::ENGINE::TypeCode*),0)) == -1) 
            {
              PyErr_SetString(PyExc_TypeError,"not a YACS::ENGINE::TypeCode*");
              return NULL;
            }
          else
            {
              if(temp->kind() == Objref)
                $1.push_back((TypeCodeObjref*)temp);
              else
                {
                  PyErr_SetString(PyExc_TypeError,"not a YACS::ENGINE::TypeCodeObjref*");
                  return NULL;
                }
            }
        }
    }
  else
    {
      PyErr_SetString(PyExc_TypeError,"not a list");
      return NULL;
    }
}

%typemap(python,out) YACS::ENGINE::Node*
{
  $result=convertNode($1);
}

%typemap(python,out) std::set<YACS::ENGINE::Node *>
{
  int i;
  std::set<YACS::ENGINE::Node *>::iterator iL;

  $result = PyList_New($1.size());
  PyObject * ob;
  for (i=0, iL=$1.begin(); iL!=$1.end(); i++, iL++)
    {
      ob=convertNode(*iL);
      PyList_SetItem($result,i,ob); 
    }
}

%typemap(python,out) std::set<YACS::ENGINE::InPort *>
{
  std::set<YACS::ENGINE::InPort *>::iterator iL;
  $result = PyList_New(0);
  PyObject * ob;
  int status;
  for (iL=$1.begin(); iL!=$1.end(); iL++)
    {
      if(dynamic_cast<InputPort *>(*iL))
        ob=SWIG_NewPointerObj((void*)(*iL),$descriptor(YACS::ENGINE::InputPort *), 0); 
      else if(dynamic_cast<InputDataStreamPort *>(*iL))
        ob=SWIG_NewPointerObj((void*)(*iL),$descriptor(YACS::ENGINE::InputDataStreamPort *), 0); 
      else
        ob=SWIG_NewPointerObj((void*)(*iL),$descriptor(YACS::ENGINE::InPort *), 0); 

      //ob=swig::from((YACS::ENGINE::InPort *)(*iL));

      status=PyList_Append($result,ob);
      Py_DECREF(ob);
      if (status < 0)
        {
          PyErr_SetString(PyExc_TypeError,"cannot build the inport list");
          return NULL;
        }
    }
}

%typemap(python,out) std::set<YACS::ENGINE::OutPort *>
{
  std::set<YACS::ENGINE::OutPort *>::iterator iL;
  $result = PyList_New(0);
  PyObject * ob;
  int status;
  for (iL=$1.begin(); iL!=$1.end(); iL++)
    {
      if(dynamic_cast<OutputPort *>(*iL))
        ob=SWIG_NewPointerObj((void*)(*iL),$descriptor(YACS::ENGINE::OutputPort *), 0); 
      else if(dynamic_cast<OutputDataStreamPort *>(*iL))
        ob=SWIG_NewPointerObj((void*)(*iL),$descriptor(YACS::ENGINE::OutputDataStreamPort *), 0); 
      else
        ob=SWIG_NewPointerObj((void*)(*iL),$descriptor(YACS::ENGINE::OutPort *), 0); 

      //ob=swig::from((YACS::ENGINE::OutPort *)(*iL));

      status=PyList_Append($result,ob);
      Py_DECREF(ob);
      if (status < 0)
        {
          PyErr_SetString(PyExc_TypeError,"cannot build the outport list");
          return NULL;
        }
    }
}

/*
 * End of Template section
 */

/*
 * Exception section
 */
// a general exception handler
%exception {
   try 
   {
      $action
   } 
   catch(YACS::Exception& _e) 
   {
      PyErr_SetString(PyExc_ValueError,_e.what());
      return NULL;
   } 
   OMNIPY_CATCH_AND_HANDLE_SYSTEM_EXCEPTIONS
}

// a specific exception handler = generic + release lock
%define PYEXCEPTION(name)
%exception name {
   try {
      InterpreterUnlocker _l;
      $action
   } catch(YACS::Exception& _e) {
      PyErr_SetString(PyExc_ValueError,_e.what());
      return NULL;
   }
}
%enddef

PYEXCEPTION(RunW)
PYEXCEPTION(RunB)
PYEXCEPTION(setExecMode)
PYEXCEPTION(resumeCurrentBreakPoint)
PYEXCEPTION(stopExecution)

/*
 * End of Exception section
 */



%wrapper %{
    /* Modification de la mecanique swig pour caster les objets dans les containers STL
     * Probablement trop dependant de swig (desactive)
  namespace swig {
    template <> struct traits_from<YACS::ENGINE::InPort *> {
      static PyObject *from(YACS::ENGINE::InPort* val) {
        //std::cerr << "typeid: " << typeid(*val).name() << std::endl;
        if(dynamic_cast<InputPort *>(val))
          return traits_from_ptr<YACS::ENGINE::InputPort>::from((YACS::ENGINE::InputPort*)val, 0);
        else if(dynamic_cast<InputDataStreamPort *>(val))
          return traits_from_ptr<YACS::ENGINE::InputDataStreamPort>::from((YACS::ENGINE::InputDataStreamPort*)val, 0);
        else
          return traits_from_ptr<YACS::ENGINE::InPort>::from(val, 0);
      }
    };
    template <> struct traits_from<YACS::ENGINE::OutPort *> {
      static PyObject *from(YACS::ENGINE::OutPort* val) {
        std::cerr << "from:YACS::ENGINE::OutPort*" << std::endl;
        std::cerr << "typeid: " << typeid(val).name() << std::endl;
        std::cerr << "typeid: " << typeid(*val).name() << std::endl;
        std::cerr << val->getNameOfTypeOfCurrentInstance() << std::endl;
        std::cerr << dynamic_cast<OutputDataStreamPort *>(val) << std::endl;
        if(dynamic_cast<OutputPort *>(val))
          return traits_from_ptr<YACS::ENGINE::OutputPort>::from((YACS::ENGINE::OutputPort*)val, 0);
        else if(dynamic_cast<OutputDataStreamPort *>(val))
          return traits_from_ptr<YACS::ENGINE::OutputDataStreamPort>::from((YACS::ENGINE::OutputDataStreamPort*)val, 0);
        else
          return traits_from_ptr<YACS::ENGINE::OutPort>::from(val, 0);
      }
    };
  }
  */
%}

/*
 * Ownership section
 */
//Take ownership : it is not the default (constructor) as it is a factory
%newobject YACS::ENGINE::Runtime::createProc;
%newobject YACS::ENGINE::Runtime::createScriptNode;
%newobject YACS::ENGINE::Runtime::createFuncNode;
%newobject YACS::ENGINE::Runtime::createRefNode;
%newobject YACS::ENGINE::Runtime::createCompoNode;
%newobject YACS::ENGINE::Runtime::createSInlineNode;
%newobject YACS::ENGINE::Runtime::createBloc;
%newobject YACS::ENGINE::Runtime::createForLoop;
%newobject YACS::ENGINE::Runtime::createForEachLoop;
%newobject YACS::ENGINE::Runtime::createWhileLoop;
%newobject YACS::ENGINE::Runtime::createSwitch;
%newobject YACS::ENGINE::Runtime::loadCatalog;
%newobject YACS::ENGINE::Node::clone;

//Release ownership : transfer it to C++
%apply SWIGTYPE *DISOWN { YACS::ENGINE::CatalogLoader* factory};
%apply SWIGTYPE *DISOWN { YACS::ENGINE::Node *DISOWNnode };
%apply SWIGTYPE *DISOWN { Node *DISOWNnode };

//Take ownership : transfer it from C++ (has to be completed)
%newobject YACS::ENGINE::Loop::edRemoveNode;
%newobject YACS::ENGINE::Switch::edReleaseDefaultNode;
%newobject YACS::ENGINE::Switch::edReleaseCase;
%newobject YACS::ENGINE::DynParaLoop::edRemoveNode;
%newobject YACS::ENGINE::DynParaLoop::edRemoveInitNode;
//No other way to do ??
%feature("pythonappend") YACS::ENGINE::Bloc::edRemoveChild(Node *node)%{
        args[1].thisown=1
%}
/*
 * End of ownership section
 */

/*
 * Reference counting section
 */
//not well understood
//%feature("ref")   YACS::ENGINE::RefCounter  "$this->incrRef();"
//%feature("unref") YACS::ENGINE::RefCounter  "$this->decrRef();"
/*
 * End of Reference counting section
 */

/*
 * Director section : classes that can be subclassed in python
 */
%feature("director") YACS::ENGINE::PyObserver;
%feature("nodirector") YACS::ENGINE::PyObserver::notifyObserver;
%feature("director") YACS::ENGINE::CatalogLoader;
/*
%feature("director:except") {
  if ($error != NULL) {
    // print the exception and clear it
    PyErr_Print();
    //do not propagate the python exception (ignore it)
    //throw Swig::DirectorMethodException();
  }
}
*/
/*
 * End of Director section
 */

%include <define.hxx>
%include <Exception.hxx>
%include <ConversionException.hxx>
%include <Runtime.hxx>
%include <Executor.hxx>
%include <ExecutorSwig.hxx>
%include <RefCounter.hxx>

%ignore YACS::ENGINE::TypeCode::getOrBuildAnyFromZippedData;
%include <TypeCode.hxx>

%include <Scheduler.hxx>
%include <Task.hxx>
%include <Dispatcher.hxx>
%include <DeploymentTree.hxx>
%include <Port.hxx>
%extend YACS::ENGINE::Port
{
  int __cmp__(Port* other)
    {
      if(self==other)
        return 0;
      else 
        return 1;
    }
  long ptr()
    {
      return (long)self;
    }
}
%include <DataPort.hxx>
%include <InPort.hxx>
%include <OutPort.hxx>
%include <InGate.hxx>
%include <OutGate.hxx>
%include <DataFlowPort.hxx>
%include <DataStreamPort.hxx>

%include <LinkInfo.hxx>
%include <Logger.hxx>

%include <ComponentInstance.hxx>
%include <Container.hxx>
%include <InputPort.hxx>
%extend YACS::ENGINE::InputPort
{
  void edInitXML(const char * s)
    {
      self->edInit("XML",s);
    }
  void edInitPy(PyObject* ob)
    {
      self->edInit("Python",ob);
    }
}
%template(edInitInt)       YACS::ENGINE::InputPort::edInit<int>;
%template(edInitBool)      YACS::ENGINE::InputPort::edInit<bool>;
%template(edInitString)    YACS::ENGINE::InputPort::edInit<std::string>;
%template(edInitDbl)       YACS::ENGINE::InputPort::edInit<double>;

%include <OutputPort.hxx>
%include <InputDataStreamPort.hxx>
%include <OutputDataStreamPort.hxx>
%include <AnyInputPort.hxx>

%include <Node.hxx>
%extend YACS::ENGINE::Node 
{
  int __cmp__(Node* other)
    {
      if(self==other)
        return 0;
      else 
        return 1;
    }
  long ptr()
    {
          return (long)self;
    }
}

%include <ElementaryNode.hxx>
%include <InlineNode.hxx>
%include <ServiceNode.hxx>
%include <ServiceInlineNode.hxx>

%include <ComposedNode.hxx>
%include <StaticDefinedComposedNode.hxx>
%include <Bloc.hxx>
%include <Proc.hxx>

%include <Loop.hxx>
%include <ForLoop.hxx>
%include <DynParaLoop.hxx>
%include <WhileLoop.hxx>
%include <ForEachLoop.hxx>
%include <OptimizerAlg.hxx>
%include <OptimizerLoop.hxx>
%include <Switch.hxx>
%include <Visitor.hxx>
%include <VisitorSaveSchema.hxx>
%include <ComponentDefinition.hxx>
%include <Catalog.hxx>

namespace YACS
{
  namespace ENGINE
  {
    class PyObserver:public Observer
    {
    public:
      virtual void pynotify(Node* object,const std::string& event);
    };
  }
}

