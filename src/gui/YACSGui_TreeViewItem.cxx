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
#include <YACSGui_Executor.h>
#include <YACSGui_LogViewer.h>

#include <guiContext.hxx>

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

#include <CORBAComponent.hxx>

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
  GuiObserver(),
  myBlockSelect( false )
{
}

/*!
  \brief Block actions performed in select() method. Usually this method is called 
         from select() in order to avoid circularity because of synchronization: 
         selection in 2D viewer --> selection in tree view --> selection in 2D viewer --> etc.
  \param toBlock block selection
  \return previous blocking value 
*/
bool YACSGui_ViewItem::blockSelection( const bool toBlock )
{
  bool prevVal = myBlockSelect;
  myBlockSelect = toBlock;
  return prevVal;
}

/*!
  \brief Verifiee whether actions performed in select() method are blocked
         (see blockSelection method for more dscription)
  \return blocking value 
*/
bool YACSGui_ViewItem::selectionBlocked() const
{
  return myBlockSelect;
}

void YACSGui_ViewItem::select( bool isSelected )
{
  if ( selectionBlocked() )
    return;

  //printf(">> YACSGui_ViewItem::select( %d ) \n", isSelected );

  if ( !isSelected ) // do nothing
    return;

  // select item in tree view
  QListView* aListView = listView();
  if ( !aListView )
    return;

  blockSelection( true );
  aListView->clearSelection();
  aListView->setCurrentItem( this );
  aListView->setSelected( this, true );
  aListView->ensureItemVisible( this );
  blockSelection( false );
}

void YACSGui_ViewItem::removeNodeItem( YACS::HMI::Subject* theSNode )
{
  //printf( "YACSGui_ViewItem::removeNodeItem (this=%s)\n", text(0).latin1() );
  QListViewItem* aNodesL = this;
  if( !aNodesL->parent() )
  {
    // get the "Nodes" label view item, which is used as a parent for first level nodes
    aNodesL = firstChild()->nextSibling();
    if ( !aNodesL || aNodesL->text(0).compare(QString("Nodes")) )
      return;
  }

  YACSGui_NodeViewItem* aNode = 0;
  QListViewItem* aChild = aNodesL->firstChild();
  while( aChild )
  {
    if ( aNode = dynamic_cast<YACSGui_NodeViewItem*>(aChild) )
      if ( aNode->getSNode() == theSNode )
      {
	//printf(">> the son is found\n");
	break;
      }

    aChild = aChild->nextSibling();
  }    

  if ( aNode )
  {
    if( YACS::HMI::SubjectNode* aSNode = dynamic_cast<SubjectNode*>( theSNode ) )
    {
      //printf( ">> remove control links\n" );
      std::list<SubjectControlLink*> aControlLinks = aSNode->getSubjectControlLinks();
      std::list<SubjectControlLink*>::iterator cIt = aControlLinks.begin();
      std::list<SubjectControlLink*>::iterator cItEnd = aControlLinks.end();
      for( ; cIt != cItEnd; cIt++ )
      {
	SubjectControlLink* aControlLink = *cIt;
	if( aControlLink )
	{
	  Subject* aParentSNode = aControlLink->getSubjectOutNode()->getParent();
	  aParentSNode->update( REMOVE, CONTROLLINK, aControlLink );
	}
      }

      //printf( ">> remove data links\n" );
      std::list<SubjectLink*> aDataLinks = aSNode->getSubjectLinks();
      std::list<SubjectLink*>::iterator dIt = aDataLinks.begin();
      std::list<SubjectLink*>::iterator dItEnd = aDataLinks.end();
      for( ; dIt != dItEnd; dIt++ )
      {
	SubjectLink* aDataLink = *dIt;
	if( aDataLink )
	{
	  Subject* aParentSNode = aDataLink->getSubjectOutNode()->getParent();
	  aParentSNode->update( REMOVE, DATALINK, aDataLink );
	}
      }

      //printf( ">> remove port links\n" );
      std::list<SubjectDataPort*> aDataPorts;

      std::list<SubjectInputPort*> anInputPorts = aSNode->getSubjectInputPorts();
      for( std::list<SubjectInputPort*>::iterator pIt = anInputPorts.begin(); pIt != anInputPorts.end(); pIt++ )
	aDataPorts.push_back( *pIt );

      std::list<SubjectOutputPort*> anOutputPorts = aSNode->getSubjectOutputPorts();
      for( std::list<SubjectOutputPort*>::iterator pIt = anOutputPorts.begin(); pIt != anOutputPorts.end(); pIt++ )
	aDataPorts.push_back( *pIt );

      std::list<SubjectInputDataStreamPort*> anInputDataStreamPorts = aSNode->getSubjectInputDataStreamPorts();
      for( std::list<SubjectInputDataStreamPort*>::iterator pIt = anInputDataStreamPorts.begin();
	   pIt != anInputDataStreamPorts.end(); pIt++ )
	aDataPorts.push_back( *pIt );

      std::list<SubjectOutputDataStreamPort*> anOutputDataStreamPorts = aSNode->getSubjectOutputDataStreamPorts();
      for( std::list<SubjectOutputDataStreamPort*>::iterator pIt = anOutputDataStreamPorts.begin();
	   pIt != anOutputDataStreamPorts.end(); pIt++ )
	aDataPorts.push_back( *pIt );

      std::list<SubjectDataPort*>::iterator dpIt = aDataPorts.begin();
      std::list<SubjectDataPort*>::iterator dpItEnd = aDataPorts.end();
      for( ; dpIt != dpItEnd; dpIt++ )
      {
	SubjectDataPort* aDataPort = *dpIt;
	if( aDataPort )
	{
	  std::list<SubjectLink*> lsl = aDataPort->getListOfSubjectLink();
	  for( std::list<SubjectLink*>::iterator it = lsl.begin(); it != lsl.end(); ++it )
	  {
	    SubjectLink* aLink = *it;
	    if( aLink )
	    {
	      //printf( ">> link : %s\n", aLink->getName().c_str() );
	      SubjectNode* aNode1 = aLink->getSubjectInNode();
	      SubjectNode* aNode2 = aLink->getSubjectOutNode();
	      //printf( ">> nodes : %s - %s\n", aNode1->getName().c_str(), aNode2->getName().c_str() );
	      if( SubjectComposedNode* aLowestCommonAncestor =
		  SubjectComposedNode::getLowestCommonAncestor( aNode1, aNode2 ) )
	      {
		//printf( ">> lowest common ancestor : %s\n", aLowestCommonAncestor->getName().c_str() );
		aLowestCommonAncestor->update( REMOVE, DATALINK, aLink );
	      }
	    }
	  }
	}
      }

      //printf( ">> remove leaving/coming current scope links\n" );
      std::vector< std::pair<OutPort *, InPort *> > listLeaving  = aSNode->getNode()->getSetOfLinksLeavingCurrentScope();
      std::vector< std::pair<InPort *, OutPort *> > listIncoming = aSNode->getNode()->getSetOfLinksComingInCurrentScope();
      std::vector< std::pair<OutPort *, InPort *> > globalList = listLeaving;
      std::vector< std::pair<InPort *, OutPort *> >::iterator it1;
      for (it1 = listIncoming.begin(); it1 != listIncoming.end(); ++it1)
      {
	std::pair<OutPort *, InPort *> outin = std::pair<OutPort *, InPort *>((*it1).second, (*it1).first);
	globalList.push_back(outin);
      }
      std::vector< std::pair<OutPort *, InPort *> >::iterator it2;
      for (it2 = globalList.begin(); it2 != globalList.end(); ++it2)
      {
	if (GuiContext::getCurrent()->_mapOfSubjectLink.count(*it2))
        {
          SubjectLink* aLink = GuiContext::getCurrent()->_mapOfSubjectLink[*it2];
	  if( aLink )
	  {
	    //printf( ">> link : %s\n", aLink->getName().c_str() );
	    SubjectNode* aNode1 = aLink->getSubjectInNode();
	    SubjectNode* aNode2 = aLink->getSubjectOutNode();
	    //printf( ">> nodes : %s - %s\n", aNode1->getName().c_str(), aNode2->getName().c_str() );
	    if( SubjectComposedNode* aLowestCommonAncestor =
		SubjectComposedNode::getLowestCommonAncestor( aNode1, aNode2 ) )
	    {
	      //printf( ">> lowest common ancestor : %s\n", aLowestCommonAncestor->getName().c_str() );
	      aLowestCommonAncestor->update( REMOVE, DATALINK, aLink );
	    }
	  }
	}
      }
    }

    //printf(">> delete the son item\n");
    aNodesL->takeItem(aNode);
    delete aNode;
  }
}

