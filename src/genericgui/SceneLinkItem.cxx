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
#include "SceneLinkItem.hxx"
#include "SceneDataPortItem.hxx"
#include "SceneElementaryNodeItem.hxx"
#include "SceneHeaderNodeItem.hxx"
#include "Scene.hxx"

#include "Menus.hxx"
// #include <QGraphicsSceneHoverEvent>
#include <QPointF>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;


SceneLinkItem::SceneLinkItem(QGraphicsScene *scene, SceneItem *parent,
                             ScenePortItem* from, ScenePortItem* to,
                             QString label, Subject *subject)
  : SceneObserverItem(scene, parent, label, subject)
{
  _from = from;
  _to = to;
  _penColor     = QColor(  0,   0,  96);
  _hiPenColor   = QColor(  0,   0, 128);
  _brushColor   = QColor(  0,   0, 192);
  _hiBrushColor = QColor(192, 192, 255);
  _level += 100;
  setZValue(_level);
  _lp.clear();
  _directions.clear();
  _nbPoints = 0;
}

SceneLinkItem::~SceneLinkItem()
{
}

void SceneLinkItem::select(bool isSelected)
{
  if (isSelected)
    setZValue(_level+100);
  else
    setZValue(_level);
  SceneObserverItem::select(isSelected);
}

QRectF SceneLinkItem::boundingRect() const
{
  return _path.boundingRect();
}

QPainterPath SceneLinkItem::shape() const
{
  //DEBTRACE("SceneLinkItem::shape");
  return _path;
}

void SceneLinkItem::setShape()
{
  _path = QPainterPath();
  _path.setFillRule(Qt::WindingFill);
  QPointF pfrom = start();
  QPointF pto   = goal();
  if (_nbPoints)
    {
      addArrow(pfrom, _lp[0], _RIGHT);
      for (int k=0; k<_nbPoints-1; k++)
        addArrow(_lp[k], _lp[k+1], _directions[k+1]);
      addArrow(_lp[_nbPoints-1], pto, _RIGHT);
    }
  else
    {
      _path.moveTo(pfrom.x() -1, pfrom.y() -1);
      _path.lineTo(pto.x() +1, pto.y() -1);
      _path.lineTo(pto.x() +1, pto.y() +1);
      _path.lineTo(pfrom.x() -1, pfrom.y() +1);
      _path.lineTo(pfrom.x() -1, pfrom.y() -1);
    }
}

void SceneLinkItem::addArrow(QPointF pfrom,
                             QPointF pto,
                             HMI::Direction dir)
{
  qreal x, y, width, height, length;
  switch (dir)
    {
    case _UP:
      x = pfrom.x() -1;
      y = pfrom.y() -1;
      width = 3;
      height = 2 + pto.y() -pfrom.y();
      length = height;
      break;
    case _RIGHT:
      x = pfrom.x() -1;
      y = pfrom.y() -1;
      width = 2 + pto.x() -pfrom.x();
      height = 3;
      length = width;
      break;
    case _DOWN:
      x = pto.x() -1;
      y = pto.y() -1;
      width = 3;
      height = 2 + pfrom.y() -pto.y();
      length = height;
      break;
    case _LEFT:
      x = pto.x() -1;
      y = pto.y() -1;
      width = 2 + pfrom.x() -pto.x();
      height = 3;
      length = width;
      break;
    }
  _path.addRect(x, y, width, height);

  if (length > 20)
    {
      int e=5, h1=4, h2=8;
      switch (dir)
        {
        case _UP:
          x = pfrom.x();
          y = (pto.y() + pfrom.y())/2 +h1;
          _path.moveTo(x, y);
          _path.lineTo(x+e, y-h2);
          _path.lineTo(x,   y-h1);
          _path.lineTo(x-e, y-h2);
          _path.lineTo(x,   y   );
          break;
        case _RIGHT:
          x = (pto.x() + pfrom.x())/2 +h1;
          y = pfrom.y();
          _path.moveTo(x, y);
          _path.lineTo(x-h2, y+e);
          _path.lineTo(x-h1, y  );
          _path.lineTo(x-h2, y-e);
          _path.lineTo(x,    y  );
          break;
        case _DOWN:
          x = pfrom.x();
          y = (pto.y() + pfrom.y())/2 -h1;
          _path.moveTo(x, y);
          _path.lineTo(x+e, y+h2);
          _path.lineTo(x,   y+h1);
          _path.lineTo(x-e, y+h2);
          _path.lineTo(x,   y   );
          break;
        case _LEFT:
          x = (pto.x() + pfrom.x())/2 -h1;
          y = pfrom.y();
          _path.moveTo(x, y);
          _path.lineTo(x+h2, y+e);
          _path.lineTo(x+h1, y  );
          _path.lineTo(x+h2, y-e);
          _path.lineTo(x,    y  );
          break;
        }
    }
}

