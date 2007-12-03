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

#include <YACSGui_TreeViewItem.h>
#include <YACSGui_TreeView.h>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>

#include <InPort.hxx>
#include <OutPort.hxx>

#include <OutGate.hxx>
#include <OutputDataStreamPort.hxx>

#include <Loop.hxx>
#include <ForEachLoop.hxx>
#include <Switch.hxx>
#include <ComposedNode.hxx>

#include <qpainter.h>
#include <qpalette.h>

using namespace YACS::ENGINE;
using namespace YACS::HMI;

/* XPM */
static const char * const closed_xpm[]={
    "16 16 6 1",
    ". c None",
    "b c #ffff00",
    "d c #000000",
    "* c #999999",
    "a c #cccccc",
    "c c #ffffff",
    "................",
    "................",
    "..*****.........",
    ".*ababa*........",
    "*abababa******..",
    "*cccccccccccc*d.",
    "*cbababababab*d.",
    "*cabababababa*d.",
    "*cbababababab*d.",
    "*cabababababa*d.",
    "*cbababababab*d.",
    "*cabababababa*d.",
    "*cbababababab*d.",
    "**************d.",
    ".dddddddddddddd.",
    "................"};

/* XPM */
static const char* const open_xpm[]={
    "16 16 6 1",
    ". c None",
    "b c #ffff00",
    "d c #000000",
    "* c #999999",
    "c c #cccccc",
    "a c #ffffff",
    "................",
    "................",
    "...*****........",
    "..*aaaaa*.......",
    ".*abcbcba******.",
    ".*acbcbcaaaaaa*d",
    ".*abcbcbcbcbcb*d",
    "*************b*d",
    "*aaaaaaaaaa**c*d",
    "*abcbcbcbcbbd**d",
    ".*abcbcbcbcbcd*d",
    ".*acbcbcbcbcbd*d",
    "..*acbcbcbcbb*dd",
    "..*************d",
    "...ddddddddddddd",
    "................"};


/* ================ items for tree view in edition mode ================ */

// YACSGui_ViewItem class:

YACSGui_ViewItem::YACSGui_ViewItem( QListView* theParent, 
				    QListViewItem* theAfter ):
  QListViewItem( theParent, theAfter ),
  GuiObserver()
{
}

YACSGui_ViewItem::YACSGui_ViewItem( QListViewItem* theParent, 
				    QListViewItem* theAfter ):
  QListViewItem( theParent, theAfter ),
  GuiObserver()
{
}

// YACSGui_LabelViewItem class:

YACSGui_LabelViewItem::YACSGui_LabelViewItem( QListView* theParent, 
					      QListViewItem* theAfter, 
					      const QString theName ):
  YACSGui_ViewItem( theParent, theAfter ),
  myName( theName )
{
  if ( !myName.isEmpty() )
    setText( 0, myName );
  setPixmap( 0, icon() );
}

YACSGui_LabelViewItem::YACSGui_LabelViewItem( QListViewItem* theParent, 
					      QListViewItem* theAfter, 
					      const QString theName ):
  YACSGui_ViewItem( theParent, theAfter ),
  myName( theName )
{
  if ( !myName.isEmpty() )
    setText( 0, myName );
  setPixmap( 0, icon() );
}
 
QString YACSGui_LabelViewItem::name() const
{
  return myName;
}

QPixmap YACSGui_LabelViewItem::icon() const
{
  if ( isOpen() )
    return QPixmap( (const char**) open_xpm );
  return QPixmap( (const char**) closed_xpm );
}

void YACSGui_LabelViewItem::update( const bool theIsRecursive )
{

}

// YACSGui_ReferenceViewItem class:

YACSGui_ReferenceViewItem::YACSGui_ReferenceViewItem( QListView* theParent, 
						      QListViewItem* theAfter,
						      YACS::HMI::SubjectReference* theSReference ):
  YACSGui_ViewItem( theParent, theAfter ),
  mySReference( theSReference )
{
  if ( mySReference ) mySReference->attach(this);
  
  QString aName = name();
  if ( !aName.isEmpty() )
    setText( 0, aName );
  setPixmap( 0, icon() );
}

YACSGui_ReferenceViewItem::YACSGui_ReferenceViewItem( QListViewItem* theParent, 
					    QListViewItem* theAfter,
					    YACS::HMI::SubjectReference* theSReference ):
  YACSGui_ViewItem( theParent, theAfter ),
  mySReference( theSReference )
{
  if ( mySReference ) mySReference->attach(this);

  QString aName = name();
  if ( !aName.isEmpty() )
    setText( 0, aName );
  setPixmap( 0, icon() );
}

YACSGui_ReferenceViewItem::~YACSGui_ReferenceViewItem()
{
  if ( mySReference ) mySReference->detach(this);
}

void YACSGui_ReferenceViewItem::select(bool isSelected)
{
  printf(">> YACSGui_ReferenceViewItem::select\n");
}

void YACSGui_ReferenceViewItem::update(YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son)
{
  printf(">> YACSGui_ReferenceViewItem::update\n");
  switch (event)
  {
  case RENAME:
    update();
    break;
  default:
    GuiObserver::update(event, type, son);
  }
}
  
QString YACSGui_ReferenceViewItem::name() const
{
  QString aName;
  
  if (getNode())
    aName = QString("* ") + QString(getNode()->getRootNode()->getChildName(getNode()));

  return aName;
}

QColor YACSGui_ReferenceViewItem::color( const ColorRole theCR ) const
{
  QColor clr;
  switch ( theCR )
  {
  case Text:
  case Highlight:
    clr = QColor( 255, 0, 0 );
    break;
  case HighlightedText:
    clr = QColor( 255, 255, 255 );
    break;
  }
  return clr;
}

QPixmap YACSGui_ReferenceViewItem::icon() const
{
  return QPixmap();
}

YACS::ENGINE::Node* YACSGui_ReferenceViewItem::getNode() const
{
  Node* aRet = 0;

  if ( mySReference )
    if ( SubjectServiceNode* aSSN = dynamic_cast<SubjectServiceNode*>( mySReference->getParent() ) )
      aRet = aSSN->getNode();

  return aRet;
}

