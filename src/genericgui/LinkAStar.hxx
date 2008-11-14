#ifndef _LINKASTAR_HXX_
#define _LINKASTAR_HXX_

#include "LinkMatrix.hxx"

#include <map>
#include <list>

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
      double distance(int i1, int j1, int i2, int j2);
    protected:
      const LinkMatrix &_linkMatrix;
      LNodeMap _closedList;
      LNodeMap _openList;
      LNode _from;
      LNode _to;
    };

  }
}
#endif
