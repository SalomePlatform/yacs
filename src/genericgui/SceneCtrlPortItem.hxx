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
    protected:
      virtual QString getMimeFormat();
      virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
      virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
      virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
      bool _draging;
      bool _dragModifier;
    };
  }
}

#endif
