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

#include "YACSPrs_BlocNode.h"

#include "YACSPrs_Def.h"
#include "YACSPrs_LoopNode.h"
#include "YACSPrs_SwitchNode.h"
#include "YACSPrs_Link.h"

#include "SUIT_ResourceMgr.h"

#include <commandsProc.hxx>
#include <guiContext.hxx>

#include <qpainter.h>

using namespace YACS::ENGINE;
using namespace YACS::HMI;

void drawText4(QPainter& thePainter, const QString& theText, 
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
 * =========================== YACSPrs_BlocNode ===========================
 !*/

YACSPrs_BlocNode::YACSPrs_BlocNode(SUIT_ResourceMgr* theMgr, QCanvas* theCanvas,
				   YACS::HMI::SubjectNode* theSNode,
				   DisplayMode theDisplayMode, int theZ,
				   int theLeft, int theTop, int theWidth, int theHeight):
  YACSPrs_ElementaryNode(theMgr, theCanvas, theSNode),
  myDisplayMode(theDisplayMode)
{
  printf("YACSPrs_BlocNode::YACSPrs_BlocNode\n");
  //setX(theLeft);
  //setY(theTop);
  
  setNodeColor(BLOCNODE_COLOR);
  setNodeSubColor(BLOCNODE_SUBCOLOR);

  setStoreColor(nodeColor());
  setStoreSubColor(nodeSubColor());

  myResizing = false;
  myResizeDirection = No;

  myWidth = theWidth;
  myHeight = theHeight;
  myArea = QRect(100, 100, theWidth, theHeight);

  if ( myDisplayMode == Expanded )
  {
    if ( myPointMaster ) myWidth = myPointMaster->width() > theWidth ? myPointMaster->width() : theWidth;
    else myWidth = theWidth;

    int anEmptyHeight = getTitleHeight() + getGateHeight() + 2*BLOCNODE_MARGIN;
    myHeight = ( (anEmptyHeight > theHeight) ? anEmptyHeight : theHeight);
    
    setTitleHeight(getTitleHeight()+2*PORT_MARGIN);
    
    myContentMoving = true;
    myBoundColor = BLOCNODERECT_COLOR;

    setZ(theZ);

    updateGates();

  }
  else
  {
    myContentMoving = false;
    setZ(2);

    update();

    // not yet implemented
  }
}

YACSPrs_BlocNode::~YACSPrs_BlocNode() 
{
  for ( std::set<YACSPrs_ElementaryNode*>::iterator it = myChildren.begin(); it != myChildren.end(); it++ )
    delete (*it);
  myChildren.clear();
}

void YACSPrs_BlocNode::update( YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son)
{
  printf(">> YACSPrs_BlocNode::update\n");
  switch (event)
  {
  case YACS::HMI::RENAME:
    if ( canvas() )
    { 
      canvas()->setChanged(getTitleRect());
      canvas()->update();
    }
    break;
  case YACS::HMI::EDIT:
    {
      int anOldHeight = maxHeight();
      updateHeight();

      bool needToCanvasUpdate = true;
      if ( anOldHeight != maxHeight() )
      {
	//emit portsChanged();

	YACS::HMI::SubjectBloc* aParentSubj = dynamic_cast<YACS::HMI::SubjectBloc*>( getSEngine()->getParent() );
	if ( aParentSubj && !dynamic_cast<YACS::HMI::SubjectProc*>( aParentSubj ) )
	{
	  aParentSubj->update( event, YACS::HMI::BLOC, getSEngine() );
	  needToCanvasUpdate = false;
	}
      }
      
      if ( needToCanvasUpdate && canvas() )
      { 
	QRect aRect((int)x(),(int)y(),maxWidth(),maxHeight());
	canvas()->setChanged(aRect);
	canvas()->update();
      }
    }
    break;
  case ADDLINK:
  case ADDCONTROLLINK:
    {
      printf(">> In prs : ADDLINK\n");
      GuiContext* aContext = GuiContext::getCurrent();
      if ( aContext )
      	if ( aContext->getSubjectProc() )
	  aContext->getSubjectProc()->update(event, type, son);
    }
    break;
  case REMOVE:
    switch (type)
    {
    case CONTROLLINK:
      {
	printf(">> In prs:  REMOVE link\n");
	removeLinkPrs(son);
      }
      break;
    }
    break;
  default:
    GuiObserver::update(event, type, son);
  }
}

void YACSPrs_BlocNode::setChildren(std::set<YACSPrs_ElementaryNode*>& theChildren)
{ 
  if ( myDisplayMode == Expanded )
  { 
    //myChildren = theChildren;
    if ( !myChildren.empty() ) myChildren.clear();
    for ( std::set<YACSPrs_ElementaryNode*>::iterator it = theChildren.begin(); it != theChildren.end(); it++ )
      myChildren.insert(*it);
      
    // resize bounding rectangle if needed
    int aMaxWidth=0, aMaxHeight=0;
    int aX = (int)x();
    int aY = (int)y() + getTitleHeight();
    bool hasBlocChild = false;
    for ( std::set<YACSPrs_ElementaryNode*>::iterator it = myChildren.begin(); it != myChildren.end(); it++ )
    {  
      if ( aMaxWidth < (*it)->maxWidth() ) aMaxWidth = (*it)->maxWidth();
      if ( aMaxHeight < (*it)->maxHeight() ) aMaxHeight = (*it)->maxHeight();
      YACSPrs_LoopNode* aLoop = dynamic_cast<YACSPrs_LoopNode*>( *it );
      if ( aLoop )
	(*it)->moveBy( aX - (*it)->boundingRect().x() + (( 2*HOOKPOINT_SIZE > 3*TITLE_HEIGHT/2 ) ? ( 2*HOOKPOINT_SIZE - 3*TITLE_HEIGHT/2 ) : 0) + BLOCNODE_MARGIN, 
		       aY - (*it)->boundingRect().y() + BLOCNODE_MARGIN );
      else
      {
	YACSPrs_BlocNode* aBloc = dynamic_cast<YACSPrs_BlocNode*>( *it );
	if ( aBloc ) {
	  (*it)->moveBy( aX - (*it)->boundingRect().x() + HOOKPOINTGATE_SIZE + BLOCNODE_MARGIN, aY - (*it)->boundingRect().y() + BLOCNODE_MARGIN );
	  hasBlocChild = true;
	}
	else
	  (*it)->moveBy( aX - (*it)->boundingRect().x() + 2*HOOKPOINT_SIZE + BLOCNODE_MARGIN,
			 aY - (*it)->boundingRect().y() + BLOCNODE_MARGIN );
      }
      (*it)->setIsInBloc(true);
    }
    if ( aMaxWidth > myWidth ) myWidth = aMaxWidth + 2*BLOCNODE_MARGIN;
    if ( aMaxHeight > myHeight ) myHeight = aMaxHeight + getTitleHeight() + getGateHeight() + 2*BLOCNODE_MARGIN;

    setZ(z());

    updateGates();
    printf("Parent : %s. Number of children : %d\n",getEngine()->getName().c_str(),myChildren.size());
  } 
}

void YACSPrs_BlocNode::setCanvas(QCanvas* theCanvas)
{
  QCanvasItem::setCanvas(theCanvas);
  
  if ( myDisplayMode == Expanded )
  {
    for ( std::set<YACSPrs_ElementaryNode*>::iterator it = myChildren.begin(); it != myChildren.end(); it++ )
    {
      (*it)->setCanvas(theCanvas);
      (*it)->setArea(getAreaRect());
    }
  }
  else
  {
    // not yet implemented
  }

  // set canvas to all ports
  YACSPrs_Port* aPort;
  for (aPort = myPortList.first(); aPort; aPort = myPortList.next())
    aPort->setCanvas(theCanvas);

  // set canvas to master point
  if (myPointMaster)
  {
    myPointMaster->setCanvas(theCanvas);
    QPoint aPnt = getConnectionMasterPoint();
    myPointMaster->setCoords(aPnt.x(), aPnt.y());
  }
}

bool YACSPrs_BlocNode::isResizable(QPoint thePoint, int& theCursorType)
{
  if ( myDisplayMode == Expanded )
  {
    theCursorType = 0;
    QRect aRect = getRect();

    int aM11 = (int)( getTMatrix().m11() < 1 ? 1/getTMatrix().m11() : 1 );
    int aM22 = (int)( getTMatrix().m22() < 1 ? 1/getTMatrix().m22() : 1 );
    
    int aW = 8*aM11; int aShiftX = (aW-2)/2;
    int aH = 8*aM22; int aShiftY = (aH-2)/2;
    
    QRect aLeft(  (int)x()-aShiftX,        (int)y()-aShiftY,         aW,        height()+aH);
    QRect aRight( (int)x()+width()-aShiftX,(int)y()-aShiftY,         aW,        height()+aH);
    QRect aTop(   (int)x()-aShiftX,        (int)y()-aShiftY,         width()+aW,aH);
    QRect aBottom((int)x()-aShiftX,        (int)y()+height()-aShiftY,width()+aW,aH);
    
    if ( aTop.contains(thePoint) && aRight.contains(thePoint) ) {
      theCursorType = Direction(TopRight); // 6
      return true;
    }
    if ( aBottom.contains(thePoint) && aLeft.contains(thePoint) ) {
      theCursorType = Direction(BottomLeft); // 8
      return true;
    }
    if ( aTop.contains(thePoint) && aLeft.contains(thePoint) ) {
      theCursorType = Direction(LeftTop); // 5
      return true;
    }
    if ( aBottom.contains(thePoint) && aRight.contains(thePoint) ) {
      theCursorType = Direction(RightBottom); // 7
      return true;
    }
    if ( aTop.contains(thePoint) ) {
      theCursorType = Direction(Top); // 2
      return true;
    }
    if ( aBottom.contains(thePoint) ) {
      theCursorType = Direction(Bottom); // 4
      return true;
    }
    if ( aLeft.contains(thePoint) ) {
      theCursorType = Direction(Left); // 1
      return true;
    }
    if ( aRight.contains(thePoint) ) {
      theCursorType = Direction(Right); // 3
      return true;
    }
    return false;
  }
  else return false;
}

bool YACSPrs_BlocNode::isResizing()
{ 
  if ( myDisplayMode == Expanded ) return myResizing;
  else return false;
}

void YACSPrs_BlocNode::beforeResizing(int theCursorType)
{ 
  if ( myDisplayMode == Expanded )
  {
    myResizing = true; 
    myResizeDirection = Direction(theCursorType);
  }
}
  
void YACSPrs_BlocNode::resize(QPoint thePoint)
{
  if ( myDisplayMode == Expanded )
  {
    if ( !checkArea(0,0,thePoint) ) return;

    QPoint aBottomP = thePoint + QPoint(0, ( myPointMaster ? myPointMaster->height()/2 : 0 ));
    QPoint aLeftP = thePoint + QPoint(-HOOKPOINTGATE_SIZE, 0);
    QPoint aRightP = thePoint + QPoint(HOOKPOINTGATE_SIZE, 0);

    myContentMoving = false;
    int xP = thePoint.x();
    int yP = thePoint.y();
    switch (myResizeDirection)
      {
      case 1: // left edge
	if ( xP < minXContent() && checkArea(0,0,aLeftP) ) {
	  myWidth += (int)x() - xP;
	  setX(xP);
	}
	break;
      case 2: // top edge
	if ( yP < minYContent() ) {
	  myHeight += (int)y() - yP;
	  setY(yP);
	}
	break;
      case 3: // right edge
	if ( xP > maxXContent() && checkArea(0,0,aRightP) )
	  myWidth += xP - (int)x() - myWidth;
	break;
      case 4: // bottom edge
	if ( yP > maxYContent() && checkArea(0,0,aBottomP) )
	  myHeight += yP - (int)y() - myHeight;
	break;
      case 5: // left and top edges
	if ( checkArea(0,0,aLeftP) )
	{
	  if ( xP < minXContent() ) {
	    myWidth += (int)x() - xP;
	    setX(xP);
	  }
	  if ( yP < minYContent() ) {
	    myHeight += (int)y() - yP;
	    setY(yP);
	  }
	}
	break;
      case 6: // right and top edges
	if ( checkArea(0,0,aRightP) )
	{
	  if ( xP > maxXContent() )
	    myWidth += xP - (int)x() - myWidth;
	  if ( yP < minYContent() ) {
	    myHeight += (int)y() - yP;
	    setY(yP);
	  }
	}
	break;
      case 7: // right and bottom edges
	if ( checkArea(0,0,aBottomP) && checkArea(0,0,aRightP) )
	{
	  if ( xP > maxXContent() )
	    myWidth += xP - (int)x() - myWidth;
	  if ( yP > maxYContent() )
	    myHeight += yP - (int)y() - myHeight;
	}
	break;
      case 8: // left and bottom edges
	if ( checkArea(0,0,aLeftP) && checkArea(0,0,aBottomP) )
	{
	  if ( xP < minXContent() ) {
	    myWidth += (int)x() - xP;
	    setX(xP);
	  }
	  if ( yP > maxYContent() )
	    myHeight += yP - (int)y() - myHeight;
	}
	break;
      default:
	break;
      }
    QPoint aPnt = getConnectionMasterPoint();
    if ( myPointMaster ) myPointMaster->setCoords(aPnt.x(), aPnt.y());

    updateGates();

    if ( canvas() ) {
      
      QRect aChangedRect = getRect();
      aChangedRect.setRect(aChangedRect.x()-2, aChangedRect.y()-2, 
                           aChangedRect.width()*2/*+4*/, aChangedRect.height()*2/*+4*/);
      canvas()->setChanged(aChangedRect);
      canvas()->update();
    }

    for ( std::set<YACSPrs_ElementaryNode*>::iterator it = myChildren.begin(); it != myChildren.end(); it++ )
      (*it)->setArea(getAreaRect());

    myContentMoving = true;
  }
}

void YACSPrs_BlocNode::afterResizing()
{ 
  if ( myDisplayMode == Expanded ) myResizing = false; 
}

void YACSPrs_BlocNode::showPopup(QWidget* theParent, QMouseEvent* theEvent, const QPoint& theMousePos)
{
}

int YACSPrs_BlocNode::rtti() const
{
  return 0;//YACSPrs_Canvas::Rtti_BlocNode;
}

void YACSPrs_BlocNode::setVisible(bool b)
{
  QCanvasPolygonalItem::setVisible(b);

  if ( myDisplayMode == Expanded )
  {
    for ( std::set<YACSPrs_ElementaryNode*>::iterator it = myChildren.begin(); it != myChildren.end(); it++ )
      (*it)->setVisible(b);
    // set visibility to all ports
    YACSPrs_Port* aPort;
    for (aPort = myPortList.first(); aPort; aPort = myPortList.next())
      aPort->setVisible(b);
  }
  else
  {
    // not yet implemented
    // set visibility to all ports
    YACSPrs_Port* aPort;
    for (aPort = myPortList.first(); aPort; aPort = myPortList.next())
      if (aPort->isVisible()) aPort->setVisible(b);
  }
  
  // set visibility to master point
  if ( myPointMaster ) myPointMaster->setVisible(b);
  updateLabelLink();
}

QPointArray YACSPrs_BlocNode::constructAreaPoints(int theW, int theH) const
{
  int w = theW+2;
  int h = theH+1; // add width of pen

  if ( myDisplayMode == Expanded )
  {
    QPointArray aPnts(4);
    aPnts[0] = QPoint((int)x()-2, (int)y()-2) + QPoint(-NODEBOUNDARY_MARGIN/2,-NODEBOUNDARY_MARGIN/2);
    aPnts[1] = aPnts[0] + QPoint(w, 0) + QPoint(NODEBOUNDARY_MARGIN,0);
    aPnts[2] = aPnts[1] + QPoint(0, h) + QPoint(0,NODEBOUNDARY_MARGIN);
    aPnts[3] = aPnts[0] + QPoint(0, h) + QPoint(0,NODEBOUNDARY_MARGIN);
    return aPnts;
  }
  else
  {
    return QPointArray(); // not yet implemented
  }
}

void YACSPrs_BlocNode::moveBy(double dx, double dy)
{
  // for constraint nodes' moving inside the Bloc-->
  if ( isCheckAreaNeeded() && !checkArea(dx,dy) ) return;
  // <--

  int aX = (int) (x()+dx);
  int aY = (int) (y()+dy);
  int xx = aX - (int)x();
  int yy = aY - (int)y();

  if ( canvas() )
  {  
    int w = aX + width() + GRAPH_MARGIN;
    int h = aY + height() + GRAPH_MARGIN;
    if (canvas()->width() > w) w = canvas()->width();
    if (canvas()->height() > h) h = canvas()->height();
    if (canvas()->width() < w || canvas()->height() < h)
      canvas()->resize(w, h);
  }

  if ( myDisplayMode == Expanded )
  {
    // move all children if mouse is in background of Bloc node in expanded mode
    QCanvasPolygonalItem::moveBy(dx, dy); // move the bounding rectangle
    if ( myContentMoving ) {
      for ( std::set<YACSPrs_ElementaryNode*>::iterator it = myChildren.begin(); it != myChildren.end(); it++ )
	(*it)->setSelfMoving(false); // for move link's points as a content of moving Bloc node
      for ( std::set<YACSPrs_ElementaryNode*>::iterator it = myChildren.begin(); it != myChildren.end(); it++ )
      {
	(*it)->setArea(getAreaRect());
	(*it)->moveBy(xx, yy);
      }
      for ( std::set<YACSPrs_ElementaryNode*>::iterator it = myChildren.begin(); it != myChildren.end(); it++ )
	(*it)->setSelfMoving(true); // for move link's points as a content of moving Bloc node
    }
    
    // update port's rectangle
    updateGates();
    //YACSPrs_Port* aPort;
    //for (aPort = myPortList.first(); aPort; aPort = myPortList.next())
    //  aPort->moveBy(xx, yy);
  }
  else
  {
    QCanvasPolygonalItem::moveBy(dx, dy);

    // not yet implemented

    // update port's rectangle
    YACSPrs_Port* aPort;
    for (aPort = myPortList.first(); aPort; aPort = myPortList.next())
      aPort->moveBy(xx, yy);
  }

  if ( myPointMaster ) myPointMaster->moveBy(dx, dy);
  if ( myLabelLink ) myLabelLink->moveByNode(this, (int)dx, (int)dy);
  
  if ( isSelected() && canvas() && isMoving() )
  {
    QRect aRect = boundingRect();
    QPoint aShift(20,20);
    aRect.setTopLeft(aRect.topLeft()-aShift);
    aRect.setBottomRight(aRect.bottomRight()+aShift);
    canvas()->setChanged(aRect);
    canvas()->update();
  }
}

void YACSPrs_BlocNode::setZ(double z)
{
  QCanvasItem::setZ(z);

  if ( myDisplayMode == Expanded )
    for ( std::set<YACSPrs_ElementaryNode*>::iterator it = myChildren.begin(); it != myChildren.end(); it++ ) {
      if ( dynamic_cast<YACSPrs_BlocNode*>( *it ) ) (*it)->setZ(z+4);
      else (*it)->setZ(z+3);
    }
  else
  {
    // not yet implemented
  }

  // update port's 
  YACSPrs_Port* aPort;
  for (aPort = myPortList.first(); aPort; aPort = myPortList.next())
    aPort->setZ(z,false);
  
  if ( myPointMaster ) myPointMaster->setZ(z);
}

void YACSPrs_BlocNode::setX(int x)
{
  QCanvasPolygonalItem::setX(x);
  myArea = boundingRect();
}

void YACSPrs_BlocNode::setY(int y)
{
  QCanvasPolygonalItem::setY(y);
  myArea = boundingRect();
}

void YACSPrs_BlocNode::update()
{
  YACSPrs_ElementaryNode::update();
}

void YACSPrs_BlocNode::updateGates()
{
  bool aDisp = isVisible();
  if (aDisp) hide();
 
  if ( myDisplayMode == Expanded )
  {
    QRect aRect = getRect();
    int aPRectWidth = (int)(aRect.width()/2) - 2*PORT_MARGIN;
    
    int ix = aRect.left() + PORT_MARGIN;// + 1;
    int iy = aRect.bottom()-getGateHeight() + PORT_MARGIN;// + 1;
    int ox = ix + aPRectWidth + 2*PORT_MARGIN + 2; // add pen width
    int oy = aRect.bottom()-getGateHeight() + PORT_MARGIN;// + 1;
    
    if ( myPortList.isEmpty() )
    { // create (and update)
      // input Gate
      YACSPrs_InOutPort* anInPort = new YACSPrs_InOutPort(myMgr,canvas(),getEngine()->getInGate(),this);
      anInPort->setPortRect(QRect(ix, iy, aPRectWidth, PORT_HEIGHT));
      anInPort->setColor(nodeSubColor());
      anInPort->setStoreColor(nodeSubColor());
      myPortList.append(anInPort);
      
      // output Gate
      YACSPrs_InOutPort* anOutPort = new YACSPrs_InOutPort(myMgr,canvas(),getEngine()->getOutGate(),this);
      anOutPort->setPortRect(QRect(ox, oy, aPRectWidth, PORT_HEIGHT));
      anOutPort->setColor(nodeSubColor());
      anOutPort->setStoreColor(nodeSubColor());
      myPortList.append(anOutPort);
    }
    else
    { // only update
      YACSPrs_Port* aPort;
      for (aPort = myPortList.first(); aPort; aPort = myPortList.next())
      { 
	YACSPrs_InOutPort* anIOPort = dynamic_cast<YACSPrs_InOutPort*>( aPort );
	if ( anIOPort && anIOPort->isGate() )
        {
	  anIOPort->setStoreColor(nodeSubColor());
	  if ( anIOPort->isInput() ) // test input ports
	    anIOPort->setPortRect(QRect(ix, iy, aPRectWidth, PORT_HEIGHT), !isSelfMoving(), myArea);
	  else // test output ports
	    anIOPort->setPortRect(QRect(ox, oy, aPRectWidth, PORT_HEIGHT), !isSelfMoving(), myArea);
	}
      }
    }
  }
  else
  {
    // not yet implemented
  }

  if (aDisp) show();  
}

int YACSPrs_BlocNode::width() const
{
  if ( myDisplayMode == Expanded ) return myWidth;
  else return 0; // not yet implemented
}

int YACSPrs_BlocNode::height() const
{
  if ( myDisplayMode == Expanded ) return myHeight;
  else return 0; // not yet implemented
}

void YACSPrs_BlocNode::resize(int theWidth, int theHeight)
{
  if ( myDisplayMode == Expanded )
  {
    myContentMoving = false;

    myWidth = theWidth;
    myHeight = theHeight;

    QPoint aPnt = getConnectionMasterPoint();
    if ( myPointMaster ) myPointMaster->setCoords(aPnt.x(), aPnt.y());

    updateGates();

    if ( canvas() ) {
      
      QRect aChangedRect = getRect();
      aChangedRect.setRect(aChangedRect.x()-2, aChangedRect.y()-2, 
                           aChangedRect.width()*2/*+4*/, aChangedRect.height()*2/*+4*/);
      canvas()->setChanged(aChangedRect);
      canvas()->update();
    }

    for ( std::set<YACSPrs_ElementaryNode*>::iterator it = myChildren.begin(); it != myChildren.end(); it++ )
      (*it)->setArea(getAreaRect());

    myContentMoving = true;
  }
}

void YACSPrs_BlocNode::updateHeight()
{
  if ( myDisplayMode == Expanded )
  { 
    int aMinY = minYContent();
    int aMaxY = maxYContent();
    myHeight = aMaxY - aMinY;
   
    resize( myWidth, myHeight );
  }
}

int YACSPrs_BlocNode::minXContent()
{
  if ( myDisplayMode == Expanded )
  {
    int aMinX = (!myChildren.empty()) ? (int)((*myChildren.begin())->boundingRect().left()) : boundingRect().left() + 2*BLOCNODE_MARGIN;
    for ( std::set<YACSPrs_ElementaryNode*>::iterator it = myChildren.begin(); it != myChildren.end(); it++ )
      if ( aMinX > (*it)->boundingRect().left() ) aMinX = (int)( (*it)->boundingRect().left() );
    
    aMinX -= 2*HOOKPOINT_SIZE + BLOCNODE_MARGIN;
    return aMinX;
  }
  return 0;
}

int YACSPrs_BlocNode::maxXContent()
{
  if ( myDisplayMode == Expanded )
  {
    int aMaxX = (!myChildren.empty()) ? (int)((*myChildren.begin())->boundingRect().right()) : boundingRect().right() - 2*BLOCNODE_MARGIN;
    for ( std::set<YACSPrs_ElementaryNode*>::iterator it = myChildren.begin(); it != myChildren.end(); it++ )
      if ( aMaxX < (*it)->boundingRect().right() ) aMaxX = (int)( (*it)->boundingRect().right() );
    
    aMaxX += 2*HOOKPOINT_SIZE + BLOCNODE_MARGIN;
    return aMaxX;
  }
  return 0;
}

int YACSPrs_BlocNode::minYContent()
{
  if ( myDisplayMode == Expanded )
  {
    int aMinY = (!myChildren.empty()) ? (int)((*myChildren.begin())->boundingRect().top()) : (int)x();
    for ( std::set<YACSPrs_ElementaryNode*>::iterator it = myChildren.begin(); it != myChildren.end(); it++ )
      if ( aMinY > (*it)->boundingRect().top() ) aMinY = (int)( (*it)->boundingRect().top() );
    
    aMinY -= getTitleHeight() + BLOCNODE_MARGIN;
    return aMinY;
  }
  return 0;
}

int YACSPrs_BlocNode::maxYContent()
{
  if ( myDisplayMode == Expanded )
  {
    int aMaxY = (!myChildren.empty()) ? (int)((*myChildren.begin())->boundingRect().bottom()) : (int)x();
    for ( std::set<YACSPrs_ElementaryNode*>::iterator it = myChildren.begin(); it != myChildren.end(); it++ )
      if ( aMaxY < (*it)->boundingRect().bottom() ) aMaxY = (int)( (*it)->boundingRect().bottom() );
    
    aMaxY += getGateHeight() + (myPointMaster ? myPointMaster->height()/2 : 0 ) + BLOCNODE_MARGIN;
    return aMaxY;
  }
  return 0;
}

int YACSPrs_BlocNode::minWidth() const
{
  if ( myDisplayMode == Expanded )
  {
    int aMinWidth = myPointMaster ? myPointMaster->width() : 0;
    int aMinX = (!myChildren.empty()) ? (int)((*myChildren.begin())->x()) : (int)x();
    int aMaxX = aMinX;
    int aChildMaxWidth = 0;
    for ( std::set<YACSPrs_ElementaryNode*>::iterator it = myChildren.begin(); it != myChildren.end(); it++ )
    {  
      if ( aMinX > (*it)->x() ) aMinX = (int)( (*it)->x() );
      if ( aMaxX < (*it)->x() ) aMaxX = (int)( (*it)->x() );
      if ( aChildMaxWidth < (*it)->maxWidth() ) aChildMaxWidth = (*it)->maxWidth();
    }
    if ( aMinWidth < aMaxX - aMinX + aChildMaxWidth ) aMinWidth = aMaxX - aMinX + aChildMaxWidth + 2*BLOCNODE_MARGIN;
    return aMinWidth;
  }
  else
  {
    return width();// + sliseWidth; // not yet implemented
  }
}

int YACSPrs_BlocNode::minHeight() const
{
  if ( myDisplayMode == Expanded )
  {
    int aMinHeight =  getTitleHeight() + getGateHeight() + 2*BLOCNODE_MARGIN;
    int aMinY = (!myChildren.empty()) ? (int)((*myChildren.begin())->y()) : (int)y();
    int aMaxY = aMinY;
    int aChildMaxHeight = 0;
    for ( std::set<YACSPrs_ElementaryNode*>::iterator it = myChildren.begin(); it != myChildren.end(); it++ )
    {  
      if ( aMinY > (*it)->y() ) aMinY = (int)( (*it)->y() );
      if ( aMaxY < (*it)->y() ) aMaxY = (int)( (*it)->y() );
      if ( aChildMaxHeight < (*it)->maxHeight() ) aChildMaxHeight = (*it)->maxHeight();
      
      YACSPrs_LoopNode* aLoop = dynamic_cast<YACSPrs_LoopNode*>( *it );
      if ( aLoop ) aChildMaxHeight -= 3*TITLE_HEIGHT/2;
      else
      {
	YACSPrs_SwitchNode* aSwitch = dynamic_cast<YACSPrs_SwitchNode*>( *it );
	if ( aSwitch ) aChildMaxHeight -= 2*TITLE_HEIGHT;
      }
      
    }
    if ( aMinHeight < aMaxY - aMinY + aChildMaxHeight ) aMinHeight += aMaxY - aMinY + aChildMaxHeight;
    return aMinHeight;
  }
  else
  {
    return height();// + sliseWidth; // not yet implemented
  }
}

int YACSPrs_BlocNode::maxWidth() const
{
  if ( myDisplayMode == Expanded )
  {
    return boundingRect().width() + 2*HOOKPOINTGATE_SIZE;
  }
  else
  {
    return width();// + sliseWidth; // not yet implemented
  }
}

int YACSPrs_BlocNode::maxHeight() const
{
  if ( myDisplayMode == Expanded )
  {
    return YACSPrs_ElementaryNode::maxHeight();
  }
  else
  {
    return YACSPrs_ElementaryNode::maxHeight();// + sliseWidth; // not yet implemented
  }
}


QRect YACSPrs_BlocNode::getRect() const
{
  return QRect((int)x(), (int)y(), width(), height());
}

QRect YACSPrs_BlocNode::getTitleRect() const
{
  if ( myDisplayMode == Expanded )
    return QRect((int)x(), (int)y(), width(), getTitleHeight());
  else
    return YACSPrs_ElementaryNode::getTitleRect();
}

QRect YACSPrs_BlocNode::getGateRect() const
{
  if ( myDisplayMode == Expanded )
    return QRect((int)x(), getRect().bottom()-getGateHeight(), width(), getGateHeight());
  else
    return YACSPrs_ElementaryNode::getGateRect();
}

QRect YACSPrs_BlocNode::getAreaRect() const
{
  if ( myDisplayMode == Expanded )
  {
    QPoint aShift(BLOCNODE_MARGIN/2, BLOCNODE_MARGIN/2);
    QPoint aTopLeft = getTitleRect().bottomLeft() + aShift;
    QPoint aBottomRight = getGateRect().topRight() - aShift;
    QRect anArea(aTopLeft, aBottomRight);
    return anArea;
  }
  return QRect();
}

bool YACSPrs_BlocNode::checkArea(double dx, double dy, QPoint thePoint)
{
  if ( myDisplayMode == Expanded )
  {
    // for constraint nodes' moving inside the Bloc-->
    if ( !myIsInBloc ) return true;
    
    if ( !thePoint.isNull() )
      if ( myArea.isValid() && myArea.contains(thePoint,true) ) return true;
      else return false;
    
    QRect aRect = boundingRect();
    aRect.moveBy((int)dx, (int)dy);
    aRect.setRect(aRect.x()-HOOKPOINTGATE_SIZE, aRect.y() + NODEBOUNDARY_MARGIN/2, maxWidth(), maxHeight());
    if ( myArea.isValid() && myArea.contains(aRect) )
      return true;
    return false;
    // <--
  }
  return true;
}

void YACSPrs_BlocNode::drawShape(QPainter& thePainter)
{
  if ( myDisplayMode == Expanded )
  {
    drawFrame(thePainter);
    drawTitle(thePainter);
    drawGate(thePainter);

    if ( isControlDMode() && myPointMaster)
    {
      myPointMaster->setVisible(false);
      myPointMaster->setCanvas(0);
    }
  }
  else
  { 
    // not yet implemented
  }
}

void YACSPrs_BlocNode::drawTitle(QPainter& thePainter)
{
  QPen savedP = thePainter.pen();
  thePainter.setPen(QPen(myBoundColor, 2, Qt::SolidLine));
  thePainter.drawLine((int)x(), (int)(y()+getTitleHeight()), (int)(x()+width()), (int)(y()+getTitleHeight()));

  QBrush savedB = thePainter.brush();
  thePainter.setBrush(nodeSubColor());
  thePainter.setPen(thePainter.brush().color().dark(140));

  QRect aRoundRect = getTitleRect();
  aRoundRect.setRect(aRoundRect.x() + PORT_MARGIN, aRoundRect.y() + PORT_MARGIN,
		     aRoundRect.width() - 2*PORT_MARGIN, aRoundRect.height() - 2*PORT_MARGIN);
  thePainter.drawRoundRect(aRoundRect,myXRnd,myYRnd);

  thePainter.setPen(Qt::white);
  drawText4(thePainter, QString(getEngine()->getName()), aRoundRect, Qt::AlignHCenter);

  thePainter.setBrush(savedB);
  thePainter.setPen(savedP);
}

void YACSPrs_BlocNode::drawGate(QPainter& thePainter)
{
  if ( myDisplayMode == Expanded )
  {
    QPen savedP = thePainter.pen();
    thePainter.setPen(QPen(myBoundColor, 2, Qt::SolidLine));
    
    QRect aRect = getRect();
    int aGateTop = aRect.bottom()-getGateHeight();
    thePainter.drawLine(aRect.left(), aGateTop, aRect.right(), aGateTop);
    
    int x0 = (aRect.left() + aRect.right())/2;
    thePainter.drawLine(x0, aGateTop, x0, aRect.bottom());
    
    int aRRectWidth = x0 - aRect.left() - 2*PORT_MARGIN;
    //QRect aTRect = getTitleRect();
    //int aXRnd = aTRect.width()*myXRnd/aRRectWidth;
    //int aYRnd = aTRect.height()*myYRnd/PORT_HEIGHT;

    thePainter.setPen(nodeSubColor());
    YACSPrs_Port* aPort;
    for (aPort = myPortList.first(); aPort; aPort = myPortList.next())
      if ( (YACSPrs_InOutPort*)aPort && ((YACSPrs_InOutPort*)aPort)->isGate() )
	aPort->draw(thePainter,myXRnd,myYRnd);//aXRnd,aYRnd);

    thePainter.setPen(savedP);
  }
  else
  {
    // not yet implemented
  }
}

void YACSPrs_BlocNode::drawFrame(QPainter& thePainter)
{
  if ( myDisplayMode == Expanded )
  {
    QBrush savedB = thePainter.brush();
    thePainter.setBrush(nodeColor());/*NoBrush*/

    QPen savedP = thePainter.pen();
    thePainter.setPen(QPen(myBoundColor, 2, Qt::DashLine));
    
    thePainter.drawRect(getRect());
    
    thePainter.setPen(savedP);
    thePainter.setBrush(savedB);

    // draw bounding nodes' polygon if node is currently selected
    if ( isSelected() ) drawBoundary(thePainter,2);
  }
  else
  { 
    // not yet implemented
  }
}

QString YACSPrs_BlocNode::getToolTipText(const QPoint& theMousePos, QRect& theRect) const
{
  // Check if the tooltip for ports is needed
  if (getBodyRect().contains(theMousePos, true) || getGateRect().contains(theMousePos, true))
    return YACSPrs_ElementaryNode::getToolTipText(theMousePos,theRect);

  QString aText = QString("");
  aText += QString("Name: %1\n").arg(getEngine()->getName());
  aText += QString("Type: %1\n").arg("Block node");
  theRect = getTitleRect();
  theRect = theRect.unite(getAreaRect());
  return aText;
}
