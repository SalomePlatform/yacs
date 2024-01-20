// Copyright (C) 2015-2024  CEA, EDF
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

#include "AbstractPoint.hxx"
#include "LinkedBlocPoint.hxx"
#include "ForkBlocPoint.hxx"
#include "NotSimpleCasePoint.hxx"
#include "SetOfPoints.hxx"
#include "BagPoint.hxx"
#include "ElementaryPoint.hxx"
#include "Node.hxx"
#include "Bloc.hxx"

#include <algorithm>

using namespace YACS::ENGINE;

AbstractPoint::~AbstractPoint()
{
}

AbstractPoint *AbstractPoint::getGodFather()
{
  if(_father==0)
    return this;
  else
    return _father->getGodFather();
}

bool AbstractPoint::isBegin()
{
  Node *beg(getFirstNode());
  InGate *ing(beg->getInGate());
  return ing->getBackLinks().empty();
}

bool AbstractPoint::isLast()
{
  Node *endd(getLastNode());
  OutGate *oug(endd->getOutGate());
  return oug->edSetInGate().empty();
}

bool AbstractPoint::isSimplyLinkedBeforeAfter(BlocPoint *sop)
{
  Node *beg(getFirstNode()),*endd(getLastNode());
  return isSimplyLinkedBefore(sop,beg) && isSimplyLinkedAfter(sop,endd);
}

bool AbstractPoint::isSimplyLinkedAfterNullBefore(BlocPoint *sop)
{
  Node *beg(getFirstNode()),*endd(getLastNode());
  return IsNoLinksBefore(beg) && isSimplyLinkedAfter(sop,endd);
}

bool AbstractPoint::isSimplyLinkedBeforeNullAfter(BlocPoint *sop)
{
  Node *beg(getFirstNode()),*endd(getLastNode());
  return IsNoLinksAfter(endd) && isSimplyLinkedBefore(sop,beg);
}

bool AbstractPoint::isNullBeforeNullAfter(BlocPoint *sop)
{
  Node *beg(getFirstNode()),*endd(getLastNode());
  return IsNoLinksBefore(beg) && IsNoLinksAfter(endd);
}

/*!
 * precondition : isSimplyLinkedBeforeAfter must return true on \a this.
 */
LinkedBlocPoint *AbstractPoint::tryAsLink(BlocPoint *sop)
{
  Node *bb(getFirstNode()),*ee(getLastNode());
  std::list<AbstractPoint *> l; l.push_back(this);
  AbstractPoint *cur2(0);
  //
  cur2=sop->getNodeB4(bb);
  while(cur2)
    {
      if(dynamic_cast<NotSimpleCasePoint *>(cur2))
        continue;
      Node *cur3(cur2->getFirstNode());
      if(cur2->isSimplyLinkedBeforeAfter(sop))
        {
          l.push_front(cur2);
          cur2=sop->getNodeB4(cur3);
          continue;
        }
      else if(IsGatherB4Ext(cur3) && isSimplyLinkedAfter(sop,cur2->getLastNode()))
        {
          l.push_front(cur2);
          break;
        }
      else
        break;
    }
  //
  cur2=sop->getNodeAfter(ee);
  while(cur2)
    {
      if(dynamic_cast<NotSimpleCasePoint *>(cur2))
        continue;
      Node *cur3(cur2->getLastNode());
      if(cur2->isSimplyLinkedBeforeAfter(sop))
        {
          l.push_back(cur2);
          cur2=sop->getNodeAfter(cur3);
          continue;
        }
      else if(IsScatterAfterExt(cur3) && isSimplyLinkedBefore(sop,cur2->getFirstNode()))
        {
          l.push_back(cur2);
          break;
        }
      else
        break;
    }
  if(l.size()>1)
    {
      return new LinkedBlocPoint(l,getFather());
    }
  else
    return nullptr;
}

/*!
 * precondition : isSimplyLinkedBeforeAfter must return true on \a this.
 */
