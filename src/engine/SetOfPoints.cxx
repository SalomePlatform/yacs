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
  if(!_bp)
    return;
  delete _bp;
}

void SetOfPoints::simplify()
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
      if(!somethingDone)
        throw Exception("SetOfPoints::simplify : not implemented yet !");
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
