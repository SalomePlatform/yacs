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

#ifdef HAVE_DOTNEATO_H
  #include <dotneato.h>
#else
  #include <gvc.h>
#endif

#include "utilities.h"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;

using namespace std;

/*! Definition in dot per inch for the input:
 *  size of elementary nodes are in pixel in Qt, and given in inches to graphviz
 */
#define DPI 72.

/*! Space to reserve (pixels) on top and below elementary nodes.
 *     Corresponds to the size of the lower and upper banner of bloc.
 */
#define BLOCB 30

static GVC_t* aGvc = 0;

//! Auto-arrange nodes inside a schema using Graphviz C API.
/*!
 */
int YACSGui_Graph::arrangeNodesAlgo( YACS::ENGINE::Bloc* theBloc )
{
  DEBTRACE("YACSGui_Graph::arrangeNodesAlgo() " << theBloc->getName());
  _savedControlLinks.clear();
  _maxdep = 0;
  _format="%1";  // ---  standard format to convert float  ex: 9.81
  //_format="%L1"; // --- localized format to convert float  ex: 9,81

  // ---- Create a graphviz context

  if(!aGvc)
    {
      aginit();
      aGvc = gvContext();
    }

  // ---- Create a graph

  _mainGraph = agopen( (char*)( theBloc ? theBloc->getName().c_str() : "mainGraph" ), AGDIGRAPH );

  // ---- Initialize and set attributes for the graph
  
  Agsym_t* anAttr;
  if ( !(anAttr = agfindattr(_mainGraph, "compound")))
    anAttr = agraphattr(_mainGraph, "compound", "false");
  agxset(_mainGraph, anAttr->index, "true");

  if ( !(anAttr = agfindattr(_mainGraph, "rankdir")))
    anAttr = agraphattr(_mainGraph, "rankdir", "TB");
  agxset(_mainGraph, anAttr->index, "LR");

//   if ( !(anAttr = agfindattr(_mainGraph, "ordering")))
//     anAttr = agraphattr(_mainGraph, "ordering", "" );
//   agxset(_mainGraph, anAttr->index, "in" );
  
  if ( !(anAttr = agfindattr(_mainGraph, "dpi")))
    anAttr = agraphattr(_mainGraph, "dpi", "72");
  agxset(_mainGraph, anAttr->index, "72"); // --- must be coherent with #define DPI

  // --- label is used to reserve place for bloc banners (adjust size with font !)

  if ( !(anAttr = agfindattr(_mainGraph, "label")))
    anAttr = agraphattr(_mainGraph, "label", "label");
  agxset(_mainGraph, anAttr->index, "myLabel");

  if ( !(anAttr = agfindattr(_mainGraph, "labelloc")))
    anAttr = agraphattr(_mainGraph, "labelloc", "top");
  agxset(_mainGraph, anAttr->index, "top");

  if ( !(anAttr = agfindattr(_mainGraph, "fontsize")))
    anAttr = agraphattr(_mainGraph, "fontsize", "24");
  agxset(_mainGraph, anAttr->index, "24");

  if ( !(anAttr = agfindattr(_mainGraph, "splines")))
    anAttr = agraphattr(_mainGraph, "splines", "");
  agxset(_mainGraph, anAttr->index, "");

  // --- Initialize attributes for nodes

  if ( !(anAttr = agfindattr( _mainGraph->proto->n, "height")))
    anAttr = agnodeattr(_mainGraph, "height", "" );

  if ( !(anAttr = agfindattr( _mainGraph->proto->n, "width")))
    anAttr = agnodeattr(_mainGraph, "width", "" );

  if ( !(anAttr = agfindattr( _mainGraph->proto->n, "shape")))
    anAttr = agnodeattr(_mainGraph, "shape", "" );

  if ( !(anAttr = agfindattr( _mainGraph->proto->n, "fixedsize")))
    anAttr = agnodeattr(_mainGraph, "fixedsize", "false" );

  int curdep = -1;

  // ---- Bind graph to graphviz context - must be done before layout
  // ---- Compute a layout
  try
    {
      arrangeNodes(theBloc, _mainGraph, curdep);
      createGraphvizNodes(theBloc, _mainGraph);
      DEBTRACE("end of graphviz input");
#ifdef _DEVDEBUG_
      agwrite(_mainGraph, stderr);
#endif
#ifdef HAVE_DOTNEATO_H
      gvBindContext(aGvc, _mainGraph);
      dot_layout(_mainGraph);
#else
      //DEBTRACE("external render for test");
      //gvRenderFilename(aGvc, _mainGraph, "dot", "graph1.dot");
      DEBTRACE("compute layout");
      gvLayout(aGvc, _mainGraph, "dot");
      DEBTRACE("external render for test");
#ifdef _DEVDEBUG_
      gvRenderFilename(aGvc, _mainGraph, "dot", "graph2.dot");
#endif
#endif
   }
  catch (std::exception &e)
    {
      DEBTRACE("Exception Graphviz layout: " << e.what());
      return 1;
    }
  catch (...)
    {
      DEBTRACE("Unknown Exception Graphviz layout ");
      return 1;
    }
  DEBTRACE("start of display");
  // ---- layout Canvas nodes recursively

  curdep = -1;
  arrangeCanvasNodes(theBloc, _mainGraph, curdep);

  DEBTRACE("clean up graphviz");
  // ---- Delete layout

#ifdef HAVE_DOTNEATO_H
  dot_cleanup(_mainGraph);
#else
  gvFreeLayout(aGvc, _mainGraph);
#endif

  // ---- Free graph structures

  agclose(_mainGraph) ;

  // ---- Free context and return number of errors

#ifndef HAVE_DOTNEATO_H
  //gvFreeContext( aGvc );
#endif
  
  if ( dynamic_cast<Proc*>( theBloc ) )
    getCanvas()->update();

  return 0;
}


