// Copyright (C) 2006-2024  CEA, EDF
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

%define DOCSTRING
"All is needed to create and execute a calculation schema."
%enddef

%module(docstring=DOCSTRING) pilot

#ifndef SWIGIMPORTED
//work around SWIG bug #1863647
#if SWIG_VERSION >= 0x010336
#define SwigPyIterator pilot_PySwigIterator
#else
#define PySwigIterator pilot_PySwigIterator
#endif
#endif

%feature("autodoc", "1");

%include "engtypemaps.i"

#ifdef DOXYGEN_IS_OK
%include docengine.i
#endif

%{
#include "Any.hxx"
#include "TypeCode.hxx"
#include "ComponentDefinition.hxx"
#include "Visitor.hxx"
#include "VisitorSaveSchema.hxx"
#include "VisitorSaveState.hxx"
#include "LinkInfo.hxx"
#include "Catalog.hxx"
#include "Executor.hxx"
#include "ExecutorSwig.hxx"
#include "Dispatcher.hxx"
#include "Container.hxx"
#include "HomogeneousPoolContainer.hxx"
#include "Logger.hxx"
#include "DeploymentTree.hxx"
#include "ComponentInstance.hxx"
#include "DataNode.hxx"
#include "PlayGround.hxx"
#include "SetOfPoints.hxx"
#include "PointVisitor.hxx"
#include "ForkBlocPoint.hxx"
#include "LinkedBlocPoint.hxx"
#include "NotSimpleCasePoint.hxx"
#include "ElementaryPoint.hxx"
#include "ObserverAsPlugin.hxx"
#include "InGate.hxx"
  
using namespace YACS::ENGINE;

%}

%init
%{
  // init section
#ifdef OMNIORB
  PyObject* omnipy = PyImport_ImportModule((char*)"_omnipy");
  if (!omnipy)
  {
    PyErr_SetString(PyExc_ImportError,(char*)"Cannot import _omnipy");
    return NULL;
  }
  PyObject* pyapi = PyObject_GetAttrString(omnipy, (char*)"API");
  api = (omniORBpyAPI*)PyCapsule_GetPointer(pyapi,"_omnipy.API");
  Py_DECREF(pyapi);
#endif
%}

%ignore YACS::ENGINE::Any::operator[];
%ignore YACS::ENGINE::TypeCode::operator=;
%ignore YACS::ENGINE::DeploymentTree::operator=;
%ignore operator<<;
%ignore YACS::ENGINE::Runtime::_tc_double;
%ignore YACS::ENGINE::Runtime::_tc_int;
%ignore YACS::ENGINE::Runtime::_tc_bool;
%ignore YACS::ENGINE::Runtime::_tc_string;
%ignore YACS::ENGINE::Runtime::_tc_file;
%rename(StateLoader) YACS::ENGINE::StateLoader; // to suppress a 503 warning
%rename(NbDoneLoader) YACS::ENGINE::NbDoneLoader; // to suppress a 503 warning
%rename(getRuntime) YACS::ENGINE::getRuntime; // to suppress a 503 warning
%rename(_from) YACS::ENGINE::DataLinkInfo::from ; // to suppress a 314 warning
%rename(_from) YACS::ENGINE::StreamLinkInfo::from ; // to suppress a 314 warning

/*
 * Template section
 */
