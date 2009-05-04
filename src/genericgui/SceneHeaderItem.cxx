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
#include "SceneHeaderItem.hxx"
#include "SceneHeaderNodeItem.hxx"
#include "SceneTextItem.hxx"
#include "Scene.hxx"
#include <QGraphicsSceneMouseEvent>

// #include "QtGuiContext.hxx"
// #include "Menus.hxx"
// #include <QGraphicsSceneHoverEvent>
// #include <QPointF>

#include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;


SceneHeaderItem::SceneHeaderItem(QGraphicsScene *scene, SceneItem *parent,
                                 QString label)
  : SceneItem(scene, parent, label)
{
  YASSERT(_parent);
  _height = 25;
  _width = _parent->getInternWidth();
  _text=0;
  _brushColor   = QColor(215,220,238);
  _hiBrushColor = QColor(161,178,238);
//   _penColor     = QColor(120,120,120);
//   _hiPenColor   = QColor( 60, 60, 60);
  _hasNml = false;
}

SceneHeaderItem::~SceneHeaderItem()
{
}

QRectF SceneHeaderItem::getMinimalBoundingRect() const
{
  return QRectF(x(), y(), 5, _height);
}

void SceneHeaderItem::paint(QPainter *painter,
                            const QStyleOptionGraphicsItem *option,
                            QWidget *widget)
{
//   DEBTRACE("SceneHeaderItem::paint");
  painter->save();
  painter->setPen(getPenColor());
  painter->setBrush(getBrushColor());
  painter->drawRoundRect(QRectF(0, 0, _width, _height), 33*_height/_width, 33);
  painter->restore();
}

void SceneHeaderItem::setText(QString label)
{
  if (!_text)
    _text = new SceneTextItem(_scene,
                              this,
                              label);
  else
    _text->setPlainText(label);
}

void SceneHeaderItem::popupMenu(QWidget *caller, const QPoint &globalPos)
{
  if (_parent) _parent->popupMenu(caller, globalPos);
}

void SceneHeaderItem::adjustGeometry()
{
  prepareGeometryChange();
  _width = _parent->getInternWidth() -1;
  update();
}

QColor SceneHeaderItem::getPenColor()
{
QColor color = _penColor;
 if (getParent())
   if(getParent()->getParent())
     if (getParent()->getParent()->isSelected())
       color = _hiPenColor;
 return color;
}

QColor SceneHeaderItem::getBrushColor()
{
  QColor color = _brushColor;
  if (dynamic_cast<SceneHeaderNodeItem*>(getParent()))
    if (getParent()->getParent()->isSelected())
      color = _hiBrushColor;
  if (_hover)
    color = hoverColor(color);
  return color;
}

void SceneHeaderItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  event->ignore();
}
