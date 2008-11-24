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

class YACSGui_Graph : public QxGraph_Prs, public YACS::HMI::GuiObserver
{
public:
  enum { FullId = 0, ControlId, DataflowId, DataStreamId };

public:
  YACSGui_Graph(YACSGui_Module*, QxGraph_Canvas*, YACS::HMI::GuiContext*);
  virtual ~YACSGui_Graph();

  virtual void select(bool isSelected);
  virtual void update(YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son);

  YACS::ENGINE::Proc*     getProc() const;
  YACS::HMI::GuiContext*  getContext() const { return myCProc; }
  
  YACSGui_Observer*       getStatusObserver() const { return myNodeStatusObserver; }

  virtual void            update();
  void                    update( YACS::ENGINE::Node*, YACS::HMI::SubjectComposedNode* );
  void                    update( YACSPrs_InOutPort* );
  void                    update( YACSPrs_LabelPort* );

  virtual void            show( bool theWithArrange = true );
  void                    viewModesConsistency( int theDModeFrom, int theDModeTo );

  void                    createChildNodesPresentations( YACS::HMI::SubjectComposedNode* );

  void                    updateNodePrs( int theNodeId, bool input,std::string thePortName, std::string thePortValue );

  int                     arrangeNodesAlgo(YACS::ENGINE::Bloc* theBloc);
  int                     arrangeNodes(YACS::ENGINE::ComposedNode* theBloc, Agraph_t* aSubGraph, int dep);
  void                    createGraphvizNodes(YACS::ENGINE::ComposedNode* theBloc, Agraph_t* aSubGraph);
  void                    arrangeCanvasNodes(YACS::ENGINE::ComposedNode* theBloc, Agraph_t* aSubGraph, int dep);
  // store some functions to have possibility to arrange nodes only on the one giving level
  int                     arrangeNodesWithinBloc(YACS::ENGINE::Bloc* theBloc);
  void                    createGraphvizNodes(YACS::ENGINE::Bloc* theBloc, YACS::ENGINE::ComposedNode* theFather, Agraph_t* theGraph);
  std::string             getInNodeName(YACS::ENGINE::Bloc* theBloc, YACS::ENGINE::Node* theOutNode, YACS::ENGINE::Node* theInNode);

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
  void                    removeNode( YACS::ENGINE::Node* );
  void                    updatePrs();

  YACS::ENGINE::Node*     getNodeById( const int theID ) const;

  YACS::ENGINE::Node*     getNodeByName( const std::string theName ) const;

  void                    getAllBlocChildren(YACS::ENGINE::Bloc*, 
                                             std::set<YACS::ENGINE::Node*>&);
  void                    getAllComposedNodeChildren(YACS::ENGINE::ComposedNode*, 
                                                     std::set<YACS::ENGINE::Node*>&);

  void                    registerStatusObserverWithNode(YACS::ENGINE::Node* theNode);

  void                    createPrs(YACS::HMI::Subject* theSubject);
  void                    deletePrs(YACS::HMI::SubjectNode* theSubject, bool removeLabelPort = true );
  void                    cutPrs(YACS::HMI::SubjectNode* theSubject);
  void                    pastePrs(YACS::HMI::SubjectNode* theSubject);
  virtual void            decrementSubjects(YACS::HMI::Subject *subject);

  bool                    isNeededToIncreaseBlocSize( YACS::ENGINE::Bloc* );

private:
  YACSGui_Node*           driver( YACS::ENGINE::Node* theNode );
  void                    createLinksFromGivenOutPortPrs( YACSPrs_InOutPort* theOutPortPrs,
                                                          std::set<YACS::ENGINE::InPort *> theInPorts );
  
private:

  typedef std::map<const char*, YACSGui_Node*> DriverMap;
  typedef std::map<int, std::map<YACS::ENGINE::Node*, YACSPrs_ElementaryNode*> > ItemMap;
  typedef std::map<YACS::ENGINE::Bloc*, std::list< std::pair<YACS::ENGINE::Bloc*,YACS::ENGINE::Bloc*> > > Bloc2InsideLinksMap;
  
  YACSGui_Module*         myModule;
  YACS::HMI::GuiContext*  myCProc; // context of corresponding Proc*
  DriverMap               myDrivers; // map of update drivers for specific node types
  ItemMap                 myItems; // map of graphic items for a given engine node
  Bloc2InsideLinksMap     myBlocInsideLinks; // map of links from block to block ( BlocOut -> BlocIn ) inside a given block
  std::list<YACSPrs_ElementaryNode*>             nodesToDelete;

  YACSGui_Observer*       myNodeStatusObserver;
  Agraph_t*               _mainGraph;
  std::multimap< YACS::ENGINE::Node*, YACS::ENGINE::Node* > _savedControlLinks;
  int _maxdep;
  int _bottom;
  std::string _format;
};

#endif
