
#ifndef _SCENECTRLLINKITEM_HXX_
#define _SCENECTRLLINKITEM_HXX_

#include "SceneLinkItem.hxx"

#include <QPointF>
#include <vector>

namespace YACS
{
  namespace HMI
  {

    class SceneCtrlLinkItem: public SceneLinkItem
    {
    public:
      SceneCtrlLinkItem(QGraphicsScene *scene, SceneItem *parent,
                        ScenePortItem* from, ScenePortItem* to,
                        QString label, Subject *subject);
      virtual ~SceneCtrlLinkItem();

      virtual QPointF start();
      virtual QPointF goal();
    protected:

    };
  }
}

#endif
