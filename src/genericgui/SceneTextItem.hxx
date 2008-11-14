
#ifndef _SCENETEXTITEM_HXX_
#define _SCENETEXTITEM_HXX_

#include "SceneItem.hxx"

namespace YACS
{
  namespace HMI
  {
    class SceneTextItem: public QGraphicsTextItem, public AbstractSceneItem
    {
    public:
      SceneTextItem(QGraphicsScene *scene, SceneItem *parent,
                    QString label);
      virtual ~SceneTextItem();
      virtual QRectF boundingRect() const;
      virtual void paint(QPainter *painter,
                         const QStyleOptionGraphicsItem *option,
                         QWidget *widget);

      virtual void setTopLeft(QPointF topLeft);
      virtual void checkGeometryChange();
      virtual void popupMenu(QWidget *caller, const QPoint &globalPos);
    protected:
    };
  }
}

#endif
