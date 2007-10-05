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
    class InlineNode;
    class InlineFuncNode;
    class Loop;
    class ForLoop;
    class Proc;
    class ServiceNode;
    class ServiceInlineNode;
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
      virtual void visitForLoop(ForLoop *node) = 0;
      virtual void visitInlineNode(InlineNode *node) = 0;
      virtual void visitInlineFuncNode(InlineFuncNode *node) = 0;
      virtual void visitLoop(Loop *node) = 0;
      virtual void visitProc(Proc *node) = 0;
      virtual void visitServiceNode(ServiceNode *node) = 0;
      virtual void visitServiceInlineNode(ServiceInlineNode *node) = 0;
      virtual void visitSwitch(Switch *node) = 0;
      virtual void visitWhileLoop(WhileLoop *node) = 0;

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
