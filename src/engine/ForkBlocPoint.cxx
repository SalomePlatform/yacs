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

#include "ForkBlocPoint.hxx"
#include "PointVisitor.hxx"
#include "Exception.hxx"

#include <algorithm>

using namespace YACS::ENGINE;

ForkBlocPoint::ForkBlocPoint(const std::list<AbstractPoint *>& nodes, AbstractPoint *father):BlocPoint(nodes,father)
{
}

AbstractPoint *ForkBlocPoint::deepCopy(AbstractPoint *father) const
{
  ForkBlocPoint *ret(new ForkBlocPoint);
  ret->deepCopyFrom(*this);
  ret->setFather(father);
  return ret;
}

ForkBlocPoint::~ForkBlocPoint()
{
}

Node *ForkBlocPoint::getFirstNode()
{
  if(_nodes.empty())
    throw Exception("ForkBlocPoint::getFirstNode : error no branches !");
  return _nodes.front()->getFirstNode();
}

Node *ForkBlocPoint::getLastNode()
{
  if(_nodes.empty())
    throw Exception("ForkBlocPoint::getLastNode : error no branches !");
  return _nodes.front()->getLastNode();//not a bug - seen from the outside only first branch exists !
}

int ForkBlocPoint::getMaxLevelOfParallelism() const
{
  int ret(0);
  for(std::list<AbstractPoint *>::const_iterator it=_nodes.begin();it!=_nodes.end();it++)
    ret+=(*it)->getMaxLevelOfParallelism();
  return ret;
}

void ForkBlocPoint::getWeightRegardingDPL(ComplexWeight *weight)
{
  ComplexWeight localWeight;
  for(std::list<AbstractPoint *>::const_iterator it=_nodes.begin();it!=_nodes.end();it++)
  {
    (*it)->getWeightRegardingDPL(&localWeight);
    weight->addWeight(&localWeight);
    localWeight.setToZero();
  }
}

void ForkBlocPoint::partitionRegardingDPL(const PartDefinition *pd, std::map<ComposedNode *, YACS::BASES::AutoRefCnt<PartDefinition> >& zeMap) const
{
  std::vector< std::pair<const PartDefinition *, const ComplexWeight *> > parts;
  std::vector< int> nbCoresPerShot;
  std::vector<std::size_t> v;
  std::vector<ComplexWeight> nodesWeight(_nodes.size());
  std::size_t ii(0);
  for(std::list<AbstractPoint *>::const_iterator it=_nodes.begin();it!=_nodes.end();it++,ii++)
    {
      ComplexWeight *w=&nodesWeight[ii];
      (*it)->getWeightRegardingDPL(w);
      parts.push_back(std::pair<const PartDefinition *, const ComplexWeight *>(pd,w));
      nbCoresPerShot.push_back((*it)->getMaxLevelOfParallelism());
      v.push_back(ii);
    }
  std::vector<AbstractPoint *> nodes2(_nodes.begin(),_nodes.end());
  if(!parts.empty())
    {
      const PlayGround *pg(pd->getPlayGround());
      std::vector< YACS::BASES::AutoRefCnt<PartDefinition> > pds(pg->partition(parts,nbCoresPerShot));
      ii=0;
      for(std::vector<std::size_t>::const_iterator iter=v.begin();iter!=v.end();iter++,ii++)
        nodes2[*iter]->partitionRegardingDPL(pds[ii],zeMap);
    }
}

std::string ForkBlocPoint::getRepr() const
{
  std::size_t sz(_nodes.size()),ii(0);
  std::string ret("[");
  std::vector<std::string> elts(sz);
  for(std::list<AbstractPoint *>::const_iterator it=_nodes.begin();it!=_nodes.end();it++,ii++)
    elts[ii]=(*it)->getRepr();
  std::sort(elts.begin(),elts.end());
  ii=0;
  for(std::list<AbstractPoint *>::const_iterator it=_nodes.begin();it!=_nodes.end();it++,ii++)
    {
      ret+=elts[ii];
      if(ii!=sz-1)
        ret+="*";
    }
  ret+="]";
  return ret;
}

void ForkBlocPoint::accept(PointVisitor *pv)
{
  pv->beginForkBlocPoint(this);
  for(auto it:_nodes)
    it->accept(pv);
  pv->endForkBlocPoint(this);
}

AbstractPoint *ForkBlocPoint::expandNonSimpleCaseOn(NotSimpleCasePoint *pathologicalPt, const std::set<Node *>& uncatchedNodes)
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

