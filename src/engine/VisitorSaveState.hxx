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

#ifndef __VISITOR_SAVESTATE_HXX__
#define __VISITOR_SAVESTATE_HXX__

#include "YACSlibEngineExport.hxx"
#include "Visitor.hxx"
#include "Exception.hxx"

#include <fstream>
#include <string>
#include <map>

namespace YACS
{
  namespace ENGINE
  {
    class YACSLIBENGINE_EXPORT VisitorSaveState: public Visitor
    {
    public:
      VisitorSaveState(ComposedNode *root);
      virtual ~VisitorSaveState();
      void openFileDump(const std::string& xmlDump) throw(Exception);
      void closeFileDump();
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
      std::ofstream _out;
      std::map<int, std::string> _nodeStateName;
    };
  }
}
#endif
