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

#ifndef __BAGPOINT_HXX__
#define __BAGPOINT_HXX__

#include "YACSlibEngineExport.hxx"
#include "BlocPoint.hxx"

#include <list>

namespace YACS
{
  namespace ENGINE
  {
    class YACSLIBENGINE_EXPORT BagPoint : public BlocPoint
    {
    public:
      BagPoint() = default;
      BagPoint(const std::list<AbstractPoint *>& nodes, AbstractPoint *father);
    public://overloading
      AbstractPoint *deepCopy(AbstractPoint *father) const override;
      Node *getFirstNode() override;
      Node *getLastNode() override;
      int getMaxLevelOfParallelism() const;
      void getWeightRegardingDPL(ComplexWeight *weight);
      void partitionRegardingDPL(const PartDefinition *pd, std::map<ComposedNode *, YACS::BASES::AutoRefCnt<PartDefinition> >& zeMap) const;
      std::string getRepr() const;
      void accept(PointVisitor *pv) override;
      AbstractPoint *expandNonSimpleCaseOn(NotSimpleCasePoint *pathologicalPt, const std::set<Node *>& uncatchedNodes) override;
    public:
      int size() const { return (int)_nodes.size(); }
      void replaceInMe(BlocPoint *aSet);
      void deal1(bool& somethingDone);
      void deal2(bool& somethingDone);
      void deal2Bis(bool& somethingDone);
      void deal2Ter(bool& somethingDone);
      void deal2Quatro(bool& somethingDone);
      void dealNotSimpleCase(bool& somethingDone);
      void expandNonSimpleCase();
    private:
      void expandNonSimpleCaseOn(AbstractPoint *pathologicalPt);
    };
  }
}

#endif
