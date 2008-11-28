//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#ifndef YACSPRS_INLINENODE_H
#define YACSPRS_INLINENODE_H

#include "YACSPrs_ElementaryNode.h"

class YACSPrs_InlineNode : public YACSPrs_ElementaryNode {
 public:
  YACSPrs_InlineNode( SUIT_ResourceMgr*, QCanvas*, YACS::HMI::SubjectNode*, const bool& thePortUpdate=true );
  virtual ~YACSPrs_InlineNode();

  /* reimplement functions from QxGraph_ActiveItem */
  virtual void showPopup(QWidget* theParent, QMouseEvent* theEvent, const QPoint& theMousePos = QPoint()) {}

  virtual int rtti() const;

  virtual QPointArray constructAreaPoints(int theW, int theH) const;

  virtual int getInfoHeight() const;

  virtual QRect getTitleRect() const;
  virtual QRect getStatusRect() const;
  virtual QRect getTimeRect() const;

  virtual QRect getWholeRect() const;
  virtual QRect getPercentRect() const;

  virtual int getCorner() const;

  virtual QString getToolTipText(const QPoint& theMousePos, QRect& theRect) const;

 protected:
  virtual void drawTitleShape(QPainter& thePainter);
  virtual void drawFrame(QPainter& thePainter);

};

#endif