%template()              std::pair<std::string, YACS::ENGINE::TypeCode *>;
%template()              std::pair<std::string, YACS::ENGINE::Node *>;
%template()              std::pair<std::string, YACS::ENGINE::InlineNode *>;
%template()              std::pair<std::string, YACS::ENGINE::ServiceNode *>;
%template()              std::pair<std::string, YACS::ENGINE::Container *>;
%template()              std::pair<YACS::ENGINE::OutPort *,YACS::ENGINE::InPort *>;
%template()              std::pair<YACS::ENGINE::InPort *,YACS::ENGINE::OutPort *>;
%template()              std::pair< std::string, int >;
%template()              std::pair< YACS::ENGINE::InGate *, bool>;
%template(ItPy3TC)       IteratorPy3<YACS::ENGINE::TypeCode *>;
//%template(TCmap)         std::map<std::string, YACS::ENGINE::TypeCode *>;
REFCOUNT_TEMPLATE(TCmap,YACS::ENGINE::TypeCode)
%template(NODEmap)       std::map<std::string, YACS::ENGINE::Node *>;
%template(INODEmap)      std::map<std::string, YACS::ENGINE::InlineNode *>;
%template(SNODEmap)      std::map<std::string, YACS::ENGINE::ServiceNode *>;
//%template(CONTAINmap)    std::map<std::string, YACS::ENGINE::Container *>;
%template(ItPy3Cont)     IteratorPy3<YACS::ENGINE::Container * >;
REFCOUNT_TEMPLATE(CONTAINmap,YACS::ENGINE::Container)
%template(strvec)        std::vector<std::string>;
%template(uivec)         std::vector<unsigned int>;
%template(ivec)          std::vector<int>;
%template(linksvec)      std::vector< std::pair<YACS::ENGINE::OutPort *,YACS::ENGINE::InPort *> >;
%template(linkvec)       std::vector< std::pair<YACS::ENGINE::InPort *,YACS::ENGINE::OutPort *> >;
%template(instreamlist)  std::list<YACS::ENGINE::InputDataStreamPort *>;
%template(outstreamlist) std::list<YACS::ENGINE::OutputDataStreamPort *>;
%template(vpsi)          std::vector< std::pair< std::string, int > >;
          
%template()              std::pair<std::string, YACS::ENGINE::CatalogLoader *>;
%template(loadermap)     std::map<std::string,YACS::ENGINE::CatalogLoader *>;
%template()              std::pair<std::string, YACS::ENGINE::ComposedNode *>;
%template(composedmap)   std::map<std::string,YACS::ENGINE::ComposedNode *>;
%template()              std::pair<std::string, YACS::ENGINE::ComponentDefinition *>;
%template(compomap)      std::map<std::string, YACS::ENGINE::ComponentDefinition *>;
%template()              std::pair<std::string, std::string>;
%template(propmap)       std::map<std::string, std::string>;
%template(ItPy3Comp)     IteratorPy3<YACS::ENGINE::ComponentInstance *>;
%template(listpairingatebool) std::list< std::pair< YACS::ENGINE::InGate *, bool> >;

REFCOUNT_TEMPLATE(CompoInstmap,YACS::ENGINE::ComponentInstance)

%include "exception.i"

/*
 * End of Template section
 */

%typemap(out) Container *
{
  $result=convertContainer($1,$owner);
}

%typemap(out) YACS::ENGINE::Container *
{
  $result=convertContainer($1,$owner);
}

/*
 * Ownership section
 */
//Take ownership : it is not the default (constructor) as it is a factory
%newobject *::createProc;
%newobject *::createScriptNode;
%newobject *::createFuncNode;
%newobject *::createRefNode;
%newobject *::createCompoNode;
%newobject *::createSInlineNode;
%newobject *::createInDataNode;
%newobject *::createOutDataNode;
%newobject *::createBloc;
%newobject *::createForLoop;
%newobject *::createForEachLoop;
%newobject *::createForEachLoopDyn;
%newobject *::createWhileLoop;
%newobject *::createSwitch;
%newobject *::loadCatalog;
%newobject *::createComponentInstance;
%newobject *::createContainer;


%newobject *::createInputPort;
%newobject *::createOutputPort;
%newobject *::createInputDataStreamPort;
%newobject *::createOutputDataStreamPort;
%newobject *::clone;
%newobject *::cloneAlways;
%newobject *::cloneWithoutCompAndContDeepCpy;
%newobject *::New;

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

%newobject *::createSequenceTc;
%newobject *::createInterfaceTc;
%newobject *::createStructTc;
%newobject *::createType;

%newobject YACS::ENGINE::SequenceAny::removeUnsetItemsFromThis;

