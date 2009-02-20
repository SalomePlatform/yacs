//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
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
#include "SceneBlocItem.hxx"
#include "Scene.hxx"
#include "Menus.hxx"
#include "QtGuiContext.hxx"
#include "guiObservers.hxx"
#include "ComposedNode.hxx"
#include "Proc.hxx"
#include "OutputPort.hxx"
#include "OutGate.hxx"
#include "InputPort.hxx"
#include "InGate.hxx"

#include <sstream>

// --- for graphviz 2.8
#undef HAVE_CONFIG_H

#ifdef HAVE_DOTNEATO_H
  #include <dotneato.h>
#else
  #include <gvc.h>
#endif

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;

/*! Definition in dot per inch for graphviz input:
 *  size of elementary nodes are in pixel in Qt, and given in inches to graphviz
 */
#define DPI 72.

static GVC_t* aGvc = 0;


SceneBlocItem::SceneBlocItem(QGraphicsScene *scene, SceneItem *parent,
                             QString label, Subject *subject)
  : SceneComposedNodeItem(scene, parent, label, subject)
{
  DEBTRACE("SceneBlocItem::SceneBlocItem " <<label.toStdString());
  _format = "%1"; // --- format to convert a float without locale: ex. 9.81
}

SceneBlocItem::~SceneBlocItem()
{
}

//! Auto-arrange nodes inside a schema using Graphviz C API.
/*!
 */

void SceneBlocItem::arrangeChildNodes()
{
  DEBTRACE("SceneBlocItem::arrangeChildNodes");

  SubjectComposedNode *scnode = dynamic_cast<SubjectComposedNode*>(getSubject());
  YASSERT(scnode);
  ComposedNode *cnode = dynamic_cast<ComposedNode*>(scnode->getNode());
  YASSERT(cnode);

  // ---- Create a graphviz context

  if(!aGvc)
    {
      aginit();
      aGvc = gvContext();
    }

  // ---- Create a graph

  _graph = agopen((char*)( cnode->getName().c_str()), AGDIGRAPH);

  // ---- Initialize and set attributes for the graph
  
  Agsym_t* attr;
  if ( !(attr = agfindattr(_graph, "compound")))
    attr = agraphattr(_graph, "compound", "false");
  agxset(_graph, attr->index, "true");

  if ( !(attr = agfindattr(_graph, "rankdir")))
    attr = agraphattr(_graph, "rankdir", "TB");
  agxset(_graph, attr->index, "LR");

//   if ( !(attr = agfindattr(_graph, "ordering")))
//     attr = agraphattr(_graph, "ordering", "" );
//   agxset(_graph, attr->index, "in" );
  
  if ( !(attr = agfindattr(_graph, "dpi")))
    attr = agraphattr(_graph, "dpi", "72");
  agxset(_graph, attr->index, "72"); // --- must be coherent with #define DPI

  // --- label is used to reserve place for bloc banners (adjust size with font !)

  if ( !(attr = agfindattr(_graph, "label")))
    attr = agraphattr(_graph, "label", "label");
  agxset(_graph, attr->index, "myLabel");

  if ( !(attr = agfindattr(_graph, "labelloc")))
    attr = agraphattr(_graph, "labelloc", "top");
  agxset(_graph, attr->index, "top");

  if ( !(attr = agfindattr(_graph, "fontsize")))
    attr = agraphattr(_graph, "fontsize", "24");
  agxset(_graph, attr->index, "24");

  if ( !(attr = agfindattr(_graph, "splines")))
    attr = agraphattr(_graph, "splines", "");
  agxset(_graph, attr->index, "");

  // --- Initialize attributes for nodes

  if ( !(attr = agfindattr( _graph->proto->n, "height")))
    attr = agnodeattr(_graph, "height", "" );

  if ( !(attr = agfindattr( _graph->proto->n, "width")))
    attr = agnodeattr(_graph, "width", "" );

  if ( !(attr = agfindattr( _graph->proto->n, "shape")))
    attr = agnodeattr(_graph, "shape", "" );

  if ( !(attr = agfindattr( _graph->proto->n, "fixedsize")))
    attr = agnodeattr(_graph, "fixedsize", "false" );

  // ---- Bind graph to graphviz context - must be done before layout
  // ---- Compute a layout

  try
    {
      getNodesInfo(cnode);
      //      createGraphvizNodes(cnode);
      DEBTRACE("end of graphviz input");
#ifdef _DEVDEBUG_
      agwrite(_graph, stderr);
#endif
#ifdef HAVE_DOTNEATO_H
      gvBindContext(aGvc, _graph);
      dot_layout(_graph);
#else
      //DEBTRACE("external render for test");
      //gvRenderFilename(aGvc, _mainGraph, "dot", "graph1.dot");
      DEBTRACE("compute layout");
      gvLayout(aGvc, _graph, "dot");
      DEBTRACE("external render for test");
#ifdef _DEVDEBUG_
      gvRenderFilename(aGvc, _graph, "dot", "graph2.dot");
#endif
#endif
   }
  catch (std::exception &e)
    {
      DEBTRACE("Exception Graphviz layout: " << e.what());
      return;
    }
  catch (...)
    {
      DEBTRACE("Unknown Exception Graphviz layout ");
      return;
    }
  DEBTRACE("start of display");
  // ---- layout Canvas nodes recursively

  arrangeCanvasNodes(cnode);

  DEBTRACE("clean up graphviz");
  // ---- Delete layout

#ifdef HAVE_DOTNEATO_H
  dot_cleanup(_graph);
#else
  gvFreeLayout(aGvc, _graph);
#endif

  // ---- Free graph structures

  agclose(_graph) ;

  // ---- Free context and return number of errors

#ifndef HAVE_DOTNEATO_H
  //gvFreeContext( aGvc );
#endif

  // --- update scene
}

