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

#include "SceneCtrlPortItem.hxx"
#include "SceneTextItem.hxx"
#include "SceneNodeItem.hxx"
#include "SceneHeaderNodeItem.hxx"
#include "Scene.hxx"
#include "ItemMimeData.hxx"
#include "QtGuiContext.hxx"

// #include "QtGuiContext.hxx"
#include "Menus.hxx"
#include <QGraphicsSceneMouseEvent>
#include <QApplication>
#include <QDrag>
#include <QPixmap>
#include <QBitmap>
// #include <QGraphicsSceneHoverEvent>
// #include <QPointF>

// #include <cassert>

#include "Resource.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;

SceneCtrlPortItem::SceneCtrlPortItem(QGraphicsScene *scene, SceneItem *parent,
                                     QString label)
  : SceneItem(scene, parent, label), ScenePortItem(label)
{
  setText(label);
  _width        = Resource::CtrlPort_Width;
  _height       = Resource::CtrlPort_Height;
  _brushColor   = Resource::CtrlPort_brush;
  _hiBrushColor = Resource::CtrlPort_hiBrush;
  _penColor     = Resource::CtrlPort_pen;
  _hiPenColor   = Resource::CtrlPort_hiPen;
}

SceneCtrlPortItem::~SceneCtrlPortItem()
{
}

void SceneCtrlPortItem::paint(QPainter *painter,
                          const QStyleOptionGraphicsItem *option,
                          QWidget *widget)
{
  //DEBTRACE("ScenePortItem::paint");
  painter->save();

  QPen pen(getPenColor());
  pen.setWidth(Resource::Thickness);
  painter->setPen(pen);
  SceneHeaderNodeItem* hd = dynamic_cast<SceneHeaderNodeItem*>(_parent);
  painter->setBrush(hd->getValidColor());
  painter->drawRoundedRect(QRectF(0, 0, Resource::CtrlPort_Width, Resource::CtrlPort_Height), Resource::Radius, Resource::Radius);

  painter->restore();
}

void SceneCtrlPortItem::setText(QString label)
{
  if (!_text)
    _text = new SceneTextItem(_scene,
                              this,
                              label, true );
}

SceneNodeItem* SceneCtrlPortItem::getParentNode()
{
  if (_parent)
    return dynamic_cast<SceneNodeItem*>(_parent->getParent());
  else
    return 0;
}

SubjectNode* SceneCtrlPortItem::getSubjectNode()
{
  SceneNodeItem* nodeItem = getParentNode();
  if (!nodeItem) return 0;
  return dynamic_cast<SubjectNode*>(nodeItem->getSubject());
}

QString SceneCtrlPortItem::getMimeFormat()
{
  return "yacs/subjectGate";
}

void SceneCtrlPortItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  DEBTRACE("SceneCtrlPortItem::mousePressEvent " << _label.toStdString()
           << " " << acceptedMouseButtons ());
  if (!_scene->isZooming())
    {
      getSubjectNode()->select(true);
      if (_dragable && (event->button() == _dragButton) && QtGuiContext::getQtCurrent()->isEdition())
        {
          setCursor(Qt::ClosedHandCursor);
          _draging = true;
          _dragModifier= event->modifiers() & Qt::ControlModifier;
        }
    }
}

/*!
 * creation of mime data if drag detected.
 * setData mime type must be coherent with SchemaModel::mimeTypes to allow drop 
 * on port item in tree view
 */
void SceneCtrlPortItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
  if (_draging)
    {
      //DEBTRACE("_draging");
      if (QLineF(event->screenPos(), 
                 event->buttonDownScreenPos(_dragButton)).length()
          < QApplication::startDragDistance())
        {
          return;
        }
      DEBTRACE("in drag");
      QDrag *drag = new QDrag(event->widget());
      ItemMimeData *mime = new ItemMimeData;
      drag->setMimeData(mime);
      mime->setSubject(getSubjectNode());
      mime->setData(getMimeFormat(), "_subject");
  
      QPixmap pixmap(34, 34);
      pixmap.fill(Qt::white);
      
      QPainter painter(&pixmap);
      painter.translate(15, 15);
      painter.setRenderHint(QPainter::Antialiasing);
      paint(&painter, 0, 0);
      painter.end();
      
      pixmap.setMask(pixmap.createHeuristicMask());
      
      drag->setPixmap(pixmap);
      drag->setHotSpot(QPoint(15, 20));
      
      drag->exec();
      setCursor(Qt::ArrowCursor);
      _draging = false;
      _dragModifier= false;
    }
}

void SceneCtrlPortItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  if (_draging)
    {
      setCursor(Qt::ArrowCursor);
    }
  _draging = false;
  _dragModifier= false;
}
