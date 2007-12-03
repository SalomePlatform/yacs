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

#include "YACSGui_Graph.h"
#include "YACSGui_Node.h"
#include "YACSGui_Module.h"
#include "YACSGui_Observer.h"

#include "YACSPrs_ElementaryNode.h"
#include "YACSPrs_BlocNode.h"
#include "YACSPrs_LoopNode.h"
#include "YACSPrs_Link.h"
#include "YACSPrs_Def.h"

#include "LineConn2d_Model.h"
#include "Standard_ProgramError.hxx"

#include "QxGraph_ViewWindow.h"
#include "QxGraph_Canvas.h"
#include "QxGraph_CanvasView.h"

#include "SUIT_Session.h"

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

/*!
  Constructor
*/
YACSGui_Graph::YACSGui_Graph(YACSGui_Module* theModule, 
			     QxGraph_Canvas* theCanvas, 
			     YACS::HMI::GuiContext* theCProc) :
  QxGraph_Prs(theCanvas),
  myModule(theModule),
  myCProc(theCProc)
{
  // Create node status observer instance
  myNodeStatusObserver = new YACSGui_Observer(this);
}

/*!
  Destructor
*/
YACSGui_Graph::~YACSGui_Graph()
{
  Dispatcher* aDispatcher = Dispatcher::getDispatcher();

  DMode2ItemList aDM = getDisplayMap();
  for ( DMode2ItemList::iterator it1 = aDM.begin();
	it1 != aDM.end();
	it1++ )
  {
    for ( std::list<QCanvasItem*>::iterator it2 = (*it1).second.begin();
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

  // maximum width and height of nodes presentation (for table arrangement first level nodes)
  int aMaxNodeWidth=0, aMaxNodeHeight=0;
  bool aNeedToArrange = false;

  // Iterate through Proc's nodes and create presentation items for them
  Proc* aProc = getProc();
  if (aProc)
  {
    std::set<Node*> aNodeSet = aProc->getAllRecursiveConstituents();

    if ( !getItem( *(aNodeSet.begin()) ) ) aNeedToArrange = true;

    createChildNodesPresentations( ( myCProc ? myCProc->getSubjectProc() : 0 ) );

    // TODO : to be removed in the future (when the arrange nodes algorithm is included) -->
    for ( std::set<Node*>::iterator it = aNodeSet.begin(); it != aNodeSet.end(); it++ )
    {
      YACSPrs_ElementaryNode* aNodePrs = getItem( *it );
      if ( aNodePrs ) {
	if ( aMaxNodeWidth < aNodePrs->maxWidth() ) aMaxNodeWidth = aNodePrs->maxWidth();
	if ( aMaxNodeHeight < aNodePrs->maxHeight() ) aMaxNodeHeight = aNodePrs->maxHeight();
      }
    }
    // <--

    // Create links
    for ( std::set<Node*>::iterator it = aNodeSet.begin(); it != aNodeSet.end(); it++ )
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
	    update( anIOPort );
	    continue;
	  }
	  // label links
	  YACSPrs_LabelPort* aLabelPort = dynamic_cast<YACSPrs_LabelPort*>( aPort );
	  if ( aLabelPort ) update( aLabelPort );
	}
      }
    }

    if ( aNeedToArrange )
    {
      // table arrangement of the first level nodes
      aNodeSet = aProc->edGetDirectDescendants();
      int aColNum, aRowNum; aColNum = aRowNum = (int)sqrt((double)aNodeSet.size());
      if ( aNodeSet.size() - aColNum*aRowNum > 0 ) aRowNum++;
      int i=0,j=0;
      int aMargin = 50;
      for ( std::set<Node*>::iterator it = aNodeSet.begin(); it != aNodeSet.end(); it++ )
      {
	YACSPrs_ElementaryNode* aNodePrs = getItem( *it );
	if ( aNodePrs )
	{
	  if ( j == aColNum) { j = 0; i++; }
	  aNodePrs->move( aNodePrs->x() + i*(aMaxNodeWidth+aMargin), aNodePrs->y() + j*(aMaxNodeHeight+aMargin) );
	  j++;
	}
      }
    }
    
  }

  QxGraph_Prs::update();
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
    myItems[theEngine] = anItem;

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
    std::string aClassName = thePort->getEngine()->getNameOfTypeOfCurrentInstance();
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
	std::set<InGate *> anInPorts = anOutPort->edSetInGate();
	for(std::set<InGate *>::iterator iter=anInPorts.begin(); iter!=anInPorts.end(); iter++)
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
    std::list<YACSPrs_Link*> aLinks = thePort->getLinks();
    std::list<YACSPrs_Link*>::iterator it = aLinks.begin();
    for(; it != aLinks.end(); it++)
      ( *it )->show();
  }
}

