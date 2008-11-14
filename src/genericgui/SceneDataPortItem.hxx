
#ifndef _SCENEDATAPORTITEM_HXX_
#define _SCENEDATAPORTITEM_HXX_

#include "SceneObserverItem.hxx"
#include "ScenePortItem.hxx"

namespace YACS
{
  namespace HMI
  {
    class SceneNodeItem;

    class SceneDataPortItem: public SceneObserverItem, public ScenePortItem
    {
    public:
      SceneDataPortItem(QGraphicsScene *scene, SceneItem *parent,
                    QString label, Subject *subject);
      virtual ~SceneDataPortItem();

      virtual void setText(QString label);
      virtual void paint(QPainter *painter,
                         const QStyleOptionGraphicsItem *option,
                         QWidget *widget);
      virtual void update(GuiEvent event, int type, Subject* son);
      virtual SceneNodeItem* getParent();
    protected:
    };
  }
}

#endif
