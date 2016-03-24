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

#ifndef _SCENEOBSERVERITEM_HXX_
#define _SCENEOBSERVERITEM_HXX_

#include "SceneItem.hxx"

#include <QString>

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
      virtual QString getToolTip();
    protected:
      virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
      virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
      virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
      virtual QColor getBrushColor();
      virtual QString getMimeFormat();
      Subject* _subject;
      bool _draging;
      bool _dragModifier;
      bool _emphasized;
    };
  }
}

#endif
