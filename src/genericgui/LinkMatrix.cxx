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

#include "LinkMatrix.hxx"
#include "Scene.hxx"
#include "SceneComposedNodeItem.hxx"
#include "SceneElementaryNodeItem.hxx"
#include "SceneHeaderItem.hxx"
#include "SceneHeaderNodeItem.hxx"
#include "SceneCtrlPortItem.hxx"
#include "SceneLinkItem.hxx"
#include "QtGuiContext.hxx"
#include "InPort.hxx"
#include "OutPort.hxx"
#include "Resource.hxx"

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
  _pas=10;
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
  if (Scene::_addRowCols) addRowCols();
  _im = _sxm.size();
  _xm.resize(_im);
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
  _ym.resize(_jm);
  DEBTRACE("_sym.size()=" << _jm);
  i =0;
  for(std::set<double>::iterator it = _sym.begin(); it != _sym.end(); ++it)
    {
      _ym[i] = *it;
      _y2j[*it] = i;
      DEBTRACE("_ym[" << i << "] = " << _ym[i]);
      i++;
    }
  _cost.resize(_im*_jm);
  for (int ij=0; ij < _im*_jm; ij++)
    _cost[ij] = 1;       // --- set the _cost matrix open everywhere (no obstacles)
  explore(_bloc, true);  // --- fill the cells cost(i,j) with obstacles

  for (int j=0; j<_jm; j++)
    {
      char* m = new char[_im+1];
      for (int i=0; i<_im; i++)
        if (cost(i,j))
          m[i] = ' ';
        else 
          m[i] = 'X';
      m[_im] = 0;
      DEBTRACE(m);
      delete [] m;
    }
}

std::pair<int,int> LinkMatrix::cellFrom(YACS::ENGINE::OutPort* outp)
{
  SubjectDataPort* subDP = _context->_mapOfSubjectDataPort[outp];
  SceneItem* item = _context->_mapOfSceneItem[subDP];
  QRectF bb = (item->mapToScene(item->boundingRect())).boundingRect();
  qreal xp = bb.right();
  qreal yp = (bb.top() + bb.bottom())*0.5;
  DEBTRACE("xp,yp:"<<xp<<","<<yp);
  int ifrom = -1;
  for (int i=0; i<_im-1; i++)
    if (_xm[i+1] >= xp && xp > _xm[i])
      {
        ifrom = i;
        break;
      }
  int jfrom = -1;
  for (int j=0; j<_jm-1; j++)
    if (_ym[j+1] >= yp && yp > _ym[j])
      {
        jfrom = j;
        break;
      }
  //if ifrom or jfrom == -1 the port is outside the matrix
  if(ifrom < 0 || jfrom < 0)return pair<int,int>(ifrom,jfrom);
  while (ifrom < _im && !cost(ifrom,jfrom)) ifrom++;  // --- from point is inside an obstacle

  return pair<int,int>(ifrom,jfrom);
}

std::pair<int,int> LinkMatrix::cellFrom(YACS::ENGINE::OutGate* outp)
{
  SubjectNode* subNode = _context->_mapOfSubjectNode[outp->getNode()];
  SceneNodeItem* itemNode = dynamic_cast<SceneNodeItem*>(_context->_mapOfSceneItem[subNode]);
  YASSERT(itemNode);
  SceneHeaderNodeItem* itemHeader = dynamic_cast<SceneHeaderNodeItem*>(itemNode->getHeader());
  YASSERT(itemHeader);
  SceneCtrlPortItem *item = itemHeader->getCtrlOutPortItem();
  YASSERT(item);
  QRectF bb = (item->mapToScene(item->boundingRect())).boundingRect();
  qreal xp = bb.right();
  qreal yp = (bb.top() + bb.bottom())*0.5;
  DEBTRACE("xp,yp:"<<xp<<","<<yp);
  int ifrom = -1;
  for (int i=0; i<_im-1; i++)
    if (_xm[i+1] >= xp && xp > _xm[i])
      {
        ifrom = i;
        break;
      }
  int jfrom = -1;
  for (int j=0; j<_jm-1; j++)
    if (_ym[j+1] >= yp && yp > _ym[j])
      {
        jfrom = j;
        break;
      }
  //if ifrom or jfrom == -1 the port is outside the matrix
  if(ifrom < 0 || jfrom < 0)return pair<int,int>(ifrom,jfrom);
  while (ifrom < _im && !cost(ifrom,jfrom)) ifrom++;  // --- from point is inside an obstacle

  return pair<int,int>(ifrom,jfrom);
}

