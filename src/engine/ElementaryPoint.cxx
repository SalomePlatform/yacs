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

#include "ElementaryPoint.hxx"
#include "Node.hxx"

using namespace YACS::ENGINE;

ElementaryPoint::~ElementaryPoint()
{
}

AbstractPoint *ElementaryPoint::findPointWithNode(Node *node)
{
  if(node==_node)
    return this;
  else
    return 0;
}

bool ElementaryPoint::contains(Node *node)
{
  return _node==node;
}

Node *ElementaryPoint::getFirstNode()
{
  return _node;
}

Node *ElementaryPoint::getLastNode()
{
  return _node;
}

int ElementaryPoint::getNumberOfNodes() const
{
  return 1;
}

int ElementaryPoint::getMaxLevelOfParallelism() const
{
  return _node->getMaxLevelOfParallelism();
}

double ElementaryPoint::getWeightRegardingDPL() const
{
  return _node->getWeightRegardingDPL();
}

void ElementaryPoint::partitionRegardingDPL(const PartDefinition *pd, std::map<ComposedNode *, YACS::BASES::AutoRefCnt<PartDefinition> >& zeMap) const
{
  _node->partitionRegardingDPL(pd,zeMap);
}

std::string ElementaryPoint::getRepr() const
{
  return _node->getName();
}