%newobject YACS::ENGINE::TypeCode::interfaceTc;
%newobject YACS::ENGINE::TypeCode::sequenceTc;
%newobject YACS::ENGINE::TypeCode::structTc;

/*
 * End of ownership section
 */


%include <define.hxx>
%include <YACSBasesExport.hxx>
%include <Exception.hxx>
%include <YACSlibEngineExport.hxx>
%include <ConversionException.hxx>
%include <Runtime.hxx>
%include <PropertyInterface.hxx>

PYEXCEPTION(YACS::ENGINE::Executor::RunW)
PYEXCEPTION(YACS::ENGINE::Executor::RunB)
PYEXCEPTION(YACS::ENGINE::Executor::setExecMode)
PYEXCEPTION(YACS::ENGINE::Executor::resumeCurrentBreakPoint)
PYEXCEPTION(YACS::ENGINE::Executor::stopExecution)
PYEXCEPTION(YACS::ENGINE::Executor::waitPause)
PYEXCEPTION(YACS::ENGINE::ComponentInstance::load)

%include <Executor.hxx>

EXCEPTION(YACS::ENGINE::ExecutorSwig::RunPy)
EXCEPTION(YACS::ENGINE::ExecutorSwig::waitPause)
%include <ExecutorSwig.hxx>

%include <RefCounter.hxx>

%feature("unref") YACS::ENGINE::Any "$this->decrRef();"
%include <Any.hxx>

%ignore YACS::ENGINE::TypeCode::getOrBuildAnyFromZippedData;
%include <TypeCode.hxx>

