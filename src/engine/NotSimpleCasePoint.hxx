// Copyright (C) 2019-2024  CEA, EDF
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

#pragma once

#include "YACSlibEngineExport.hxx"
#include "BlocPoint.hxx"

namespace YACS
{
  namespace ENGINE
  {
    class YACSLIBENGINE_EXPORT NotSimpleCasePoint : public BlocPoint
    {
    public:
      NotSimpleCasePoint() = default;
      NotSimpleCasePoint(const std::list<AbstractPoint *>& nodes, AbstractPoint *father);
      ~NotSimpleCasePoint() = default;
      AbstractPoint *deepCopy(AbstractPoint *father) const override;
      //
      Node *getFirstNode() override;
      Node *getLastNode() override;
      int getMaxLevelOfParallelism() const override;
      void getWeightRegardingDPL(ComplexWeight *weight) override;
      void partitionRegardingDPL(const PartDefinition *pd, std::map<ComposedNode *, YACS::BASES::AutoRefCnt<PartDefinition> >& zeMap) const override;
      std::string getRepr() const override;
      void accept(PointVisitor *pv) override;
      AbstractPoint *expandNonSimpleCaseOn(NotSimpleCasePoint *pathologicalPt, const std::set<Node *>& uncatchedNodes) override;
    };
  }
}
