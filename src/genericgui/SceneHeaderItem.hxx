
#ifndef _SCENEHEADERITEM_HXX_
#define _SCENEHEADERITEM_HXX_

#include "SceneItem.hxx"

namespace YACS
{
  namespace HMI
  {
    class SceneTextItem;

    class SceneHeaderItem: public SceneItem
    {
    public:
      SceneHeaderItem(QGraphicsScene *scene, SceneItem *parent,
                      QString label);
      virtual ~SceneHeaderItem();
      virtual QRectF getMinimalBoundingRect() const;
      virtual void paint(QPainter *painter,
                         const QStyleOptionGraphicsItem *option,
                         QWidget *widget);
      virtual void setText(QString label);
      virtual void popupMenu(QWidget *caller, const QPoint &globalPos);
      virtual void adjustGeometry();

    protected:
      virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
      QColor getPenColor();
      QColor getBrushColor();
      SceneTextItem* _text;
    };
  }
}

#endif
