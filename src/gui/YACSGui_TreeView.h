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

#ifndef YACSGui_TreeView_HeaderFile
#define YACSGui_TreeView_HeaderFile

#include <YACSGui_Module.h>

#include <ServiceNode.hxx>
#include <Proc.hxx>
#include <SalomeContainer.hxx>
#include <guiObservers.hxx>

#include <qlistview.h>

class YACSGui_NodeViewItem;

class QPopupMenu;

/*! Base class for tree view widgets.
 */
class YACSGui_TreeView: public QListView
{
  Q_OBJECT
    
 public:
  YACSGui_TreeView( YACSGui_Module*, YACS::HMI::SubjectProc*, QWidget* = 0,
		    const char* = 0, WFlags = 0 );
  ~YACSGui_TreeView();

  // get the list of selected objects
  std::list<QListViewItem*> getSelected();

  YACS::ENGINE::Proc* getProc() const;

  virtual void update( QListViewItem* theItem = 0, const bool theIsRecursive = false );

  virtual void build(); //temporary solution, for testing only

  virtual void syncPageTypeWithSelection() {}

 protected slots:
  virtual void onContextMenuRequested( QListViewItem*, const QPoint& ) {}

 protected:
  enum { SchemaItem, NodeItem };

  //virtual void build();
  virtual QPopupMenu* contextMenuPopup( const int ) {}


 protected:
  YACSGui_Module* myModule;
  YACS::HMI::SubjectProc* mySProc; 
};

/*! Class for tree view widget in edition mode.
 */
class YACSGui_EditionTreeView: public YACSGui_TreeView
{
  Q_OBJECT

 public:
  YACSGui_EditionTreeView( YACSGui_Module*, YACS::HMI::SubjectProc*, QWidget* = 0,
			   const char* = 0, WFlags = 0 );
  ~YACSGui_EditionTreeView();

  virtual void update( QListViewItem* theItem = 0, const bool theIsRecursive = false );

  virtual void build(); //temporary solution, for testing only

  virtual void syncPageTypeWithSelection();

  YACSGui_NodeViewItem* displayNodeWithPorts( QListViewItem* theRoot,
					      YACSGui_NodeViewItem* theAfter,
					      YACS::HMI::SubjectNode* theSNode );
  void displayChildren( YACSGui_NodeViewItem* theNodeItem );

 public slots:
  void onCreateDataType();
  void onAddToLibrary();

  void onCopyItem();
  void onPasteItem();
  void onDeleteItem();

  void onDisplaySchema();
  void onEraseSchema();

  void onDisplayNode();
  void onEraseNode();

 protected slots:
  virtual void onContextMenuRequested( QListViewItem*, const QPoint& );

 protected:
 enum { ServiceNodeItem = YACSGui_TreeView::NodeItem+1, LoopNodeItem, PortItem,
	LinkItem, ContainerItem, ComponentItem, CorbaComponentItem };

  //virtual void build();
  void fillContainerData( YACS::HMI::SubjectComposedNode* theSNode );
  virtual QPopupMenu* contextMenuPopup( const int );
  void showPopup( QPopupMenu*, const QPoint );

 private slots:
  void onSelectionChanged();
  void onDblClick( QListViewItem* );
  void sampleSlot() {}// for testing only!

 private:
  typedef std::map< YACS::ENGINE::ComponentInstance*, std::list<YACS::HMI::SubjectServiceNode*> > Component2ServiceNodesMap;
  typedef std::map< YACS::ENGINE::SalomeContainer*, Component2ServiceNodesMap > Container2ComponentsMap;

  Container2ComponentsMap mySalomeComponentData;
  Component2ServiceNodesMap myCorbaComponentData;
};

/*! Class for tree view widget in run mode.
 */
class YACSGui_RunTreeView: public YACSGui_TreeView
{
  Q_OBJECT

 public:
  YACSGui_RunTreeView( YACSGui_Module*, YACS::HMI::SubjectProc*, QWidget* = 0,
		       const char* = 0, WFlags = 0 );
  ~YACSGui_RunTreeView();

  virtual void update( QListViewItem* theItem = 0, const bool theIsRecursive = false );
  virtual void update( QCheckListItem* theItem, const bool theIsRecursive = false );

  virtual void build(); //temporary solution, for testing only

  virtual void syncPageTypeWithSelection();

 public slots:
   virtual void onNotifyNodeStatus( int theNodeId, int theStatus );
  virtual void onNotifyStatus( int theStatus );

 protected:
  //virtual void build();
  void addTreeNode( QListViewItem* theParent,
		    YACS::ENGINE::ComposedNode* theFather );

 private slots:
  void onSelectionChanged();
  virtual void onBreakpointClicked( QListViewItem* theItem );

 private:
  std::set<int>                 myBreakpointSet;
  std::map<int, QListViewItem*> myMapListViewItem;
};

#endif
