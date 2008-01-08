//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "yacsconfig.h"
#include "YACSGui_Graph.h"
#include "YACSGui_Node.h"
#include "YACSGui_Module.h"
#include "YACSGui_Observer.h"

#include "YACSPrs_ElementaryNode.h"
#include "YACSPrs_BlocNode.h"
#include "YACSPrs_LoopNode.h"
#include "YACSPrs_ForEachLoopNode.h"
#include "YACSPrs_SwitchNode.h"
#include "YACSPrs_Link.h"
#include "YACSPrs_Def.h"

#include "LineConn2d_Model.h"
#include "Standard_ProgramError.hxx"

#include "QxGraph_ViewWindow.h"
#include "QxGraph_Canvas.h"
#include "QxGraph_CanvasView.h"

#include "SUIT_Session.h"
#include "SUIT_MessageBox.h"

#include <Node.hxx>
#include <ElementaryNode.hxx>
#include <ServiceNode.hxx>
#include <InlineNode.hxx>
#include <ComposedNode.hxx>
#include <Switch.hxx>
#include <Loop.hxx>
#include <ForLoop.hxx>
#include <WhileLoop.hxx>
#include <ForEachLoop.hxx>
#include <Bloc.hxx>
#include <Proc.hxx>

#include <OutputPort.hxx>
#include <OutputDataStreamPort.hxx>
#include <CalStreamPort.hxx>
#include <OutGate.hxx>
#include "utilities.h"

using namespace YACS::ENGINE;
using namespace YACS::HMI;

using namespace std;

/*!
  Constructor
*/
YACSGui_Graph::YACSGui_Graph(YACSGui_Module* theModule, 
			     QxGraph_Canvas* theCanvas, 
			     YACS::HMI::GuiContext* theCProc) :
  QxGraph_Prs(theCanvas),
  GuiObserver(),
  myModule(theModule),
  myCProc(theCProc)
{
  _mainGraph = 0;
  setDMode(YACSGui_Graph::FullId);

  if ( myCProc->getSubjectProc() ) myCProc->getSubjectProc()->attach(this);

  // Create node status observer instance
  myNodeStatusObserver = new YACSGui_Observer(this);
}

/*!
  Destructor
*/
YACSGui_Graph::~YACSGui_Graph()
{
  if ( myCProc->getSubjectProc() ) myCProc->getSubjectProc()->detach(this);

  Dispatcher* aDispatcher = Dispatcher::getDispatcher();

  DMode2ItemList aDM = getDisplayMap();
  for ( DMode2ItemList::iterator it1 = aDM.begin();
	it1 != aDM.end();
	it1++ )
  {
    for ( list<QCanvasItem*>::iterator it2 = (*it1).second.begin();
	  it2 != (*it1).second.end();
	  it2++ )
    {
      QCanvasItem* anItem = *it2;
      YACSPrs_ElementaryNode* aEN = 0;
      aEN = dynamic_cast<YACSPrs_ElementaryNode*>( *it2 );
      if ( aEN && aEN->isInBloc() )
	// remove item from the map of displayed items, because it is forbidden
	// to destroy such items in QxGraph_Prs destructor: they will be deleted
	// in the destructor of corresponding YACSPrs_BlocNode
	removeItem( anItem );
      
      // Remove status observer from dispatcher
      aDispatcher->removeObserver(myNodeStatusObserver, aEN->getEngine(), "status");
    }
  }
}

void YACSGui_Graph::select(bool isSelected)
{
  printf(">> YACSGui_Graph::select\n");
}

void YACSGui_Graph::update(YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son)
{
  printf(">> YACSGui_Graph::update\n");
  switch (event)
  {
  case ADD:
    switch (type)
    {
    case BLOC:
    case FOREACHLOOP:
    case OPTIMIZERLOOP:
    case FORLOOP:
    case WHILELOOP:
    case SWITCH:
    case PYTHONNODE:
    case PYFUNCNODE:
    case CORBANODE:
    case SALOMENODE:
    case CPPNODE:
    case SALOMEPYTHONNODE:
    case XMLNODE:
      {
	// add a node item (this = schema item)
	printf("Graph:  ADD node\n");
	createPrs( son );
      }
      break;
    default:
      break;
    }
    break;
  case REMOVE:
    switch (type)
    {
    case BLOC:
    case FOREACHLOOP:
    case OPTIMIZERLOOP:
    case FORLOOP:
    case WHILELOOP:
    case SWITCH:
    case PYTHONNODE:
    case PYFUNCNODE:
    case CORBANODE:
    case SALOMENODE:
    case CPPNODE:
    case SALOMEPYTHONNODE:
    case XMLNODE:
      {
	// remove a node item (this = schema item)
	printf("Graph:  REMOVE node\n");
	if ( SubjectNode* aNode = dynamic_cast<SubjectNode*>(son) )
	  deletePrs( aNode );
      }
      break;
    default:
      break;
    }
    break;
  case ADDLINK:
  case ADDCONTROLLINK:
    {
      // add link item (this = composed schema item)
      printf("Graph:  ADDLINK\n");
      createPrs(son);
    }
    break;
  default:
    GuiObserver::update(event, type, son);
  }
}

