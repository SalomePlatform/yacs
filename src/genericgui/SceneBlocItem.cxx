// Copyright (C) 2006-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
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

#include <gvc.h>

#include "Resource.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;
using namespace YACS::HMI;

/*! Definition in dot per inch for graphviz input:
 *  size of elementary nodes are in pixel in Qt, and given in inches to graphviz
 */
#define DPI 72.

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
#ifdef _DEVDEBUG_
  DEBTRACE("SceneBlocItem::arrangeChildNodes");
  clock_t start_t, end_t;
  start_t = clock();
#endif //_DEVDEBUG_
  GVC_t* aGvc = 0;


  SubjectComposedNode *scnode = dynamic_cast<SubjectComposedNode*>(getSubject());
  YASSERT(scnode);
  ComposedNode *cnode = dynamic_cast<ComposedNode*>(scnode->getNode());
  YASSERT(cnode);

  // ---- Create a graphviz context

  if(!aGvc)
    {
      DEBTRACE(setlocale(LC_ALL,NULL));
      //Graphviz is sensitive to locale : set the mimimal one ("C")for numeric
      setlocale(LC_NUMERIC, "C");
      //aginit();
      aGvc = gvContext();
    }

  // ---- Create a graph

  _graph = agopen((char*)( cnode->getName().c_str()), Agdirected, NULL);

  // ---- Initialize and set attributes for the graph

  agattr(_graph, AGRAPH, const_cast<char*>("compound"), const_cast<char*>("true"));
  agattr(_graph, AGRAPH, const_cast<char*>("rankdir"), const_cast<char*>("LR"));
  agattr(_graph, AGRAPH, const_cast<char*>("dpi"), const_cast<char*>("72"));
  agattr(_graph, AGRAPH, const_cast<char*>("label"), const_cast<char*>("myLabel"));
  agattr(_graph, AGRAPH, const_cast<char*>("labelloc"), const_cast<char*>("top"));
  agattr(_graph, AGRAPH, const_cast<char*>("fontsize"), const_cast<char*>("24"));
  agattr(_graph, AGRAPH, const_cast<char*>("splines"), const_cast<char*>(""));

  // --- Initialize attributes for nodes

  agattr(_graph, AGNODE, const_cast<char*>("height"), const_cast<char*>("" ));
  agattr(_graph, AGNODE, const_cast<char*>("width"), const_cast<char*>("" ));
  agattr(_graph, AGNODE, const_cast<char*>("shape"), const_cast<char*>("" ));
  agattr(_graph, AGNODE, const_cast<char*>("fixedsize"), const_cast<char*>("false" ));

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
      //DEBTRACE("external render for test");
      //gvRenderFilename(aGvc, _mainGraph, "dot", "graph1.dot");
      DEBTRACE("compute layout");
      gvLayout(aGvc, _graph, "dot");
      DEBTRACE("external render for test");
#ifdef _DEVDEBUG_
      gvRenderFilename(aGvc, _graph, "dot", "graph2.dot");
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
#ifdef _DEVDEBUG_
  {
    end_t = clock();
    double passe =  (end_t -start_t);
    passe = passe/CLOCKS_PER_SEC;
    DEBTRACE("graphviz : " << passe);
    start_t = end_t;
  }
#endif //_DEVDEBUG_
  DEBTRACE("start of display");
  // ---- layout Canvas nodes recursively

  arrangeCanvasNodes(cnode);

  DEBTRACE("clean up graphviz");
  // ---- Delete layout

  gvFreeLayout(aGvc, _graph);

  // ---- Free graph structures

  agclose(_graph) ;

  // ---- Free context and return number of errors

  gvFreeContext( aGvc );

  // --- update scene
#ifdef _DEVDEBUG_
  {
    end_t = clock();
    double passe =  (end_t -start_t);
    passe = passe/CLOCKS_PER_SEC;
    DEBTRACE("display : " << passe);
    start_t = end_t;
  }
#endif //_DEVDEBUG_
}

