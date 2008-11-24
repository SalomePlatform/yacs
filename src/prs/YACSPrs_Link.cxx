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

#include "YACSPrs_ElementaryNode.h"

#include "YACSPrs_Link.h"
#include "YACSPrs_Def.h"

#include "guiContext.hxx"

#include "SUIT_ResourceMgr.h"
#include "SUIT_Session.h"

#include "CAM_Application.h"

#include "InGate.hxx"
#include "InPort.hxx"
#include "OutGate.hxx"
#include "OutPort.hxx"
#include <DataStreamPort.hxx>

#include <qpopupmenu.h>
#include <math.h>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::HMI;
using namespace YACS::ENGINE;

using namespace std;

/*!
 * =========================== YACSPrs_Link ===========================
 !*/

YACSPrs_Link::YACSPrs_Link( SUIT_ResourceMgr* theMgr, QCanvas* theCanvas ):
  QObject(theCanvas),
  myMgr(theMgr),
  myCanvas(theCanvas),
  myHilighted(false),
  mySelected(false),
  mySelectedItem(0)
{
  myColor = myMgr->colorValue("YACSGui", "link_draw_color", LINKDRAW_COLOR);
  myZ = 0;
}

YACSPrs_Link::~YACSPrs_Link()
{
  DEBTRACE("YACSPrs_Link::~YACSPrs_Link");
  // remove presentation objects (points and edges)
  for (QCanvasItemList::Iterator it = myPrs.begin(); it != myPrs.end(); ++it) {
    (*it)->setCanvas(0);
    delete *it;
  }
  myPrs.clear();
  
  myPoints.clear();
}

void YACSPrs_Link::show()
{
  DEBTRACE("YACSPrs_Link::show");
  if (myPrs.isEmpty()) createPrs();

  for (QCanvasItemList::Iterator it = myPrs.begin(); it != myPrs.end(); ++it) {
    (*it)->show();
  }
}

void YACSPrs_Link::hide()
{
  DEBTRACE("YACSPrs_Link::hide");
  if (myPrs.isEmpty()) return;

  for (QCanvasItemList::Iterator it = myPrs.begin(); it != myPrs.end(); ++it) {
    (*it)->hide();
  }
}

void YACSPrs_Link::merge()
{
  DEBTRACE("YACSPrs_Link::merge");
  // save z of the link
  double aZ = ( !myPrs.isEmpty() ? myPrs.first()->z()-1 : 0 ); // the first item is a point

  // remove old presentation
  for (QCanvasItemList::Iterator it = myPrs.begin(); it != myPrs.end(); ++it) {
    (*it)->setCanvas(0);
    delete *it;
  }
  myPrs.clear();

  // display a new one
  show();

  // restore z of the link
  setZ(aZ);
}

void YACSPrs_Link::setCanvas(QCanvas* theCanvas)
{
  myCanvas = theCanvas;

  // set canvas for all prs items
  for (QCanvasItemList::Iterator it = myPrs.begin(); it != myPrs.end(); ++it)
    (*it)->setCanvas(theCanvas);
}

void YACSPrs_Link::updatePoints(QCanvasItem* thePointItem)
{
  if ( thePointItem == myPrs.first() || thePointItem == myPrs.last() ) return;
  
  YACSPrs_Point* aPoint = dynamic_cast<YACSPrs_Point*>( thePointItem );
  if ( aPoint )
  {
    list<QPoint>::iterator it;
    int id = 1;
    for ( it = myPoints.begin(); it != myPoints.end(); it++, id++ )
      if ( id == aPoint->getIndex() ) {
	(*it).setX((int)(aPoint->x()));
	(*it).setY((int)(aPoint->y()));
	break;
      }
  }
}

void YACSPrs_Link::setHilighted(bool state)
{
  myHilighted = state;
  QColor aHilightColor = myMgr->colorValue("YACSGui", "link_hilight_color", LINK_HILIGHT_COLOR);
  setColor(myHilighted ? aHilightColor : myColor);
  if (!myPrs.isEmpty()) {
    bool disp = myPrs.first()->isVisible();
    if (disp) {
      for (QCanvasItemList::Iterator it = myPrs.begin(); it != myPrs.end(); ++it) {
	(*it)->hide(); (*it)->show();
      }
      if (myCanvas) myCanvas->update();
    }
  }
}

void YACSPrs_Link::setSelected(bool state)
{
  mySelected = state;
  setColor(mySelected ? LINK_SELECT_COLOR : myColor);
  if (!myPrs.isEmpty()) {
    bool disp = myPrs.first()->isVisible();
    if (disp) {
      for (QCanvasItemList::Iterator it = myPrs.begin(); it != myPrs.end(); ++it) {
	if ( YACSPrs_Point* aPoint = dynamic_cast<YACSPrs_Point*>( *it ) ) aPoint->setSelected(state);
	else if ( YACSPrs_Edge* anEdge = dynamic_cast<YACSPrs_Edge*>( *it ) ) anEdge->setSelected(state);
	(*it)->hide(); (*it)->show();
      }
      if (myCanvas) myCanvas->update();
    }
  }
}

