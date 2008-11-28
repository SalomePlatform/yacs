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
#ifndef YACSPRS_FOREACHLOOPNODE_H
#define YACSPRS_FOREACHLOOPNODE_H

#include "YACSPrs_LoopNode.h"

class YACSPrs_ForEachLoopNode : public YACSPrs_LoopNode {
 public:
  YACSPrs_ForEachLoopNode( SUIT_ResourceMgr*, QCanvas*, YACS::HMI::SubjectNode*);
  virtual ~YACSPrs_ForEachLoopNode();

  /* reimplement functions from QxGraph_ActiveItem */
  virtual void showPopup(QWidget* theParent, QMouseEvent* theEvent, const QPoint& theMousePos = QPoint()) {}

  virtual int rtti() const;

  virtual void updatePorts(bool theForce=false);

  virtual void nextTimeIteration(YACS::ENGINE::Node* theEngine=0);
  virtual int  getPercentage(YACS::ENGINE::Node* theEngine=0) const;
};

#endif