ForkBlocPoint *AbstractPoint::tryAsFork(BlocPoint *sop)
{
  Node *bb(GetNodeB4(getFirstNode())),*ee(GetNodeAfter(getLastNode()));
  AbstractPoint *bb2(sop->findPointWithNode(bb)),*ee2(sop->findPointWithNode(ee));
  //
  const std::list<AbstractPoint *>& lp(sop->getListOfPoints());
  std::list<AbstractPoint *> l; l.push_back(this);
  for(std::list<AbstractPoint *>::const_iterator it=lp.begin();it!=lp.end();it++)
    {
      if(*it==this)
        continue;
      if(dynamic_cast<NotSimpleCasePoint *>(*it))
        continue;
      Node *curFirst((*it)->getFirstNode()),*curEnd((*it)->getLastNode());
      if(!IsSimplyLinkedBeforeExt(curFirst) || !IsSimplyLinkedAfterExt(curEnd))
        continue;
      Node *curbb(GetNodeB4(curFirst)),*curee(GetNodeAfter(curEnd));
      AbstractPoint *bb3(sop->findPointWithNode(curbb)),*ee3(sop->findPointWithNode(curee));
      if(bb2==bb3 && ee2==ee3)
        l.push_back(*it);
    }
  if(l.size()>1)
    {
      return new ForkBlocPoint(l,getFather());
    }
  else
    return nullptr;
}

ForkBlocPoint *AbstractPoint::tryAsForkBis(BlocPoint *sop)
{
  Node *bb(GetNodeB4(getFirstNode())),*ee(GetNodeAfter(getLastNode()));
  AbstractPoint *ee2(sop->findPointWithNode(ee));
  //
  const std::list<AbstractPoint *>& lp(sop->getListOfPoints());
  std::list<AbstractPoint *> l; l.push_back(this);
  for(std::list<AbstractPoint *>::const_iterator it=lp.begin();it!=lp.end();it++)
    {
      if(*it==this)
        continue;
      if(dynamic_cast<NotSimpleCasePoint *>(*it))
        continue;
      Node *curFirst((*it)->getFirstNode()),*curEnd((*it)->getLastNode());
      if(!IsNoLinksBefore(curFirst) || !IsSimplyLinkedAfterExt(curEnd))
        continue;
      Node *curee(GetNodeAfter(curEnd));
      AbstractPoint *ee3(sop->findPointWithNode(curee));
      if(ee2==ee3)
        l.push_back(*it);
    }
  if(l.size()>1)
    {
      return new ForkBlocPoint(l,getFather());
    }
  else
    return nullptr;
}

ForkBlocPoint *AbstractPoint::tryAsForkTer(BlocPoint *sop)
{
  Node *bb(GetNodeB4(getFirstNode())),*ee(GetNodeAfter(getLastNode()));
  AbstractPoint *bb2(sop->findPointWithNode(bb));
  //
  const std::list<AbstractPoint *>& lp(sop->getListOfPoints());
  std::list<AbstractPoint *> l; l.push_back(this);
  for(std::list<AbstractPoint *>::const_iterator it=lp.begin();it!=lp.end();it++)
    {
      if(*it==this)
        continue;
      if(dynamic_cast<NotSimpleCasePoint *>(*it))
        continue;
      Node *curFirst((*it)->getFirstNode()),*curEnd((*it)->getLastNode());
      if(!IsSimplyLinkedBeforeExt(curFirst) || !IsNoLinksAfter(curEnd))
        continue;
      Node *curbb(GetNodeB4(curFirst));
      AbstractPoint *bb3(sop->findPointWithNode(curbb));
      if(bb2==bb3)
        l.push_back(*it);
    }
  if(l.size()>1)
    {
      return new ForkBlocPoint(l,getFather());
    }
  else
    return nullptr;
}

ForkBlocPoint *AbstractPoint::tryAsForkQuatro(BlocPoint *sop)
{
  Node *bb(GetNodeB4(getFirstNode())),*ee(GetNodeAfter(getLastNode()));
  AbstractPoint *bb2(sop->findPointWithNode(bb));
  //
  const std::list<AbstractPoint *>& lp(sop->getListOfPoints());
  std::list<AbstractPoint *> l; l.push_back(this);
  for(std::list<AbstractPoint *>::const_iterator it=lp.begin();it!=lp.end();it++)
    {
      if(*it==this)
        continue;
      if(dynamic_cast<NotSimpleCasePoint *>(*it))
        continue;
      Node *curFirst((*it)->getFirstNode()),*curEnd((*it)->getLastNode());
      if(!IsNoLinksBefore(curFirst) || !IsNoLinksAfter(curEnd))
        continue;
      Node *curbb(GetNodeB4(curFirst));
      AbstractPoint *bb3(sop->findPointWithNode(curbb));
      if(bb2==bb3)
        l.push_back(*it);
    }
  if(l.size()>1)
    {
      return new ForkBlocPoint(l,getFather());
    }
  else
    return nullptr;
}

