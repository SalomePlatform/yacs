
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
      virtual int getLevel();
      virtual qreal getMargin();
      virtual void checkGeometryChange() = 0;
      virtual void reorganize();
      virtual QString getLabel();
      virtual void addHeader();
      virtual qreal getHeaderBottom();
      virtual qreal getWidth();
      virtual qreal getHeight();
      virtual qreal getInternWidth();
      virtual qreal getInternHeight();
      virtual void popupMenu(QWidget *caller, const QPoint &globalPos) = 0;
      virtual void activateSelection(bool selected);
      virtual void setGeometryOptimization(bool optimize);
      inline bool hasNml() { return _hasNml; };
      inline SceneItem* getParent() { return _parent; };

    protected:
      virtual QRectF childBoundingRect(AbstractSceneItem *child) const;

      SceneItem *_parent;
      YACS::HMI::Scene *_scene;
      QString _label;
      int _level;
      qreal _margin;
      qreal _nml;
      qreal _width;
      qreal _height;
      QColor _penColor;
      QColor _brushColor;
      QColor _hiPenColor;
      QColor _hiBrushColor;
      bool _hasHeader;
      bool _hasNml;
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

    protected:
//       virtual bool sceneEvent(QEvent *event);
      virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
      virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
      virtual void hoverMoveEvent(QGraphicsSceneHoverEvent * event);
      virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
      virtual QColor getPenColor();
      virtual QColor getBrushColor();
    };

  }
}

#endif
