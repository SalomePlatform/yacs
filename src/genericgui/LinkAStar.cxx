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

#include "LinkAStar.hxx"

#include <map>
#include <list>
#include <cmath>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::HMI;


LCostNode::LCostNode() : _gCost(0), _hCost(0), _fCost(0),
                         _parent( std::pair<int,int>(0,0) )
{
  //DEBTRACE("LCostNode::LCostNode()");
}

LCostNode::LCostNode(std::pair<int,int> parent) : _gCost(0), _hCost(0), _fCost(0),
                                                  _parent(parent)
{
  //DEBTRACE("LCostNode::LCostNode(std::pair<int,int> parent)");
}



LinkAStar::LinkAStar(const LinkMatrix& linkMatrix) : _linkMatrix(linkMatrix), _from(0,0), _to(0,0)
{
  _closedList.clear();
  _openList.clear();
  _pq=std::priority_queue<Cost>();
}

LinkAStar::~LinkAStar()
{
}
      
bool LinkAStar::computePath(LNode from, LNode to)
{
  _closedList.clear();
  _openList.clear();
  _pq=std::priority_queue<Cost>();
  _from = from;
  _to = to;
  bool isPath = false;

  //pair<int, int> curPos(0, 0);
  //LCostNode startCost(curPos);

  pair<int, int> curPos = _from.getPos();
  LCostNode startCost(curPos);
  _openList[curPos] = startCost;
  moveToClosedList(curPos);
  addNeighbours(curPos);

  while (! ((curPos.first == _to.getX()) && (curPos.second == _to.getY()))
         && (!_openList.empty()))
    {
      curPos = bestNode(_openList);
      moveToClosedList(curPos);
      DEBTRACE("curPos(" << curPos.first << "," << curPos.second << ")");
      addNeighbours(curPos);
    }

  if ((curPos.first == _to.getX()) && (curPos.second == _to.getY()))
    isPath = true;

  return isPath;
}

LNodePath LinkAStar::givePath()
{
  LNodePath aPath;
  aPath.clear();
  aPath.push_front(_to);

  LNode current = _to;
  while (! current.isEqual(_from))
    {
      current = LNode(_closedList[current.getPos()].getParent());
      aPath.push_front(current);
      DEBTRACE("(" << current.getX() << "," << current.getY() << ")");
    }
//   aPath.push_front(_from);
//   DEBTRACE("(" << _from.getX() << "," << _from.getY() << ")");
  return aPath;
}

bool LinkAStar::isAlreadyInList(std::pair<int,int> n, const LNodeMap& aList)
{
  LNodeMap::const_iterator it = aList.find(n);
  if (it == aList.end())
    return false;
  else
    return true;
}

void LinkAStar::addNeighbours(std::pair<int,int> currentNode)
{
  LCostNode tmp(currentNode);
  int x = currentNode.first;
  int y = currentNode.second;
  for (int i = x-1; i <= x+1; i++)
    {
      if ((i<0) || (i >= _linkMatrix.imax()))
        continue;   // --- skip: outside matrix
      for (int j = y-1; j <= y+1; j++)
        {
          if ((j<0) || (j >= _linkMatrix.jmax()))
            continue; // --- skip: outside matrix
          
          if ((i == x) && (j == y))
            continue; // --- skip: current node

          if ((i != x) && (j != y))
            continue; // --- skip: diagonals (move only vertical or horizontal)
          
          int cost = _linkMatrix.cost(i,j);
          if (! cost)
            continue; // --- skip: blocked
          
          pair<int,int> pos(i,j);
          if (isAlreadyInList(pos, _closedList))
            continue; // --- skip: already in closed list
          
          tmp.setGCost(_closedList[currentNode].getGCost() + cost*distance(x, y, i, j));
          tmp.setHCost(distance(i, j, _to.getX(), _to.getY()));
          tmp.setFCost(tmp.getGCost() + tmp.getHCost());
          if (isAlreadyInList(pos, _openList))
            {
              if (tmp.getFCost() < _openList[pos].getFCost())
                {
                  _openList[pos] = tmp; // --- new path better, update node
                  _pq.push(Cost(tmp.getFCost(),pos));
                }
            }
          else
            {
              _openList[pos] = tmp; // --- add node
              _pq.push(Cost(tmp.getFCost(),pos));
            }
        }
    }
}
 
std::pair<int,int> LinkAStar::bestNode(const LNodeMap& aList)
{
  std::pair<int, int> pos;
  do
    {
      pos=_pq.top().pos;
      _pq.pop();
    }
  while(aList.count(pos)==0);
  return pos;
}

void LinkAStar::moveToClosedList(std::pair<int,int> pos)
{
  _closedList[pos] = _openList[pos];
  if (_openList.erase(pos) == 0)
    DEBTRACE("node not in open list, can't delete");
}
