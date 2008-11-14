#ifndef _GRAPHICSVIEW_HXX_
#define _GRAPHICSVIEW_HXX_

#include "WrapGraphicsView.hxx"

#include <QContextMenuEvent>
#include <QAction>

namespace YACS
{
  namespace HMI
  {
    class GraphicsView: public WrapGraphicsView
    {
      Q_OBJECT

    public:
      GraphicsView(QWidget *parent = 0);
      virtual ~GraphicsView();

    public slots:
      virtual void onViewFitAll();
      virtual void onViewFitArea();
      virtual void onViewZoom();
      virtual void onViewPan(); 
      virtual void onViewGlobalPan(); 
      virtual void onViewReset(); 

    protected:
      void contextMenuEvent(QContextMenuEvent *event);
      void mouseMoveEvent (QMouseEvent *e);
      void mousePressEvent (QMouseEvent *e);
      void mouseReleaseEvent (QMouseEvent *e);

      bool _zooming;
      bool _fittingArea;
      bool _panning;
      int _prevX;
      qreal _scale;
      QPoint _prevPos;
    };
  }
}
#endif