#include <iostream>

class Visitor1 : public YACS::ENGINE::PointVisitor
{
public:
  Visitor1(std::map< std::string, std::tuple< ElementaryPoint *, Node *, std::shared_ptr<Bloc> > > *m):_m(m) { }
  void beginForkBlocPoint(ForkBlocPoint *pt) { }
  void endForkBlocPoint(ForkBlocPoint *pt) { }
  void beginLinkedBlocPoint(LinkedBlocPoint *pt) { }
  void endLinkedBlocPoint(LinkedBlocPoint *pt) { }
  void beginElementaryPoint(ElementaryPoint *pt)
  {
    std::string nodeName(pt->getNodeName());
    auto it(_m->find(nodeName));
    if(it==_m->end())
      {
        (*_m)[nodeName] = std::tuple< ElementaryPoint *, Node *, std::shared_ptr<Bloc> >(pt,pt->getNode(),std::make_shared<Bloc>(nodeName));
      }
  }
  void endElementaryPoint(ElementaryPoint *pt) { }
  void beginNotSimpleCasePoint(NotSimpleCasePoint *pt) { }
  void endNotSimpleCasePoint(NotSimpleCasePoint *pt) { }
private:
  std::map< std::string, std::tuple< ElementaryPoint *, Node *, std::shared_ptr<Bloc> > > *_m;
};

/*!
 * Feed m with all ElementaryPoints inside \a ptToBeRewired.
 */
void AbstractPoint::FeedData(AbstractPoint *ptToBeRewired, std::map< std::string, std::tuple< ElementaryPoint *, Node *, std::shared_ptr<Bloc> > > *m)
{
  Visitor1 vis(m);
  ptToBeRewired->accept(&vis);
}

/*!
 * Feed m with all ElementaryPoints inside \a ptsToBeRewired.
 */
void AbstractPoint::FeedData(const std::list<AbstractPoint *>& ptsToBeRewired, std::map< std::string, std::tuple< ElementaryPoint *, Node *, std::shared_ptr<Bloc> > > *m)
{
  for(auto it : ptsToBeRewired)
    FeedData(it,m);
}

bool containsPtsToKill(const std::vector<AbstractPoint *>& ptsToKill, Node *node)
{
  for(auto it : ptsToKill)
    if(it->contains(node))
      return true;
  return false;
}

/*!
 * This method go throw all ElementaryPoints of \a m. 
 * For each ElementaryPoint change the Node instance underneath. And then create links between those new Node by excluding all links going whose destination :
 * - is inside \a ptToKill
 * - is not refered by \a m
 * 
 * CF links of old nodes are NOT touched for unrewire phase.
 * Typically all ElementaryPoints inside \a ptToKill are NOT into \a m.
 */
void AbstractPoint::Rewire(const std::vector<AbstractPoint *>& ptsToKill, std::map< std::string, std::tuple< ElementaryPoint *, Node *, std::shared_ptr<Bloc> > > *m)
{
  for(auto it : *m)
    {
      ElementaryPoint *pt(std::get<0>(it.second));
      Node *node(std::get<1>(it.second));
      auto newNode(std::get<2>(it.second).get());
      pt->setNode(newNode);
    }
  for(auto it : *m)
    {
      ElementaryPoint *pt(std::get<0>(it.second));
      Node *node(std::get<1>(it.second));
      auto newNode(std::get<2>(it.second).get());
      if(containsPtsToKill(ptsToKill,newNode))
        continue;
      for(auto it2 : node->getOutGate()->edSetInGate())
        {
          Node *nodeFwd(it2->getNode());
          std::string nodeFwdName(nodeFwd->getName());
          auto it3(m->find(nodeFwdName));
          if(it3!=m->end())
            {
              Node *nodeFwdNew = std::get<2>(it3->second).get();
              if(!containsPtsToKill(ptsToKill,newNode))
                {
                  newNode->getOutGate()->edAddInGate(nodeFwdNew->getInGate());
                }
              //else
              // node after nodeFwd is not in m fall into \a ptToKill
              // -> ignore link.
            }
          //else
          // node after nodeFwd is not in m. Typically because nodeFwd has not been put in m
          // concretely : do not link incoming links to Node * inside ptToKill
        }
    }
}

