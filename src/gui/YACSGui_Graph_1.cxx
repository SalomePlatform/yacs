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

#ifdef HAVE_DOTNEATO_H
  #include <dotneato.h>
#else
  #include <gvc.h>
#endif

#include "utilities.h"

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

//! Auto-arrange nodes inside a schema using Graphviz C API.
/*!
 */
int YACSGui_Graph::arrangeNodesAlgo( YACS::ENGINE::Bloc* theBloc )
{
  MESSAGE("YACSGui_Graph::arrangeNodesAlgo() " << theBloc->getName());
  _savedControlLinks.clear();
  _maxdep = 0;
  _format="%1";  // ---  standard format to convert float  ex: 9.81
  //_format="%L1"; // --- localized format to convert float  ex: 9,81

  // ---- Create a graphviz context

  aginit();
  GVC_t* aGvc = gvContext();

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
  arrangeNodes(theBloc, _mainGraph, curdep);
  createGraphvizNodes(theBloc, _mainGraph);
  MESSAGE("end of graphviz input");

  // ---- Bind graph to graphviz context - must be done before layout
  // ---- Compute a layout
  try
    {
#ifdef HAVE_DOTNEATO_H
      gvBindContext(aGvc, _mainGraph);
      dot_layout(_mainGraph);
#else
      MESSAGE("external render for test");
      gvRenderFilename(aGvc, _mainGraph, "dot", "graph1.dot");
      MESSAGE("compute layout");
      gvLayout(aGvc, _mainGraph, "dot");
#endif
      MESSAGE("external render for test");
      gvRenderFilename(aGvc, _mainGraph, "dot", "graph2.dot");
   }
  catch (std::exception &e)
    {
      MESSAGE("Exception Graphviz layout: " << e.what());
      return 1;
    }
  catch (...)
    {
      MESSAGE("Unknown Exception Graphviz layout ");
      return 1;
    }
  MESSAGE("start of display");
  // ---- layout Canvas nodes recursively

  curdep = -1;
  arrangeCanvasNodes(theBloc, _mainGraph, curdep);

  MESSAGE("clean up graphviz");
  // ---- Delete layout

#ifdef HAVE_DOTNEATO_H
  dot_cleanup(_mainGraph);
#else
  gvFreeLayout(aGvc, _mainGraph);
#endif

  // ---- Free graph structures

  agclose(_mainGraph) ;

  // ---- Free context and return number of errors

  gvFreeContext( aGvc );
  
  if ( dynamic_cast<Proc*>( theBloc ) )
    getCanvas()->update();

  return 0;
}


