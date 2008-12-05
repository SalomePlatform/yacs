//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#include "LinkMatrix.hxx"
#include "SceneComposedNodeItem.hxx"
#include "SceneElementaryNodeItem.hxx"
#include "SceneHeaderItem.hxx"
#include "SceneLinkItem.hxx"
#include "QtGuiContext.hxx"
#include "InPort.hxx"
#include "OutPort.hxx"

#include <cmath>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;

double LNode::distance(const LNode& o) const
{
  int dx = _x -o._x;
  int dy = _y -o._y;
  return sqrt(double(dx*dx +dy*dy));
}


LinkMatrix::LinkMatrix(SceneComposedNodeItem *bloc): _bloc(bloc)
{
  _im=0;
  _jm=0;
  _sxm.clear();
  _sym.clear();
  _xm.clear();
  _ym.clear();
  _x2i.clear();
  _y2j.clear();
  _cost.clear();
  _context = QtGuiContext::getQtCurrent();
}

LinkMatrix::~LinkMatrix()
{
}

void LinkMatrix::compute()
{
  getBoundingBox(_bloc,0);
  explore(_bloc);        // --- define the boundaries _xm[i] and _ym[j]
  _im = _sxm.size();
  _xm.reserve(_im);
  DEBTRACE("_sxm.size()=" << _im);
  int i =0;
  for(std::set<double>::iterator it = _sxm.begin(); it != _sxm.end(); ++it)
    {
      _xm[i] = *it;
      _x2i[*it] = i;
      DEBTRACE("_xm[" << i << "] = " << _xm[i]);
      i++;
    }
  _jm = _sym.size();
  _ym.reserve(_jm);
  DEBTRACE("_sym.size()=" << _jm);
  i =0;
  for(std::set<double>::iterator it = _sym.begin(); it != _sym.end(); ++it)
    {
      _ym[i] = *it;
      _y2j[*it] = i;
      DEBTRACE("_ym[" << i << "] = " << _ym[i]);
      i++;
    }
  _cost.reserve(_im*_jm);
  for (int ij=0; ij < _im*_jm; ij++)
    _cost[ij] = 1;       // --- set the _cost matrix open everywhere (no obstacles)
  explore(_bloc, true);  // --- fill the cells cost(i,j) with obstacles

  for (int j=0; j<_jm; j++)
    {
      char m[_im+1];
      for (int i=0; i<_im; i++)
        if (cost(i,j))
          m[i] = ' ';
        else 
          m[i] = 'X';
      m[_im] = 0;
      DEBTRACE(m);
    }
}

std::pair<int,int> LinkMatrix::cellFrom(YACS::ENGINE::OutPort* outp)
{
  SubjectDataPort* subDP = _context->_mapOfSubjectDataPort[outp];
  SceneItem* item = _context->_mapOfSceneItem[subDP];
  QRectF bb = (item->mapToScene(item->boundingRect())).boundingRect();
  qreal xp = bb.right();
  qreal yp = (bb.top() + bb.bottom())*0.5;
  int ifrom = -1;
  for (int i=0; i<_im-1; i++)
    if (_xm[i+1] > xp)
      {
        ifrom = i;
        break;
      }
  int jfrom = -1;
  for (int j=0; j<_jm-1; j++)
    if (_ym[j+1] > yp)
      {
        jfrom = j;
        break;
      }
  while (!cost(ifrom,jfrom)) ifrom++;  // --- from point is inside an obstacle
  //ifrom++;
  return pair<int,int>(ifrom,jfrom);
}

std::pair<int,int> LinkMatrix::cellFrom(YACS::ENGINE::OutGate* outp)
{
  SubjectNode* subNode = _context->_mapOfSubjectNode[outp->getNode()];
  SceneItem* item = _context->_mapOfSceneItem[subNode];
  QRectF bb = (item->mapToScene(item->boundingRect())).boundingRect();
  qreal xp = bb.right();
  qreal yp = (bb.top() + bb.bottom())*0.5;
  int ifrom = -1;
  for (int i=0; i<_im-1; i++)
    if (_xm[i+1] > xp)
      {
        ifrom = i;
        break;
      }
  int jfrom = -1;
  for (int j=0; j<_jm-1; j++)
    if (_ym[j+1] > yp)
      {
        jfrom = j;
        break;
      }
  while (!cost(ifrom,jfrom)) ifrom++;  // --- from point is inside an obstacle
  //ifrom++;
  return pair<int,int>(ifrom,jfrom);
}

std::pair<int,int> LinkMatrix::cellTo(YACS::ENGINE::InPort* inp)
{
  SubjectDataPort* subDP = _context->_mapOfSubjectDataPort[inp];
  SceneItem* item = _context->_mapOfSceneItem[subDP];
  QRectF bb = (item->mapToScene(item->boundingRect())).boundingRect();
  qreal xp = bb.left();
  qreal yp = (bb.top() + bb.bottom())*0.5;
  int ito = -1;
  for (int i=0; i<_im-1; i++)
    if (_xm[i+1] > xp)
      {
        ito = i;
        break;
      }
  int jto = -1;
  for (int j=0; j<_jm-1; j++)
    if (_ym[j+1] > yp)
      {
        jto = j;
        break;
      }
  while (!cost(ito,jto)) ito--;  // --- from point is inside an obstacle
  //ito--;
  return pair<int,int>(ito,jto);
}