void YACSPrs_Link::setColor(const QColor& theColor)
{
  for (QCanvasItemList::Iterator it = myPrs.begin(); it != myPrs.end(); ++it) {
    YACSPrs_Point* aPoint = dynamic_cast<YACSPrs_Point*>( *it );
    if ( aPoint ) { //if ((*it)->rtti() == YACSPrs_Canvas::Rtti_Point) {
      aPoint->setColor(theColor);
    }
    YACSPrs_Edge* anEdge = dynamic_cast<YACSPrs_Edge*>( *it );
    if ( anEdge ) { //if ((*it)->rtti() == YACSPrs_Canvas::Rtti_Edge) {
      anEdge->setColor(theColor);
    }
  }
}

void YACSPrs_Link::setSelectedObject(QCanvasItem* theItem, const QPoint& thePoint)
{
  mySelectedItem = theItem;
  mySelectedPoint = thePoint;
}

QString YACSPrs_Link::getToolTipText() const {
  return QString();
}

void YACSPrs_Link::remove() {
  delete this;
}

void YACSPrs_Link::addPoint() {
  DEBTRACE("YACSPrs_Link::addPoint");
  if ( mySelected ) setSelected(false);

  YACSPrs_Edge* anEdge = dynamic_cast<YACSPrs_Edge*>( mySelectedItem );
  if ( anEdge ) {
    int anIndex = 1;
    QCanvasItemList::Iterator it;
    for (it = myPrs.begin(); it != myPrs.end(); ++it) {
      if ((*it) == anEdge) break;
    }
    if (it != myPrs.begin()) {
      --it;
      YACSPrs_Point* aPoint = (YACSPrs_Point*) (*it);
      anIndex = aPoint->getIndex()+1;
      if (anIndex < 1) anIndex = 1;
    }
    // TO DO : add point coordinates to link engine
    if ( myPoints.empty() )
      myPoints.push_back(mySelectedPoint);
    else {
      list<QPoint>::iterator it;
      int id = 0;
      for ( it = myPoints.begin(); it != myPoints.end(); it++, id++ )
      	if ( id == anIndex-1 ) break;
      myPoints.insert(it, mySelectedPoint);
    }

    merge();
    if (myCanvas) myCanvas->update();

    /*
    GuiContext* aContext = GuiContext::getCurrent();
    if ( aContext )
      if ( aContext->getSubjectProc() )
	aContext->getSubjectProc()->update( EDIT, DATALINK, 0 );
    */
  }
}

void YACSPrs_Link::removePoint() {
  if ( mySelected ) setSelected(false);

  YACSPrs_Point* aPoint = dynamic_cast<YACSPrs_Point*>( mySelectedItem );
  if ( aPoint ) {
    // TO DO : remove point coordinates from link engine
    QPoint aP((int)(aPoint->x()), (int)(aPoint->y()));
    myPoints.remove(aP);

    merge();
    if (myCanvas) myCanvas->update();

    /*
    GuiContext* aContext = GuiContext::getCurrent();
    if ( aContext )
      if ( aContext->getSubjectProc() )
	aContext->getSubjectProc()->update( EDIT, DATALINK, 0 );
    */
  }
}

bool YACSPrs_Link::isFirst(QCanvasEllipse* thePoint)
{
  bool aRet = false;
  if ( !myPrs.empty() 
       && 
       myPrs.first()->x() == thePoint->x() && myPrs.first()->y() == thePoint->y() ) aRet = true;
  return aRet;
}

bool YACSPrs_Link::isLast(QCanvasEllipse* thePoint)
{
  bool aRet = false;
  if ( !myPrs.empty() 
       && 
       myPrs.last()->x() == thePoint->x() && myPrs.last()->y() == thePoint->y() ) aRet = true;
  return aRet;
}

void YACSPrs_Link::setMyZ(double z)
{
  myZ = z;
}

double YACSPrs_Link::getMyZ()
{
  return myZ;
}

void YACSPrs_Link::setZ(double z)
{
  DEBTRACE("YACSPrs_Link::setZ: " << z);
  for (QCanvasItemList::Iterator it = myPrs.begin(); it != myPrs.end(); ++it)
    if ( YACSPrs_Point* pt=dynamic_cast<YACSPrs_Point*>( *it ) )
      {
        DEBTRACE("YACSPrs_Link::setZ: " << z);
        pt->setZ(z+1);
      }
    else if (YACSPrs_Edge* ed= dynamic_cast<YACSPrs_Edge*>( *it ) ) 
      {
        DEBTRACE("YACSPrs_Link::setZ: " << z);
        ed->setZ(z);
      }
}

double YACSPrs_Link::z()
{
  if ( !myPrs.empty() ) return myPrs.first()->z();
  return 0;			  
}

QPoint YACSPrs_Link::getConnectionPoint(YACSPrs_Port* thePort)
{
  DEBTRACE("YACSPrs_Link::getConnectionPoint");
  QPoint aCP;

  YACSPrs_InOutPort* anIOPort = dynamic_cast<YACSPrs_InOutPort*>( thePort );
  if ( anIOPort )
  {
    aCP = anIOPort->getConnectionPoint();
    if ( anIOPort->isGate() )
      aCP += QPoint( ( anIOPort->isInput() ? -LINKPOINT_SIZE/2-(NODEBOUNDARY_MARGIN+1) : 
		                              LINKPOINT_SIZE/2+(NODEBOUNDARY_MARGIN+1) ), 0 );
  }
  else
  {
    YACSPrs_LabelPort* aLPort = dynamic_cast<YACSPrs_LabelPort*>( thePort );
    if ( aLPort ) aCP = aLPort->getConnectionPoint();
  }

  return aCP;
}

