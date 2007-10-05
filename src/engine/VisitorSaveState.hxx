#ifndef __VISITOR_SAVESTATE_HXX__
#define __VISITOR_SAVESTATE_HXX__

#include "Visitor.hxx"
#include "Exception.hxx"

#include <fstream>
#include <string>
#include <map>

namespace YACS
{
  namespace ENGINE
  {
    class VisitorSaveState: public Visitor
    {
    public:
      VisitorSaveState(ComposedNode *root);
      virtual ~VisitorSaveState();
      void openFileDump(std::string xmlDump) throw(Exception);
      void closeFileDump();
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
      std::ofstream _out;
      std::map<int, std::string> _nodeStateName;
    };
  }
}
#endif
