
#ifndef _SCENECTRLINPORTITEM_HXX_
#define _SCENECTRLINPORTITEM_HXX_

#include "SceneCtrlPortItem.hxx"

namespace YACS
{
  namespace HMI
  {
    class SceneCtrlInPortItem: public SceneCtrlPortItem
    {
    public:
      SceneCtrlInPortItem(QGraphicsScene *scene, SceneItem *parent,
                          QString label);
      virtual ~SceneCtrlInPortItem();
      virtual void popupMenu(QWidget *caller, const QPoint &globalPos);
    protected:
      void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
      void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
      void dropEvent(QGraphicsSceneDragDropEvent *event);
      bool _dragOver;

    };
  }
}

#endif
