// Copyright (C) 2015-2016  CEA/DEN, EDF R&D
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
#include "SetOfPoints.hxx"
#include "Node.hxx"

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
    return 0;
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
    return 0;
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
    return 0;
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
    return 0;
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
