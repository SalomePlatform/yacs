// Copyright (C) 2006-2016  CEA/DEN, EDF R&D
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

#ifndef _LINKASTAR_HXX_
#define _LINKASTAR_HXX_

#include "LinkMatrix.hxx"

#include <map>
#include <queue>
#include <list>
#include <cmath>
#include <stdlib.h>

namespace YACS
{
  namespace HMI
  {

    class LCostNode
    {
    public:
      LCostNode();
      LCostNode(std::pair<int,int> parent);
      virtual ~LCostNode() {};
      inline void setParent(std::pair<int,int> parent) { _parent = parent; };
      std::pair<int,int> getParent() const {return _parent; };
      inline void setGCost(double c) { _gCost = c; };
      inline void setHCost(double c) { _hCost = c; };
      inline void setFCost(double c) { _fCost = c; };
      inline double getGCost() const { return _gCost; };
      inline double getHCost() const { return _hCost; };
      inline double getFCost() const { return _fCost; };
    protected:
      double _gCost, _hCost, _fCost;
      std::pair<int,int> _parent;
    };

    typedef std::map<std::pair<int,int>, LCostNode> LNodeMap;

    struct Cost
      {
        Cost(double f,std::pair<int,int> p):F(f),pos(p) {};
        double F;
        std::pair<int,int> pos;
        bool operator<(const Cost& a) const
          {
            return (a.F <= F);
          }
      };

    class LinkAStar
    {
    public:
      LinkAStar(const LinkMatrix& linkMatrix);
      ~LinkAStar();
      
      bool computePath(LNode from, LNode to);
      LNodePath givePath();
      bool isAlreadyInList(std::pair<int,int> n, const LNodeMap& aList);
      void addNeighbours(std::pair<int,int> n);
      std::pair<int,int> bestNode(const LNodeMap& aList);
      void moveToClosedList(std::pair<int,int> n);
      //inline double distance(int i1, int j1, int i2, int j2) { return std::sqrt(double((i1-i2)*(i1-i2) + (j1-j2)*(j1-j2)));};
      //inline double distance(int i1, int j1, int i2, int j2) { return double((i1-i2)*(i1-i2) + (j1-j2)*(j1-j2));};
      //manhattan distance is better for 4 connected cells
      inline double distance(int i1, int j1, int i2, int j2) { return abs(i1-i2)+abs(j1-j2);};
    protected:
      const LinkMatrix &_linkMatrix;
      LNodeMap _closedList;
      LNodeMap _openList;
      LNode _from;
      LNode _to;
      std::priority_queue<Cost> _pq;
    };

  }
}
#endif