/*!
 * Unrewire consists into replacing newly created nodes into old one in ElementaryPoints contained in m.
 * As CF links of old ENGINE::Node * has not be touched the CF graph is the same.
 */
void AbstractPoint::UnRewire(std::map< std::string, std::tuple< ElementaryPoint *, Node *, std::shared_ptr<Bloc> > >& m)
{
  for(auto it : m)
    {
      ElementaryPoint *pt(std::get<0>(it.second));
      Node *node(std::get<1>(it.second));
      pt->setNode(node);
    }
}

void AbstractPoint::Display(std::map< std::string, std::tuple< ElementaryPoint *, Node *, std::shared_ptr<Bloc> > > *m)
{
  for(auto it : *m)
    {
      ElementaryPoint *pt(std::get<0>(it.second));
      auto newNode(pt->getNode());
      for(auto it2 : newNode->getOutGate()->edSetInGate())
        {
          std::cerr << pt->getNodeName() << " -> " << it2->getNode()->getName() << " " << newNode->typeName() << std::endl;
        }
    }
}

/*!
 * This methods tries to deal with \a sop that can t be considered as a composite of Link, Fork.
 * 
 * Do do so, this methods tries to exclude \a this from \a sop and then analyze if the graph can be decomposited into Links and Forks. If yes \a somethingDone is set to true and \a nodes is updated in consequence.
 * 
 * If the algorithm fails \a nodes are let untouched and \a somethingDone is let false.
 *
 */
void AbstractPoint::TryAsNotSimpleCase(AbstractPoint *father, const std::vector<AbstractPoint *>& ptsToKill, std::list<AbstractPoint *>& nodes, bool& somethingDone)
{
  std::list<AbstractPoint *> lp2;
  for(auto it : nodes)
    {
      if(std::find(ptsToKill.cbegin(),ptsToKill.cend(),it)==ptsToKill.cend())
        lp2.push_back(it->deepCopy(nullptr));
    }
  BagPoint *tmp(new BagPoint(lp2,nullptr));
  for(auto it : lp2)
    it->setFather(tmp);
  SetOfPoints sopTest(tmp);
  std::map< std::string, std::tuple< ElementaryPoint *, Node *, std::shared_ptr<Bloc> > > m;
  FeedData(tmp,&m);
  Rewire(ptsToKill,&m);
  try
    {
      sopTest.basicSimplify();
    }
  catch(YACS::Exception& ex)
    {// By removing elements in ptsToKill from this impossible to have a classical case -> un rewire to rollback nodes state
      UnRewire(m);
      return ;
    }
  AbstractPoint *pt(sopTest.getUniqueAndReleaseIt());
  pt->setFather(father);
  UnRewire(m);
  for(auto it : nodes)
    if(std::find(ptsToKill.cbegin(),ptsToKill.cend(),it)==ptsToKill.cend())
      delete it;
  nodes.clear();
  nodes.push_back(pt);
  for(auto it : ptsToKill)
    {
      std::list<AbstractPoint *> l; l.push_back(it);
      nodes.push_back( new NotSimpleCasePoint(l,father) );
    }
  somethingDone = true;
}

bool AbstractPoint::IsGatherB4Ext(Node *node)
{
  InGate *ing(node->getInGate());
  return ing->getBackLinks().size()!=1;
}

bool AbstractPoint::isSimplyLinkedAfter(BlocPoint *sop, Node *node)
{
  OutGate *oug(node->getOutGate());
  std::list<InGate *> ings(oug->edSetInGate());
  if(ings.size()==1)
    return true;
  if(ings.size()==0)
    return false;
  AbstractPoint *dummy=0;
  return IsCommonDirectSonOf(sop,ings,dummy);
}

