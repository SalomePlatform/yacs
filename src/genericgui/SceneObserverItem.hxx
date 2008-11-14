
#ifndef _SCENEOBSERVERITEM_HXX_
#define _SCENEOBSERVERITEM_HXX_

#include "SceneItem.hxx"

namespace YACS
{
  namespace HMI
  {
    class SceneObserverItem: public SceneItem, public GuiObserver
    {
    public:
      SceneObserverItem(QGraphicsScene *scene, SceneItem *parent,
                QString label, Subject *subject);
      virtual ~SceneObserverItem();
      virtual void update(GuiEvent event, int type, Subject* son);
      virtual void select(bool isSelected);
      virtual void activateSelection(bool selected);
      virtual Subject* getSubject();
    protected:
      virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
      virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
      virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
      virtual QString getMimeFormat();
      Subject* _subject;
      bool _draging;
    };
  }
}

#endif
