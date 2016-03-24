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

#ifndef _LINKMATRIX_HXX_
#define _LINKMATRIX_HXX_

#include <vector>
#include <set>
#include <map>
#include <list>

namespace YACS
{
  namespace ENGINE
  {
    class OutPort;
    class InPort;
    class OutGate;
    class InGate;
  }

  namespace HMI
  {
    class AbstractSceneItem;
    class SceneItem;
    class SceneComposedNodeItem;
    class SceneLinkItem;
    class QtGuiContext;

    class LNode
    {
    public:
      LNode(int x, int y) : _x(x), _y(y) {};
      LNode(std::pair<int,int> m) : _x(m.first), _y(m.second) {};
      virtual ~LNode() {};
      inline int getX() const {return _x; };
      inline int getY() const {return _y; };
      inline std::pair<int,int> getPos() const { return std::pair<int,int>(_x, _y); };
      inline bool isEqual(const LNode& o) const { return ((_x == o._x) && (_y == o._y)); };
      double distance(const LNode& o) const;
    protected:
      int _x;
      int _y;
    };

    typedef std::list<LNode> LNodePath;

    struct linkdef
    {
      std::pair<int,int> from;
      std::pair<int,int> to;
      YACS::HMI::SceneLinkItem* item;
    };

    struct linkPoint
    {
      double x;
      double y;
    };

    typedef  std::list<linkPoint> LinkPath;

    class LinkMatrix
    {
    public:
      LinkMatrix(SceneComposedNodeItem* bloc);
      virtual ~LinkMatrix();
      void compute();
      void addRowCols();
      void explore(AbstractSceneItem *child, bool setObstacle=false);
      void defineCost(AbstractSceneItem *child);
      void getBoundingBox(SceneItem *obstacle, int margin, bool setObstacle=false);
      std::pair<int,int> cellFrom(YACS::ENGINE::OutPort* outp);
      std::pair<int,int> cellFrom(YACS::ENGINE::OutGate* outp);
      std::pair<int,int> cellTo(YACS::ENGINE::InPort* inp);
      std::pair<int,int> cellTo(YACS::ENGINE::InGate* inp);
      std::list<linkdef> getListOfCtrlLinkDef();
      std::list<linkdef> getListOfDataLinkDef();
      LinkPath getPath(LNodePath lnp);
      void incrementCost(LNodePath lnp);

      inline int cost(int i, int j) const { return _cost[i*_jm +j]; };
      inline int imax() const { return _im; };
      inline int jmax() const { return _jm; };
      
    protected:
      SceneComposedNodeItem* _bloc;
      int _im;
      int _jm;
      double _pas;
      std::set<double> _sxm;
      std::set<double> _sym;
      std::vector<double> _xm;
      std::vector<double> _ym;
      std::map<double,int> _x2i;
      std::map<double,int> _y2j;
      std::vector<int> _cost;
      QtGuiContext* _context;
    };
  }
}

#endif
