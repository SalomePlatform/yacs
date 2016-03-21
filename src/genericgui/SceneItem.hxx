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

#ifndef _SCENEITEM_HXX_
#define _SCENEITEM_HXX_

#include <QGraphicsItem>
#include <QGraphicsTextItem>
#include <QGraphicsScene>
#include <QString>
#include <QPainter>
#include <QEvent>

#include "guiObservers.hxx"

namespace YACS
{
  namespace HMI
  {
    typedef enum
      {
        CurrentNode,
        ChildrenNodes,
        ElementaryNodes
      } ShrinkMode;
    
    class Scene;

    class RootSceneItem: public GuiObserver
    {
    public:
      RootSceneItem(YACS::HMI::Subject *context);
      virtual ~RootSceneItem();
      virtual void update(GuiEvent event, int type, Subject* son);
    protected:
      void setNewRoot(YACS::HMI::Subject *root);
      Subject *_context;
      Subject *_root;
    };

    // ----------------------------------------------------------------------------

    class SceneItem;
    /*!
     * Base class for 2D graphics item, does not herit neither from QGraphicsItem,
     * nor from GuiObserver. not instantiable. See derived classes.
     */
    class AbstractSceneItem
    {
    public:
      AbstractSceneItem(QGraphicsScene *scene, SceneItem *parent,
                        QString label);
      virtual ~AbstractSceneItem();
      
      virtual QRectF boundingRect() const = 0;
      virtual void paint(QPainter *painter,
                         const QStyleOptionGraphicsItem *option,
                         QWidget *widget) = 0;

      virtual void setTopLeft(QPointF topLeft) = 0;
      int getLevel();
      void setLevel();
      virtual void checkGeometryChange() = 0;
      virtual void reorganize();
      virtual QString getLabel();
      virtual void addHeader();
      virtual void addProgressItem();
      virtual qreal getHeaderBottom();
      qreal getWidth();
      qreal getHeight();
      virtual void setWidth(qreal width);
      virtual void setHeight(qreal height);
      virtual void popupMenu(QWidget *caller, const QPoint &globalPos) = 0;
      virtual void activateSelection(bool selected);
      virtual void setGeometryOptimization(bool optimize);
      inline SceneItem* getParent() { return _parent; };

    protected:
      virtual QRectF childBoundingRect(AbstractSceneItem *child) const;

      SceneItem *_parent;
      YACS::HMI::Scene *_scene;
      QString _label;
      int _level;
      qreal _width;
      qreal _height;
      qreal _incHeight;
      QColor _penColor;
      QColor _brushColor;
      QColor _hiPenColor;
      QColor _hiBrushColor;
      bool _hasHeader;
      bool _optimize;
      bool _dragable;
      enum Qt::MouseButton _dragButton;
    };

    // ----------------------------------------------------------------------------

    /*!
     * Generic class for 2D graphics item, inherits QGraphicsItem,
     * specialised in 2 kind of classes, with or without inheritance from GuiObserver.
     */
    class SceneItem: public QGraphicsItem, public AbstractSceneItem
    {
    public:
      SceneItem(QGraphicsScene *scene, SceneItem *parent,
                QString label);
      virtual ~SceneItem();
      
      virtual QRectF boundingRect() const;
      virtual QRectF childrenBoundingRect () const;
      virtual void paint(QPainter *painter,
                         const QStyleOptionGraphicsItem *option,
                         QWidget *widget);
      virtual void setTopLeft(QPointF topLeft);
      virtual void checkGeometryChange();
      virtual void popupMenu(QWidget *caller, const QPoint &globalPos);
      void setParent(SceneItem* parent);
      virtual QString getToolTip();
      void setEventPos(QPointF point);
      virtual void updateChildItems();
      virtual void updateLinks();
      virtual void shrinkExpandLink(bool se);
      virtual void shrinkExpandRecursive(bool isExpanding, bool fromHere, ShrinkMode theShrinkMode);
      bool isAncestorShrinked() { return _ancestorShrinked; };
      bool _blocX;
      bool _blocY;

    protected:
//       virtual bool sceneEvent(QEvent *event);
      virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
      virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
      virtual void hoverMoveEvent(QGraphicsSceneHoverEvent * event);
      virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
      virtual QColor getPenColor();
      virtual QColor getBrushColor();
      QColor hoverColor(QColor origColor);
      bool _hover;
      bool _ancestorShrinked;
      QPointF _eventPos;
    };

  }
}

#endif
