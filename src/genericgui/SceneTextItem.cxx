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

#include "SceneTextItem.hxx"

// #include "QtGuiContext.hxx"
// #include "Menus.hxx"
// #include <QGraphicsSceneHoverEvent>
#include <QPointF>

#include "Resource.hxx"

// #include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;


SceneTextItem::SceneTextItem(QGraphicsScene *scene, SceneItem *parent,
                             QString label, bool center )
  : QGraphicsTextItem(parent), AbstractSceneItem(scene, parent, label)
{
  setToolTip(label);
  _center = center;
  DEBTRACE("SceneTextItem::SceneTextItem "<<label.toStdString()<<" "<<this<<" "<<_parent<<" "<< _level); 
}

SceneTextItem::~SceneTextItem()
{
}

QRectF SceneTextItem::boundingRect() const
{
  return QGraphicsTextItem::boundingRect();
}
void SceneTextItem::paint(QPainter *painter,
           const QStyleOptionGraphicsItem *option,
           QWidget *widget)
{
  QGraphicsTextItem::paint(painter, option, widget);

  int dx, fl;
  if (_center) {
    dx = 0;
    fl = Qt::AlignCenter;
  } else {
    dx = Resource::Text_DX;
    fl = Qt::AlignLeft | Qt::AlignVCenter;
  };
  painter->drawText(dx, 0, _parent->getWidth()-dx*2, _parent->getHeight(), fl, _label);
}

void SceneTextItem::setTopLeft(QPointF topLeft)
{
}

void SceneTextItem::setPlainTextTrunc(QString label) {
  _label = label;
}

void SceneTextItem::checkGeometryChange()
{
}

void SceneTextItem::popupMenu(QWidget *caller, const QPoint &globalPos)
{
  DEBTRACE("SceneTextItem::popupMenu " << _parent);
  if (_parent) _parent->popupMenu(caller, globalPos);
}

/*! generic behaviour for headers:
 *  obtain the tooltip from parent.
 */
QString SceneTextItem::getToolTip()
{
  QString tooltip = _label;
  SceneItem *parent = getParent();
  if (parent)
    tooltip = parent->getToolTip();
  return tooltip;
}

void SceneTextItem::setEventPos(QPointF point)
{
  _eventPos = mapFromScene(point);
}
