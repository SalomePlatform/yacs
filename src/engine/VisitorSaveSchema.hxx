#ifndef __VISITOR_SAVESCHEMA_HXX__
#define __VISITOR_SAVESCHEMA_HXX__

#include "Visitor.hxx"
#include "Exception.hxx"

#include <fstream>
#include <string>
#include <map>
#include <set>

namespace YACS
{
  namespace ENGINE
  {
    class OutputPort;
    class OutputDataStreamPort;
    class InPort;
    class ComponentInstance;

    struct DataLinkInfo
    {
      Node* from;
      Node* to;
      OutputPort* outp;
      InPort* inp;
      bool toDelete;
    };

    struct StreamLinkInfo
    {
      Node* from;
      Node* to;
      OutputDataStreamPort* outp;
      InPort* inp;
      bool toDelete;
    };

    class VisitorSaveSchema: public Visitor
    {
    public:
      VisitorSaveSchema(ComposedNode *root);
      virtual ~VisitorSaveSchema();
      void openFileSchema(std::string xmlDump) throw(Exception);
      void closeFileSchema();
      virtual void visitBloc(Bloc *node);
      virtual void visitElementaryNode(ElementaryNode *node);
      virtual void visitForEachLoop(ForEachLoop *node);
      virtual void visitForLoop(ForLoop *node);
      virtual void visitInlineNode(InlineNode *node);
      virtual void visitInlineFuncNode(InlineFuncNode *node);
      virtual void visitLoop(Loop *node);
      virtual void visitProc(Proc *node);
      virtual void visitServiceNode(ServiceNode *node);
      virtual void visitServiceInlineNode(ServiceInlineNode *node);
      virtual void visitSwitch(Switch *node);
      virtual void visitWhileLoop(WhileLoop *node);

    protected:
      virtual void writeProperties(Node *node);
      virtual void dumpTypeCode(TypeCode* type, std::set<std::string>& typeNames,std::map<std::string, TypeCode*>& typeMap,int depth);
      virtual void writeTypeCodes(Proc *proc);
      virtual void writeContainers(Proc *proc);
      virtual void writeInputPorts(Node *node);
      virtual void writeInputDataStreamPorts(Node *node); // OCC : mkr : add possibility to write input data stream ports
      virtual void writeOutputPorts(Node *node);
      virtual void writeOutputDataStreamPorts(Node *node); // OCC : mkr : add possibility to write output data stream ports
      virtual void writeControls(ComposedNode *node);
      virtual void writeSimpleDataLinks(ComposedNode *node);
      virtual void writeSimpleStreamLinks(ComposedNode *node); // OCC : mkr : add possibility to write stream links
      virtual void writeParameters(Proc *proc);
      virtual void writeParametersNode(ComposedNode *proc, Node *node);
      virtual void beginCase(Node* node);
      virtual void endCase(Node* node);
      std::set<Node*> getAllNodes(ComposedNode *node);
      int depthNode(Node* node);
      inline std::string indent(int val)
      {std::string white; white.append(3*val,' '); return white;};

      std::ofstream _out;
      std::map<int, std::string> _nodeStateName;
      std::map<std::string, Container*> _containerMap;
      std::map<ComponentInstance*, std::string> _componentInstanceMap;
      ComposedNode *_root;
      std::multimap<int, DataLinkInfo> _mapOfDLtoCreate;
      std::multimap<int, StreamLinkInfo> _mapOfSLtoCreate;
    };

    class SchemaSave
    {
    public:
      SchemaSave(Proc* proc);
      virtual void save(std::string xmlSchemaFile);
    protected:
      Proc* _p;
    };
  }
}
#endif
