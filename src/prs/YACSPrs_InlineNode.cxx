// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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

#include "YACSPrs_InlineNode.h"
#include "YACSPrs_Def.h"

#include <qpainter.h>

void drawText2(QPainter& thePainter, const QString& theText, 
	      const QRect& theRect, int theHAlign = Qt::AlignAuto)
{
  int flags = theHAlign | Qt::AlignVCenter;
  QRect r(theRect.x() + TEXT_MARGIN, theRect.y(), 
	  theRect.width() - 2*TEXT_MARGIN, theRect.height());

  QWMatrix aMat = thePainter.worldMatrix();
  if (aMat.m11() != 1.0) { 
    // for scaled picture only
    QRect r1 = aMat.mapRect(r);
    QFont saved = thePainter.font();
    QFont f(saved);
    if (f.pointSize() == -1) {
      f.setPixelSize((int)(f.pixelSize()*aMat.m11()));
    }
    else {
      f.setPointSize((int)(f.pointSize()*aMat.m11()));
    }
    thePainter.save();
    QWMatrix m;
    thePainter.setWorldMatrix(m);
    thePainter.setFont(f);
    thePainter.drawText(r1, flags, theText);
    thePainter.setFont(saved);
    thePainter.restore();
  }
  else {
    thePainter.drawText(r, flags, theText);
  }
}


/*!
  Constructor
*/
YACSPrs_InlineNode::YACSPrs_InlineNode(SUIT_ResourceMgr* theMgr, QCanvas* theCanvas,
				       YACS::HMI::SubjectNode* theSNode, const bool& thePortUpdate):
  YACSPrs_ElementaryNode(theMgr, theCanvas, theSNode)
{
  setNodeColor(INLINENODE_COLOR);
  setNodeSubColor(INLINENODE_SUBCOLOR);
  
  setStoreColor(nodeColor());
  setStoreSubColor(nodeSubColor());

  QRect aTRect = getTitleRect();
  myXRnd = 50*aTRect.height()/aTRect.width();
  myYRnd = 50;

  if ( thePortUpdate ) {
    //updatePorts(); // will be called in moveBy(...) function
    moveBy(2*HOOKPOINT_SIZE+NODEBOUNDARY_MARGIN,2*HOOKPOINT_SIZE+NODEBOUNDARY_MARGIN);
  }

  update();
}

/*!
  Destructor
*/
YACSPrs_InlineNode::~YACSPrs_InlineNode() 
{
}

int YACSPrs_InlineNode::rtti() const
{
  return 0;//YACSPrs_Canvas::Rtti_InlineNode;
}

QPointArray YACSPrs_InlineNode::constructAreaPoints(int theW, int theH) const
{
  int aCorner = getCorner();

  QPointArray aPnts(5);
  QPoint p((int)x(), (int)y());
  aPnts[0] = p + QPoint(aCorner, 0) + QPoint(0,-NODEBOUNDARY_MARGIN);
  aPnts[1] = p + QPoint(theW, 0) + QPoint(NODEBOUNDARY_MARGIN,-NODEBOUNDARY_MARGIN);
  aPnts[2] = aPnts[1] + QPoint(0, theH) + QPoint(0,NODEBOUNDARY_MARGIN);
  aPnts[3] = p + QPoint(0, theH) + QPoint(-NODEBOUNDARY_MARGIN,0);
  aPnts[4] = p + QPoint(0, aCorner) + QPoint(-NODEBOUNDARY_MARGIN,0);
  return aPnts;
}

int YACSPrs_InlineNode::getInfoHeight() const
{
  if ( isControlDMode() )
    return getTitleHeight() + NODE_SPACE;
  
  // Full view by default
  return getTitleHeight() + NODE_SPACE + 
         getStatusHeight() + NODE_SPACE +
         getTimeHeight();
}

QRect YACSPrs_InlineNode::getTitleRect() const
{
  if ( isControlDMode() )
    return QRect((int)x()+NODE_MARGIN+getCorner(), (int)y()+NODE_MARGIN, 
		 width()-2*NODE_MARGIN-getCorner(), getTitleHeight());

  // Full view by default
  return QRect((int)x()+NODE_MARGIN+getCorner(), (int)y()+NODE_MARGIN, 
	       width()-3*NODE_MARGIN-getCorner()-getPixMapWidth(), getTitleHeight());
}