std::pair<int,int> LinkMatrix::cellTo(YACS::ENGINE::InGate* inp)
{
  SubjectNode* subNode = _context->_mapOfSubjectNode[inp->getNode()];
  SceneItem* item = _context->_mapOfSceneItem[subNode];
  QRectF bb = (item->mapToScene(item->boundingRect())).boundingRect();
  qreal xp = bb.left();
  qreal yp = (bb.top() + bb.bottom())*0.5;
  int ito = -1;
  for (int i=0; i<_im-1; i++)
    if (_xm[i+1] > xp)
      {
        ito = i;
        break;
      }
  int jto = -1;
  for (int j=0; j<_jm-1; j++)
    if (_ym[j+1] > yp)
      {
        jto = j;
        break;
      }
  while (!cost(ito,jto)) ito--;  // --- from point is inside an obstacle
  //ito--;
  return pair<int,int>(ito,jto);
}

std::list<linkdef> LinkMatrix::getListOfCtrlLinkDef()
{
  list<linkdef> alist;
  map<pair<Node*, Node*>, SubjectControlLink*>::const_iterator it;
  for (it = _context->_mapOfSubjectControlLink.begin();
       it != _context->_mapOfSubjectControlLink.end(); ++it)
    {
      linkdef ali;
      pair<Node*, Node*> outin = it->first;
      SubjectControlLink* sub = it->second;
      ali.from = cellFrom(outin.first->getOutGate());
      ali.to = cellTo(outin.second->getInGate());
      ali.item = dynamic_cast<SceneLinkItem*>(_context->_mapOfSceneItem[sub]);
      alist.push_back(ali);
      DEBTRACE("from("<<ali.from.first<<","<<ali.from.second
               <<") to ("<<ali.to.first<<","<<ali.to.second
               <<") " << ali.item->getLabel().toStdString());
    }
  return alist;
}

std::list<linkdef> LinkMatrix::getListOfDataLinkDef()
{
  list<linkdef> alist;
  map<pair<OutPort*, InPort*>, SubjectLink*>::const_iterator it;
  for (it = _context->_mapOfSubjectLink.begin();
       it != _context->_mapOfSubjectLink.end(); ++it)
    {
      linkdef ali;
      pair<OutPort*, InPort*> outin = it->first;
      SubjectLink* sub = it->second;
      ali.from = cellFrom(outin.first);
      ali.to = cellTo(outin.second);
      ali.item = dynamic_cast<SceneLinkItem*>(_context->_mapOfSceneItem[sub]);
      alist.push_back(ali);
      DEBTRACE("from("<<ali.from.first<<","<<ali.from.second
               <<") to ("<<ali.to.first<<","<<ali.to.second
               <<") " << ali.item->getLabel().toStdString());
    }
  return alist;
}

LinkPath LinkMatrix::getPath(LNodePath lnp)
{
  DEBTRACE("LinkMatrix::getPath " << lnp.size());
  LinkPath lp;
  lp.clear();
  int dim = lnp.size();  
  LNodePath::const_iterator it = lnp.begin();
  for (int k=0; k<dim; k++)
    {
      int i = it->getX();
      int j = it->getY();
      linkPoint a;
      a.x = 0.5*(_xm[i] + _xm[i+1]);
      a.y = 0.5*(_ym[j] + _ym[j+1]);
      lp.push_back(a);
      DEBTRACE(a.x << " " << a.y);
      ++it;
    }
  return lp;
}

int LinkMatrix::cost(int i, int j) const
{
  int ij = i*_jm +j;
  return _cost[ij];
}


/*!
 * find recursively elementary nodes and node headers and call getBoundingBox on each item.
 * first pass with setObstacle = false stores the x and y coordinates of the mesh boundaries.
 * second pass with setObstacle = true fills the mesh with obstacles i.e. elementary nodes or
 * nodeHeaders.
 */
void LinkMatrix::explore(AbstractSceneItem *child, bool setObstacle)
{
  SceneComposedNodeItem *cnItem = dynamic_cast<SceneComposedNodeItem*>(child);
  if (cnItem)
    {
      SceneHeaderItem *obstacle = cnItem->getHeader();
      if (obstacle) getBoundingBox(obstacle, 0, setObstacle);
      list<AbstractSceneItem*> children = cnItem->getChildren();
      for (list<AbstractSceneItem*>::const_iterator it = children.begin(); it != children.end(); ++it)
        explore(*it, setObstacle);
    }
  SceneElementaryNodeItem *ceItem = dynamic_cast<SceneElementaryNodeItem*>(child);
  if (ceItem)
    {
      getBoundingBox(ceItem, 1, setObstacle);
    }
}

/*!
 * first pass with setObstacle = false stores the x and y coordinates of the mesh boundaries.
 * second pass with setObstacle = true fills the mesh with obstacles i.e. elementary nodes or
 * nodeHeaders. For elementary nodes, the bounding box is smaller to let a path between nodes
 * that are stick together.
 */
void LinkMatrix::getBoundingBox(SceneItem *obstacle, int margin, bool setObstacle)
{
  QRectF bb = (obstacle->mapToScene(obstacle->boundingRect())).boundingRect();
  if (setObstacle)
    {
      int imin = _x2i[bb.left()   + margin];
      int imax = _x2i[bb.right()  - margin];
      int jmin = _y2j[bb.top()    + margin];
      int jmax = _y2j[bb.bottom() - margin];
      DEBTRACE(bb.left() << " " << bb.right() << " " << bb.top() << " " << bb.bottom() );
      DEBTRACE(imin << " " << imax << " " << jmin << " " << jmax);
      for (int j=jmin; j<jmax; j++)
        for (int i=imin; i<imax; i++)
          {
            int ij = i*_jm +j;
            _cost[ij] = 0;       // --- obstacle
          }
    }
  else
    {
      _sxm.insert(bb.left()   + margin);
      _sxm.insert(bb.right()  - margin);
      _sym.insert(bb.top()    + margin);
      _sym.insert(bb.bottom() - margin);
    }
}