void SceneLinkItem::paint(QPainter *painter,
                          const QStyleOptionGraphicsItem *option,
                          QWidget *widget)
{
  //DEBTRACE("SceneLinkItem::paint " << _label.toStdString());
  if (_path.isEmpty()) setShape();
  painter->save();
  QPen pen;
  pen.setColor(getPenColor());
  painter->setPen(pen);
  painter->setBrush(getBrushColor());
  painter->drawPath(_path);
  painter->restore();
}

void SceneLinkItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE("SceneLinkItem::update "<< eventName(event)<<" "<<type<<" "<<son);
}

void SceneLinkItem::popupMenu(QWidget *caller, const QPoint &globalPos)
{
  LinkMenu m;
  m.popupMenu(caller, globalPos);
}

void SceneLinkItem::setPath(LinkPath lp)
{
  DEBTRACE("SceneLinkItem::setPath " << lp.size());
  _nbPoints = lp.size();
  _lp.reserve(_nbPoints+1);
  _directions.reserve(_nbPoints +2);
  std::list<linkPoint>::const_iterator it = lp.begin();
  int k=0;
  qreal prevx = 0;
  qreal prevy = 0;
  bool dirx = true;
  bool diry = false;
  for (; it != lp.end(); ++it)
    {
      QPointF p = mapFromScene(QPointF((*it).x, (*it).y));
      DEBTRACE("p(" << p.x() << "," << p.y() << ")");
      if (k == 0)
        {
         _lp[k] = p;
         _lp[k].setY(start().y());
         _directions[k] = _RIGHT; // --- direction from start to point k=0
         k++;
        }
      else
        {
          bool changeDir = true;
          if (dirx && (p.y() == prevy))
            {
              _lp[k-1].setX(p.x());
              changeDir = false;
            }
          if (diry && (p.x() == prevx))
            {
              _lp[k-1].setY(p.y());
              changeDir = false;
            }
          if (changeDir)
            {
              dirx = !dirx;
              diry = !diry;
              _lp[k] = p;
              if      (p.x() > prevx) _directions[k] = _RIGHT; // --- direction from k-1 to k
              else if (p.x() < prevx) _directions[k] = _LEFT;
              else if (p.y() > prevy) _directions[k] = _UP;
              else if (p.y() < prevy) _directions[k] = _DOWN;
              k++;
            }
        }
      prevx = p.x();
      prevy = p.y();
    }
  _nbPoints = k;
  _lp[k-1].setY(goal().y());
  _directions[k] = _RIGHT; // --- direction from point k-1 to goal
  dirx = true;
  diry = false;
  for (k = _nbPoints -2; k >= 0; k--)
    {
      if(_lp[k].y() == prevy)
        _lp[k].setY(goal().y());
      else
        break;
    }
  if (Scene::_simplifyLinks) minimizeDirectionChanges();
  if (Scene::_force2NodesLink) force2points();
  for (k=0; k<_nbPoints; k++)
    DEBTRACE("_lp[" << k << "](" << _lp[k].x() << "," << _lp[k].y() << ")");
  setShape();
}

/*!
 *  replace patterns like (right, down, right, down) with (right, right, down, down)
 *  if new path is OK (no obstacle in the modified rectangle), then combine the segments.
 */
