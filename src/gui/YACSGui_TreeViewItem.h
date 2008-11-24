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

#ifndef YACSGui_TreeViewItem_HeaderFile
#define YACSGui_TreeViewItem_HeaderFile

#include <qlistview.h>

#include <Port.hxx>
#include <ElementaryNode.hxx>
#include <ServiceNode.hxx>
#include <Proc.hxx>
#include <TypeCode.hxx>
#include <SalomeContainer.hxx>
#include <ComponentInstance.hxx>
#include <guiObservers.hxx>

/* ================ items for tree view in edition mode ================ */

class YACSGui_Executor;

/*!
 * YACS tree view item
 */
class YACSGui_ViewItem : public QListViewItem, public YACS::HMI::GuiObserver
{
public:
  YACSGui_ViewItem( QListView* theParent, 
		    QListViewItem* theAfter );
  YACSGui_ViewItem( QListViewItem* theParent, 
		    QListViewItem* theAfter );
  virtual ~YACSGui_ViewItem();
  
  virtual void select(bool isSelected);
  //virtual void update(YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son);

  // axiluary methods for blocking reaction inside select() method
  bool blockSelection( const bool toBlock );
  bool selectionBlocked() const;

  virtual void update( const bool theIsRecursive = false ) {}

protected:
  void removeNodeItem( YACS::HMI::Subject* theSNode );
  void removeLinkItem( YACS::HMI::Subject* theSLink );

protected:
  bool myBlockSelect;
};

/*!
 * YACS label tree view item
 */
class YACSGui_LabelViewItem : public YACSGui_ViewItem
{
public:
  YACSGui_LabelViewItem( QListView* theParent, 
			 QListViewItem* theAfter, 
			 const QString theName );
  YACSGui_LabelViewItem( QListViewItem* theParent, 
			 QListViewItem* theAfter, 
			 const QString theName );
  virtual ~YACSGui_LabelViewItem();

  virtual void select( bool isSelected );
  
  QString name() const;
  QPixmap icon() const;

  virtual void update( const bool theIsRecursive = false );
  
protected:
  QString myName;
};

/*!
 * YACS reference tree view item
 */
class YACSGui_ReferenceViewItem : public YACSGui_ViewItem
{
public:
  typedef enum { Text, HighlightedText, Highlight } ColorRole;

  YACSGui_ReferenceViewItem( QListView* theParent, 
			     QListViewItem* theAfter,
			     YACS::HMI::SubjectReference* theSReference );
  YACSGui_ReferenceViewItem( QListViewItem* theParent, 
			     QListViewItem* theAfter,
			     YACS::HMI::SubjectReference* theSReference );
  virtual ~YACSGui_ReferenceViewItem();
  
  virtual void select(bool isSelected);
  virtual void update(YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son);

  QString name() const;
  QPixmap icon() const;
  QColor  color( const ColorRole = Text ) const;
  
  YACS::ENGINE::Node* getNode() const;
  YACS::HMI::SubjectReference* getSReference() const { return mySReference; }

  virtual void paintCell( QPainter* p, const QColorGroup& cg, int c, int w, int align );

  virtual void update( const bool theIsRecursive = false );
    
private:
  YACS::HMI::SubjectReference* mySReference;
};

/*!
 * YACS tree view item for port
 */
class YACSGui_PortViewItem : public YACSGui_ViewItem
{
public:
  YACSGui_PortViewItem( QListView* theParent, 
			QListViewItem* theAfter,
			YACS::HMI::SubjectDataPort* theSPort );
  YACSGui_PortViewItem( QListViewItem* theParent, 
			QListViewItem* theAfter,
			YACS::HMI::SubjectDataPort* theSPort );
  virtual ~YACSGui_PortViewItem();

  QString name() const;
  QPixmap icon() const;

  YACS::ENGINE::DataPort* getPort() const;
  YACS::HMI::SubjectDataPort* getSPort() const { return mySPort; }

  virtual void update( const bool theIsRecursive = false );
    
private:
  YACS::HMI::SubjectDataPort* mySPort;
};

/*!
 * YACS tree view item for node
 */
class YACSGui_NodeViewItem : public YACSGui_ViewItem
{
public:
  typedef enum { Text, HighlightedText, Highlight } ColorRole;

  YACSGui_NodeViewItem( QListView* theParent, 
			QListViewItem* theAfter,
			YACS::HMI::SubjectNode* theSNode );
  YACSGui_NodeViewItem( QListViewItem* theParent, 
			QListViewItem* theAfter,
			YACS::HMI::SubjectNode* theSNode );
  virtual ~YACSGui_NodeViewItem();
  
  virtual void update(YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son);

  QString name() const;
  QPixmap icon() const;

  static QPixmap icon( YACS::ENGINE::Node* );
  