//! Returns the subject's Proc
/*!
 */
YACS::ENGINE::Proc* YACSGui_Graph::getProc() const
{
  return ( myCProc ? myCProc->getProc() : 0 );
}


//! Re-builds all the elements of the graph's presentation
/*!
 */
void YACSGui_Graph::update()
{
  MESSAGE("YACSGui_Graph::update 1");
  if ( !isToUpdate() )
    return;

  MESSAGE("YACSGui_Graph::update 2");

  // TODO - clean exisiting items first
  // ...

  // Iterate through Proc's nodes and create presentation items for them
  Proc* aProc = getProc();
  if (aProc)
  {
    set<Node*> aNodeSet = aProc->getAllRecursiveConstituents();

    createChildNodesPresentations( ( myCProc ? myCProc->getSubjectProc() : 0 ) );

    // Create links
    for ( set<Node*>::iterator it = aNodeSet.begin(); it != aNodeSet.end(); it++ )
    {
      YACSPrs_ElementaryNode* aNodePrs = getItem( *it );
      if ( aNodePrs )
      {
	QPtrList<YACSPrs_Port> aPorts = aNodePrs->getPortList();
	for (YACSPrs_Port* aPort = aPorts.first(); aPort; aPort = aPorts.next())
	{
	  // data and control links
	  YACSPrs_InOutPort* anIOPort = dynamic_cast<YACSPrs_InOutPort*>( aPort );
	  if ( anIOPort ) {
	    if ( getDMode() == YACSGui_Graph::FullId
		 ||
		 getDMode() == YACSGui_Graph::ControlId && anIOPort->isGate() )
	      update( anIOPort );
	    continue;
	  }
	  
	  if ( getDMode() == YACSGui_Graph::FullId )
	  {
	    // label links
	    YACSPrs_LabelPort* aLabelPort = dynamic_cast<YACSPrs_LabelPort*>( aPort );
	    if ( aLabelPort ) update( aLabelPort );
	  }
	}
      }
    }
  }

  QxGraph_Prs::update();
}

//! Arrange nodes of the graph's presentation
/*!
 */
void YACSGui_Graph::show( bool theWithArrange )
{
  QxGraph_Prs::show();

  if ( myModule && theWithArrange ) myModule->onArrangeNodes();
}

//! Corresponds the nodes positions : Full view <-> Control view.
/*!
 */
void YACSGui_Graph::viewModesConsistency( int theDModeFrom, int theDModeTo )
{
  if ( myItems.find(theDModeFrom) != myItems.end()
       &&
       myItems.find(theDModeTo) != myItems.end() )
  {
    map<Node*, YACSPrs_ElementaryNode*> aMapFrom = myItems[theDModeFrom];
    map<Node*, YACSPrs_ElementaryNode*>::iterator it = aMapFrom.begin();
    for ( ; it!=aMapFrom.end(); it++ )
    {
      YACSPrs_ElementaryNode* aFrom = (*it).second;
      YACSPrs_ElementaryNode* aTo = 0;
      
      if ( myItems[theDModeTo].find((*it).first) != myItems[theDModeTo].end() )
	aTo = myItems[theDModeTo][(*it).first];
      
      if ( aFrom && aTo )
      {
	aTo->hide();

	if ( theDModeTo == YACSGui_Graph::ControlId ) aTo->setIsCheckAreaNeeded(false);

	YACSPrs_BlocNode* aBloc = dynamic_cast<YACSPrs_BlocNode*>(aTo);
	if ( aBloc
	     &&
	     ( theDModeTo == YACSGui_Graph::ControlId
	       ||
	       aBloc->width() <= aFrom->width() && aBloc->height() <= aFrom->height() ) )
	  aBloc->resize( aFrom->width(), aFrom->height() );

	aTo->move( (int)(aFrom->x()), (int)(aFrom->y()) );

	if ( theDModeTo == YACSGui_Graph::ControlId ) aTo->setIsCheckAreaNeeded(true);

	aTo->show();
      }
    }
  }
}

//! Updates the presentation items related to given graph node.
/*!
 */
void YACSGui_Graph::update(Node* theEngine, SubjectComposedNode* theParent)
{
  MESSAGE("YACSGui_Graph::update " << theEngine->getQualifiedName());
  /// comment set for testing, uncomment this code when theEngine'll be really not null node
  if ( !theEngine )
    return;

  YACSGui_Node* aDriver = driver( theEngine );
  if ( !aDriver )
    return;

  YACSPrs_ElementaryNode* anItem = getItem( theEngine );
  bool needToAddItem = false;
  if ( !anItem )     // we need to add items, which will be created in update(...) by a driver,
    needToAddItem = true; // into display map for the current display mode
  
  // If <anItems> is empty, it is filled by a driver, if not empty - only update by a driver
  aDriver->update( theEngine, theParent, anItem );

  if (needToAddItem)
  {
    addItem( anItem ); // add item for the current display mode

    int aDMode = getDMode();
    if ( myItems.find(aDMode) == myItems.end() )
    {
      map<Node*, YACSPrs_ElementaryNode*> aMap;
      aMap.insert(std::make_pair(theEngine,anItem));
      myItems.insert(std::make_pair(aDMode,aMap));
    }
    else
    {
      if ( myItems[aDMode].find(theEngine) == myItems[aDMode].end() )
      	myItems[aDMode].insert(std::make_pair(theEngine,anItem));
    }

    //myModule->connectArrangeNodes(anItem);
  }
}

