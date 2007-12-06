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

#include "YACSPrs_ElementaryNode.h"
#include "YACSPrs_BlocNode.h"
#include "YACSPrs_LoopNode.h"
#include "YACSPrs_Link.h"
#include "YACSPrs_Def.h"

#include "QxGraph_Canvas.h"

#include <Node.hxx>
#include <Bloc.hxx>
#include <Proc.hxx>

#include <OutputPort.hxx>
#include <OutGate.hxx>
#include <CalStreamPort.hxx>

//#define WITH_DOTNEATO
#ifdef WITH_DOTNEATO
  #include <dotneato.h>
#else
  #include <gvc.h>
#endif

using namespace YACS::ENGINE;


//! Auto-arrange nodes inside a schema using Graphviz C API.
/*!
 */
int YACSGui_Graph::arrangeNodes(YACS::ENGINE::Bloc* theBloc)
{
  if ( theBloc )
  {
    // collect all Bloc children nodes of the given theBloc
    std::set<Node*> aChildren = theBloc->edGetDirectDescendants();
    for ( std::set<Node*>::iterator it = aChildren.begin(); it != aChildren.end(); it++ )
      // iterates on a Bloc children to find a Bloc with the maximum nested level
      if ( dynamic_cast<Bloc*>( *it ) )
	arrangeNodes( dynamic_cast<Bloc*>( *it ) );
    
    arrangeNodesWithinBloc( theBloc );
    
    if ( dynamic_cast<Proc*>( theBloc ) )
      getCanvas()->update();
  }
}