void YACSGui_Graph::arrangeCanvasNodes(YACS::ENGINE::ComposedNode* theBloc, Agraph_t* aSubGraph, int dep)
{
  DEBTRACE("arrangeCanvasNodes, bloc: " << aSubGraph->name);
  int curdep = dep +1;
  int offX = 0;
  int offY = 0;
  DEBTRACE("dep="<< curdep << " maxdep= " << _maxdep << " offY=" << offY);
  
  
  // ---- Rendering the graph : retrieve nodes positions
  
  // --- bloc bounding box from graphviz

  int llx = GD_bb( aSubGraph ).LL.x;
  int lly = GD_bb( aSubGraph ).LL.y;
  int urx = GD_bb( aSubGraph ).UR.x;
  int ury = GD_bb( aSubGraph ).UR.y;
  DEBTRACE("boundingBox " << aSubGraph->name << " (" << llx << "," << lly << ") (" << urx << "," << ury << ")");
  int bbHeight = (ury -lly);
  int bbWhidth = (urx -llx);
  if (curdep == 0) _bottom = bbHeight;
  DEBTRACE(_bottom);

  if (!dynamic_cast<Proc*>(theBloc))
    {
      YACSPrs_BlocNode* aBlocNodePrs = dynamic_cast<YACSPrs_BlocNode*>(getItem(theBloc));
      if (aBlocNodePrs)
        {
          DEBTRACE("arrangeCanvasNodes " << llx + offX<<","<<_bottom -ury<<","<<bbWhidth<<","<<bbHeight);
          aBlocNodePrs->setIsCheckAreaNeeded(false);
          aBlocNodePrs->resize(bbWhidth,bbHeight);
	  aBlocNodePrs->move(llx + offX,_bottom -ury);
          //aBlocNodePrs->setX(llx + offX);
          //aBlocNodePrs->setY(_bottom -ury);
	  aBlocNodePrs->setZ(aBlocNodePrs->z());
          aBlocNodePrs->setIsCheckAreaNeeded(true);
        }
    }
      
  // --- subgraphs
  
  list<Node*> children = theBloc->edGetDirectDescendants();
  for ( list<Node*>::iterator it = children.begin(); it != children.end(); it++ )
    {
      if (ComposedNode* childBloc = dynamic_cast<ComposedNode*>(*it))
        {
          DEBTRACE("arrangeCanvasNodes ComposedNode" );
          string clusterName="cluster_";
          if (dynamic_cast<Proc*>(childBloc)) clusterName += childBloc->getName();
          else clusterName += getProc()->getChildName(childBloc);
          DEBTRACE(clusterName);
          Agraph_t* childGraph = agsubg(aSubGraph,(char*)clusterName.c_str());
          assert(childGraph);
          arrangeCanvasNodes(childBloc, childGraph, curdep);
        }
      if (!dynamic_cast<Bloc*>(*it))
        {
          DEBTRACE("arrangeCanvasNodes not a Bloc" );
          string nodeName = getProc()->getChildName(*it);
          Agnode_t* aNode = agnode(aSubGraph,(char*)nodeName.c_str());
          int aXCenter = ND_coord_i(aNode).x;
          int aYCenter = ND_coord_i(aNode).y;
          YACSPrs_ElementaryNode* aNodePrs = getItem(*it);
          if ( aNodePrs )
            {
              int aXLeft = aXCenter - aNodePrs->maxWidth()/2+2*HOOKPOINT_SIZE;
              int aYTop  = aYCenter + aNodePrs->maxHeight()/2;

	      if ( dynamic_cast<YACSPrs_LoopNode*>( aNodePrs ) && getDMode() == YACSGui_Graph::FullId )
		{
		  aXLeft += 3*TITLE_HEIGHT/2;
		  aYTop += 3*TITLE_HEIGHT/2;
		}
	      
              DEBTRACE("Node: " << nodeName << " aXCenter = " << aXCenter << " aYCenter = " << aYCenter);
              DEBTRACE("Node: " << nodeName << " aXLeft = " << aXLeft << " aYTop = " << aYTop);
              aNodePrs->setIsCheckAreaNeeded(false);
	      aNodePrs->move(aXLeft + offX,_bottom -(aYTop + offY));
              //aNodePrs->setX(aXLeft + offX);
              //aNodePrs->setY(_bottom -(aYTop + offY));
              aNodePrs->setIsCheckAreaNeeded(true);
            }
        }
    }
}




