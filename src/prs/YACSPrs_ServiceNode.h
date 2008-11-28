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
#ifndef YACSPRS_SERVICENODE_H
#define YACSPRS_SERVICENODE_H

#include "YACSPrs_ElementaryNode.h"

class YACSPrs_ServiceNode : public YACSPrs_ElementaryNode {
 public:
  YACSPrs_ServiceNode( SUIT_ResourceMgr*, QCanvas*, YACS::HMI::SubjectNode* );
  virtual ~YACSPrs_ServiceNode();

  /* reimplement functions from QxGraph_ActiveItem */
  virtual void showPopup(QWidget* theParent, QMouseEvent* theEvent, const QPoint& theMousePos = QPoint()) {}

  virtual int rtti() const;

  virtual int getInfoHeight() const;
  virtual int getServiceHeight() const;
  virtual int getComponentHeight() const;
  virtual int getMachineHeight() const;
  
  virtual QRect getServiceRect() const;
  virtual QRect getComponentRect() const;
  virtual QRect getMachineRect() const;
  virtual QRect getStatusRect() const;

  virtual QRect getWholeRect() const;
  virtual QRect getPercentRect() const;

  virtual void updateInfo(); // update service, component, machine

  virtual QString getToolTipText(const QPoint& theMousePos, QRect& theRect) const;
  
 protected:
  virtual void drawTitleShape(QPainter& thePainter);

 private:
  int myServiceHeight;
  int myComponentHeight;
  int myMachineHeight;

};

#endif
