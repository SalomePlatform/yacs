#ifndef _RUNTIME_HXX_
#define _RUNTIME_HXX_

#include "ConversionException.hxx"

#include<string>
#include<set>
#include<map>

namespace YACS
{
  namespace ENGINE
  {
    class InputPort;
    class OutputPort;
    class ForLoop;
    class ForEachLoop;
    class WhileLoop;
    class Switch;
    class InlineNode;
    class InlineFuncNode;
    class ServiceNode;
    class Container;
    class ServiceInlineNode;
    class ComponentInstance;
    class Proc;
    class Bloc;
    class ElementaryNode;
    class Node;
    class TypeCode;
    class InputDataStreamPort;
    class OutputDataStreamPort;
    class Catalog;
    class CatalogLoader;

    class Runtime
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

      virtual ComponentInstance* createComponentInstance(const std::string& name,
                                                         const std::string& kind="");
      virtual Container *createContainer(const std::string& kind="");
      virtual Proc* createProc(const std::string& name);
      virtual Bloc* createBloc(const std::string& name);
      virtual WhileLoop* createWhileLoop(const std::string& name);
      virtual ForLoop* createForLoop(const std::string& name);
      virtual ForEachLoop* createForEachLoop(const std::string& name,TypeCode * type);
      virtual Switch* createSwitch(const std::string& name);

      
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

      virtual InputPort* adapt(InputPort* source, const std::string& impl, TypeCode * type) throw (ConversionException) = 0;

      virtual void removeRuntime();
      virtual ~Runtime();
    public:
      static const char RUNTIME_ENGINE_INTERACTION_IMPL_NAME[];
      static  YACS::ENGINE::TypeCode *_tc_double;
      static  YACS::ENGINE::TypeCode *_tc_int;
      static  YACS::ENGINE::TypeCode *_tc_bool;
      static  YACS::ENGINE::TypeCode *_tc_string;
      static  YACS::ENGINE::TypeCode *_tc_file;
      virtual void setCatalogLoaderFactory(const std::string& name, CatalogLoader* factory);
      std::map<std::string,CatalogLoader*> _catalogLoaderFactoryMap;
      Catalog* getBuiltinCatalog();

    protected:
      static Runtime* _singleton;
      Runtime();
      std::set<std::string> _setOfImplementation;
      Catalog* _builtinCatalog;
    };

    Runtime* getRuntime() throw(Exception);
  }
}
#endif