void YACSPrs_Link::addPoint(const QPoint& thePoint, const int& theIndex)
{
  DEBTRACE("YACSPrs_Link::addPoint");
  YACSPrs_Point* aPoint;
  if (myPrs.empty()) {
    aPoint = new YACSPrs_Point(myCanvas, this, theIndex);
    aPoint->setColor(myColor);
    aPoint->move(thePoint.x(), thePoint.y());
  }
  else {
    YACSPrs_Point* aPrev = (YACSPrs_Point*) myPrs.last();
    
    YACSPrs_Edge* anEdge = new YACSPrs_Edge(myCanvas, this);
    anEdge->setColor(myColor);
    myPrs.append(anEdge);
    
    aPoint = new YACSPrs_Point(myCanvas, this, theIndex);
    aPoint->setColor(myColor);
    aPoint->move(thePoint.x(), thePoint.y());
    
    aPrev->setOutEdge(anEdge);
    aPoint->setInEdge(anEdge);
  }
  myPrs.append(aPoint);
}

void YACSPrs_Link::createPrs()
{
  QColor aHilightColor = myMgr->colorValue("YACSGui", "link_hilight_color", LINK_HILIGHT_COLOR);
  setColor(myHilighted ? aHilightColor : myColor);
}

/*!
 * =========================== YACSPrs_PortLink ===========================
 !*/

YACSPrs_PortLink::YACSPrs_PortLink( SUIT_ResourceMgr* theMgr,
				    QCanvas* theCanvas, 
				    YACSPrs_InOutPort* theInputPort, 
				    YACSPrs_InOutPort* theOutputPort ):
  GuiObserver(),
  YACSPrs_Link(theMgr, theCanvas),
  myInputPort(theInputPort),
  myOutputPort(theOutputPort)
{
  DEBTRACE("Construct YACSPrs_PortLink");
  //change color if datastream link
  if ( dynamic_cast<DataStreamPort*>(theInputPort->getEngine()))
    myColor = myMgr->colorValue("YACSGui", "stream_link_draw_color", STREAMLINKDRAW_COLOR);

  if (myInputPort) myInputPort->addLink(this);
  if (myOutputPort) myOutputPort->addLink(this);

  // attach to HMI myself
  Subject* aSub = getSubject();
  if ( aSub )
    aSub->attach( this );
}

YACSPrs_PortLink::~YACSPrs_PortLink()
{
  DEBTRACE("YACSPrs_PortLink::~YACSPrs_PortLink");
  if (myInputPort) {
    myInputPort->removeLink(this);
    myInputPort = 0;
  }
  if (myOutputPort) {
    myOutputPort->removeLink(this);
    myOutputPort = 0;
  }
}

void YACSPrs_PortLink::select( bool isSelected )
{
  DEBTRACE(">> YACSPrs_PortLink::select");
  setSelected(isSelected);
}

void YACSPrs_PortLink::update( YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son)
{
  DEBTRACE(">> YACSPrs_PortLink::update");
}

YACS::HMI::Subject* YACSPrs_PortLink::getSubject() const
{
  Subject* aSub = 0;

  if ( !myOutputPort || !myInputPort ) return aSub;

  if ( !myOutputPort->isGate() && !myInputPort->isGate() ) // data link
  {
    OutPort* anOP = dynamic_cast<OutPort*>( myOutputPort->getEngine() );
    InPort* anIP = dynamic_cast<InPort*>( myInputPort->getEngine() );
    if ( anOP && anIP )
    {
      GuiContext* aContext = GuiContext::getCurrent();
      if ( aContext )
      {
	pair<OutPort*,InPort*> aPair(anOP,anIP);
       	if ( aContext->_mapOfSubjectLink.find( aPair ) != aContext->_mapOfSubjectLink.end() )
	  aSub = aContext->_mapOfSubjectLink[ aPair ];
      }
    }
  }
  else // control link
  {
    OutGate* anOG = dynamic_cast<OutGate*>( myOutputPort->getEngine() );
    InGate* anIG = dynamic_cast<InGate*>( myInputPort->getEngine() );
    if ( anOG && anIG )
    {
      Node* anON = anOG->getNode();
      Node* anIN = anIG->getNode();
      if ( anON && anIN )
      {
	GuiContext* aContext = GuiContext::getCurrent();
	if ( aContext )
	{
	  pair<Node*,Node*> aPair(anON,anIN);
	  if ( aContext->_mapOfSubjectControlLink.find( aPair ) != aContext->_mapOfSubjectControlLink.end() )
	    aSub = aContext->_mapOfSubjectControlLink[ aPair ];
	}
      }
    }
  }

  return aSub;
}

