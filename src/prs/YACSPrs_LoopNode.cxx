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

#include "YACSPrs_LoopNode.h"
#include "YACSPrs_Def.h"

#include "SUIT_ResourceMgr.h"

#include <qpainter.h>

#include <ForLoop.hxx>
#include <WhileLoop.hxx>

using namespace YACS::ENGINE;

/*!
  Constructor
*/
YACSPrs_LoopNode::YACSPrs_LoopNode( SUIT_ResourceMgr* theMgr, QCanvas* theCanvas,
				    YACS::HMI::SubjectNode* theSNode, const bool& thePortUpdate ):
  YACSPrs_InlineNode(theMgr, theCanvas, theSNode, false)
{
  setNodeColor(LOOPNODE_COLOR);
  setNodeSubColor(LOOPNODE_SUBCOLOR);
  setBracketColor(LOOPNODE_BRACKETCOLOR);

  setStoreColor(nodeColor());
  setStoreSubColor(nodeSubColor());
  myStoreBracketColor = myBracketColor;

  myTopPixmap = myMgr->loadPixmap( "YACSPrs", QObject::tr( "ICON_TITLE_RARROW" ));
  myBottomPixmap = myMgr->loadPixmap( "YACSPrs", QObject::tr( "ICON_TITLE_LARROW" ));
  
  if ( thePortUpdate ) {
    //updatePorts(); // will be called in moveBy(...) function
    if ( isFullDMode() )
      moveBy(3*TITLE_HEIGHT/2+NODEBOUNDARY_MARGIN, 3*TITLE_HEIGHT/2+NODEBOUNDARY_MARGIN);
    else if ( isControlDMode() )
      moveBy(2*HOOKPOINT_SIZE+NODEBOUNDARY_MARGIN,2*HOOKPOINT_SIZE+NODEBOUNDARY_MARGIN);
  }
}

/*!
  Destructor
*/
YACSPrs_LoopNode::~YACSPrs_LoopNode()
{
}

void YACSPrs_LoopNode::select(const QPoint& theMousePos, const bool toSelect)
{
  if ( toSelect ) 
  {
    // unhilight the item under the mouse cursor
    hilight(theMousePos, false);

    bool isOnPort = false;
    if (getBodyRect().contains(theMousePos, true) || getGateRect().contains(theMousePos, true))
    {
      // process ports
      QPtrList<YACSPrs_Port> aPortList = getPortList();
      for (YACSPrs_Port* aPort = aPortList.first(); aPort; aPort = aPortList.next()) {
	if (aPort->getPortRect().contains(theMousePos, true)) {
	  isOnPort = true;
	  if ( aPort != mySelectedPort )
	  {
	    if ( mySelectedPort )
	    {
	      mySelectedPort->setSelected(false);
	      mySelectedPort->setColor(mySelectedPort->storeColor(), false, true, true);
              synchronize( mySelectedPort, false );
	    }
	    else
	    {
	      setSelected(false);
	      getSEngine()->select(false);

	      setBracketColor( myStoreBracketColor );
	      setNodeSubColor( storeSubColor(), true );
	    }
	    aPort->setSelected(true);
	    aPort->setColor(aPort->Color().dark(130), false, true, true);
	    mySelectedPort = aPort;
            synchronize( mySelectedPort, true );
	  }
	  break;
	}
      }
    }
    
    if ( !isOnPort )
    {
      if ( mySelectedPort )
      {
	mySelectedPort->setSelected(false);
        synchronize( mySelectedPort, false );
	mySelectedPort = 0;
      }
      
      if ( storeSubColor().dark(130) != nodeSubColor() )
      {
	myStoreBracketColor = myBracketColor;
	setStoreSubColor( nodeSubColor() );
	
	setSelected(true);
	getSEngine()->select(true);

	setBracketColor( bracketColor().dark(130) );
	setNodeSubColor( nodeSubColor().dark(130), true );
      }
    }
  }
  else
  {
    if ( mySelectedPort ) {
      mySelectedPort->setSelected(false);
      mySelectedPort->setColor(mySelectedPort->storeColor(), false, true, true);
      synchronize( mySelectedPort, false );
      mySelectedPort = 0;
    }
    else {
      setSelected(false);
      getSEngine()->select(false);

      setBracketColor( myStoreBracketColor );
      setNodeSubColor( storeSubColor(), true );
    }
  }
}