void YACSGui_ReferenceViewItem::paintCell( QPainter* p, const QColorGroup& cg, int c, int w, int align )
{
  QColorGroup col_group( cg );

  if ( color( YACSGui_ReferenceViewItem::Text ).isValid() )
    col_group.setColor( QColorGroup::Text, color( YACSGui_ReferenceViewItem::Text ) );
  if ( color( YACSGui_ReferenceViewItem::Highlight ).isValid() )
    col_group.setColor( QColorGroup::Highlight, color( YACSGui_ReferenceViewItem::Highlight ) );
  if ( color( YACSGui_ReferenceViewItem::HighlightedText ).isValid() )
    col_group.setColor( QColorGroup::HighlightedText, color( YACSGui_ReferenceViewItem::HighlightedText ) );

  p->fillRect( 0, 0, w, height(), cg.brush( QColorGroup::Base ) );

  int W = w;
  if ( listView() && !listView()->allColumnsShowFocus() )
    W = width( p->fontMetrics(), listView(), c );

  QListViewItem::paintCell( p, col_group, c, W<w ? W : w, align );
}

void YACSGui_ReferenceViewItem::update( const bool theIsRecursive )
{
  setText( 0, name() ); // only rename
}

// YACSGui_PortViewItem class:

YACSGui_PortViewItem::YACSGui_PortViewItem( QListView* theParent, 
					    QListViewItem* theAfter,
					    YACS::ENGINE::Port* thePort ):
  YACSGui_ViewItem( theParent, theAfter ),
  myPort( thePort )
{
  QString aName = name();
  if ( !aName.isEmpty() )
    setText( 0, aName );
  setPixmap( 0, icon() );
}

YACSGui_PortViewItem::YACSGui_PortViewItem( QListViewItem* theParent, 
					    QListViewItem* theAfter,
					    YACS::ENGINE::Port* thePort ):
  YACSGui_ViewItem( theParent, theAfter ),
  myPort( thePort )
{
  QString aName = name();
  if ( !aName.isEmpty() )
    setText( 0, aName );
  setPixmap( 0, icon() );
}
  
QString YACSGui_PortViewItem::name() const
{
  QString aName = "";

  DataPort* aDataPort = dynamic_cast<DataPort*>( myPort );
  if (aDataPort)
    aName = aDataPort->getName();
  else if (dynamic_cast<InGate*>( myPort ) || dynamic_cast<OutGate*>( myPort ))
    aName = QString("Gate");
  
  return aName;
}

QPixmap YACSGui_PortViewItem::icon() const
{
  QPixmap aRes;
  
  QString anIconName;

  if ( dynamic_cast<OutPort*>( myPort ) || dynamic_cast<OutGate*>( myPort ) )
    anIconName = QString(QObject::tr("ICON_OUT_PORT_OBJECT"));
  else if ( dynamic_cast<InPort*>( myPort ) || dynamic_cast<InGate*>( myPort ) )
    anIconName = QString(QObject::tr("ICON_IN_PORT_OBJECT"));

  if ( !anIconName.isEmpty() )
    aRes = SUIT_Session::session()->resourceMgr()->loadPixmap("YACSGui",  anIconName, false);
  
  return aRes;
}

void YACSGui_PortViewItem::update( const bool theIsRecursive )
{

}

// YACSGui_NodeViewItem class:

YACSGui_NodeViewItem::YACSGui_NodeViewItem( QListView* theParent, 
					    QListViewItem* theAfter,
					    YACS::HMI::SubjectNode* theSNode ):
  YACSGui_ViewItem( theParent, theAfter ),
  mySNode( theSNode )
{
  if ( mySNode ) mySNode->attach(this);

  QString aName = name();
  if ( !aName.isEmpty() )
    setText( 0, aName );
  setPixmap( 0, icon() );
}

YACSGui_NodeViewItem::YACSGui_NodeViewItem( QListViewItem* theParent, 
					    QListViewItem* theAfter,
					    YACS::HMI::SubjectNode* theSNode ):
  YACSGui_ViewItem( theParent, theAfter ),
  mySNode( theSNode )
{
  if ( mySNode ) mySNode->attach(this);

  QString aName = name();
  if ( !aName.isEmpty() )
    setText( 0, aName );
  setPixmap( 0, icon() );
}

YACSGui_NodeViewItem::~YACSGui_NodeViewItem()
{
  if ( mySNode ) mySNode->detach(this);
}

void YACSGui_NodeViewItem::select(bool isSelected)
{
  printf(">> YACSGui_NodeViewItem::select\n");
}

void YACSGui_NodeViewItem::update(YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son)
{
  printf(">> YACSGui_NodeViewItem::update\n");
  switch (event)
  {
  case RENAME:
    update();
    break;
  case EDIT:
    switch (type)
    {
    case INPUTPORT:
    case OUTPUTPORT:
    case INPUTDATASTREAMPORT:
    case OUTPUTDATASTREAMPORT:
      {
	// rename a port item (this = node item)
	printf("NodeViewItem:  EDIT port\n");
	renamePortItem(son);
      }
      break;
    default:
      {
	if ( !type )
	  update(true);
      }
      break;
    }
    break;
  case ADD:
    switch (type)
    {
    case INPUTPORT:
    case OUTPUTPORT:
    case INPUTDATASTREAMPORT:
    case OUTPUTDATASTREAMPORT:
      {
	// add a port item (this = node item)
	printf("NodeViewItem:  ADD port\n");
	addPortItem(son);
      }
      break;
    default:
      break;
    }
    break;
  case REMOVE:
    switch (type)
    {
    case INPUTPORT:
    case OUTPUTPORT:
    case INPUTDATASTREAMPORT:
    case OUTPUTDATASTREAMPORT:
      {
	// remove a port item (this = node item)
	printf("NodeViewItem:  REMOVE port\n");
	removePortItem(son);
      }
      break;
    default:
      {
	// remove a node inside a block (this = block item)
	printf("NodeViewItem:  REMOVE\n");
	YACSGui_NodeViewItem* aNode = 0;
	QListViewItem* aChild = firstChild();
	while( aChild )
	{
	  if ( aNode = dynamic_cast<YACSGui_NodeViewItem*>(aChild) )
	    if ( aNode->getSNode() == son )
	    {
	      printf(">> the son is found\n");
	      break;
	    }
	  
	  aChild = aChild->nextSibling();
	}
	
	if ( aNode )
	{
	  printf(">> delete the son item\n");
	  takeItem(aNode);
	  delete aNode;
	}
      }
      break;
    }
    break;
  case UP:
    switch (type)
    {
    case INPUTPORT:
    case OUTPUTPORT:
    case INPUTDATASTREAMPORT:
    case OUTPUTDATASTREAMPORT: 
      {
	// move up a port item (this = node item)
	printf("NodeViewItem:  UP port\n");
	moveUpPortItem(son);
      }
      break;
    default:
      break;
    }
    break;
  case DOWN:
    switch (type)
    {
    case INPUTPORT:
    case OUTPUTPORT:
    case INPUTDATASTREAMPORT:
    case OUTPUTDATASTREAMPORT: 
      {
	// move down a port item (this = node item)
	printf("NodeViewItem:  DOWN port\n");
	moveDownPortItem(son);
      }
      break;
    default:
      break;
    }
    break;
  default:
    GuiObserver::update(event, type, son);
  }
}
  