void YACSGui_ViewItem::removeLinkItem( YACS::HMI::Subject* theSLink )
{
  //printf( "YACSGui_ViewItem::removeLinkItem (this=%s)\n", text(0).latin1() );
  SubjectLink* aSLink = dynamic_cast<SubjectLink*>(theSLink);
  SubjectControlLink* aSCLink = dynamic_cast<SubjectControlLink*>(theSLink);
  if ( aSLink || aSCLink )
  {
    // get the "Links" label view item under the parent node of this node view item
    YACSGui_LabelViewItem* aLinksL = 0;
    QListViewItem* aChildLinks = this;
    if( !aChildLinks->parent() )
      aChildLinks = aChildLinks->firstChild();
    while( aChildLinks )
    {
      //printf( ">> item : %s\n", aChildLinks->text(0).latin1() );
      aLinksL = dynamic_cast<YACSGui_LabelViewItem*>(aChildLinks);
      if ( aLinksL && aLinksL->text(0).compare(QString("Links")) == 0 )
	break;
      aChildLinks = aChildLinks->nextSibling();
    }

    if ( !aLinksL )
      return;

    // find the link item published under in the Links folder to delete
    QListViewItem* aChild = aLinksL->firstChild();
    while( aChild )
    {
      if ( aSLink )
      {
	if ( YACSGui_LinkViewItem* aLink = dynamic_cast<YACSGui_LinkViewItem*>(aChild) )
	  if ( aLink->getSLink() == aSLink )
	  {
	    //printf(">> the data link is found\n");
	    aLinksL->takeItem(aChild);
	    delete aChild;
	    break;
	  }
      }
      else if ( aSCLink )
      {
	if ( YACSGui_ControlLinkViewItem* aCLink = dynamic_cast<YACSGui_ControlLinkViewItem*>(aChild) )
	  if ( aCLink->getSLink() == aSCLink )
	  {
	    //printf(">> the control link is found\n");
	    aLinksL->takeItem(aChild);
	    delete aChild;
	    break;
	  }
      }
      
      aChild = aChild->nextSibling();
    }
  }
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

void YACSGui_LabelViewItem::select( bool isSelected )
{
  //printf(">> YACSGui_ReferenceViewItem::select( %d ) does nothing\n", isSelected);
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
  printf(">> YACSGui_ReferenceViewItem::select( %d ) does nothing\n", isSelected);
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
					    YACS::HMI::SubjectDataPort* theSPort ):
  YACSGui_ViewItem( theParent, theAfter ),
  mySPort( theSPort )
{
  if ( mySPort )
  {
    mySPort->attach( this );

    QString aName = name();
    if ( !aName.isEmpty() )
      setText( 0, aName );
    setPixmap( 0, icon() );
  }
}

YACSGui_PortViewItem::YACSGui_PortViewItem( QListViewItem* theParent, 
					    QListViewItem* theAfter,
					    YACS::HMI::SubjectDataPort* theSPort ):
  YACSGui_ViewItem( theParent, theAfter ),
  mySPort( theSPort )
{
  if ( mySPort )
  {
    mySPort->attach( this );
    
    QString aName = name();
    if ( !aName.isEmpty() )
      setText( 0, aName );
    setPixmap( 0, icon() );
  }
}

YACSGui_PortViewItem::~YACSGui_PortViewItem()
{
  if ( mySPort ) mySPort->detach(this);
}

QString YACSGui_PortViewItem::name() const
{
  QString aName = "";

  DataPort* aDataPort = getPort();
  if (aDataPort)
    aName = aDataPort->getName();
  
  return aName;
}

QPixmap YACSGui_PortViewItem::icon() const
{
  QPixmap aRes;
  
  QString anIconName;

  if ( dynamic_cast<OutPort*>( getPort() ) )
    anIconName = QString(QObject::tr("ICON_OUT_PORT_OBJECT"));
  else if ( dynamic_cast<InPort*>( getPort() ) )
    anIconName = QString(QObject::tr("ICON_IN_PORT_OBJECT"));

  if ( !anIconName.isEmpty() )
    aRes = SUIT_Session::session()->resourceMgr()->loadPixmap("YACSGui",  anIconName, false);
  
  return aRes;
}

YACS::ENGINE::DataPort* YACSGui_PortViewItem::getPort() const
{
  return ( mySPort ? mySPort->getPort() : 0 );
}

void YACSGui_PortViewItem::update( const bool theIsRecursive )
{

}

// YACSGui_DataTypeItem class:

/*!
  \brief Constructor
*/
YACSGui_DataTypeItem::YACSGui_DataTypeItem( QListView*                  theParent,
			                    QListViewItem*              theAfter,
			                    YACS::HMI::SubjectDataType* theSDataType )
: YACSGui_ViewItem( theParent, theAfter ),
  mySDataType( theSDataType )
{
  if ( theSDataType ) 
    theSDataType->attach( this );

  QString aName = name();
  if ( !aName.isEmpty() )
    setText( 0, aName );
  setPixmap( 0, icon() );
}

/*!
  \brief Constructor
*/
YACSGui_DataTypeItem::YACSGui_DataTypeItem( QListViewItem* theParent, 
			                    QListViewItem* theAfter,
			                    YACS::HMI::SubjectDataType* theSDataType )
: YACSGui_ViewItem( theParent, theAfter ),
  mySDataType( theSDataType )
{
  if ( theSDataType ) 
    theSDataType->attach( this );

  QString aName = name();
  if ( !aName.isEmpty() )
    setText( 0, aName );
  setPixmap( 0, icon() );
}

/*!
  \brief Destructor
*/
YACSGui_DataTypeItem::~YACSGui_DataTypeItem()
{
}
  
/*!
  \brief Updates item in accordance with GUI event
*/
void YACSGui_DataTypeItem::update( YACS::HMI::GuiEvent event, 
                                   int type, 
                                   YACS::HMI::Subject* son )
{
  printf(">> YACSGui_DataTypeItem::update\n");
  switch ( event )
  {
  case RENAME:
    update();
    break;
  default:
    GuiObserver::update( event, type, son );
    break;
  }
}

/*!
  \brief Gets item name
*/
QString YACSGui_DataTypeItem::name() const
{
  QString aName;
  if ( mySDataType )
    aName = QString( mySDataType->getName() );
  return aName;
}

/*!
  \brief Gets item icon
*/
QPixmap YACSGui_DataTypeItem::icon() const
{
  QPixmap aRes;
  QString anIconName = QString( QObject::tr( "ICON_TEXT" ) ); 

  anIconName = QString( QObject::tr( "ICON_TEXT" ) );
  
  aRes = SUIT_Session::session()->resourceMgr()->loadPixmap(
    "YACSGui", anIconName, false );

  return aRes;
}
  
void YACSGui_DataTypeItem::update( const bool /*theIsRecursive*/ )
{
  // nothing to be updated
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
	//printf("NodeViewItem:  EDIT port\n");
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
  case ADDREF:
    {
      // add a reference item (this = node item)
      //printf("NodeViewItem:  ADDREF\n");
      addReferenceItem(son);
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
	//printf("NodeViewItem:  ADD port\n");
	addPortItem(son);
      }
      break;
    case BLOC:
    case FOREACHLOOP:
    case OPTIMIZERLOOP:
    case FORLOOP:
    case WHILELOOP:
    case SWITCH:
    case PYTHONNODE:
    case PYFUNCNODE:
    case CORBANODE:
    case SALOMENODE:
    case CPPNODE:
    case SALOMEPYTHONNODE:
    case XMLNODE:
      {
	// remove a node inside a block (this = block item)
	//printf("NodeViewItem:  ADD\n");
	addNodeItem(son);
      }
      break;
    default:
      break;
    }
    break;
  case REMOVE:
    switch (type)
    {
    case DATATYPE:
      {
        // Readdress this event to schema item 
        YACSGui_SchemaViewItem* aSchema = 
          dynamic_cast<YACSGui_SchemaViewItem*>( listView()->firstChild() );
          if ( aSchema )
            aSchema->update( event, type, son );
      }
      break;
    case DATALINK:
    case CONTROLLINK:
      {
	// remove a link item (this = out node item)
	//printf("NodeViewItem:  REMOVE link\n");
	removeLinkItem(son);
      }
      break;
    case INPUTPORT:
    case OUTPUTPORT:
    case INPUTDATASTREAMPORT:
    case OUTPUTDATASTREAMPORT:
      {
	// remove a port item (this = node item)
	//printf("NodeViewItem:  REMOVE port\n");
	removePortItem(son);
      }
      break;
    case BLOC:
    case FOREACHLOOP:
    case OPTIMIZERLOOP:
    case FORLOOP:
    case WHILELOOP:
    case SWITCH:
    case PYTHONNODE:
    case PYFUNCNODE:
    case CORBANODE:
    case SALOMENODE:
    case CPPNODE:
    case SALOMEPYTHONNODE:
    case XMLNODE:
      {
	// remove a node inside a block (this = block item)
	//printf("NodeViewItem:  REMOVE\n");
	removeNodeItem(son);
      }
      break;
    case REFERENCE:
      {
	// remove a reference to a component (this = service node item)
	//printf("NodeViewItem:  REMOVE reference\n");
	removeReferenceItem(son);
      }
      break;
    default:
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
	//printf("NodeViewItem:  UP port\n");
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
	//printf("NodeViewItem:  DOWN port\n");
	moveDownPortItem(son);
      }
      break;
    default:
      break;
    }
    break;
  case ADDLINK:
  case ADDCONTROLLINK:
    {
      // add link item (this = composed node item)
      //printf("NodeViewItem:  ADDLINK\n");
      addLinkItem(son);
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

QPixmap YACSGui_NodeViewItem::icon( YACS::ENGINE::Node* theNode )
{
  QPixmap aRes;

  QString anIconName;
    
  if ( dynamic_cast<Bloc*>( theNode ) )
    anIconName = QString(QObject::tr("ICON_BLOCK_OBJECT"));
  else if ( dynamic_cast<Loop*>( theNode ) || dynamic_cast<ForEachLoop*>( theNode ) )
    anIconName = QString(QObject::tr("ICON_LOOP_NODE_OBJECT"));
  else if ( dynamic_cast<Switch*>( theNode ) )
    anIconName = QString(QObject::tr("ICON_SWITCH_NODE_OBJECT"));
  else
    anIconName = QString(QObject::tr("ICON_NODE_OBJECT"));
  
  if ( !anIconName.isEmpty() )
    aRes = SUIT_Session::session()->resourceMgr()->loadPixmap("YACSGui", anIconName, false);
  
  return aRes;
}

QPixmap YACSGui_NodeViewItem::icon() const
{
  return icon( getNode() );
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
	  //printf(">> the son is found\n");
	  break;
	}
      
      aChild = aChild->nextSibling();
    }
    
    if ( aPort )
    {
      //printf(">> rename the son item\n");
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

    if ( isPublished(aPort) ) return;

    YACSGui_PortViewItem* aPortItem = new YACSGui_PortViewItem( this, 0, aSPort );
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

    QListViewItem* aChild;
    if ( dynamic_cast<InPort*>(aPort) )
      aChild = firstChild();
    else if ( dynamic_cast<OutPort*>(aPort) )
    {
      // find tree view item corresponds to the last input port
      QListViewItem* aLastInPort = firstChild();
      while( aLastInPort )
      {
	if ( YACSGui_PortViewItem* aFirstOutPortItem = dynamic_cast<YACSGui_PortViewItem*>(aLastInPort->nextSibling()) )
	  if ( dynamic_cast<OutPort*>(aFirstOutPortItem->getPort()) )
	    break;
	aLastInPort = aLastInPort->nextSibling();
      }
      aChild = aLastInPort;
      
      if ( !anAfter )
      {
	aPortItem->moveItem(aLastInPort);
	return;
      }
    }

    while( aChild )
    {
      if ( anAfterItem = dynamic_cast<YACSGui_PortViewItem*>(aChild) )
	if ( anAfterItem->getPort() == anAfter )
	{
	  //printf(">> the son is found\n");
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
	  //printf(">> the son is found\n");
	  takeItem(aChild);
	  delete aChild;
	  break;
	}
      
      aChild = aChild->nextSibling();
    }
  }
}

