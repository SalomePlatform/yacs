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
#include "Catalog.hxx"
#include "TypeCode.hxx"
#include "Executor.hxx"

#include<iostream>
#include<cstdlib>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

Runtime* Runtime::_singleton = 0;
TypeCode* Runtime::_tc_double = 0;
TypeCode* Runtime::_tc_int = 0;
TypeCode* Runtime::_tc_bool = 0;
TypeCode* Runtime::_tc_string = 0;
TypeCode* Runtime::_tc_file = 0;

// --- init typecodes for edInit with C++ Any


const char Runtime::RUNTIME_ENGINE_INTERACTION_IMPL_NAME[]="Neutral";

// singleton creation must be done before by a derived class

Runtime* YACS::ENGINE::getRuntime() throw(Exception)
{
  if ( !  Runtime::_singleton )
    throw Exception("Runtime is not yet initialized");
  return Runtime::_singleton;
}

Runtime::Runtime()
{
  DEBTRACE("Runtime::Runtime");
  Runtime::_tc_double = new TypeCode(Double);
  Runtime::_tc_int    = new TypeCode(Int);
  Runtime::_tc_bool   = new TypeCode(Bool);
  Runtime::_tc_string = new TypeCode(String);
  Runtime::_tc_file = new TypeCodeObjref("file", "file");
  DEBTRACE( "_tc_double refcnt: " << Runtime::_tc_double->getRefCnt() );
  DEBTRACE( "_tc_int refcnt: " << Runtime::_tc_int->getRefCnt() );
  DEBTRACE( "_tc_bool refcnt: " << Runtime::_tc_bool->getRefCnt() );
  DEBTRACE( "_tc_string refcnt: " << Runtime::_tc_string->getRefCnt() );
  DEBTRACE( "_tc_file refcnt: " << Runtime::_tc_file->getRefCnt() );
  _builtinCatalog = new Catalog("builtins");
  _builtinCatalog->_composednodeMap["Bloc"]=new Bloc("Bloc");
  _builtinCatalog->_composednodeMap["Switch"]=new Switch("Switch");
  _builtinCatalog->_composednodeMap["WhileLoop"]=new WhileLoop("WhileLoop");
  _builtinCatalog->_composednodeMap["ForLoop"]=new ForLoop("ForLoop");
  _builtinCatalog->_composednodeMap["ForEachLoop_double"]=new ForEachLoop("ForEachLoop_double",Runtime::_tc_double);
  _builtinCatalog->_composednodeMap["ForEachLoop_string"]=new ForEachLoop("ForEachLoop_string",Runtime::_tc_string);
  _builtinCatalog->_composednodeMap["ForEachLoop_int"]=new ForEachLoop("ForEachLoop_int",Runtime::_tc_int);
  _builtinCatalog->_composednodeMap["ForEachLoop_bool"]=new ForEachLoop("ForEachLoop_bool",Runtime::_tc_bool);
  std::map<std::string,TypeCode*>& typeMap=_builtinCatalog->_typeMap;
  Runtime::_tc_double->incrRef();
  typeMap["double"]=Runtime::_tc_double;
  Runtime::_tc_int->incrRef();
  typeMap["int"]=Runtime::_tc_int;
  Runtime::_tc_bool->incrRef();
  typeMap["bool"]=Runtime::_tc_bool;
  Runtime::_tc_string->incrRef();
  typeMap["string"]=Runtime::_tc_string;
  Runtime::_tc_file->incrRef();
  typeMap["file"]=Runtime::_tc_file;

  char *maxThreadStr = getenv("YACS_MAX_THREADS");
  if (!maxThreadStr) return;
  int maxThreads = atoi(maxThreadStr);
  DEBTRACE("maxThreads = " << maxThreads);
  if (maxThreads <1) return;
  Executor::_maxThreads = maxThreads;
}

void Runtime::removeRuntime()
{
  delete this;
}

Runtime::~Runtime()
{
  delete _builtinCatalog;
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
  for(std::vector<Catalog *>::const_iterator it=_catalogs.begin();it !=_catalogs.end();it++)
    delete (*it);
  Runtime::_singleton=0;
  DEBTRACE( "Total YACS::ENGINE::Refcount: " << RefCounter::_totalCnt );
}