QString YACSGui_NodeViewItem::name() const
{
  QString aName;
  
  if (getNode())
    aName = QString(getNode()->getName());

  return aName;
}

QPixmap YACSGui_NodeViewItem::icon() const
{
  QPixmap aRes;

  QString anIconName;
    
  if ( dynamic_cast<Bloc*>( getNode() ) )
    anIconName = QString(QObject::tr("ICON_BLOCK_OBJECT"));
  else if ( dynamic_cast<Loop*>( getNode() ) || dynamic_cast<ForEachLoop*>( getNode() ) )
    anIconName = QString(QObject::tr("ICON_LOOP_NODE_OBJECT"));
  else if ( dynamic_cast<Switch*>( getNode() ) )
    anIconName = QString(QObject::tr("ICON_SWITCH_NODE_OBJECT"));
  else
    anIconName = QString(QObject::tr("ICON_NODE_OBJECT"));
  
  if ( !anIconName.isEmpty() )
    aRes = SUIT_Session::session()->resourceMgr()->loadPixmap("YACSGui", anIconName, false);
  
  return aRes;
}

YACS::ENGINE::Node* YACSGui_NodeViewItem::getNode() const
{
  return ( mySNode ? mySNode->getNode() : 0 );
}

void YACSGui_NodeViewItem::update( const bool theIsRecursive )
{
  if ( theIsRecursive ) // total update
  {
    ComposedNode* aComposedNode = dynamic_cast<ComposedNode*>( getNode() );
    
    std::list<SubjectNode*> aChildrenToMove;
    
    QListViewItem* aChild = firstChild();
    while( aChild )
    {
      if ( aComposedNode )
      { // this node is a block
	if ( YACSGui_NodeViewItem* aChildNode = dynamic_cast<YACSGui_NodeViewItem*>( aChild ) )
	{
	  if ( !aComposedNode->isInMyDescendance( aChildNode->getNode() ) )
	  { // a current child is not in the block now
	    aChildrenToMove.push_back( aChildNode->getSNode() );
	  }
	}
      }
      
      // remove old child objects of this view item (this case is suitable for the elementary nodes,
      // where we delete and recreate its ports)
      takeItem(aChild);
      delete aChild;
      
      aChild = firstChild();
    }
    
    if ( YACSGui_EditionTreeView* anETV = dynamic_cast<YACSGui_EditionTreeView*>(listView()) )
      anETV->displayChildren( this );

    if ( aComposedNode )
      if ( YACSGui_SchemaViewItem* aSchema = dynamic_cast<YACSGui_SchemaViewItem*>(listView()->firstChild()) )
      {
	//aSchema->update( true, 0, this );
	
	if ( !aChildrenToMove.empty() )
	{ 
	  // get the "Nodes" label view item, which is used as a parent for first level nodes
	  YACSGui_LabelViewItem* aNodesL = 
	    dynamic_cast<YACSGui_LabelViewItem*>(listView()->firstChild()->firstChild()->nextSibling());
	  if ( !aNodesL || aNodesL->text(0).compare(QString("Nodes")) ) return;
	
	  // find the last view item under "Nodes" label
	  YACSGui_NodeViewItem* aLast = 0;
	  QListViewItem* aChildL = aNodesL->firstChild();
	  while( aChildL )
	  {
	    if ( YACSGui_NodeViewItem* aChildLNodeItem = dynamic_cast<YACSGui_NodeViewItem*>(aChildL) )
	      aLast = aChildLNodeItem;
	    aChildL = aChildL->nextSibling();
	  }
	
	  if ( YACSGui_EditionTreeView* anETV = dynamic_cast<YACSGui_EditionTreeView*>(listView()) )
	  {
	    std::list<SubjectNode*>::iterator anIt = aChildrenToMove.begin();
	    for (; anIt != aChildrenToMove.end(); anIt++)
	    {
	      // create list view items after the last view item under "Nodes" label
	      aLast = anETV->displayNodeWithPorts( aNodesL, aLast, *anIt );
	    }
	  }

	}
      }
  }
  
  setText( 0, name() ); // only rename
}

void YACSGui_NodeViewItem::renamePortItem( YACS::HMI::Subject* theSPort )
{
  if ( SubjectDataPort* aSPort = dynamic_cast<SubjectDataPort*>(theSPort) )
  {
    YACSGui_PortViewItem* aPort = 0;
    QListViewItem* aChild = firstChild();
    while( aChild )
    {
      if ( aPort = dynamic_cast<YACSGui_PortViewItem*>(aChild) )
	if ( aPort->getPort() == aSPort->getPort() )
	{
	  printf(">> the son is found\n");
	  break;
	}
      
      aChild = aChild->nextSibling();
    }
    
    if ( aPort )
    {
      printf(">> rename the son item\n");
      aPort->setText( 0, aPort->name() );
    }
  }
}