void  SceneBlocItem::getNodesInfo(YACS::ENGINE::ComposedNode *cnode)
{
  Proc *proc = GuiContext::getCurrent()->getProc();

  // --- Create Nodes = direct descendants in the bloc

  list<Node*> children = cnode->edGetDirectDescendants();
  for (list<Node*>::iterator it = children.begin(); it != children.end(); ++it)
    {
      Agnode_t* aNode = agnode(_graph, (char*)(proc->getChildName(*it).c_str()));
      DEBTRACE("Add node in graph: " << aNode->name);

      SubjectNode* snode = GuiContext::getCurrent()->_mapOfSubjectNode[(*it)];
      SceneItem* sci = QtGuiContext::getQtCurrent()->_mapOfSceneItem[snode];
      double nh = sci->getHeight();
      double nw = sci->getWidth();
      double lh = nh/DPI;
      double lw = nw/DPI;

      QString height, width;
      height = QString(_format.c_str()).arg(lh, 0, 'g', 3);
      width  = QString(_format.c_str()).arg(lw, 0, 'g', 3);

      DEBTRACE(aNode->name << " (" << nh << "," << nw << ") = (" << height.toStdString()  << " ; " << width.toStdString() <<")");
      agxset( aNode, agfindattr(_graph->proto->n,"height")->index, (char*)(height.toAscii().data()));
      agxset( aNode, agfindattr(_graph->proto->n,"width")->index, (char*)(width.toAscii().data()));
      agxset( aNode, agfindattr(_graph->proto->n,"shape")->index, "box" );
      agxset( aNode, agfindattr(_graph->proto->n,"fixedsize")->index, "true" );
    }

  // --- Create edges (i.e. links)

  Agnode_t* aNode;
  for (aNode = agfstnode(_graph); aNode; aNode = agnxtnode(_graph, aNode))
  {
    string aNodeName = aNode->name;
    DEBTRACE("--- tail node " << aNode->name);
    Agnode_t* aTailNode = aNode;
    Node* outNode = proc->getChildByName(string(aTailNode->name));
    if (outNode->getFather() != cnode)
      {
        DEBTRACE(" =========== problem here ! =============================");
        continue; // Create edges only with outgoing nodes directly in bloc
      }

    // --- control link from node, keep only link staying inside the bloc

    {
      OutGate *outGate = outNode->getOutGate();
      set<InGate*> setOfInGate = outGate->edSetInGate();
      set<InGate*>::const_iterator itin = setOfInGate.begin();
      for (; itin != setOfInGate.end(); ++itin)
        {
          Node *inNode = (*itin)->getNode();
          string inName = proc->getChildName(inNode);
          DEBTRACE("--- control link from tail node: --- "<<inName);
          // --- isInMyDescendance(this)   return this
          //     isInMyDescendance(inNode) return direct child if inNode is a direct child or grandchild
          if (Node *inDCNode = cnode->isInMyDescendance(inNode))
            {
              DEBTRACE("--- edge inside the bloc " << inDCNode->getName());
              string inDCName = proc->getChildName(inDCNode);
              Agnode_t* aHeadNode = agnode(_graph, (char*)(inDCName.c_str()));
              Agedge_t* anEdge    = agedge(_graph, aTailNode, aHeadNode);
              DEBTRACE("--- control link from tail node: --- " << aNode->name << " --> " << inDCName);
            }
        }
    }

    // --- datalink from node, keep only link staying inside the bloc

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
              string inName = proc->getChildName(inNode);
              DEBTRACE("------ data link from tail node: ---- ");
              if (Node *inDCNode = cnode->isInMyDescendance(inNode))
                {
                  DEBTRACE("--- edge inside the bloc " << inDCNode->getName());
                  string inDCName = proc->getChildName(inDCNode);
                  Agnode_t* aHeadNode = agnode(_graph, (char*)(inDCName.c_str()));
                  Agedge_t* anEdge    = agedge(_graph, aTailNode, aHeadNode);
                  DEBTRACE("------ data link from tail node: ---- " << aNode->name << " --> " << inDCName);
                }
            }
        }
    }
  }
}


