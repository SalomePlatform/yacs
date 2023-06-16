// Copyright (C) 2006-2023  CEA/DEN, EDF R&D
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

#include "Bloc.hxx"
#include "Proc.hxx"
#include "Visitor.hxx"
#include "ForEachLoop.hxx"
#include "InlineNode.hxx"
#include "ServiceNode.hxx"
#include "ServerNode.hxx"
#include "HomogeneousPoolContainer.hxx"

using namespace YACS::ENGINE;

void Bloc::fitToPlayGround(const PlayGround *pg)
{
  static const char MSG[]="Bloc::fitToPlayGround : Not implemented yet for this type of node !";
  class MyVisitor : public Visitor
    {
    public:
      MyVisitor(ComposedNode *root):Visitor(root),_lev(0),_max_lev(0) { }
      void visitBloc(Bloc *node) { node->ComposedNode::accept(this); }
      void visitElementaryNode(ElementaryNode *node) { }
      void visitForEachLoop(ForEachLoop *node)
      {
        _max_lev=std::max(_max_lev,_lev);
        {
          _lev++;
          node->ComposedNode::accept(this);
          _lev--;
        }
        node->edGetNbOfBranchesPort()->edInit(1);
        if(_lev==_max_lev)
          {
          _fes.push_back(node);// locate all leaves ForEach
          }
        if(_lev==0)
          _max_lev=0;
        }
      void visitForEachLoopDyn(ForEachLoopDyn *node) { throw YACS::Exception(MSG); }
      void visitOptimizerLoop(OptimizerLoop *node) { throw YACS::Exception(MSG); }
      void visitDynParaLoop(DynParaLoop *node) { throw YACS::Exception(MSG); }
      void visitForLoop(ForLoop *node) { throw YACS::Exception(MSG); }
      void visitInlineNode(InlineNode *node)
      {
          Container *cont(node->getContainer());
          HomogeneousPoolContainer *cont2(dynamic_cast<HomogeneousPoolContainer *>(cont));
          if(!cont2)
            return ;
          _cont.push_back(cont2);
          _cont2.insert(cont2);
      }
      void visitInlineFuncNode(InlineFuncNode *node) { throw YACS::Exception(MSG); }
      void visitLoop(Loop *node) { throw YACS::Exception(MSG); }
      void visitProc(Proc *node) { node->ComposedNode::accept(this); }
      void visitServiceNode(ServiceNode *node) { throw YACS::Exception(MSG); }
      void visitServerNode(ServerNode *node) { throw YACS::Exception(MSG); }
      void visitServiceInlineNode(ServiceInlineNode *node) { throw YACS::Exception(MSG); }
      void visitSwitch(Switch *node) { throw YACS::Exception(MSG); }
      void visitWhileLoop(WhileLoop *node) { throw YACS::Exception(MSG); }
      void visitPresetNode(DataNode *node) { throw YACS::Exception(MSG); }
      void visitOutNode(DataNode *node) { throw YACS::Exception(MSG); }
      void visitStudyInNode(DataNode *node) { throw YACS::Exception(MSG); }
      void visitStudyOutNode(DataNode *node) { throw YACS::Exception(MSG); }
    public:
      std::list<ForEachLoop *> _fes;
      std::list< HomogeneousPoolContainer *> _cont;
      std::set< HomogeneousPoolContainer * > _cont2;
      int _lev;
      int _max_lev;
  };
  YACS::BASES::AutoRefCnt<PartDefinition> pd(new AllPartDefinition(pg));
  std::map<ComposedNode *,YACS::BASES::AutoRefCnt<PartDefinition> > zeMap;
  MyVisitor vis(this);
  this->accept(&vis);
  for(std::list<ForEachLoop *>::const_iterator it=vis._fes.begin();it!=vis._fes.end();it++)
    (*it)->edGetNbOfBranchesPort()->edInit(1);
  this->removeRecursivelyRedundantCL();
  if (this->getMaxLevelOfParallelism() > pg->getNumberOfCoresAvailable())
    throw YACS::Exception("Bloc::fitToPlayGround : Not enough cores available to run the calculation !");
  this->partitionRegardingDPL(pd,zeMap);
  this->accept(&vis);
  for(std::list<ForEachLoop *>::const_iterator it=vis._fes.begin();it!=vis._fes.end();it++)
    {
      std::map<ComposedNode *,YACS::BASES::AutoRefCnt<PartDefinition> >::iterator it2(zeMap.find(*it));
      if(it2==zeMap.end())
        throw YACS::Exception("Bloc::fitToPlayGround : internal error !");
      int maxLev((*it)->getExecNode()->getMaxLevelOfParallelism());
      int a((*it2).second->getNumberOfCoresConsumed());
      int res(a/maxLev);
      (*it)->edGetNbOfBranchesPort()->edInit(res);
    }
  for(std::set< HomogeneousPoolContainer * >::const_iterator it=vis._cont2.begin();it!=vis._cont2.end();it++)
    (*it)->setSizeOfPool(pg->getNumberOfWorkers((*it)->getNumberOfCoresPerWorker()));
  //FIXME
}

