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

#ifndef _WRAPGRAPHICSVIEW_HXX_
#define _WRAPGRAPHICSVIEW_HXX_

#include "SalomeWrapExport.hxx"

#include <QGraphicsView>
#include <QContextMenuEvent>

namespace YACS
{
  namespace HMI
  {
    class SALOMEWRAP_EXPORT WrapGraphicsView: public QGraphicsView
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