//! Updates the data and control links presentations related to given node port.
/*!
 */
void YACSGui_Graph::update( YACSPrs_InOutPort* thePort )
{
  if ( thePort->getLinks().empty() )
  {
    string aClassName = thePort->getEngine()->getNameOfTypeOfCurrentInstance();
    if ( !aClassName.compare(OutputPort::NAME) )
    { // this is an output data port => create all links going from it
      OutputPort* anOutputDFPort = dynamic_cast<OutputPort*>( thePort->getEngine() );
      if ( anOutputDFPort ) createLinksFromGivenOutPortPrs( thePort, anOutputDFPort->edSetInPort() );
    }

    if ( !aClassName.compare(OutputDataStreamPort::NAME) || !aClassName.compare(OutputCalStreamPort::NAME) )
    { // this is an output data stream port => create all links going from it
      OutputDataStreamPort* anOutputDSPort = dynamic_cast<OutputDataStreamPort*>( thePort->getEngine() );
      if ( anOutputDSPort ) createLinksFromGivenOutPortPrs( thePort, anOutputDSPort->edSetInPort() );
    }
    
    if ( !aClassName.compare(OutGate::NAME) )
    { // this is an output control port (i.e. Gate) => create all links going from it
      OutGate* anOutPort = dynamic_cast<OutGate*>( thePort->getEngine() );
      if ( anOutPort )
      {
	set<InGate *> anInPorts = anOutPort->edSetInGate();
	for(set<InGate *>::iterator iter=anInPorts.begin(); iter!=anInPorts.end(); iter++)
	{ // the pair <(*iter),anOutPort> is a link
	  YACSPrs_ElementaryNode* aToNodePrs = getItem((*iter)->getNode());
	  if ( aToNodePrs )
	  {
	    YACSPrs_InOutPort* anInPortPrs = aToNodePrs->getPortPrs( *iter );
	    YACSPrs_Link* aLink = 
	      new YACSPrs_PortLink( SUIT_Session::session()->resourceMgr(), getCanvas(), anInPortPrs, thePort );
	    aLink->show();
	    // push_back into the map<LinkName,LinkPrs*> in YACSGui_Graph
	  }
	}
      }
    }
  }
  else 
  { // update already existing port links
    list<YACSPrs_Link*> aLinks = thePort->getLinks();
    list<YACSPrs_Link*>::iterator it = aLinks.begin();
    for(; it != aLinks.end(); it++)
      ( *it )->show();
  }
}

void YACSGui_Graph::createLinksFromGivenOutPortPrs( YACSPrs_InOutPort* theOutPortPrs, std::set<YACS::ENGINE::InPort *> theInPorts )
{
  for(set<InPort *>::iterator iter=theInPorts.begin(); iter!=theInPorts.end(); iter++)
  { // the pair <(*iter),OutPort> is a link
    YACSPrs_ElementaryNode* aToNodePrs = 0;
    if ( dynamic_cast<SplitterNode*>( (*iter)->getNode() ) )
      aToNodePrs = getItem((*iter)->getNode()->getFather()); // for ForEachLoop nodes only
    else
      aToNodePrs = getItem((*iter)->getNode());
    if ( aToNodePrs )
    {
      YACSPrs_InOutPort* anInPortPrs = aToNodePrs->getPortPrs( *iter );
      YACSPrs_Link* aLink = 
	new YACSPrs_PortLink( SUIT_Session::session()->resourceMgr(), getCanvas(), anInPortPrs, theOutPortPrs );
      aLink->show();
      // push_back into the map<LinkName,LinkPrs*> in YACSGui_Graph
    }
  }
}

//! Updates the label links presentations related to given node port.
/*!
 */
void YACSGui_Graph::update( YACSPrs_LabelPort* thePort )
{
  if ( thePort->getLinks().empty() )
  {
    if ( thePort->getSlaveNode() )
    {
      YACSPrs_ElementaryNode* aSlaveNodePrs = getItem(thePort->getSlaveNode());
      // the pair <thePort,aSlaveNodePrs> is a label link
      if ( aSlaveNodePrs )
      {
	YACSPrs_Link* aLink = 
	  new YACSPrs_LabelLink( SUIT_Session::session()->resourceMgr(), getCanvas(), thePort, aSlaveNodePrs );
	aLink->show();
	// push_back into the map<LinkName,LinkPrs*> in YACSGui_Graph
      }
    }
  }
  else 
  { // update already existing label links
    list<YACSPrs_Link*> aLinks = thePort->getLinks();
    list<YACSPrs_Link*>::iterator it = aLinks.begin();
    for(; it != aLinks.end(); it++)
      ( *it )->show();
  }
}