int YACSPrs_LoopNode::rtti() const
{
  return 0;//YACSPrs_Canvas::Rtti_LoopNode;
}

QPointArray YACSPrs_LoopNode::constructAreaPoints(int theW, int theH) const
{
  QPointArray aPnts;

  if ( isControlDMode() )
  {
    aPnts = QPointArray(4);
    aPnts[0] = QPoint((int)x(), (int)y()) + QPoint(-NODEBOUNDARY_MARGIN,-NODEBOUNDARY_MARGIN);
    aPnts[1] = aPnts[0] + QPoint(theW, 0) + QPoint(NODEBOUNDARY_MARGIN,0);
    aPnts[2] = aPnts[1] + QPoint(0, theH) + QPoint(0,NODEBOUNDARY_MARGIN);
    aPnts[3] = aPnts[0] + QPoint(0, theH) + QPoint(0,NODEBOUNDARY_MARGIN);
  }
  else if ( isFullDMode() )
  {
    int h = theH-1;
    
    int aCorner = 3*TITLE_HEIGHT/2;
    
    aPnts = QPointArray(8);
    QPoint p((int)x(), (int)y());
    p = p + QPoint(-NODEBOUNDARY_MARGIN,0);
    aPnts[0] = p;
    aPnts[1] = p + QPoint(aCorner, -aCorner) + QPoint(NODEBOUNDARY_MARGIN,-NODEBOUNDARY_MARGIN);
    aPnts[2] = aPnts[1] + QPoint(theW, 0) + QPoint(NODEBOUNDARY_MARGIN,0);
    aPnts[3] = p + QPoint(theW, 0) + QPoint(NODEBOUNDARY_MARGIN,0);
    aPnts[4] = aPnts[3] + QPoint(0, h) + QPoint(0,-NODEBOUNDARY_MARGIN/2);
    aPnts[5] = aPnts[4] + QPoint(-aCorner, aCorner) + QPoint(-NODEBOUNDARY_MARGIN,NODEBOUNDARY_MARGIN);
    aPnts[6] = aPnts[5] + QPoint(-theW, 0) + QPoint(-NODEBOUNDARY_MARGIN,0);
    aPnts[7] = p + QPoint(0, h) + QPoint(0,-NODEBOUNDARY_MARGIN/2);
  }

  return aPnts;
}

void YACSPrs_LoopNode::setBracketColor(const QColor& theColor, bool theUpdate)
{
  myBracketColor = theColor;
  if ( canvas() && theUpdate )
  { 
    canvas()->setChanged(boundingRect());
    canvas()->update();
  }
}