int YACSGui_Graph::arrangeNodesWithinBloc( Bloc* theBloc )
{
  //printf(">> YACSGui_Graph::arrangeNodes() method was called\n");

  aginit();

  // ---- Create a graphviz context
  GVC_t* aGvc = gvContext();

  // ---- Create a graph
  Agraph_t* aGraph = agopen( (char*)( theBloc ? theBloc->getName().c_str() : "aGraph" ), AGDIGRAPH );
  //printf(">> Create graph\n--------------\n");

  // ---- Initialize and set attributes for the graph
  // 1) compound attribute
  Agsym_t* anAttr;
  if ( !(anAttr = agfindattr( aGraph, "compound" )) )
    anAttr = agraphattr( aGraph, "compound", "false" );
  agxset( aGraph, anAttr->index, "true" );

  // 2) mindist (minlen) attribute
  if ( !(anAttr = agfindattr( aGraph, "mindist" )) )
    anAttr = agraphattr( aGraph, "mindist", "1.0" );
  agxset( aGraph, anAttr->index, (char*)( QString("%1").arg( (float)(8*HOOKPOINT_SIZE)/72., 0, 'g', 3 ).latin1() ) );

  // 3) ordering attribute
  if ( !(anAttr = agfindattr( aGraph, "ordering" )) )
    anAttr = agraphattr( aGraph, "ordering", "" );
  agxset( aGraph, anAttr->index, "in" );

  // 4) rankdir attribute
  if ( !(anAttr = agfindattr( aGraph, "rankdir" )) )
    anAttr = agraphattr( aGraph, "rankdir", "TB" );
  agxset( aGraph, anAttr->index, "LR" );

  // Initialize attributes for nodes
  // 1) height attribute
  if ( !(anAttr = agfindattr( aGraph->proto->n, "height" )) )
    anAttr = agnodeattr( aGraph, "height", "" );

  // 2) width attribute
  if ( !(anAttr = agfindattr( aGraph->proto->n, "width" )) )
    anAttr = agnodeattr( aGraph, "width", "" );

  // 3) shape attribute
  if ( !(anAttr = agfindattr( aGraph->proto->n, "shape" )) )
    anAttr = agnodeattr( aGraph, "shape", "" );

  createGraphvizNodes( theBloc, theBloc, aGraph );

  // ---- Initialize attributes for edges
  // 1) headport attribute
  if ( !(anAttr = agfindattr( aGraph->proto->e, "headport" )) )
    anAttr = agedgeattr( aGraph, "headport", "center" );

  // 2) len attribute
  if ( !(anAttr = agfindattr( aGraph->proto->e, "len" )) )
    anAttr = agedgeattr( aGraph, "len", 
			 (char*)( QString("%1").arg( (float)(8*HOOKPOINT_SIZE)/72., 0, 'g', 3 ).latin1() ) );
  
  // 3) lhead attribute (for blocks). TO BE IMPROVED.
  if ( !(anAttr = agfindattr( aGraph->proto->e, "lhead" )) )
    anAttr = agedgeattr( aGraph, "lhead", "" );

  // 4) ltail attribute (for blocks). TO BE IMPROVED.
  if ( !(anAttr = agfindattr( aGraph->proto->e, "ltail" )) )
    anAttr = agedgeattr( aGraph, "ltail", "" );
  
  // 5) tailport attribute
  if ( !(anAttr = agfindattr( aGraph->proto->e, "tailport" )) )
    anAttr = agedgeattr( aGraph, "tailport", "center" );

  // ---- Create edges (i.e. links)
  Agnode_t* aNode;
  for ( aNode = agfstnode( aGraph ); aNode; aNode = agnxtnode( aGraph, aNode) )
  {
    //printf(">> tail node %s\n", aNode->name);
    // lets, aNodes[i] is a tail (from) node of the link
    Agnode_t* aTailNode = aNode; //aNodes[i];
    Node* aNodeEngine = theBloc->getChildByName( std::string(aTailNode->name) ); //getNodeByName( std::string(aTailNode->name) );
    YACSPrs_ElementaryNode* aNodePrs = getItem( aNodeEngine );
    if ( aNodePrs )
    {
      QPtrList<YACSPrs_Port> aPortList = aNodePrs->getPortList();
      for (YACSPrs_Port* aPort = aPortList.first(); aPort; aPort = aPortList.next())
      {
	// data, stream and control links ... and label links (label links are considered here, but its have to be removed in the future)
	YACSPrs_InOutPort* anIOPort = dynamic_cast<YACSPrs_InOutPort*>( aPort );
	if ( anIOPort )
	{
	  // take links from output ports of the node (i.e. from tail ports)
	  std::string aClassName = anIOPort->getEngine()->getNameOfTypeOfCurrentInstance();
	  if ( !aClassName.compare(OutputPort::NAME) ||
	       !aClassName.compare(OutputDataStreamPort::NAME) ||
	       !aClassName.compare(OutputCalStreamPort::NAME) ||
	       !aClassName.compare(OutGate::NAME) )
	  {
	    std::list<YACSPrs_Link*> aLinks = anIOPort->getLinks();
	    std::list<YACSPrs_Link*>::iterator it = aLinks.begin();
	    for(; it != aLinks.end(); it++)
	      if ( YACSPrs_PortLink* aLink = dynamic_cast<YACSPrs_PortLink*>( *it ) )
	      {
		// search head node of the link
		Agnode_t* aHeadNode = agnode( aGraph, 
					      //(char*)(aLink->getInputPort()->getNode()->getEngine()->getQualifiedName().c_str())
					      (char*)(theBloc->getChildName( aLink->getInputPort()->getNode()->getEngine() ).c_str()) );
		Agedge_t* anEdge = agedge( aGraph, aTailNode, aHeadNode );
		//printf(">> Add edge : %s -> %s\n", aTailNode->name, aHeadNode->name);

		// ---- Set attributes for the concrete edge
		// 1) headport attribute
		agxset( anEdge, agfindattr(aGraph->proto->e,"headport")->index, "w" );

		// 2) tailport attribute
		agxset( anEdge, agfindattr(aGraph->proto->e,"tailport")->index, "e" );
	      }
	  }
	}
	else if ( YACSPrs_LabelPort* aLPort = dynamic_cast<YACSPrs_LabelPort*>( aPort ) )
	{
	  // label links (its have to be removed in the future)
	  if ( aLPort->getSlaveNode() )
	  {
	    Agnode_t* aHeadNode = agnode( aGraph,
					  (char*)(theBloc->getChildName( aLPort->getSlaveNode() ).c_str()) );
	    Agedge_t* anEdge = agedge( aGraph, aTailNode, aHeadNode );
	    //printf(">> Add edge : %s -> %s\n", aTailNode->name, aHeadNode->name);

	    // ---- Set attributes for the concrete edge
	    // 1) headport attribute
	    agxset( anEdge, agfindattr(aGraph->proto->e,"headport")->index, "s" );
	    
	    // 2) tailport attribute
	    agxset( anEdge, agfindattr(aGraph->proto->e,"tailport")->index, "e" );
	  }
	}
      }
    }
    //printf("\n");
  }

  //printf("--------------\n");

  // ---- Bind graph to graphviz context - currently must be done before layout
#ifdef WITH_DOTNEATO
  gvBindContext( aGvc, aGraph );

  // ---- Compute a layout
  dot_layout( aGraph );
#else
  gvLayout( aGvc, aGraph, "dot" );
#endif

  // ---- Rendering the graph : retrieve nodes positions
  // bounding box from graphviz
  //printf(">> GD_bb( aGraph ).LL.x = %d\n",GD_bb( aGraph ).LL.x);
  //printf(">> GD_bb( aGraph ).UR.y = %d\n",GD_bb( aGraph ).UR.y);
  int aBBheight = GD_bb( aGraph ).UR.y - GD_bb( aGraph ).LL.y;
  if ( !dynamic_cast<Proc*>( theBloc ) )
  {
    // we have to resize Bloc node before layouting its internal nodes
    //printf(">> resize Block %s\n",theBloc->getName().c_str());
    int aBBwidth = GD_bb( aGraph ).UR.x - GD_bb( aGraph ).LL.x;
    YACSPrs_BlocNode* aBlocNodePrs = dynamic_cast<YACSPrs_BlocNode*>( getItem( theBloc ) );
    if ( aBlocNodePrs )
    {
      int aXRight = aBlocNodePrs->width() + (aBBwidth - aBlocNodePrs->getAreaRect().width()) + BLOCNODE_MARGIN;
      int aYBottom = aBlocNodePrs->height() + (aBBheight - aBlocNodePrs->getAreaRect().height()) + BLOCNODE_MARGIN;
      aBlocNodePrs->resize( aXRight,aYBottom );
      //printf(">> aXRight = %d; aYBottom = %d\n",aXRight,aYBottom);
      //printf(">> getAreaRect().x = %d, getAreaRect().y = %d\n",
      //           aBlocNodePrs->getAreaRect().x(),aBlocNodePrs->getAreaRect().y());
      aBlocNodePrs->setZ(aBlocNodePrs->z());
    }
  }
  for ( aNode = agfstnode( aGraph ); aNode; aNode = agnxtnode( aGraph, aNode))
  {
    int aXCenter = ND_coord_i( aNode ).x;
    int aYCenter = ND_coord_i( aNode ).y;
    //printf(">> node %s graphviz center (%d,%d)\n", aNode->name, aXCenter, aYCenter);

    Node* aNodeEngine = theBloc->getChildByName( std::string(aNode->name) ); //getNodeByName( std::string(aNode->name) );
    YACSPrs_ElementaryNode* aNodePrs = getItem( aNodeEngine );
    if ( aNodePrs )
    {
      // aXCenter is the same as horizontal center of the real node presentation
      int aXLeft = aXCenter - aNodePrs->boundingRect().width()/2; //aNodePrs->maxWidth()/2 + 2*HOOKPOINT_SIZE;
      
      // now we have to recompute vertical center of the real node presentaion
      // according to aYCenter
      // NOTE: the lower left corner of the drawing is at the origin in graphviz,
      //       graphviz draws graphs from up to down
      //   -|----------------------> X
      //    |
      //   /|\ Y(gvz)
      //    |
      //    |=================-----> X'      (X', Y') - a new canvas view coordinate
      //    |                 |                         system after layouting
      //    |  bounding box   |
      //    |  from graphviz  |
      //    |                 |
      //    |----------------------> X(gvz)
      //    |
      //    |
      //   \|/ Y (Y')
      //    |
      int aYTop = aBBheight - ( aYCenter + aNodePrs->maxHeight()/2/*aNodePrs->boundingRect().height()/2*/ );
      //printf(">> aXLeft = %d, aYTop = %d\n",aXLeft,aYTop);

      if ( !dynamic_cast<Proc*>( theBloc ) )
      {
	YACSPrs_BlocNode* aBlocNodePrs = dynamic_cast<YACSPrs_BlocNode*>( getItem( theBloc ) );
	if ( aBlocNodePrs )
	{
	  aXLeft += aBlocNodePrs->getAreaRect().x() + BLOCNODE_MARGIN/2;// + 2*HOOKPOINT_SIZE;
	  aYTop += aBlocNodePrs->getAreaRect().y() + BLOCNODE_MARGIN/2;
	  //printf(">> +Area : aXLeft = %d, aYTop = %d\n",aXLeft,aYTop);
	}
      }

      if ( dynamic_cast<YACSPrs_LoopNode*>( aNodePrs ) )
      {
	aXLeft += 3*TITLE_HEIGHT/2;
	aYTop += 3*TITLE_HEIGHT/2;
      }

      // move presentation of the node
      //printf(">> aNodePrs->oldX = %f, aNodePrs->oldY = %f\n",aNodePrs->x(),aNodePrs->y());
      //aNodePrs->setX( aXLeft ); aNodePrs->setY( aYTop );      
      aNodePrs->move( aXLeft, aYTop );
      //printf(">> height = %d, width = %d\n",aNodePrs->height(),aNodePrs->width());

      // print a new node's position for checking
      //char buf[50];
      //sprintf(buf, "left=%d,top=%d", aXLeft, aYTop);
      //printf(">> node %s at position (%s)\n\n", aNode->name, buf);
    }
  }

  // ---- Delete layout
#ifdef WITH_DOTNEATO
  dot_cleanup( aGraph );
#else
  gvFreeLayout( aGvc, aGraph );
#endif

  // ---- Free graph structures
  agclose( aGraph );

  // ---- Free context and return number of errors
  int aRetVal = 0;//gvFreeContext( aGvc );

  //getCanvas()->update();

  //printf(">> YACSGui_Graph::arrangeNodes() method was finished\n");

  return aRetVal;
}