//! Creates presentations (and at first subjects), i.e. observers of the given composed node.
/*!
 */
void YACSGui_Graph::createChildNodesPresentations( YACS::HMI::SubjectComposedNode* theParent )
{
  if ( !theParent ) return;

  if ( ComposedNode* theNode = dynamic_cast<ComposedNode*>( theParent->getNode() ) )
  {
    set<Node*> aNodeSet = theNode->edGetDirectDescendants();
    for ( set<Node*>::iterator itN = aNodeSet.begin(); itN != aNodeSet.end(); itN++ )
      update( *itN, theParent );
  }
}

void YACSGui_Graph::updateNodePrs( int theNodeId, std::string thePortName, std::string thePortValue )
{
  if ( Node::idMap.count(theNodeId) == 0 ) return;
  Node* aNode= Node::idMap[theNodeId];
  
  if ( getDMode() == YACSGui_Graph::FullId )
  {
    YACSPrs_ElementaryNode* aNodePrs = getItem(aNode);
    if ( aNodePrs )
    {
      Port* aPort = 0;
      try {
	aPort = aNode->getInPort(thePortName);
      }
      catch (YACS::Exception& ex) {
	try {
	  aPort = aNode->getOutPort(thePortName);
	}
	catch (YACS::Exception& ex) {
	  SUIT_MessageBox::warn1(myModule->getApp()->desktop(), 
				 QObject::tr("ERROR"), 
				 QString("Update %1 node presentation : ").arg(aNode->getName().c_str()) + QString(ex.what()),
				 QObject::tr("BUT_OK"));
	  return;
	}
      }
      if ( !aPort ) return;
	
      if ( YACSPrs_InOutPort* aPortPrs = aNodePrs->getPortPrs(aPort) )
	aPortPrs->updateValue(QString(thePortValue));      
    }
  }
}

void YACSGui_Graph::rebuildLinks()
{
  // Bloc I : create LineConn2d_Model object
  LineConn2d_Model* aLineModel = new LineConn2d_Model;
  aLineModel->SetTolerance( 0.01  );
  aLineModel->SetPortLength( 0. );
  aLineModel->SetSearchDepth( 2 );

  // Bloc II : iteration on nodes -> output ports -> links => fill LineConn2d_Model object with datas
  map<int,YACSPrs_Link*> aConnId2Link;
  Proc* aProc = getProc();
  if (aProc)
  {
    map<YACSPrs_ElementaryNode*, int> aNodePrs2ObjId;
    map<YACSPrs_Port*, int> aPortPrs2PortId;
    //                             1    1
    // commented because LabelPort <----> MasterPoint
    //map<YACSPrs_Hook*, int> aHookPrs2PortId;

    set<Node*> aNodeSet = aProc->getAllRecursiveConstituents();
    for ( set<Node*>::iterator itN = aNodeSet.begin(); itN != aNodeSet.end(); itN++ )
    {
      YACSPrs_ElementaryNode* aNodePrs = getItem( *itN );
      if ( aNodePrs )
      {
	addObjectToLine2dModel(aNodePrs, aLineModel, aNodePrs2ObjId);

	QPtrList<YACSPrs_Port> aPorts = aNodePrs->getPortList();
	for (YACSPrs_Port* aPort = aPorts.first(); aPort; aPort = aPorts.next())
	{
	  // data and control links
	  YACSPrs_InOutPort* anIOPort = dynamic_cast<YACSPrs_InOutPort*>( aPort );
	  if ( anIOPort && !anIOPort->isInput() )
	  { // anIOPort is a "from" port of the link
	    list<YACSPrs_Link*> aLinks = anIOPort->getLinks();
	    for(list<YACSPrs_Link*>::iterator itL = aLinks.begin(); itL != aLinks.end(); itL++)
	    {
	      int PortId1 = addPortToLine2dModel(anIOPort, aNodePrs, *itL, aLineModel, aNodePrs2ObjId, aPortPrs2PortId);

	      if ( YACSPrs_PortLink* aPortLink = dynamic_cast<YACSPrs_PortLink*>( *itL ) )
	      {
		YACSPrs_InOutPort* anIPort = aPortLink->getInputPort();

		int PortId2 = addPortToLine2dModel(anIPort, 0, *itL, aLineModel, aNodePrs2ObjId, aPortPrs2PortId);
		int ConnId = aLineModel->AddConnection( PortId1, PortId2 );
		aConnId2Link.insert(make_pair(ConnId,aPortLink));
	      }
	    }
    	  }

	  // label links
	  YACSPrs_LabelPort* aLabelPort = dynamic_cast<YACSPrs_LabelPort*>( aPort );
	  if ( aLabelPort )
	  { // aLabelPort is a "from" port of the link,
	    //the master point of the slave node can be considered as a "to" port of the link
	    list<YACSPrs_Link*> aLinks = aLabelPort->getLinks();
	    for(list<YACSPrs_Link*>::iterator itL = aLinks.begin(); itL != aLinks.end(); itL++)
	    {
	      int PortId1 = addPortToLine2dModel(aLabelPort, aNodePrs, *itL, aLineModel, aNodePrs2ObjId, aPortPrs2PortId);
	      
	      if ( YACSPrs_LabelLink* aLabelLink = dynamic_cast<YACSPrs_LabelLink*>( *itL ) )
	      {
		YACSPrs_Hook* aMPoint = aLabelLink->getSlaveNode()->getMasterPoint();
		int PortId2 = addPortToLine2dModel(aMPoint, aLabelLink->getSlaveNode(), aLabelLink, aLineModel, aNodePrs2ObjId);
		int ConnId = aLineModel->AddConnection( PortId1, PortId2 );
		aConnId2Link.insert(make_pair(ConnId,aLabelLink));
	      }
	    }
	  }
	}
      }
    }
  }

  // Bloc V : compute new links points with help of LineConn2d_Model
  if ( !aLineModel->NbObjects() )
    return;
  int nbCon;
  try
  {
    aLineModel->Compute();
    nbCon = aLineModel->NbConnections();
  }
  catch (Standard_ProgramError)
  {
    nbCon = 0;
  }
  for ( int i = 1; i <= nbCon; i++ )
  { // iterates on computed connections
    const LineConn2d_Connection& curConn = aLineModel->operator ()( i );
    // add check that no new segments computed.
    bool isSimpleLine = curConn.NbSegments() == 0;
  
    const int portId1 = curConn.Port( 0 );
    const int portId2 = curConn.Port( 1 );
    if ( portId1 == -1 || portId2 == -1 )
      continue; // should not be

    list<QPoint> aList;
    if ( !isSimpleLine )
    {
      int nbSeg = curConn.NbSegments();
      LineConn2d_Connection::SegIterator segIt = curConn.SegmentIterator();
      for( int segId = 1;  segIt.More(); segIt.Next(), segId++ )
      {
        const LineConn2d_Segment& seg = segIt.Value();
        gp_XY aXY;
	if ( segId > 1 ) {
          aXY = seg.Origin();
	  aList.push_front( QPoint( (int)(aXY.X()), (int)(aXY.Y()) ) );
	}
      }
      aConnId2Link[i]->setPoints(aList);
      aConnId2Link[i]->merge();
    }
  }

  getCanvas()->update();
}