QRect YACSPrs_InlineNode::getStatusRect() const
{
  return QRect(getTitleRect().x(), getTitleRect().bottom()+NODE_SPACE,
	       getTitleRect().width(), getStatusHeight());
}
  
QRect YACSPrs_InlineNode::getTimeRect() const
{
  return QRect(getStatusRect().x(), getStatusRect().bottom()+NODE_SPACE,
	       getStatusRect().width(), getTimeHeight());
}

QRect YACSPrs_InlineNode::getWholeRect() const
{
  return QRect(getTimeRect().topLeft()+QPoint(3,3), getTimeRect().bottomRight()-QPoint(3,3));
}

QRect YACSPrs_InlineNode::getPercentRect() const
{
  int aPercentageW = (int)(getWholeRect().width()*( (getStoredPercentage() < 0) ? getPercentage() : getStoredPercentage() )/100.);
  return QRect(getWholeRect().topLeft(), getWholeRect().bottomLeft()+QPoint(aPercentageW,0));
}

int YACSPrs_InlineNode::getCorner() const
{
  return NODE_MARGIN + getInfoHeight();
}

void YACSPrs_InlineNode::drawTitleShape(QPainter& thePainter)
{
  // draw information: title, status, time
  QBrush savedB = thePainter.brush();

  QPen savedP = thePainter.pen();
  thePainter.setPen(thePainter.brush().color().dark(140));

  // title
  thePainter.drawRoundRect(getTitleRect(),myXRnd,myYRnd);

  if ( isFullDMode() )
  {
    // status
    thePainter.drawRoundRect(getStatusRect(),myXRnd,myYRnd);
    // time
    thePainter.drawRoundRect(getTimeRect(),myXRnd,myYRnd);
    
    // draw progress bar
    thePainter.setPen(NoPen);

    //savedB = thePainter.brush();

    thePainter.setBrush(savedB.color().light(130));
    thePainter.drawRect(getWholeRect());
    
    if ( getPercentRect().width() > 1 ) {
      thePainter.setBrush(savedB.color().dark(160));
      thePainter.drawRect(getPercentRect());
    }
    
    thePainter.setBrush(savedB);
  }
  
  // draw texts
  thePainter.setPen(Qt::white);

  if ( isControlDMode() )
    drawText2(thePainter, QString(getEngine()->getName()), getTitleRect(), Qt::AlignHCenter);
  else if ( isFullDMode() )
  {
    drawText2(thePainter, QString(getEngine()->getName()), getTitleRect(), Qt::AlignLeft);
    drawText2(thePainter, myStatus, getStatusRect(), Qt::AlignLeft);
    drawText2(thePainter, myTime, getTimeRect(), Qt::AlignLeft);
    drawText2(thePainter, QString::number(( (getStoredPercentage() < 0) ? getPercentage() : getStoredPercentage() ), 'g', 4)+QString("%"),
	      getTimeRect(), Qt::AlignRight); // percentage
  }
   
  thePainter.setPen(savedP);

  if ( isFullDMode() )
  {
    // draw pixmap
    thePainter.setBrush(NoBrush);
    
    int aXRnd = getTitleRect().width()*myXRnd/getPixmapRect().width();
    int aYRnd = getTitleRect().height()*myYRnd/getPixmapRect().height();
    thePainter.drawRoundRect(getPixmapRect(),aXRnd,aYRnd);
    QRect aPRect = getPixmapRect();
    aPRect.setX(aPRect.x()+PIXMAP_MARGIN+2);
    aPRect.setY(aPRect.y()+PIXMAP_MARGIN);
    aPRect.setWidth(aPRect.width()-2*PIXMAP_MARGIN);
    aPRect.setHeight(aPRect.height()-2*PIXMAP_MARGIN);
    thePainter.drawPixmap(aPRect,myStatePixmap);

    thePainter.setBrush(savedB);
  }
}

