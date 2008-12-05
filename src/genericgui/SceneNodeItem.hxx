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
#ifndef _SCENENODEITEM_HXX_
#define _SCENENODEITEM_HXX_

#include "SceneObserverItem.hxx"

#include <QString>

namespace YACS
{
  namespace HMI
  {
    class SceneHeaderItem;
    class SceneHeaderNodeItem;
    class SceneComposedNodeItem;
    class ScenePortItem;

    class SceneNodeItem: public SceneObserverItem
    {
    public:
      SceneNodeItem(QGraphicsScene *scene, SceneItem *parent,
                    QString label, Subject *subject);
      virtual ~SceneNodeItem();

      virtual void addHeader();
      virtual SceneHeaderItem* getHeader();
      virtual void paint(QPainter *painter,
                         const QStyleOptionGraphicsItem *option,
                         QWidget *widget);
      virtual void update(GuiEvent event, int type, Subject* son);
      virtual qreal getHeaderBottom();
      virtual void autoPosNewPort(AbstractSceneItem *item, int nbPorts);
      virtual void popupMenu(QWidget *caller, const QPoint &globalPos);
      virtual SceneComposedNodeItem* getParent();
      virtual void removeInPortFromList(AbstractSceneItem* inPort);
      virtual void removeOutPortFromList(AbstractSceneItem* outPort);
      virtual void setMoving(bool moving);
      virtual void setTopLeft(QPointF topLeft);
      virtual void adjustHeader();
      virtual ScenePortItem* getCtrlInPortItem();
      virtual ScenePortItem* getCtrlOutPortItem();
      virtual void updateName();

    protected:
      virtual QString getMimeFormat();
      virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
      virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
      virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
      virtual void updateState();
      virtual void setExecState(int execState);

      QString getHeaderLabel();

      std::list<AbstractSceneItem*> _inPorts;
      std::list<AbstractSceneItem*> _outPorts;    
      SceneHeaderNodeItem *_header;
      int _execState;
      bool _moving;
      QPointF _prevPos;
    };
  }
}

#endif