int YACSGui_Graph::addObjectToLine2dModel(YACSPrs_ElementaryNode* theNode,
					  LineConn2d_Model*       theLineModel,
					  std::map<YACSPrs_ElementaryNode*, int>& theNodePrs2ObjId)
{
  int ObjId = -1;

  if ( !theNode ) return ObjId;

  if ( theNodePrs2ObjId.find( theNode ) != theNodePrs2ObjId.end() )
    ObjId = theNodePrs2ObjId[ theNode ];
  else
  { // Bloc III : add node to LineConn2d_Model -->
    ObjId = theLineModel->AddObject();
    LineConn2d_Object& anObj2d = theLineModel->ChangeObject( ObjId );
    
    QPointArray aPA = theNode->maxAreaPoints();
    for (QPointArray::Iterator itPA = aPA.begin(); itPA != aPA.end(); itPA++) {
      anObj2d.AddPoint( gp_XY( (*itPA).x(), (*itPA).y() ) );

      // for Bloc nodes in expanded mode add to LineConn2d_Object only the first point from area points array
      // ( Bloc nodes in expanded mode are transparent for links ).
      // In this case the check in LineConn2d_Model (checkPort(...) method) allways returns true
      // for all ports of other nodes inside Bloc node (i.e. any ports or objects are outside of the one point)
      // => intersections with Bloc nodes isn't taken into account.
      YACSPrs_BlocNode* aBNode = dynamic_cast<YACSPrs_BlocNode*>( theNode );
      if ( aBNode && aBNode->getDisplayMode() == YACSPrs_BlocNode::Expanded ) break;
    }
    
    theNodePrs2ObjId.insert( make_pair(theNode, ObjId) );
    // <--
  }

  return ObjId;
}

