
#ifndef _SCENE_HXX_
#define _SCENE_HXX_

#include <QGraphicsScene>
#include <QPointF>

namespace YACS
{
  namespace HMI
  {
    class Scene: public QGraphicsScene
    {
    public:
      Scene(QObject *parent = 0);
      virtual ~Scene();
      
      static bool _autoComputeLinks;
      static bool _simplifyLinks;
      static bool _force2NodesLink;
      
      void setZoom(bool zooming);
      bool isZooming();
      
    protected:
      virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
      virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent);
      virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent);
      
      bool _zooming;
    };
  }
}

#endif
