#include "Runtime.hxx"

#include<cassert>
#include "WhileLoop.hxx"
#include "ForLoop.hxx"
#include "ForEachLoop.hxx"
#include "Switch.hxx"
#include "Bloc.hxx"
#include "Proc.hxx"
#include "InputDataStreamPort.hxx"
#include "OutputDataStreamPort.hxx"
#include<iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

Runtime* Runtime::_singleton = 0;

// --- init typecodes for edInit with C++ Any

TypeCode *Runtime::_tc_double = new TypeCode(Double);
TypeCode *Runtime::_tc_int    = new TypeCode(Int);
TypeCode *Runtime::_tc_bool   = new TypeCode(Bool);
TypeCode *Runtime::_tc_string = new TypeCode(String);
TypeCode *Runtime::_tc_file = new TypeCodeObjref("file", "file");

const char Runtime::RUNTIME_ENGINE_INTERACTION_IMPL_NAME[]="Neutral";

// singleton creation must be done before by a derived class

Runtime* YACS::ENGINE::getRuntime() throw(Exception)
{
  if ( !  Runtime::_singleton )
    throw Exception("Runtime is not yet initialized");
  return Runtime::_singleton;
}

Runtime::~Runtime()
{
  DEBTRACE( "_tc_double refcnt: " << Runtime::_tc_double->getRefCnt() );
  DEBTRACE( "_tc_int refcnt: " << Runtime::_tc_int->getRefCnt() );
  DEBTRACE( "_tc_bool refcnt: " << Runtime::_tc_bool->getRefCnt() );
  DEBTRACE( "_tc_string refcnt: " << Runtime::_tc_string->getRefCnt() );
  DEBTRACE( "_tc_file refcnt: " << Runtime::_tc_file->getRefCnt() );
  Runtime::_tc_double->decrRef();
  Runtime::_tc_int->decrRef();
  Runtime::_tc_bool->decrRef();
  Runtime::_tc_string->decrRef();
  Runtime::_tc_file->decrRef();
  Runtime::_singleton=0;
  DEBTRACE( "Total YACS::ENGINE::Refcount: " << RefCounter::_totalCnt );
}

InlineFuncNode* Runtime::createFuncNode(const std::string& kind,const std::string& name)
{
  throw Exception("FuncNode factory not implemented");
}

InlineNode* Runtime::createScriptNode(const std::string& kind,const std::string& name)
{
  throw Exception("ScriptNode factory not implemented");
}

ServiceNode* Runtime::createRefNode(const std::string& kind,const std::string& name)
{
  throw Exception("RefNode factory not implemented");
}

ServiceNode* Runtime::createCompoNode(const std::string& kind,const std::string& name)
{
  throw Exception("CompoNode factory not implemented");
}

ServiceInlineNode *Runtime::createSInlineNode(const std::string& kind, const std::string& name)
{
  throw Exception("SInlineNode factory not implemented");
}

ComponentInstance* Runtime::createComponentInstance(const std::string& name,
                                                    const std::string& kind)
{
  throw Exception("ComponentInstance factory not implemented");
}

Container *Runtime::createContainer(const std::string& kind)
{
  throw Exception("Container factory not implemented");
}

Proc* Runtime::createProc(const std::string& name)
{
  return new Proc(name);
}

Bloc* Runtime::createBloc(const std::string& name)
{
  return new Bloc(name);
}

Switch* Runtime::createSwitch(const std::string& name)
{
  return new Switch(name);
}

WhileLoop* Runtime::createWhileLoop(const std::string& name)
{
  return new WhileLoop(name);
}

ForLoop* Runtime::createForLoop(const std::string& name)
{
  return new ForLoop(name);
}

ForEachLoop* Runtime::createForEachLoop(const std::string& name,TypeCode *type)
{
  return new ForEachLoop(name,type);
}

InputDataStreamPort* Runtime::createInputDataStreamPort(const std::string& name,Node *node,TypeCode *type)
{
  return new InputDataStreamPort(name,node,type);
}

OutputDataStreamPort* Runtime::createOutputDataStreamPort(const std::string& name,Node *node,TypeCode *type)
{
  return new OutputDataStreamPort(name,node,type);
}
