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
#include "Runtime.hxx"
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

%}

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

%typemap(python,out) std::set<YACS::ENGINE::Node *>
{
  int i;
  std::set<YACS::ENGINE::Node *>::iterator iL;

  $result = PyList_New($1.size());
  PyObject * ob;
  for (i=0, iL=$1.begin(); iL!=$1.end(); i++, iL++)
    {
      if(dynamic_cast<Bloc *>(*iL))
        ob=SWIG_NewPointerObj((void*)(*iL),$descriptor(YACS::ENGINE::Bloc *),0); 
      else if(dynamic_cast<ForLoop *>(*iL))
        ob=SWIG_NewPointerObj((void*)(*iL),$descriptor(YACS::ENGINE::ForLoop *),0); 
      else if(dynamic_cast<WhileLoop *>(*iL))
        ob=SWIG_NewPointerObj((void*)(*iL),$descriptor(YACS::ENGINE::WhileLoop *), 0); 
      else if(dynamic_cast<ForEachLoop *>(*iL))
        ob=SWIG_NewPointerObj((void*)(*iL),$descriptor(YACS::ENGINE::ForEachLoop *), 0); 
      else if(dynamic_cast<Switch *>(*iL))
        ob=SWIG_NewPointerObj((void*)(*iL),$descriptor(YACS::ENGINE::Switch *), 0); 
      else if(dynamic_cast<ComposedNode *>(*iL))
        ob=SWIG_NewPointerObj((void*)(*iL),$descriptor(YACS::ENGINE::ComposedNode *), 0); 
      else if(dynamic_cast<InlineFuncNode *>(*iL))
        ob=SWIG_NewPointerObj((void*)(*iL),$descriptor(YACS::ENGINE::InlineFuncNode *), 0);
      else if(dynamic_cast<InlineNode *>(*iL))
        ob=SWIG_NewPointerObj((void*)(*iL),$descriptor(YACS::ENGINE::InlineNode *), 0);
      else if(dynamic_cast<ServiceNode *>(*iL))
        ob=SWIG_NewPointerObj((void*)(*iL),$descriptor(YACS::ENGINE::ServiceNode *), 0); 
      else if(dynamic_cast<ServiceInlineNode *>(*iL))
        ob=SWIG_NewPointerObj((void*)(*iL),$descriptor(YACS::ENGINE::ServiceInlineNode *), 0); 
      else if(dynamic_cast<ElementaryNode *>(*iL))
        ob=SWIG_NewPointerObj((void*)(*iL),$descriptor(YACS::ENGINE::ElementaryNode *), 0); 
      else
        ob=SWIG_NewPointerObj((void*)(*iL),$descriptor(YACS::ENGINE::Node *), 0); 

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
   catch (Swig::DirectorException &e) 
   { 
     SWIG_fail; 
   }
}

// a specific exception handler = generic + release lock
%exception RunW {
   try {
      InterpreterUnlocker _l;
      $action
   } catch(YACS::Exception& _e) {
      PyErr_SetString(PyExc_ValueError,_e.what());
      return NULL;
   }
}
%exception RunB {
   try {
      InterpreterUnlocker _l;
      $action
   } catch(YACS::Exception& _e) {
      PyErr_SetString(PyExc_ValueError,_e.what());
      return NULL;
   }
}

%exception setExecMode {
   try {
      InterpreterUnlocker _l;
      $action
   } catch(YACS::Exception& _e) {
      PyErr_SetString(PyExc_ValueError,_e.what());
      return NULL;
   }
}

%exception resumeCurrentBreakPoint {
   try {
      InterpreterUnlocker _l;
      $action
   } catch(YACS::Exception& _e) {
      PyErr_SetString(PyExc_ValueError,_e.what());
      return NULL;
   }
}

%exception stopExecution {
   try {
      InterpreterUnlocker _l;
      $action
   } catch(YACS::Exception& _e) {
      PyErr_SetString(PyExc_ValueError,_e.what());
      return NULL;
   }
}

/*
 * End of Exception section
 */

/*
 * Template section
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


%template()              std::pair<std::string, YACS::ENGINE::TypeCode *>;
%template()              std::pair<std::string, YACS::ENGINE::Node *>;
%template()              std::pair<std::string, YACS::ENGINE::InlineNode *>;
%template()              std::pair<std::string, YACS::ENGINE::ServiceNode *>;
%template()              std::pair<YACS::ENGINE::OutPort *,YACS::ENGINE::InPort *>;
%template()              std::pair<YACS::ENGINE::InPort *,YACS::ENGINE::OutPort *>;
%template(TypeList)      std::list<YACS::ENGINE::TypeCodeObjref *>;
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

//Pb : cannot iterate with SWIG 1.3.24
//%template(NODEset)      std::set<YACS::ENGINE::Node *>;
//%template(InPortset)    std::set<YACS::ENGINE::InPort *>;
//%template(OutPortset)   std::set<YACS::ENGINE::OutPort *>;

/*
 * End of Template section
 */

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

//Release ownership : transfer it to C++
%apply SWIGTYPE *DISOWN { YACS::ENGINE::Node *DISOWNnode };

//Take ownership : transfer it from C++ (to complete)
%newobject YACS::ENGINE::Loop::edRemoveNode;
%newobject YACS::ENGINE::Switch::edReleaseDefaultNode;
%newobject YACS::ENGINE::Switch::edReleaseCase;
%newobject YACS::ENGINE::DynParaLoop::edRemoveNode;
%newobject YACS::ENGINE::DynParaLoop::edRemoveInitNode;
//No other way to do
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

%feature("director") YACS::ENGINE::PyObserver;
%feature("nodirector") YACS::ENGINE::PyObserver::notifyObserver;
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

%include <define.hxx>
%include <Exception.hxx>
%include <ConversionException.hxx>
%include <Runtime.hxx>
%include <Executor.hxx>
%include <ExecutorSwig.hxx>
%include <RefCounter.hxx>

%import <Scheduler.hxx>
%import <Task.hxx>


namespace YACS
{
  namespace ENGINE
  {
    typedef enum
      {
      //Problem with None (same name as python)
       // None     = 0,
        Double   = 1,
        Int      = 2,
        String   = 3,
        Bool     = 4,
        Objref   = 5,
        Sequence = 6,
        Array    = 7
      } DynType;

    class TypeCode : public RefCounter
    {
    public:
      TypeCode(DynType kind);
      DynType kind() const;
      virtual TypeCode *clone() const;
      virtual const char * name()       ;
      virtual const char * shortName()  ;
      virtual const char * id()         ;
      virtual const TypeCode * contentType() ;
      virtual int isA(const char* repositoryId) ;
      virtual int isA(const TypeCode* tc) const;
      virtual int isAdaptable(const TypeCode* tc) const;
    protected:
      TypeCode(const TypeCode& tc);
      virtual ~TypeCode();
    };

    class TypeCodeObjref: public TypeCode
    {
    public:
      TypeCodeObjref(const char* repositoryId, const char* name);
      TypeCodeObjref(const char* repositoryId, const char* name, 
                      const std::list<TypeCodeObjref *>& ltc);
    protected:
      virtual ~TypeCodeObjref();
    };

    class DeploymentTree
    {
    public:
      DeploymentTree();
      ~DeploymentTree();
      unsigned char appendTask(Task *task, Scheduler *cloner);
      //
      unsigned getNumberOfCTDefContainer() const;
      unsigned getNumberOfRTODefContainer() const;
      unsigned getNumberOfCTDefComponentInstances() const;
      unsigned getNumberOfRTODefComponentInstances() const;
      //
      bool presenceOfDefaultContainer() const;
    };

    class Node;

    class Observer
    {
    public:
      virtual void notifyObserver(Node* object,const std::string& event);
      virtual ~Observer();
    };

    class PyObserver:public Observer
    {
    public:
      //virtual void notifyObserver(Node* object,const std::string& event);
      virtual void pynotify(Node* object,const std::string& event);
    };

    class Dispatcher
    {
    public:
      static Dispatcher* getDispatcher();
      virtual void dispatch(Node* object,const std::string& event);
      virtual void addObserver(Observer* observer,Node* object,const std::string& event);
      virtual void removeObserver(Observer* observer,Node* object,const std::string& event);
      virtual void printObservers();
    };

    class Port
    {
    public:
      virtual ~Port();
      Node *getNode() const ;
      virtual std::string getNameOfTypeOfCurrentInstance() const;
      %extend{
        int __cmp__(Port* other)
        {
          if(self==other)return 0;
          else return 1;
        }
        long ptr()
        {
          return (long)self;
        }
      }
    protected:
      Port(Node *node);
      Port(const Port& other, Node *newHelder);
    };

    class DataPort : public virtual Port
    {
    public:
      static const char NAME[];
      TypeCode* edGetType() const ;
      void edSetType(TypeCode* type);
      std::string getName() const ;
      std::string getNameOfTypeOfCurrentInstance() const;
      virtual void edRemoveAllLinksLinkedWithMe() = 0;
    protected:
      virtual ~DataPort();
      DataPort(const DataPort& other, Node *newHelder);
      DataPort(const std::string& name, Node *node, TypeCode* type);
    };

    class InPort;
    class OutPort : virtual public DataPort
    {
    protected:
      OutPort(const OutPort& other, Node *newHelder);
      OutPort(const std::string& name, Node *node, TypeCode* type);
    public:
      virtual int edGetNumberOfOutLinks() const;
      virtual std::set<InPort *> edSetInPort() const = 0;
      virtual bool isAlreadyLinkedWith(InPort *with) const = 0;
      virtual void getAllRepresented(std::set<OutPort *>& represented) const;
      virtual bool addInPort(InPort *inPort) throw(Exception) = 0;
      virtual int removeInPort(InPort *inPort, bool forward) throw(Exception) = 0;
      virtual ~OutPort();
      std::vector<DataPort *> calculateHistoryOfLinkWith(InPort *end);
    };

    class InPort : virtual public DataPort
    {
    public:
      virtual int edGetNumberOfLinks() const;
      virtual std::set<OutPort *> edSetOutPort() const;
      virtual ~InPort();
    protected:
      InPort(const InPort& other, Node *newHelder);
      InPort(const std::string& name, Node *node, TypeCode* type);
      void edRemoveAllLinksLinkedWithMe() throw(Exception);
      virtual void edNotifyReferencedBy(OutPort *fromPort);
      virtual void edNotifyDereferencedBy(OutPort *fromPort);
      virtual void getAllRepresentants(std::set<InPort *>& repr) const;
    };
  }
}

%include <InGate.hxx>
%include <OutGate.hxx>
%include <DataFlowPort.hxx>
%include <DataStreamPort.hxx>

namespace YACS
{
  namespace ENGINE
  {
    class InputPort : public DataFlowPort, public InPort
    {
    public:
      std::string getNameOfTypeOfCurrentInstance() const;
      template<class T>
      void edInit(T value);
      %template(edInitInt)    edInit<int>;
      %template(edInitBool)    edInit<bool>;
      %template(edInitString)    edInit<std::string>;
      %template(edInitDbl)    edInit<double>;
      void edInit(const std::string& impl,char* value);
      virtual InputPort *clone(Node *newHelder) const = 0;
      bool isEmpty();
      virtual void put(const void *data) throw(ConversionException) = 0;
      virtual std::string dump();
    protected:
      InputPort(const std::string& name, Node *node, TypeCode* type);
    };

    class OutputPort : public DataFlowPort, public OutPort
    {
    public:
      virtual ~OutputPort();
      std::set<InPort *> edSetInPort() const;
      bool isAlreadyLinkedWith(InPort *with) const;
      bool isAlreadyInSet(InputPort *inputPort) const;
      std::string getNameOfTypeOfCurrentInstance() const;
      int removeInPort(InPort *inPort, bool forward) throw(Exception);
      virtual bool edAddInputPort(InputPort *phyPort) throw(Exception);
      virtual int edRemoveInputPort(InputPort *inputPort, bool forward) throw(Exception);
      bool addInPort(InPort *inPort) throw(Exception);
      void edRemoveAllLinksLinkedWithMe() throw(Exception);//entry point for forward port deletion
      virtual void exInit();
      virtual OutputPort *clone(Node *newHelder) const = 0;
      virtual void put(const void *data) throw(ConversionException);
      virtual std::string dump();
    protected:
      OutputPort(const OutputPort& other, Node *newHelder);
      OutputPort(const std::string& name, Node *node, TypeCode* type);
    };

    class InputDataStreamPort : public DataStreamPort, public InPort
    {
    public:
      InputDataStreamPort(const std::string& name, Node *node, TypeCode* type);
      std::string getNameOfTypeOfCurrentInstance() const;
      InputDataStreamPort *clone(Node *newHelder) const;
    };

    class OutputDataStreamPort : public DataStreamPort, public OutPort
    {
    public:
      OutputDataStreamPort(const OutputDataStreamPort& other, Node *newHelder);
      OutputDataStreamPort(const std::string& name, Node *node, TypeCode* type);
      virtual ~OutputDataStreamPort();
      virtual OutputDataStreamPort *clone(Node *newHelder) const;
      std::set<InPort *> edSetInPort() const;
      virtual int edGetNumberOfOutLinks() const;
      bool isAlreadyLinkedWith(InPort *with) const;
      virtual std::string getNameOfTypeOfCurrentInstance() const;
      virtual bool addInPort(InPort *inPort) throw(Exception);
      virtual bool edAddInputDataStreamPort(InputDataStreamPort *port) throw(ConversionException);
      int edRemoveInputDataStreamPort(InputDataStreamPort *inPort, bool forward) throw(Exception);
      void edRemoveAllLinksLinkedWithMe() throw(Exception);
      int removeInPort(InPort *inPort, bool forward) throw(Exception);
    };

    class ComposedNode;
    class ElementaryNode;

    class Node
    {
    protected:
      Node(const std::string& name);
    public:
      virtual ~Node();
      YACS::StatesForNode getState() ;
      virtual YACS::StatesForNode getEffectiveState() ;
      std::string getColorState(YACS::StatesForNode state);
      InGate *getInGate() ;
      OutGate *getOutGate();
      const std::string& getName();
      ComposedNode * getFather() ;
      std::set<Node *> getOutNodes() const;
      virtual std::set<ElementaryNode *> getRecursiveConstituents() const = 0;
      virtual int getNumberOfInputPorts() const = 0;
      virtual int getNumberOfOutputPorts() const = 0;
      std::list<InPort *> getSetOfInPort() const;
      std::list<OutPort *> getSetOfOutPort() const;
      virtual std::list<InputPort *> getSetOfInputPort() const = 0;
      virtual std::list<OutputPort *> getSetOfOutputPort() const = 0;
      virtual std::string getInPortName(const InPort *) const = 0;
      virtual std::string getOutPortName(const OutPort *) const  = 0;
      virtual std::list<InputDataStreamPort *> getSetOfInputDataStreamPort() const = 0;
      virtual std::list<OutputDataStreamPort *> getSetOfOutputDataStreamPort() const = 0;
      InPort *getInPort(const std::string& name) const ;
      virtual OutPort *getOutPort(const std::string& name) const ;
      virtual std::set<OutPort *> getAllOutPortsLeavingCurrentScope() const = 0;
      virtual std::set<InPort *> getAllInPortsComingFromOutsideOfCurrentScope() const = 0;
      virtual InputPort *getInputPort(const std::string& name) const = 0;
      virtual OutputPort *getOutputPort(const std::string& name) const = 0;
      virtual InputDataStreamPort *getInputDataStreamPort(const std::string& name) const = 0;
      virtual OutputDataStreamPort *getOutputDataStreamPort(const std::string& name) const = 0;
      std::set<ComposedNode *> getAllAscendanceOf(ComposedNode *levelToStop = 0);
      std::string getImplementation();
      virtual ComposedNode *getRootNode() ;
      virtual void setProperty(const std::string& name,const std::string& value);
      virtual Node *getChildByName(const std::string& name) const = 0;
      virtual void sendEvent(const std::string& event);

      %extend{
        int __cmp__(Node* other)
        {
          if(self==other)return 0;
          else return 1;
        }
        long ptr()
        {
          return (long)self;
        }
      }
    };

    class Container
    {
    public:
      virtual bool isAlreadyStarted() const;
      virtual std::string getPlacementId() const = 0;
      virtual void attachOnCloning() const;
      virtual void dettachOnCloning() const;
      bool isAttachedOnCloning() const;
    protected:
      ~Container();
    };

    class ElementaryNode: public Node, public Task
    {
    public:
      virtual InputPort *edAddInputPort(const std::string& inputPortName, TypeCode* type) ;
      virtual OutputPort *edAddOutputPort(const std::string& outputPortName, TypeCode* type) ;
      InputPort *getInputPort(const std::string& name) ;
      OutputPort *getOutputPort(const std::string& name) ;
      InputDataStreamPort *getInputDataStreamPort(const std::string& name) ;
      OutputDataStreamPort *getOutputDataStreamPort(const std::string& name) ;
      virtual InputDataStreamPort *edAddInputDataStreamPort(const std::string& inputPortDSName, TypeCode* type) ;
      virtual OutputDataStreamPort *edAddOutputDataStreamPort(const std::string& outputPortDSName, TypeCode* type) ;
    protected:
      ElementaryNode(const std::string& name);
    };

    class ComposedNode: public Node, public Scheduler
    {
    protected:
      ComposedNode(const std::string& name);
      ComposedNode(const ComposedNode& other, ComposedNode *father);
    public:
      virtual ~ComposedNode();
      bool isFinished();
      DeploymentTree getDeploymentTree() const;
      std::vector<Task *> getNextTasks(bool& isMore);
      void notifyFrom(const Task *sender, YACS::Event event);
      bool edAddLink(OutPort *start, InPort *end) throw(Exception);
      virtual bool edAddDFLink(OutPort *start, InPort *end) throw(Exception);
      bool edAddLink(OutGate *start, InGate *end) throw(Exception);
      bool edAddCFLink(Node *nodeS, Node *nodeE) throw(Exception);
      void edRemoveLink(OutPort *start, InPort *end) throw(Exception);
      void edRemoveLink(OutGate *start, InGate *end) throw(Exception);
      virtual bool isRepeatedUnpredictablySeveralTimes() const { return false; }
      virtual std::set<Node *> edGetDirectDescendants() const =  0;
      std::set<ElementaryNode *> getRecursiveConstituents() const;
      std::string getInPortName(const InPort *) const throw (Exception);
      std::string getOutPortName(const OutPort *) const throw (Exception);

      int getNumberOfInputPorts() const;
      int getNumberOfOutputPorts() const;
      std::list<InputPort *> getSetOfInputPort() const;
      std::list<OutputPort *> getSetOfOutputPort() const;
      std::set<OutPort *> getAllOutPortsLeavingCurrentScope() const;
      std::set<InPort *> getAllInPortsComingFromOutsideOfCurrentScope() const;
      std::list<InputDataStreamPort *> getSetOfInputDataStreamPort() const;
      std::list<OutputDataStreamPort *> getSetOfOutputDataStreamPort() const;
      OutPort *getOutPort(const std::string& name) const throw(Exception);
      InputPort *getInputPort(const std::string& name) const throw(Exception);
      OutputPort *getOutputPort(const std::string& name) const throw(Exception);
      InputDataStreamPort *getInputDataStreamPort(const std::string& name) const throw(Exception);
      OutputDataStreamPort *getOutputDataStreamPort(const std::string& name) const throw(Exception);
      std::vector< std::pair<OutPort *, InPort *> > getSetOfInternalLinks() const;
      std::vector< std::pair<OutPort *, InPort *> > getSetOfLinksLeavingCurrentScope() const;
      virtual std::vector< std::pair<InPort *, OutPort *> > getSetOfLinksComingInCurrentScope() const;
      //
      ComposedNode *getRootNode() throw(Exception);
      bool isNodeAlreadyAggregated(Node *node) const;
      Node *isInMyDescendance(Node *nodeToTest) const;
      std::string getChildName(Node* node) const throw(Exception);
      Node *getChildByName(const std::string& name) const throw(Exception);
      static ComposedNode *getLowestCommonAncestor(Node *node1, Node *node2) throw(Exception);
      void loaded();
    };

    class Bloc: public ComposedNode
    {
    public:
      Bloc(const std::string& name);
      bool edAddChild(Node *DISOWNnode) ;
      void edRemoveChild(Node *node) ;
      std::set<Node *> getChildren();
      std::set<Node *> edGetDirectDescendants() const;
      bool isFinished();
    };

    class Loop : public ComposedNode
    {
    public:
      Loop(const std::string& name);
      int getNbOfTurns() ;
      void edSetNode(Node *DISOWNnode);
      std::set<Node *> edGetDirectDescendants() const;
      Node *edRemoveNode();
    };

    class ForLoop : public Loop
    {
    public:
      ForLoop(const std::string& name);
      InputPort *edGetNbOfTimesInputPort();
    };

    class WhileLoop : public Loop
    {
    public:
      WhileLoop(const std::string& name);
      InputPort *edGetConditionPort();
    };

    class DynParaLoop : public ComposedNode
    {
    public:
      Node *edRemoveNode();
      Node *edRemoveInitNode();
      Node *edSetNode(Node *DISOWNnode);
      Node *edSetInitNode(Node *DISOWNnode);
      InputPort *edGetNbOfBranchesPort();
      OutputPort *edGetSamplePort();
      unsigned getNumberOfBranchesCreatedDyn() const throw(Exception);
      Node *getChildByNameExec(const std::string& name, unsigned id) const throw(Exception);
    protected:
      ~DynParaLoop();
    };

    class ForEachLoop : public DynParaLoop
    {
    public:
      ForEachLoop(const std::string& name, TypeCode *typeOfDataSplitted);
      std::set<Node *> edGetDirectDescendants() const;
      InputPort *edGetSeqOfSamplesPort();
    };

    class Switch : public ComposedNode
    {
    public:
      Switch(const std::string& name);
      Node *clone(ComposedNode *father) const;
      Node *edSetDefaultNode(Node *DISOWNnode);
      Node *edReleaseDefaultNode() ;
      Node *edReleaseCase(int caseId) ;
      Node *edSetNode(int caseId, Node *DISOWNnode) ;
      InputPort *edGetConditionPort();
      std::set<Node *> edGetDirectDescendants() const;
      void checkConsistency();
    };

    class Proc: public Bloc
    {
    public:
      Proc(const std::string& name);
      bool isFinished();
      virtual TypeCode *createType(const std::string& name, const std::string& kind);
      virtual TypeCodeObjref *createInterfaceTc(const std::string& id, const std::string& name,
                                                std::list<TypeCodeObjref *> ltc);
      virtual TypeCode *createSequenceTc(const std::string& id, const std::string& name,
                                        TypeCode *content);
      virtual TypeCode *createStructTc(const std::string& id, const std::string& name);

      virtual TypeCode* getTypeCode(const std::string& name);
      virtual void setTypeCode(const std::string& name,TypeCode *t);

      YACS::StatesForNode getNodeState(int numId);
      std::string getXMLState(int numId);
      std::list<int> getNumIds();
      std::list<std::string> getIds();

      std::map<std::string, TypeCode*> typeMap;
      std::map<std::string, Node*> nodeMap;
      std::map<std::string, ServiceNode*> serviceMap;
      std::map<std::string, InlineNode*> inlineMap;
      std::vector<std::string> names;
      void setName(const std::string& name);
    };
  }
}

%include <InlineNode.hxx>
%include <ServiceNode.hxx>
%include <ComponentInstance.hxx>
%include <ServiceInlineNode.hxx>
%include <OptimizerAlg.hxx>
%include <OptimizerLoop.hxx>

namespace YACS
{
  namespace ENGINE
  {
    class SchemaSave
    {
    public:
      SchemaSave(Proc* proc);
      virtual void save(std::string xmlSchemaFile);
    };
  }
}
