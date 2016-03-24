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

#include "SceneObserverItem.hxx"
#include "QtGuiContext.hxx"
#include "ItemMimeData.hxx"
#include "Scene.hxx"
#include "SchemaItem.hxx"
#include "Resource.hxx"

#include <QGraphicsSceneMouseEvent>
#include <QApplication>
#include <QDrag>
#include <QPixmap>
#include <QBitmap>
#include <QPainter>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;


SceneObserverItem::SceneObserverItem(QGraphicsScene *scene, SceneItem *parent,
                                     QString label, Subject *subject)
  : SceneItem(scene, parent, label), GuiObserver()
{
  _subject = subject;
  _draging = false;
  _dragModifier = false;
  _emphasized = false;
  _subject->attach(this);
  QtGuiContext::getQtCurrent()->_mapOfSceneItem[_subject]=this;
}

SceneObserverItem::~SceneObserverItem()
{
  QtGuiContext::getQtCurrent()->_mapOfSceneItem.erase(_subject);
}

void SceneObserverItem::update(GuiEvent event, int type, Subject* son)
{
  DEBTRACE(" SceneObserverItem::update " << eventName(event)<< " " << type << " " << son);
  switch (event)
    {
    case YACS::HMI::EMPHASIZE:
      DEBTRACE("SceneObserverItem::update EMPHASIZE " << type);
      if (type)
        _emphasized = true;
      else
        _emphasized = false;
      QGraphicsItem::update();
      break;
    default:
      ;
    }
}

void SceneObserverItem::select(bool isSelected)
{
  DEBTRACE("SceneObserverItem::select "  << _label.toStdString() << " " << isSelected);
  if (isSelected)
    {
//       _scene->clearSelection();
      setSelected(true);
      QtGuiContext::getQtCurrent()->setSelectedSubject(_subject);
    }
  else setSelected(false);
}

QString SceneObserverItem::getToolTip()
{
  DEBTRACE("SceneObserverItem::getToolTip");
  if (!_subject)
    return _label;
  if ( !QtGuiContext::getQtCurrent() || !QtGuiContext::getQtCurrent()->_mapOfSchemaItem.count(_subject))
    return _label;
  QString val ="";
  SchemaItem * item = QtGuiContext::getQtCurrent()->_mapOfSchemaItem[_subject];
  val = item->data(0, Qt::ToolTipRole).toString();
  return val;
}

void SceneObserverItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  DEBTRACE("SceneObserverItem::mousePressEvent " << _label.toStdString()
           << " " << acceptedMouseButtons ());
  if (!_scene->isZooming())
    {
      _subject->select(true);
      if (_dragable && (event->button() == _dragButton) && QtGuiContext::getQtCurrent()->isEdition())
        {
          setCursor(Qt::ClosedHandCursor);
          _draging = true;
          _dragModifier= event->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
        }
    }
}

/*!
 * creation of mime data if drag detected.
 * setData mime type must be coherent with SchemaModel::mimeTypes to allow drop 
 * on port item in tree view
 */
void SceneObserverItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
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
      mime->setSubject(_subject);
      mime->setData(getMimeFormat(), "_subject");
      if(_dragModifier)
        mime->setControl(false);
      else
        mime->setControl(true);
  
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
      //restore non drag state
      setCursor(Qt::ArrowCursor);
      _draging = false;
      _dragModifier = false;
    }
}

void SceneObserverItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  if (_draging)
    {
      setCursor(Qt::ArrowCursor);
    }
  _draging = false;
  _dragModifier = false;
}

QColor SceneObserverItem::getBrushColor()
{
  QColor color;
  color = _brushColor;
  if (isSelected())
    color = _hiBrushColor;
  if (_emphasized)
    color = Resource::emphasizeBrushColor;
  if (_hover)
    color = hoverColor(color);
  return color;
}

void SceneObserverItem::activateSelection(bool selected)
{
  DEBTRACE("SceneObserverItem::activateSelection " << _label.toStdString()<< " " << selected);
  _subject->select(true);
}

Subject* SceneObserverItem::getSubject()
{
  return _subject;
}

QString SceneObserverItem::getMimeFormat()
{
  return "yacs/subject";
}
