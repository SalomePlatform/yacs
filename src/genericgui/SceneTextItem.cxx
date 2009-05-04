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
#include "SceneTextItem.hxx"

// #include "QtGuiContext.hxx"
// #include "Menus.hxx"
// #include <QGraphicsSceneHoverEvent>
#include <QPointF>

// #include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;


SceneTextItem::SceneTextItem(QGraphicsScene *scene, SceneItem *parent,
                             QString label)
  : QGraphicsTextItem(label, parent), AbstractSceneItem(scene, parent, label)
{
  setToolTip(label);
  DEBTRACE("SceneTextItem::SceneTextItem "<<label.toStdString()<<" "<<this<<" "<<_parent<<" "<< _level); 
  setPos(x()+10, y());
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
}

void SceneTextItem::setTopLeft(QPointF topLeft)
{
}

void SceneTextItem::checkGeometryChange()
{
}

void SceneTextItem::popupMenu(QWidget *caller, const QPoint &globalPos)
{
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
