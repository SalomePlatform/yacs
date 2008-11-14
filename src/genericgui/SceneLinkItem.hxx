
#ifndef _SCENELINKITEM_HXX_
#define _SCENELINKITEM_HXX_

#include "SceneObserverItem.hxx"
#include "LinkMatrix.hxx"

#include <QPointF>
#include <vector>

namespace YACS
{
  namespace HMI
  {

    typedef enum
      {
        _UP    = 0,
        _RIGHT = 1,
        _DOWN  = 2,
        _LEFT  = 3
      } Direction;

    class ScenePortItem;

    class SceneLinkItem: public SceneObserverItem
    {
    public:
      SceneLinkItem(QGraphicsScene *scene, SceneItem *parent,
                    ScenePortItem* from, ScenePortItem* to,
                    QString label, Subject *subject);
      virtual ~SceneLinkItem();

      virtual void select(bool isSelected);
      virtual void paint(QPainter *painter,
                         const QStyleOptionGraphicsItem *option,
                         QWidget *widget);
      virtual void update(GuiEvent event, int type, Subject* son);
      virtual void popupMenu(QWidget *caller, const QPoint &globalPos);
      virtual void setPath(LinkPath lp);
      virtual void minimizeDirectionChanges();
      virtual void force2points();
      virtual QPointF start();
      virtual QPointF goal();
    protected:
      ScenePortItem* _from;
      ScenePortItem* _to;
      std::vector<QPointF> _lp;
      std::vector<Direction> _directions;
      int _nbPoints;
    };
  }
}

#endif
