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
#include <SalomeContainer.hxx>
#include <ComponentInstance.hxx>
#include <guiObservers.hxx>

/* ================ items for tree view in edition mode ================ */

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
  
  //virtual void select(bool isSelected);
  //virtual void update(YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son);

  virtual void update( const bool theIsRecursive = false ) {}
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
			YACS::ENGINE::Port* thePort );
  YACSGui_PortViewItem( QListViewItem* theParent, 
			QListViewItem* theAfter,
			YACS::ENGINE::Port* thePort );
  
  QString name() const;
  QPixmap icon() const;

  YACS::ENGINE::Port* getPort() const { return myPort; }

  virtual void update( const bool theIsRecursive = false );
    
private:
  YACS::ENGINE::Port* myPort;
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
  
  virtual void select(bool isSelected);
  virtual void update(YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son);

  QString name() const;
  QPixmap icon() const;
  
  YACS::ENGINE::Node* getNode() const;
  YACS::HMI::SubjectNode* getSNode() const { return mySNode; }

  virtual void update( const bool theIsRecursive = false );
    
private:
  void renamePortItem( YACS::HMI::Subject* theSPort );
  void addPortItem( YACS::HMI::Subject* theSPort );
  void removePortItem( YACS::HMI::Subject* theSPort );
  void removeNodeItem( YACS::HMI::Subject* theSNode );
  void moveUpPortItem( YACS::HMI::Subject* theSPort );
  void moveDownPortItem( YACS::HMI::Subject* theSPort );
  bool isPublished( YACS::ENGINE::Port* thePort );

  void addReferenceItem( YACS::HMI::Subject* theSRef );

  YACS::HMI::SubjectNode* mySNode;
};

/*!
 * YACS tree view item for link
 */
class YACSGui_LinkViewItem : public YACSGui_ViewItem
{
public:
  YACSGui_LinkViewItem( QListView* theParent, 
			QListViewItem* theAfter,
			YACS::ENGINE::Port* theOutPort,
			YACS::ENGINE::Port* theInPort );
  YACSGui_LinkViewItem( QListViewItem* theParent, 
			QListViewItem* theAfter,
			YACS::ENGINE::Port* theOutPort,
			YACS::ENGINE::Port* theInPort );
  
  QString name() const;
  QPixmap icon() const;

  YACS::ENGINE::Port* getOutPort() const { return myOutPort; }
  YACS::ENGINE::Port* getInPort() const { return myInPort; }

  virtual void update( const bool theIsRecursive = false );
    
private:
  YACS::ENGINE::Port* myOutPort;
  YACS::ENGINE::Port* myInPort;
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
  
  virtual void select(bool isSelected);
  virtual void update(YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son);
  
  QPixmap icon() const;
  
  YACS::ENGINE::Proc* getProc() const;
  YACS::HMI::SubjectProc* getSProc() const { return mySProc; }

  virtual void update( const bool theIsRecursive = false,
		       YACS::HMI::Subject* theSon = 0,
		       YACSGui_NodeViewItem* theBlocItem = 0 );
    
private:
  void addNodeItem( YACS::HMI::Subject* theSNode );
  void addContainerItem( YACS::HMI::Subject* theSContainer );
  
  YACS::HMI::SubjectProc* mySProc;
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

  virtual void select(bool isSelected);
  virtual void update(YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son);

  QString name() const;
  QPixmap icon() const;  
  
  YACS::ENGINE::Container* getContainer() const;
  YACS::HMI::SubjectContainer* getSContainer() const { return mySContainer; }

  virtual void update( YACSGui_ComponentViewItem* theComponent = 0 );
    
private:
  void addComponentItem( YACS::HMI::Subject* theSComponent );

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

  virtual void select(bool isSelected);
  virtual void update(YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son);
  
  QString name() const;
  QPixmap icon() const;
  
  YACS::ENGINE::ComponentInstance* getComponent() const;
  YACS::HMI::SubjectComponent* getSComponent() const { return mySComponent; }

  virtual void update( const bool theMove = false );
  virtual void move( YACS::HMI::Subject* theSReference );
    
private:
  YACS::HMI::SubjectComponent* mySComponent;
};

/* ================ items for tree view in run mode ================ */

class YACSGui_ComposedNodeViewItem: public QListViewItem
{
 public:
  YACSGui_ComposedNodeViewItem(QListView *parent,
			       QString label,
			       YACS::ENGINE::ComposedNode *node = 0);
  YACSGui_ComposedNodeViewItem(QListViewItem *parent,
			       QString label,
			       YACS::ENGINE::ComposedNode *node = 0);

  void setState(int state);
  void setStatus(int status);
  virtual void paintCell( QPainter *p, const QColorGroup &cg,
                          int column, int width, int alignment );
  QColor statusBgColor() const;
  YACS::ENGINE::ComposedNode* getNode() {return _node;};

  QPixmap icon() const;
  
  void update( const bool theIsRecursive = false );

 protected:
  int _state;
  QColor _cf;
  YACS::ENGINE::ComposedNode *_node;
};

class YACSGui_ElementaryNodeViewItem: public QCheckListItem
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
  
  void setState(int state); 
  virtual void paintCell( QPainter *p, const QColorGroup &cg,
                          int column, int width, int alignment );
  YACS::ENGINE::ElementaryNode* getNode() {return _node;};

  void update( const bool theIsRecursive = false );
  
 protected:
  int _state;
  QColor _cf;
  YACS::ENGINE::ElementaryNode *_node;
};

#endif