void YACSGui_Graph::createLinksFromGivenOutPortPrs( YACSPrs_InOutPort* theOutPortPrs, std::set<YACS::ENGINE::InPort *> theInPorts )
{
  for(std::set<InPort *>::iterator iter=theInPorts.begin(); iter!=theInPorts.end(); iter++)
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
    std::list<YACSPrs_Link*> aLinks = thePort->getLinks();
    std::list<YACSPrs_Link*>::iterator it = aLinks.begin();
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
    std::set<Node*> aNodeSet = theNode->edGetDirectDescendants();
    for ( std::set<Node*>::iterator itN = aNodeSet.begin(); itN != aNodeSet.end(); itN++ )
      update( *itN, theParent );
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
  std::map<int,YACSPrs_Link*> aConnId2Link;
  Proc* aProc = getProc();
  if (aProc)
  {
    std::map<YACSPrs_ElementaryNode*, int> aNodePrs2ObjId;
    std::map<YACSPrs_Port*, int> aPortPrs2PortId;
    //                             1    1
    // commented because LabelPort <----> MasterPoint
    //std::map<YACSPrs_Hook*, int> aHookPrs2PortId;

    std::set<Node*> aNodeSet = aProc->getAllRecursiveConstituents();
    for ( std::set<Node*>::iterator itN = aNodeSet.begin(); itN != aNodeSet.end(); itN++ )
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
	    std::list<YACSPrs_Link*> aLinks = anIOPort->getLinks();
	    for(std::list<YACSPrs_Link*>::iterator itL = aLinks.begin(); itL != aLinks.end(); itL++)
	    {
	      int PortId1 = addPortToLine2dModel(anIOPort, aNodePrs, *itL, aLineModel, aNodePrs2ObjId, aPortPrs2PortId);

	      if ( YACSPrs_PortLink* aPortLink = dynamic_cast<YACSPrs_PortLink*>( *itL ) )
	      {
		YACSPrs_InOutPort* anIPort = aPortLink->getInputPort();

		int PortId2 = addPortToLine2dModel(anIPort, 0, *itL, aLineModel, aNodePrs2ObjId, aPortPrs2PortId);
		int ConnId = aLineModel->AddConnection( PortId1, PortId2 );
		aConnId2Link.insert(std::make_pair(ConnId,aPortLink));
	      }
	    }
    	  }

	  // label links
	  YACSPrs_LabelPort* aLabelPort = dynamic_cast<YACSPrs_LabelPort*>( aPort );
	  if ( aLabelPort )
	  { // aLabelPort is a "from" port of the link,
	    //the master point of the slave node can be considered as a "to" port of the link
	    std::list<YACSPrs_Link*> aLinks = aLabelPort->getLinks();
	    for(std::list<YACSPrs_Link*>::iterator itL = aLinks.begin(); itL != aLinks.end(); itL++)
	    {
	      int PortId1 = addPortToLine2dModel(aLabelPort, aNodePrs, *itL, aLineModel, aNodePrs2ObjId, aPortPrs2PortId);
	      
	      if ( YACSPrs_LabelLink* aLabelLink = dynamic_cast<YACSPrs_LabelLink*>( *itL ) )
	      {
		YACSPrs_Hook* aMPoint = aLabelLink->getSlaveNode()->getMasterPoint();
		int PortId2 = addPortToLine2dModel(aMPoint, aLabelLink->getSlaveNode(), aLabelLink, aLineModel, aNodePrs2ObjId);
		int ConnId = aLineModel->AddConnection( PortId1, PortId2 );
		aConnId2Link.insert(std::make_pair(ConnId,aLabelLink));
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

    std::list<QPoint> aList;
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
    
    theNodePrs2ObjId.insert( std::make_pair(theNode, ObjId) );
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
    thePortPrs2PortId.insert( std::make_pair(thePort, PortId) );
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
  if ( myItems.find( theEngine ) == myItems.end() )
    myItems[theEngine] = aNode;
  return myItems[theEngine];
}

void YACSGui_Graph::getAllBlocChildren(Bloc* theNode, std::set<Node*>& theSet)
{
  if ( theNode )
  {
    std::set<Node*> aChildren = theNode->getChildren();
    for ( std::set<Node*>::iterator it = aChildren.begin(); it != aChildren.end(); it++ )
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
    std::set<Node*> aDescendants = theNode->edGetDirectDescendants();
    for ( std::set<Node*>::iterator it = aDescendants.begin(); it != aDescendants.end(); it++ )
    {
      if ( dynamic_cast<Bloc*>( *it )
	   ||
	   dynamic_cast<ElementaryNode*>( *it ) ) theSet.insert( *it );
      else
      {
	ComposedNode* aCNode = dynamic_cast<ComposedNode*>( *it );
	if ( aCNode ) getAllComposedNodeChildren( aCNode, theSet );
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