void YACSGui_Graph::arrangeCanvasNodes(YACS::ENGINE::ComposedNode* theBloc, Agraph_t* aSubGraph, int dep)
{
  MESSAGE("arrangeCanvasNodes, bloc: " << aSubGraph->name);
  int curdep = dep +1;
  int offX = 0;
  int offY = 0;
  MESSAGE("dep="<< curdep << " maxdep= " << _maxdep << " offY=" << offY);
  
  
  // ---- Rendering the graph : retrieve nodes positions
  
  // --- bloc bounding box from graphviz

  int llx = GD_bb( aSubGraph ).LL.x;
  int lly = GD_bb( aSubGraph ).LL.y;
  int urx = GD_bb( aSubGraph ).UR.x;
  int ury = GD_bb( aSubGraph ).UR.y;
  MESSAGE("boundingBox " << aSubGraph->name << " (" << llx << "," << lly << ") (" << urx << "," << ury << ")");
  int bbHeight = (ury -lly);
  int bbWhidth = (urx -llx);
  if (curdep == 0) _bottom = bbHeight;
  SCRUTE(_bottom);

  if (!dynamic_cast<Proc*>(theBloc))
    {
      YACSPrs_BlocNode* aBlocNodePrs = dynamic_cast<YACSPrs_BlocNode*>(getItem(theBloc));
      if (aBlocNodePrs)
        {
          aBlocNodePrs->setIsCheckAreaNeeded(false);
          aBlocNodePrs->resize(bbWhidth, bbHeight);
          aBlocNodePrs->setX(llx + offX);
          aBlocNodePrs->setY(_bottom -ury);
          aBlocNodePrs->setIsCheckAreaNeeded(true);
        }
    }
      
  // --- subgraphs
  
  set<Node*> children = theBloc->edGetDirectDescendants();
  for ( set<Node*>::iterator it = children.begin(); it != children.end(); it++ )
    {
      if (ComposedNode* childBloc = dynamic_cast<ComposedNode*>(*it))
        {
          string clusterName="cluster_";
          if (dynamic_cast<Proc*>(childBloc)) clusterName += childBloc->getName();
          else clusterName += getProc()->getChildName(childBloc);
          MESSAGE(clusterName);
          Agraph_t* childGraph = agsubg(aSubGraph,(char*)clusterName.c_str());
          assert(childGraph);
          arrangeCanvasNodes(childBloc, childGraph, curdep);
        }
      if (!dynamic_cast<Bloc*>(*it))
        {
          string nodeName = getProc()->getChildName(*it);
          Agnode_t* aNode = agnode(aSubGraph,(char*)nodeName.c_str());
          int aXCenter = ND_coord_i(aNode).x;
          int aYCenter = ND_coord_i(aNode).y;
          YACSPrs_ElementaryNode* aNodePrs = getItem(*it);
          if ( aNodePrs )
            {
              int aXLeft = aXCenter - aNodePrs->maxWidth()/2;
              int aYTop  = aYCenter + aNodePrs->maxHeight()/2;
              MESSAGE("Node: " << nodeName << " aXCenter = " << aXCenter << " aYCenter = " << aYCenter);
              MESSAGE("Node: " << nodeName << " aXLeft = " << aXLeft << " aYTop = " << aYTop);
              aNodePrs->setIsCheckAreaNeeded(false);
              aNodePrs->setX(aXLeft + offX);
              aNodePrs->setY(_bottom -(aYTop + offY));
              aNodePrs->setIsCheckAreaNeeded(true);
            }
        }
    }
}




int YACSGui_Graph::arrangeNodes( YACS::ENGINE::ComposedNode* theBloc, Agraph_t* aSubGraph, int dep)
{
  MESSAGE("YACSGui_Graph::arrangeNodes() " << theBloc->getName());

  int curdep = dep +1;
  if (curdep > _maxdep)
    {
      _maxdep = curdep;
      MESSAGE("Max level of nested blocs: "<< _maxdep);
    }
  int aRetVal = 0;

  // --- collect all Bloc children nodes of the given theBloc

  set<Node*> children = theBloc->edGetDirectDescendants();
  for ( set<Node*>::iterator it = children.begin(); it != children.end(); it++ )
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

          Agraph_t* aCluster = agsubg(aClustertop , (char*)(clusterName.c_str()));
          agset(aCluster, "labelloc", "top");
          MESSAGE("Add Block node " << aCluster->name);

          aRetVal = arrangeNodes(childBloc, aCluster, curdep);
          createGraphvizNodes(childBloc, aCluster);
        }
    }
  return aRetVal;
}


