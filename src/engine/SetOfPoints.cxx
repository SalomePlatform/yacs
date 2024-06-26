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

#include "SetOfPoints.hxx"
#include "BagPoint.hxx"
#include "LinkedBlocPoint.hxx"
#include "ForkBlocPoint.hxx"
#include "ElementaryPoint.hxx"
#include "Exception.hxx"

#include <algorithm>

using namespace YACS::ENGINE;

SetOfPoints::SetOfPoints(const std::list<Node *>& nodes):_bp(0)
{
  std::list<AbstractPoint *> nodes2;
  for(std::list<Node *>::const_iterator it=nodes.begin();it!=nodes.end();it++)
    {
      nodes2.push_back(new ElementaryPoint(*it));
    }
  _bp=new BagPoint(nodes2,0);
}

SetOfPoints::~SetOfPoints()
{
  delete _bp;
}

void SetOfPoints::basicSimplify()
{
  while(_bp->size()>1)
    {
      bool somethingDone(false);
      _bp->deal1(somethingDone);
      if(somethingDone)
        continue;
      _bp->deal2(somethingDone);
      if(somethingDone)
        continue;
      _bp->deal2Bis(somethingDone);
      if(somethingDone)
        continue;
      _bp->deal2Ter(somethingDone);
      if(somethingDone)
        continue;
      _bp->deal2Quatro(somethingDone);
      if(!somethingDone)
        throw Exception("SetOfPoints::simplify : not implemented yet !\nPlease check if there are any recursively redundant links (can be removed by removeRecursivelyRedundantCL method).");
    }
}

void SetOfPoints::simplify()
{
  while( _bp->internalContinueForSimplify() )
    {
      bool somethingDone(false);
      _bp->deal1(somethingDone);
      if(somethingDone)
        continue;
      _bp->deal2(somethingDone);
      if(somethingDone)
        continue;
      _bp->deal2Bis(somethingDone);
      if(somethingDone)
        continue;
      _bp->deal2Ter(somethingDone);
      if(somethingDone)
        continue;
      _bp->deal2Quatro(somethingDone);
      if(somethingDone)
        continue;
      _bp->dealNotSimpleCase(somethingDone);
      if(!somethingDone)
        throw Exception("SetOfPoints::simplify : not implemented yet !\nPlease check if there are any recursively redundant links (can be removed by removeRecursivelyRedundantCL method).");
    }
  if( _bp->presenceOfNonSimpleCase() )
    {
      _bp->expandNonSimpleCase();
    }
}

std::string SetOfPoints::getRepr() const
{
  return _bp->getRepr();
}

AbstractPoint *SetOfPoints::findPointWithNode(Node *node)
{
  if(node==0)
    return 0;
  return _bp->findPointWithNode(node);
}

const std::list<AbstractPoint *>& SetOfPoints::getListOfPoints() const
{
  return _bp->getListOfPoints();
}

int SetOfPoints::getMaxLevelOfParallelism() const
{
  return _bp->getMaxLevelOfParallelism();
}

void SetOfPoints::getWeightRegardingDPL(ComplexWeight *weight)
{
  _bp->getWeightRegardingDPL(weight);
}

void SetOfPoints::partitionRegardingDPL(const PartDefinition *pd, std::map<ComposedNode *, YACS::BASES::AutoRefCnt<PartDefinition> >& zeMap) const
{
  _bp->partitionRegardingDPL(pd,zeMap);
}

AbstractPoint *SetOfPoints::getUniqueAndReleaseIt() const
{
  return _bp->getUniqueAndReleaseIt();
}

void SetOfPoints::accept(PointVisitor *pv)
{
  _bp->accept(pv);
}
