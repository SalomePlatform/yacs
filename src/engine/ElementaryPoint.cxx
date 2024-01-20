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

#include "ElementaryPoint.hxx"
#include "PointVisitor.hxx"
#include "NotSimpleCasePoint.hxx"
#include "ForkBlocPoint.hxx"
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
    return nullptr;
}

bool ElementaryPoint::contains(Node *node) const
{
  return _node==node;
}

bool ElementaryPoint::anyOf(const std::set<Node *>& nodes) const
{
  return nodes.find(_node)!=nodes.end();
}

AbstractPoint *ElementaryPoint::deepCopy(AbstractPoint *father) const
{
  AbstractPoint *ret(new ElementaryPoint(_node));
  ret->setFather(father);
  return ret;
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

void ElementaryPoint::getWeightRegardingDPL(ComplexWeight *weight)
{
  _node->getWeightRegardingDPL(weight);
}

void ElementaryPoint::partitionRegardingDPL(const PartDefinition *pd, std::map<ComposedNode *, YACS::BASES::AutoRefCnt<PartDefinition> >& zeMap) const
{
  _node->partitionRegardingDPL(pd,zeMap);
}

std::string ElementaryPoint::getRepr() const
{
  return _node->getName();
}

void ElementaryPoint::accept(PointVisitor *pv)
{
  pv->beginElementaryPoint(this);
  pv->endElementaryPoint(this);
}

AbstractPoint *ElementaryPoint::expandNonSimpleCaseOn(NotSimpleCasePoint *pathologicalPt, const std::set<Node *>& uncatchedNodes)
{
  if(uncatchedNodes.find(_node)!=uncatchedNodes.end())
    return this;
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

std::string ElementaryPoint::getNodeName() const
{
  return _node->getName();
}