void YACSGui_NodeViewItem::addNodeItem( YACS::HMI::Subject* theSNode )
{
  if ( SubjectNode* aSNode = dynamic_cast<SubjectNode*>(theSNode) )
    YACSGui_NodeViewItem* aNodeItem = new YACSGui_NodeViewItem( this, 0, aSNode );
}

void YACSGui_NodeViewItem::moveUpPortItem( YACS::HMI::Subject* theSPort )
{
  if ( SubjectDataPort* aSPort = dynamic_cast<SubjectDataPort*>(theSPort) )
  {
    QListViewItem* aChild = firstChild();
    while( aChild )
    {
      if ( YACSGui_PortViewItem* aPort = dynamic_cast<YACSGui_PortViewItem*>(aChild->nextSibling()) )
	if ( aPort->getPort() == aSPort->getPort() )
	  break;
      
      aChild = aChild->nextSibling();
    }

    // here aChild is an after item for old position of theSPort
    YACSGui_PortViewItem* aCPort = dynamic_cast<YACSGui_PortViewItem*>(aChild);
    if ( !aCPort ) return;

    if ( dynamic_cast<InPort*>(aSPort->getPort()) && dynamic_cast<InPort*>(aCPort->getPort())
	 ||
	 dynamic_cast<OutPort*>(aSPort->getPort()) && dynamic_cast<OutPort*>(aCPort->getPort()) )
    //if ( dynamic_cast<InPort*>(aSPort->getPort())
    //	   ||
    //     aChild->text(0).compare( QString("Gate") ) )
      {
	aChild->moveItem(aChild->nextSibling());
      }
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
	  //printf(">> the son is found\n");
	  break;
	}
      
      aChild = aChild->nextSibling();
    }

    // here aChild is an item corresponding to the old position of theSPort
    YACSGui_PortViewItem* aCPort = dynamic_cast<YACSGui_PortViewItem*>(aChild);
    if ( !aCPort ) return;

    YACSGui_PortViewItem* aCNPort = dynamic_cast<YACSGui_PortViewItem*>(aChild->nextSibling());
    if ( !aCNPort ) return;

    if ( !dynamic_cast<InPort*>(aCPort->getPort()) || !dynamic_cast<OutPort*>(aCNPort->getPort()) )
    //if ( aChild->nextSibling()->text(0).compare( QString("Gate") ) )
      aChild->moveItem(aChild->nextSibling());
  }
}

bool YACSGui_NodeViewItem::isPublished( YACS::ENGINE::Port* thePort )
{
  QListViewItem* aChild = firstChild();
  while( aChild )
  {
    if ( YACSGui_PortViewItem* aPort = dynamic_cast<YACSGui_PortViewItem*>(aChild) )
      if ( aPort->getPort() == thePort )
	return true;
    aChild = aChild->nextSibling();
  }
  return false;
}