void YACSPrs_PortLink::moveByPort(YACSPrs_Port* thePort, bool theMoveInternalLinkPoints, QRect theArea)
{
  DEBTRACE("YACSPrs_PortLink::moveByPort");
  YACSPrs_InOutPort* aPort = dynamic_cast<YACSPrs_InOutPort*>( thePort );
  if ( aPort )
  {
    QPoint p = getConnectionPoint(aPort);
    
    if ( theMoveInternalLinkPoints )
    {
      // choose and collect only those points from myPoints, which is inside of theArea
      map<int,QPoint> anIndex2MovePointMap;
      int id = 1;
      for ( list<QPoint>::iterator it = myPoints.begin(); it != myPoints.end(); it++, id++)
	if ( theArea.contains(*it,true) ) anIndex2MovePointMap.insert(make_pair(id,*it));

      if ( !anIndex2MovePointMap.empty() )
      {
	QCanvasItemList::Iterator it = myPrs.begin();
	for (it++; it != myPrs.end(); ++it) {
	  if ( YACSPrs_Point* aPoint = dynamic_cast<YACSPrs_Point*>( *it ) ) {
	    if ( anIndex2MovePointMap.find(aPoint->getIndex()) != anIndex2MovePointMap.end() ) {
	      int dx=0,dy=0;
	      if ( myInputPort && myInputPort == aPort ) {
		dx = p.x() - (int)(myPrs.first()->x());
		dy = p.y() - (int)(myPrs.first()->y());
	      }
	      if ( myOutputPort && myOutputPort == aPort ) {
		if ( getInputPort()->getNode()->isSelfMoving() ) {
		  // move internal points if moveByPort function didn't call yet for the input port of this link
		  // (because we have to move internal points only once on moving link by input or output port)
		  dx = p.x() - (int)(myPrs.last()->x());
		  dy = p.y() - (int)(myPrs.last()->y());
		}
	      }
	      aPoint->moveBy( dx, dy );
	    }
	  }
	}
      }
    }

    if ( myInputPort && myInputPort == aPort ) {
      myPrs.first()->move(p.x(), p.y());
      return;
    }
    if ( myOutputPort && myOutputPort == aPort ) {
      myPrs.last()->move(p.x(), p.y());
      return;
    }
  }
}

void YACSPrs_PortLink::moveByPort(YACSPrs_Port* thePort, int dx, int dy)
{
  DEBTRACE("YACSPrs_PortLink::moveByPort " << dx << "," << dy);
  YACSPrs_InOutPort* aPort = dynamic_cast<YACSPrs_InOutPort*>( thePort );
  if ( aPort )
  {
    if ( myInputPort && myInputPort == aPort ) {
      myPrs.first()->moveBy(dx, dy);
      return;
    }
    if ( myOutputPort && myOutputPort == aPort ) {
      myPrs.last()->moveBy(dx, dy);
      return;
    }
  }
}

QString YACSPrs_PortLink::getToolTipText() const {
  QString aText;
  if (myInputPort && myOutputPort)
    aText = myOutputPort->getEngine()->getNode()->getName() + QString(" : ") + 
      myOutputPort->getName() + QString(" => ") +
      myInputPort->getEngine()->getNode()->getName() + QString(" : ") + 
      myInputPort->getName();
  return aText;
}

void YACSPrs_PortLink::remove() {
  // for update ports' value after link deletion (only for YACSPrs_InOutPort, i.e. YACSPrs_PortLink)
  /*QString aValue;
  YACSPrs_InOutPort* aPort = dynamic_cast<YACSPrs_InOutPort*>( myInputPort );
  if ( aPort ) {
    if (aPort->getEngine()->IsParam() || myInputPort->getEngine()->IsInLine()) {
      aValue = QString(aPort->getEngine()->ToString());
    }
  }*/

  YACSPrs_Link::remove();

  // for update ports' value after link deletion (only for YACSPrs_InOutPort, i.e. YACSPrs_PortLink)
  /*if ( aPort && !aValue.isEmpty() ) { //&& GraphLevel() == 0
    aPort->setValue(aValue);
  }*/

  if (myCanvas) myCanvas->update();
}

void YACSPrs_PortLink::createPrs()
{
  DEBTRACE("YACSPrs_PortLink::createPrs");
  // create without internal points now
  if ( myInputPort ) addPoint(getConnectionPoint(myInputPort),0);

  int i = 1;
  for ( list<QPoint>::iterator it = myPoints.begin(); it != myPoints.end(); it++, i++ )
    addPoint(*it, i);

  if ( myOutputPort ) addPoint(getConnectionPoint(myOutputPort),myPoints.size()+1);

  // the first and last points is smaller than an internal points
  if ( QCanvasEllipse* aFP = dynamic_cast<QCanvasEllipse*>( getFirstPoint() ) ) aFP->setSize(LINKPOINT_SIZE/2, LINKPOINT_SIZE/2);
  if ( QCanvasEllipse* aLP = dynamic_cast<QCanvasEllipse*>( getLastPoint() ) ) aLP->setSize(LINKPOINT_SIZE/2, LINKPOINT_SIZE/2);

  YACSPrs_Link::createPrs();
}

void YACSPrs_PortLink::setHilighted(bool state)
{
  QColor aHilightColor = myMgr->colorValue("YACSGui", "hook_hilight_color", HOOK_HILIGHT_COLOR);
  QColor aDrawColor = myMgr->colorValue("YACSGui", "hook_color", HOOK_COLOR);
  QColor aColor = state?aHilightColor:aDrawColor;

  // hilight hooks
  if (getInputPort()) {
    YACSPrs_Hook* aHook = getInputPort()->GetHook();
    if (aHook) aHook->setColor(aColor);
  }
  if (getOutputPort()) {
    YACSPrs_Hook* aHook = getOutputPort()->GetHook();
    if (aHook) aHook->setColor(aColor);
  }

  // hilight link
  YACSPrs_Link::setHilighted(state);
}