int YACSGui_Graph::arrangeNodes( YACS::ENGINE::ComposedNode* theBloc, Agraph_t* aSubGraph, int dep)
{
  DEBTRACE("YACSGui_Graph::arrangeNodes() " << theBloc->getName());

  int curdep = dep +1;
  if (curdep > _maxdep)
    {
      _maxdep = curdep;
      DEBTRACE("Max level of nested blocs: "<< _maxdep);
    }
  int aRetVal = 0;

  // --- collect all Bloc children nodes of the given theBloc

  list<Node*> children = theBloc->edGetDirectDescendants();
  for ( list<Node*>::iterator it = children.begin(); it != children.end(); it++ )
    {
      // iterates on a Bloc children to find a Bloc with the maximum nested level
      if ( ComposedNode* childBloc = dynamic_cast<ComposedNode*>( *it ) )
        {
          string clusterName="cluster_";
          if (dynamic_cast<Proc*>(childBloc)) clusterName += childBloc->getName();
          else clusterName += getProc()->getChildName(childBloc);

          // --- create a subgraph for each bloc within a dummy cluster
          //     top and bottom labels reserve space for top and bottom of the bloc widget

          string topname = clusterName + "_top";
          Agraph_t* aClustertop = agsubg( aSubGraph, (char*)(topname.c_str()));
          agset(aClustertop, "labelloc", "bottom");
 	  agset(aClustertop, "rankdir", "LR");
// 	  agset(aClustertop, "ordering", "in");

	  string bottomname = clusterName + "_bottom";
          Agraph_t* aClusterbottom = agsubg( aClustertop, (char*)(bottomname.c_str()));
          agset(aClusterbottom, "labelloc", "top");
 	  agset(aClusterbottom, "rankdir", "LR");
// 	  agset(aClusterbottom, "ordering", "in");

	  Agraph_t* aCluster = agsubg(aClusterbottom , (char*)(clusterName.c_str()));
          agset(aCluster, "labelloc", "");
 	  agset(aCluster, "rankdir", "LR");
// 	  agset(aCluster, "ordering", "in");
          DEBTRACE("Add Block node " << aCluster->name);

          aRetVal = arrangeNodes(childBloc, aCluster, curdep);
          createGraphvizNodes(childBloc, aCluster);
        }
    }
  return aRetVal;
}


