#ifndef _WRAPGRAPHICSVIEW_HXX_
#define _WRAPGRAPHICSVIEW_HXX_

#include <QGraphicsView>
#include <QContextMenuEvent>

namespace YACS
{
  namespace HMI
  {
    class WrapGraphicsView: public QGraphicsView
    {
      Q_OBJECT

    public:
      WrapGraphicsView(QWidget *parent = 0);
      virtual ~WrapGraphicsView();

    public slots:
      virtual void onViewFitAll();
      virtual void onViewFitArea();
      virtual void onViewZoom();
      virtual void onViewPan(); 
      virtual void onViewGlobalPan(); 
      virtual void onViewReset(); 
    };
  }
}
#endif