void YACSPrs_PortLink::setSelected(bool state)
{
  QColor aSelectColor = myMgr->colorValue("YACSGui", "hook_select_color", HOOK_SELECT_COLOR);
  QColor aDrawColor = myMgr->colorValue("YACSGui", "hook_color", HOOK_COLOR);
  QColor aColor = state?aSelectColor:aDrawColor;

  // select hooks
  if (getInputPort()) {
    YACSPrs_Hook* aHook = getInputPort()->GetHook();
    if (aHook) {
      aHook->setSelected(state);
      aHook->setColor(aColor);
    }
  }
  if (getOutputPort()) {
    YACSPrs_Hook* aHook = getOutputPort()->GetHook();
    if (aHook) {
      aHook->setSelected(state);
      aHook->setColor(aColor);
    }
  }

  // select link
  YACSPrs_Link::setSelected(state);
}

/*!
 * =========================== YACSPrs_LabelLink ===========================
 !*/

YACSPrs_LabelLink::YACSPrs_LabelLink( SUIT_ResourceMgr* theMgr,
				      QCanvas* theCanvas,
				      YACSPrs_LabelPort* theOutputPort,
				      YACSPrs_ElementaryNode* theSlaveNode ):
  YACSPrs_Link(theMgr, theCanvas),
  myOutputPort(theOutputPort),
  mySlaveNode(theSlaveNode)
{
  DEBTRACE("Construct YACSPrs_LabelLink");
  if (myOutputPort) myOutputPort->addLink(this);
  if (mySlaveNode)
  {
    mySlaveNode->addLabelLink(this);
    mySlaveNode->setMasterPointColor(theOutputPort->Color());
  }
  myColor = theOutputPort->Color();//LABELLINKDRAW_COLOR;
}

YACSPrs_LabelLink::~YACSPrs_LabelLink()
{
  DEBTRACE("YACSPrs_LabelLink::~YACSPrs_LabelLink");
  if (myOutputPort) {
    myOutputPort->removeLink(this);
    myOutputPort = 0;
  }
  if (mySlaveNode) {
    mySlaveNode->removeLabelLink();
    mySlaveNode = 0;
  }
}

void YACSPrs_LabelLink::moveByPort(YACSPrs_Port* thePort, bool theMoveInternalLinkPoints, QRect theArea)
{
  DEBTRACE("YACSPrs_LabelLink::moveByPort");
  YACSPrs_LabelPort* aPort = dynamic_cast<YACSPrs_LabelPort*>( thePort );
  if ( aPort )
  {
    QPoint p = getConnectionPoint(aPort);
    
    if ( theMoveInternalLinkPoints )
    {
      // choose and collect only those points from myPoints, which is inside of theArea
      map<int,QPoint> anIndex2MovePointMap;
      int id = 1;
      for ( list<QPoint>::iterator it = myPoints.begin(); it != myPoints.end(); it++, id++)
	if ( theArea.contains(*it,true) ) anIndex2MovePointMap.insert(make_pair(id,*it));

      if ( !anIndex2MovePointMap.empty() )
      {
	QCanvasItemList::Iterator it = myPrs.begin();
	for (it++; it != myPrs.end(); ++it) {
	  if ( YACSPrs_Point* aPoint = dynamic_cast<YACSPrs_Point*>( *it ) ) {
	    if ( anIndex2MovePointMap.find(aPoint->getIndex()) != anIndex2MovePointMap.end() ) {
	      int dx=0,dy=0;
	      if ( myOutputPort && myOutputPort == aPort ) {
		dx = p.x() - (int)(myPrs.last()->x());
		dy = p.y() - (int)(myPrs.last()->y());
	      }
	      aPoint->moveBy( dx, dy );
	    }
	  }
	}
      }
    }

    if ( myOutputPort && myOutputPort == aPort ) {
      myPrs.last()->move(p.x(), p.y());
      return;
    }
  }
}

void YACSPrs_LabelLink::moveByPort(YACSPrs_Port* thePort, int dx, int dy)
{
  DEBTRACE("YACSPrs_LabelLink::moveByPort " << dx << "," << dy);
  YACSPrs_LabelPort* aPort = dynamic_cast<YACSPrs_LabelPort*>( thePort );
  if ( aPort )
  {
    if ( myOutputPort && myOutputPort == aPort ) {
      myPrs.last()->moveBy(dx, dy);
      return;
    }
  }
}

void YACSPrs_LabelLink::moveByNode(YACSPrs_ElementaryNode* theNode)
{
  if ( mySlaveNode && mySlaveNode == theNode ) {
    QPoint p = getConnectionMasterPoint();
    myPrs.first()->move(p.x(), p.y());
    return;
  }
}

void YACSPrs_LabelLink::moveByNode(YACSPrs_ElementaryNode* theNode, int dx, int dy)
{
  if ( mySlaveNode && mySlaveNode == theNode ) {
    myPrs.first()->moveBy(dx, dy);
    return;
  }
}