void YACSGui_NodeViewItem::addReferenceItem( YACS::HMI::Subject* theSRef )
{
  if ( SubjectReference* aSRef = dynamic_cast<SubjectReference*>(theSRef) )
  {
    // get component subject of the given reference
    if ( SubjectComponent* aSComp = dynamic_cast<SubjectComponent*>(aSRef->getReference()) )
    {     
      // get "Containers" label view item
      YACSGui_LabelViewItem* aContainersL = 
	dynamic_cast<YACSGui_LabelViewItem*>(listView()->firstChild()->firstChild()->nextSibling()->nextSibling()->nextSibling());
      if ( !aContainersL || aContainersL->text(0).compare(QString("Containers")) ) return;
  
      ComponentInstance* aComp = aSComp->getComponent();
      if ( aComp && aComp->getKind() != CORBAComponent::KIND )
      {
	// get container corresponds to the component
	Container* aCont = aComp->getContainer();
	if ( aCont )
	{
	  QListViewItem* aContItem = 0;
	  QListViewItem* aChild = aContainersL->firstChild();
	  while( aChild )
	  {
	    if ( YACSGui_ContainerViewItem* aC = dynamic_cast<YACSGui_ContainerViewItem*>(aChild) )
	      if ( aC->getContainer() == aCont )
	      {
		aContItem = aChild;
		break;
	      }
	    aChild = aChild->nextSibling();
	  }

	  if ( aContItem )
	  {
	    // find component view item corresponds to the aSComp
	    QListViewItem* aCompItem = 0;
	    QListViewItem* aChild = aContItem->firstChild();
	    while( aChild )
	    {
	      if ( YACSGui_ComponentViewItem* aC = dynamic_cast<YACSGui_ComponentViewItem*>(aChild) )
		if ( aC->getComponent() == aComp )
		{
		  aCompItem = aChild;
		  break;
		}
	      aChild = aChild->nextSibling();
	    }
	    
	    if ( aCompItem )
	    {
	      // add a new reference to the end
	      QListViewItem* anAfter = 0;
	      
	      if ( QListViewItem* aChild = aCompItem->firstChild() )
	      {
		while( aChild->nextSibling() )
		  aChild = aChild->nextSibling();
		anAfter = aChild;
	      }

	      new YACSGui_ReferenceViewItem( aCompItem, anAfter, aSRef );
	    }
	  }
	}
      }
      else
      { // it is a CORBA component
	// find component view item corresponds to the aSComp
	QListViewItem* aCompItem = 0;
	QListViewItem* aChild = aContainersL->firstChild();
	while( aChild )
	{
	  if ( YACSGui_ComponentViewItem* aC = dynamic_cast<YACSGui_ComponentViewItem*>(aChild) )
	    if ( aC->getComponent() == aComp )
	    {
	      aCompItem = aChild;
	      break;
	    }
	  aChild = aChild->nextSibling();
	}
	    
	if ( aCompItem )
	{
	  // add a new reference to the end
	  QListViewItem* anAfter = 0;
	  
	  if ( QListViewItem* aChild = aCompItem->firstChild() )
	  {
	    while( aChild->nextSibling() )
	      aChild = aChild->nextSibling();
	    anAfter = aChild;
	  }
	  
	  new YACSGui_ReferenceViewItem( aCompItem, anAfter, aSRef );
	}
      }
    }
  }
}

void YACSGui_NodeViewItem::removeReferenceItem( YACS::HMI::Subject* theSRef )
{
  if ( SubjectReference* aSRef = dynamic_cast<SubjectReference*>(theSRef) )
  {
    // get component subject of the given reference
    if ( SubjectComponent* aSComp = dynamic_cast<SubjectComponent*>(aSRef->getReference()) )
    {     
      // get "Containers" label view item
      YACSGui_LabelViewItem* aContainersL = 
	dynamic_cast<YACSGui_LabelViewItem*>(listView()->firstChild()->firstChild()->nextSibling()->nextSibling()->nextSibling());
      if ( !aContainersL || aContainersL->text(0).compare(QString("Containers")) ) return;
  
      ComponentInstance* aComp = aSComp->getComponent();
      if ( aComp && aComp->getKind() != CORBAComponent::KIND )
      {
	// get container corresponds to the component
	Container* aCont = aComp->getContainer();
	if ( aCont )
	{
	  QListViewItem* aContItem = 0;
	  QListViewItem* aChild = aContainersL->firstChild();
	  while( aChild )
	  {
	    if ( YACSGui_ContainerViewItem* aC = dynamic_cast<YACSGui_ContainerViewItem*>(aChild) )
	      if ( aC->getContainer() == aCont )
	      {
		aContItem = aChild;
		break;
	      }
	    aChild = aChild->nextSibling();
	  }

	  if ( aContItem )
	  {
	    // find component view item corresponds to the aSComp
	    QListViewItem* aCompItem = 0;
	    QListViewItem* aChild = aContItem->firstChild();
	    while( aChild )
	    {
	      if ( YACSGui_ComponentViewItem* aC = dynamic_cast<YACSGui_ComponentViewItem*>(aChild) )
		if ( aC->getComponent() == aComp )
		{
		  aCompItem = aChild;
		  break;
		}
	      aChild = aChild->nextSibling();
	    }
	    
	    if ( aCompItem )
	    {
	      // delete a given reference
	      if ( QListViewItem* aChild = aCompItem->firstChild() )
	      {
		while( aChild )
		{
		  if ( YACSGui_ReferenceViewItem* aRefItem = dynamic_cast<YACSGui_ReferenceViewItem*>(aChild) )
		    if ( aRefItem->getSReference() == aSRef )
		    {
		      aCompItem->takeItem(aChild);
		      delete aChild;
		      break;
		    }
		  aChild = aChild->nextSibling();
		}
	      }
	    }
	  }
	}
      }
      else
      { // it is a CORBA component
	// find component view item corresponds to the aSComp
	QListViewItem* aCompItem = 0;
	QListViewItem* aChild = aContainersL->firstChild();
	while( aChild )
	{
	  if ( YACSGui_ComponentViewItem* aC = dynamic_cast<YACSGui_ComponentViewItem*>(aChild) )
	    if ( aC->getComponent() == aComp )
	    {
	      aCompItem = aChild;
	      break;
	    }
	  aChild = aChild->nextSibling();
	}
	    
	if ( aCompItem )
	{
	  // delete a given reference
	  if ( QListViewItem* aChild = aCompItem->firstChild() )
	  {
	    while( aChild )
	    {
	      if ( YACSGui_ReferenceViewItem* aRefItem = dynamic_cast<YACSGui_ReferenceViewItem*>(aChild) )
		if ( aRefItem->getSReference() == aSRef )
		{
		  aCompItem->takeItem(aChild);
		  delete aChild;
		  break;
		}
	      aChild = aChild->nextSibling();
	    }
	  }
	}
      }
    }
  }
}

void YACSGui_NodeViewItem::addLinkItem( YACS::HMI::Subject* theSLink )
{
  SubjectLink* aSLink = dynamic_cast<SubjectLink*>(theSLink);
  SubjectControlLink* aSCLink = dynamic_cast<SubjectControlLink*>(theSLink);
  if ( aSLink || aSCLink )
  {
    // get the "Links" label view item under this node view item
    YACSGui_LabelViewItem* aLinksL = 0;
    QListViewItem* aChildLinks = firstChild();
    while( aChildLinks )
    {
      aLinksL = dynamic_cast<YACSGui_LabelViewItem*>(aChildLinks);
      if ( aLinksL && aLinksL->text(0).compare(QString("Links")) == 0 )
	break;
      aChildLinks = aChildLinks->nextSibling();
    }

    if ( !aLinksL )
      return;

    // find the last link item published under in the Links folder
    QListViewItem* anAfter = 0;
    if ( QListViewItem* aChild = aLinksL->firstChild() )
    {
      while( aChild->nextSibling() )
	aChild = aChild->nextSibling();
      anAfter = aChild;
    }

    if ( aSLink ) new YACSGui_LinkViewItem( aLinksL, anAfter, aSLink );
    else if ( aSCLink ) new YACSGui_ControlLinkViewItem( aLinksL, anAfter, aSCLink );
  }
}

// YACSGui_LinkViewItem class:

