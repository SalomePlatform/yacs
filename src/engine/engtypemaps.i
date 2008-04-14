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
struct omniORBPYAPI
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

omniORBPYAPI* api;

#define OMNIPY_CATCH_AND_HANDLE_SYSTEM_EXCEPTIONS \
catch (const CORBA::SystemException& ex) { \
  return api->handleCxxSystemException(ex); \
}
#else
#define OMNIPY_CATCH_AND_HANDLE_SYSTEM_EXCEPTIONS 
#endif

#include "Node.hxx"
#include "InlineNode.hxx"
#include "ComposedNode.hxx"
#include "ServiceNode.hxx"
#include "ServiceInlineNode.hxx"
#include "Proc.hxx"
#include "Bloc.hxx"
#include "ForLoop.hxx"
#include "WhileLoop.hxx"
#include "ForEachLoop.hxx"
#include "Switch.hxx"
#include "InputPort.hxx"
#include "OutputPort.hxx"
#include "InputDataStreamPort.hxx"
#include "OutputDataStreamPort.hxx"
#include "OptimizerLoop.hxx"

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

static PyObject* convertNode(YACS::ENGINE::Node* node,int owner=0)
{
  if (!node)
    return SWIG_NewPointerObj((void*)node,SWIGTYPE_p_YACS__ENGINE__Node,owner);
  PyObject * ob;
  //should use $descriptor(YACS::ENGINE::Bloc *) and so on but $descriptor is not defined here
  // It is better to define a helper function to avoid code bloat
  // First try to find a swig type info by its mangled name
  std::string swigtypename="_p_"+node->typeName();
  swig_type_info *ret = SWIG_MangledTypeQuery(swigtypename.c_str());
  if (ret) 
    ob=SWIG_NewPointerObj((void*)node,ret,owner);
  else
    {
      //typeName not known by swig. Try dynamic_cast on known classes
      //You must respect inheritance order in casting : Bloc before ComposedNode and so on
      if(dynamic_cast<YACS::ENGINE::Proc *>(node))
        ob=SWIG_NewPointerObj((void*)node,SWIGTYPE_p_YACS__ENGINE__Proc,owner);
      else if(dynamic_cast<YACS::ENGINE::Bloc *>(node))
        ob=SWIG_NewPointerObj((void*)node,SWIGTYPE_p_YACS__ENGINE__Bloc,owner);
      else if(dynamic_cast<YACS::ENGINE::ForLoop *>(node))
        ob=SWIG_NewPointerObj((void*)node,SWIGTYPE_p_YACS__ENGINE__ForLoop,owner);
      else if(dynamic_cast<YACS::ENGINE::WhileLoop *>(node))
        ob=SWIG_NewPointerObj((void*)node,SWIGTYPE_p_YACS__ENGINE__WhileLoop,owner);
      else if(dynamic_cast<YACS::ENGINE::ForEachLoop *>(node))
        ob=SWIG_NewPointerObj((void*)node,SWIGTYPE_p_YACS__ENGINE__ForEachLoop,owner);
      else if(dynamic_cast<YACS::ENGINE::Switch *>(node))
        ob=SWIG_NewPointerObj((void*)node,SWIGTYPE_p_YACS__ENGINE__Switch,owner);
      else if(dynamic_cast<YACS::ENGINE::ComposedNode *>(node))
        ob=SWIG_NewPointerObj((void*)node,SWIGTYPE_p_YACS__ENGINE__ComposedNode,owner);
      else if(dynamic_cast<YACS::ENGINE::InlineFuncNode *>(node))
        ob=SWIG_NewPointerObj((void*)node,SWIGTYPE_p_YACS__ENGINE__InlineFuncNode,owner);
      else if(dynamic_cast<YACS::ENGINE::InlineNode *>(node))
        ob=SWIG_NewPointerObj((void*)node,SWIGTYPE_p_YACS__ENGINE__InlineNode,owner);
      else if(dynamic_cast<YACS::ENGINE::ServiceInlineNode *>(node))
        ob=SWIG_NewPointerObj((void*)node,SWIGTYPE_p_YACS__ENGINE__ServiceInlineNode,owner);
      else if(dynamic_cast<YACS::ENGINE::ServiceNode *>(node))
        ob=SWIG_NewPointerObj((void*)node,SWIGTYPE_p_YACS__ENGINE__ServiceNode,owner);
      else if(dynamic_cast<YACS::ENGINE::ElementaryNode *>(node))
        ob=SWIG_NewPointerObj((void*)node,SWIGTYPE_p_YACS__ENGINE__ElementaryNode,owner);
      else
        ob=SWIG_NewPointerObj((void*)node,SWIGTYPE_p_YACS__ENGINE__Node,owner);
    }
  return ob;
}

