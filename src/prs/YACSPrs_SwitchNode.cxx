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

#include "YACSPrs_SwitchNode.h"
#include "YACSPrs_Def.h"

#include "QxGraph_Canvas.h"
#include "QxGraph_Prs.h"

#include "SUIT_ResourceMgr.h"

#include <qpainter.h>

#include <Switch.hxx>

using namespace YACS::ENGINE;

/*!
  Constructor
*/
YACSPrs_SwitchNode::YACSPrs_SwitchNode( SUIT_ResourceMgr* theMgr, QCanvas* theCanvas, YACS::HMI::SubjectNode* theSNode ):
  YACSPrs_InlineNode(theMgr, theCanvas, theSNode, false)
{
  setNodeColor(SWITCHNODE_COLOR);
  setNodeSubColor(SWITCHNODE_SUBCOLOR);

  setStoreColor(nodeColor());
  setStoreSubColor(nodeSubColor());

  myTitlePixmap = myMgr->loadPixmap( "YACSPrs", QObject::tr( "ICON_TITLE_RARROW" ));
  
  //updatePorts(); // will be called in moveBy(...) function
  moveBy(2*HOOKPOINT_SIZE,2*TITLE_HEIGHT);
}

/*!
  Destructor
*/
YACSPrs_SwitchNode::~YACSPrs_SwitchNode()
{
}

int YACSPrs_SwitchNode::rtti() const
{
  return 0;//YACSPrs_Canvas::Rtti_SwitchNode;
}

QPointArray YACSPrs_SwitchNode::constructAreaPoints(int theW, int theH) const
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
    int aCorner = 2*TITLE_HEIGHT;
    
    aPnts = QPointArray(5);
    QPoint p((int)x(), (int)y());
    aPnts[0] = p + QPoint(-NODEBOUNDARY_MARGIN,0);
    aPnts[1] = p + QPoint(theW/2, -aCorner) + QPoint(0,-NODEBOUNDARY_MARGIN);
    aPnts[2] = p + QPoint(theW, 0) + QPoint(NODEBOUNDARY_MARGIN,0);
    aPnts[3] = aPnts[2] + QPoint(0, theH) + QPoint(0,NODEBOUNDARY_MARGIN/2);
    aPnts[4] = p + QPoint(0, theH) + QPoint(-NODEBOUNDARY_MARGIN,NODEBOUNDARY_MARGIN/2);
  }

  return aPnts;
}

