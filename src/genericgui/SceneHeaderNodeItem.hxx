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

#ifndef _SCENEHEADERNODEITEM_HXX_
#define _SCENEHEADERNODEITEM_HXX_

#include "SceneHeaderItem.hxx"
#include "SceneNodeItem.hxx"

namespace YACS
{
  namespace HMI
  {
    class SceneInPortItem;
    class SceneOutPortItem;
    class SceneCtrlPortItem;

    class SceneHeaderNodeItem: public SceneHeaderItem
    {
    public:
      SceneHeaderNodeItem(QGraphicsScene *scene, SceneItem *parent,
                      QString label);
      virtual ~SceneHeaderNodeItem();

      virtual void paint(QPainter *painter,
                         const QStyleOptionGraphicsItem *option,
                         QWidget *widget);
      virtual void setText(QString label);
      virtual qreal getHeaderBottom() const;
      virtual void autoPosControl(AbstractSceneItem *item);
      virtual void autoPosNewPort(AbstractSceneItem *item);
      virtual void reorganizePorts(shownState ss);
      virtual void popupMenu(QWidget *caller, const QPoint &globalPos);
      virtual void adjustGeometry();
      virtual void adjustPosPorts();
      virtual QRectF getMinimalBoundingRect() const;
      virtual void adaptComposedNode(SceneItem* port, qreal deltaY);
      virtual void setEdited(bool isEdited);
      virtual void setValid(bool isValid);
      virtual void setExecState(int execState);
      virtual SceneCtrlPortItem* getCtrlInPortItem();
      virtual SceneCtrlPortItem* getCtrlOutPortItem();
      QColor getValidColor();

    protected:
      virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
      virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

      SceneNodeItem* _fatherNode;
      SceneHeaderItem *_header;
      SceneCtrlPortItem *_controlIn;
      SceneCtrlPortItem *_controlOut;
      std::list<SceneInPortItem*> _inPorts;
      std::list<SceneOutPortItem*> _outPorts;
      int _maxPorts;

      QColor _sc;
      QString _stateDef;
      int _execState;
      bool _isProc;

      bool _isValid;
      bool _isEdited;
    };
  }
}

#endif
