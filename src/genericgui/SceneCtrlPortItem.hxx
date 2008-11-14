
#ifndef _SCENECTRLPORTITEM_HXX_
#define _SCENECTRLPORTITEM_HXX_

#include "SceneItem.hxx"
#include "ScenePortItem.hxx"

namespace YACS
{
  namespace HMI
  {
    class SceneNodeItem;
    class SubjectNode;

    class SceneCtrlPortItem: public SceneItem, public ScenePortItem
    {
    public:
      SceneCtrlPortItem(QGraphicsScene *scene, SceneItem *parent,
                    QString label);
      virtual ~SceneCtrlPortItem();

      virtual void setText(QString label);
      virtual void paint(QPainter *painter,
                         const QStyleOptionGraphicsItem *option,
                         QWidget *widget);
      virtual SceneNodeItem* getParentNode();
      SubjectNode* getSubjectNode();
      static int getPortWidth();
      static int getPortHeight();
    protected:
      virtual QString getMimeFormat();
      virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
      virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
      virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
      static const int _portWidth;
      static const int _portHeight;
      bool _draging;
    };
  }
}

#endif
