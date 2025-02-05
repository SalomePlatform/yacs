// Copyright (C) 2015-2025  CEA, EDF
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

#include "LinkedBlocPoint.hxx"
#include "ElementaryPoint.hxx"
#include "NotSimpleCasePoint.hxx"
#include "ForkBlocPoint.hxx"
#include "PointVisitor.hxx"
#include "Exception.hxx"

#ifdef WIN32
#include <algorithm>
#endif


using namespace YACS::ENGINE;

LinkedBlocPoint::LinkedBlocPoint(const std::list<AbstractPoint *>& nodes, AbstractPoint *father):BlocPoint(nodes,father)
{
}

AbstractPoint *LinkedBlocPoint::deepCopy(AbstractPoint *father) const
{
  LinkedBlocPoint *ret(new LinkedBlocPoint);
  ret->deepCopyFrom(*this);
  ret->setFather(father);
  return ret;
}

Node *LinkedBlocPoint::getFirstNode()
{
  if(_nodes.empty())
    throw Exception("LinkedBlocPoint::getFirstNode : error no branches !");
  return _nodes.front()->getFirstNode();
}

Node *LinkedBlocPoint::getLastNode()
{
  if(_nodes.empty())
    throw Exception("LinkedBlocPoint::getFirstNode : error no branches !");
  return _nodes.back()->getLastNode();
}

int LinkedBlocPoint::getMaxLevelOfParallelism() const
{
  int ret(0);
  for(std::list<AbstractPoint *>::const_iterator it=_nodes.begin();it!=_nodes.end();it++)
    ret=std::max(ret,(*it)->getMaxLevelOfParallelism());
  return ret;
}

void LinkedBlocPoint::getWeightRegardingDPL(ComplexWeight *weight)
{
  ComplexWeight localWeight;
  for(std::list<AbstractPoint *>::const_iterator it=_nodes.begin();it!=_nodes.end();it++)
  {
    (*it)->getWeightRegardingDPL(&localWeight);
    weight->addWeight(&localWeight);
    localWeight.setToZero();
  }
}

void LinkedBlocPoint::partitionRegardingDPL(const PartDefinition *pd, std::map<ComposedNode *, YACS::BASES::AutoRefCnt<PartDefinition> >& zeMap) const
{
  for(std::list<AbstractPoint *>::const_iterator it=_nodes.begin();it!=_nodes.end();it++)
    (*it)->partitionRegardingDPL(pd,zeMap);
}

std::string LinkedBlocPoint::getRepr() const
{
  std::size_t sz(_nodes.size()),ii(0);
  std::string ret("(");
  for(std::list<AbstractPoint *>::const_iterator it=_nodes.begin();it!=_nodes.end();it++,ii++)
    {
      ret+=(*it)->getRepr();
      if(ii!=sz-1)
        ret+="+";
    }
  ret+=")";
  return ret;
}

AbstractPoint *LinkedBlocPoint::expandNonSimpleCaseOn(NotSimpleCasePoint *pathologicalPt, const std::set<Node *>& uncatchedNodes)
{
  if(anyOf(uncatchedNodes))
    {
      for(auto& it : _nodes)
        {
          AbstractPoint *ret(it->expandNonSimpleCaseOn(pathologicalPt,uncatchedNodes));
          if(ret!=it)
            {
              ret->setFather(this);
              auto oldIt(it);
              it = ret;
              delete oldIt;
            }
        }
      return this;
    }
  else
    {
      std::list<AbstractPoint *> l;
      AbstractPoint *p0(this->deepCopy(getFather())),*p1(pathologicalPt->getUnique()->deepCopy(getFather()));
      l.push_back(p0);
      l.push_back(p1);
      AbstractPoint *ret(new ForkBlocPoint(l,getFather()));
      p0->setFather(ret); p1->setFather(ret);
      return ret;
    }
}

void LinkedBlocPoint::accept(PointVisitor *pv)
{
  pv->beginLinkedBlocPoint(this);
  for(auto it:_nodes)
    it->accept(pv);
  pv->endLinkedBlocPoint(this);
}

LinkedBlocPoint::~LinkedBlocPoint()
{
}
