
#ifndef _SCENEOUTPORTITEM_HXX_
#define _SCENEOUTPORTITEM_HXX_

#include "SceneDataPortItem.hxx"

namespace YACS
{
  namespace HMI
  {
    class SceneOutPortItem: public SceneDataPortItem
    {
    public:
      SceneOutPortItem(QGraphicsScene *scene, SceneItem *parent,
                       QString label, Subject *subject);
      virtual ~SceneOutPortItem();
      virtual void popupMenu(QWidget *caller, const QPoint &globalPos);
    protected:
      virtual QString getMimeFormat();
    };
  }
}

#endif