static PyObject* convertPort(YACS::ENGINE::Port* port,int owner=0)
{
  if(!port)
    return SWIG_NewPointerObj((void*)port,SWIGTYPE_p_YACS__ENGINE__Port, owner);
  PyObject * ob;
  std::string swigtypename="_p_"+port->typeName();
  swig_type_info *ret = SWIG_MangledTypeQuery(swigtypename.c_str());
  if (ret) 
    {
      YACS::ENGINE::InputPort *inport = dynamic_cast<YACS::ENGINE::InputPort *>(port);
      if(inport)
        return SWIG_NewPointerObj((void*)inport,ret,owner);

      YACS::ENGINE::OutputPort *outport = dynamic_cast<YACS::ENGINE::OutputPort *>(port);
      if(outport)
        return SWIG_NewPointerObj((void*)outport,ret,owner);

      YACS::ENGINE::InputDataStreamPort *indsport = dynamic_cast<YACS::ENGINE::InputDataStreamPort *>(port);
      if(indsport)
        return SWIG_NewPointerObj((void*)indsport,ret,owner);

      YACS::ENGINE::OutputDataStreamPort *outdsport = dynamic_cast<YACS::ENGINE::OutputDataStreamPort *>(port);
      if(outdsport)
        return SWIG_NewPointerObj((void*)outdsport,ret,owner);

      return SWIG_NewPointerObj((void*)port,ret,owner);
    }
  else
    {
      if(dynamic_cast<YACS::ENGINE::InputPort *>(port))
        ob=SWIG_NewPointerObj((void*)port,SWIGTYPE_p_YACS__ENGINE__InputPort,owner);
      else if(dynamic_cast<YACS::ENGINE::OutputPort *>(port))
        ob=SWIG_NewPointerObj((void*)port,SWIGTYPE_p_YACS__ENGINE__OutputPort,owner);
      else if(dynamic_cast<YACS::ENGINE::InputDataStreamPort *>(port))
        ob=SWIG_NewPointerObj((void*)port,SWIGTYPE_p_YACS__ENGINE__InputDataStreamPort, owner);
      else if(dynamic_cast<YACS::ENGINE::OutputDataStreamPort *>(port))
        ob=SWIG_NewPointerObj((void*)port,SWIGTYPE_p_YACS__ENGINE__OutputDataStreamPort, owner);
      else if(dynamic_cast<YACS::ENGINE::InPort *>(port))
        ob=SWIG_NewPointerObj((void*)port,SWIGTYPE_p_YACS__ENGINE__InPort, owner);
      else if(dynamic_cast<YACS::ENGINE::OutPort *>(port))
        ob=SWIG_NewPointerObj((void*)port,SWIGTYPE_p_YACS__ENGINE__OutPort, owner);
      else if(dynamic_cast<YACS::ENGINE::InGate *>(port))
        ob=SWIG_NewPointerObj((void*)port,SWIGTYPE_p_YACS__ENGINE__InGate, owner);
      else if(dynamic_cast<YACS::ENGINE::OutGate *>(port))
        ob=SWIG_NewPointerObj((void*)port,SWIGTYPE_p_YACS__ENGINE__OutGate, owner);
      else
        ob=SWIG_NewPointerObj((void*)port,SWIGTYPE_p_YACS__ENGINE__Port, owner);
    }
  return ob;
}

%}

#if SWIG_VERSION >= 0x010329
%template()        std::list<int>;
%template()        std::list<std::string>;
#else

