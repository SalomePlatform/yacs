
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
      virtual void collisionResolv(SceneItem* child, QPointF oldPos);
      virtual void rebuildLinks();
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