QString YACSPrs_LabelLink::getToolTipText() const {
  QString aText;
  if (myOutputPort)
    aText = /*<name of node> + QString(" : ") + */
            myOutputPort->getName() + QString(" => ") +
            QString("node \"") +
            mySlaveNode->getEngine()->getName() + QString("\"");
  return aText;
}

void YACSPrs_LabelLink::remove() {
  // TO DO : remove slave node from the switch case or loop body

  YACSPrs_Link::remove();
  if (myCanvas) myCanvas->update();
}

QPoint YACSPrs_LabelLink::getConnectionMasterPoint()
{
  if ( mySlaveNode )
    return mySlaveNode->getConnectionMasterPoint() + QPoint(0,HOOKPOINTMASTER_SIZE/4-LINKPOINT_SIZE/2);
  else
    return QPoint();
}

void YACSPrs_LabelLink::createPrs()
{
  // create without internal points now
  if ( mySlaveNode ) addPoint(getConnectionMasterPoint(),0);

  int i = 1;
  for ( list<QPoint>::iterator it = myPoints.begin(); it != myPoints.end(); it++, i++ )
    addPoint(*it, i);

  if ( myOutputPort ) addPoint(getConnectionPoint(myOutputPort),myPoints.size()+1);

  // the first and last points is smaller than an internal points
  if ( QCanvasEllipse* aFP = dynamic_cast<QCanvasEllipse*>( getFirstPoint() ) ) aFP->setSize(LINKPOINT_SIZE/2, LINKPOINT_SIZE/2);
  if ( QCanvasEllipse* aLP = dynamic_cast<QCanvasEllipse*>( getLastPoint() ) ) aLP->setSize(LINKPOINT_SIZE/2, LINKPOINT_SIZE/2);

  YACSPrs_Link::createPrs();
}

void YACSPrs_LabelLink::setHilighted(bool state)
{
  QColor aHilightColor = myMgr->colorValue("YACSGui", "hook_hilight_color", HOOK_HILIGHT_COLOR);

  YACSPrs_LabelPort* anOutputPort = getOutputPort();
  if (anOutputPort) {
    QColor aHookColor = state?aHilightColor:anOutputPort->storeColor();
    YACSPrs_Hook* aHook = anOutputPort->GetHook();
    if (aHook) aHook->setColor(aHookColor);
  }
  
  if (getSlaveNode()) {
    QColor aMasterColor = state?aHilightColor:anOutputPort->storeColor();
    getSlaveNode()->setMasterPointColor(aMasterColor);
  }
  
  YACSPrs_Link::setHilighted(state);
}

void YACSPrs_LabelLink::setSelected(bool state)
{
  QColor aSelectColor = myMgr->colorValue("YACSGui", "hook_select_color", HOOK_SELECT_COLOR);

  YACSPrs_LabelPort* anOutputPort = getOutputPort();
  if (anOutputPort) {
    QColor aHookColor = state?aSelectColor:anOutputPort->storeColor();
    YACSPrs_Hook* aHook = anOutputPort->GetHook();
    if (aHook) {
      aHook->setSelected(state);
      aHook->setColor(aHookColor);
    }
  }
  
  if (getSlaveNode()) {
    if ( getSlaveNode()->getMasterPoint() ) getSlaveNode()->getMasterPoint()->setSelected(state);
    QColor aMasterColor = state?aSelectColor:anOutputPort->storeColor();
    getSlaveNode()->setMasterPointColor(aMasterColor);
  }
  
  YACSPrs_Link::setSelected(state);
}

/*!
 * =========================== YACSPrs_Point ===========================
 !*/

YACSPrs_Point::YACSPrs_Point(QCanvas* theCanvas, 
			     YACSPrs_Link* theLink,
			     const int& theIndex):
  QCanvasEllipse(theCanvas),
  myLink(theLink), myIndex(theIndex),
  myInEdge(0), myOutEdge(0), myMoving(false), mySelected(false)
{
  setSize(LINKPOINT_SIZE, LINKPOINT_SIZE);
  setZ(-1);
}

YACSPrs_Point::~YACSPrs_Point()
{
  DEBTRACE("YACSPrs_Point::~YACSPrs_Point");
  hide();
  //This call to setCanvas must be done here because we need to call removeItem before
  //  calling the destructor of QxGraph_ActiveItem.
  //  removeItem make a dynamic_cast<QxGraph_ActiveItem*>
  setCanvas(0);
}

bool YACSPrs_Point::isMoveable()
{
  if ( !myInEdge || !myOutEdge ) return false;
  return true;
}

void YACSPrs_Point::hilight(const QPoint& theMousePos, const bool toHilight)
{
  if ( mySelected ) return;

  // process the hilighting depending on mouse position
  if (YACSPrs_Link* aLink = getLink())
    aLink->setHilighted(toHilight);
}

void YACSPrs_Point::select(const QPoint& theMousePos, const bool toSelect)
{
  // unhilight the item under the mouse cursor
  hilight(theMousePos, false);

  if (YACSPrs_Link* aLink = getLink())
  {
    aLink->setSelected(toSelect);
    
    YACSPrs_PortLink* aPLink = dynamic_cast<YACSPrs_PortLink*>(aLink);
    if ( !aPLink ) return;

    Subject* aSub = aPLink->getSubject();
    if ( aSub )
      aSub->select(toSelect);
  }
}

