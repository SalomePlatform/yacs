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
#include "ServerNode.hxx"
#include "Proc.hxx"
#include "Bloc.hxx"
#include "ForLoop.hxx"
#include "WhileLoop.hxx"
#include "ForEachLoop.hxx"
#include "Switch.hxx"
#include "InputPort.hxx"
#include "OutputPort.hxx"
#include "InPropertyPort.hxx"
#include "InputDataStreamPort.hxx"
#include "OutputDataStreamPort.hxx"
#include "OptimizerLoop.hxx"
#include "HomogeneousPoolContainer.hxx"

#include <sstream>

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
      else if(dynamic_cast<YACS::ENGINE::ServerNode *>(node))
        ob=SWIG_NewPointerObj((void*)node,SWIGTYPE_p_YACS__ENGINE__ServerNode,owner);
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
      YACS::ENGINE::InPropertyPort *inpropertyport = dynamic_cast<YACS::ENGINE::InPropertyPort*>(port);
      if(inpropertyport)
        return SWIG_NewPointerObj((void*)inpropertyport,ret,owner);

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
      if(YACS::ENGINE::AnyInputPort *cport =dynamic_cast<YACS::ENGINE::AnyInputPort *>(port))
        ob=SWIG_NewPointerObj((void*)cport,SWIGTYPE_p_YACS__ENGINE__AnyInputPort,owner);
      else if(YACS::ENGINE::AnyOutputPort *cport =dynamic_cast<YACS::ENGINE::AnyOutputPort *>(port))
        ob=SWIG_NewPointerObj((void*)cport,SWIGTYPE_p_YACS__ENGINE__AnyOutputPort,owner);
      else if(dynamic_cast<YACS::ENGINE::InPropertyPort*>(port))
        ob=SWIG_NewPointerObj((void*)port,SWIGTYPE_p_YACS__ENGINE__InPropertyPort,owner);
      else if(dynamic_cast<YACS::ENGINE::InputPort *>(port))
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

static PyObject *convertContainer(YACS::ENGINE::Container *cont, int owner=0)
{
  if(!cont)
    return SWIG_NewPointerObj((void*)cont,SWIGTYPE_p_YACS__ENGINE__Container, owner);
  if(dynamic_cast<YACS::ENGINE::HomogeneousPoolContainer *>(cont))
    {
      return SWIG_NewPointerObj((void*)dynamic_cast<YACS::ENGINE::HomogeneousPoolContainer *>(cont),SWIGTYPE_p_YACS__ENGINE__HomogeneousPoolContainer, owner);
    }
  return SWIG_NewPointerObj((void*)cont,SWIGTYPE_p_YACS__ENGINE__Container, owner);
}