void YACSGui_Graph::createGraphvizNodes( Bloc* theBloc, ComposedNode* theFather, Agraph_t* theGraph )
{
  // ---- Create nodes
  // NOTE: it is a test code for graphs without block nodes (clusters) only. TO BE IMPROVED.
  if ( theFather )
  {
    std::set<Node*> aNodeSet = theFather->edGetDirectDescendants(); //getChildren(); //getAllRecursiveConstituents();
    for ( std::set<Node*>::iterator it = aNodeSet.begin(); it != aNodeSet.end(); it++ )
    {
      // create graphviz objects for all not Block nodes and nodes not including into the Block
      Agnode_t* aNode = agnode( theGraph, (char*)(theBloc->getChildName( *it ).c_str()) );
      //printf(">> Add node %s\n", aNode->name);
      
      // ---- Set attributes for the concrete node
      // 1) height attribute
      agxset( aNode, agfindattr(theGraph->proto->n,"height")->index,
	      (char*)( QString("%1").arg( (float)(getItem( *it )->maxHeight())/72., 0, 'g', 3 ).latin1() ) );
      
      // 2) width attribute
      agxset( aNode, agfindattr(theGraph->proto->n,"width")->index,
	      (char*)( QString("%1").arg( (float)(getItem( *it )->maxWidth())/72., 0, 'g', 3 ).latin1() ) );
      
      //printf(">> height = %d, width = %d\n\n",getItem( *it )->maxHeight(),getItem( *it )->maxWidth());
      
      // 3) shape attribute
      agxset( aNode, agfindattr(theGraph->proto->n,"shape")->index, "box" );
      
      // recurtion
      if ( !dynamic_cast<Bloc*>( *it ) )
	createGraphvizNodes( theBloc, dynamic_cast<ComposedNode*>( *it ), theGraph );

      /*if ( dynamic_cast<Bloc*>( *it ) )
      {
	// create graphviz objects for all Block nodes
	Agraph_t* aCluster = agsubg( theGraph, (char*)((std::string("cluster")+getProc()->getChildName( *it )).c_str()) );
	printf(">> Add Block node %s\n", aCluster->name);

	// ---- Set attributes for the concrete node
	Agsym_t* anAttr;
	// 1) peripheries attribute
	if ( !(anAttr = agfindattr( aCluster, "peripheries" )) )
	  anAttr = agraphattr( theGraph, "peripheries", "1" );
	agxset( aCluster, anAttr->index, "4" );

	// recurtion
	createGraphvizNodes( dynamic_cast<ComposedNode*>( *it ), aCluster );
      }*/
    }
  }
}