void YACSGui_Graph::createGraphvizNodes( ComposedNode* theBloc, Agraph_t* aSubGraph )
{
  DEBTRACE("YACSGui_Graph::createGraphvizNodes() " << theBloc->getName());

  // ---- Create nodes (+ small dummy node for the bloc)

  if ( !dynamic_cast<Proc*>(theBloc) && !getItem(theBloc) ) return;

  Agnode_t* dummyNode;
  if (dynamic_cast<Bloc*>(theBloc))
    {
      string dummyName = "dummy_";
      if (dynamic_cast<Proc*>(theBloc)) dummyName += theBloc->getName();
      else dummyName += getProc()->getChildName(theBloc);
      DEBTRACE(dummyName);
      dummyNode = agnode( aSubGraph, (char*)(dummyName.c_str()) );
      {
        int nh = 10;
        int nw = 10;
        double lh = nh/DPI;
        double lw = nw/DPI;
        stringstream height, width;
        height << lh; width << lw;
        agxset( dummyNode, agfindattr(_mainGraph->proto->n,"height")->index, (char*)(height.str().c_str()));
        agxset( dummyNode, agfindattr(_mainGraph->proto->n,"width")->index, (char*)(width.str().c_str()));
        agxset( dummyNode, agfindattr(_mainGraph->proto->n,"shape")->index, "box" );
        agxset( dummyNode, agfindattr(_mainGraph->proto->n,"fixedsize")->index, "true" );
	
	Agsym_t* anAttr;
	if ( !(anAttr = agfindattr(_mainGraph->proto->n,"label")) )
	  anAttr = agnodeattr(_mainGraph, "label", "label");
        agxset( dummyNode, anAttr->index, "" );
      }
    }
  else
    {
      dummyNode = agnode( aSubGraph, (char*)(getProc()->getChildName(theBloc).c_str()) );
      DEBTRACE("Add node in subgraph " << aSubGraph->name << ": " << dummyNode->name);
      
      // ---- Set attributes for the concrete node
      int nh = getItem(theBloc)->maxHeight()+2*BLOCB;
      int nw = getItem(theBloc)->maxWidth();
      double lh = nh/DPI;
      double lw = nw/DPI;
      // stringstream height, width;
      // height << lh; width << lw;
      // DEBTRACE(dummyNode->name << " (" << nh << "," << nw << ") = (" << height.str()  << " ; " << width.str() <<")");
      QString height, width;
      //       height = QString("%L1").arg(lh, 0, 'g', 3 ); // Localized format with comma needed on some config (why ?)
      //       width  = QString("%L1").arg(lw, 0, 'g', 3 );
      height = QString(_format.c_str()).arg(lh, 0, 'g', 3 ); // Localized format with comma needed on some config (why ?)
      width  = QString(_format.c_str()).arg(lw, 0, 'g', 3 );
      DEBTRACE(dummyNode->name << " (" << nh << "," << nw << ") = (" << height.latin1()  << " ; " << width.latin1() <<")");
      agxset( dummyNode, agfindattr(_mainGraph->proto->n,"height")->index, (char*)(height.latin1()));
      agxset( dummyNode, agfindattr(_mainGraph->proto->n,"width")->index, (char*)(width.latin1()));
      agxset( dummyNode, agfindattr(_mainGraph->proto->n,"shape")->index, "box" );
      agxset( dummyNode, agfindattr(_mainGraph->proto->n,"fixedsize")->index, "true" );
    }
  
  list<Node*> children = theBloc->edGetDirectDescendants();
  for ( list<Node*>::iterator it = children.begin(); it != children.end(); it++ )
    {
      if ( !getItem(*it) ) continue;

      // --- create graphviz objects for all direct descendants except ComposedNodes
      if (!dynamic_cast<ComposedNode*>(*it))
        {
          Agnode_t* aNode = agnode( aSubGraph, (char*)(getProc()->getChildName(*it).c_str()) );
          DEBTRACE("Add node in subgraph " << aSubGraph->name << ": " << aNode->name);
          
          // ---- Set attributes for the concrete node
          int nh = getItem(*it)->maxHeight()+2*BLOCB;
          int nw = getItem( *it )->maxWidth();
          double lh = nh/DPI;
          double lw = nw/DPI;
          // stringstream height, width;
          // height << lh; width << lw;
          // DEBTRACE(aNode->name << " (" << nh << "," << nw << ") = (" << height.str()  << " ; " << width.str() <<")");
          QString height, width;
//           height = QString("%L1").arg(lh, 0, 'g', 3 );
//           width  = QString("%L1").arg(lw, 0, 'g', 3 );
          height = QString(_format.c_str()).arg(lh, 0, 'g', 3 );
          width  = QString(_format.c_str()).arg(lw, 0, 'g', 3 );
          DEBTRACE(aNode->name << " (" << nh << "," << nw << ") = (" << height.latin1()  << " ; " << width.latin1() <<")");
          agxset( aNode, agfindattr(_mainGraph->proto->n,"height")->index, (char*)(height.latin1()));
          agxset( aNode, agfindattr(_mainGraph->proto->n,"width")->index, (char*)(width.latin1()));
          agxset( aNode, agfindattr(_mainGraph->proto->n,"shape")->index, "box" );
          agxset( aNode, agfindattr(_mainGraph->proto->n,"fixedsize")->index, "true" );
        }
    }

  // ---- Create edges (i.e. links)

  Agnode_t* aNode;
  for ( aNode = agfstnode( aSubGraph ); aNode; aNode = agnxtnode( aSubGraph, aNode) )
  {
    string aNodeName = aNode->name;
    if (aNodeName.find("dummy_") != 0)
      {
        DEBTRACE("--tail node " << aNode->name);
        Agnode_t* aTailNode = aNode;
        Node* outNode = getProc()->getChildByName( string(aTailNode->name) );
        if (outNode->getFather() != theBloc)continue; // Create edges only with outgoing nodes directly in bloc

        // --- control link from node 
        {
          OutGate *outGate = outNode->getOutGate();
          set<InGate*> setOfInGate = outGate->edSetInGate();
          set<InGate*>::const_iterator itin = setOfInGate.begin();
          for (; itin != setOfInGate.end(); ++itin)
            {
              Node *inNode = (*itin)->getNode();
              string inNodeName = getProc()->getChildName(inNode);
              DEBTRACE("---control link from tail node: ---- "<<inNodeName);
              if (Node *inFather = theBloc->isInMyDescendance(inNode))
                {
                  DEBTRACE("---edge inside the bloc");
                  Bloc *inBloc = dynamic_cast<Bloc*>(inFather);
                  if (inBloc == theBloc) // link to a bloc whitch is a direct son
                    { 
                      string inName="";
                      if (dynamic_cast<Bloc*>(inNode)) inName = "dummy_" + inNodeName;
                      else inName = inNodeName;
                      Agnode_t* aHeadNode = agnode( aSubGraph, (char*)(inName.c_str()) );
                      Agedge_t* anEdge    = agedge( aSubGraph, aTailNode, aHeadNode );
                      DEBTRACE("---control link from tail node: ---- " << aNode->name << " --> " << inName);
                    }
                  else
                    {
                      if (inBloc) // link to a bloc whitch is a grandchild
                        {
                          string fatherName = "dummy_" + getProc()->getChildName(inFather);
                          Agnode_t* aHeadNode = agnode( aSubGraph, (char*)(fatherName.c_str()) );
                          Agedge_t* anEdge    = agedge( aSubGraph, aTailNode, aHeadNode );
                          DEBTRACE("---control link from tail node: ---- " << aNode->name << " --> " << fatherName);
                        }
                      else // link to a son whitch is of another type of composed node
                        {
                          string fatherName = getProc()->getChildName(inFather);
                          Agnode_t* aHeadNode = agnode( aSubGraph, (char*)(fatherName.c_str()) );
                          Agedge_t* anEdge    = agedge( aSubGraph, aTailNode, aHeadNode );
                          DEBTRACE("---control link from tail node: ---- " << aNode->name << " --> " << fatherName);
                        }
                    }
                }
              else 
                {
                  DEBTRACE("---edge going outside the current bloc scope, to memorize for later");
                  _savedControlLinks.insert(pair<Node*, Node*>(inNode, outNode)); 
                }
            }
        }
        // --- datalink from node
        {
          list<OutPort*> outPortList = outNode->getSetOfOutPort();
          list<OutPort*>::const_iterator itou = outPortList.begin();
          for (; itou != outPortList.end(); ++itou)
            {
              set<InPort*> inPortList = (*itou)->edSetInPort();
              set<InPort*>::const_iterator itin = inPortList.begin();
              for (; itin != inPortList.end(); ++itin)
                {
                  Node *inNode = (*itin)->getNode();
                  string inNodeName = getProc()->getChildName(inNode);
                  DEBTRACE("------data link from tail node: ---- ");
                  if (Node *inFather = theBloc->isInMyDescendance(inNode))
                    {
                      DEBTRACE("---edge inside the bloc");
                      Bloc *inBloc = dynamic_cast<Bloc*>(inFather);
                      if (inBloc == theBloc) // link to a bloc whitch is a direct son
                        { 
                          string inName="";
                          if (dynamic_cast<Bloc*>(inNode)) inName = "dummy_" + inNodeName;
                          else inName = inNodeName;
                          Agnode_t* aHeadNode = agnode( aSubGraph, (char*)(inName.c_str()) );
                          Agedge_t* anEdge    = agedge( aSubGraph, aTailNode, aHeadNode );
                          DEBTRACE("------data link from tail node: ---- " << aNode->name << " --> " << inName);
                        }
                      else
                        {
                          if (inBloc) // link to a bloc whitch is a grandchild
                            {
                              string fatherName = "dummy_" + getProc()->getChildName(inFather);
                              Agnode_t* aHeadNode = agnode( aSubGraph, (char*)(fatherName.c_str()) );
                              Agedge_t* anEdge    = agedge( aSubGraph, aTailNode, aHeadNode );
                              DEBTRACE("------data link from tail node: ---- " << aNode->name << " --> " << fatherName);
                            }
                          else // link to a son whitch is of another type of composed node
                            {
                              string fatherName = getProc()->getChildName(inFather);
                              Agnode_t* aHeadNode = agnode( aSubGraph, (char*)(fatherName.c_str()) );
                              Agedge_t* anEdge    = agedge( aSubGraph, aTailNode, aHeadNode );
                              DEBTRACE("------data link from tail node: ---- " << aNode->name << " --> " << fatherName);
                            }
                        }
                    }
                  else 
                    {
                      DEBTRACE("---edge going outside the current bloc scope, to memorize for later");
                      _savedControlLinks.insert(pair<Node*, Node*>(inNode, outNode)); 
                    }
                }
            }
        }
        // --- dummy node for the bloc --> all nodes
        {
          DEBTRACE("------------------------------------ dummy --> " << aNode->name);
          Agnode_t* anEdgeNode = aNode;
          if (!dynamic_cast<Bloc*>(theBloc) && dummyNode != anEdgeNode)
            {
              Agedge_t* anEdge    = agedge( aSubGraph, dummyNode, anEdgeNode );
              DEBTRACE("-----link from " << dummyNode->name<< " to " << anEdgeNode->name);
            }

          // --- retreive arriving links saved previously

          DEBTRACE("--edge node " << aNode->name);
          Node* inNode = outNode;
          InGate *inGate = inNode->getInGate();
          
          pair<multimap<Node*, Node*>::iterator, multimap<Node*, Node*>::iterator> ppp;
          ppp = _savedControlLinks.equal_range(inNode); // --- saved edges comming to inNode
          for(multimap<Node*, Node*>::iterator itm = ppp.first; itm != ppp.second; ++itm)
            {
              Node *outNode = (*itm).second;
              DEBTRACE("saved control link from Node: " << outNode->getName());
              string outName = "";
              if (dynamic_cast<Bloc*>(outNode)) outName = "dummy_" + getProc()->getChildName(outNode);
              else outName = getProc()->getChildName(outNode);
              DEBTRACE("------------------------ saved: ---- " << outName << " --> " << aNode->name);
              Agnode_t* aTailNode = agnode( aSubGraph, (char*)(outName.c_str()) );
              Agedge_t* anEdge    = agedge( aSubGraph, aTailNode, anEdgeNode );
            }
          _savedControlLinks.erase(_savedControlLinks.lower_bound(inNode), _savedControlLinks.upper_bound(inNode));
        }
      }
  }

  //Only for Blocs create edges for out control link
  for ( list<Node*>::iterator it = children.begin(); it != children.end(); it++ )
    {
      Node* outNode = *it;
      DEBTRACE(outNode);
      DEBTRACE(outNode->getName());
      if (dynamic_cast<Bloc*>(outNode))
        {
          string outName = "dummy_" + getProc()->getChildName(outNode);
          DEBTRACE(outName);
          Agnode_t* aTailNode = agnode( aSubGraph, (char*)(outName.c_str()) );
          OutGate *outGate = outNode->getOutGate();
          set<InGate*> setOfInGate = outGate->edSetInGate();
          set<InGate*>::const_iterator itin = setOfInGate.begin();
          for (; itin != setOfInGate.end(); ++itin)
            {
              Node *inNode = (*itin)->getNode();
              DEBTRACE(inNode);
              DEBTRACE(inNode->getName());
              string inName = getProc()->getChildName(inNode);
              if (dynamic_cast<Bloc*>(inNode)) inName = "dummy_" + inName;
              Agnode_t* aHeadNode = agnode( aSubGraph, (char*)(inName.c_str()) );
              DEBTRACE("---control link from tail node: ---- " << outName << " to --> " << inName);
              Agedge_t* anEdge    = agedge( aSubGraph, aTailNode, aHeadNode );
            }
          if (!dynamic_cast<Bloc*>(theBloc))
            {
              //If the parent is not a Bloc add an edge between parent and son
              Agedge_t* anEdge    = agedge( aSubGraph, dummyNode, aTailNode );
              DEBTRACE("-----link from " << dummyNode->name<< " to --> " << aTailNode->name);
            }
        }
    }
}

