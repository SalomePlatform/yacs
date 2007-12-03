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

#ifndef YACSGui_Graph_HeaderFile
#define YACSGui_Graph_HeaderFile

#include <Proc.hxx>
#include <Node.hxx>

#include <guiObservers.hxx>
#include <guiContext.hxx>

#include <QxGraph_Prs.h>

#include <map>

class QxGraph_ViewWindow;
class QxGraph_Canvas;
class QxGraph_CanvasView;

class YACSGui_Module;
class YACSGui_Node;
class YACSGui_Observer;
class YACSPrs_ElementaryNode;
class YACSPrs_Hook;
class YACSPrs_Port;
class YACSPrs_InOutPort;
class YACSPrs_LabelPort;
class YACSPrs_Link;
class YACSPrs_LabelLink;

struct Agraph_t;
class LineConn2d_Model;

class YACSGui_Graph : public QxGraph_Prs
{
public:
  YACSGui_Graph(YACSGui_Module*, QxGraph_Canvas*, YACS::HMI::GuiContext*);
  virtual ~YACSGui_Graph();

  YACS::ENGINE::Proc*     getProc() const;
  YACS::HMI::GuiContext*  getContext() const { return myCProc; }
  
  YACSGui_Observer*       getStatusObserver() const { return myNodeStatusObserver; }

  virtual void            update();
  void                    update( YACS::ENGINE::Node*, YACS::HMI::SubjectComposedNode* );
  void                    update( YACSPrs_InOutPort* );
  void                    update( YACSPrs_LabelPort* );

  void                    createChildNodesPresentations( YACS::HMI::SubjectComposedNode* );

  int                     arrangeNodes(YACS::ENGINE::Bloc* theBloc);
  int                     arrangeNodesWithinBloc(YACS::ENGINE::Bloc* theBloc);
  void                    createGraphvizNodes(YACS::ENGINE::Bloc* theBloc, YACS::ENGINE::ComposedNode* theFather, Agraph_t* theGraph);

  void                    rebuildLinks();
  int                     addObjectToLine2dModel(YACSPrs_ElementaryNode* theNode,
						 LineConn2d_Model*       theLineModel,
						 std::map<YACSPrs_ElementaryNode*, int>& theNodePrs2ObjId);
  int                     addPortToLine2dModel(YACSPrs_Port*           thePort,
					       YACSPrs_ElementaryNode* theNode,
					       YACSPrs_Link*           theLink,
					       LineConn2d_Model*       theLineModel,
					       std::map<YACSPrs_ElementaryNode*, int>& theNodePrs2ObjId,
					       std::map<YACSPrs_Port*, int>&           thePortPrs2PortId);
  int                     addPortToLine2dModel(YACSPrs_Hook*           theHook,
					       YACSPrs_ElementaryNode* theNode,
					       YACSPrs_LabelLink*      theLink,
					       LineConn2d_Model*       theLineModel,
					       std::map<YACSPrs_ElementaryNode*, int>& theNodePrs2ObjId);

  YACSPrs_ElementaryNode* getItem( YACS::ENGINE::Node* );

  YACS::ENGINE::Node*     getNodeById( const int theID ) const;

  YACS::ENGINE::Node*     getNodeByName( const std::string theName ) const;

  void                    getAllBlocChildren(YACS::ENGINE::Bloc*, 
					     std::set<YACS::ENGINE::Node*>&);
  void                    getAllComposedNodeChildren(YACS::ENGINE::ComposedNode*, 
						     std::set<YACS::ENGINE::Node*>&);

  void                    registerStatusObserverWithNode(YACS::ENGINE::Node* theNode);

private:
  YACSGui_Node*           driver( YACS::ENGINE::Node* theNode );
  void                    createLinksFromGivenOutPortPrs( YACSPrs_InOutPort* theOutPortPrs,
							  std::set<YACS::ENGINE::InPort *> theInPorts );
  
private:

  typedef std::map<const char*, YACSGui_Node*> DriverMap;
  typedef std::map<YACS::ENGINE::Node*, YACSPrs_ElementaryNode*> ItemMap;
  
  YACSGui_Module*         myModule;
  YACS::HMI::GuiContext*  myCProc; // context of corresponding Proc*
  DriverMap               myDrivers; // map of update drivers for specific node types
  ItemMap                 myItems; // map of graphic items for a given engine node

  YACSGui_Observer*       myNodeStatusObserver;
};

#endif
