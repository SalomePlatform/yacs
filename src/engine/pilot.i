//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
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

#define YACSBASES_EXPORT

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
#include "Logger.hxx"
#include "DeploymentTree.hxx"
#include "ComponentInstance.hxx"
#include "DataNode.hxx"

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
    return;
  }
  PyObject* pyapi = PyObject_GetAttrString(omnipy, (char*)"API");
  api = (omniORBPYAPI*)PyCObject_AsVoidPtr(pyapi);
  Py_DECREF(pyapi);
#endif
%}

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
//%template(TCmap)         std::map<std::string, YACS::ENGINE::TypeCode *>;
REFCOUNT_TEMPLATE(TCmap,YACS::ENGINE::TypeCode)
%template(NODEmap)       std::map<std::string, YACS::ENGINE::Node *>;
%template(INODEmap)      std::map<std::string, YACS::ENGINE::InlineNode *>;
%template(SNODEmap)      std::map<std::string, YACS::ENGINE::ServiceNode *>;
//%template(CONTAINmap)    std::map<std::string, YACS::ENGINE::Container *>;
REFCOUNT_TEMPLATE(CONTAINmap,YACS::ENGINE::Container)
%template(strvec)        std::vector<std::string>;
%template(linksvec)      std::vector< std::pair<YACS::ENGINE::OutPort *,YACS::ENGINE::InPort *> >;
%template(linkvec)       std::vector< std::pair<YACS::ENGINE::InPort *,YACS::ENGINE::OutPort *> >;
%template(instreamlist)  std::list<YACS::ENGINE::InputDataStreamPort *>;
%template(outstreamlist) std::list<YACS::ENGINE::OutputDataStreamPort *>;

%template()              std::pair<std::string, YACS::ENGINE::CatalogLoader *>;
%template(loadermap)     std::map<std::string,YACS::ENGINE::CatalogLoader *>;
%template()              std::pair<std::string, YACS::ENGINE::ComposedNode *>;
%template(composedmap)   std::map<std::string,YACS::ENGINE::ComposedNode *>;
%template()              std::pair<std::string, YACS::ENGINE::ComponentDefinition *>;
%template(compomap)      std::map<std::string, YACS::ENGINE::ComponentDefinition *>;
%template()              std::pair<std::string, std::string>;
%template(propmap)       std::map<std::string, std::string>;

REFCOUNT_TEMPLATE(CompoInstmap,YACS::ENGINE::ComponentInstance)
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
%newobject YACS::ENGINE::Runtime::createInDataNode;
%newobject YACS::ENGINE::Runtime::createOutDataNode;
%newobject YACS::ENGINE::Runtime::createBloc;
%newobject YACS::ENGINE::Runtime::createForLoop;
%newobject YACS::ENGINE::Runtime::createForEachLoop;
%newobject YACS::ENGINE::Runtime::createWhileLoop;
%newobject YACS::ENGINE::Runtime::createSwitch;
%newobject YACS::ENGINE::Runtime::loadCatalog;
%newobject YACS::ENGINE::Runtime::createComponentInstance;
%newobject YACS::ENGINE::Runtime::createContainer;
%newobject YACS::ENGINE::Runtime::createInputPort;
%newobject YACS::ENGINE::Runtime::createOutputPort;
%newobject YACS::ENGINE::Runtime::createInputDataStreamPort;
%newobject YACS::ENGINE::Runtime::createOutputDataStreamPort;
%newobject YACS::ENGINE::Node::clone;

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

%newobject YACS::ENGINE::Proc::createContainer;
%newobject YACS::ENGINE::Proc::createSequenceTc;
%newobject YACS::ENGINE::Proc::createInterfaceTc;
%newobject YACS::ENGINE::Proc::createStructTc;
%newobject YACS::ENGINE::Proc::createType;

%newobject YACS::ENGINE::TypeCode::interfaceTc;
%newobject YACS::ENGINE::TypeCode::sequenceTc;
%newobject YACS::ENGINE::TypeCode::structTc;

/*
 * End of ownership section
 */

%include <define.hxx>
%include <Exception.hxx>
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
%include <OptimizerAlg.hxx>
%include <OptimizerLoop.hxx>
%include <Switch.hxx>
%include <Visitor.hxx>
%include <VisitorSaveSchema.hxx>
%include <ComponentDefinition.hxx>
%include <Catalog.hxx>

%extend YACS::ENGINE::ConditionInputPort
{
  bool getPyObj()
  {
    return self->getValue();
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