// store some functions to have possibility to arrange nodes only on the one giving level

int YACSGui_Graph::arrangeNodesWithinBloc( Bloc* theBloc )
{
  DEBTRACE("YACSGui_Graph::arrangeNodes()");

  // ---- Create a graphviz context
  if(!aGvc)
    {
      aginit();
      aGvc = gvContext();
    }

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
//   if ( !(anAttr = agfindattr( aGraph, "ordering" )) )
//     anAttr = agraphattr( aGraph, "ordering", "" );
//   agxset( aGraph, anAttr->index, "in" );

  // 4) rankdir attribute
  if ( !(anAttr = agfindattr( aGraph, "rankdir" )) )
    anAttr = agraphattr( aGraph, "rankdir", "TB" );
  agxset( aGraph, anAttr->index, "LR" );

  if ( !(anAttr = agfindattr(aGraph, "splines")))
    anAttr = agraphattr(aGraph, "splines", "");
  agxset(aGraph, anAttr->index, "");

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
    DEBTRACE(">> tail node "<<aNode->name);
    // lets, aNodes[i] is a tail (from) node of the link
    Agnode_t* aTailNode = aNode; //aNodes[i];
    Node* aNodeEngine = theBloc->getChildByName( string(aTailNode->name) ); //getNodeByName( string(aTailNode->name) );
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
	  string aClassName = anIOPort->getEngine()->getNameOfTypeOfCurrentInstance();
	  if ( !aClassName.compare(OutputPort::NAME) ||
	       !aClassName.compare(OutputDataStreamPort::NAME) ||
	       !aClassName.compare(OutputCalStreamPort::NAME) ||
	       !aClassName.compare(OutGate::NAME) )
	  {
	    list<YACSPrs_Link*> aLinks = anIOPort->getLinks();
	    list<YACSPrs_Link*>::iterator it = aLinks.begin();
	    for(; it != aLinks.end(); it++)
	      if ( YACSPrs_PortLink* aLink = dynamic_cast<YACSPrs_PortLink*>( *it ) )
	      {
		// search head node of the link
		string aNodeName = getInNodeName(theBloc,aNodeEngine,aLink->getInputPort()->getNode()->getEngine());
		if ( aNodeName != "" )
		{
		  Agnode_t* aHeadNode = agnode( aGraph, (char*)(aNodeName.c_str()) );
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
	}
	else if ( YACSPrs_LabelPort* aLPort = dynamic_cast<YACSPrs_LabelPort*>( aPort ) )
	{
	  // label links (its have to be removed in the future)
	  if ( aLPort->getSlaveNode() )
	  {
	    string aNodeName = getInNodeName(theBloc,aNodeEngine,aLPort->getSlaveNode());
	    if ( aNodeName != "" )
	    {
	      Agnode_t* aHeadNode = agnode( aGraph, (char*)(aNodeName.c_str()) );
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
    }
    //printf("\n");
  }

  // check if any "OutBloc -> InBloc" links from myBlocInsideLinks map have to be added
  // into aGraph corresponding to theBloc
  if ( myBlocInsideLinks.find(theBloc) != myBlocInsideLinks.end() )
  {
    list< pair<Bloc*,Bloc*> > aBLinks = myBlocInsideLinks[theBloc];
    list< pair<Bloc*,Bloc*> >::iterator aBLinksIt = aBLinks.begin();
    for ( ; aBLinksIt != aBLinks.end(); aBLinksIt++ )
    {
      Bloc* anOutBloc = (*aBLinksIt).first;
      Bloc* anInBloc = (*aBLinksIt).second;

      Agnode_t* aTailNode = agnode( aGraph, (char*)(anOutBloc->getName().c_str()) );
      Agnode_t* aHeadNode = agnode( aGraph, (char*)(anInBloc->getName().c_str()) );
      Agedge_t* anEdge = agedge( aGraph, aTailNode, aHeadNode );
      //printf(">> 11 Add edge : %s -> %s\n", aTailNode->name, aHeadNode->name);
		  
      // ---- Set attributes for the concrete edge
      // 1) headport attribute
      agxset( anEdge, agfindattr(aGraph->proto->e,"headport")->index, "w" );
      
      // 2) tailport attribute
      agxset( anEdge, agfindattr(aGraph->proto->e,"tailport")->index, "e" );
    }
  }

  DEBTRACE("--------------");

  // ---- Bind graph to graphviz context - currently must be done before layout
#ifdef HAVE_DOTNEATO_H
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
    DEBTRACE(">> resize Block "<<theBloc->getName());
    int aBBwidth = GD_bb( aGraph ).UR.x - GD_bb( aGraph ).LL.x;
    YACSPrs_BlocNode* aBlocNodePrs = dynamic_cast<YACSPrs_BlocNode*>( getItem( theBloc ) );
    if ( aBlocNodePrs )
    {
      int aXRight = aBlocNodePrs->width() + (aBBwidth - aBlocNodePrs->getAreaRect().width()) + BLOCNODE_MARGIN;
      int aYBottom = aBlocNodePrs->height() + (aBBheight - aBlocNodePrs->getAreaRect().height()) + BLOCNODE_MARGIN;
      DEBTRACE(">> aXRight = " << aXRight << "; aYBottom = " << aYBottom);
      aBlocNodePrs->resize( aXRight,aYBottom );
      DEBTRACE(">> getAreaRect().x = "<<aBlocNodePrs->getAreaRect().x()<<", getAreaRect().y = "<< aBlocNodePrs->getAreaRect().y());
      aBlocNodePrs->setZ(aBlocNodePrs->z());
    }
  }
  for ( aNode = agfstnode( aGraph ); aNode; aNode = agnxtnode( aGraph, aNode))
  {
    int aXCenter = ND_coord_i( aNode ).x;
    int aYCenter = ND_coord_i( aNode ).y;
    //printf(">> node %s graphviz center (%d,%d)\n", aNode->name, aXCenter, aYCenter);

    Node* aNodeEngine = theBloc->getChildByName( string(aNode->name) ); //getNodeByName( string(aNode->name) );
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
          DEBTRACE(">> +Area : aXLeft = "<<aXLeft<<", aYTop = "<<aYTop);
	}
      }

      if ( dynamic_cast<YACSPrs_LoopNode*>( aNodePrs ) && getDMode() == YACSGui_Graph::FullId )
      {
	aXLeft += 3*TITLE_HEIGHT/2;
	aYTop += 3*TITLE_HEIGHT/2;
      }

      // move presentation of the node
      //aNodePrs->setX( aXLeft ); aNodePrs->setY( aYTop );      
      DEBTRACE(">> aNodePrs->oldX = "<<aNodePrs->x()<<", aNodePrs->oldY = "<<aNodePrs->y());
      aNodePrs->move( aXLeft, aYTop );
      //printf(">> height = %d, width = %d\n",aNodePrs->height(),aNodePrs->width());

      // print a new node's position for checking
      //char buf[50];
      //sprintf(buf, "left=%d,top=%d", aXLeft, aYTop);
      //printf(">> node %s at position (%s)\n\n", aNode->name, buf);
    }
  }

  // ---- Delete layout