void YACSPrs_InlineNode::drawFrame(QPainter& thePainter)
{
  QRect aRect = getRect();
  QRect aTRect = getTitleRect();
  int aXRnd = aTRect.width()*myXRnd/aRect.width();
  int aYRnd = aTRect.height()*myYRnd/aRect.height();

  QPen savedP = thePainter.pen();
  thePainter.setPen(NoPen);

  // calculate width and height for acrs and chords
  int w = 4*(aXRnd*aRect.width()/100)/3;
  int h = 4*(aYRnd*aRect.height()/100)/3;
  // draw chords
  thePainter.drawChord( aRect.right()-w+1,aRect.y(), w,h, 0*16, 90*16 );
  thePainter.drawChord( aRect.right()-w+1,aRect.bottom()-h+1, w,h, 270*16, 90*16 );
  thePainter.drawChord( aRect.x(),aRect.bottom()-h+1, w,h, 180*16, 90*16 );
  
  // draw polygon
  int aCorner = getCorner();
  QPoint aP1(aRect.x(),aRect.y()+aCorner); 
  QPoint aP2(aRect.x()+aCorner,aRect.y());
  QPoint aP3(aRect.right()-(w-1)/2,aRect.y());
  QPoint aP4(aRect.right(),aRect.y()+h/2-1);
  QPoint aP5(aRect.right(),aRect.bottom()-h/2+1);
  QPoint aP6(aRect.right()-(w-1)/2,aRect.bottom());
  QPoint aP7(aRect.x()+(w-1)/2,aRect.bottom());
  QPoint aP8(aRect.x(),aRect.bottom()-h/2+1);
  QPointArray aPA(8);
  aPA.putPoints(0, 8, 
		aP1.x(),aP1.y(), aP2.x(),aP2.y(), aP3.x(),aP3.y(), aP4.x(),aP4.y(),
		aP5.x(),aP5.y(), aP6.x(),aP6.y(), aP7.x(),aP7.y(), aP8.x(),aP8.y());
  thePainter.drawPolygon( aPA );
  thePainter.setPen(savedP);

  // draw arcs
  thePainter.drawArc( aRect.right()-w+1,aRect.y(), w,h, 0*16, 90*16 );
  thePainter.drawArc( aRect.right()-w+1,aRect.bottom()-h+1, w,h, 270*16, 90*16 );
  thePainter.drawArc( aRect.x(),aRect.bottom()-h+1, w,h, 180*16, 90*16 );

  // draw line segments
  thePainter.drawLine(aP1,aP2);
  thePainter.drawLine(aP2,aP3);
  thePainter.drawLine(aP4,aP5);
  thePainter.drawLine(aP6,aP7);
  thePainter.drawLine(aP8,aP1);

  // draw corner
  savedP = thePainter.pen();
  thePainter.setPen(NoPen);
  QBrush savedB = thePainter.brush();
  thePainter.setBrush(savedB.color().dark(120));
  
  thePainter.drawChord( aRect.x()+aCorner-w+1,aRect.y()+aCorner-h+1, w,h, 270*16, 90*16 );

  QPoint aP9(aRect.x()+aCorner,aRect.y()+aCorner-h/2+1);
  QPoint aP10(aRect.x()+aCorner-(w-1)/2,aRect.y()+aCorner); 
  QPointArray aPAC(4);
  aPAC.putPoints(0, 4, aP1.x(),aP1.y(), aP2.x(),aP2.y(), aP9.x(),aP9.y(), aP10.x(),aP10.y());
  thePainter.drawPolygon( aPAC );

  thePainter.setBrush(savedB);
  thePainter.setPen(savedP);

  thePainter.drawArc( aRect.x()+aCorner-w+1,aRect.y()+aCorner-h+1, w,h, 270*16, 90*16 );

  thePainter.drawLine(aP1,aP2);
  thePainter.drawLine(aP2,aP9);
  thePainter.drawLine(aP10,aP1);

  // draw bounding nodes' polygon if node is currently selected
  if ( isSelected() ) drawBoundary(thePainter,2);
}

QString YACSPrs_InlineNode::getToolTipText(const QPoint& theMousePos, QRect& theRect) const
{
  // Check if the tooltip for ports is needed
  if (getBodyRect().contains(theMousePos, true) || getGateRect().contains(theMousePos, true))
    return YACSPrs_ElementaryNode::getToolTipText(theMousePos,theRect);
  
  // Return tooltip text for node
  QString aText = QString("");
  aText += QString("Name: %1\n").arg(getEngine()->getName());
  aText += QString("Type: %1\n").arg("Inline node");
  theRect = getTitleRect();
  theRect = theRect.unite(getStatusRect());
  theRect = theRect.unite(getTimeRect());
  theRect = theRect.unite(getPixmapRect());
  return aText;
}
