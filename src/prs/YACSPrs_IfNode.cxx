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
#include "YACSPrs_IfNode.h"
#include "YACSPrs_Def.h"

#include "SUIT_ResourceMgr.h"

#include <qpainter.h>

/*!
  Constructor
*/
YACSPrs_IfNode::YACSPrs_IfNode( SUIT_ResourceMgr* theMgr, QCanvas* theCanvas, YACS::HMI::SubjectNode* theSNode ):
  YACSPrs_InlineNode(theMgr, theCanvas, theSNode)
{
  setNodeColor(IFNODE_COLOR);
  setNodeSubColor(IFNODE_SUBCOLOR);

  setStoreColor(nodeColor());
  setStoreSubColor(nodeSubColor());

  myTitlePixmap = myMgr->loadPixmap( "YACSPrs", QObject::tr( "ICON_TITLE_RARROW" ));
  moveBy(0, 2*TITLE_HEIGHT);
}

/*!
  Destructor
*/
YACSPrs_IfNode::~YACSPrs_IfNode()
{
  hide();
}

int YACSPrs_IfNode::rtti() const
{
  return 0;//YACSPrs_Canvas::Rtti_IfNode;
}

QPointArray YACSPrs_IfNode::constructAreaPoints(int theW, int theH) const
{
  int aCorner = 2*TITLE_HEIGHT;

  QPointArray aPnts(5);
  QPoint p((int)x(), (int)y());
  aPnts[0] = p + QPoint(-NODEBOUNDARY_MARGIN,0);
  aPnts[1] = p + QPoint(theW/2, -aCorner) + QPoint(0,-NODEBOUNDARY_MARGIN);
  aPnts[2] = p + QPoint(theW, 0) + QPoint(NODEBOUNDARY_MARGIN,0);
  aPnts[3] = aPnts[2] + QPoint(0, theH) + QPoint(0,NODEBOUNDARY_MARGIN/2);
  aPnts[4] = p + QPoint(0, theH) + QPoint(-NODEBOUNDARY_MARGIN,NODEBOUNDARY_MARGIN/2);
  return aPnts;
}

void YACSPrs_IfNode::drawFrame(QPainter& thePainter)
{
  QRect aRect = getRect();
  QRect aTRect = getTitleRect();
  int aXRnd = aTRect.width()*myXRnd/aRect.width();
  int aYRnd = aTRect.height()*myYRnd/aRect.height();

  QPen savedP = thePainter.pen();
  thePainter.setPen(NoPen);

  // calculate width and height for acrs
  int w = 4*(aXRnd*aRect.width()/100)/3;
  int h = 4*(aYRnd*aRect.height()/100)/3;
  
  // draw chords without pen
  thePainter.drawChord( aRect.x(),aRect.y(), w,h, 90*16, 90*16 );
  thePainter.drawChord( aRect.right()-w+1,aRect.y(), w,h, 0*16, 90*16 );
  thePainter.drawChord( aRect.right()-w+1,aRect.bottom()-h+1, w,h, 270*16, 90*16 );
  thePainter.drawChord( aRect.x(),aRect.bottom()-h+1, w,h, 180*16, 90*16 );

  //thePainter.drawRoundRect(aRect,aXRnd,aYRnd);
  int aCorner =  2*TITLE_HEIGHT;
  QPoint aP1(aRect.x()+(w-1)/2,aRect.y());
  QPoint aP2(aRect.x()+aRect.width()/2,aRect.y()-aCorner);
  QPoint aP3(aRect.right()-(w-1)/2,aRect.y());
  QPoint aP4(aRect.right(),aRect.y()+h/2-1);
  QPoint aP5(aRect.right(),aRect.bottom()-h/2+1);
  QPoint aP6(aRect.right()-(w-1)/2,aRect.bottom());
  QPoint aP7(aRect.x()+(w-1)/2,aRect.bottom());
  QPoint aP8(aRect.x(),aRect.bottom()-h/2+1);
  QPoint aP9(aRect.x(),aRect.y()+h/2-1);
  QPointArray aPA(9);
  aPA.putPoints(0, 9, 
		aP1.x(),aP1.y(), aP2.x(),aP2.y(), aP3.x(),aP3.y(), aP4.x(),aP4.y(),
		aP5.x(),aP5.y(), aP6.x(),aP6.y(), aP7.x(),aP7.y(), aP8.x(),aP8.y(), aP9.x(),aP9.y());
  thePainter.drawPolygon( aPA );
  thePainter.setPen(savedP);

  // draw arcs
  thePainter.drawArc( aRect.x(),aRect.y(), w,h, 90*16, 90*16 );
  thePainter.drawArc( aRect.right()-w+1,aRect.y(), w,h, 0*16, 90*16 );
  thePainter.drawArc( aRect.right()-w+1,aRect.bottom()-h+1, w,h, 270*16, 90*16 );
  thePainter.drawArc( aRect.x(),aRect.bottom()-h+1, w,h, 180*16, 90*16 );

  // draw line segments
  thePainter.drawLine(aP1,aP2);
  thePainter.drawLine(aP2,aP3);
  thePainter.drawLine(aP4,aP5);
  thePainter.drawLine(aP6,aP7);
  thePainter.drawLine(aP8,aP9);

  // draw title pixmap
  int x = aRect.x()+aRect.width()/2-myTitlePixmap.width()/2;
  int y = aRect.y()-(aCorner-NODE_MARGIN)/2-myTitlePixmap.height()/2;
  QRect aTPRect(aRect.x()+aRect.width()/2-aCorner/3, aRect.y()-(aCorner-NODE_MARGIN)/2-aCorner/3,
		2*aCorner/3, 2*aCorner/3);
  thePainter.drawPixmap(aTPRect,myTitlePixmap);

  // draw bounding nodes' polygon if node is currently selected
  if ( isSelected() ) drawBoundary(thePainter,3);
}

void YACSPrs_IfNode::reorderPorts()
{
}

