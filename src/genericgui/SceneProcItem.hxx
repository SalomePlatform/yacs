#ifndef _SCENEPROCITEM_HXX_
#define _SCENEPROCITEM_HXX_

#include "SceneComposedNodeItem.hxx"

namespace YACS
{
  namespace HMI
  {
    class SceneProcItem: public SceneComposedNodeItem
    {
    public:
      SceneProcItem(QGraphicsScene *scene, SceneItem *parent,
                    QString label, Subject *subject);
      virtual ~SceneProcItem();
      virtual void popupMenu(QWidget *caller, const QPoint &globalPos);
    };
  }
}
#endif