void  SceneBlocItem::getNodesInfo(YACS::ENGINE::ComposedNode *cnode)
{
  Proc *proc = GuiContext::getCurrent()->getProc();

  // --- Create Nodes = direct descendants in the bloc

  list<Node*> children = cnode->edGetDirectDescendants();
  for (list<Node*>::iterator it = children.begin(); it != children.end(); ++it)
    {
      Agnode_t* aNode = agnode(_graph, (char*)(proc->getChildName(*it).c_str()), 1);
      DEBTRACE("Add node in graph: " << agnameof(aNode));

      SubjectNode* snode = GuiContext::getCurrent()->_mapOfSubjectNode[(*it)];
      SceneItem* sci = QtGuiContext::getQtCurrent()->_mapOfSceneItem[snode];
      double nh = sci->getHeight();
      double nw = sci->getWidth();
      double lh = nh/DPI;
      double lw = nw/DPI;

      QString height, width;
      height = QString(_format.c_str()).arg(lh, 0, 'g', 3);
      width  = QString(_format.c_str()).arg(lw, 0, 'g', 3);

      DEBTRACE(agnameof(aNode) << " (" << nh << "," << nw << ") = (" << height.toStdString()  << " ; " << width.toStdString() <<")");
      agset(aNode, const_cast<char*>("height"),    height.toLatin1().data());
      agset(aNode, const_cast<char*>("width"),     width.toLatin1().data());
      agset(aNode, const_cast<char*>("shape"),     const_cast<char*>("box") );
      agset(aNode, const_cast<char*>("fixedsize"), const_cast<char*>("true") );
    }

  // --- Create edges (i.e. links)

  Agnode_t* aNode;
  for (aNode = agfstnode(_graph); aNode; aNode = agnxtnode(_graph, aNode))
  {
    string aNodeName = agnameof(aNode);
    DEBTRACE("--- tail node " << aNodeName);
    Agnode_t* aTailNode = aNode;
    Node* outNode = proc->getChildByName(string(agnameof(aTailNode)));
    if (outNode->getFather() != cnode)
      {
        DEBTRACE(" =========== problem here ! =============================");
        continue; // Create edges only with outgoing nodes directly in bloc
      }

    // --- control link from node, keep only link staying inside the bloc

    {
      OutGate *outGate = outNode->getOutGate();
      list<InGate*> setOfInGate = outGate->edSetInGate();
      list<InGate*>::const_iterator itin = setOfInGate.begin();
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
              Agnode_t* aHeadNode = agnode(_graph, (char*)(inDCName.c_str()), 1);
              Agedge_t* anEdge    = agedge(_graph, aTailNode, aHeadNode, NULL, 1);
              DEBTRACE("--- control link from tail node: --- " << agnameof(aNode) << " --> " << inDCName);
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
                  Agnode_t* aHeadNode = agnode(_graph, (char*)(inDCName.c_str()), 1);
                  Agedge_t* anEdge    = agedge(_graph, aTailNode, aHeadNode, NULL, 1);
                  DEBTRACE("------ data link from tail node: ---- " << agnameof(aNode) << " --> " << inDCName);
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
  qreal yHead = sceneCompo->getHeaderBottom() + Resource::Space_Margin;
  qreal xOffset = Resource::Space_Margin;

  list<Node*> children = cnode->edGetDirectDescendants();
  for (list<Node*>::iterator it = children.begin(); it != children.end(); ++it)
    {
      Agnode_t* aNode = agnode(_graph, (char*)(proc->getChildName(*it).c_str()), 1);
      DEBTRACE("Get node in graph: " << agnameof(aNode));
      SubjectNode* snode = GuiContext::getCurrent()->_mapOfSubjectNode[(*it)];
      SceneItem* sci = QtGuiContext::getQtCurrent()->_mapOfSceneItem[snode];

      qreal xCenter = ND_coord(aNode).x;
      qreal yCenter = ND_coord(aNode).y;
      qreal halfWidth = sci->boundingRect().width()/2.;
      qreal halfHeight = sci->boundingRect().height()/2.;

      sci->setPos(xOffset + xCenter -halfWidth, yHead + yCenter -halfHeight);
      SceneNodeItem *scni = dynamic_cast<SceneNodeItem*>(sci);
      if (scni) scni->setExpandedPos(QPointF(xOffset + xCenter -halfWidth, yHead + yCenter -halfHeight));
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
