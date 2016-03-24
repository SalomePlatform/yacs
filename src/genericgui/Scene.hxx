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
      
      static bool _straightLinks;
      static bool _autoComputeLinks;
      static bool _simplifyLinks;
      static bool _force2NodesLink;
      static bool _addRowCols;
      
      void setZoom(bool zooming);
      bool isZooming();
      
    protected:
      virtual void helpEvent(QGraphicsSceneHelpEvent *event);
      virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
      virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent);
      virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent);
      
      bool _zooming;
    };
  }
}

#endif
