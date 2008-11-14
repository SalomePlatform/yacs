
#ifndef _SCENEINPORTITEM_HXX_
#define _SCENEINPORTITEM_HXX_

#include "SceneDataPortItem.hxx"

namespace YACS
{
  namespace HMI
  {
    class SceneInPortItem: public SceneDataPortItem
    {
    public:
      SceneInPortItem(QGraphicsScene *scene, SceneItem *parent,
                      QString label, Subject *subject);
      virtual ~SceneInPortItem();
      virtual void popupMenu(QWidget *caller, const QPoint &globalPos);
    protected:
      void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
      void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
      void dropEvent(QGraphicsSceneDragDropEvent *event);
      virtual QColor getPenColor();
      virtual QColor getBrushColor();
      bool _dragOver;

    };
  }
}

#endif
