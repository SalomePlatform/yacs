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

#ifndef _RUNTIME_HXX_
#define _RUNTIME_HXX_

#include "YACSlibEngineExport.hxx"
#include "ConversionException.hxx"

#include<string>
#include<set>
#include<map>
#include<vector>
#include<list>

namespace YACS
{
  namespace ENGINE
  {
    class Runtime;
    YACSLIBENGINE_EXPORT Runtime* getRuntime() throw(Exception);

    class Any;
    class InputPort;
    class OutputPort;
    class InPropertyPort;
    class ForLoop;
    class ForEachLoop;
    class OptimizerLoop;
    class WhileLoop;
    class Switch;
    class InlineNode;
    class InlineFuncNode;
    class ServiceNode;
    class DataNode;
    class Container;
    class ServiceInlineNode;
    class ComponentInstance;
    class Proc;
    class Bloc;
    class ElementaryNode;
    class Node;
    class TypeCode;
    class TypeCodeStruct;
    class TypeCodeObjref;
    class InputDataStreamPort;
    class OutputDataStreamPort;
    class Catalog;
    class CatalogLoader;

    class YACSLIBENGINE_EXPORT Runtime
    {
      friend Runtime* getRuntime() throw(Exception);
    public:
      virtual void init() { }
      virtual void fini() { }

      virtual Catalog* loadCatalog(const std::string& sourceKind,const std::string& path);
      virtual InlineFuncNode* createFuncNode(const std::string& kind,const std::string& name);
      virtual InlineNode* createScriptNode(const std::string& kind,const std::string& name);

      virtual ServiceNode* createRefNode(const std::string& kind,const std::string& name);
      virtual ServiceNode* createCompoNode(const std::string& kind,const std::string& name);
      virtual ServiceInlineNode *createSInlineNode(const std::string& kind, const std::string& name);
      virtual DataNode* createInDataNode(const std::string& kind,const std::string& name);
      virtual DataNode* createOutDataNode(const std::string& kind,const std::string& name);

      virtual ComponentInstance* createComponentInstance(const std::string& name,
                                                         const std::string& kind="");
      virtual Container *createContainer(const std::string& kind="");
      virtual Proc* createProc(const std::string& name);
      virtual Bloc* createBloc(const std::string& name);
      virtual WhileLoop* createWhileLoop(const std::string& name);
      virtual ForLoop* createForLoop(const std::string& name);
      virtual ForEachLoop* createForEachLoop(const std::string& name,TypeCode * type);
      virtual OptimizerLoop* createOptimizerLoop(const std::string& name,const std::string& algLib,
                                                 const std::string& factoryName,bool algInitOnFile,
                                                 const std::string& kind="", Proc * procForTypes = NULL);
      virtual Switch* createSwitch(const std::string& name);

      virtual TypeCode * createInterfaceTc(const std::string& id, const std::string& name,
                                            std::list<TypeCodeObjref *> ltc);
      virtual TypeCode * createSequenceTc(const std::string& id, const std::string& name, TypeCode *content);
      virtual TypeCodeStruct * createStructTc(const std::string& id, const std::string& name);
      
      virtual InputPort* createInputPort(const std::string& name,
                                         const std::string& impl,
                                         Node * node,
                                         TypeCode * type) = 0;

      virtual OutputPort* createOutputPort(const std::string& name,
                                           const std::string& impl,
                                           Node * node,
                                           TypeCode * type) = 0;

      virtual InputDataStreamPort* createInputDataStreamPort(const std::string& name,
                                                             Node * node,
                                                             TypeCode * type);
      virtual OutputDataStreamPort* createOutputDataStreamPort(const std::string& name,
                                                               Node * node,
                                                               TypeCode * type);

      virtual InputPort* adapt(InputPort* source, const std::string& impl, TypeCode * type,
                               bool init=false) throw (ConversionException) = 0;

      virtual InputPort* adapt(InPropertyPort* source,
                               const std::string& impl,
                               TypeCode * type,
                               bool init=false) throw (ConversionException) = 0;

      virtual void* convertNeutral(TypeCode * type, Any *data);
      virtual std::string convertNeutralAsString(TypeCode * type, Any *data);

      virtual void removeRuntime();
      virtual ~Runtime();
    public:
      static const char RUNTIME_ENGINE_INTERACTION_IMPL_NAME[];
      static  YACS::ENGINE::TypeCode *_tc_double;
      static  YACS::ENGINE::TypeCode *_tc_int;
      static  YACS::ENGINE::TypeCode *_tc_bool;
      static  YACS::ENGINE::TypeCode *_tc_string;
      static  YACS::ENGINE::TypeCode *_tc_file;
      static  YACS::ENGINE::TypeCode *_tc_stringpair;
      static  YACS::ENGINE::TypeCode *_tc_propvec;
      virtual void setCatalogLoaderFactory(const std::string& name, CatalogLoader* factory);
      std::map<std::string,CatalogLoader*> _catalogLoaderFactoryMap;
      Catalog* getBuiltinCatalog();
      virtual void addCatalog(Catalog* catalog);
      virtual TypeCode* getTypeCode(const std::string& name);

    protected:
      static Runtime* _singleton;
      Runtime();
      std::set<std::string> _setOfImplementation;
      Catalog* _builtinCatalog;
      std::vector<Catalog*> _catalogs;
    };

  }
}
#endif