%include <Scheduler.hxx>
%include <Task.hxx>
%include <Dispatcher.hxx>
%include <DeploymentTree.hxx>
%include <Port.hxx>
%extend YACS::ENGINE::Port
{
  /* __cmp__ does not exist in Python 3 */
  int  __lt__(Port* other)
    {
      if(self==other)
        return 0;
      else 
        return 1;
    }
  int  __gt__(Port* other)
    {
      if(self==other)
        return 0;
      else 
        return 1;
    }
  int __ne__(Port* other)
    {
      if(self==other)
        return 0;
      else 
        return 1;
    }
  int __eq__(Port* other)
    {
      if(self==other)
        return 0;
      else 
        return 1;
    }
  int __le__(Port* other)
    {
      if(self==other)
        return 0;
      else 
        return 1;
    }
  int __ge__(Port* other)
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
%include <HomogeneousPoolContainer.hxx>
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
%include <InPropertyPort.hxx>
%extend YACS::ENGINE::InPropertyPort
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

%include <AnyInputPort.hxx>
%include <ConditionInputPort.hxx>
%include <OutputPort.hxx>
%include <AnyOutputPort.hxx>
%include <InputDataStreamPort.hxx>
%include <OutputDataStreamPort.hxx>
%include <DataPort.hxx>

%include <Node.hxx>
%extend YACS::ENGINE::Node 
{
  /* __cmp__ does not exist in Python 3 */
  int  __lt__(Node* other)
    {
      if(self==other)
        return 0;
      else 
        return 1;
    }
  int  __gt__(Node* other)
    {
      if(self==other)
        return 0;
      else 
        return 1;
    }
  int __ne__(Node* other)
    {
      if(self==other)
        return 0;
      else 
        return 1;
    }
  int __eq__(Node* other)
    {
      if(self==other)
        return 0;
      else 
        return 1;
    }
  int __le__(Node* other)
    {
      if(self==other)
        return 0;
      else 
        return 1;
    }
  int __ge__(Node* other)
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
%include <ComplexWeight.hxx>
%include <ElementaryNode.hxx>
%include <InlineNode.hxx>
%include <ServiceNode.hxx>
%include <ServiceInlineNode.hxx>
%include <ServerNode.hxx>
%include <DataNode.hxx>

%include <ComposedNode.hxx>
%include <StaticDefinedComposedNode.hxx>
%include <Bloc.hxx>
%include <Proc.hxx>

%include <Loop.hxx>
%include <ForLoop.hxx>
%include <DynParaLoop.hxx>
%include <WhileLoop.hxx>
%include <ForEachLoop.hxx>
%include <OptimizerLoop.hxx>
%include <Switch.hxx>
%include <Visitor.hxx>
%include <VisitorSaveSchema.hxx>
%include <ComponentDefinition.hxx>
%include <Catalog.hxx>
%include <Pool.hxx>

%include <AlternateThread.hxx>
%include <AlternateThreadPT.hxx>

// Ignore class OptimizerAlgASync to avoid confusion with class
// OptimizerAlgASync in module SALOMERuntime
%ignore YACS::ENGINE::OptimizerAlgASync;
%include <OptimizerAlg.hxx>
%include "PlayGround.i"

%extend YACS::ENGINE::ConditionInputPort
{
  bool getPyObj()
  {
    return self->getValue();
  }
}
%extend YACS::ENGINE::Any
{
  PyObject *getBytes()
  {
    YACS::ENGINE::AtomAny *self2(dynamic_cast<YACS::ENGINE::AtomAny *>(self));
    if(!self2)
      throw YACS::Exception("getBytes : self is not an AtomAny !");
    std::size_t len(0);
    const char *pt(self2->getBytesValue(len));
    return PyBytes_FromStringAndSize(pt,len);
  }
}

%extend YACS::ENGINE::AnyInputPort
{
  PyObject * getPyObj()
  {
    return (PyObject *)getRuntime()->convertNeutral(self->edGetType(),self->getValue());
  }
}

%extend YACS::ENGINE::AnyOutputPort
{
  PyObject * getPyObj()
  {
    return (PyObject *)getRuntime()->convertNeutral(self->edGetType(),self->getValue());
  }
}

%extend YACS::ENGINE::Any
{
  PyObject *getPyObj()
  {
    return (PyObject *)getRuntime()->convertNeutral(const_cast<YACS::ENGINE::TypeCode *>(self->getType()),self);
  }
}

%newobject YACS::ENGINE::SequenceAny::__getitem__;
%extend YACS::ENGINE::SequenceAny
{
  Any* __getitem__(int i)
  {
    if (i < self->size())
      {
        AnyPtr a=(*self)[i];
        a->incrRef();
        return a;
      }
    else
      throw std::length_error("index too large");
  }
}

%newobject YACS::ENGINE::StructAny::__getitem__;
%extend YACS::ENGINE::StructAny
{
  Any* __getitem__(const char * key)
  {
    AnyPtr a=(*self)[key];
    a->incrRef();
    return a;
  }
}

%extend YACS::ENGINE::ForEachLoop
{
  PyObject *getPassedResults(Executor *execut) const
  {
    std::vector<SequenceAny *> ret1;
    std::vector<std::string> ret2;
    std::vector<unsigned int> ret0(self->getPassedResults(execut,ret1,ret2));
    PyObject *ret(PyTuple_New(3));
    // param 0
    PyObject *ret0Py(PyList_New(ret0.size()));
    for(std::size_t i=0;i<ret0.size();i++)
      PyList_SetItem(ret0Py,i,PyLong_FromLong(ret0[i]));
    PyTuple_SetItem(ret,0,ret0Py);
    // param 1
    PyObject *ret1Py(PyList_New(ret1.size()));
    for(std::size_t i=0;i<ret1.size();i++)
      PyList_SetItem(ret1Py,i,SWIG_NewPointerObj(SWIG_as_voidptr(ret1[i]),SWIGTYPE_p_YACS__ENGINE__SequenceAny, SWIG_POINTER_OWN | 0 ));
    PyTuple_SetItem(ret,1,ret1Py);
    // param 2
    PyObject *ret2Py(PyList_New(ret2.size()));
    for(std::size_t i=0;i<ret2.size();i++)
      PyList_SetItem(ret2Py,i,PyUnicode_FromString(ret2[i].c_str()));
    PyTuple_SetItem(ret,2,ret2Py);
    return ret;
  }

  void assignPassedResults(const std::vector<unsigned int>& passedIds, PyObject *passedOutputs, const std::vector<std::string>& nameOfOutputs)
  {
    std::vector<SequenceAny *> passedOutputsCpp;
    convertFromPyObjVectorOfObj<YACS::ENGINE::SequenceAny *>(passedOutputs,SWIGTYPE_p_YACS__ENGINE__SequenceAny,"SequenceAny",passedOutputsCpp);
    self->assignPassedResults(passedIds,passedOutputsCpp,nameOfOutputs);
  }
}

namespace YACS
{
  namespace ENGINE
  {
    class AbstractPoint
    {
    protected:
      virtual ~AbstractPoint();
      AbstractPoint();
      AbstractPoint(const AbstractPoint&);
    };

    class ElementaryPoint : public AbstractPoint
    {
    public:
      Node *getFirstNode();
    private:
      ~ElementaryPoint();
      ElementaryPoint();
      ElementaryPoint(const ElementaryPoint&);
    };

    class BlocPoint : public AbstractPoint
    {
    protected:
      ~BlocPoint();
      BlocPoint();
      BlocPoint(const BlocPoint&);
    };

    class LinkedBlocPoint : public BlocPoint
    {
    private:
      ~LinkedBlocPoint();
      LinkedBlocPoint();
      LinkedBlocPoint(const LinkedBlocPoint&);
    };

    class ForkBlocPoint : public BlocPoint
    {
    private:
      ~ForkBlocPoint();
      ForkBlocPoint();
      ForkBlocPoint(const ForkBlocPoint&);
    };

    class NotSimpleCasePoint : public BlocPoint
    {
    private:
      ~NotSimpleCasePoint();
      NotSimpleCasePoint();
      NotSimpleCasePoint(const NotSimpleCasePoint&);
    };
    
    class SetOfPoints
    {
    public:
      SetOfPoints(const std::list<Node *>& nodes);
      ~SetOfPoints();
      void simplify();
      std::string getRepr() const;
      %extend
      {
      void accept(PyObject *pv)
      {
        class MyPointVisitor : public YACS::ENGINE::PointVisitor
        {
        public:
          MyPointVisitor(PyObject *py):_py(py) { }
          void beginForkBlocPoint(ForkBlocPoint *pt)
          {
            PyObject *ptPy(SWIG_NewPointerObj((void*)pt,SWIGTYPE_p_YACS__ENGINE__ForkBlocPoint,0));
            PyObject *meth(PyString_FromString("beginForkBlocPoint"));
            PyObject *ret(PyObject_CallMethodObjArgs(_py,meth,ptPy,nullptr));
            Py_XDECREF(ret); Py_XDECREF(meth); Py_XDECREF(ptPy);
          }
          void endForkBlocPoint(ForkBlocPoint *pt)
          {
            PyObject *ptPy(SWIG_NewPointerObj((void*)pt,SWIGTYPE_p_YACS__ENGINE__ForkBlocPoint,0));
            PyObject *meth(PyString_FromString("endForkBlocPoint"));
            PyObject *ret(PyObject_CallMethodObjArgs(_py,meth,ptPy,nullptr));
            Py_XDECREF(ret); Py_XDECREF(meth); Py_XDECREF(ptPy);
          }
          void beginLinkedBlocPoint(LinkedBlocPoint *pt)
          {
            PyObject *ptPy(SWIG_NewPointerObj((void*)pt,SWIGTYPE_p_YACS__ENGINE__LinkedBlocPoint,0));
            PyObject *meth(PyString_FromString("beginLinkedBlocPoint"));
            PyObject *ret(PyObject_CallMethodObjArgs(_py,meth,ptPy,nullptr));
            Py_XDECREF(ret); Py_XDECREF(meth); Py_XDECREF(ptPy);
          }
          void endLinkedBlocPoint(LinkedBlocPoint *pt)
          {
            PyObject *ptPy(SWIG_NewPointerObj((void*)pt,SWIGTYPE_p_YACS__ENGINE__LinkedBlocPoint,0));
            PyObject *meth(PyString_FromString("endLinkedBlocPoint"));
            PyObject *ret(PyObject_CallMethodObjArgs(_py,meth,ptPy,nullptr));
            Py_XDECREF(ret); Py_XDECREF(meth); Py_XDECREF(ptPy);
          }
          void beginElementaryPoint(ElementaryPoint *pt)
          {
            PyObject *ptPy(SWIG_NewPointerObj((void*)pt,SWIGTYPE_p_YACS__ENGINE__ElementaryPoint,0));//$descriptor(YACS::ENGINE::ElementaryPoint *)
            PyObject *meth(PyString_FromString("beginElementaryPoint"));
            PyObject *ret(PyObject_CallMethodObjArgs(_py,meth,ptPy,nullptr));
            Py_XDECREF(ret); Py_XDECREF(meth); Py_XDECREF(ptPy);
          }
          void endElementaryPoint(ElementaryPoint *pt)
          {
            PyObject *ptPy(SWIG_NewPointerObj((void*)pt,SWIGTYPE_p_YACS__ENGINE__ElementaryPoint,0));
            PyObject *meth(PyString_FromString("endElementaryPoint"));
            PyObject *ret(PyObject_CallMethodObjArgs(_py,meth,ptPy,nullptr));
            Py_XDECREF(ret); Py_XDECREF(meth); Py_XDECREF(ptPy);
          }
          void beginNotSimpleCasePoint(NotSimpleCasePoint *pt)
          {
            PyObject *ptPy(SWIG_NewPointerObj((void*)pt,SWIGTYPE_p_YACS__ENGINE__NotSimpleCasePoint,0));//$descriptor(YACS::ENGINE::NotSimpleCasePoint *)
            PyObject *meth(PyString_FromString("beginNotSimpleCasePoint"));
            PyObject *ret(PyObject_CallMethodObjArgs(_py,meth,ptPy,nullptr));
            Py_XDECREF(ret); Py_XDECREF(meth); Py_XDECREF(ptPy);
          }
          void endNotSimpleCasePoint(NotSimpleCasePoint *pt)
          {
            PyObject *ptPy(SWIG_NewPointerObj((void*)pt,SWIGTYPE_p_YACS__ENGINE__NotSimpleCasePoint,0));
            PyObject *meth(PyString_FromString("endNotSimpleCasePoint"));
            PyObject *ret(PyObject_CallMethodObjArgs(_py,meth,ptPy,nullptr));
            Py_XDECREF(ret); Py_XDECREF(meth); Py_XDECREF(ptPy);
          }
        private:
          PyObject *_py;
        };
        MyPointVisitor mpv(pv);
        self->accept(&mpv);
        }
      }
    };
  }
}

%rename(LoadObserversPluginIfAny) LoadObserversPluginIfAnySwig;
%rename(UnLoadObserversPluginIfAny) UnLoadObserversPluginIfAnySwig;
                                  
%inline{
  void LoadObserversPluginIfAnySwig(YACS::ENGINE::ComposedNode *rootNode, YACS::ENGINE::ExecutorSwig *executor)
  {
    YACS::ENGINE::LoadObserversPluginIfAny(rootNode,executor);
  }

  void UnLoadObserversPluginIfAnySwig()
  {
    YACS::ENGINE::UnLoadObserversPluginIfAny();
  }

  PyObject *ToBase64Swig(PyObject *bytes)
  {
    char *pt = nullptr;
    Py_ssize_t length=0;
    PyBytes_AsStringAndSize(bytes,&pt,&length);
    std::string input(pt,length);
    std::string ret(YACS::ENGINE::ToBase64(input));
    return PyBytes_FromStringAndSize(ret.c_str(),ret.size());
  }

  PyObject *FromBase64Swig(PyObject *base64Str)
  {
    char *pt = nullptr;
    Py_ssize_t length=0;
    PyBytes_AsStringAndSize(base64Str,&pt,&length);
    std::string input(pt,length);
    std::string ret(YACS::ENGINE::FromBase64(input));
    return PyBytes_FromStringAndSize(ret.c_str(),ret.size());
  }
}
