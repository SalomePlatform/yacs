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

#include "SceneCtrlInPortItem.hxx"
#include "SceneNodeItem.hxx"
#include "ItemMimeData.hxx"

#include "QtGuiContext.hxx"
#include "Menus.hxx"
#include "Message.hxx"

#include <QGraphicsSceneDragDropEvent>


//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;

SceneCtrlInPortItem::SceneCtrlInPortItem(QGraphicsScene *scene, SceneItem *parent,
                                         QString label)
  : SceneCtrlPortItem(scene, parent, label)
{
  _dragOver = false;
  setAcceptDrops(true);
}

SceneCtrlInPortItem::~SceneCtrlInPortItem()
{
}

void SceneCtrlInPortItem::popupMenu(QWidget *caller, const QPoint &globalPos)
{
  CtrlInPortMenu m;
  m.popupMenu(caller, globalPos);
}

void SceneCtrlInPortItem::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
  bool accepted = false;
  const ItemMimeData *myData = dynamic_cast<const ItemMimeData*>(event->mimeData());
  if (myData && myData->hasFormat("yacs/subjectOutGate"))
    {
      Subject *sub = myData->getSubject();
      if (sub && dynamic_cast<SubjectNode*>(sub)) 
        {
          event->setAccepted(true);
          _dragOver = true;
          QGraphicsItem::update();
          return;
        }
    }
  event->setAccepted(accepted);
}

void SceneCtrlInPortItem::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    Q_UNUSED(event);
    _dragOver = false;
    QGraphicsItem::update();
}

void SceneCtrlInPortItem::dropEvent(QGraphicsSceneDragDropEvent *event)
{
  Q_UNUSED(event);
  _dragOver = false;
  QGraphicsItem::update();

  const ItemMimeData *myData = dynamic_cast<const ItemMimeData*>(event->mimeData());
  if (!myData) return;
  if(!myData->hasFormat("yacs/subjectOutGate")) return;
  Subject *sub = myData->getSubject();
  if (!sub) return;
  SceneItem *item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[sub];
  if (!item) return;
  SceneNodeItem* outItem = dynamic_cast<SceneNodeItem*>(item);
  if (!outItem) return;

  Subject *subFrom = outItem->getSubject();
  if (!this->getParentNode()) return;
  Subject *subTo = this->getParentNode()->getSubject();
  SubjectNode* from = dynamic_cast<SubjectNode*>(subFrom);
  SubjectNode* to = dynamic_cast<SubjectNode*>(subTo);
  if (from && to)
    {
      if (!SubjectNode::tryCreateLink(from, to))
        Message mess;
    }
}