int YACSGui_Graph::addPortToLine2dModel(YACSPrs_Port*           thePort, 
					YACSPrs_ElementaryNode* theNode,
					YACSPrs_Link*           theLink,
					LineConn2d_Model*       theLineModel,
					std::map<YACSPrs_ElementaryNode*, int>& theNodePrs2ObjId,
					std::map<YACSPrs_Port*, int>&           thePortPrs2PortId)
{
  if ( thePortPrs2PortId.find( thePort ) != thePortPrs2PortId.end() )
    return thePortPrs2PortId[ thePort ];

  int PortId = -1;

  YACSPrs_InOutPort* anIOPort = dynamic_cast<YACSPrs_InOutPort*>( thePort );
  if ( anIOPort && !theNode)
    // take from anIOPort its node presentation
    theNode = getItem( anIOPort->getEngine()->getNode() );
  
  if ( theNode )
  {
    int ObjId = addObjectToLine2dModel(theNode, theLineModel, theNodePrs2ObjId);
    
    // Bloc IV : add port to LineConn2d_Model -->
    gp_XY aP2d( theLink->getConnectionPoint(thePort).x(), theLink->getConnectionPoint(thePort).y() );
    gp_Dir2d aDir2d( 1, 0 );
    if ( anIOPort && anIOPort->isInput() ) aDir2d.SetX( -1 );
    PortId = theLineModel->AddPoort( ObjId, aP2d, aDir2d );
    thePortPrs2PortId.insert( make_pair(thePort, PortId) );
    // <--
  }

  return PortId;
}

int YACSGui_Graph::addPortToLine2dModel(YACSPrs_Hook*           theHook,
					YACSPrs_ElementaryNode* theNode,
					YACSPrs_LabelLink*      theLink,
					LineConn2d_Model*       theLineModel,
					std::map<YACSPrs_ElementaryNode*, int>& theNodePrs2ObjId)
{
  int PortId = -1;

  if ( theNode )
  {
    int ObjId = addObjectToLine2dModel(theNode, theLineModel, theNodePrs2ObjId);

    // Bloc IV : add port to LineConn2d_Model -->
    gp_XY aP2d( theLink->getConnectionMasterPoint().x(), 
		theLink->getConnectionMasterPoint().y()+(NODEBOUNDARY_MARGIN+1)*2+LINKPOINT_SIZE/2 );
    gp_Dir2d aDir2d( 0, 1 );
    PortId = theLineModel->AddPoort( ObjId, aP2d, aDir2d );
    // <--
  }

  return PortId;
}

//! Returns the presentation driver for given graph node.
/*!
 */
YACSGui_Node* YACSGui_Graph::driver(Node* theEngine)
{
  const char* aTypeName = typeid( *theEngine ).name();
  printf("== aTypeName = %s\n",aTypeName);
  if ( myDrivers.find( aTypeName ) == myDrivers.end() )
  {
    YACSGui_Node* aDriver = 0;

    // TODO - Analyze the type of <theEngine> and create a driver instance 
    if (ServiceNode* sNode = dynamic_cast<ServiceNode*>( theEngine ) )
      {
        if (sNode->getKind() == "XML")
          aDriver = new YACSGui_InlineNode(this);
        else
          aDriver = new YACSGui_ServiceNode(this);
      }
    else if ( dynamic_cast<InlineNode*>( theEngine ) )
      aDriver = new YACSGui_InlineNode(this);
    else if ( dynamic_cast<Switch*>( theEngine ) )
      // but getNbOfCases() is a private method of Switch node => how to recognize If node type in engine?
      //if ( dynamic_cast<Switch*>( theEngine )->getNbOfCases() == 1 )
      //  aDriver = new YACSGui_IfNode(this);
      //else 
          aDriver = new YACSGui_SwitchNode(this);
    else if ( dynamic_cast<ForLoop*>( theEngine ) || dynamic_cast<WhileLoop*>( theEngine ) )
      aDriver = new YACSGui_LoopNode(this);
    else if ( dynamic_cast<ForEachLoop*>( theEngine ) )
      aDriver = new YACSGui_ForEachLoopNode(this);
    else if ( dynamic_cast<Bloc*>( theEngine ) )
      aDriver = new YACSGui_BlocNode(this);

    if ( aDriver )
      myDrivers[aTypeName] = aDriver;
  }

  return myDrivers[aTypeName];
} 

YACSPrs_ElementaryNode* YACSGui_Graph::getItem( YACS::ENGINE::Node* theEngine )
{
  YACSPrs_ElementaryNode* aNode = 0;
  int aDMode = getDMode();

  if ( myItems.find(aDMode) == myItems.end() ) return aNode;

  if ( myItems[aDMode].find(theEngine) == myItems[aDMode].end() ) return aNode;

  //if ( myItems[getDMode()].find( theEngine ) == myItems[getDMode()].end() )
  //  myItems[getDMode()][theEngine] = aNode;
  //return myItems[getDMode()][theEngine];

  return myItems[aDMode][theEngine];
}

void YACSGui_Graph::removeNode( YACS::ENGINE::Node* theNode )
{
  if ( !theNode ) return;

  map<int, map<Node*, YACSPrs_ElementaryNode*> >::iterator it = myItems.begin();
  for ( ; it!=myItems.end(); it++ )
  {
    if ( (*it).second.find(theNode) != (*it).second.end() )
    {
      if ( (*it).first != getDMode() )
	// not current view mode => remove the canvas item associated with theNode from display mode map
	removeItem( (*it).second[theNode], (*it).first );

      (*it).second.erase(theNode);
    }
  }
}