constexpr char MSG[]="Bloc::propagePlayGround : Not implemented yet for this type of node !";
class MyVisitorPropagate : public Visitor
  {
  public:
    MyVisitorPropagate(ComposedNode *root):Visitor(root) { }
    void visitBloc(Bloc *node) { node->ComposedNode::accept(this); }
    void visitElementaryNode(ElementaryNode *node) { }
    void visitForEachLoop(ForEachLoop *node) { node->ComposedNode::accept(this); }
    void visitForEachLoopDyn(ForEachLoopDyn *node) { node->ComposedNode::accept(this); }
    void visitOptimizerLoop(OptimizerLoop *node) { throw YACS::Exception(MSG); }
    void visitDynParaLoop(DynParaLoop *node) { throw YACS::Exception(MSG); }
    void visitForLoop(ForLoop *node) { throw YACS::Exception(MSG); }
    template<class NodeClass>
    void visitNodeWithContainer(NodeClass *node)
    {
        Container *cont(node->getContainer());
        HomogeneousPoolContainer *cont2(dynamic_cast<HomogeneousPoolContainer *>(cont));
        if(!cont2)
          return ;
        _cont2.insert(cont2);
    }
    void visitInlineNode(InlineNode *node) { this->visitNodeWithContainer<InlineNode>(node); }
    void visitInlineFuncNode(InlineFuncNode *node) { visitInlineNode(node); }
    void visitLoop(Loop *node) { throw YACS::Exception(MSG); }
    void visitProc(Proc *node) { node->ComposedNode::accept(this); }
    void visitServiceNode(ServiceNode *node) { this->visitNodeWithContainer<ServiceNode>(node); }
    void visitServerNode(ServerNode *node) { visitInlineNode(node); }
    void visitServiceInlineNode(ServiceInlineNode *node) { throw YACS::Exception(MSG); }
    void visitSwitch(Switch *node) { throw YACS::Exception(MSG); }
    void visitWhileLoop(WhileLoop *node) { throw YACS::Exception(MSG); }
    void visitPresetNode(DataNode *node) { throw YACS::Exception(MSG); }
    void visitOutNode(DataNode *node) { throw YACS::Exception(MSG); }
    void visitStudyInNode(DataNode *node) { throw YACS::Exception(MSG); }
    void visitStudyOutNode(DataNode *node) { throw YACS::Exception(MSG); }
  public:
    std::set< HomogeneousPoolContainer * > _cont2;
};

void Bloc::propagePlayGround(const PlayGround *pg)
{
  MyVisitorPropagate vis(this);
  this->accept(&vis);
  for(auto cont : vis._cont2)
    cont->assignPG(pg);
}