TypeCode * Runtime::createInterfaceTc(const std::string& id, const std::string& name,
                                      std::list<TypeCodeObjref *> ltc)
{
  return TypeCode::interfaceTc(id.c_str(),name.c_str(),ltc);
}

TypeCode * Runtime::createSequenceTc(const std::string& id,
                                     const std::string& name,
                                     TypeCode *content)
{
  return TypeCode::sequenceTc(id.c_str(),name.c_str(),content);
};

TypeCodeStruct * Runtime::createStructTc(const std::string& id, const std::string& name)
{
  return (TypeCodeStruct *)TypeCode::structTc(id.c_str(),name.c_str());
}

DataNode* Runtime::createInDataNode(const std::string& kind,const std::string& name)
{
  throw Exception("InDataNode factory not implemented");
}

DataNode* Runtime::createOutDataNode(const std::string& kind,const std::string& name)
{
  throw Exception("OutDataNode factory not implemented");
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

//! Load a catalog of calculation to use as factory
/*!
 * \param sourceKind: the kind of catalog source. It depends on runtime. It could be a file, a server, a directory
 * \param path: the path to the catalog. 
 * \return the loaded Catalog
 */
Catalog* Runtime::loadCatalog(const std::string& sourceKind,const std::string& path)
{
  if (_catalogLoaderFactoryMap.find(sourceKind) == _catalogLoaderFactoryMap.end())
    {
      throw Exception("This type of catalog loader does not exist: " + sourceKind);
    }
  else
    {
      Catalog* cata=new Catalog(path);
      CatalogLoader* proto=_catalogLoaderFactoryMap[sourceKind];
      proto->load(cata,path);
      return cata;
    }
}

//! Add a catalog loader factory to the map _catalogLoaderFactoryMap under the name name
/*!
 * \param name: name under which the factory is registered
 * \param factory: the factory
 */
void Runtime::setCatalogLoaderFactory(const std::string& name, CatalogLoader* factory)
{
  _catalogLoaderFactoryMap[name]=factory;
}

//! Get the catalog of base nodes (elementary and composed)
/*!
 * \return the builtin Catalog
 */
Catalog* Runtime::getBuiltinCatalog()
{
  return _builtinCatalog;
}

//! Add a catalog of types and nodes to the runtime
/*!
 * These catalogs are searched when calling getTypeCode method
 */
void Runtime::addCatalog(Catalog* catalog)
{
  _catalogs.push_back(catalog);
}

//! Get a typecode by its name from runtime catalogs
/*!
 * \return the typecode if it exists or NULL
 */
TypeCode* Runtime::getTypeCode(const std::string& name)
{
  if (_builtinCatalog->_typeMap.count(name) != 0)
    return _builtinCatalog->_typeMap[name];
  for(std::vector<Catalog *>::const_iterator it=_catalogs.begin();it !=_catalogs.end();it++)
    {
      if ((*it)->_typeMap.count(name) != 0)
        return (*it)->_typeMap[name];
    }
  return 0;
}

//! Convert a YACS::ENGINE::Any object to an external object with type type
/*!
 * This method is used to convert Neutral objects to script languages. For example
 * Python language. The runtime has one external script language.
 * The object is returned as a void * because engine knows nothing about external script language.
 *
 * \param type: the type of the converted object if the conversion is possible
 * \param data: the object to convert
 * \return the converted object
 */
void* Runtime::convertNeutral(TypeCode * type, Any *data)
{
  throw Exception("convertNeutral is not implemented by your runtime");
}

//! Convert a YACS::ENGINE::Any object to a string to be used in GUI for example
/*!
 * engine package does not provide a conversion to string. It has to be implemented in the 
 * runtime package.
 *
 * \param type: the type of the object to convert
 * \param data: the object to convert
 * \return the string representation of the object
 */
std::string Runtime::convertNeutralAsString(TypeCode * type, Any *data)
{
  throw Exception("convertNeutralAsString is not implemented by your runtime");
}

