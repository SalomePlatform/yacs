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

#include "YACSPrs_ServiceNode.h"
#include "YACSPrs_Def.h"

#include <SUIT_ResourceMgr.h>

#include <qpainter.h>

#include <ServiceNode.hxx>
#include <ComponentInstance.hxx>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;

void drawText1(QPainter& thePainter, const QString& theText, 
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
YACSPrs_ServiceNode::YACSPrs_ServiceNode(SUIT_ResourceMgr* theMgr, QCanvas* theCanvas, YACS::HMI::SubjectNode* theSNode):
  YACSPrs_ElementaryNode(theMgr, theCanvas, theSNode)
{
  DEBTRACE("YACSPrs_ServiceNode::YACSPrs_ServiceNode");
  myServiceHeight = TITLE_HEIGHT;
  myComponentHeight = TITLE_HEIGHT;
  myMachineHeight = TITLE_HEIGHT;
  
  setNodeColor(SERVICENODE_COLOR);
  setNodeSubColor(SERVICENODE_SUBCOLOR);

  setStoreColor(nodeColor());
  setStoreSubColor(nodeSubColor());

  //updatePorts(); // will be called in moveBy(...) function
  moveBy(2*HOOKPOINT_SIZE+NODEBOUNDARY_MARGIN,2*HOOKPOINT_SIZE+NODEBOUNDARY_MARGIN);

  update();
}

/*!
  Destructor
*/
YACSPrs_ServiceNode::~YACSPrs_ServiceNode() 
{
  hide();
}

int YACSPrs_ServiceNode::rtti() const
{
  return 0;//YACSPrs_Canvas::Rtti_ServiceNode;
}

int YACSPrs_ServiceNode::getInfoHeight() const
{
  if ( isControlDMode() )
    return getTitleHeight() + NODE_SPACE;
  
  // Full view by default
  return getTitleHeight() + NODE_SPACE +
         getServiceHeight() + NODE_SPACE + 
         getComponentHeight() + NODE_SPACE + 
         getMachineHeight() + NODE_SPACE + 
         getStatusHeight();
}
  
int YACSPrs_ServiceNode::getServiceHeight() const
{
  return myServiceHeight;
}

int YACSPrs_ServiceNode::getComponentHeight() const
{
  return myComponentHeight;
}

int YACSPrs_ServiceNode::getMachineHeight() const
{
  return myMachineHeight;
}

QRect YACSPrs_ServiceNode::getServiceRect() const
{
  QRect aPMRect = getPixmapRect(true,true);
  int width = getTitleRect().width() - aPMRect.width() - NODE_MARGIN;
  return QRect(aPMRect.right() + NODE_MARGIN, getTitleRect().bottom()+NODE_SPACE,
	       width, getServiceHeight());
}

QRect YACSPrs_ServiceNode::getComponentRect() const
{
  QRect aPMRect = getPixmapRect(true,true);
  int width = getTitleRect().width() - aPMRect.width() - NODE_MARGIN;
  return QRect(aPMRect.right() + NODE_MARGIN, getServiceRect().bottom()+NODE_SPACE,
	       width, getComponentHeight());
}

QRect YACSPrs_ServiceNode::getMachineRect() const
{
  QRect aPMRect = getPixmapRect(true,true);
  int width = getTitleRect().width() - aPMRect.width() - NODE_MARGIN;
  return QRect(aPMRect.right() + NODE_MARGIN, getComponentRect().bottom()+NODE_SPACE,
	       width, getMachineHeight());
}

QRect YACSPrs_ServiceNode::getStatusRect() const
{
  QRect aPMRect = getPixmapRect(true,true);
  int width = getTitleRect().width() - aPMRect.width() - NODE_MARGIN;
  return QRect(aPMRect.right() + NODE_MARGIN, getMachineRect().bottom()+NODE_SPACE,
	       width, getStatusHeight());
}

QRect YACSPrs_ServiceNode::getWholeRect() const
{
  return QRect(getStatusRect().topLeft()+QPoint(getStatusRect().width()/3+3,3),
	       getStatusRect().bottomRight()-QPoint(3,3));
}

QRect YACSPrs_ServiceNode::getPercentRect() const
{
  int aPercentageW = (int)(getWholeRect().width()*( (getStoredPercentage() < 0) ? getPercentage() : getStoredPercentage() )/100.);
  return QRect(getWholeRect().topLeft(), getWholeRect().bottomLeft()+QPoint(aPercentageW,0));
}

/*!
  Update service, component, machine
*/
void YACSPrs_ServiceNode::updateInfo()
{

}
  
void YACSPrs_ServiceNode::drawTitleShape(QPainter& thePainter)
{
  // draw information: title, service, component, machine, status
  int aXRnd = getTitleRect().width()*myXRnd/getServiceRect().width();
  int aYRnd = getTitleRect().height()*myYRnd/TITLE_HEIGHT;

  QBrush savedB = thePainter.brush();

  QPen savedP = thePainter.pen();
  thePainter.setPen(thePainter.brush().color().dark(140));

  // title
  thePainter.drawRoundRect(getTitleRect(),myXRnd,myYRnd);
  
  if ( isFullDMode() )
  {
    // service
    thePainter.drawRoundRect(getServiceRect(),aXRnd,aYRnd);
    // component
    thePainter.drawRoundRect(getComponentRect(),aXRnd,aYRnd);
    // machine
    thePainter.drawRoundRect(getMachineRect(),aXRnd,aYRnd);
    //state and time
    thePainter.drawRoundRect(getStatusRect(),aXRnd,aYRnd);
    
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
    drawText1(thePainter, QString(getEngine()->getName()), getTitleRect(), Qt::AlignHCenter);
  else if ( isFullDMode() )
  {
    drawText1(thePainter, QString(getEngine()->getName()), getTitleRect(), Qt::AlignLeft);
    ServiceNode* aSEngine = dynamic_cast<ServiceNode*>( getEngine() );
    drawText1(thePainter, QString( "Service: ") + QString( aSEngine ? aSEngine->getMethod() : "..." ), getServiceRect(), Qt::AlignLeft);
    drawText1(thePainter, QString( "Component: " ) +
	      QString( (aSEngine && aSEngine->getComponent()) ? aSEngine->getComponent()->getInstanceName() : "..." ),
	      getComponentRect(), Qt::AlignLeft);
    drawText1(thePainter, QString("Machine: ..."), getMachineRect(), Qt::AlignLeft);
    
    int aMidX = getTitleRect().right()-getTitleRect().width()/2;
    thePainter.drawLine(aMidX, getStatusRect().top()+2, aMidX, getStatusRect().bottom()-2);
    
    QRect aStateRect(getStatusRect().x(), getStatusRect().y(), aMidX-getStatusRect().left(), TITLE_HEIGHT);
    QRect aTimeRect(aMidX, getStatusRect().y(), 2*getStatusRect().width()/3, TITLE_HEIGHT);

    drawText1(thePainter, myStatus, aStateRect, Qt::AlignLeft);
    drawText1(thePainter, myTime, aTimeRect, Qt::AlignLeft);
    drawText1(thePainter, QString::number(( (getStoredPercentage() < 0) ? getPercentage() : getStoredPercentage() ), 'g', 4)+QString("%"),
	      aTimeRect, Qt::AlignRight); // percentage
  }
  
  thePainter.setPen(savedP);

  if ( isFullDMode() )
  {
    // draw pixmap
    thePainter.setBrush(NoBrush);
    
    aXRnd = getTitleRect().width()*myXRnd/getPixmapRect(true,true).width();
    aYRnd = getTitleRect().height()*myYRnd/getPixmapRect(true,true).height();
    thePainter.drawRoundRect(getPixmapRect(true,true),aXRnd,aYRnd);
    QRect aPRect = getPixmapRect(true,true);
    aPRect.setX(aPRect.x()+PIXMAP_MARGIN+2);
    aPRect.setY(aPRect.y()+PIXMAP_MARGIN);
    aPRect.setWidth(aPRect.width()-2*PIXMAP_MARGIN);
    aPRect.setHeight(aPRect.height()-2*PIXMAP_MARGIN);
    thePainter.drawPixmap(aPRect,myStatePixmap);
    
    thePainter.setBrush(savedB);
  }
}

QString YACSPrs_ServiceNode::getToolTipText(const QPoint& theMousePos, QRect& theRect) const
{
  // Check if the tooltip for ports is needed
  if (getBodyRect().contains(theMousePos, true) || getGateRect().contains(theMousePos, true))
    return YACSPrs_ElementaryNode::getToolTipText(theMousePos,theRect);
  
  // Return tooltip text for node
  QString aText = QString("");
  aText += QString("Name: %1\n").arg(getEngine()->getName());
  aText += QString("Type: %1\n").arg("Service node");
  theRect = getTitleRect();
  theRect = theRect.unite(getServiceRect());
  theRect = theRect.unite(getComponentRect());
  theRect = theRect.unite(getMachineRect());
  theRect = theRect.unite(getStatusRect());
  theRect = theRect.unite(getPixmapRect());
  return aText;
}