void YACSPrs_Point::showPopup(QWidget* theParent, QMouseEvent* theEvent, const QPoint& theMousePos)
{
  if ( !getLink() ) return;

  getLink()->setSelectedObject(this, theMousePos);

  // construct popup menu
  QPopupMenu* aPopup = new QPopupMenu(theParent);
  aPopup->insertItem(QObject::tr("MSG_DEL_POINT"), getLink(), SLOT(removePoint()));
  
  CAM_Application* anApp = ( CAM_Application* )(SUIT_Session::session()->activeApplication());
  if ( !anApp->activeModule() ) return;
  if ( anApp->activeModule()->moduleName().compare( anApp->moduleTitle( "YACSGui" ) ) !=0 ) return;

  if (theEvent->button() == RightButton) aPopup->exec(theEvent->globalPos());
}

QString YACSPrs_Point::getToolTipText(const QPoint& theMousePos, QRect& theRect) const
{
  if (YACSPrs_Link* aLink = getLink())
    {
      theRect = QRect(theMousePos.x(), theMousePos.y(), MARGIN, MARGIN);
      return aLink->getToolTipText();
    }
}

bool YACSPrs_Point::arePartsOfOtherItem(QxGraph_ActiveItem* theSecondItem)
{
  if ( YACSPrs_Point* aPoint = dynamic_cast<YACSPrs_Point*>( theSecondItem ) ) {
    if ( getLink() == aPoint->getLink() ) return true;
  }
  else if ( YACSPrs_Edge* anEdge = dynamic_cast<YACSPrs_Edge*>( theSecondItem ) ) {
    if ( getLink() == anEdge->getLink() ) return true;
  }
  return false;
}

int YACSPrs_Point::rtti() const
{
  return 0;//YACSPrs_Canvas::Rtti_Point;
}

void YACSPrs_Point::setInEdge(YACSPrs_Edge* theEdge)
{
  myInEdge = theEdge;
  theEdge->setFromPoint(this);
}

void YACSPrs_Point::setOutEdge(YACSPrs_Edge* theEdge)
{
  myOutEdge = theEdge;
  theEdge->setToPoint(this); 
}

void YACSPrs_Point::moveBy(double dx, double dy)
{
  QCanvasEllipse::moveBy(dx, dy);
  
  if ( getLink() ) getLink()->updatePoints(this);

  if (myInEdge) myInEdge->setFromPoint(this);
  if (myOutEdge) myOutEdge->setToPoint(this);
  
  if ( canvas() && getLink() && !getLink()->isEmptyPrs() ) {
    //resize canvas view if mouse is outside
    int w = (int)(x()+dx) + width() + GRAPH_MARGIN;
    int h = (int)(y()+dy) + height() + GRAPH_MARGIN;
    if (canvas()->width() > w) w = canvas()->width();
    if (canvas()->height() > h) h = canvas()->height();
    if (canvas()->width() < w || canvas()->height() < h) canvas()->resize(w, h);
  }
  //if (myIndex > 0 && isMoving()) {
  //  myLink->getEngine()->ChangeCoord(myIndex, (int)x(), (int)y());
  //}
}

void YACSPrs_Point::setColor(const QColor& theColor)
{
  setBrush(theColor);
}

/*!
 * =========================== YACSPrs_Edge ===========================
 !*/

YACSPrs_Edge::YACSPrs_Edge(QCanvas* theCanvas, 
			   YACSPrs_Link* theLink):
  QCanvasLine(theCanvas),
  myLink(theLink),
  myStartPoint(0), myEndPoint(0), mySelected(false)
{
  myArrow=new QCanvasPolygon(theCanvas);
  theCanvas->removeItem(myArrow);
  setZ(-2);
}

YACSPrs_Edge::~YACSPrs_Edge()
{
  DEBTRACE("YACSPrs_Edge::~YACSPrs_Edge");
  hide();
  //This call to setCanvas must be done here because we need to call removeItem before
  //  calling the destructor of QxGraph_ActiveItem.
  //  removeItem make a dynamic_cast<QxGraph_ActiveItem*>
  setCanvas(0);
  if(myArrow)
    {
      delete myArrow;
      myArrow=0;
    }
}

bool YACSPrs_Edge::isMoveable()
{
  if ( myStartPoint && ( myLink->isFirst(myStartPoint) || myLink->isLast(myStartPoint) )
       ||
       myEndPoint && ( myLink->isFirst(myEndPoint) || myLink->isLast(myEndPoint) ) )
    return false;
  return true;
}

void YACSPrs_Edge::hilight(const QPoint& theMousePos, const bool toHilight)
{
  if ( mySelected ) return;

  // process the hilighting depending on mouse position
  if (YACSPrs_Link* aLink = getLink())
    aLink->setHilighted(toHilight);
}

void YACSPrs_Edge::select(const QPoint& theMousePos, const bool toSelect)
{
  // unhilight the item under the mouse cursor
  hilight(theMousePos, false);

  if (YACSPrs_Link* aLink = getLink())
  {
    aLink->setSelected(toSelect);
    
    YACSPrs_PortLink* aPLink = dynamic_cast<YACSPrs_PortLink*>(aLink);
    if ( !aPLink ) return;

    Subject* aSub = aPLink->getSubject();
    if ( aSub )
      aSub->select(toSelect);
  }
}