void YACSPrs_LoopNode::updatePorts(bool theForce)
{
  bool aDisp = isVisible();
  if (aDisp) hide();

  bool withCreate = theForce;

  if (theForce)
  {
    if ( isFullDMode() )
      myPortHeight = 2*PORT_MARGIN;
    else
      myPortHeight = 0;

    myPortList.setAutoDelete(true);
    //myPortList.clear();

    QPtrList<YACSPrs_LabelPort> aDeletePortList;
    for (YACSPrs_Port* aPort = myPortList.first(); aPort; aPort = myPortList.next())
    {
      if( YACSPrs_LabelPort* aLabelPort = dynamic_cast<YACSPrs_LabelPort*>( aPort ) )
      {
	aDeletePortList.append( aLabelPort );
	withCreate = true;
      }
    }

    for (YACSPrs_Port* aPort = aDeletePortList.first(); aPort; aPort = aDeletePortList.next())
      myPortList.remove( aPort );
  }

  // ForLoop and WhileLoop nodes have only 1 input port.
  // ForLoop : its name is 'nsteps' ,
  //           its type is 'int',
  //           its value is a number of iterations.
  // WhileLoop : its name is 'condition' ,
  //             its type is 'bool',
  //             its value is a while condition, iteration process breaks when condition switch to false.
  // ForLoop and WhileLoop nodes have no output ports, but in presentation we want to display a 'label' port with
  // name 'Body'. This 'label' port connects with help of 'case' link to 'Master' hook
  // of the node, which is set as an internal node of the loop.

  if ( myPortList.isEmpty() ) withCreate = true;

  if ( isFullDMode() )
  {
    QRect r = getBodyRect();
    int aPRectWidth = (int)(r.width()/2) - 2*PORT_MARGIN;
    if ( aPRectWidth < PORT_WIDTH ) aPRectWidth = PORT_WIDTH;
    
    int ix = r.x() + PORT_MARGIN + 1;
    int iy = r.y() + PORT_MARGIN;// + 1;
    int ox = ix + aPRectWidth + 2*PORT_MARGIN;
    int oy = r.y() + PORT_MARGIN;// + 1;
    
    ForLoop* aFLoop = dynamic_cast<ForLoop*>( getEngine() );
    WhileLoop* aWLoop = dynamic_cast<WhileLoop*>( getEngine() );
    
    if ( withCreate )
    { // create (and update)
      // 'nsteps'/'condition' input port (name, type (and value) of the port will set in YACSPrs_InOutPort from port engine)
      bool isConditionPortCreated = false;
      InputPort* aConditionPort = 0;
      if( aFLoop )
	aConditionPort = aFLoop->edGetNbOfTimesInputPort();
      else if( aWLoop )
	aConditionPort = aWLoop->edGetConditionPort();

      for (YACSPrs_Port* aPort = myPortList.first(); aPort; aPort = myPortList.next())
      {
	if( !aPort->getName().compare( QString( aConditionPort->getName() ) ) )
	{
	  isConditionPortCreated = true;
	  break;
	}
      }
      if( !isConditionPortCreated )
      {
	YACSPrs_InOutPort* anInPort = 0;
	if ( aFLoop )
	  anInPort = new YACSPrs_InOutPort(myMgr,canvas(),aFLoop->edGetNbOfTimesInputPort(),this);
	else if ( aWLoop )
	  anInPort = new YACSPrs_InOutPort(myMgr,canvas(),aWLoop->edGetConditionPort(),this);
      
	if ( anInPort )
	{
	  anInPort->setPortRect(QRect(ix, iy, aPRectWidth, PORT_HEIGHT));
	  anInPort->setColor(nodeSubColor());
	  anInPort->setStoreColor(nodeSubColor());
	  myPortList.append(anInPort);
	}
      }
      
      if ( aFLoop || aWLoop )
      {
	// get a set of internal loop nodes (in fact in ForLoop and WhileLoop we have only one internal node)
	std::list<Node*> aNodes = aFLoop ? aFLoop->edGetDirectDescendants() : aWLoop->edGetDirectDescendants();
	if ( aNodes.empty() )
	  myPortHeight += PORT_HEIGHT;
	else
	{
	  std::list<Node*>::iterator aNodesIter = aNodes.begin();
	  for (; aNodesIter != aNodes.end(); aNodesIter++)
	  { // output label port
	    YACSPrs_LabelPort* anOutPort = new YACSPrs_LabelPort(myMgr,canvas(),*aNodesIter,this);
	    anOutPort->setPortRect(QRect(ox, oy, aPRectWidth, PORT_HEIGHT));
	    anOutPort->setColor(nodeSubColor().dark(140));
	    anOutPort->setStoreColor(nodeSubColor().dark(140));
	    myPortList.append(anOutPort);
	    myPortHeight += PORT_HEIGHT;
	  }
	}
      }
    }
    else
    { // only update
      YACSPrs_Port* aPort;
      for (aPort = myPortList.first(); aPort; aPort = myPortList.next())
      { 
	YACSPrs_InOutPort* anInPort = dynamic_cast<YACSPrs_InOutPort*>( aPort );
	if ( anInPort )
	{ 
	  if ( !anInPort->isGate() && anInPort->isInput() )
	  { // input data (i.e. not Gate) ports
	    anInPort->setPortRect(QRect(ix, iy, aPRectWidth, PORT_HEIGHT), !isSelfMoving(), myArea);
	    iy += PORT_HEIGHT+PORT_SPACE;
	  }
	}
	else
	{ // not YACSPrs_InOutPort => it is YACSPrs_LabelPort => we not need to dynamic cast
	  aPort->setPortRect(QRect(ox, oy, aPRectWidth, PORT_HEIGHT), !isSelfMoving(), myArea);
	  oy += PORT_HEIGHT+PORT_SPACE;
	}
      }
    }
  }
    
  // can update gates only after body height will be defined
  bool createGates = withCreate;
  for (YACSPrs_Port* aPort = myPortList.first(); aPort; aPort = myPortList.next())
  {
    if( YACSPrs_InOutPort* anIOPort = dynamic_cast<YACSPrs_InOutPort*>( aPort ) )
    {
      if ( anIOPort->isGate() ) 
      { // gate ports are already created - we should only update them
	createGates = false;
	break;
      }
    }
  }
  updateGates(createGates);

  if (theForce && myPointMaster)
  {
    QPoint aPnt = getConnectionMasterPoint();
    myPointMaster->setCoords(aPnt.x(), aPnt.y());
  }

  if (aDisp) show();
}

