
#ifndef _SCENENODEITEM_HXX_
#define _SCENENODEITEM_HXX_

#include "SceneObserverItem.hxx"

#include <QString>

namespace YACS
{
  namespace HMI
  {
    class SceneHeaderItem;
    class SceneHeaderNodeItem;
    class SceneComposedNodeItem;
    class ScenePortItem;

    class SceneNodeItem: public SceneObserverItem
    {
    public:
      SceneNodeItem(QGraphicsScene *scene, SceneItem *parent,
                    QString label, Subject *subject);
      virtual ~SceneNodeItem();

      virtual void addHeader();
      virtual SceneHeaderItem* getHeader();
      virtual void paint(QPainter *painter,
                         const QStyleOptionGraphicsItem *option,
                         QWidget *widget);
      virtual void update(GuiEvent event, int type, Subject* son);
      virtual qreal getHeaderBottom();
      virtual void autoPosNewPort(AbstractSceneItem *item, int nbPorts);
      virtual void popupMenu(QWidget *caller, const QPoint &globalPos);
      virtual SceneComposedNodeItem* getParent();
      virtual void removeInPortFromList(AbstractSceneItem* inPort);
      virtual void removeOutPortFromList(AbstractSceneItem* outPort);
      virtual void setMoving(bool moving);
      virtual void setTopLeft(QPointF topLeft);
      virtual void adjustHeader();
      virtual ScenePortItem* getCtrlInPortItem();
      virtual ScenePortItem* getCtrlOutPortItem();

    protected:
      virtual QString getMimeFormat();
      virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
      virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
      virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
      virtual void updateState();
      virtual void setExecState(int execState);

      QString getHeaderLabel();

      std::list<AbstractSceneItem*> _inPorts;
      std::list<AbstractSceneItem*> _outPorts;    
      SceneHeaderNodeItem *_header;
      int _execState;
      bool _moving;
      QPointF _prevPos;
    };
  }
}

#endif