#ifdef SWIGPYTHON
%typemap(out) std::list<int>
{
  int i;
  std::list<int>::iterator iL;

  $result = PyList_New($1.size());
  for (i=0, iL=$1.begin(); iL!=$1.end(); i++, iL++)
    PyList_SetItem($result,i,PyLong_FromLong((*iL))); 
}

%typemap(out) std::list<std::string>
{
  int i;
  std::list<std::string>::iterator iL;

  $result = PyList_New($1.size());
  for (i=0, iL=$1.begin(); iL!=$1.end(); i++, iL++)
    PyList_SetItem($result,i,PyString_FromString((*iL).c_str())); 
}

%typemap(in) std::list<std::string>
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
#endif

#ifdef SWIGPYTHON
%typemap(in) std::list<YACS::ENGINE::TypeCodeObjref*>
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
              if(temp->kind() == YACS::ENGINE::Objref)
                $1.push_back((YACS::ENGINE::TypeCodeObjref*)temp);
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

%typemap(out) YACS::ENGINE::Node*
{
  $result=convertNode($1,$owner);
}

%typemap(out) YACS::ENGINE::ServiceNode*
{
  $result=convertNode($1,$owner);
}

%typemap(out) YACS::ENGINE::InlineNode*
{
  $result=convertNode($1,$owner);
}

%typemap(out) YACS::ENGINE::ComposedNode*
{
  $result=convertNode($1,$owner);
}

%typemap(out) YACS::ENGINE::Proc*
{
  $result=convertNode($1,$owner);
}

%typemap(out) std::set<YACS::ENGINE::Node *>
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

%typemap(out) std::list<YACS::ENGINE::Node *>
{
  int i;
  std::list<YACS::ENGINE::Node *>::iterator iL;

  $result = PyList_New($1.size());
  PyObject * ob;
  for (i=0, iL=$1.begin(); iL!=$1.end(); i++, iL++)
    {
      ob=convertNode(*iL);
      PyList_SetItem($result,i,ob); 
    }
}

%typemap(out) std::set<YACS::ENGINE::InGate *>
{
  int i;
  std::set<YACS::ENGINE::InGate *>::iterator iL;
  $result = PyList_New($1.size());
  PyObject * ob;
  for (i=0, iL=$1.begin(); iL!=$1.end(); i++, iL++)
    {
      ob=convertPort(*iL);
      PyList_SetItem($result,i,ob); 
    }
}

%typemap(out) std::set<YACS::ENGINE::OutGate *>
{
  int i;
  std::set<YACS::ENGINE::OutGate *>::iterator iL;
  $result = PyList_New($1.size());
  PyObject * ob;
  for (i=0, iL=$1.begin(); iL!=$1.end(); i++, iL++)
    {
      ob=convertPort(*iL);
      PyList_SetItem($result,i,ob); 
    }
}

%typemap(out) std::set<YACS::ENGINE::InPort *>
{
  std::set<YACS::ENGINE::InPort *>::iterator iL;
  $result = PyList_New(0);
  PyObject * ob;
  int status;
  for (iL=$1.begin(); iL!=$1.end(); iL++)
    {
      ob=convertPort(*iL);
      status=PyList_Append($result,ob);
      Py_DECREF(ob);
      if (status < 0)
        {
          PyErr_SetString(PyExc_TypeError,"cannot build the inport list");
          return NULL;
        }
    }
}

%typemap(out) std::set<YACS::ENGINE::OutPort *>
{
  std::set<YACS::ENGINE::OutPort *>::iterator iL;
  $result = PyList_New(0);
  PyObject * ob;
  int status;
  for (iL=$1.begin(); iL!=$1.end(); iL++)
    {
      ob=convertPort(*iL);
      status=PyList_Append($result,ob);
      Py_DECREF(ob);
      if (status < 0)
        {
          PyErr_SetString(PyExc_TypeError,"cannot build the outport list");
          return NULL;
        }
    }
}