void YACSPrs_SwitchNode::updatePorts(bool theForce)
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

  // Switch node has only 1 input port: its name is 'select',
  //                                    its type is 'int',
  //                                    its value is a number of active case (i.e. the case to execute)
  // Switch node has no output ports, but in presentation we want to display a switch cases with
  // its IDs (the number of cases and its IDs are set by user at creation process of switch node )
  // as a 'label' ports. Each 'label' port connects with help of 'case' link to 'Master' hook
  // of the node, which is set to this case ID.

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
    
    if ( withCreate )
    { // create (and update)
      // 'select' input port (name, type (and value) of the port will set in YACSPrs_InOutPort from port engine)
      Switch* aSEngine = dynamic_cast<Switch*>( getEngine() );
      if ( aSEngine )
      {
	bool isConditionPortCreated = false;
	InputPort* aConditionPort = aSEngine->edGetConditionPort();
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
	  YACSPrs_InOutPort* anInPort = new YACSPrs_InOutPort(myMgr,canvas(),aConditionPort,this);
	  anInPort->setPortRect(QRect(ix, iy, aPRectWidth, PORT_HEIGHT));
	  anInPort->setColor(nodeSubColor());
	  anInPort->setStoreColor(nodeSubColor());
	  myPortList.append(anInPort);
	}
	
	// get a set of internal case nodes
	std::set<Node*> aNodes = aSEngine->edGetDirectDescendants();
	if ( aNodes.empty() )
	  myPortHeight += PORT_HEIGHT;
	else
	{
	  std::set<Node*>::iterator aNodesIter = aNodes.begin();
	  
	  // get default node
	  Node* aDefaultNode = aSEngine->getChildByShortName(Switch::DEFAULT_NODE_NAME);
	  
	  int aMinCaseId, aMaxCaseId;
	  aMinCaseId = aMaxCaseId = aSEngine->getRankOfNode(*aNodesIter);
	  // a list of case nodes ordered from minimum to maximum case id
	  std::list<Node*> aCaseNodes;
	  for (; aNodesIter != aNodes.end(); aNodesIter++)
	  {
	    if ( *aNodesIter == aDefaultNode) continue;
	    
	    // less than min => push front
	    if ( aMinCaseId >= aSEngine->getRankOfNode(*aNodesIter) ) {
	      aCaseNodes.push_front(*aNodesIter);
	      aMinCaseId = aSEngine->getRankOfNode(*aNodesIter);
	    }
	    // in the middle
	    else if ( aMinCaseId < aSEngine->getRankOfNode(*aNodesIter)
		      &&
		      aMaxCaseId > aSEngine->getRankOfNode(*aNodesIter) ) {
	      std::list<Node*>::iterator aCaseNodesIter = aCaseNodes.begin();
	      for (std::list<Node*>::iterator anIt = aCaseNodesIter;
		   anIt++ != aCaseNodes.end();
		   aCaseNodesIter++, anIt = aCaseNodesIter) {
		if ( aSEngine->getRankOfNode(*aNodesIter) >= aSEngine->getRankOfNode(*aCaseNodesIter)
		     &&
		     aSEngine->getRankOfNode(*aNodesIter) <= aSEngine->getRankOfNode(*anIt) ) {
		  aCaseNodes.insert(anIt,*aNodesIter);
		  break;
		}
	      }
	    }
	    // more than max => push back
	    else if ( aMaxCaseId <= aSEngine->getRankOfNode(*aNodesIter) ) {
	      aCaseNodes.push_back(*aNodesIter);
	      aMaxCaseId = aSEngine->getRankOfNode(*aNodesIter);
	    }
	  }
	  if ( aDefaultNode )
	    aCaseNodes.push_back(aDefaultNode);
	  
	  int heightIncr = 0;
	  std::list<Node*>::iterator aCaseNodesIter = aCaseNodes.begin();
	  for (; aCaseNodesIter != aCaseNodes.end(); aCaseNodesIter++)
	  { // (in fact we have to get from user number of switch cases)
	    // output label ports	
	    YACSPrs_LabelPort* anOutPort = new YACSPrs_LabelPort(myMgr,canvas(),*aCaseNodesIter,this,
								 true,aSEngine->getRankOfNode(*aCaseNodesIter));
	    anOutPort->setPortRect(QRect(ox, oy+heightIncr, aPRectWidth, PORT_HEIGHT));
	    anOutPort->setColor(nodeSubColor().dark(140));
	    anOutPort->setStoreColor(nodeSubColor().dark(140));
	    if ( *aCaseNodesIter == aDefaultNode) anOutPort->setName(Switch::DEFAULT_NODE_NAME);
	    myPortList.append(anOutPort);
	    heightIncr += PORT_HEIGHT+PORT_SPACE;
	  }
	  
	  myPortHeight += aNodes.size()*PORT_HEIGHT + (aNodes.size()-1)*PORT_SPACE;
	}
      }
    }
    else
    { // only update
      YACSPrs_Port* aPort;
      for (aPort = myPortList.first(); aPort; aPort = myPortList.next())
      {
	YACSPrs_InOutPort* anIOPort = dynamic_cast<YACSPrs_InOutPort*>( aPort );
	if ( anIOPort )
	{
	  if ( !anIOPort->isGate() && anIOPort->isInput() )
	  { // input data (i.e. not Gate) ports
	    anIOPort->setPortRect(QRect(ix, iy, aPRectWidth, PORT_HEIGHT), !isSelfMoving(), myArea);
	    iy += PORT_HEIGHT+PORT_SPACE;
	  }
	}
	else
	{ // not YACSPrs_InOutPort => it is YACSPrs_LabelPort (output!) => we not need to dynamic cast
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

void YACSPrs_SwitchNode::drawPort(QPainter& thePainter)
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

void YACSPrs_SwitchNode::drawFrame(QPainter& thePainter)
{
  QRect aRect = getRect();
  QRect aTRect = getTitleRect();
  int aXRnd = aTRect.width()*myXRnd/aRect.width();
  int aYRnd = aTRect.height()*myYRnd/aRect.height();

  if ( isControlDMode() )
    thePainter.drawRoundRect(aRect,aXRnd,aYRnd);
  else if ( isFullDMode() )
  {
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
    QRect aTPRect(aRect.x()+aRect.width()/2-aCorner/3, aRect.y()-(aCorner-NODE_MARGIN)/2-aCorner/3,
		  2*aCorner/3, 2*aCorner/3);
    thePainter.drawPixmap(aTPRect,myTitlePixmap);
  }

  // draw bounding nodes' polygon if node is currently selected
  if ( isSelected() ) drawBoundary(thePainter,(isFullDMode() ? 3 : 2));
}