void YACSGui_NodeViewItem::addPortItem( YACS::HMI::Subject* theSPort )
{
  if ( SubjectDataPort* aSPort = dynamic_cast<SubjectDataPort*>(theSPort) )
  {
    Port* aPort = aSPort->getPort();
    if ( !aPort ) return;

    YACSGui_PortViewItem* aPortItem = new YACSGui_PortViewItem( this, 0, aPort );
    if ( !aPortItem ) return;

    Port* anAfter = 0;
    // find the port, after which aPort is stored in the engine
    if ( dynamic_cast<InPort*>(aPort) )
    {
      std::list<InPort *> anInPorts = getNode()->getSetOfInPort();
      std::list<InPort *>::iterator InPortsIter = anInPorts.begin();
      for(; InPortsIter != anInPorts.end(); InPortsIter++)
      {
	if ( *InPortsIter == aPort ) break;
	anAfter = *InPortsIter;
      }
    }
    else if ( dynamic_cast<OutPort*>(aPort) )
    {
      std::list<OutPort *> anOutPorts = getNode()->getSetOfOutPort();
      std::list<OutPort *>::iterator OutPortsIter = anOutPorts.begin();
      for(; OutPortsIter != anOutPorts.end(); OutPortsIter++)
      {
	if ( *OutPortsIter == aPort ) break;
	anAfter = *OutPortsIter;
      }
    }
    
    // find the port, after which aPortItem should be stored in the tree
    YACSGui_PortViewItem* anAfterItem = 0;
    QListViewItem* aChild = firstChild();
    while( aChild )
    {
      if ( anAfterItem = dynamic_cast<YACSGui_PortViewItem*>(aChild) )
	if ( anAfterItem->getPort() == anAfter )
	{
	  printf(">> the son is found\n");
	  aPortItem->moveItem(anAfterItem);
	  break;
	}
      
      aChild = aChild->nextSibling();
    }
  }
}

void YACSGui_NodeViewItem::removePortItem( YACS::HMI::Subject* theSPort )
{
  if ( SubjectDataPort* aSPort = dynamic_cast<SubjectDataPort*>(theSPort) )
  {
    QListViewItem* aChild = firstChild();
    while( aChild )
    {
      if ( YACSGui_PortViewItem* aPort = dynamic_cast<YACSGui_PortViewItem*>(aChild) )
	if ( aPort->getPort() == aSPort->getPort() )
	{
	  printf(">> the son is found\n");
	  takeItem(aChild);
	  delete aChild;
	  break;
	}
      
      aChild = aChild->nextSibling();
    }
  }
}

void YACSGui_NodeViewItem::moveUpPortItem( YACS::HMI::Subject* theSPort )
{
  printf(">> 1\n");
  if ( SubjectDataPort* aSPort = dynamic_cast<SubjectDataPort*>(theSPort) )
  {
    printf(">> 2\n");
    QListViewItem* aChild = firstChild();
    while( aChild )
    {
      printf(">> 3 aChild->text(0) = %s\n",aChild->text(0).latin1());
      if ( YACSGui_PortViewItem* aPort = dynamic_cast<YACSGui_PortViewItem*>(aChild->nextSibling()) )
      {
	printf(">> 3.5\n");
	if ( aPort->getPort() == aSPort->getPort() )
	{
	  printf(">> the son is found\n");
	  break;
	}
      }
      
      aChild = aChild->nextSibling();
    }

    printf(">> 4\n");
    // here aChild is an after item for old position of theSPort
    if ( dynamic_cast<InPort*>(aSPort->getPort())
	 ||
	 aChild->text(0).compare( QString("Gate") ) )
      {
	printf(">> 5\n");
	aChild->moveItem(aChild->nextSibling());
      }
    printf(">> 6\n");
  }
}

void YACSGui_NodeViewItem::moveDownPortItem( YACS::HMI::Subject* theSPort )
{
  if ( SubjectDataPort* aSPort = dynamic_cast<SubjectDataPort*>(theSPort) )
  {
    QListViewItem* aChild = firstChild();
    while( aChild )
    {
      if ( YACSGui_PortViewItem* aPort = dynamic_cast<YACSGui_PortViewItem*>(aChild) )
	if ( aPort->getPort() == aSPort->getPort() )
	{
	  printf(">> the son is found\n");
	  break;
	}
      
      aChild = aChild->nextSibling();
    }

    // here aChild is an item corresponding to the old position of theSPort
    if ( aChild->nextSibling()->text(0).compare( QString("Gate") ) )
      aChild->moveItem(aChild->nextSibling());
  }
}

// YACSGui_LinkViewItem class:

YACSGui_LinkViewItem::YACSGui_LinkViewItem( QListView* theParent, 
					    QListViewItem* theAfter,
					    YACS::ENGINE::Port* theOutPort,
					    YACS::ENGINE::Port* theInPort ):
  YACSGui_ViewItem( theParent, theAfter ),
  myOutPort( theOutPort ),
  myInPort( theInPort )
{
  QString aName = name();
  if ( !aName.isEmpty() )
    setText( 0, aName );
  setPixmap( 0, icon() );
}

YACSGui_LinkViewItem::YACSGui_LinkViewItem( QListViewItem* theParent, 
					    QListViewItem* theAfter,
					    YACS::ENGINE::Port* theOutPort,
					    YACS::ENGINE::Port* theInPort ):
  YACSGui_ViewItem( theParent, theAfter ),
  myOutPort( theOutPort ),
  myInPort( theInPort )
{
  QString aName = name();
  if ( !aName.isEmpty() )
    setText( 0, aName );
  setPixmap( 0, icon() );
}
  
