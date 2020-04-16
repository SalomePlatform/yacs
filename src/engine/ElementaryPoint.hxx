// Copyright (C) 2015-2020  CEA/DEN, EDF R&D
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

#ifndef __ELEMENTARYPOINT_HXX__
#define __ELEMENTARYPOINT_HXX__

#include "YACSlibEngineExport.hxx"
#include "AbstractPoint.hxx"

#include <vector>

namespace YACS
{
  namespace ENGINE
  {
    class Node;

    class YACSLIBENGINE_EXPORT ElementaryPoint : public AbstractPoint
    {
    private:
      Node *_node;
    public:
      ElementaryPoint(Node *node):AbstractPoint(0),_node(node) { }
      AbstractPoint *findPointWithNode(Node *node);
      bool contains(Node *node) const override;
      bool anyOf(const std::set<Node *>& nodes) const override;
      AbstractPoint *deepCopy(AbstractPoint *father) const override;
      Node *getFirstNode() override;
      Node *getLastNode() override;
      int getNumberOfNodes() const;
      int getMaxLevelOfParallelism() const;
      void getWeightRegardingDPL(ComplexWeight *weight);
      void partitionRegardingDPL(const PartDefinition *pd, std::map<ComposedNode *, YACS::BASES::AutoRefCnt<PartDefinition> >& zeMap) const;
      std::string getRepr() const;
      void accept(PointVisitor *pv) override;
      AbstractPoint *expandNonSimpleCaseOn(NotSimpleCasePoint *pathologicalPt, const std::set<Node *>& uncatchedNodes) override;
      virtual ~ElementaryPoint();
    public:
      void setNode(Node *node) { _node=node; }
      Node *getNode() const { return _node; }
      std::string getNodeName() const;
    };
  }
}


#endif
