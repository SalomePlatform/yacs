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

#include "SceneProgressItem.hxx"
#include "SceneHeaderNodeItem.hxx"
#include "SceneTextItem.hxx"
#include "Scene.hxx"
#include <QGraphicsSceneMouseEvent>

#include "Resource.hxx"

#include <cassert>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;


SceneProgressItem::SceneProgressItem(QGraphicsScene *scene, SceneItem *parent,
                                 QString label)
  : SceneItem(scene, parent, label)
{
  YASSERT(_parent);
  _progress = 0;
  _width  = Resource::Corner_Margin + 2*Resource::DataPort_Width + 2*Resource::Space_Margin;
  _height = Resource::progressBar_Height;
  _text=0;
  _tooltip = "";
  _brushColor   = Resource::Header_brush;
  _hiBrushColor = Resource::Header_hiBrush;
  _penColor     = Resource::Header_pen;
  _hiPenColor   = Resource::Header_hiPen;
  int x = Resource::Border_Margin;
  int y = Resource::DataPort_Height + 2*Resource::Space_Margin;
  setTopLeft(QPointF(x, y));
}

SceneProgressItem::~SceneProgressItem()
{
}

QRectF SceneProgressItem::getMinimalBoundingRect() const
{
  return QRectF(x(), y(), _width, _height);
}

void SceneProgressItem::paint(QPainter *painter,
                            const QStyleOptionGraphicsItem *option,
                            QWidget *widget)
{
//   DEBTRACE("SceneProgressItem::paint");
  painter->save();

  int w = Resource::Corner_Margin + 2*Resource::DataPort_Width + 2*Resource::Space_Margin;
  if (_parent->getWidth() > w) w = _parent->getWidth() - Resource::Corner_Margin - Resource::Space_Margin;
  int h = Resource::progressBar_Height;
  QPen pen(getPenColor());
  pen.setWidth(Resource::Thickness);
  painter->setPen(pen);
  QRect boundRect(0, 0, w, h);
  painter->drawRect(boundRect);
  
  painter->setBrush(Resource::progressBarColor);
  //correct width according to progress
  int corr_w = w * _progress / 100;
  painter->drawRect(QRect(0, 0, corr_w, h));
  painter->restore();
}

void SceneProgressItem::setProgress(QString newProgress)
{
  QString percentageLabel;
  QString nbStepsLabel = "-/-";
  QStringList aSteps = newProgress.split('/');
  if (aSteps.count() == 2)
  { //case '5/10' view of progress
    _progress = aSteps.at(0).toInt() * 100 / aSteps.at(1).toInt();
    nbStepsLabel = newProgress;
  }
  else
  { //case '50' view of progress
    _progress = newProgress.toInt(); //set 0 if the conversion fails.
  }
  percentageLabel = QString("%1\%").arg(_progress);
  QString resultLabel;
  switch(Resource::progressBarLabel)
  {
    case 0: //Percentage: 50%
      resultLabel = QString("%1").arg(percentageLabel);
      break;
    case 1: //Nb.steps:   5/10
      resultLabel = QString("%1").arg(nbStepsLabel);
      break;
    case 2: //Both:       50% (5/10)
      resultLabel = QString("%1 (%2)").arg(percentageLabel).arg(nbStepsLabel);
      break;
  }
  setText(resultLabel);
  _tooltip = QString("%1 (%2)").arg(percentageLabel).arg(nbStepsLabel);
  update();
}

void SceneProgressItem::setText(QString label)
{
  if (!_text)
    _text = new SceneTextItem(_scene, this, label, true);
  else
    _text->setPlainTextTrunc(label);
  //QGraphicsItem::update();
}

void SceneProgressItem::popupMenu(QWidget *caller, const QPoint &globalPos)
{
  if (_parent) _parent->popupMenu(caller, globalPos);
}

void SceneProgressItem::adjustGeometry()
{
  prepareGeometryChange();
  _width = _parent->getWidth() - Resource::Corner_Margin - Resource::Space_Margin;
  update();
}

QColor SceneProgressItem::getPenColor()
{
  return _hiPenColor;
}

QColor SceneProgressItem::getBrushColor()
{
  QColor color = _brushColor;
  if (dynamic_cast<SceneHeaderNodeItem*>(this))
    if (getParent()->isSelected())
      color = _hiBrushColor;
  if (_hover)
    color = hoverColor(color);
  return color;
}

void SceneProgressItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  event->ignore();
}

QString SceneProgressItem::getToolTip()
{
  return _tooltip;;
}