void SceneBlocItem::arrangeCanvasNodes(YACS::ENGINE::ComposedNode *cnode)
{
  DEBTRACE("SceneBlocItem::arrangeCanvasNodes");
  Proc *proc = GuiContext::getCurrent()->getProc();

  SubjectNode* subCompo = GuiContext::getCurrent()->_mapOfSubjectNode[cnode];
  SceneItem* sci = QtGuiContext::getQtCurrent()->_mapOfSceneItem[subCompo];
  SceneComposedNodeItem *sceneCompo = dynamic_cast<SceneComposedNodeItem*>(sci);
  YASSERT(sceneCompo);
  qreal yHead = sceneCompo->getHeaderBottom() + sceneCompo->getMargin() + sceneCompo->getNml();
  qreal xOffset = sceneCompo->getMargin() + sceneCompo->getNml();

  list<Node*> children = cnode->edGetDirectDescendants();
  for (list<Node*>::iterator it = children.begin(); it != children.end(); ++it)
    {
      Agnode_t* aNode = agnode(_graph, (char*)(proc->getChildName(*it).c_str()));
      DEBTRACE("Get node in graph: " << aNode->name);
      SubjectNode* snode = GuiContext::getCurrent()->_mapOfSubjectNode[(*it)];
      SceneItem* sci = QtGuiContext::getQtCurrent()->_mapOfSceneItem[snode];

      qreal xCenter = ND_coord_i(aNode).x;
      qreal yCenter = ND_coord_i(aNode).y;
      qreal halfWidth = sci->boundingRect().width()/2.;
      qreal halfHeight = sci->boundingRect().height()/2.;

      sci->setPos(xOffset + xCenter -halfWidth, yHead + yCenter -halfHeight);
    }
  sceneCompo->checkGeometryChange();
  if (Scene::_autoComputeLinks)
    {
      YACS::HMI::SubjectProc* subproc = QtGuiContext::getQtCurrent()->getSubjectProc();
      SceneItem *item = QtGuiContext::getQtCurrent()->_mapOfSceneItem[subproc];
      SceneComposedNodeItem *proc = dynamic_cast<SceneComposedNodeItem*>(item);
      proc->rebuildLinks();
    }

}