QString YACSGui_LinkViewItem::name() const
{
  QString aName = "";
  
  if ( myOutPort && myInPort )
  {
    Node* anOutPortNode = myOutPort->getNode();

    ComposedNode* aRoot = 0;
    if ( dynamic_cast<Bloc*>( anOutPortNode->getFather() ) )
      aRoot = anOutPortNode->getFather();
    else
      aRoot = anOutPortNode->getRootNode();

    if (aRoot)
      aName += QString(aRoot->getChildName(anOutPortNode));
    
    DataPort* aDataPort = dynamic_cast<DataPort*>(myOutPort);
    if (aDataPort)
      aName += QString(".%1").arg(aDataPort->getName());
    
    aName += QString(" --> ");
    
    Node* anInPortNode = myInPort->getNode();
    
    aRoot = 0;
    if ( dynamic_cast<Bloc*>( anInPortNode->getFather() ) )
      aRoot = anInPortNode->getFather();
    else
      aRoot = anInPortNode->getRootNode();
    
    if (aRoot)
      aName += QString(aRoot->getChildName(anInPortNode));
    
    aDataPort = dynamic_cast<DataPort*>(myInPort);
    if (aDataPort)
      aName += QString(".%1").arg(aDataPort->getName());
  }

  return aName;
}

QPixmap YACSGui_LinkViewItem::icon() const
{
  QPixmap aRes;
  
  QString anIconName;

  if ( dynamic_cast<OutGate*>( myOutPort ) )
    anIconName = QString(QObject::tr("ICON_CONTROL_LINK_OBJECT"));
  else if ( dynamic_cast<OutputDataStreamPort*>( myOutPort ) )
    anIconName = QString(QObject::tr("ICON_STREAM_LINK_OBJECT"));
  else if ( dynamic_cast<OutputPort*>( myOutPort ) )
    anIconName = QString(QObject::tr("ICON_DATA_LINK_OBJECT"));

  if ( !anIconName.isEmpty() )
    aRes = SUIT_Session::session()->resourceMgr()->loadPixmap("YACSGui",  anIconName, false);

  return aRes;
}

void YACSGui_LinkViewItem::update( const bool theIsRecursive )
{

}

// YACSGui_SchemaViewItem class:

YACSGui_SchemaViewItem::YACSGui_SchemaViewItem( QListView* theParent, 
						QListViewItem* theAfter, 
						YACS::HMI::SubjectProc* theSProc ):
  YACSGui_ViewItem( theParent, theAfter ),
  mySProc( theSProc )
{
  if ( mySProc ) mySProc->attach(this);

  if ( getProc() )
    setText( 0, QString( getProc()->getName() ) );
  setPixmap( 0, icon() );
}

YACSGui_SchemaViewItem::YACSGui_SchemaViewItem( QListViewItem* theParent,
						QListViewItem* theAfter,
						YACS::HMI::SubjectProc* theSProc ):
  YACSGui_ViewItem( theParent, theAfter ),
  mySProc( theSProc )
{
  if ( mySProc ) mySProc->attach(this);
  
  if ( getProc() )
    setText( 0, QString( getProc()->getName() ) );
  setPixmap( 0, icon() );
}

YACSGui_SchemaViewItem::~YACSGui_SchemaViewItem()
{
  if ( mySProc ) mySProc->detach(this);
}

void YACSGui_SchemaViewItem::select(bool isSelected)
{
  printf(">> YACSGui_SchemaViewItem::select\n");
}

void YACSGui_SchemaViewItem::update(YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son)
{
  printf(">> YACSGui_SchemaViewItem::update\n");
  switch (event)
  {
  case RENAME:
    update();
  case EDIT:
    update(true, son);
    break;
  case REMOVE:
    {
      printf("SchemaViewItem:  REMOVE\n");

      // get the "Nodes" label view item, which is used as a parent for first level nodes
      YACSGui_LabelViewItem* aNodesL = dynamic_cast<YACSGui_LabelViewItem*>(firstChild()->nextSibling());
      if ( !aNodesL || aNodesL->text(0).compare(QString("Nodes")) ) return;

      YACSGui_NodeViewItem* aNode = 0;
      QListViewItem* aChild = aNodesL->firstChild();
      while( aChild )
      {
	if ( aNode = dynamic_cast<YACSGui_NodeViewItem*>(aChild) )
	  if ( aNode->getSNode() == son )
	    {
	      printf(">> the son is found\n");
	      break;
	    }
	
	aChild = aChild->nextSibling();
      }
      
      if ( aNode )
      {
	printf(">> delete the son item\n");
	aNodesL->takeItem(aNode);
	delete aNode;
      }
    }
    break;
  default:
    GuiObserver::update(event, type, son);
  }
}

QPixmap YACSGui_SchemaViewItem::icon() const
{
  QPixmap aRes;

  QString anIconName = QString(QObject::tr("ICON_SCHEMA_OBJECT"));
  if ( !anIconName.isEmpty() )
    aRes = SUIT_Session::session()->resourceMgr()->loadPixmap("YACSGui", anIconName, false);
  
  return aRes;
}

YACS::ENGINE::Proc* YACSGui_SchemaViewItem::getProc() const
{
  return ( mySProc ? dynamic_cast<Proc*>(mySProc->getNode()) : 0 );
}

