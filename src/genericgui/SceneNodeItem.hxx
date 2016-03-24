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

#ifndef _SCENENODEITEM_HXX_
#define _SCENENODEITEM_HXX_

#include "SceneObserverItem.hxx"

#include <QString>

namespace YACS
{
  namespace HMI
  {
    typedef enum
      {
        expandShown,
        shrinkShown,
        shrinkHidden
      } shownState;
    
    class SceneHeaderItem;
    class SceneHeaderNodeItem;
    class SceneComposedNodeItem;
    class ScenePortItem;
    class SceneProgressItem;

    class SceneNodeItem: public SceneObserverItem
    {
    public:
      SceneNodeItem(QGraphicsScene *scene, SceneItem *parent,
                    QString label, Subject *subject);
      virtual ~SceneNodeItem();

      virtual void setWidth(qreal width);
      virtual void setHeight(qreal height);
      virtual void addHeader();
      virtual SceneHeaderItem* getHeader();
      virtual void addProgressItem();
      virtual SceneProgressItem* getProgressItem() { return _progressItem; };
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
      virtual void arrangeNodes(bool isRecursive);
      virtual void arrangeChildNodes();
      virtual void reorganizeShrinkExpand(ShrinkMode theShrinkMode);
      virtual void updateChildItems();
      virtual void shrinkExpandLink(bool se);
      virtual void showOutScopeLinks();
      virtual void updateLinks();
      virtual void setShownState(shownState ss);
      bool isExpanded() { return _expanded; };
      void setExpanded(bool e){ _expanded = e; };
      void setExpandedPos(QPointF epos) { _expandedPos = epos; };
      qreal getExpandedX() { return _expandedPos.x(); };
      qreal getExpandedY() { return _expandedPos.y(); };
      void setExpandedWH() {_expandedWidth = _width; _expandedHeight = _height; };
      void setExpandedWH(qreal w, qreal h) {_expandedWidth = w; _expandedHeight = h; };
      qreal getExpandedWidth() { return _expandedWidth; };
      qreal getExpandedHeight() { return _expandedHeight; };
      shownState getShownState() {return _shownState; };
      bool hasProgressBar() const;

    protected:
      virtual QString getMimeFormat();
      virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
      virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
      virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
      virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
      virtual void updateState();
      virtual void setExecState(int execState);

      QString getHeaderLabel();

      std::list<AbstractSceneItem*> _inPorts;
      std::list<AbstractSceneItem*> _outPorts;    
      SceneHeaderNodeItem *_header;
      SceneProgressItem *_progressItem;
      int _execState;
      bool _moving;
      bool _moved;
      bool _expanded;
      QPointF _prevPos;
      QPointF _expandedPos;
      qreal _expandedWidth;
      qreal _expandedHeight;
      shownState _shownState;
    };
  }
}

#endif