int YACSPrs_LoopNode::maxWidth() const
{
  if ( isControlDMode() )
    return YACSPrs_ElementaryNode::maxWidth();

  // Full view mode as a default
  return YACSPrs_ElementaryNode::maxWidth() - 4*HOOKPOINT_SIZE + ( ( 4*HOOKPOINT_SIZE > 3*TITLE_HEIGHT ) ? 4*HOOKPOINT_SIZE-3*TITLE_HEIGHT : 0 );
}

int YACSPrs_LoopNode::minX() const
{
  return YACSPrs_ElementaryNode::minX() + 2*HOOKPOINT_SIZE - ( ( 2*HOOKPOINT_SIZE > 3*TITLE_HEIGHT/2 ) ? 
							       2*HOOKPOINT_SIZE :
							       3*TITLE_HEIGHT/2 );
}

int YACSPrs_LoopNode::maxX() const
{
  return YACSPrs_ElementaryNode::maxX() - 2*HOOKPOINT_SIZE + ( ( 2*HOOKPOINT_SIZE > 3*TITLE_HEIGHT/2 ) ? 
							       2*HOOKPOINT_SIZE :
							       3*TITLE_HEIGHT/2 );
}

void YACSPrs_LoopNode::drawPort(QPainter& thePainter)
{
  QRect r = getBodyRect();
  r.setHeight(r.height()+1);

  thePainter.drawRect(r);
  int x0 = (r.left() + r.right())/2;
  thePainter.drawLine(x0, r.top(), x0, r.bottom());

  int aRRectWidth = (x0 - r.left() - 2*PORT_MARGIN - 2*PORT_SPACE)/3;
  int aRRectWidthLabel = x0 - r.left() - 2*PORT_MARGIN;
  QRect aTRect = getTitleRect();
  int aXRnd = aTRect.width()*myXRnd/aRRectWidth;
  int aXRndLabel = aTRect.width()*myXRnd/aRRectWidthLabel;
  int aYRnd = aTRect.height()*myYRnd/PORT_HEIGHT;

  YACSPrs_Port* aPort;
  for (aPort = myPortList.first(); aPort; aPort = myPortList.next())
  {
    YACSPrs_InOutPort* anInPort = dynamic_cast<YACSPrs_InOutPort*>( aPort );
    YACSPrs_LabelPort* anOutPort = dynamic_cast<YACSPrs_LabelPort*>( aPort );
    if ( anInPort && !anInPort->isGate() )
      aPort->draw(thePainter, aXRnd, aYRnd);
    if ( anOutPort )
      aPort->draw(thePainter, aXRndLabel, aYRnd);
  } 
}

