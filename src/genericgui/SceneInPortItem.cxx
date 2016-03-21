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

#include "SceneInPortItem.hxx"
#include "SceneOutPortItem.hxx"
#include "SceneNodeItem.hxx"
#include "ItemMimeData.hxx"
#include "Menus.hxx"
#include "Message.hxx"
#include "QtGuiContext.hxx"

#include <QGraphicsSceneDragDropEvent>

#include "Resource.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;


SceneInPortItem::SceneInPortItem(QGraphicsScene *scene, SceneItem *parent,
                                 QString label, Subject *subject)
  : SceneDataPortItem(scene, parent, label, subject)
{
  _dragOver = false;
  setAcceptDrops(true);
}

SceneInPortItem::~SceneInPortItem()
{
  if (SceneNodeItem* parent = getParent())
    parent->removeInPortFromList(this);
}

void SceneInPortItem::popupMenu(QWidget *caller, const QPoint &globalPos)
{
  InPortMenu m;
  m.popupMenu(caller, globalPos);
}

void SceneInPortItem::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
  bool accepted = false;
  const ItemMimeData *myData = dynamic_cast<const ItemMimeData*>(event->mimeData());
  if (myData && myData->hasFormat("yacs/subjectOutPort"))
    {
      Subject *sub = myData->getSubject();
      if (sub && (dynamic_cast<SubjectOutputPort*>(sub) ||
                  dynamic_cast<SubjectOutputDataStreamPort*>(sub))) 
        {
          event->setAccepted(true);
          _dragOver = true;
          QGraphicsItem::update();
          return;
        }
    }
  event->setAccepted(accepted);
}

void SceneInPortItem::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    Q_UNUSED(event);
    _dragOver = false;
    QGraphicsItem::update();
}

void SceneInPortItem::dropEvent(QGraphicsSceneDragDropEvent *event)
{
  Q_UNUSED(event);
  _dragOver = false;
  QGraphicsItem::update();

  const ItemMimeData *myData = dynamic_cast<const ItemMimeData*>(event->mimeData());
  if (!myData) return;
  if(!myData->hasFormat("yacs/subjectOutPort")) return;
  Subject *sub = myData->getSubject();
  if (!sub) return;
  SceneItem *item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[sub];
  if (!item) return;
  SceneOutPortItem* outItem = dynamic_cast<SceneOutPortItem*>(item);
  if (!outItem) return;

  Subject *subFrom = outItem->getSubject();
  Subject *subTo = this->getSubject();
  SubjectDataPort* from = dynamic_cast<SubjectDataPort*>(subFrom);
  SubjectDataPort* to = dynamic_cast<SubjectDataPort*>(subTo);
  if (from && to)
    if (!SubjectDataPort::tryCreateLink(from, to,myData->getControl()))
      Message mess;
}

QColor SceneInPortItem::getPenColor()
{
  if (_dragOver)
    return Resource::dragOver;
  if (isSelected())
    return _hiPenColor;
  else 
    return _penColor;
}

QColor SceneInPortItem::getBrushColor()
{
  if (_dragOver)
    return _hiBrushColor;

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
