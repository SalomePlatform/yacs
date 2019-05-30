// Copyright (C) 2019  CEA/DEN, EDF R&D
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

#include "NotSimpleCasePoint.hxx"
#include "PointVisitor.hxx"
#include "Exception.hxx"

#include <algorithm>


using namespace YACS::ENGINE;

NotSimpleCasePoint::NotSimpleCasePoint(const std::list<AbstractPoint *>& nodes, AbstractPoint *father):BlocPoint(nodes,father)
{
}

AbstractPoint *NotSimpleCasePoint::deepCopy(AbstractPoint *father) const
{
  NotSimpleCasePoint *ret(new NotSimpleCasePoint);
  ret->deepCopyFrom(*this);
  ret->setFather(father);
  return ret;
}

Node *NotSimpleCasePoint::getFirstNode()
{
  if(_nodes.empty())
    throw Exception("NotSimpleCasePoint::getFirstNode : error no branches !");
  return _nodes.front()->getFirstNode();
}

Node *NotSimpleCasePoint::getLastNode()
{
  if(_nodes.empty())
    throw Exception("NotSimpleCasePoint::getFirstNode : error no branches !");
  return _nodes.back()->getLastNode();
}

int NotSimpleCasePoint::getMaxLevelOfParallelism() const
{
  int ret(0);
  for(std::list<AbstractPoint *>::const_iterator it=_nodes.begin();it!=_nodes.end();it++)
    ret=std::max(ret,(*it)->getMaxLevelOfParallelism());
  return ret;
}

void NotSimpleCasePoint::getWeightRegardingDPL(ComplexWeight *weight)
{
  ComplexWeight localWeight;
  for(std::list<AbstractPoint *>::const_iterator it=_nodes.begin();it!=_nodes.end();it++)
  {
    (*it)->getWeightRegardingDPL(&localWeight);
    weight->addWeight(&localWeight);
    localWeight.setToZero();
  }
}

void NotSimpleCasePoint::partitionRegardingDPL(const PartDefinition *pd, std::map<ComposedNode *, YACS::BASES::AutoRefCnt<PartDefinition> >& zeMap) const
{
  for(std::list<AbstractPoint *>::const_iterator it=_nodes.begin();it!=_nodes.end();it++)
    (*it)->partitionRegardingDPL(pd,zeMap);
}

std::string NotSimpleCasePoint::getRepr() const
{
  std::size_t sz(_nodes.size()),ii(0);
  std::string ret("|");
  for(std::list<AbstractPoint *>::const_iterator it=_nodes.begin();it!=_nodes.end();it++,ii++)
    {
      ret+=(*it)->getRepr();
      if(ii!=sz-1)
        ret+="^";
    }
  ret+="|";
  return ret;
}

void NotSimpleCasePoint::accept(PointVisitor *pv)
{
  pv->beginNotSimpleCasePoint(this);
  for(auto it:_nodes)
    it->accept(pv);
  pv->endNotSimpleCasePoint(this);
}

AbstractPoint *NotSimpleCasePoint::expandNonSimpleCaseOn(NotSimpleCasePoint *pathologicalPt, const std::set<Node *>& uncatchedNodes)
{
  throw YACS::Exception("NotSimpleCasePoint::expandNonSimpleCaseOn : ooops !");
}