void YACSGui_Graph::createGraphvizNodes( ComposedNode* theBloc, Agraph_t* aSubGraph )
{
  MESSAGE("YACSGui_Graph::createGraphvizNodes() " << theBloc->getName());

  // ---- Create nodes (+ small dummy node for the bloc)


  Agnode_t* dummyNode;
  if (dynamic_cast<Bloc*>(theBloc))
    {
      string dummyName = "dummy_";
      if (dynamic_cast<Proc*>(theBloc)) dummyName += theBloc->getName();
      else dummyName += getProc()->getChildName(theBloc);
      SCRUTE(dummyName);
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
        agxset( dummyNode, agfindattr(_mainGraph->proto->n,"label")->index, "" );
      }
    }
  else
    {
      dummyNode = agnode( aSubGraph, (char*)(getProc()->getChildName(theBloc).c_str()) );
      MESSAGE("Add node in subgraph " << aSubGraph->name << ": " << dummyNode->name);
      
      // ---- Set attributes for the concrete node
      int nh = getItem(theBloc)->maxHeight()+2*BLOCB;
      int nw = getItem(theBloc)->maxWidth();
      double lh = nh/DPI;
      double lw = nw/DPI;
      // stringstream height, width;
      // height << lh; width << lw;
      // MESSAGE(dummyNode->name << " (" << nh << "," << nw << ") = (" << height.str()  << " ; " << width.str() <<")");
      QString height, width;
//       height = QString("%L1").arg(lh, 0, 'g', 3 ); // Localized format with comma needed on some config (why ?)
//       width  = QString("%L1").arg(lw, 0, 'g', 3 );
      height = QString(_format.c_str()).arg(lh, 0, 'g', 3 ); // Localized format with comma needed on some config (why ?)
      width  = QString(_format.c_str()).arg(lw, 0, 'g', 3 );
      MESSAGE(dummyNode->name << " (" << nh << "," << nw << ") = (" << height.latin1()  << " ; " << width.latin1() <<")");
      agxset( dummyNode, agfindattr(_mainGraph->proto->n,"height")->index, (char*)(height.latin1()));
      agxset( dummyNode, agfindattr(_mainGraph->proto->n,"width")->index, (char*)(width.latin1()));
      agxset( dummyNode, agfindattr(_mainGraph->proto->n,"shape")->index, "box" );
      agxset( dummyNode, agfindattr(_mainGraph->proto->n,"fixedsize")->index, "true" );
    }
  
  set<Node*> children = theBloc->edGetDirectDescendants();
  for ( set<Node*>::iterator it = children.begin(); it != children.end(); it++ )
    {
      // --- create graphviz objects for all direct descendants except ComposedNodes
      if (!dynamic_cast<ComposedNode*>(*it))
        {
          Agnode_t* aNode = agnode( aSubGraph, (char*)(getProc()->getChildName(*it).c_str()) );
          MESSAGE("Add node in subgraph " << aSubGraph->name << ": " << aNode->name);
          
          // ---- Set attributes for the concrete node
          int nh = getItem(*it)->maxHeight()+2*BLOCB;
          int nw = getItem( *it )->maxWidth();
          double lh = nh/DPI;
          double lw = nw/DPI;
          // stringstream height, width;
          // height << lh; width << lw;
          // MESSAGE(aNode->name << " (" << nh << "," << nw << ") = (" << height.str()  << " ; " << width.str() <<")");
          QString height, width;
//           height = QString("%L1").arg(lh, 0, 'g', 3 );
//           width  = QString("%L1").arg(lw, 0, 'g', 3 );
          height = QString(_format.c_str()).arg(lh, 0, 'g', 3 );
          width  = QString(_format.c_str()).arg(lw, 0, 'g', 3 );
          MESSAGE(aNode->name << " (" << nh << "," << nw << ") = (" << height.latin1()  << " ; " << width.latin1() <<")");
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
        MESSAGE("--tail node " << aNode->name);
        Agnode_t* aTailNode = aNode;
        Node* outNode = getProc()->getChildByName( string(aTailNode->name) );

        // --- control link from node 
        {
          OutGate *outGate = outNode->getOutGate();
          set<InGate*> setOfInGate = outGate->edSetInGate();
          set<InGate*>::const_iterator itin = setOfInGate.begin();
          for (; itin != setOfInGate.end(); ++itin)
            {
              Node *inNode = (*itin)->getNode();
              string inNodeName = getProc()->getChildName(inNode);
              MESSAGE("---control link from tail node: ---- ");
              if (Node *inFather = theBloc->isInMyDescendance(inNode))
                {
                  MESSAGE("---edge inside the bloc");
                  Bloc *inBloc = dynamic_cast<Bloc*>(inFather);
                  if (inBloc == theBloc) // link to a bloc whitch is a direct son
                    { 
                      string inName="";
                      if (dynamic_cast<Bloc*>(inNode)) inName = "dummy_" + inNodeName;
                      else inName = inNodeName;
                      Agnode_t* aHeadNode = agnode( aSubGraph, (char*)(inName.c_str()) );
                      Agedge_t* anEdge    = agedge( aSubGraph, aTailNode, aHeadNode );
                      MESSAGE("---control link from tail node: ---- " << aNode->name << " --> " << inName);
                    }
                  else
                    {
                      if (inBloc) // link to a bloc whitch is a grandchild
                        {
                          string fatherName = "dummy_" + getProc()->getChildName(inFather);
                          Agnode_t* aHeadNode = agnode( aSubGraph, (char*)(fatherName.c_str()) );
                          Agedge_t* anEdge    = agedge( aSubGraph, aTailNode, aHeadNode );
                          MESSAGE("---control link from tail node: ---- " << aNode->name << " --> " << fatherName);
                        }
                      else // link to a son whitch is of another type of composed node
                        {
                          string fatherName = getProc()->getChildName(inFather);
                          Agnode_t* aHeadNode = agnode( aSubGraph, (char*)(fatherName.c_str()) );
                          Agedge_t* anEdge    = agedge( aSubGraph, aTailNode, aHeadNode );
                          MESSAGE("---control link from tail node: ---- " << aNode->name << " --> " << fatherName);
                        }
                    }
                }
              else 
                {
                  MESSAGE("---edge going outside the current bloc scope, to memorize for later");
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
                  MESSAGE("------data link from tail node: ---- ");
                  if (Node *inFather = theBloc->isInMyDescendance(inNode))
                    {
                      MESSAGE("---edge inside the bloc");
                      Bloc *inBloc = dynamic_cast<Bloc*>(inFather);
                      if (inBloc == theBloc) // link to a bloc whitch is a direct son
                        { 
                          string inName="";
                          if (dynamic_cast<Bloc*>(inNode)) inName = "dummy_" + inNodeName;
                          else inName = inNodeName;
                          Agnode_t* aHeadNode = agnode( aSubGraph, (char*)(inName.c_str()) );
                          Agedge_t* anEdge    = agedge( aSubGraph, aTailNode, aHeadNode );
                          MESSAGE("------data link from tail node: ---- " << aNode->name << " --> " << inName);
                        }
                      else
                        {
                          if (inBloc) // link to a bloc whitch is a grandchild
                            {
                              string fatherName = "dummy_" + getProc()->getChildName(inFather);
                              Agnode_t* aHeadNode = agnode( aSubGraph, (char*)(fatherName.c_str()) );
                              Agedge_t* anEdge    = agedge( aSubGraph, aTailNode, aHeadNode );
                              MESSAGE("------data link from tail node: ---- " << aNode->name << " --> " << fatherName);
                            }
                          else // link to a son whitch is of another type of composed node
                            {
                              string fatherName = getProc()->getChildName(inFather);
                              Agnode_t* aHeadNode = agnode( aSubGraph, (char*)(fatherName.c_str()) );
                              Agedge_t* anEdge    = agedge( aSubGraph, aTailNode, aHeadNode );
                              MESSAGE("------data link from tail node: ---- " << aNode->name << " --> " << fatherName);
                            }
                        }
                    }
                  else 
                    {
                      MESSAGE("---edge going outside the current bloc scope, to memorize for later");
                      _savedControlLinks.insert(pair<Node*, Node*>(inNode, outNode)); 
                    }
                }
            }
        }
        // --- dummy node for the bloc --> all nodes
        {
          MESSAGE("------------------------------------ dummy --> " << aNode->name);
          Agnode_t* anEdgeNode = aNode;
          Agedge_t* anEdge    = agedge( aSubGraph, dummyNode, anEdgeNode );

          // --- retreive arriving links saved previously

          MESSAGE("--edge node " << aNode->name);
          Node* inNode = outNode;
          InGate *inGate = inNode->getInGate();
          
          pair<multimap<Node*, Node*>::iterator, multimap<Node*, Node*>::iterator> ppp;
          ppp = _savedControlLinks.equal_range(inNode); // --- saved edges comming to inNode
          for(multimap<Node*, Node*>::iterator itm = ppp.first; itm != ppp.second; ++itm)
            {
              Node *outNode = (*itm).second;
              MESSAGE("saved control link from Node: " << outNode->getName());
              string outName = "";
              if (dynamic_cast<Bloc*>(outNode)) outName = "dummy_" + getProc()->getChildName(outNode);
              else outName = getProc()->getChildName(outNode);
              MESSAGE("------------------------ saved: ---- " << outName << " --> " << aNode->name);
              Agnode_t* aTailNode = agnode( aSubGraph, (char*)(outName.c_str()) );
              Agedge_t* anEdge    = agedge( aSubGraph, aTailNode, anEdgeNode );
            }
          _savedControlLinks.erase(_savedControlLinks.lower_bound(inNode), _savedControlLinks.upper_bound(inNode));
        }
      }
  }
}

