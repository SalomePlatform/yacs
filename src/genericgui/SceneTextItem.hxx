// Copyright (C) 2006-2024  CEA, EDF
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

#ifndef _SCENETEXTITEM_HXX_
#define _SCENETEXTITEM_HXX_

#include "SceneItem.hxx"

namespace YACS
{
  namespace HMI
  {
    class SceneTextItem: public QGraphicsTextItem, public AbstractSceneItem
    {
    public:
      SceneTextItem(QGraphicsScene *scene, SceneItem *parent,
                    QString label, bool center=false );
      virtual ~SceneTextItem();
      virtual QRectF boundingRect() const;
      virtual void paint(QPainter *painter,
                         const QStyleOptionGraphicsItem *option,
                         QWidget *widget);

      virtual void setTopLeft(QPointF topLeft);
      virtual void setPlainTextTrunc(QString label);
      virtual void checkGeometryChange();
      virtual void popupMenu(QWidget *caller, const QPoint &globalPos);
      virtual QString getToolTip();
      void setEventPos(QPointF point);
    protected:
      QPointF _eventPos;
      bool _center;
    };
  }
}

#endif