#ifdef HAVE_DOTNEATO_H
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
    list<Node*> aNodeSet = theFather->edGetDirectDescendants(); //getChildren(); //getAllRecursiveConstituents();
    for ( list<Node*>::iterator it = aNodeSet.begin(); it != aNodeSet.end(); it++ )
    {
      if ( !getItem( *it ) ) continue;

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
	Agraph_t* aCluster = agsubg( theGraph, (char*)((string("cluster")+getProc()->getChildName( *it )).c_str()) );
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

std::string YACSGui_Graph::getInNodeName(YACS::ENGINE::Bloc* theBloc,
					 YACS::ENGINE::Node* theOutNode,
					 YACS::ENGINE::Node* theInNode)
{
  Node* anInNode = theInNode;
  Node* aStoredNode = anInNode;
  
  string aNodeName = "";

  if ( !theBloc || !theOutNode || !theInNode )
    return aNodeName;

  try {
    aNodeName = theBloc->getChildName(anInNode);
  }
  catch (YACS::Exception& ex) {
    // check two cases:
    // 1) when anInNode is in descendance of theBloc, but not direct
    //  - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // |theBloc             - - - - - - - - - - - - - - - - -  |
    // |                   |another block                    | |
    // |                   |     - - - - - - - - - - - - - - | |
    // |  ______________   |    |another block  _________   || |
    // | |theOutNode    |__|____|______________|anInNode |  || |
    // | |              |  |    |              |         |  || |
    // | |______________|  |    |              |_________|  || |
    // |                   |    |_ _ _ _ _ _ _ _ _ _ _ _ _ _|| |
    // |                   |_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _| |
    // |_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _|
    //
    // 2) when anInNode is not of descendance of theBloc
    //  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    // |aCommonFather                                                          |
    // |  - - - - - - - - - - - -                                              |
    // | |theBloc                |        - - - - - - - - - - - - - - - - -    |
    // | |                       |       |another block                    |   |
    // | |                       |       |     - - - - - - - - - - - - - - |   |
    // | |  ______________       |       |    |another block  _________   ||   |
    // | | |theOunNode    |______|_______|____|______________|anInNode |  ||   |
    // | | |              |      |       |    |              |_________|  ||   |
    // | | |______________|      |       |    |                           ||   |
    // | |                       |       |    |_ _ _ _ _ _ _ _ _ _ _ _ _ _||   |
    // | |_ _ _ _ _ _ _ _ _ _ _ _|       |_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _|   |
    // |_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _|
    //
    while ( anInNode && anInNode->getFather() != theBloc && anInNode->getFather() != getProc() )
    //                 case 1                               case 2
    {
      anInNode = anInNode->getFather();
    }
    
    if ( anInNode->getFather() == theBloc )
    { // case 1
      aNodeName = theBloc->getChildName(anInNode);
    }
    else if ( anInNode->getFather() == getProc() )
    { // case 2
      anInNode = aStoredNode;
      
      Bloc* aBlocOut = 0;
      Bloc* aBlocIn = 0;
      // find the nearest common ancestor for anOutNode and anInNode
      while ( anInNode->getFather() != getProc() )
      {
	if ( aBlocOut = dynamic_cast<Bloc*>(anInNode->getFather()->isInMyDescendance(theOutNode)) )
	{
	  aBlocIn = dynamic_cast<Bloc*>(anInNode);
	  break;
	}
	anInNode = anInNode->getFather();
      }
      
      if ( aBlocOut && aBlocIn )
      {
	if ( Bloc* aCommonFather = dynamic_cast<Bloc*>(aBlocOut->getFather()) )
	{
	  if ( myBlocInsideLinks.find(aCommonFather) == myBlocInsideLinks.end() )
	    myBlocInsideLinks.insert( make_pair( aCommonFather, 
						 list<pair<Bloc*,Bloc*> >(1,make_pair(aBlocOut,aBlocIn)) ) );
	  else
	    myBlocInsideLinks[aCommonFather].push_back( make_pair(aBlocOut,aBlocIn) );
	}
      }
    }
  }

  return aNodeName;
}
 