void YACSGui_SchemaViewItem::update( const bool theIsRecursive,
				     YACS::HMI::Subject* theSon,
				     YACSGui_NodeViewItem* theBlocItem )
{
  if ( theIsRecursive ) // total update
  {
    if ( SubjectNode* aNodeSon = dynamic_cast<SubjectNode*>(theSon) ) // theSon is a node
    {
      // get the "Nodes" label view item, which is used as a parent for first level nodes
      YACSGui_LabelViewItem* aNodesL = dynamic_cast<YACSGui_LabelViewItem*>(firstChild()->nextSibling());
      if ( !aNodesL || aNodesL->text(0).compare(QString("Nodes")) ) return;

      // find list view item before theSon
      YACSGui_NodeViewItem* anAfter = 0;
      YACSGui_NodeViewItem* anUpdated = 0;
      QListViewItem* aChild = aNodesL->firstChild();

      if ( YACSGui_NodeViewItem* aFirst = dynamic_cast<YACSGui_NodeViewItem*>(aChild) )
	if ( aFirst->getSNode() == aNodeSon )
	  anUpdated = aFirst;
	else
	{
	  while( aChild )
	  {
	    if ( YACSGui_NodeViewItem* aNext = dynamic_cast<YACSGui_NodeViewItem*>(aChild->nextSibling()) )
	      if ( aNext->getSNode() == aNodeSon )
	      {
		anAfter = dynamic_cast<YACSGui_NodeViewItem*>(aChild);
		anUpdated = aNext;
		break;
	      }
	    aChild = aChild->nextSibling();
	  }
	}
      
      if ( anUpdated )
      {
	if ( YACSGui_EditionTreeView* anETV = dynamic_cast<YACSGui_EditionTreeView*>(listView()) )
	{
	  bool anIsOpen = anUpdated->isOpen();

	  // remove list view item for theSon from the tree
	  aNodesL->takeItem(anUpdated);
	  delete anUpdated;
	  
	  // recreate list view item for theSon
	  if ( anUpdated = anETV->displayNodeWithPorts( aNodesL, anAfter, aNodeSon ) )
	    anUpdated->setOpen(anIsOpen);
	}
      }
    }
    else if ( !theSon && theBlocItem ) // theSon is null => remove any nodes under theBlocItem
                                       //                   and add these nodes under "Nodes" label
    {
      Bloc* aBloc = dynamic_cast<Bloc*>( theBlocItem->getNode() );
      if ( !aBloc ) return;

      // get the "Nodes" label view item, which is used as a parent for first level nodes
      YACSGui_LabelViewItem* aNodesL = dynamic_cast<YACSGui_LabelViewItem*>(firstChild()->nextSibling());
      if ( !aNodesL || aNodesL->text(0).compare(QString("Nodes")) ) return;

      QListViewItem* aChild = theBlocItem->firstChild();
      while( aChild )
      {
	if ( YACSGui_NodeViewItem* aChildNodeItem = dynamic_cast<YACSGui_NodeViewItem*>(aChild) )
	  if ( !aBloc->isInMyDescendance( aChildNodeItem->getNode() ) )
	  {
	    printf(">> Delete and insert : %s\n",aChildNodeItem->getNode()->getName().c_str());
	    QListViewItem* anAuxItem = aChild->nextSibling();

	    // remove view item from theBlocItem 
	    takeItem(aChild);
	    aNodesL->insertItem(aChild);
	   	    
	    aChild = anAuxItem;
	    continue;
	  }
	
	aChild = aChild->nextSibling();
      }
    }
  }
  else if ( mySProc ) // only rename
    setText( 0, QString( mySProc->getName() ) );
}

// YACSGui_ContainerViewItem class:

YACSGui_ContainerViewItem::YACSGui_ContainerViewItem(  QListView* theParent, 
						       QListViewItem* theAfter,
						       YACS::HMI::SubjectContainer* theSContainer ):
  YACSGui_ViewItem( theParent, theAfter ),
  mySContainer( theSContainer )
{
  if ( mySContainer )
  {
    mySContainer->attach(this);

    QString aName = name();
    if ( !aName.isEmpty() )
      setText( 0, aName );
    setPixmap( 0, icon() );
  }
}

YACSGui_ContainerViewItem::YACSGui_ContainerViewItem(  QListViewItem* theParent, 
						       QListViewItem* theAfter,
						       YACS::HMI::SubjectContainer* theSContainer ):
  YACSGui_ViewItem( theParent, theAfter ),
  mySContainer( theSContainer )
{
  if ( mySContainer )
  {
    mySContainer->attach(this);

    QString aName = name();
    if ( !aName.isEmpty() )
      setText( 0, aName );
    setPixmap( 0, icon() );
  }
}

YACSGui_ContainerViewItem::~YACSGui_ContainerViewItem()
{
  if ( mySContainer ) mySContainer->detach(this);
}

void YACSGui_ContainerViewItem::select(bool isSelected)
{
  printf(">> YACSGui_ContainerViewItem::select\n");
}

void YACSGui_ContainerViewItem::update(YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son)
{
  printf(">> YACSGui_ContainerViewItem::update\n");
  switch (event)
  {
  case RENAME:
    update();
    break;
  default:
    GuiObserver::update(event, type, son);
  }
}

QString YACSGui_ContainerViewItem::name() const
{
  return ( mySContainer ? QString(mySContainer->getName()) : QString("") );
}

QPixmap YACSGui_ContainerViewItem::icon() const
{
  QPixmap aRes;
  
  if ( getContainer() )
    aRes = SUIT_Session::session()->resourceMgr()->loadPixmap("YACSGui",  QObject::tr("ICON_CONTAINER_OBJECT"));

  return aRes;
}

YACS::ENGINE::Container* YACSGui_ContainerViewItem::getContainer() const
{
  return ( mySContainer ? mySContainer->getContainer() : 0 );
}

void YACSGui_ContainerViewItem::update( YACSGui_ComponentViewItem* theComponent )
{
  if ( theComponent )
  {
    takeItem(theComponent);
    
    // get the "Containers" label view item
    YACSGui_LabelViewItem* aContsL = 
      dynamic_cast<YACSGui_LabelViewItem*>(listView()->firstChild()->firstChild()->nextSibling()->nextSibling()->nextSibling());
    if ( !aContsL || aContsL->text(0).compare(QString("Containers")) ) return;

    // find new container view item, under which theComponent view item should be moved
    YACSGui_ContainerViewItem* aCont = 0;
    QListViewItem* aChild = aContsL->firstChild();
    while( aChild )
    {
      if ( YACSGui_ContainerViewItem* aChildContItem = dynamic_cast<YACSGui_ContainerViewItem*>(aChild) )
	if ( aChildContItem->getContainer() == theComponent->getComponent()->getContainer() )
	{
	  aCont = aChildContItem;
	  break;
	}
      aChild = aChild->nextSibling();
    }

    if ( aCont ) aCont->insertItem(theComponent);
  }

  setText( 0, name() ); // only rename
}

// YACSGui_ComponentViewItem class:

YACSGui_ComponentViewItem::YACSGui_ComponentViewItem( QListView* theParent, 
						      QListViewItem* theAfter,
						      YACS::HMI::SubjectComponent* theSComponent ):
  YACSGui_ViewItem( theParent, theAfter ),
  mySComponent( theSComponent )
{
  if ( mySComponent )
  {
    mySComponent->attach(this);
    
    QString aName = name();
    if ( !aName.isEmpty() )
      setText( 0, aName );
    setPixmap( 0, icon() );
  }
}

