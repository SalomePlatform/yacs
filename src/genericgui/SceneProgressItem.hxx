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

#ifndef _SCENEPROGRESSITEM_HXX_
#define _SCENEPROGRESSITEM_HXX_

#include "SceneItem.hxx"

namespace YACS
{
  namespace HMI
  {
    class SceneTextItem;

    class SceneProgressItem: public SceneItem
    {
    public:
      SceneProgressItem(QGraphicsScene *scene, SceneItem *parent,
                      QString label);
      virtual ~SceneProgressItem();
      virtual QRectF getMinimalBoundingRect() const;
      virtual void paint(QPainter *painter,
                         const QStyleOptionGraphicsItem *option,
                         QWidget *widget);
      virtual void setProgress(QString newProgress);
      virtual int getProgress() { return _progress; };
      virtual void setText(QString label);
      virtual void popupMenu(QWidget *caller, const QPoint &globalPos);
      virtual void adjustGeometry();

    protected:
      virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
      virtual QString getToolTip();
      QColor getPenColor();
      QColor getBrushColor();
      SceneTextItem* _text;
      QString _tooltip;
      int _progress;

    };
  }
}

#endif
