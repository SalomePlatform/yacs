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

#ifndef __VISITOR_HXX__
#define __VISITOR_HXX__

#include <string>
#include <map>
#include <list>

namespace YACS
{
  namespace ENGINE
  {
    class Node;
    class Bloc;
    class ElementaryNode;
    class ComposedNode;
    class ForEachLoop;
    class OptimizerLoop;
    class DynParaLoop;
    class InlineNode;
    class InlineFuncNode;
    class Loop;
    class ForLoop;
    class Proc;
    class ServiceNode;
    class ServerNode;
    class ServiceInlineNode;
    class DataNode;
    class Switch;
    class WhileLoop;
    class TypeCode;
    class TypeCodeObjref;
    class Container;

    class Visitor
    {
    public:
      Visitor(ComposedNode *root);
      virtual void visitBloc(Bloc *node) = 0;
      virtual void visitElementaryNode(ElementaryNode *node) = 0;
      virtual void visitForEachLoop(ForEachLoop *node) = 0;
      virtual void visitOptimizerLoop(OptimizerLoop *node) = 0;
      virtual void visitDynParaLoop(DynParaLoop *node) = 0;
      virtual void visitForLoop(ForLoop *node) = 0;
      virtual void visitInlineNode(InlineNode *node) = 0;
      virtual void visitInlineFuncNode(InlineFuncNode *node) = 0;
      virtual void visitLoop(Loop *node) = 0;
      virtual void visitProc(Proc *node) = 0;
      virtual void visitServiceNode(ServiceNode *node) = 0;
      virtual void visitServerNode(ServerNode *node) = 0;
      virtual void visitServiceInlineNode(ServiceInlineNode *node) = 0;
      virtual void visitSwitch(Switch *node) = 0;
      virtual void visitWhileLoop(WhileLoop *node) = 0;
      virtual void visitPresetNode(DataNode *node) = 0;
      virtual void visitOutNode(DataNode *node) = 0;
      virtual void visitStudyInNode(DataNode *node) = 0;
      virtual void visitStudyOutNode(DataNode *node) = 0;

    protected:
      std::map<std::string, std::string> getNodeProperties(Node *node);
      std::map<std::string, TypeCode*> getTypeCodeMap(Proc *proc);
      std::map<std::string, Container*> getContainerMap(Proc *proc);
      std::list<TypeCodeObjref *> getListOfBases(TypeCodeObjref *objref);

      ComposedNode *_root;
    };
  }
}
#endif