bool AbstractPoint::IsSimplyLinkedAfterExt(Node *node)
{
  OutGate *oug(node->getOutGate());
  return oug->edSetInGate().size()<=1;
}

bool AbstractPoint::IsScatterAfterExt(Node *node)
{
  OutGate *oug(node->getOutGate());
  return oug->edSetInGate().size()!=1;
}

bool AbstractPoint::isSimplyLinkedBefore(BlocPoint *sop, Node *node)
{
  InGate *ing(node->getInGate());
  std::list<OutGate *> outgs(ing->getBackLinks());
  if(outgs.size()==1)
    return true;
  if(outgs.size()==0)
    return false;
  AbstractPoint *dummy=0;
  return IsCommonDirectSonOf(sop,outgs,dummy);
}

bool AbstractPoint::IsSimplyLinkedBeforeExt(Node *node)
{
  InGate *ing(node->getInGate());
  return ing->getBackLinks().size()<=1;
}

bool AbstractPoint::IsNoLinksBefore(Node *node)
{
  InGate *ing(node->getInGate());
  return ing->getBackLinks().size()==0;
}

bool AbstractPoint::IsNoLinksAfter(Node *node)
{
  OutGate *oug(node->getOutGate());
  return oug->edSetInGate().size()==0;
}

Node *AbstractPoint::GetNodeB4(Node *node)
{
  InGate *ing(node->getInGate());
  std::list<OutGate *> bl(ing->getBackLinks());
  if(bl.size()>1)
    throw Exception("AbstractPoint::GetNodeB4 : precond not OK !");
  if(bl.size()==0)
    return 0;
  return bl.front()->getNode();
}

Node *AbstractPoint::GetNodeAfter(Node *node)
{
  OutGate *oug(node->getOutGate());
  std::list<InGate *> fl(oug->edSetInGate());
  if(fl.size()>1)
    throw Exception("AbstractPoint::GetNodeAfter : precond not OK !");
  if(fl.size()==0)
    return 0;
  return (*fl.begin())->getNode();
}

AbstractPoint *AbstractPoint::GetDirectSonOf(AbstractPoint *refFather, AbstractPoint *sonOrLittleSon)
{
  if(!sonOrLittleSon)
    throw YACS::Exception("AbstractPoint::GetDirectSonOf : sonOrLittleSon is null !");
  AbstractPoint *curFath(sonOrLittleSon->getFather()),*cur(sonOrLittleSon);
  while(curFath && curFath!=refFather)
    {
      cur=curFath;
      curFath=cur->getFather();
    }
  if(!curFath)
    throw YACS::Exception("AbstractPoint::GetDirectSonOf : not in the same family !");
  return cur;
}

bool AbstractPoint::IsCommonDirectSonOf(AbstractPoint *refFather, const std::list<OutGate *>& outgs, AbstractPoint *&ret)
{
  if(outgs.size()<1)
    throw YACS::Exception("AbstractPoint::GetCommonDirectSonOf1 : not enough !");
  std::list<OutGate *>::const_iterator it(outgs.begin());
  OutGate *ref(*(it++));
  AbstractPoint *ref2(GetDirectSonOf(refFather,refFather->findPointWithNode(ref->getNode())));
  for(;it!=outgs.end();it++)
    {
      if(!ref2->contains((*it)->getNode()))
        return false;
    }
  ret=ref2;
  return true;
}

bool AbstractPoint::IsCommonDirectSonOf(AbstractPoint *refFather, const std::list<InGate *>& ings, AbstractPoint *&ret)
{
  if(ings.size()<1)
    throw YACS::Exception("AbstractPoint::GetCommonDirectSonOf2 : not enough !");
  std::list<InGate *>::const_iterator it(ings.begin());
  InGate *ref(*(it++));
  AbstractPoint *ref2(GetDirectSonOf(refFather,refFather->findPointWithNode(ref->getNode())));
  for(;it!=ings.end();it++)
    {
      if(!ref2->contains((*it)->getNode()))
        return false;
    }
  ret=ref2;
  return true;
}