void SceneLinkItem::minimizeDirectionChanges()
{
  vector<QPointF> newlp;
  vector<Direction> newdir;
  newlp.reserve(_nbPoints);
  newdir.reserve(_nbPoints +1);

  bool modified = true;
  while (modified)
    {
      modified = false;
      Direction prevdir = _RIGHT;
      newlp[0] = _lp[0];
      newdir[0] = _directions[0];
      int newk = 0;
      for (int k = 0; k < _nbPoints; k++)
        {
          if (k< _nbPoints-2) // --- _nbPoints + 1 directions, look to k+3
            if ((_directions[k] == _directions[k+2]) && (_directions[k+1] == _directions[k+3]))
              {
                bool kmodif = true;
                qreal tryx;
                qreal tryy;
                if ((_directions[k] == _RIGHT) || (_directions[k] == _LEFT))
                  {
                    tryy = _lp[k].y();
                    tryx = _lp[k+2].x();
                  }
                else
                  {
                    tryy = _lp[k+2].y();
                    tryx = _lp[k].x();
                }

                qreal xtop, ytop, width, height;
                if (_lp[k].x() < _lp[k+2].x())
                  {
                    xtop = _lp[k].x();
                    width = _lp[k+2].x() - _lp[k].x();
                  }
                else
                  {
                    xtop = _lp[k+2].x();
                    width = _lp[k].x() - _lp[k+2].x();
                  }
                if (_lp[k].y() < _lp[k+2].y())
                  {
                    ytop = _lp[k].y();
                    height = _lp[k+2].y() - _lp[k].y();
                  }
                else
                  {
                    ytop = _lp[k+2].y();
                    height = _lp[k].y() - _lp[k+2].y();
                  }
                QRectF aRect(mapToScene(QPointF(xtop, ytop)),
                             mapToScene(QPointF(xtop + width, ytop + height)));
                QList<QGraphicsItem*> itemList =  scene()->items(aRect);
                for (int kk=0; kk<itemList.size(); kk++)
                  {
                    QGraphicsItem *item = itemList[kk];
                    if ((dynamic_cast<SceneElementaryNodeItem*>(item)) ||
                        (dynamic_cast<SceneHeaderNodeItem*>(item)))
                      {
                        kmodif = false;
                        break;
                      }}

                if (kmodif)
                  {
                    modified = true;
                    _lp[k+1] = QPointF(tryx, tryy);
                    _directions[k+1] = _directions[k];
                    _directions[k+2] = _directions[k+3];
                  }
              }
          if (k>0)
            {
              if (prevdir == _directions[k])
                {
                  if ((prevdir == _RIGHT) || (prevdir == _LEFT))
                    newlp[newk].setX(_lp[k].x());
                  else
                    newlp[newk].setY(_lp[k].y());
                }
              else
                {
                  newk++;
                  newlp[newk] = _lp[k];
                  newdir[newk] = _directions[k];         
                }

            }
          prevdir = _directions[k];          
        }
      DEBTRACE("modified : _nbPoints=" << _nbPoints << " newk=" << newk+1);
      if (modified)
        {
//           for (int i=0; i<=newk; i++)
//             {
//               DEBTRACE("("<< _lp[i].x() << "," << _lp[i].y() << ") ("
//                        << newlp[i].x() << "," << newlp[i].y() << ") "
//                        << _directions[i] << "-" << newdir[i]);
//             }
//           for (int i=newk+1; i<_nbPoints; i++)
//             {
//               DEBTRACE("("<< _lp[i].x() << "," << _lp[i].y() << ") "
//                        << _directions[i]);
//             }
          _nbPoints = newk+1;
          for (int i=0; i<_nbPoints; i++)
            {
              _lp[i] = newlp[i];
              _directions[i] = newdir[i];
            }
          _directions[_nbPoints] = _RIGHT;
          DEBTRACE("_nbPoints " << _nbPoints);
        }
    }
}

void SceneLinkItem::force2points()
{
  if (_nbPoints == 1)
    {
      QPointF a = start();
      QPointF b = goal();
      if (a.y() != b.y())
        {
          vector<QPointF> newlp;
          vector<Direction> newdir;
          newlp.reserve(_nbPoints+1);
          newdir.reserve(_nbPoints +2);
          //_lp.reserve(_nbPoints +1);         // --- not OK : data may be lost! pre reserve enough before!
          //_directions.reserve(_nbPoints +2);
          newlp[0] = _lp[0];
          newlp[1] = _lp[0];
          newlp[0].setY(a.y());
          newlp[1].setY(b.y());
          newdir[0] = _RIGHT;
          newdir[2] = _RIGHT;
          if (a.y() > b.y())
            newdir[1] = _DOWN;
          else
            newdir[1] = _UP;
          _nbPoints = 2;
          for (int i=0; i<_nbPoints; i++)
            {
              _lp[i] = newlp[i];
              _directions[i] = newdir[i];
            }
          _directions[_nbPoints] = _RIGHT;
        }
    }
}


QPointF SceneLinkItem::start()
{
  SceneDataPortItem* dpif = dynamic_cast<SceneDataPortItem*>(_from);
  QPointF localFrom(dpif->getWidth()*(9.5/10), dpif->getHeight()/2);
  DEBTRACE("localFrom(" << localFrom.x() << "," << localFrom.y() << ")");
  return mapFromItem(dpif, localFrom);
}

QPointF SceneLinkItem::goal()
{
  SceneDataPortItem* dpit = dynamic_cast<SceneDataPortItem*>(_to);
  QPointF localTo(dpit->getWidth()/20, dpit->getHeight()/2);
  DEBTRACE("localTo(" << localTo.x() << "," << localTo.y() << ")");
  return mapFromItem(dpit, localTo);
}
