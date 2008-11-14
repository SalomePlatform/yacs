
#ifndef _SCENEELEMENTARYNODEITEM_HXX_
#define _SCENEELEMENTARYNODEITEM_HXX_

#include "SceneNodeItem.hxx"

namespace YACS
{
  namespace HMI
  {
    class SceneElementaryNodeItem: public SceneNodeItem
    {
    public:
      SceneElementaryNodeItem(QGraphicsScene *scene, SceneItem *parent,
                             QString label, Subject *subject);
      virtual ~SceneElementaryNodeItem();

      virtual void paint(QPainter *painter,
                         const QStyleOptionGraphicsItem *option,
                         QWidget *widget);
      virtual void update(GuiEvent event, int type, Subject* son);
      virtual void popupMenu(QWidget *caller, const QPoint &globalPos);
      virtual void reorganize();
    protected:

    };
  }
}

#endif