YACSGui_LinkViewItem::YACSGui_LinkViewItem( QListView* theParent, 
					    QListViewItem* theAfter,
					    YACS::HMI::SubjectLink* theSLink ):
  YACSGui_ViewItem( theParent, theAfter ),
  mySLink( theSLink )
{
  if ( mySLink )
  {
    mySLink->attach(this);
    
    QString aName = name();
    if ( !aName.isEmpty() )
      setText( 0, aName );
    setPixmap( 0, icon() );
  }
}

YACSGui_LinkViewItem::YACSGui_LinkViewItem( QListViewItem* theParent, 
					    QListViewItem* theAfter,
					    YACS::HMI::SubjectLink* theSLink ):
  YACSGui_ViewItem( theParent, theAfter ),
  mySLink( theSLink )
{
  if ( mySLink )
  {
    mySLink->attach(this);
    
    QString aName = name();
    if ( !aName.isEmpty() )
      setText( 0, aName );
    setPixmap( 0, icon() );
  }
}

YACSGui_LinkViewItem::~YACSGui_LinkViewItem()
{
  if ( mySLink ) mySLink->detach(this);
}

QString YACSGui_LinkViewItem::name() const
{
  QString aName = "";

  if ( !mySLink ) return aName;

  DataPort* anOutPort = mySLink->getSubjectOutPort()->getPort();
  DataPort* anInPort = mySLink->getSubjectInPort()->getPort();
  if ( anOutPort && anInPort )
  {
    Node* anOutPortNode = anOutPort->getNode();
    ComposedNode* aRootO = 0;
    if ( dynamic_cast<Bloc*>( anOutPortNode->getFather() ) )
      aRootO = anOutPortNode->getFather();
    else
      aRootO = anOutPortNode->getRootNode();

    Node* anInPortNode = anInPort->getNode();
    ComposedNode* aRootI = 0;
    if ( dynamic_cast<Bloc*>( anInPortNode->getFather() ) )
      aRootI = anInPortNode->getFather();
    else
      aRootI = anInPortNode->getRootNode();

    ComposedNode* aRoot = 0;
    if ( aRootO->isInMyDescendance(aRootI) )
      aRoot = aRootO;
    else if ( aRootI->isInMyDescendance(aRootO) )
      aRoot = aRootI;
    else
    {
      Bloc* aBlocOut = 0;
      Bloc* aBlocIn = 0;
      // find the nearest common ancestor for anOutNode and anInNode
      Proc* aProc = dynamic_cast<Proc*>(aRootI->getRootNode());
      if ( !aProc ) return aName;

      aRoot = aProc;
      while ( aRootI->getFather() != aProc )
      {
	if ( aBlocOut = dynamic_cast<Bloc*>(aRootI->getFather()->isInMyDescendance(aRootO)) )
	{
	  aBlocIn = dynamic_cast<Bloc*>(aRootI);
	  break;
	}
	aRootI = aRootI->getFather();
      }
      
      if ( aBlocOut && aBlocIn )
      	aRoot = aBlocOut->getFather();
    }

    if (aRoot)
      aName += QString(aRoot->getChildName(anOutPortNode));
    
    aName += QString(".%1").arg(anOutPort->getName());
    aName += QString(" --> ");
            
    if (aRoot)
      aName += QString(aRoot->getChildName(anInPortNode));
    
    aName += QString(".%1").arg(anInPort->getName());
  }

  return aName;
}

QPixmap YACSGui_LinkViewItem::icon() const
{
  QPixmap aRes;
  
  QString anIconName;

  if ( !mySLink ) return aRes;

  if ( DataPort* anOutPort = mySLink->getSubjectOutPort()->getPort() )
  {
    if ( dynamic_cast<OutputDataStreamPort*>( anOutPort ) )
      anIconName = QString(QObject::tr("ICON_STREAM_LINK_OBJECT"));
    else if ( dynamic_cast<OutputPort*>( anOutPort ) )
      anIconName = QString(QObject::tr("ICON_DATA_LINK_OBJECT"));

    if ( !anIconName.isEmpty() )
      aRes = SUIT_Session::session()->resourceMgr()->loadPixmap("YACSGui",  anIconName, false);
  }

  return aRes;
}

void YACSGui_LinkViewItem::update( const bool theIsRecursive )
{

}

// YACSGui_ControlLinkViewItem class:

YACSGui_ControlLinkViewItem::YACSGui_ControlLinkViewItem( QListView* theParent, 
							  QListViewItem* theAfter,
							  YACS::HMI::SubjectControlLink* theSLink ):
  YACSGui_ViewItem( theParent, theAfter ),
  mySLink( theSLink )
{
  if ( mySLink )
  {
    mySLink->attach(this);
    
    QString aName = name();
    if ( !aName.isEmpty() )
      setText( 0, aName );
    setPixmap( 0, icon() );
  }
}

YACSGui_ControlLinkViewItem::YACSGui_ControlLinkViewItem( QListViewItem* theParent, 
							  QListViewItem* theAfter,
							  YACS::HMI::SubjectControlLink* theSLink ):
  YACSGui_ViewItem( theParent, theAfter ),
  mySLink( theSLink )
{
  if ( mySLink )
  {
    mySLink->attach(this);
    
    QString aName = name();
    if ( !aName.isEmpty() )
      setText( 0, aName );
    setPixmap( 0, icon() );
  }
}
  
YACSGui_ControlLinkViewItem::~YACSGui_ControlLinkViewItem()
{
  if ( mySLink ) mySLink->detach(this);
}

QString YACSGui_ControlLinkViewItem::name() const
{
  QString aName = "";

  if ( !mySLink ) return aName;

  Node* anOutPortNode = mySLink->getSubjectOutNode()->getNode();
  ComposedNode* aRootO = 0;
  if ( dynamic_cast<Bloc*>( anOutPortNode->getFather() ) )
    aRootO = anOutPortNode->getFather();
  else
    aRootO = anOutPortNode->getRootNode();
  
  Node* anInPortNode = mySLink->getSubjectInNode()->getNode();
  ComposedNode* aRootI = 0;
  if ( dynamic_cast<Bloc*>( anInPortNode->getFather() ) )
    aRootI = anInPortNode->getFather();
  else
    aRootI = anInPortNode->getRootNode();

  ComposedNode* aRoot = 0;
  if ( aRootO->isInMyDescendance(aRootI) )
    aRoot = aRootO;
  else if ( aRootI->isInMyDescendance(aRootO) )
    aRoot = aRootI;
  else
  {
    Bloc* aBlocOut = 0;
    Bloc* aBlocIn = 0;
    // find the nearest common ancestor for anOutNode and anInNode
    Proc* aProc = dynamic_cast<Proc*>(aRootI->getRootNode());
    if ( !aProc ) return aName;
    
    while ( aRootI->getFather() != aProc )
    {
      if ( aBlocOut = dynamic_cast<Bloc*>(aRootI->getFather()->isInMyDescendance(aRootO)) )
      {
	aBlocIn = dynamic_cast<Bloc*>(aRootI);
	break;
      }
      aRootI = aRootI->getFather();
    }
    
    if ( aBlocOut && aBlocIn )
      aRoot = aBlocOut->getFather();
  }
  
  if (aRoot)
    aName += QString(aRoot->getChildName(anOutPortNode));

  aName += QString(" --> ");
    
  if (aRoot)
    aName += QString(aRoot->getChildName(anInPortNode));
    
  return aName;
}

QPixmap YACSGui_ControlLinkViewItem::icon() const
{
  QPixmap aRes;
  
  QString anIconName;

  anIconName = QString(QObject::tr("ICON_CONTROL_LINK_OBJECT"));
  if ( !anIconName.isEmpty() )
    aRes = SUIT_Session::session()->resourceMgr()->loadPixmap("YACSGui",  anIconName, false);
  
  return aRes;
}

void YACSGui_ControlLinkViewItem::update( const bool theIsRecursive )
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