//convertFromPyObjVectorOfObj<YACS::ENGINE::SequenceAny *>(pyLi,SWIGTYPE_p_YACS__ENGINE__SequenceAny,"SequenceAny")
template<class T>
static void convertFromPyObjVectorOfObj(PyObject *pyLi, swig_type_info *ty, const char *typeStr, typename std::vector<T>& ret)
{
  void *argp=0;
  if(PyList_Check(pyLi))
    {
      int size=PyList_Size(pyLi);
      ret.resize(size);
      for(int i=0;i<size;i++)
        {
          PyObject *obj=PyList_GetItem(pyLi,i);
          int status=SWIG_ConvertPtr(obj,&argp,ty,0|0);
          if(!SWIG_IsOK(status))
            {
              std::ostringstream oss; oss << "convertFromPyObjVectorOfObj : list is excepted to contain only " << typeStr << " instances !";
              throw YACS::Exception(oss.str());
            }
          T arg=reinterpret_cast< T >(argp);
          ret[i]=arg;
        }
    }
  else if(PyTuple_Check(pyLi))
    {
      int size=PyTuple_Size(pyLi);
      ret.resize(size);
      for(int i=0;i<size;i++)
        {
          PyObject *obj=PyTuple_GetItem(pyLi,i);
          int status=SWIG_ConvertPtr(obj,&argp,ty,0|0);
          if(!SWIG_IsOK(status))
            {
              std::ostringstream oss; oss << "convertFromPyObjVectorOfObj : tuple is excepted to contain only " << typeStr << " instances !";
              throw YACS::Exception(oss.str());
            }
          T arg=reinterpret_cast< T >(argp);
          ret[i]=arg;
        }
    }
  else if(SWIG_IsOK(SWIG_ConvertPtr(pyLi,&argp,ty,0|0)))
    {
      ret.resize(1);
      T arg=reinterpret_cast< T >(argp);
      ret[0]=arg;
    }
  else
    throw YACS::Exception("convertFromPyObjVectorOfObj : not a list nor a tuple");
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

%typecheck(SWIG_TYPECHECK_POINTER) YACS::ENGINE::Any*
{
  void *ptr;
  if (SWIG_ConvertPtr($input, (void **) &ptr, $1_descriptor, 0) == 0) 
    $1 = 1;
  else if (PyInt_Check($input))
    $1 = 1;
  else if(PyFloat_Check($input))
    $1 = 1;
  else if (PyString_Check($input))
    $1 = 1;
  else 
    $1 = 0;
}

%typemap(in) YACS::ENGINE::Any* (int is_new_object)
{
  if ((SWIG_ConvertPtr($input,(void **) &$1, $1_descriptor,SWIG_POINTER_EXCEPTION)) == 0)
    {
      // It is an Any : it is converted by SWIG_ConvertPtr $input -> $1
      is_new_object=0;
    }
  else if (PyInt_Check($input))
    {
      // It is an Int
      $1=YACS::ENGINE::AtomAny::New((int)PyInt_AsLong($input));
      is_new_object=1;
    }
  else if(PyFloat_Check($input))
    {
      // It is a Float
      $1=YACS::ENGINE::AtomAny::New(PyFloat_AsDouble($input));
      is_new_object=1;
    }
  else if(PyString_Check($input))
    {
      // It is a Float
      $1=YACS::ENGINE::AtomAny::New(PyString_AsString($input));
      is_new_object=1;
    }
  else
    {
      // It is an error
      PyErr_SetString(PyExc_TypeError,"not a yacs any or a convertible type");
      return NULL;
    }
}

%typemap(directorout) YACS::ENGINE::Any*
{
  if ((SWIG_ConvertPtr($1,(void **) &$result, $1_descriptor,SWIG_POINTER_EXCEPTION)) == 0)
    {
      // It is an Any : it is converted by SWIG_ConvertPtr $input -> $1
    }
  else if (PyInt_Check($1))
    {
      // It is an Int
      $result=YACS::ENGINE::AtomAny::New((int)PyInt_AsLong($1));
    }
  else if(PyFloat_Check($1))
    {
      // It is a Float
      $result=YACS::ENGINE::AtomAny::New(PyFloat_AsDouble($1));
    }
  else if(PyString_Check($1))
    {
      // It is a String
      $result=YACS::ENGINE::AtomAny::New(PyString_AsString($1));
    }
  else
    {
      // It is an error
      PyErr_SetString(PyExc_TypeError,"not a yacs any or a convertible type");
      return NULL;
    }
}

%typemap(freearg) YACS::ENGINE::Any *inSample
{
  //a reference is taken by the routine called
  if (!is_new_object$argnum) $1->incrRef();
}

%typemap(freearg) YACS::ENGINE::Any*
{
  //no reference taken by the routine called
  if (is_new_object$argnum) $1->decrRef();
}

%typemap(out) YACS::ENGINE::Any*
{
  if(dynamic_cast<YACS::ENGINE::SequenceAny *>($1))
    $result=SWIG_NewPointerObj((void*)$1,SWIGTYPE_p_YACS__ENGINE__SequenceAny,$owner);
  else if(dynamic_cast<YACS::ENGINE::ArrayAny *>($1))
    $result=SWIG_NewPointerObj((void*)$1,SWIGTYPE_p_YACS__ENGINE__ArrayAny,$owner);
  else if(dynamic_cast<YACS::ENGINE::StructAny *>($1))
    $result=SWIG_NewPointerObj((void*)$1,SWIGTYPE_p_YACS__ENGINE__StructAny,$owner);
  else
    $result=SWIG_NewPointerObj((void*)$1,SWIGTYPE_p_YACS__ENGINE__Any,$owner);
}

%typemap(out) YACS::ENGINE::TypeCode*
{
  if(dynamic_cast<YACS::ENGINE::TypeCodeStruct *>($1))
    $result=SWIG_NewPointerObj((void*)$1,SWIGTYPE_p_YACS__ENGINE__TypeCodeStruct,$owner);
  else if(dynamic_cast<YACS::ENGINE::TypeCodeSeq *>($1))
    $result=SWIG_NewPointerObj((void*)$1,SWIGTYPE_p_YACS__ENGINE__TypeCodeSeq,$owner);
  else if(dynamic_cast<YACS::ENGINE::TypeCodeObjref *>($1))
    $result=SWIG_NewPointerObj((void*)$1,SWIGTYPE_p_YACS__ENGINE__TypeCodeObjref,$owner);
  else
    $result=SWIG_NewPointerObj((void*)$1,SWIGTYPE_p_YACS__ENGINE__TypeCode,$owner);
}

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

%typemap(out) YACS::ENGINE::InlineFuncNode*
{
  $result=convertNode($1,$owner);
}

%typemap(out) YACS::ENGINE::ComposedNode*
{
  $result=convertNode($1,$owner);
}

%typemap(out) YACS::ENGINE::OptimizerLoop*
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

%typemap(out) YACS::ENGINE::InputPort*,YACS::ENGINE::OutputPort*,YACS::ENGINE::InPort*,YACS::ENGINE::OutPort*,YACS::ENGINE::InPropertyPort*
{
  $result=convertPort($1,$owner);
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
%typemap(out) std::list<YACS::ENGINE::InPropertyPort*>
{
  std::list<YACS::ENGINE::InPropertyPort *>::const_iterator it;
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
   } catch (std::domain_error& e) {
      SWIG_exception(SWIG_ValueError, e.what() );
   } catch (std::overflow_error& e) {
      SWIG_exception(SWIG_OverflowError, e.what() );
   } catch (std::out_of_range& e) {
      PyErr_SetString(PyExc_KeyError,e.what());
      return NULL;
   } catch (std::length_error& e) {
      SWIG_exception(SWIG_IndexError, e.what() );
   } catch (std::runtime_error& e) {
      SWIG_exception(SWIG_RuntimeError, e.what() );
   }
   OMNIPY_CATCH_AND_HANDLE_SYSTEM_EXCEPTIONS
   catch (std::exception& e) {
      SWIG_exception(SWIG_SystemError, e.what() );
   }
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
   } catch (std::domain_error& e) {
      SWIG_exception(SWIG_ValueError, e.what() );
   } catch (std::overflow_error& e) {
      SWIG_exception(SWIG_OverflowError, e.what() );
   } catch (std::out_of_range& e) {
      PyErr_SetString(PyExc_KeyError,e.what());
      return NULL;
   } catch (std::length_error& e) {
      SWIG_exception(SWIG_IndexError, e.what() );
   } catch (std::runtime_error& e) {
      SWIG_exception(SWIG_RuntimeError, e.what() );
   }
   OMNIPY_CATCH_AND_HANDLE_SYSTEM_EXCEPTIONS
   catch (std::exception& e) {
      SWIG_exception(SWIG_SystemError, e.what() );
   }
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
   } catch (std::domain_error& e) {
      SWIG_exception(SWIG_ValueError, e.what() );
   } catch (std::overflow_error& e) {
      SWIG_exception(SWIG_OverflowError, e.what() );
   } catch (std::out_of_range& e) {
      PyErr_SetString(PyExc_KeyError,e.what());
      return NULL;
   } catch (std::length_error& e) {
      SWIG_exception(SWIG_IndexError, e.what() );
   } catch (std::runtime_error& e) {
      SWIG_exception(SWIG_RuntimeError, e.what() );
   }
   OMNIPY_CATCH_AND_HANDLE_SYSTEM_EXCEPTIONS
   catch (std::exception& e) {
      SWIG_exception(SWIG_SystemError, e.what() );
   }
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
 * reference counted objects are created with a count of 1 so we do not incrRef them on wrapping creation
 * we only decrRef them on wrapping destruction.
 * Do not forget to declare them new (%newobject) when they are not returned from a constructor
 * unless they will not be decrRef on wrapping destruction
 */
