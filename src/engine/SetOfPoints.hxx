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

#ifndef __SETOFPOINTS_HXX__
#define __SETOFPOINTS_HXX__

#include "YACSlibEngineExport.hxx"
#include "PlayGround.hxx"
#include "PointVisitor.hxx"
#include "AutoRefCnt.hxx"

#include <map>
#include <list>
#include <string>

namespace YACS
{
  namespace ENGINE
  {
    class Node;
    class BagPoint;
    class AbstractPoint;
    class ComposedNode;
    
    class YACSLIBENGINE_EXPORT SetOfPoints
    {
    public:
      SetOfPoints(const std::list<Node *>& nodes);
      SetOfPoints(BagPoint *bp):_bp(bp) { }
      ~SetOfPoints();
      void basicSimplify();
      void simplify();
      std::string getRepr() const;
      AbstractPoint *findPointWithNode(Node *node);
      const std::list<AbstractPoint *>& getListOfPoints() const;
      int getMaxLevelOfParallelism() const;
      void getWeightRegardingDPL(ComplexWeight *weight);
      void partitionRegardingDPL(const PartDefinition *pd, std::map<ComposedNode *, YACS::BASES::AutoRefCnt<PartDefinition> >& zeMap) const;
      AbstractPoint *getUniqueAndReleaseIt() const;
      void accept(PointVisitor *pv);
    private:
      BagPoint *_bp;
    };
  }
}


#endif
