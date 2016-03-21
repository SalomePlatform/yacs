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

#ifndef _SCENELINKITEM_HXX_
#define _SCENELINKITEM_HXX_

#include "SceneObserverItem.hxx"
#include "LinkMatrix.hxx"

#include <QPointF>
#include <QPainterPath>
#include <vector>

#ifdef WIN32
#ifdef _UP
#undef _UP
#endif
#endif

namespace YACS
{
  namespace HMI
  {

    typedef enum
      {
        _UP    = 0,
        _RIGHT = 1,
        _DOWN  = 2,
        _LEFT  = 3
      } Direction;

    class ScenePortItem;
    class SceneNodeItem;

    class SceneLinkItem: public SceneObserverItem
    {
    public:
      SceneLinkItem(QGraphicsScene *scene, SceneItem *parent,
                    ScenePortItem* from, ScenePortItem* to,
                    QString label, Subject *subject);
      virtual ~SceneLinkItem();

      virtual void select(bool isSelected);
      virtual QRectF boundingRect() const;
      virtual QPainterPath shape() const;
      virtual void paint(QPainter *painter,
                         const QStyleOptionGraphicsItem *option,
                         QWidget *widget);
      virtual void update(GuiEvent event, int type, Subject* son);
      virtual void popupMenu(QWidget *caller, const QPoint &globalPos);
      virtual void setPath(LinkPath lp);
      virtual void minimizeDirectionChanges();
      virtual void force2points();
      virtual QPointF start();
      virtual QPointF goal();
      virtual void updateShape();
      virtual SceneNodeItem* getFromNode();
      virtual SceneNodeItem* getToNode();

    protected:
      void setShape(int thickness = 1);
      void addArrow(QPointF pfrom,
                    QPointF pto,
                    HMI::Direction dir,
                    int thickness = 1);
      virtual QColor getBrushColor();
      virtual QColor getPenColor();

      QPainterPath _path;
      ScenePortItem* _from;
      ScenePortItem* _to;
      std::vector<QPointF> _lp;
      std::vector<Direction> _directions;
      int _nbPoints;
    };
  }
}

#endif