void YACSPrs_Edge::showPopup(QWidget* theParent, QMouseEvent* theEvent, const QPoint& theMousePos)
{
  if ( !getLink() ) return;

  getLink()->setSelectedObject(this, theMousePos);

  // construct popup menu
  QPopupMenu* aPopup = new QPopupMenu(theParent);
  aPopup->insertItem(QObject::tr("MSG_ADD_POINT"), getLink(), SLOT(addPoint()));
  
  CAM_Application* anApp = ( CAM_Application* )(SUIT_Session::session()->activeApplication());
  if ( !anApp->activeModule() ) return;
  if ( anApp->activeModule()->moduleName().compare( anApp->moduleTitle( "YACSGui" ) ) !=0 ) return;

  if (theEvent->button() == RightButton) aPopup->exec(theEvent->globalPos());
}

QString YACSPrs_Edge::getToolTipText(const QPoint& theMousePos, QRect& theRect) const
{
  if (YACSPrs_Link* aLink = getLink())
    {
      theRect = boundingRect();
      return aLink->getToolTipText();
    }
}

bool YACSPrs_Edge::arePartsOfOtherItem(QxGraph_ActiveItem* theSecondItem)
{
  if ( YACSPrs_Point* aPoint = dynamic_cast<YACSPrs_Point*>( theSecondItem ) ) {
    if ( getLink() == aPoint->getLink() ) return true;
  }
  else if ( YACSPrs_Edge* anEdge = dynamic_cast<YACSPrs_Edge*>( theSecondItem ) ) {
    if ( getLink() == anEdge->getLink() ) return true;
  }
  return false;
}

int YACSPrs_Edge::rtti() const
{
  return 0;//YACSPrs_Canvas::Rtti_Edge;
}

void YACSPrs_Edge::setFromPoint(YACSPrs_Point* thePoint)
{
  DEBTRACE("YACSPrs_Edge::setFromPoint");
  myStartPoint = thePoint;
  setPoints((int)(thePoint->x()), (int)(thePoint->y()), endPoint().x(), endPoint().y());
  setArrow();
}

void YACSPrs_Edge::setToPoint(YACSPrs_Point* thePoint)
{
  DEBTRACE("YACSPrs_Edge::setToPoint");
  myEndPoint = thePoint;
  setPoints(startPoint().x(), startPoint().y(), (int)(thePoint->x()), (int)(thePoint->y()));
  setArrow();
}

void YACSPrs_Edge::setColor(const QColor& theColor)
{
  setPen(QPen(theColor, LINKEDGE_WIDTH));
  myArrow->setBrush(theColor);
}

void YACSPrs_Edge::moveBy(double dx, double dy)
{
  DEBTRACE("YACSPrs_Edge::moveBy " << dx << "," << dy);
  // for moving segment of link
  if (myStartPoint && myEndPoint) {
    myStartPoint->setMoving(true);
    myStartPoint->moveBy(dx, dy);
    
    myEndPoint->setMoving(true);
    myEndPoint->moveBy(dx,dy);
  }
  setArrow();
}

void YACSPrs_Edge::setZ(double z)
{
  DEBTRACE("YACSPrs_Edge::setZ: " << z);
  QCanvasLine::setZ(z);
  myArrow->setZ(z);
}

void YACSPrs_Edge::setVisible(bool on)
{
  DEBTRACE("YACSPrs_Edge::setVisible " << this << ","<<on);
  QCanvasLine::setVisible(on);
  myArrow->setVisible(on);
}

void YACSPrs_Edge::setArrow()
{
  DEBTRACE("YACSPrs_Edge::setArrow");
  bool aDisp = isVisible();
  if (aDisp) hide();

  int x1=startPoint().x();
  int y1=startPoint().y();
  int x2=endPoint().x();
  int y2=endPoint().y();
  DEBTRACE(x1 << "," << y1 << "," << x2 << "," << y2 );
  double d=hypot(x2-x1,y2-y1);
  double x=(x1+x2)/2.;
  double y=(y1+y2)/2.;

  //size of arrow
  double l=0.,e=0.;
  double sina=0.;
  double cosa=1.;
  if(d > 12)
    {
      l=12.;
      e=6.;
      sina=(y2-y1)/d;
      cosa=(x2-x1)/d;
    }
  DEBTRACE(x << "," << y << "," << sina << "," << cosa );

  QPointArray pa=QPointArray(3);
  pa.setPoint(0, QPoint((int)(x+l*cosa),(int)(y+l*sina)));
  pa.setPoint(1, QPoint((int)(x-e*sina),(int)(y+e*cosa)));
  pa.setPoint(2, QPoint((int)(x+e*sina),(int)(y-e*cosa)));
  myArrow->setPoints(pa);
  myArrow->setZ(z());
  if (aDisp) show();
}

void YACSPrs_Edge::setCanvas(QCanvas* theCanvas)
{
  DEBTRACE("YACSPrs_Edge::setCanvas");
  QCanvasLine::setCanvas(theCanvas);
  myArrow->setCanvas(theCanvas);
  if(theCanvas)theCanvas->removeItem(myArrow);
}