void YACSGui_Graph::getAllBlocChildren(Bloc* theNode, std::set<Node*>& theSet)
{
  if ( theNode )
  {
    set<Node*> aChildren = theNode->getChildren();
    for ( set<Node*>::iterator it = aChildren.begin(); it != aChildren.end(); it++ )
    {
      if ( dynamic_cast<Bloc*>( *it )
	   ||
	   dynamic_cast<ElementaryNode*>( *it ) ) theSet.insert( *it );
      else
      {
	ComposedNode* aCNode = dynamic_cast<ComposedNode*>( *it );
	if ( aCNode ) {
	  getAllComposedNodeChildren( aCNode, theSet );
	  theSet.insert( *it );
	}
      }
    }
  }
}

void YACSGui_Graph::getAllComposedNodeChildren(ComposedNode* theNode, std::set<Node*>& theSet)
{
  if ( theNode )
  {
    set<Node*> aDescendants = theNode->edGetDirectDescendants();
    for ( set<Node*>::iterator it = aDescendants.begin(); it != aDescendants.end(); it++ )
    {
      if ( dynamic_cast<Bloc*>( *it )
	   ||
	   dynamic_cast<ElementaryNode*>( *it ) ) theSet.insert( *it );
      else
      {
	ComposedNode* aCNode = dynamic_cast<ComposedNode*>( *it );
	if ( aCNode ) {
	  getAllComposedNodeChildren( aCNode, theSet );
	  theSet.insert( *it );
	}
      }
    }
  }
}


//! Register the status observer in the Dispatcher with the given node
/*!
 */
void YACSGui_Graph::registerStatusObserverWithNode(Node* theNode)
{
  MESSAGE("YACSGui_Graph::registerStatusObserverWithNode " << theNode->getQualifiedName());
  Dispatcher* aDispatcher = Dispatcher::getDispatcher();
  aDispatcher->addObserver(myNodeStatusObserver, theNode, "status");
}

//! Return node with id equal to theID
/*!
 */
YACS::ENGINE::Node* YACSGui_Graph::getNodeById( const int theID ) const
{
  if(YACS::ENGINE::Node::idMap.count(theID) == 0)
    return 0;
  
  YACS::ENGINE::Node* aNode= YACS::ENGINE::Node::idMap[theID];
  
  return aNode;
}

//! Return node with name equal to theID
/*!
 */
YACS::ENGINE::Node* YACSGui_Graph::getNodeByName( const std::string theName ) const
{
  if (!getProc())
    return 0;

  return getProc()->getChildByName(theName);
}

//! Create prs for the defined subject in graph
/*!
 */
void YACSGui_Graph::createPrs(YACS::HMI::Subject* theSubject)
{
  if( SubjectNode* aSubject = dynamic_cast<SubjectNode*>(theSubject) )
  {
    update( aSubject->getNode(), dynamic_cast<SubjectComposedNode*>(aSubject->getParent()) );
    show( false );

    if( SubjectSwitch* aSwitch = dynamic_cast<SubjectSwitch*>( aSubject->getParent() ) )
    {
      if ( YACSPrs_ElementaryNode* aNodePrs = getItem( aSwitch->getNode() ) )
      {
	aNodePrs->updatePorts( true );

	QPtrList<YACSPrs_Port> aPorts = aNodePrs->getPortList();
	for (YACSPrs_Port* aPort = aPorts.first(); aPort; aPort = aPorts.next())
	{
	  if ( YACSPrs_LabelPort* aLabelPort = dynamic_cast<YACSPrs_LabelPort*>( aPort ) )
	    update( aLabelPort );
	}
      }
    }
    getCanvas()->update();
  }
  // 1. In the Full mode we can create presentation for all types of links: data and control
  // 2. In the Control mode we can create presentation only for control links
  else if ( SubjectLink* aSL = dynamic_cast<SubjectLink*>(theSubject) )
  {
    if ( getDMode() == YACSGui_Graph::FullId )
    {
      Port* anOP = aSL->getSubjectOutPort()->getPort();
      Node* anON = aSL->getSubjectOutNode()->getNode();
      
      Port* anIP = aSL->getSubjectInPort()->getPort();
      Node* anIN = aSL->getSubjectInNode()->getNode();
      
      if ( !anOP || !anON || !anIP || !anIN ) return;
      
      YACSPrs_ElementaryNode* anONPrs = getItem(anON);
      YACSPrs_ElementaryNode* anINPrs = getItem(anIN);
      
      if ( !anONPrs || !anINPrs ) return;
      
      YACSPrs_InOutPort* anOPPrs = dynamic_cast<YACSPrs_InOutPort*>( anONPrs->getPortPrs( anOP ) );
      YACSPrs_InOutPort* anIPPrs = dynamic_cast<YACSPrs_InOutPort*>( anINPrs->getPortPrs( anIP ) );
      
      YACSPrs_Link* aLink = new YACSPrs_PortLink( SUIT_Session::session()->resourceMgr(), getCanvas(),
						  anIPPrs, anOPPrs );
      aLink->setZ(anOPPrs->z());
      //anOPPrs->moveBy(0,0);
      aLink->show();
      if ( getCanvas() ) getCanvas()->update();
    }
  }
  else if ( SubjectControlLink* aSCL = dynamic_cast<SubjectControlLink*>(theSubject) )
  {
    Node* anON = aSCL->getSubjectOutNode()->getNode();
    Node* anIN = aSCL->getSubjectInNode()->getNode();

    if ( !anON || !anIN ) return;

    Port* anOP = anON->getOutGate();
    Port* anIP = anIN->getInGate();
    
    if ( !anOP || !anIP ) return;

    YACSPrs_ElementaryNode* anONPrs = getItem(anON);
    YACSPrs_ElementaryNode* anINPrs = getItem(anIN);

    if ( !anONPrs || !anINPrs ) return;

    YACSPrs_InOutPort* anOPPrs = dynamic_cast<YACSPrs_InOutPort*>( anONPrs->getPortPrs( anOP ) );
    YACSPrs_InOutPort* anIPPrs = dynamic_cast<YACSPrs_InOutPort*>( anINPrs->getPortPrs( anIP ) );

    YACSPrs_Link* aLink = new YACSPrs_PortLink( SUIT_Session::session()->resourceMgr(), getCanvas(),
						anIPPrs, anOPPrs );
    aLink->setZ(anOPPrs->z());
    //anOPPrs->moveBy(0,0);
    aLink->show();
    getCanvas()->update();
  }
}

