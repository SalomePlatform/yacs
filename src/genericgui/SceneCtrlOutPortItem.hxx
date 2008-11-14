
#ifndef _SCENECTRLOUTPORTITEM_HXX_
#define _SCENECTRLOUTPORTITEM_HXX_

#include "SceneCtrlPortItem.hxx"

namespace YACS
{
  namespace HMI
  {
    class SceneCtrlOutPortItem: public SceneCtrlPortItem
    {
    public:
      SceneCtrlOutPortItem(QGraphicsScene *scene, SceneItem *parent,
                           QString label);
      virtual ~SceneCtrlOutPortItem();
      virtual void popupMenu(QWidget *caller, const QPoint &globalPos);
    protected:
      virtual QString getMimeFormat();

    };
  }
}

#endif
