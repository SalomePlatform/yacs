// Copyright (C) 2015-2021  CEA/DEN, EDF R&D
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

#ifndef __ABSTRACTPOINT_HXX__
#define __ABSTRACTPOINT_HXX__

#include "YACSlibEngineExport.hxx"
#include "PlayGround.hxx"
#include "AutoRefCnt.hxx"

#include <map>
#include <set>
#include <list>
#include <vector>
#include <string>
#include <memory>

namespace YACS
{
  namespace ENGINE
  {
    class Node;
    class Bloc;
    class InGate;
    class OutGate;
    class BlocPoint;
    class ComposedNode;
    class ForkBlocPoint;
    class PointVisitor;
    class LinkedBlocPoint;
    class NotSimpleCasePoint;
    class ElementaryPoint;
    
    class YACSLIBENGINE_EXPORT AbstractPoint
    {
    public:
      AbstractPoint() = default;
      AbstractPoint(AbstractPoint *father):_father(father) { }
      AbstractPoint *getFather() const { return _father; }
      AbstractPoint *getGodFather();
      bool amIGod() { return getGodFather()==nullptr; }
      void setFather(AbstractPoint *father) { _father=father; }
      //
      bool isBegin();
      bool isLast();
      bool isSimplyLinkedBeforeAfter(BlocPoint *sop);
      bool isSimplyLinkedAfterNullBefore(BlocPoint *sop);
      bool isSimplyLinkedBeforeNullAfter(BlocPoint *sop);
      bool isNullBeforeNullAfter(BlocPoint *sop);
      //
      LinkedBlocPoint *tryAsLink(BlocPoint *sop);
      ForkBlocPoint *tryAsFork(BlocPoint *sop);
      ForkBlocPoint *tryAsForkBis(BlocPoint *sop);
      ForkBlocPoint *tryAsForkTer(BlocPoint *sop);
      ForkBlocPoint *tryAsForkQuatro(BlocPoint *sop);
      //void tryAsNotSimpleCase(BlocPoint *sop, std::list<AbstractPoint *>& nodes, bool& somethingDone);
      static void TryAsNotSimpleCase(AbstractPoint *father, const std::vector<AbstractPoint *>& ptsToKill, std::list<AbstractPoint *>& nodes, bool& somethingDone);
      //
      virtual AbstractPoint *deepCopy(AbstractPoint *father) const = 0;
      //
      virtual Node *getFirstNode() = 0;
      virtual Node *getLastNode() = 0;
      virtual AbstractPoint *findPointWithNode(Node *node) = 0;
      virtual bool contains(Node *node) const = 0;
      virtual bool anyOf(const std::set<Node *>& nodes) const = 0;
      virtual int getNumberOfNodes() const = 0;
      virtual int getMaxLevelOfParallelism() const = 0;
      virtual void getWeightRegardingDPL(ComplexWeight *weight) = 0;
      virtual void partitionRegardingDPL(const PartDefinition *pd, std::map<ComposedNode *, YACS::BASES::AutoRefCnt<PartDefinition> >& zeMap) const = 0;
      virtual std::string getRepr() const = 0;
      virtual void accept(PointVisitor *pv) = 0;
      virtual AbstractPoint *expandNonSimpleCaseOn(NotSimpleCasePoint *pathologicalPt, const std::set<Node *>& uncatchedNodes) = 0;
      virtual ~AbstractPoint();
    public:
      static bool IsGatherB4Ext(Node *node);
      bool isSimplyLinkedAfter(BlocPoint *sop, Node *node);
      static bool IsSimplyLinkedAfterExt(Node *node);
      static bool IsScatterAfterExt(Node *node);
      bool isSimplyLinkedBefore(BlocPoint *sop, Node *node);
      static bool IsSimplyLinkedBeforeExt(Node *node);
      static bool IsNoLinksBefore(Node *node);
      static bool IsNoLinksAfter(Node *node);
      static Node *GetNodeB4(Node *node);
      static Node *GetNodeAfter(Node *node);
      static AbstractPoint *GetDirectSonOf(AbstractPoint *refFather, AbstractPoint *sonOrLittleSon);
      static bool IsCommonDirectSonOf(AbstractPoint *refFather, const std::list<OutGate *>& outgs, AbstractPoint *&ret);
      static bool IsCommonDirectSonOf(AbstractPoint *refFather, const std::list<InGate *>& ings, AbstractPoint *&ret);
      static void FeedData(AbstractPoint *ptToBeRewired, std::map< std::string, std::tuple< ElementaryPoint *, Node *, std::shared_ptr<Bloc> > > *m);
      static void FeedData(const std::list<AbstractPoint *>& ptsToBeRewired, std::map< std::string, std::tuple< ElementaryPoint *, Node *, std::shared_ptr<Bloc> > > *m);
      static void Rewire(const std::vector<AbstractPoint *>& ptsToKill, std::map< std::string, std::tuple< ElementaryPoint *, Node *, std::shared_ptr<Bloc> > > *m);
      static void UnRewire(std::map< std::string, std::tuple< ElementaryPoint *, Node *, std::shared_ptr<Bloc> > >& m);
      static void Display(std::map< std::string, std::tuple< ElementaryPoint *, Node *, std::shared_ptr<Bloc> > > *m);
    protected:
      AbstractPoint *_father = nullptr;
    };
  }
}


#endif
