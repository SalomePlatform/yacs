// Copyright (C) 2006-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef _GRAPHICSVIEW_HXX_
#define _GRAPHICSVIEW_HXX_

#include "WrapGraphicsView.hxx"

#include <QContextMenuEvent>
#include <QAction>
#include <QGraphicsRectItem>

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

      virtual void onZoomToBloc();
      virtual void onCenterOnNode();

    protected:
      void contextMenuEvent(QContextMenuEvent *event);
      void mouseMoveEvent (QMouseEvent *e);
      void mousePressEvent (QMouseEvent *e);
      void mouseReleaseEvent (QMouseEvent *e);
      virtual void wheelEvent (QWheelEvent *e);

      bool _zooming;
      bool _fittingArea;
      bool _panning;
      int _prevX;
      int _prevY;
      qreal _scale;
      QPoint _prevPos;
      QGraphicsRectItem *_rect;
    };
  }
}
#endif