std::pair<int,int> LinkMatrix::cellTo(YACS::ENGINE::InPort* inp)
{
  SubjectDataPort* subDP = _context->_mapOfSubjectDataPort[inp];
  SceneItem* item = _context->_mapOfSceneItem[subDP];
  QRectF bb = (item->mapToScene(item->boundingRect())).boundingRect();
  qreal xp = bb.left();
  qreal yp = (bb.top() + bb.bottom())*0.5;
  DEBTRACE("xp,yp:"<<xp<<","<<yp);
  int ito = -1;
  for (int i=0; i<_im-1; i++)
    if (_xm[i+1] >= xp && xp > _xm[i])
      {
        ito = i;
        break;
      }
  int jto = -1;
  for (int j=0; j<_jm-1; j++)
    if (_ym[j+1] >= yp && yp > _ym[j])
      {
        jto = j;
        break;
      }
  //if ito or jto == -1 the port is outside the matrix
  if(ito < 0 || jto < 0)return pair<int,int>(ito,jto);
  while (ito >0 && !cost(ito,jto)) ito--;  // --- to point is inside an obstacle

  return pair<int,int>(ito,jto);
}

std::pair<int,int> LinkMatrix::cellTo(YACS::ENGINE::InGate* inp)
{
  SubjectNode* subNode = _context->_mapOfSubjectNode[inp->getNode()];
  SceneNodeItem* itemNode = dynamic_cast<SceneNodeItem*>(_context->_mapOfSceneItem[subNode]);
  YASSERT(itemNode);
  SceneHeaderNodeItem* itemHeader = dynamic_cast<SceneHeaderNodeItem*>(itemNode->getHeader());
  YASSERT(itemHeader);
  SceneCtrlPortItem *item = itemHeader->getCtrlInPortItem();
  YASSERT(item);
  QRectF bb = (item->mapToScene(item->boundingRect())).boundingRect();
  qreal xp = bb.left();
  qreal yp = (bb.top() + bb.bottom())*0.5;
  DEBTRACE("xp,yp:"<<xp<<","<<yp);
  int ito = -1;
  for (int i=0; i<_im-1; i++)
    if (_xm[i+1] >= xp && xp > _xm[i])
      {
        ito = i;
        break;
      }
  int jto = -1;
  for (int j=0; j<_jm-1; j++)
    if (_ym[j+1] >= yp && yp > _ym[j])
      {
        jto = j;
        break;
      }
  //if ito or jto == -1 the port is outside the matrix
  if(ito < 0 || jto < 0)return pair<int,int>(ito,jto);
  while (ito > 0 && !cost(ito,jto)) ito--;  // --- to point is inside an obstacle

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
  //use a random coefficient between 0.25 and 0.75 to try to separate links
  double coef=-0.25+rand()%101*0.005;
  coef=0.5 + coef* Resource::link_separation_weight/10.;
  LNodePath::const_iterator it = lnp.begin();
  for (int k=0; k<dim; k++)
    {
      int i = it->getX();
      int j = it->getY();
      DEBTRACE("i, j: " << i << " " << j << " Xmax, Ymax: " << _im << " " << _jm);
      linkPoint a;

      if ( (i+1)==_im ) {
        a.x = _xm[i];
      } else {
        a.x = coef*_xm[i] + (1.-coef)*_xm[i+1];
      };

      if ( (j+1)==_jm ) {
        a.y = _ym[j];
      } else {
        a.y = coef*_ym[j] + (1.-coef)*_ym[j+1];
      };

      lp.push_back(a);
      DEBTRACE(a.x << " " << a.y);
      ++it;
    }
  return lp;
}

void LinkMatrix::incrementCost(LNodePath lnp)
{
  int dim = lnp.size();  
  LNodePath::const_iterator it = lnp.begin();
  for (; it != lnp.end(); ++it)
    {
      int i = it->getX();
      int j = it->getY();
      int ij = i*_jm +j;
      _cost[ij] += Resource::link_separation_weight; // --- big cost, because distance is x2+y2

    }    
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

void LinkMatrix::addRowCols()
{
  {
    set<double> sxmCpy = _sxm;
    if (sxmCpy.empty()) return;
    set<double>::iterator itx = sxmCpy.begin();
    double xmin = *itx;
    double xmax = xmin;
    itx++;
    for (; itx != sxmCpy.end(); ++itx)
      {
        xmax = *itx;
        int nbpas = floor((xmax -xmin)/_pas);
        
        if (nbpas >= 2)
          {
            double xpas = (xmax -xmin)/nbpas;
            for (int i=1; i<nbpas; i++)
              _sxm.insert(xmin +i*xpas);
          }
        xmin = xmax;
      }
  }
  {
    set<double> symCpy = _sym;
    if (symCpy.empty()) return;
    set<double>::iterator ity = symCpy.begin();
    double ymin = *ity;
    double ymax = ymin;
    ity++;
    for (; ity != symCpy.end(); ++ity)
      {
        ymax = *ity;
        int nbpas = floor((ymax -ymin)/_pas);
        
        if (nbpas >= 2)
          {
            double ypas = (ymax -ymin)/nbpas;
            for (int i=1; i<nbpas; i++)
              _sym.insert(ymin +i*ypas);
          }
        ymin = ymax;
      }
  }
}