  YACS::ENGINE::Node* getNode() const;
  YACS::HMI::SubjectNode* getSNode() const { return mySNode; }

  virtual void update( const bool theIsRecursive = false );

private:
  void renamePortItem( YACS::HMI::Subject* theSPort );
  void addPortItem( YACS::HMI::Subject* theSPort );
  void removePortItem( YACS::HMI::Subject* theSPort );
  void addNodeItem( YACS::HMI::Subject* theSNode );
  void moveUpPortItem( YACS::HMI::Subject* theSPort );
  void moveDownPortItem( YACS::HMI::Subject* theSPort );
  bool isPublished( YACS::ENGINE::Port* thePort );

  void addReferenceItem( YACS::HMI::Subject* theSRef );
  void removeReferenceItem( YACS::HMI::Subject* theSRef );

  void addLinkItem( YACS::HMI::Subject* theSLink );

  YACS::HMI::SubjectNode* mySNode;
};

/*!
 * YACS tree view item for data link
 */
class YACSGui_LinkViewItem : public YACSGui_ViewItem
{
public:
  YACSGui_LinkViewItem( QListView* theParent, 
			QListViewItem* theAfter,
			YACS::HMI::SubjectLink* theSLink );
  YACSGui_LinkViewItem( QListViewItem* theParent, 
			QListViewItem* theAfter,
			YACS::HMI::SubjectLink* theSLink );
  virtual ~YACSGui_LinkViewItem();
  
  QString name() const;
  QPixmap icon() const;

  YACS::HMI::SubjectLink* getSLink() const { return mySLink; }

  virtual void update( const bool theIsRecursive = false );
    
private:
  YACS::HMI::SubjectLink* mySLink;
};

/*!
 * YACS tree view item for control link
 */
class YACSGui_ControlLinkViewItem : public YACSGui_ViewItem
{
public:
  YACSGui_ControlLinkViewItem( QListView* theParent, 
			       QListViewItem* theAfter,
			       YACS::HMI::SubjectControlLink* theSLink );
  YACSGui_ControlLinkViewItem( QListViewItem* theParent, 
			       QListViewItem* theAfter,
			       YACS::HMI::SubjectControlLink* theSLink );
  virtual ~YACSGui_ControlLinkViewItem();
  
  QString name() const;
  QPixmap icon() const;

  YACS::HMI::SubjectControlLink* getSLink() const { return mySLink; }

  virtual void update( const bool theIsRecursive = false );
    
private:
  YACS::HMI::SubjectControlLink* mySLink;
};

/*!
 * YACS tree view item for schema
 */
class YACSGui_SchemaViewItem : public YACSGui_ViewItem
{
public:
  YACSGui_SchemaViewItem( QListView* theParent, 
			  QListViewItem* theAfter, 
			  YACS::HMI::SubjectProc* theSProc );
  YACSGui_SchemaViewItem( QListViewItem* theParent,
			  QListViewItem* theAfter,
			  YACS::HMI::SubjectProc* theSProc );
  virtual ~YACSGui_SchemaViewItem();
  
  virtual void update(YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son);
  
  QPixmap icon() const;
  
  YACS::ENGINE::Proc* getProc() const;
  YACS::HMI::SubjectProc* getSProc() const { return mySProc; }

  virtual void update( const bool theIsRecursive = false,
		       YACS::HMI::Subject* theSon = 0,
		       YACSGui_NodeViewItem* theBlocItem = 0 );

  YACSGui_LabelViewItem* buildDataTypesTree();

private:
  void addNodeItem( YACS::HMI::Subject* theSNode );
  void addContainerItem( YACS::HMI::Subject* theSContainer );
  void removeContainerItem( YACS::HMI::Subject* theSContainer );
  void removeComponentItem( YACS::HMI::Subject* theSComponent );
  void addDataTypeItem( YACS::HMI::Subject* theSDataType );
  void removeDataTypeItem( YACS::HMI::Subject* theSDataType );

  void addLinkItem( YACS::HMI::Subject* theSLink );
  
  YACS::HMI::SubjectProc* mySProc;

  std::map< YACS::ENGINE::DynType, QListViewItem* > myRootDataTypeItems;

};

class YACSGui_ComponentViewItem;
/*!
 * YACS tree view item for container
 */
class YACSGui_ContainerViewItem : public YACSGui_ViewItem
{
public:
  YACSGui_ContainerViewItem(  QListView* theParent, 
			      QListViewItem* theAfter,
			      YACS::HMI::SubjectContainer* theSContainer = 0 );
  YACSGui_ContainerViewItem(  QListViewItem* theParent, 
			      QListViewItem* theAfter,
			      YACS::HMI::SubjectContainer* theSContainer = 0 );
  virtual ~YACSGui_ContainerViewItem();

  virtual void update(YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son);