YACSGui_ComponentViewItem::YACSGui_ComponentViewItem( QListViewItem* theParent, 
						      QListViewItem* theAfter,
						      YACS::HMI::SubjectComponent* theSComponent ):
  YACSGui_ViewItem( theParent, theAfter ),
  mySComponent( theSComponent )
{
  if ( mySComponent )
  {
    mySComponent->attach(this);

    QString aName = name();
    if ( !aName.isEmpty() )
      setText( 0, aName );
    setPixmap( 0, icon() );
  }
}

YACSGui_ComponentViewItem::~YACSGui_ComponentViewItem()
{
  if ( mySComponent ) mySComponent->detach(this);
}

void YACSGui_ComponentViewItem::select(bool isSelected)
{
  printf(">> YACSGui_ComponentViewItem::select\n");
}

void YACSGui_ComponentViewItem::update(YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son)
{
  printf(">> YACSGui_ComponentViewItem::update\n");
  switch (event)
  {
  case RENAME:
    update();
    break;
  case EDIT:
    update(true);
    break;
  default:
    GuiObserver::update(event, type, son);
  }
}
 
QString YACSGui_ComponentViewItem::name() const
{
  return ( mySComponent ? QString(mySComponent->getName()) : QString("") );
}

QPixmap YACSGui_ComponentViewItem::icon() const
{
  QPixmap aRes;

  if ( getComponent() )
    aRes = SUIT_Session::session()->resourceMgr()->loadPixmap("YACSGui",  QObject::tr("ICON_COMPONENT_OBJECT"));

  return aRes;
}

YACS::ENGINE::ComponentInstance* YACSGui_ComponentViewItem::getComponent() const
{
  return ( mySComponent ? mySComponent->getComponent() : 0 );
}

void YACSGui_ComponentViewItem::update( const bool theMove )
{
  if ( theMove )
  {
    // find the container view item under which this component is published now
    YACSGui_ContainerViewItem* aCont = 0;
    QListViewItem* aParent = parent();
    while( aParent )
    {
      if ( aCont = dynamic_cast<YACSGui_ContainerViewItem*>(aParent) )
	break;
      aParent = aParent->parent();
    }

    if ( aCont ) aCont->update(this);
  }

  setText( 0, name() ); // only rename
}

/* ================ items for tree view in run mode ================ */

// YACSGui_ComposedNodeViewItem class:

YACSGui_ComposedNodeViewItem::YACSGui_ComposedNodeViewItem(QListView *parent,
							   QString label,
							   YACS::ENGINE::ComposedNode *node)
  : QListViewItem(parent, label),
    _node(node)
{
  _cf = Qt::black;
  setPixmap( 0, icon() );
}

YACSGui_ComposedNodeViewItem::YACSGui_ComposedNodeViewItem(QListViewItem *parent,
							   QString label,
							   YACS::ENGINE::ComposedNode *node)
  : QListViewItem(parent, label),
    _node(node)
{
  _cf = Qt::black;
}

void YACSGui_ComposedNodeViewItem::paintCell( QPainter *p, const QColorGroup &cg,
					      int column, int w, int alignment )
{
  //MESSAGE("ComposedNodeViewItem::paintCell " << column);
  QColorGroup _cg( cg );
  QColor c = _cg.text();
  if (column == 1)
  {
    _cg.setColor( QColorGroup::Text, _cf );
    if ( dynamic_cast<Proc*>(getNode()) )
      _cg.setColor( QColorGroup::Background, statusBgColor() );
  }

  p->fillRect( 0, 0, w, height(), cg.brush( QColorGroup::Base ) );

  int W = w;
  if ( listView() && !listView()->allColumnsShowFocus() )
    W = width( p->fontMetrics(), listView(), column );

  QListViewItem::paintCell( p, _cg, column, W<w ? W : w, alignment );

  if (column == 1) _cg.setColor( QColorGroup::Text, c );
}

QColor YACSGui_ComposedNodeViewItem::statusBgColor() const
{
  QColor wbg;
  switch (_state)
    {
    case YACS::NOTYETINITIALIZED: wbg.setHsv( 45, 50, 255); break;
    case YACS::INITIALISED:       wbg.setHsv( 90, 50, 255); break;
    case YACS::RUNNING:           wbg.setHsv(135, 50, 255); break;
    case YACS::WAITINGTASKS:      wbg.setHsv(180, 50, 255); break;
    case YACS::PAUSED:            wbg.setHsv(225, 50, 255); break;
    case YACS::FINISHED:          wbg.setHsv(270, 50, 255); break;
    case YACS::STOPPED:           wbg.setHsv(315, 50, 255); break;
    default:                      wbg.setHsv(360, 50, 255);
    }
  return wbg;
}

void YACSGui_ComposedNodeViewItem::setState(int state)
{
  //MESSAGE("ComposedNodeViewItem::setState");
  _state = state;
  switch (_state)
    {
    case YACS::UNDEFINED:    _cf=Qt::lightGray;       setText(1,"UNDEFINED");    repaint(); break;
    case YACS::INITED:       _cf=Qt::gray;            setText(1,"INITED");       repaint(); break;
    case YACS::TOLOAD:       _cf=Qt::darkYellow;      setText(1,"TOLOAD");       repaint(); break;
    case YACS::LOADED:       _cf=Qt::darkMagenta;     setText(1,"LOADED");       repaint(); break;
    case YACS::TOACTIVATE:   _cf=Qt::darkCyan;        setText(1,"TOACTIVATE");   repaint(); break;
    case YACS::ACTIVATED:    _cf=Qt::darkBlue;        setText(1,"ACTIVATED");    repaint(); break;
    case YACS::DESACTIVATED: _cf=Qt::gray;            setText(1,"DESACTIVATED"); repaint(); break;
    case YACS::DONE:         _cf=Qt::darkGreen;       setText(1,"DONE");         repaint(); break;
    case YACS::SUSPENDED:    _cf=Qt::gray;            setText(1,"SUSPENDED");    repaint(); break;
    case YACS::LOADFAILED:   _cf.setHsv(320,255,255); setText(1,"LOADFAILED");   repaint(); break;
    case YACS::EXECFAILED:   _cf.setHsv( 20,255,255); setText(1,"EXECFAILED");   repaint(); break;
    case YACS::PAUSE:        _cf.setHsv(180,255,255); setText(1,"PAUSE");        repaint(); break;
    case YACS::INTERNALERR:  _cf.setHsv(340,255,255); setText(1,"INTERNALERR");  repaint(); break;
    case YACS::DISABLED:     _cf.setHsv( 40,255,255); setText(1,"DISABLED");     repaint(); break;
    case YACS::FAILED:       _cf.setHsv( 20,255,255); setText(1,"FAILED");       repaint(); break;
    case YACS::ERROR:        _cf.setHsv(  0,255,255); setText(1,"ERROR");        repaint(); break;
    default:                 _cf=Qt::lightGray; repaint();
    }
}