YACSGui_LabelViewItem* YACSGui_SchemaViewItem::buildDataTypesTree()
{
  GuiContext* aContext = GuiContext::getCurrent();
  if ( !aContext )
    return 0;

  YACS::ENGINE::Proc* aProc = aContext->getProc();
  if ( !aProc )
    return 0;

  // Create "Data Types" label

  YACSGui_LabelViewItem* aDataTypesItem = 
    new YACSGui_LabelViewItem( this, 0, QObject::tr( "DT_DATA_TYPES" ) );

  // Create "Simple" label
  YACSGui_LabelViewItem* aSimpleItem = new YACSGui_LabelViewItem( aDataTypesItem, 0, QObject::tr( "DT_SIMPLE" ) );
  myRootDataTypeItems[ YACS::ENGINE::Double ] = aSimpleItem;
  myRootDataTypeItems[ YACS::ENGINE::Int ] = aSimpleItem;
  myRootDataTypeItems[ YACS::ENGINE::String ] = aSimpleItem;
  myRootDataTypeItems[ YACS::ENGINE::Bool ] = aSimpleItem;

  // Create "Objref" label
  myRootDataTypeItems[ YACS::ENGINE::Objref ] = 
    new YACSGui_LabelViewItem( aDataTypesItem, aSimpleItem, QObject::tr( "DT_OBJREF" ) );

  // Create "Sequence" label
  myRootDataTypeItems[ YACS::ENGINE::Sequence ] = new YACSGui_LabelViewItem( 
    aDataTypesItem, myRootDataTypeItems[ YACS::ENGINE::Objref ], QObject::tr( "DT_SEQUENCE" ) );

  // Create "Array" label
  myRootDataTypeItems[ YACS::ENGINE::Array ] = new YACSGui_LabelViewItem( 
    aDataTypesItem, myRootDataTypeItems[ YACS::ENGINE::Sequence ], QObject::tr( "DT_ARRAY" ) );

  // Create "Struct" label
  myRootDataTypeItems[ YACS::ENGINE::Struct ] = new YACSGui_LabelViewItem( 
    aDataTypesItem, myRootDataTypeItems[ YACS::ENGINE::Array ], QObject::tr( "DT_STRUCT" ) );

  // Iterate through DataTypes and build tree

  std::map<std::string, YACS::HMI::SubjectDataType*>::iterator it;
  for ( it = aContext->_mapOfSubjectDataType.begin(); 
        it != aContext->_mapOfSubjectDataType.end(); ++it )
  {
    std::string aName = (*it).first;
    YACS::HMI::SubjectDataType* aSub = (*it).second;
    if ( !aSub )
      continue;
    
    addDataTypeItem( aSub );
  }

  return aDataTypesItem;
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
  case ADD:
    switch (type)
    {
    case BLOC:
    case FOREACHLOOP:
    case OPTIMIZERLOOP:
    case FORLOOP:
    case WHILELOOP:
    case SWITCH:
    case PYTHONNODE:
    case PYFUNCNODE:
    case CORBANODE:
    case SALOMENODE:
    case CPPNODE:
    case SALOMEPYTHONNODE:
    case XMLNODE:
      {
	// add a node item (this = schema item)
	//printf("SchemaViewItem:  ADD node\n");
	addNodeItem(son);
      }
      break;
    case CONTAINER:
      {
	// add a container item (this = schema item)
	//printf("SchemaViewItem:  ADD container\n");
	addContainerItem(son);
      }
      break;
    case DATATYPE:
      {
	//printf("SchemaViewItem:  ADD data type\n");
	addDataTypeItem( son );
      }
      break;
    default:
      break;
    }
    break;
  case REMOVE:
    switch (type)
    {
    case CONTAINER:
      {
	// remove a container item (this = schema item)
	//printf("SchemaViewItem:  REMOVE container\n");
	removeContainerItem(son);
      }
      break;
    case COMPONENT:
      {
	// remove a CORBA component item (this = schema item)
	//printf("SchemaViewItem:  REMOVE component\n");
	removeComponentItem(son);
      }
      break;
    case DATATYPE:
      {
	// remove a CORBA component item (this = schema item)
	//printf("SchemaViewItem:  REMOVE Data type\n");
	removeDataTypeItem( son );
      }
      break;
    case DATALINK:
    case CONTROLLINK:
      {
	// remove a link item (this = schema item)
	//printf("SchemaViewItem:  REMOVE link\n");
	removeLinkItem(son);
      }
      break;
    case BLOC:
    case FOREACHLOOP:
    case OPTIMIZERLOOP:
    case FORLOOP:
    case WHILELOOP:
    case SWITCH:
    case PYTHONNODE:
    case PYFUNCNODE:
    case CORBANODE:
    case SALOMENODE:
    case CPPNODE:
    case SALOMEPYTHONNODE:
    case XMLNODE:
      {
	//printf("SchemaViewItem:  REMOVE\n");
	removeNodeItem(son);
      }
    default:
      break;
    }
    break;
  case ADDLINK:
  case ADDCONTROLLINK:
    {
      // add link item (this = schema item)
      //printf("SchemaViewItem:  ADDLINK\n");
      addLinkItem(son);
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
	    //printf(">> Delete and insert : %s\n",aChildNodeItem->getNode()->getName().c_str());
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

/*!
  \brief Add data type item in tree view
*/
void YACSGui_SchemaViewItem::addDataTypeItem( YACS::HMI::Subject* theSDataType )
{
  YACS::HMI::SubjectDataType* aSub = 
    dynamic_cast<YACS::HMI::SubjectDataType*>( theSDataType );

  if ( !aSub )
    return;
  
  GuiContext* aContext = GuiContext::getCurrent();
  if ( !aContext )
    return;

  YACS::ENGINE::Proc* aProc = aContext->getProc();
  if ( !aProc )
    return;

  std::string aName = aSub->getName();

  if ( aProc->typeMap.find( aName ) == aProc->typeMap.end() )
    return;

  YACS::ENGINE::TypeCode* aTypeCode = aProc->typeMap[ aName ];
  if ( !aTypeCode )
    return;

  DynType aDynType = aTypeCode->kind();

  // Create types item if necessary
  if ( myRootDataTypeItems.find( aDynType ) == myRootDataTypeItems.end() )
    return;
  
  QListViewItem* aFatherItem = myRootDataTypeItems[ aDynType ];

  // Create new item under aFatherItem
  new YACSGui_DataTypeItem( aFatherItem, 0, aSub );
}


void YACSGui_SchemaViewItem::addNodeItem( YACS::HMI::Subject* theSNode )
{
  if ( SubjectNode* aSNode = dynamic_cast<SubjectNode*>(theSNode) )
  {
    // get the "Nodes" label view item, which is used as a parent for first level nodes
    YACSGui_LabelViewItem* aNodesL = dynamic_cast<YACSGui_LabelViewItem*>(firstChild()->nextSibling());
    if ( !aNodesL || aNodesL->text(0).compare(QString("Nodes")) ) return;

    QListViewItem* anAfter = 0;

    if ( QListViewItem* aChild = aNodesL->firstChild() )
    {
      while( aChild->nextSibling() )
	aChild = aChild->nextSibling();
      anAfter = aChild;
    }

    YACSGui_NodeViewItem* aNodeItem = new YACSGui_NodeViewItem( aNodesL, anAfter, aSNode );
  }
}

void YACSGui_SchemaViewItem::addContainerItem( YACS::HMI::Subject* theSContainer )
{
  if ( SubjectContainer* aSContainer = dynamic_cast<SubjectContainer*>(theSContainer) )
  {
    // get the "Containers" label view item
    YACSGui_LabelViewItem* aContainersL = dynamic_cast<YACSGui_LabelViewItem*>(firstChild()->nextSibling()->nextSibling()->nextSibling());
    if ( !aContainersL || aContainersL->text(0).compare(QString("Containers")) ) return;

    QListViewItem* anAfter = 0;

    if ( QListViewItem* aChild = aContainersL->firstChild() )
    {
      while( aChild->nextSibling() )
	aChild = aChild->nextSibling();
      anAfter = aChild;
    }

    new YACSGui_ContainerViewItem( aContainersL, anAfter, aSContainer );
  }
}

void YACSGui_SchemaViewItem::removeContainerItem( YACS::HMI::Subject* theSContainer )
{
  if ( SubjectContainer* aSContainer = dynamic_cast<SubjectContainer*>(theSContainer) )
  {
    // get the "Containers" label view item
    YACSGui_LabelViewItem* aContainersL = dynamic_cast<YACSGui_LabelViewItem*>(firstChild()->nextSibling()->nextSibling()->nextSibling());
    if ( !aContainersL || aContainersL->text(0).compare(QString("Containers")) ) return;

    QListViewItem* aChild = aContainersL->firstChild();
    while( aChild )
    {
      if ( YACSGui_ContainerViewItem* aCont = dynamic_cast<YACSGui_ContainerViewItem*>(aChild) )
	if ( aCont->getSContainer() == aSContainer )
	{
	  aContainersL->takeItem(aChild);
	  delete aChild;
	  break;
	}
      aChild = aChild->nextSibling();
    }
  }
}

void YACSGui_SchemaViewItem::removeDataTypeItem( YACS::HMI::Subject* theSDataType )
{
  YACS::HMI::SubjectDataType* aSub = 
    dynamic_cast<YACS::HMI::SubjectDataType*>( theSDataType );

  if ( !aSub )
    return;
  
  GuiContext* aContext = GuiContext::getCurrent();
  if ( !aContext )
    return;

  YACS::ENGINE::Proc* aProc = aContext->getProc();
  if ( !aProc )
    return;

  std::string aName = aSub->getName();

  if ( aProc->typeMap.find( aName ) == aProc->typeMap.end() )
    return;

  YACS::ENGINE::TypeCode* aTypeCode = aProc->typeMap[ aName ];
  if ( !aTypeCode )
    return;

  DynType aDynType = aTypeCode->kind();
  if ( myRootDataTypeItems.find( aDynType ) == myRootDataTypeItems.end() )
    return;

  // find item corresponding to aSub

  QListViewItem* aFatherItem = myRootDataTypeItems[ aDynType ];

  QListViewItemIterator it( aFatherItem );
  QListViewItem* toRemoveItem = 0;
  while ( it.current() ) 
  {
    YACSGui_DataTypeItem* currItem = dynamic_cast<YACSGui_DataTypeItem*>( it.current() );
    if ( currItem && currItem->getSDataType() == aSub )
    {
      toRemoveItem = currItem;
      break;
    }
    ++it;
  }

  if ( !toRemoveItem )
    return;

  // remove item
  
  aFatherItem->takeItem( toRemoveItem );
  delete toRemoveItem;
}

void YACSGui_SchemaViewItem::removeComponentItem( YACS::HMI::Subject* theSComponent )
{
  // ! for CORBA components only
  if ( SubjectComponent* aSComponent = dynamic_cast<SubjectComponent*>(theSComponent) )
  {
    CORBAComponent* aCComp = dynamic_cast<CORBAComponent*>(aSComponent->getComponent());
    if ( !aCComp ) return;

    // get the "Containers" label view item
    YACSGui_LabelViewItem* aContainersL = dynamic_cast<YACSGui_LabelViewItem*>(firstChild()->nextSibling()->nextSibling()->nextSibling());
    if ( !aContainersL || aContainersL->text(0).compare(QString("Containers")) ) return;

    QListViewItem* aChild = aContainersL->firstChild();
    while( aChild )
    {
      if ( YACSGui_ComponentViewItem* aCont = dynamic_cast<YACSGui_ComponentViewItem*>(aChild) )
	if ( aCont->getSComponent() == aSComponent )
	{
	  aContainersL->takeItem(aChild);
	  delete aChild;
	  break;
	}
      aChild = aChild->nextSibling();
    }
  }
}

void YACSGui_SchemaViewItem::addLinkItem( YACS::HMI::Subject* theSLink )
{
  SubjectLink* aSLink = dynamic_cast<SubjectLink*>(theSLink);
  SubjectControlLink* aSCLink = dynamic_cast<SubjectControlLink*>(theSLink);
  if ( aSLink || aSCLink )
  {
    if ( aSLink && aSLink->getSubjectOutNode()->getParent() != mySProc ) return;

    if ( aSCLink && aSCLink->getSubjectOutNode()->getParent() != mySProc ) return;

    // get the "Links" label view item under the schema (root) view item
    YACSGui_LabelViewItem* aLinksL = dynamic_cast<YACSGui_LabelViewItem*>(firstChild()->nextSibling()->nextSibling());
    if ( !aLinksL || aLinksL->text(0).compare(QString("Links")) ) return;

    QListViewItem* anAfter = 0;

    if ( QListViewItem* aChild = aLinksL->firstChild() )
    {
      while( aChild->nextSibling() )
	aChild = aChild->nextSibling();
      anAfter = aChild;
    }

    if ( aSLink ) new YACSGui_LinkViewItem( aLinksL, anAfter, aSLink );
    else if ( aSCLink ) new YACSGui_ControlLinkViewItem( aLinksL, anAfter, aSCLink );
  }
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

void YACSGui_ContainerViewItem::update(YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son)
{
  printf(">> YACSGui_ContainerViewItem::update\n");
  switch (event)
  {
  case RENAME:
    update();
    break;
  case ADD:
    switch (type)
    {
    case COMPONENT:
      {
	// add a component item (this = container item)
	//printf("ContainerViewItem:  ADD component\n");
	addComponentItem(son);
      }
      break;
    default:
      break;
    }
    break;
  case REMOVE:
    switch (type)
    {
    case COMPONENT:
      {
	// remove a component item (this = container item)
	//printf("ContainerViewItem:  REMOVE component\n");
	removeComponentItem(son);
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

void YACSGui_ContainerViewItem::addComponentItem( YACS::HMI::Subject* theSComponent )
{
  if ( SubjectComponent* aSComponent = dynamic_cast<SubjectComponent*>(theSComponent) )
  {
    QListViewItem* anAfter = 0;

    if ( QListViewItem* aChild = firstChild() )
    {
      while( aChild->nextSibling() )
	aChild = aChild->nextSibling();
      anAfter = aChild;
    }
    
    new YACSGui_ComponentViewItem( this, anAfter, aSComponent );
  }
}

void YACSGui_ContainerViewItem::removeComponentItem( YACS::HMI::Subject* theSComponent )
{
  if ( SubjectComponent* aSComponent = dynamic_cast<SubjectComponent*>(theSComponent) )
  {
    QListViewItem* aChild = firstChild();
    while( aChild )
    {
      if ( YACSGui_ComponentViewItem* aComp = dynamic_cast<YACSGui_ComponentViewItem*>(aChild) )
	if ( aComp->getSComponent() == aSComponent )
	{
	  takeItem(aChild);
	  delete aChild;
	  break;
	}
      
      aChild = aChild->nextSibling();
    }
  }
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

void YACSGui_ComponentViewItem::update(YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son)
{
  printf(">> YACSGui_ComponentViewItem::update\n");
  switch (event)
  {
  case RENAME:
    update();
    break;
  case EDIT:
    switch (type)
    {
    case REFERENCE:
      move(son);
      break;
    default:
      update(true);
      break;
    }
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

void YACSGui_ComponentViewItem::move( YACS::HMI::Subject* theSReference )
{
  //printf( "YACSGui_ComponentViewItem::move %s\n", theSReference->getName().c_str() );

  SubjectServiceNode* aSServiceNode = dynamic_cast<SubjectServiceNode*>( theSReference );

  if( aSServiceNode )
  {
    ServiceNode* aServiceNode = dynamic_cast<ServiceNode*>( aSServiceNode->getNode() );

    // get the "Containers" label view item
    YACSGui_LabelViewItem* aContsL = 
      dynamic_cast<YACSGui_LabelViewItem*>(listView()->firstChild()->firstChild()->nextSibling()->nextSibling()->nextSibling());
    if ( !aContsL || aContsL->text(0).compare(QString("Containers")) ) return;

    // find new component view item, under which theReference view item should be moved
    YACSGui_ReferenceViewItem* aReference = 0;
    YACSGui_ComponentViewItem* aComponent = 0;
    QListViewItem* aChildContainer = aContsL->firstChild();
    while( aChildContainer )
    {
      //printf( "ContainerViewItem - %s\n", aChildContainer->text(0).latin1() );
      if( YACSGui_ContainerViewItem* aChildContItem = dynamic_cast<YACSGui_ContainerViewItem*>(aChildContainer) )
      {
	QListViewItem* aChildComponent = aChildContItem->firstChild();
	while( aChildComponent )
	{
	  //printf( "  ComponentViewItem - %s\n", aChildComponent->text(0).latin1() );
	  if( YACSGui_ComponentViewItem* aChildCompItem = dynamic_cast<YACSGui_ComponentViewItem*>(aChildComponent) )
	  {
	    QListViewItem* aChildReference = aChildCompItem->firstChild();
	    while( aChildReference )
	    {
	      //printf( "    ReferenceViewItem - %s\n", aChildReference->text(0).latin1() );
	      if( YACSGui_ReferenceViewItem* aChildRefItem = dynamic_cast<YACSGui_ReferenceViewItem*>(aChildReference) )
	      {
		if( !aReference && aChildRefItem->getNode() == aServiceNode )
		  aReference = aChildRefItem;
	      }
	      aChildReference = aChildReference->nextSibling();
	    }

	    if( !aComponent && aChildCompItem->getComponent() == aServiceNode->getComponent() )
	      aComponent = aChildCompItem;
	  }
	  aChildComponent = aChildComponent->nextSibling();
	}
      }
      aChildContainer = aChildContainer->nextSibling();
    }

    //printf( "Reference - %s\n", aReference->name().latin1() );
    takeItem( aReference );
    if( aComponent )
      aComponent->insertItem( aReference );
  }
}

/* ================ items for tree view in run mode ================ */

// YACSGui_ComposedNodeViewItem class:

YACSGui_ComposedNodeViewItem::YACSGui_ComposedNodeViewItem(QListView *parent,
							   QString label,
							   YACS::ENGINE::ComposedNode *node)
  : YACSGui_ViewItem(parent, 0),
    _node(node)
{
  setText( 0, label );

  _cf = Qt::black;
  setPixmap( 0, icon() );

  // attach to HMI 
  YACS::HMI::Subject* aSub = getSubject();
  if ( aSub )
    aSub->attach( this );
}

YACSGui_ComposedNodeViewItem::YACSGui_ComposedNodeViewItem(QListViewItem *parent,
							   QString label,
							   YACS::ENGINE::ComposedNode *node)
  : YACSGui_ViewItem(parent, 0),
    _node(node)
{
  setText( 0, label );

  _cf = Qt::black;

  // attach to HMI 
  YACS::HMI::Subject* aSub = getSubject();
  if ( aSub )
    aSub->attach( this );
}

YACSGui_ComposedNodeViewItem::~YACSGui_ComposedNodeViewItem()
{
  // detach from HMI 
  YACS::HMI::Subject* aSub = getSubject();
  if ( aSub )
    aSub->detach( this );
}

YACS::HMI::Subject* YACSGui_ComposedNodeViewItem::getSubject() const
{
  YACS::HMI::Subject* aSub = 0;

  if ( _node )
  {
    GuiContext* aContext = GuiContext::getCurrent();
    if ( aContext )
    {
      if ( aContext->_mapOfSubjectNode.find( _node ) != aContext->_mapOfSubjectNode.end() )
        aSub = aContext->_mapOfSubjectNode[ _node ];
    }
  }

  return aSub;
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
    case YACS::READY:        _cf=Qt::gray;            setText(1,"READY");       repaint(); break;
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

void YACSGui_ComposedNodeViewItem::popup(YACSGui_Executor* anExecutor,const QPoint & point)
{
  std::cerr << anExecutor->getErrorDetails(getNode()) << std::endl;
  QPopupMenu menu(listView());
  menu.insertItem("Error Details",0);
  menu.insertItem("Error Report",1);
  int id=menu.exec(point);
  if(id==0)
    {
      std::string msg = anExecutor->getErrorDetails(getNode());
      LogViewer* log=new LogViewer(msg,listView());
      log->show();
    }
  else if(id==1)
    {
      std::string msg = anExecutor->getErrorReport(getNode());
      LogViewer* log=new LogViewer(msg,listView());
      log->show();
    }
}

// YACSGui_ElementaryNodeViewItem class:

YACSGui_ElementaryNodeViewItem::YACSGui_ElementaryNodeViewItem(QListView *parent,
							       const QString &text,
							       Type tt,
							       YACS::ENGINE::ElementaryNode *node)
  : QCheckListItem(parent, text, tt),
    myBlockSelect( false )
{
  _cf = Qt::green;
  _node = node;

  // attach to HMI 
  YACS::HMI::Subject* aSub = getSubject();
  if ( aSub )
    aSub->attach( this );
}

YACSGui_ElementaryNodeViewItem::YACSGui_ElementaryNodeViewItem(QListViewItem *parent,
							       const QString &text,
							       Type tt,
							       YACS::ENGINE::ElementaryNode *node)
  : QCheckListItem(parent, text, tt),
    myBlockSelect( false )
{
  _cf = Qt::green;
  _node = node;

  // attach to HMI 
  YACS::HMI::Subject* aSub = getSubject();
  if ( aSub )
    aSub->attach( this );
}

YACSGui_ElementaryNodeViewItem::~YACSGui_ElementaryNodeViewItem()
{
  // detach from HMI 
  YACS::HMI::Subject* aSub = getSubject();
  if ( aSub )
    aSub->detach( this );
}

YACS::HMI::Subject* YACSGui_ElementaryNodeViewItem::getSubject() const
{
  YACS::HMI::Subject* aSub = 0;

  if ( _node )
  {
    GuiContext* aContext = GuiContext::getCurrent();
    if ( aContext )
    {
      if ( aContext->_mapOfSubjectNode.find( _node ) != aContext->_mapOfSubjectNode.end() )
        aSub = aContext->_mapOfSubjectNode[ _node ];
    }
  }

  return aSub;
}

/*!
  \brief Block actions performed in select() method. Usually this method is called 
         from select() in order to avoid circularity because of synchronization: 
         selection in 2D viewer --> selection in tree view --> selection in 2D viewer --> etc.
  \param toBlock block selection
  \return previous blocking value 
*/
bool YACSGui_ElementaryNodeViewItem::blockSelection( const bool toBlock )
{
  bool prevVal = myBlockSelect;
  myBlockSelect = toBlock;
  return prevVal;
}

/*!
  \brief Verifiee whether actions performed in select() method are blocked
         (see blockSelection method for more dscription)
  \return blocking value 
*/
bool YACSGui_ElementaryNodeViewItem::selectionBlocked() const
{
  return myBlockSelect;
}

/*!
  \brief Selects item in tree view 
  \param isSelected specifies whether item has to be selected
*/
void YACSGui_ElementaryNodeViewItem::select( bool isSelected )
{
  if ( selectionBlocked() )
    return;

  //printf(">> YACSGui_ViewItem::select( %d ) \n", isSelected );

  if ( !isSelected ) // do nothing
    return;

  // select item in tree view
  QListView* aListView = listView();
  if ( !aListView )
    return;

  blockSelection( true );
  aListView->clearSelection();
  aListView->setCurrentItem( this );
  aListView->setSelected( this, true );
  aListView->ensureItemVisible( this );
  blockSelection( false );
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
    case YACS::READY:        _cf=Qt::gray;            setText(1,"READY");        repaint(); break;
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

void YACSGui_ElementaryNodeViewItem::popup(YACSGui_Executor* anExecutor,const QPoint & point)
{
  QPopupMenu menu(listView());
  menu.insertItem("Error Details",0);
  menu.insertItem("Error Report",1);
  int id=menu.exec(point);
  if(id==0)
    {
      std::string msg = anExecutor->getErrorDetails(getNode());
      LogViewer* log=new LogViewer(msg,listView());
      log->show();
    }
  else if(id==1)
    {
      std::string msg = anExecutor->getErrorReport(getNode());
      LogViewer* log=new LogViewer(msg,listView());
      log->show();
    }
}