//! Delete prs with the defined subject from graph
/*!
 */
void YACSGui_Graph::deletePrs(YACS::HMI::SubjectNode* theSubject, bool removeLabelPort )
{
  if ( !theSubject ) return;

  if( YACSPrs_ElementaryNode* aPrs = getItem( theSubject->getNode() ) )
  {
    if ( dynamic_cast<YACSPrs_SwitchNode*>(aPrs) ||
	 dynamic_cast<YACSPrs_BlocNode*>(aPrs) ||
	 dynamic_cast<YACSPrs_LoopNode*>(aPrs) ||
	 dynamic_cast<YACSPrs_ForEachLoopNode*>(aPrs) )
      theSubject->detach(this);

    if ( ComposedNode* aNodeToDelete = dynamic_cast<ComposedNode*>(theSubject->getNode()) )
    {
      // remove from canvas all canvas items corresponds to the constituents of the deleted composed node
      set<Node*> aNodeSet = aNodeToDelete->getAllRecursiveConstituents();
      for ( set<Node*>::iterator it = aNodeSet.begin(); it != aNodeSet.end(); it++ )
      {
	if ( YACSPrs_ElementaryNode* aChildPrs = getItem(*it) )
	{
	  if ( dynamic_cast<YACSPrs_SwitchNode*>(aChildPrs) ||
	       dynamic_cast<YACSPrs_BlocNode*>(aChildPrs) ||
	       dynamic_cast<YACSPrs_LoopNode*>(aChildPrs) ||
	       dynamic_cast<YACSPrs_ForEachLoopNode*>(aChildPrs) )
	    aChildPrs->getSEngine()->detach(this);

	  aChildPrs->hide();

	  if ( !dynamic_cast<YACSPrs_BlocNode*>(aPrs) )
	  {
	    SubjectNode* aChildSub = aChildPrs->getSEngine();
	    // to remove body nodes properly (with all its links), when a father loop or switch node is removed
	    aChildSub->detach(aChildPrs);
	    deletePrs(aChildSub);
	    // clear the content of the property page of deleted node
	    aChildSub->update( REMOVE, 0, 0 );
	  }

	  removeItem(aChildPrs);
	}
	removeNode(*it);
      }
    }

    if( SubjectSwitch* aSwitch = dynamic_cast<SubjectSwitch*>( theSubject->getParent() ) )
      aSwitch->removeNode( theSubject );
    
    aPrs->hide();
    removeItem( aPrs );
    delete aPrs;

    if( removeLabelPort )
    {
      SubjectComposedNode* aComposedNode = dynamic_cast<SubjectComposedNode*>( theSubject->getParent() );
      if( aComposedNode && !dynamic_cast<SubjectBloc*>( aComposedNode ) )
      {
	if ( YACSPrs_ElementaryNode* aNodePrs = getItem( aComposedNode->getNode() ) )
	{
	  QPtrList<YACSPrs_Port> aPorts = aNodePrs->getPortList();
	  for (YACSPrs_Port* aPort = aPorts.first(); aPort; aPort = aPorts.next())
	  {
	    if ( YACSPrs_LabelPort* aLabelPort = dynamic_cast<YACSPrs_LabelPort*>( aPort ) )
	    {
	      if( aLabelPort->getSlaveNode() == theSubject->getNode() )
		aNodePrs->removeLabelPortPrs( aLabelPort );
	    }
	  }
	}
      }
    }
    getCanvas()->update();
  }
}
