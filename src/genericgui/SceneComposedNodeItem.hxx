//  Copyright (C) 2006-2010  CEA/DEN, EDF R&D
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

#ifndef _SCENECOMPOSEDNODEITEM_HXX_
#define _SCENECOMPOSEDNODEITEM_HXX_

#include "SceneNodeItem.hxx"

namespace YACS
{
  namespace HMI
  {
    class SceneComposedNodeItem: public SceneNodeItem
    {
    public:
      SceneComposedNodeItem(QGraphicsScene *scene, SceneItem *parent,
                            QString label, Subject *subject);
      virtual ~SceneComposedNodeItem();

      virtual QRectF childrenBoundingRect() const;
      virtual void paint(QPainter *painter,
                         const QStyleOptionGraphicsItem *option,
                         QWidget *widget);
      virtual void update(GuiEvent event, int type, Subject* son);
      virtual void autoPosNewChild(AbstractSceneItem *item,
                                   const std::list<AbstractSceneItem*> alreadySet,
                                   bool isNew = false);
      virtual void popupMenu(QWidget *caller, const QPoint &globalPos);
      virtual std::list<AbstractSceneItem*> getChildren();
      virtual void removeChildFromList(AbstractSceneItem* child);
      virtual void reorganize();
      virtual void reorganizeShrinkExpand();
      virtual void shrinkExpandRecursive(bool isExpanding, bool fromHere);
      virtual void shrinkExpandLink(bool se);
      virtual void collisionResolv(SceneItem* child, QPointF oldPos);
      virtual void rebuildLinks();
      virtual void arrangeNodes(bool isRecursive);
      virtual void arrangeChildNodes();
      virtual void adjustColors();
      virtual void setShownState(shownState ss);
    protected:
      void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
      void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
      void dropEvent(QGraphicsSceneDragDropEvent *event);
      virtual QColor getPenColor();
      virtual QColor getBrushColor();

      std::list<AbstractSceneItem*> _children;      
      bool _dragOver;
    };
  }
}

#endif
