
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

//       virtual void select(bool isSelected);
//       virtual void paint(QPainter *painter,
//                          const QStyleOptionGraphicsItem *option,
//                          QWidget *widget);
//       virtual void update(GuiEvent event, int type, Subject* son);
//       virtual void popupMenu(QWidget *caller, const QPoint &globalPos);
//       virtual void setPath(LinkPath lp);
//       virtual void minimizeDirectionChanges();
//       virtual void force2points();

      virtual QPointF start();
      virtual QPointF goal();
    protected:

//       ScenePortItem* _from;
//       ScenePortItem* _to;
//       std::vector<QPointF> _lp;
//       std::vector<Direction> _directions;
//       int _nbPoints;

    };
  }
}

#endif
