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

#ifndef __VISITOR_SAVESCHEMA_HXX__
#define __VISITOR_SAVESCHEMA_HXX__

#include "YACSlibEngineExport.hxx"
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

    class YACSLIBENGINE_EXPORT VisitorSaveSchema: public Visitor
    {
    public:
      VisitorSaveSchema(ComposedNode *root);
      virtual ~VisitorSaveSchema();
      void openFileSchema(std::string xmlDump) throw(Exception);
      void closeFileSchema();
      virtual void visitBloc(Bloc *node);
      virtual void visitElementaryNode(ElementaryNode *node);
      virtual void visitForEachLoop(ForEachLoop *node);
      virtual void visitOptimizerLoop(OptimizerLoop *node);
      virtual void visitDynParaLoop(DynParaLoop *node);
      virtual void visitForLoop(ForLoop *node);
      virtual void visitInlineNode(InlineNode *node);
      virtual void visitInlineFuncNode(InlineFuncNode *node);
      virtual void visitLoop(Loop *node);
      virtual void visitProc(Proc *node);
      virtual void visitServiceNode(ServiceNode *node);
      virtual void visitServerNode(ServerNode *node);
      virtual void visitServiceInlineNode(ServiceInlineNode *node);
      virtual void visitSwitch(Switch *node);
      virtual void visitWhileLoop(WhileLoop *node);
      virtual void visitPresetNode(DataNode *node);
      virtual void visitOutNode(DataNode *node);
      virtual void visitStudyInNode(DataNode *node);
      virtual void visitStudyOutNode(DataNode *node);

    protected:
      virtual void writeProperties(Node *node);
      virtual void dumpTypeCode(TypeCode* type, std::set<std::string>& typeNames,std::map<std::string, TypeCode*>& typeMap,int depth);
      virtual void writeTypeCodes(Proc *proc);
      virtual void writeContainers(Proc *proc);
      virtual void writeComponentInstances(Proc *proc);
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

    class YACSLIBENGINE_EXPORT SchemaSave
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