void YACSPrs_LoopNode::drawFrame(QPainter& thePainter)
{
  QRect aRect = getRect();
  QRect aTRect = getTitleRect();

  thePainter.drawRect(aRect);

  if ( isFullDMode() )
  {
    QBrush savedB = thePainter.brush();
    thePainter.setBrush(bracketColor());
    
    int aCorner =  3*TITLE_HEIGHT/2;
    QPoint aP1(aRect.x(),            aRect.y());
    QPoint aP2(aRect.x()+aCorner,    aRect.y()-aCorner);
    QPoint aP3(aRect.right()+aCorner,aRect.y()-aCorner);
    QPoint aP4(aRect.right(),        aRect.y());
    QPoint aP5(aRect.x(),            aRect.bottom());
    QPoint aP6(aRect.x()-aCorner,    aRect.bottom()+aCorner);
    QPoint aP7(aRect.right()-aCorner,aRect.bottom()+aCorner);
    QPoint aP8(aRect.right(),        aRect.bottom());
    QPointArray aPAUp(4);
    aPAUp.putPoints(0, 4, aP1.x(),aP1.y(), aP2.x(),aP2.y(), aP3.x(),aP3.y(), aP4.x(),aP4.y());
    thePainter.drawPolygon( aPAUp );
    QPointArray aPADown(4);
    aPADown.putPoints(0, 4, aP5.x(),aP5.y(), aP6.x(),aP6.y(), aP7.x(),aP7.y(), aP8.x(),aP8.y());
    thePainter.drawPolygon( aPADown );
    
    thePainter.setBrush(savedB);
    
    // draw top pixmap
    QRect aTPRect(aRect.x()+aRect.width()/2-aCorner/3, aRect.y()-aCorner/2-aCorner/3,
		  2*aCorner/3, 2*aCorner/3);
    thePainter.drawPixmap(aTPRect,myTopPixmap);
    
    // draw bottom pixmap
    QRect aBPRect(aRect.x()+aRect.width()/2-aCorner/3, aRect.bottom()+aCorner/2-aCorner/3,
		  2*aCorner/3, 2*aCorner/3);
    thePainter.drawPixmap(aBPRect,myBottomPixmap);
  }

  // draw bounding nodes' polygon if node is currently selected
  if ( isSelected() ) drawBoundary(thePainter,5);
}

QPoint YACSPrs_LoopNode::getConnectionMasterPoint()
{
  QRect aRect = getRect();
  return QPoint(aRect.left()+aRect.width()/2, aRect.bottom()+3*TITLE_HEIGHT/2);
}

bool YACSPrs_LoopNode::checkArea(double dx, double dy)
{
  // for constraint nodes' moving inside the Bloc-->
  if ( !myIsInBloc ) return true;

  QRect aRect = boundingRect();
  aRect.moveBy((int)dx, (int)dy);
  aRect.setRect(aRect.x() - ( ( 2*HOOKPOINT_SIZE > 3*TITLE_HEIGHT/2 ) ? 2*HOOKPOINT_SIZE-3*TITLE_HEIGHT/2 : 0 ) + NODEBOUNDARY_MARGIN,
		aRect.y(), maxWidth(), maxHeight());
  if ( myArea.isValid() && myArea.contains(aRect) )
    return true;
  return false;
  // <--
}

bool YACSPrs_LoopNode::synchronize( YACSPrs_Port* port, const bool toSelect )
{
  return YACSPrs_InOutPort::synchronize( port, toSelect );
}