%feature("ref")   YACS::ENGINE::RefCounter  ""
%feature("unref") YACS::ENGINE::RefCounter  "$this->decrRef();"

// Unfortunately, class ComponentInstance inherits from RefCounter AND PropertyInterface. Thus the ref and
// unref features are ambiguous and with swig 2.0.7 at least, we must re-specify those features for class
// ComponentInstance unless the instances are destroyed when the Swig object is unref'ed.
%feature("ref")   YACS::ENGINE::ComponentInstance  ""
%feature("unref") YACS::ENGINE::ComponentInstance  "$this->decrRef();"
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

%define REFCOUNT_TEMPLATE(tname, T...)
/*
 This macro is a special wrapping for map with value type which derives from RefCounter.
 To overload standard SWIG wrapping we define a full specialization of std::map
 with %extend for 4 basic methods : getitem, setitem, delitem and keys.
 Then we complete the interface by deriving the shadow wrapper from
 the python mixin class (UserDict.DictMixin).
 Do not forget to declare the new shadow class to SWIG with tname_swigregister(tname).
 Objects returned by __getitem__ are declared new (%newobject) so that when destroyed they
 call decrRef (see feature("unref") for RefCounter).
*/
template<>
class std::map<std::string,T*>
{
public:
%extend
{
  void __setitem__(const std::string& name, T* c)
    {
      std::map<std::string, T* >::iterator i = self->find(name);
      if (i != self->end())
        {
          if(c==i->second)
            return;
          i->second->decrRef();
        }
      (*self)[name]=c;
      c->incrRef();
    }
  T* __getitem__(std::string name)
    {
      std::map<std::string, T* >::iterator i = self->find(name);
      if (i != self->end())
        {
          i->second->incrRef();
          return i->second;
        }
      else
        throw std::out_of_range("key not found");
    }
  void __delitem__(std::string name)
    {
      std::map<std::string, T* >::iterator i = self->find(name);
      if (i != self->end()){
        i->second->decrRef();
        self->erase(i);
      }
      else
        throw std::out_of_range("key not found");
    }
  PyObject* keys() {
      int pysize = self->size();
      PyObject* keyList = PyList_New(pysize);
      std::map<std::string, T* >::const_iterator i = self->begin();
      for (int j = 0; j < pysize; ++i, ++j) {
        PyList_SET_ITEM(keyList, j, PyString_FromString(i->first.c_str()));
      }
      return keyList;
    }
}
};

%newobject std::map<std::string,T* >::__getitem__;
%template()   std::pair<std::string, T* >;
%template(tname)    std::map<std::string, T* >;
%pythoncode{
from UserDict import DictMixin
class tname(tname,DictMixin):pass
tname##_swigregister(tname)
}
%enddef