%typemap(out) std::list<YACS::ENGINE::OutPort *>
{
  std::list<YACS::ENGINE::OutPort *>::const_iterator it;
  $result = PyTuple_New($1.size());
  int i = 0;
  for (it = $1.begin(); it != $1.end(); ++it, ++i) {
    PyTuple_SetItem($result,i,convertPort(*it));
  }
}
%typemap(out) std::list<YACS::ENGINE::InPort *>
{
  std::list<YACS::ENGINE::InPort *>::const_iterator it;
  $result = PyTuple_New($1.size());
  int i = 0;
  for (it = $1.begin(); it != $1.end(); ++it, ++i) {
    PyTuple_SetItem($result,i,convertPort(*it));
  }
}
%typemap(out) std::list<YACS::ENGINE::OutputPort *>
{
  std::list<YACS::ENGINE::OutputPort *>::const_iterator it;
  $result = PyTuple_New($1.size());
  int i = 0;
  for (it = $1.begin(); it != $1.end(); ++it, ++i) {
    PyTuple_SetItem($result,i,convertPort(*it));
  }
}
%typemap(out) std::list<YACS::ENGINE::InputPort *>
{
  std::list<YACS::ENGINE::InputPort *>::const_iterator it;
  $result = PyTuple_New($1.size());
  int i = 0;
  for (it = $1.begin(); it != $1.end(); ++it, ++i) {
    PyTuple_SetItem($result,i,convertPort(*it));
  }
}

#endif

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
   catch(std::invalid_argument& _e) 
   {
      PyErr_SetString(PyExc_IOError ,_e.what());
      return NULL;
   }
   OMNIPY_CATCH_AND_HANDLE_SYSTEM_EXCEPTIONS
   catch(...) 
   {
     SWIG_exception(SWIG_UnknownError, "Unknown exception");
   }
}

// a specific exception handler = generic + release lock
%define PYEXCEPTION(name)
%exception name {
   try 
   {
      InterpreterUnlocker _l;
      $action
   } 
   catch(YACS::Exception& _e) 
   {
      PyErr_SetString(PyExc_ValueError,_e.what());
      return NULL;
   }
   catch(std::invalid_argument& _e) 
   {
      PyErr_SetString(PyExc_IOError ,_e.what());
      return NULL;
   }
   OMNIPY_CATCH_AND_HANDLE_SYSTEM_EXCEPTIONS
   catch(...) 
   {
     SWIG_exception(SWIG_UnknownError, "Unknown exception");
   }
}
%enddef

%define EXCEPTION(name)
%exception name {
   try 
   {
      $action
   } 
   catch(YACS::Exception& _e) 
   {
      PyErr_SetString(PyExc_ValueError,_e.what());
      return NULL;
   }
   catch(std::invalid_argument& _e) 
   {
      PyErr_SetString(PyExc_IOError ,_e.what());
      return NULL;
   }
   OMNIPY_CATCH_AND_HANDLE_SYSTEM_EXCEPTIONS
   catch(...) 
   {
     SWIG_exception(SWIG_UnknownError, "Unknown exception");
   }
}
%enddef
/*
 * End of Exception section
 */

/*
 * Ownership section
 */
//Release ownership : transfer it to C++
%apply SWIGTYPE *DISOWN { YACS::ENGINE::CatalogLoader* factory};
%apply SWIGTYPE *DISOWN { YACS::ENGINE::Node *DISOWNnode };
%apply SWIGTYPE *DISOWN { Node *DISOWNnode };
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
%wrapper %{
  namespace swig {
    template <> struct traits_from<YACS::ENGINE::InPort *> {
      static PyObject *from(YACS::ENGINE::InPort* val){
        return convertPort(val);
      }
    };
    template <> struct traits_from<YACS::ENGINE::OutPort *> {
      static PyObject *from(YACS::ENGINE::OutPort* val) {
        return convertPort(val);
      }
    };
    template <> struct traits_from<YACS::ENGINE::InputPort *> {
      static PyObject *from(YACS::ENGINE::InPort* val){
        return convertPort(val);
      }
    };
    template <> struct traits_from<YACS::ENGINE::OutputPort *> {
      static PyObject *from(YACS::ENGINE::OutPort* val) {
        return convertPort(val);
      }
    };
  }
%}
*/