void YACSGui_ComposedNodeViewItem::setStatus(int status)
{
  if ( dynamic_cast<Proc*>(getNode()) ) // this veiw item is a schema (root) view item
  {
    QString aStatus("");

    _state = status;
    switch (_state)
    {
    case YACS::NOTYETINITIALIZED: aStatus = "Not Yet Initialized"; break;
    case YACS::INITIALISED:       aStatus = "Initialized"; break;
    case YACS::RUNNING:           aStatus = "Running"; break;
    case YACS::WAITINGTASKS:      aStatus = "Waiting Tasks"; break;
    case YACS::PAUSED:            aStatus = "Paused"; break;
    case YACS::FINISHED:          aStatus = "Finished"; break;
    case YACS::STOPPED:           aStatus = "Stopped"; break;
    default:                      aStatus = "Status Unknown";
    }
    setText(1,aStatus);
    repaint();
  }
}

QPixmap YACSGui_ComposedNodeViewItem::icon() const
{
  QPixmap aRes;

  QString anIconName = QString(QObject::tr("ICON_SCHEMA_OBJECT"));
  if ( !anIconName.isEmpty() )
    aRes = SUIT_Session::session()->resourceMgr()->loadPixmap("YACSGui", anIconName, false);
  
  return aRes;
}

void YACSGui_ComposedNodeViewItem::update( const bool theIsRecursive )
{
  if ( this == listView()->firstChild() )
  { // this is a root schema object
    if ( YACSGui_RunTreeView* aRTV = dynamic_cast<YACSGui_RunTreeView*>( listView() ) )
      setText( 0, QString(aRTV->getProc()->getName()) );
  }
  else
  { // this is a composed node
    // update state
    // ...
  }
}

// YACSGui_ElementaryNodeViewItem class:

YACSGui_ElementaryNodeViewItem::YACSGui_ElementaryNodeViewItem(QListView *parent,
							       const QString &text,
							       Type tt,
							       YACS::ENGINE::ElementaryNode *node)
  : QCheckListItem(parent, text, tt)
{
  _cf = Qt::green;
  _node = node;
}

YACSGui_ElementaryNodeViewItem::YACSGui_ElementaryNodeViewItem(QListViewItem *parent,
							       const QString &text,
							       Type tt,
							       YACS::ENGINE::ElementaryNode *node)
  : QCheckListItem(parent, text, tt)
{
  _cf = Qt::green;
  _node = node;
}

void YACSGui_ElementaryNodeViewItem::paintCell( QPainter *p, const QColorGroup &cg,
						int column, int width, int alignment )
{
  QColorGroup _cg( cg );
  QColor c = _cg.text();
  if (column == 1) _cg.setColor( QColorGroup::Text, _cf );  
  QCheckListItem::paintCell( p, _cg, column, width, alignment );
  if (column == 1) _cg.setColor( QColorGroup::Text, c );
}

void YACSGui_ElementaryNodeViewItem::setState(int state)
{
  //MESSAGE("ElementaryNodeViewItem::setState");
  _state = state;
  switch (_state)
    {
    case YACS::UNDEFINED:    _cf=Qt::lightGray;       setText(1,"UNDEFINED");    repaint(); break;
    case YACS::INITED:       _cf=Qt::gray;            setText(1,"INITED");       repaint(); break;
    case YACS::TOLOAD:       _cf=Qt::darkYellow;      setText(1,"TOLOAD");       repaint(); break;
    case YACS::LOADED:       _cf=Qt::darkMagenta;     setText(1,"LOADED");       repaint(); break;
    case YACS::TOACTIVATE:   _cf=Qt::darkCyan;        setText(1,"TOACTIVATE");   repaint(); break;
    case YACS::ACTIVATED:    _cf=Qt::darkBlue;        setText(1,"ACTIVATED");    repaint(); break;
    case YACS::DESACTIVATED: _cf=Qt::gray;            setText(1,"DESACTIVATED"); repaint(); break;
    case YACS::DONE:         _cf=Qt::darkGreen;       setText(1,"DONE");         repaint(); break;
    case YACS::SUSPENDED:    _cf=Qt::gray;            setText(1,"SUSPENDED");    repaint(); break;
    case YACS::LOADFAILED:   _cf.setHsv(320,255,255); setText(1,"LOADFAILED");   repaint(); break;
    case YACS::EXECFAILED:   _cf.setHsv( 20,255,255); setText(1,"EXECFAILED");   repaint(); break;
    case YACS::PAUSE:        _cf.setHsv(180,255,255); setText(1,"PAUSE");        repaint(); break;
    case YACS::INTERNALERR:  _cf.setHsv(340,255,255); setText(1,"INTERNALERR");  repaint(); break;
    case YACS::DISABLED:     _cf.setHsv( 40,255,255); setText(1,"DISABLED");     repaint(); break;
    case YACS::FAILED:       _cf.setHsv( 20,255,255); setText(1,"FAILED");       repaint(); break;
    case YACS::ERROR:        _cf.setHsv(  0,255,255); setText(1,"ERROR");        repaint(); break;
    default:                 _cf=Qt::lightGray; repaint();
   }
}

void YACSGui_ElementaryNodeViewItem::update( const bool theIsRecursive )
{
  // update state
  // ...
}