  QString name() const;
  QPixmap icon() const;  
  
  YACS::ENGINE::Container* getContainer() const;
  YACS::HMI::SubjectContainer* getSContainer() const { return mySContainer; }

  virtual void update( YACSGui_ComponentViewItem* theComponent = 0 );
  
  void addComponentItem( YACS::HMI::Subject* theSComponent );
  
private:
  void removeComponentItem( YACS::HMI::Subject* theSComponent );

  YACS::HMI::SubjectContainer* mySContainer;
};

/*!
 * YACS tree view item for component
 */
class YACSGui_ComponentViewItem : public YACSGui_ViewItem
{
public:
  YACSGui_ComponentViewItem( QListView* theParent, 
			     QListViewItem* theAfter,
			     YACS::HMI::SubjectComponent* theSComponent = 0 );
  YACSGui_ComponentViewItem( QListViewItem* theParent, 
			     QListViewItem* theAfter,
			     YACS::HMI::SubjectComponent* theSComponent = 0 );

  virtual ~YACSGui_ComponentViewItem();

  virtual void update(YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son);
  
  QString name() const;
  QString instanceName() const;
  QPixmap icon() const;
  
  YACS::ENGINE::ComponentInstance* getComponent() const;
  YACS::HMI::SubjectComponent* getSComponent() const { return mySComponent; }

  virtual void update( const bool theMove = false );
  virtual void move( YACS::HMI::Subject* theSReference );
    
private:
  YACS::HMI::SubjectComponent* mySComponent;
};

/*!
 * YACS tree view item for data type
 */
class YACSGui_DataTypeItem : public YACSGui_ViewItem
{
public:
  //typedef enum { Text, HighlightedText, Highlight } ColorRole;

  YACSGui_DataTypeItem( QListView* theParent, 
			QListViewItem* theAfter,
			YACS::HMI::SubjectDataType* theSDataType );
  YACSGui_DataTypeItem( QListViewItem* theParent, 
			QListViewItem* theAfter,
			YACS::HMI::SubjectDataType* theSDataType );
  virtual ~YACSGui_DataTypeItem();
  
  virtual void update(YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son);

  QString name() const;
  static QPixmap icon();
  
  YACS::HMI::SubjectDataType* getSDataType() const { return mySDataType; }

  virtual void update( const bool theIsRecursive = false );

private:
  YACS::HMI::SubjectDataType* mySDataType;
};

/* ================ items for tree view in run mode ================ */


class YACSGui_ComposedNodeViewItem: public YACSGui_ViewItem
{
 public:
  YACSGui_ComposedNodeViewItem(QListView *parent,
			       QString label,
			       YACS::ENGINE::ComposedNode *node = 0);
  YACSGui_ComposedNodeViewItem(QListViewItem *parent,
			       QString label,
			       YACS::ENGINE::ComposedNode *node = 0);
  virtual ~YACSGui_ComposedNodeViewItem();

  void setState(int state);
  void setStatus(int status);
  virtual void paintCell( QPainter *p, const QColorGroup &cg,
                          int column, int width, int alignment );
  QColor statusBgColor() const;
  YACS::ENGINE::ComposedNode* getNode() {return _node;};

  QPixmap icon() const;

  void update( const bool theIsRecursive = false );

  YACS::HMI::Subject* getSubject() const;
  void popup(YACSGui_Executor* anExecutor,const QPoint & point);

 protected:
  int _state;
  QColor _cf;
  YACS::ENGINE::ComposedNode *_node;
};

class YACSGui_ElementaryNodeViewItem: public QCheckListItem,
                                      public YACS::HMI::GuiObserver
{
 public:
  YACSGui_ElementaryNodeViewItem(QListView *parent,
				 const QString &text,
				 Type tt = RadioButtonController,
				 YACS::ENGINE::ElementaryNode *node = 0);
  YACSGui_ElementaryNodeViewItem(QListViewItem *parent,
				 const QString &text,
				 Type tt = RadioButtonController,
				 YACS::ENGINE::ElementaryNode *node = 0);
  virtual ~YACSGui_ElementaryNodeViewItem();
  
  void setState(int state); 
  virtual void paintCell( QPainter *p, const QColorGroup &cg,
                          int column, int width, int alignment );
  YACS::ENGINE::ElementaryNode* getNode() {return _node;};

  virtual void select(bool isSelected);

  void update( const bool theIsRecursive = false );

  // axiluary methods for blocking reaction inside select() method
  bool blockSelection( const bool toBlock );
  bool selectionBlocked() const;

  YACS::HMI::Subject* getSubject() const;
  void popup(YACSGui_Executor* anExecutor,const QPoint & point);
  
 protected:
  int _state;
  QColor _cf;
  YACS::ENGINE::ElementaryNode *_node;

  bool myBlockSelect;
};

#endif
