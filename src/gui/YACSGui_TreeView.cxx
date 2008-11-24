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

#include <YACSGui_TreeView.h>

#include <YACSGui_TreeViewItem.h>
#include <YACSGui_InputPanel.h>
#include <YACSGui_Executor.h>

#include <SUIT_MessageBox.h>

#include <OutputDataStreamPort.hxx>
#include <CalStreamPort.hxx>

#include <ServiceNode.hxx>
#include <CORBANode.hxx>
#include <DataNode.hxx>
#include <PresetNode.hxx>
#include <OutNode.hxx>
#include <StudyNodes.hxx>
#include <InlineNode.hxx>
#include <Switch.hxx>
#include <ForLoop.hxx>
#include <ForEachLoop.hxx>
#include <WhileLoop.hxx>
#include <Bloc.hxx>
#include "Logger.hxx"
#include "LinkInfo.hxx"

#include <ComponentInstance.hxx>
#include <CORBAComponent.hxx>

#include <qpopupmenu.h>
#include <qtextedit.h>

#include <iostream> //for debug only

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace YACS::HMI;

using namespace std;

/*!
  Method returns vector of the internal control links for the given composed node
*/
std::vector< std::pair< OutGate*,InGate* > > getInternalControlLinks( ComposedNode* theNode )
{
  vector< pair< OutGate*,InGate* > > anInternalGateLinks;
  
  if (theNode)
  {
    list<Node *> constituents = theNode->edGetDirectDescendants();
    for(list<Node *>::iterator iter=constituents.begin();iter!=constituents.end();iter++)
    {
      // fill control links: check links going from output gate port of each direct child node
      OutGate* anOutGate = ( *iter )->getOutGate();
      if (anOutGate) {
	set<InGate *> anInGates = anOutGate->edSetInGate();
	set<InGate *>::iterator anInGatesIter = anInGates.begin();
	for(; anInGatesIter!=anInGates.end(); anInGatesIter++) {
	  InGate* aCurInGate = *anInGatesIter;
	  if ( find(constituents.begin(),constituents.end(),aCurInGate->getNode()) != constituents.end() )
	    anInternalGateLinks.push_back(pair<OutGate *, InGate *>(anOutGate,aCurInGate));
	}
      }
    }
  }  
   
  return anInternalGateLinks;
}

/*!
  Method returns vector of the internal data and stream links for the given composed node
*/
std::vector< std::pair< OutPort*,InPort* > > getInternalLinks( ComposedNode* theNode )
{
  vector< pair< OutPort*,InPort* > > anInternalLinks;
  
  if (theNode)
  {
    list<Node *> constituents = theNode->edGetDirectDescendants();
    list<OutPort *> temp;
    for(list<Node *>::iterator iter=constituents.begin();iter!=constituents.end();iter++)
    {
      if ( !dynamic_cast<ComposedNode*>( *iter ) )
      { // not cross link
	list<OutputPort *> currentsPorts=(*iter)->getSetOfOutputPort(); // only data links!
	temp.insert(temp.end(),currentsPorts.begin(),currentsPorts.end());

	list<OutputDataStreamPort *> currentsDataStreamPorts=(*iter)->getSetOfOutputDataStreamPort(); // only data stream links!
	temp.insert(temp.end(),currentsDataStreamPorts.begin(),currentsDataStreamPorts.end());
      }
      else
      { // cross links
	vector< pair<OutPort *, InPort *> > aCrossLinks = 
	  dynamic_cast<ComposedNode*>( *iter )->getSetOfLinksLeavingCurrentScope(); // data links and data stream links
	vector< pair< OutPort*,InPort* > >::iterator aCrossLinksIter = aCrossLinks.begin();
	for ( ; aCrossLinksIter != aCrossLinks.end(); aCrossLinksIter++ ) {
	  InPort* aCurInPort = (*aCrossLinksIter).second;
	  if ( find(constituents.begin(),constituents.end(),aCurInPort->getNode()) != constituents.end() )
	    temp.insert(temp.end(),(*aCrossLinksIter).first);
	}
      }
    }

    // fill data and stream links
    for(list<OutPort *>::iterator iter2=temp.begin();iter2!=temp.end();iter2++)
    {
      set<InPort *> temp2=(*iter2)->edSetInPort();
      for(set<InPort *>::iterator iter3=temp2.begin();iter3!=temp2.end();iter3++)
        if(theNode->isInMyDescendance((*iter3)->getNode()))
          anInternalLinks.push_back(pair<OutPort *, InPort *>((*iter2),(*iter3)));
    }
  }

  return anInternalLinks;
}

/*!
  Method returns vectors of all internal links for the given composed node
*/
void getAllInternalLinks( ComposedNode* theNode,
			  std::set< std::pair< OutGate*,InGate* > >& theInternalGateLinks,
			  std::set< std::pair< OutPort*,InPort* > >& theInternalLinks )
{
  if (theNode)
  {
    list<Node *> constituents = theNode->edGetDirectDescendants();
    list<OutPort *> temp;
    for(list<Node *>::iterator iter=constituents.begin();iter!=constituents.end();iter++)
    {
      if ( !dynamic_cast<ComposedNode*>( *iter ) )
      { // not cross link

	list<OutputPort *> currentsPorts=(*iter)->getSetOfOutputPort(); // only data links!
	temp.insert(temp.end(),currentsPorts.begin(),currentsPorts.end());

	list<OutputDataStreamPort *> currentsDataStreamPorts=(*iter)->getSetOfOutputDataStreamPort(); // only data stream links!
	temp.insert(temp.end(),currentsDataStreamPorts.begin(),currentsDataStreamPorts.end());
      }
      else
      { // cross links
	vector< pair<OutPort *, InPort *> > aCrossLinks = 
	  dynamic_cast<ComposedNode*>( *iter )->getSetOfLinksLeavingCurrentScope(); // data links and data stream links
	vector< pair< OutPort*,InPort* > >::iterator aCrossLinksIter = aCrossLinks.begin();
	for ( ; aCrossLinksIter != aCrossLinks.end(); aCrossLinksIter++ ) {
	  InPort* aCurInPort = (*aCrossLinksIter).second;
	  if ( find(constituents.begin(),constituents.end(),aCurInPort->getNode()) != constituents.end() )
	    temp.insert(temp.end(),(*aCrossLinksIter).first);
	}
      }

      // fill control links: check links going from output gate port of each direct child node
      OutGate* anOutGate = ( *iter )->getOutGate();
      if (anOutGate) {
	set<InGate *> anInGates = anOutGate->edSetInGate();
	set<InGate *>::iterator anInGatesIter = anInGates.begin();
	for(; anInGatesIter!=anInGates.end(); anInGatesIter++) {
	  InGate* aCurInGate = *anInGatesIter;
	  if ( find(constituents.begin(),constituents.end(),aCurInGate->getNode()) != constituents.end() )
	    theInternalGateLinks.insert(pair<OutGate *, InGate *>(anOutGate,aCurInGate));
	}
      }
    }

    // fill data and stream links
    for(list<OutPort *>::iterator iter2=temp.begin();iter2!=temp.end();iter2++)
    {
      set<InPort *> temp2=(*iter2)->edSetInPort();
      for(set<InPort *>::iterator iter3=temp2.begin();iter3!=temp2.end();iter3++)
        if(theNode->isInMyDescendance((*iter3)->getNode()))
          theInternalLinks.insert(pair<OutPort *, InPort *>((*iter2),(*iter3)));
    }
  }
}


//! Constructor.
/*!
 */
YACSGui_TreeView::YACSGui_TreeView( YACSGui_Module* theModule, YACS::HMI::SubjectProc* theSProc,
				    QWidget* theParent, const char* theName, WFlags theFlags )
  : QListView( theParent, theName, theFlags ),
    myModule( theModule ),
    mySProc( theSProc )
{
  setSorting( -1 );
  setRootIsDecorated( true );
  setSelectionMode( QListView::Extended );
  //viewport()->installEventFilter( this );

  connect( this, SIGNAL( contextMenuRequested( QListViewItem*, const QPoint&, int ) ), 
	   this, SLOT  ( onContextMenuRequested( QListViewItem*, const QPoint& ) ) );
}

//! Destructor.
/*!
 */
YACSGui_TreeView::~YACSGui_TreeView()
{
}

//! Gets the list of selected objects
/*!
 */
std::list<QListViewItem*> YACSGui_TreeView::getSelected()
{
  list<QListViewItem*> aSelList;
  for ( QListViewItemIterator it( this ); it.current(); ++it )
    if ( it.current()->isSelected() ) 
      aSelList.push_back( it.current() );
  return aSelList;
}

//! Gets the engine Proc* object from the subject proc
/*!
 */
YACS::ENGINE::Proc* YACSGui_TreeView::getProc() const
{
  return ( mySProc ? dynamic_cast<Proc*>(mySProc->getNode()) : 0 );
}

//! Builds the tree view according to the schema content. Schema is used as the root object.
/*!
 */
void YACSGui_TreeView::build()
{

}

void YACSGui_TreeView::removeLastSelected(YACSGui_ViewItem* v)
{
}

//! Udates the tree view starting from the given item.
/*!
 *  \param theItem - the item, which subtree have to be updated.
 *  \param theIsRecursive - the flag for recursive or not recursive update.
 *                          By default theIsRecursive = false.
 */
void YACSGui_TreeView::update( QListViewItem* theItem, const bool theIsRecursive )
{
}

//! Constructor.
/*!
 */
YACSGui_EditionTreeView::YACSGui_EditionTreeView( YACSGui_Module* theModule, YACS::HMI::SubjectProc* theSProc,
						  QWidget* theParent, const char* theName, WFlags theFlags )
  : YACSGui_TreeView( theModule, theSProc, theParent, theName, theFlags )
{
  if ( getProc() )
  {
    setCaption( tr( "EDIT_TREE_VIEW" ) + QString( " of " ) + QString( getProc()->getName() ) );
    
    addColumn( "Object", 250 );
    
    fillContainerData(mySProc);
    build();

    myPreviousSelected = 0;
    itemToCheck=0;
    mySelectedSubjectOutPort = 0;
    mySelectedSubjectOutNode = 0;

    connect( this, SIGNAL( doubleClicked( QListViewItem* ) ), this, SLOT( onDblClick( QListViewItem* ) ) );
    connect( this, SIGNAL( selectionChanged() ), this, SLOT( onSelectionChanged() ) );
  }
}

//! Destructor.
/*!
 */
YACSGui_EditionTreeView::~YACSGui_EditionTreeView()
{
}

//! Udates the tree view starting from the given item.
/*!
 *  \param theItem - the item, which subtree have to be updated.
 *  \param theIsRecursive - the flag for recursive or not recursive update.
 *                          By default theIsRecursive = false.
 */
void YACSGui_EditionTreeView::update( QListViewItem* theItem, const bool theIsRecursive )
{
  YACSGui_ViewItem* anItem = dynamic_cast<YACSGui_ViewItem*>( theItem );

  if ( !anItem )
    anItem = dynamic_cast<YACSGui_ViewItem*>( firstChild() ); // get a root item

  if ( anItem && anItem->listView() == this )
  {
    anItem->update( theIsRecursive );
    //triggerUpdate();
  }
}

/*!
  Method for displaying the node with all its subnodes in the tree view
*/
YACSGui_NodeViewItem* YACSGui_EditionTreeView::displayNodeWithPorts( QListViewItem* theRoot,
								     YACSGui_NodeViewItem* theAfter,
								     SubjectNode* theSNode )
{
  YACSGui_NodeViewItem* aNodeItem = 0;

  if ( !theSNode ) return aNodeItem;

  // Display the given node
  aNodeItem = new YACSGui_NodeViewItem( theRoot, theAfter, theSNode );
  
  displayChildren( aNodeItem );

  return aNodeItem;
}

/*!
  Method for displaying all nodes ports and subnodes in the tree view
*/
void YACSGui_EditionTreeView::displayChildren( YACSGui_NodeViewItem* theNodeItem )
{
  if ( !theNodeItem ) return;

  SubjectComposedNode* aSComposedNode = dynamic_cast<SubjectComposedNode*>( theNodeItem->getSNode() );
  Node* aNode = theNodeItem->getNode();
  ComposedNode* aComposedNode = dynamic_cast<ComposedNode*>( aNode );

  // Process all subnodes of a composed node
  YACSGui_NodeViewItem* aSubNodeItem = 0;
  if (aSComposedNode && aComposedNode) {
    list<Node*> aChildNodeSet = aComposedNode->edGetDirectDescendants();
    list<Node*>::const_iterator aChildIter = aChildNodeSet.begin();
    for(;aChildIter!=aChildNodeSet.end();aChildIter++)
      aSubNodeItem = displayNodeWithPorts( theNodeItem, aSubNodeItem, aSComposedNode->getChild(*aChildIter) );
  }

  // Display ports of a given node
    
  // input ports

  QListViewItem* anInPortItem = aSubNodeItem;
  if (!aComposedNode) {
    list<InPort *> anInPorts = aNode->getSetOfInPort();
 
    list<InPort *>::iterator InPortsIter = anInPorts.begin(); 
    for(; InPortsIter != anInPorts.end(); InPortsIter++)
      anInPortItem = new YACSGui_PortViewItem( theNodeItem, anInPortItem,
					       GuiContext::getCurrent()->_mapOfSubjectDataPort[*InPortsIter] );
  }
  else if ( ForLoop* aForLoopNode = dynamic_cast<ForLoop*>( aNode ) ) {
    InputPort* anInputPort = aForLoopNode->edGetNbOfTimesInputPort();
    if (anInputPort)
      anInPortItem = new YACSGui_PortViewItem( theNodeItem, aSubNodeItem,
					       GuiContext::getCurrent()->_mapOfSubjectDataPort[anInputPort] );
  }
  else if ( Switch* aSwitchNode = dynamic_cast<Switch*>( aNode ) ) {
    InputPort* anInputPort = aSwitchNode->edGetConditionPort();
    if (anInputPort)
      anInPortItem = new YACSGui_PortViewItem( theNodeItem, aSubNodeItem,
					       GuiContext::getCurrent()->_mapOfSubjectDataPort[anInputPort] );
  }
  else if ( WhileLoop* aWhileLoopNode = dynamic_cast<WhileLoop*>( aNode ) ) {
    InputPort* anInputPort = aWhileLoopNode->edGetConditionPort();
    if (anInputPort)
      anInPortItem = new YACSGui_PortViewItem( theNodeItem, aSubNodeItem,
					       GuiContext::getCurrent()->_mapOfSubjectDataPort[anInputPort] );
  }
  else if ( ForEachLoop* aForEachLoopNode = dynamic_cast<ForEachLoop*>( aNode ) ) {
    InputPort* anInputPort1 = aForEachLoopNode->edGetNbOfBranchesPort();
    if (anInputPort1)
      anInPortItem = new YACSGui_PortViewItem( theNodeItem, aSubNodeItem,
					       GuiContext::getCurrent()->_mapOfSubjectDataPort[anInputPort1] );
    InputPort* anInputPort2 = aForEachLoopNode->edGetSeqOfSamplesPort();
    if (anInputPort2)
      anInPortItem = new YACSGui_PortViewItem( theNodeItem, anInPortItem,
					       GuiContext::getCurrent()->_mapOfSubjectDataPort[anInputPort2] );
  }
  
  //InGate* anInGate = aNode->getInGate();
  //if (anInGate)
  //  anInPortItem = new YACSGui_PortViewItem( theNodeItem, anInPortItem, anInGate );
  
  // output ports
  
  QListViewItem* anOutPortItem = anInPortItem;
  if (!aComposedNode) {
    list<OutPort *> anOutPorts = aNode->getSetOfOutPort();
          
    list<OutPort *>::iterator OutPortsIter = anOutPorts.begin();
    for(; OutPortsIter != anOutPorts.end(); OutPortsIter++)
      anOutPortItem = new YACSGui_PortViewItem( theNodeItem, anOutPortItem,
						GuiContext::getCurrent()->_mapOfSubjectDataPort[*OutPortsIter] );
  }
  else if ( ForEachLoop* aForEachLoopNode = dynamic_cast<ForEachLoop*>( aNode ) ) {
    OutputPort* anOutputPort = aForEachLoopNode->edGetSamplePort();
    if (anOutputPort)
      anOutPortItem = new YACSGui_PortViewItem( theNodeItem, anInPortItem,
						GuiContext::getCurrent()->_mapOfSubjectDataPort[anOutputPort] );
  }
    
  //OutGate* anOutGate = aNode->getOutGate();
  //if (anOutGate)
  //  anOutPortItem = new YACSGui_PortViewItem( theNodeItem, anOutPortItem, anOutGate );
  
  if (aComposedNode)
  {
    set< pair< OutGate*,InGate* > > anInternalGateLinks;
    set< pair< OutPort*,InPort* > > anInternalLinks;
    getAllInternalLinks( aComposedNode, anInternalGateLinks, anInternalLinks );

    // Create internal links

    // Create "Links" label
    YACSGui_LabelViewItem* aLinksItem = new YACSGui_LabelViewItem( theNodeItem, anOutPortItem, tr( "LINKS" ) );

    // Control links
    QListViewItem* aControlLinkItem = 0;
    set< pair< OutGate*,InGate* > >::iterator aGateLinksIter = anInternalGateLinks.begin();
    for ( ; aGateLinksIter != anInternalGateLinks.end(); aGateLinksIter++ )
    {
      pair<Node*,Node*> On2IN = make_pair( (*aGateLinksIter).first->getNode(), (*aGateLinksIter).second->getNode() );
      aControlLinkItem = new YACSGui_ControlLinkViewItem( aLinksItem, aControlLinkItem,
							  GuiContext::getCurrent()->_mapOfSubjectControlLink[On2IN] );
    }
    
    // Process out ports list of the node : Data links
    QListViewItem* aDataLinkItem = aControlLinkItem;
    set< pair< OutPort*,InPort* > >::iterator aLinksIter = anInternalLinks.begin();
    for ( ; aLinksIter != anInternalLinks.end(); aLinksIter++ ) {
      OutPort* aCurOutPort = (*aLinksIter).first;
      string aClassName = aCurOutPort->getNameOfTypeOfCurrentInstance();
      
      // Data links
      if ( !aClassName.compare(OutputPort::NAME) ) {
	OutputPort* anOutputDFPort = dynamic_cast<OutputPort*>( aCurOutPort );
	if ( anOutputDFPort )
	  aDataLinkItem = new YACSGui_LinkViewItem( aLinksItem, aDataLinkItem,
						    GuiContext::getCurrent()->_mapOfSubjectLink[*aLinksIter] );
      }
    } 

    // Process out ports list of the node : Stream links
    QListViewItem* aStreamLinkItem = aDataLinkItem;
    aLinksIter = anInternalLinks.begin();
    for ( ; aLinksIter != anInternalLinks.end(); aLinksIter++ ) {
      OutPort* aCurOutPort = (*aLinksIter).first;
      string aClassName = aCurOutPort->getNameOfTypeOfCurrentInstance();
      
      // Stream links
      if ( !aClassName.compare(OutputDataStreamPort::NAME) || !aClassName.compare(OutputCalStreamPort::NAME) ) {
	OutputDataStreamPort* anOutputDSPort = dynamic_cast<OutputDataStreamPort*>( aCurOutPort );
	if ( anOutputDSPort ) 
	  aStreamLinkItem = new YACSGui_LinkViewItem( aLinksItem, aStreamLinkItem,
						      GuiContext::getCurrent()->_mapOfSubjectLink[*aLinksIter] );
      }
    }
  }

}

/*!
  Method returns a subject of given tree viewitem 
*/

YACS::HMI::Subject* YACSGui_EditionTreeView::getSubject( QListViewItem* theItem )
{
  Subject* aSub = 0;

  if ( YACSGui_ReferenceViewItem* aRefItem = dynamic_cast<YACSGui_ReferenceViewItem*>( theItem ) )
    aSub = aRefItem->getSReference();
  if ( YACSGui_PortViewItem* aPortItem = dynamic_cast<YACSGui_PortViewItem*>( theItem ) )
    aSub = aPortItem->getSPort();
  else if ( YACSGui_NodeViewItem* aNodeItem = dynamic_cast<YACSGui_NodeViewItem*>( theItem ) )
    aSub = aNodeItem->getSNode();
  else if ( YACSGui_LinkViewItem* aLinkItem = dynamic_cast<YACSGui_LinkViewItem*>( theItem ) )
    aSub = aLinkItem->getSLink();
  else if ( YACSGui_ControlLinkViewItem* aCLinkItem = dynamic_cast<YACSGui_ControlLinkViewItem*>( theItem ) )
    aSub = aCLinkItem->getSLink();
  else if ( YACSGui_SchemaViewItem* aSchemaItem = dynamic_cast<YACSGui_SchemaViewItem*>( theItem ) )
    aSub = aSchemaItem->getSProc();
  else if ( YACSGui_ContainerViewItem* aContItem = dynamic_cast<YACSGui_ContainerViewItem*>( theItem ) )
    aSub = aContItem->getSContainer();
  else if ( YACSGui_ComponentViewItem* aCompItem = dynamic_cast<YACSGui_ComponentViewItem*>( theItem ) )
    aSub = aCompItem->getSComponent();

  return aSub;
}

/*!
  Method returns a subject of the selected tree viewitem if the selection is a single selection or 0, otherwise
*/
YACS::HMI::Subject* YACSGui_EditionTreeView::getSelectedSubject()
{
  std::list<QListViewItem*> aSelList = getSelected();
  QListViewItem* anItem = 0;
  
  // check if the current selection is a single selection
  if ( aSelList.size() == 1 ) anItem = aSelList.front();
   
  return getSubject( anItem );
}

//! Public slot.  Adds the selected node into the library.
/*!
 */
void YACSGui_EditionTreeView::onAddToLibrary()
{

}

//! Public slot.  Adds the data (flow or stream) / control link from the selected port / node to the other (selected later).
/*!
 */
void YACSGui_EditionTreeView::onAddLink()
{
  control=true;
  if ( Subject* aSub = getSelectedSubject() )
  {
    if ( dynamic_cast<SubjectOutputPort*>(aSub) || dynamic_cast<SubjectOutputDataStreamPort*>(aSub) )
      mySelectedSubjectOutPort = static_cast<SubjectDataPort*>(aSub);
    else if (dynamic_cast<SubjectNode*>(aSub))
      mySelectedSubjectOutNode = static_cast<SubjectNode*>(aSub);
  }
}

//! Public slot.  Adds a simple data link from the selected port / node to the other (selected later).
/*!
 * does not add a control link
 */
void YACSGui_EditionTreeView::onAddDataLink()
{
  control=false;
  if ( Subject* aSub = getSelectedSubject() )
  {
    if ( dynamic_cast<SubjectOutputPort*>(aSub) || dynamic_cast<SubjectOutputDataStreamPort*>(aSub) )
      mySelectedSubjectOutPort = static_cast<SubjectDataPort*>(aSub);
  }
}

//! Public slot.  Copies the selected item.
/*!
 */
void YACSGui_EditionTreeView::onCopyItem()
{

}

//! Public slot.  Pastes an item.
/*!
 */
void YACSGui_EditionTreeView::onPasteItem()
{

}

//! Public slot.  Deletes the selected item.
/*!
 */
void YACSGui_EditionTreeView::onDeleteItem()
{
  DEBTRACE("YACSGui_EditionTreeView::onDeleteItem");
  if ( Subject* aSub = getSelectedSubject() )
  {
    clearSelection();
    
    if ( SubjectLink* aSLink = dynamic_cast<SubjectLink*>(aSub) )
    {
      DEBTRACE("delete data link");
      SubjectNode* aNode = aSLink->getSubjectOutNode();
      if ( !aSLink->getParent()->destroy( aSLink ) )
        {
          SUIT_MessageBox::warn1(myModule->getApp()->desktop(), 
                               tr("WARNING"), 
                               GuiContext::getCurrent()->_lastErrorMessage,
                               tr("BUT_OK"));
          //To do restore the link views
        }
      else 
        {
          myModule->temporaryExport();
        }
    }
    else if ( SubjectControlLink* aSCLink = dynamic_cast<SubjectControlLink*>(aSub) )
    {
      DEBTRACE("control link");
      Subject* aParent = aSCLink->getParent();
      if ( !aParent ) return;
      aParent->destroy( aSCLink );
      myModule->temporaryExport();
    }
    else if ( SubjectDataPort* aSDPort = dynamic_cast<SubjectDataPort*>(aSub) )
    {
      DEBTRACE("port");
      Subject* aNode = aSDPort->getParent();
      if ( !aNode ) return;
      aNode->destroy( aSDPort );
      myModule->temporaryExport();
    }
    else if ( SubjectNode* aSNode = dynamic_cast<SubjectNode*>(aSub) )
    {
      DEBTRACE("node");
      SubjectComposedNode* aSParent = dynamic_cast<SubjectComposedNode*>(aSNode->getParent());
      if ( !aSParent ) return;
      aSParent->destroy( aSNode );
      myModule->temporaryExport();
    }
    else if ( SubjectComponent* aSComp = dynamic_cast<SubjectComponent*>(aSub) )
    {
      DEBTRACE("component");
      Subject* aProc = aSComp->getParent();
      if ( !aProc ) return;
      aProc->destroy( aSComp );
      myModule->temporaryExport();
    }
    else if ( SubjectContainer* aSCont = dynamic_cast<SubjectContainer*>(aSub) )
    {
      DEBTRACE("container");
      SubjectProc* aProc = dynamic_cast<SubjectProc*>(aSCont->getParent());
      if ( !aProc ) return;
      aProc->destroy( aSCont );
      myModule->temporaryExport();
    }
  }
}

void YACSGui_EditionTreeView::removeLastSelected(YACSGui_ViewItem* v)
{
  DEBTRACE("YACSGui_EditionTreeView::removeLastSelected:"<<v);
  myLastSelected.erase(v);
}

//! Public slot.  Display 2D view of the selected schema.
/*!
 */
void YACSGui_EditionTreeView::onDisplaySchema()
{

}

//! Public slot.  Erase 2D view of the selected schema.
/*!
 */
void YACSGui_EditionTreeView::onEraseSchema()
{

}

//! Public slot.  Display 2D presentation of the selected node.
/*!
 */
void YACSGui_EditionTreeView::onDisplayNode()
{

}

//! Public slot.  Erase 2D presentation of the selected node.
/*!
 */
void YACSGui_EditionTreeView::onEraseNode()
{

}

//! Protected slot.  Shows context popup menu.
/*!
 */
void YACSGui_EditionTreeView::onContextMenuRequested( QListViewItem* theItem, const QPoint& thePoint )
{
  list<QListViewItem*> aSelList = getSelected();
  if ( aSelList.size() > 1 )
    return;

  if ( YACSGui_LabelViewItem* aLIt = dynamic_cast<YACSGui_LabelViewItem*>( theItem ) )
  {
    if ( !aLIt->name().compare( QString("Data Types") ) )
      showPopup( contextMenuPopup( YACSGui_EditionTreeView::DataTypeItem ), thePoint );
    if ( !aLIt->name().compare( QString("Nodes") ) )
      showPopup( contextMenuPopup( YACSGui_EditionTreeView::LabelNodesItem ), thePoint );
    if ( !aLIt->name().compare( QString("Containers") ) )
      showPopup( contextMenuPopup( YACSGui_EditionTreeView::LabelContainersItem ), thePoint );
  }
  else if ( dynamic_cast<YACSGui_SchemaViewItem*>( theItem ) )
  {
    showPopup( contextMenuPopup( YACSGui_TreeView::SchemaItem ), thePoint );
  }
  else if ( YACSGui_NodeViewItem* aNIt = dynamic_cast<YACSGui_NodeViewItem*>( theItem ) )
  {
    if ( dynamic_cast<ServiceNode*>( aNIt->getNode() ) )
      showPopup( contextMenuPopup( YACSGui_EditionTreeView::ServiceNodeItem ), thePoint );
    else if ( dynamic_cast<Bloc*>( aNIt->getNode() ) || dynamic_cast<Switch*>( aNIt->getNode() ) )
      showPopup( contextMenuPopup( YACSGui_EditionTreeView::ComposedNodeItem ), thePoint );
    else if ( dynamic_cast<Loop*>( aNIt->getNode() ) || dynamic_cast<ForEachLoop*>( aNIt->getNode() ) )
      showPopup( contextMenuPopup( YACSGui_EditionTreeView::LoopNodeItem ), thePoint );
    else
      showPopup( contextMenuPopup( YACSGui_TreeView::NodeItem ), thePoint );
  }
  else if ( YACSGui_PortViewItem* aPIt = dynamic_cast<YACSGui_PortViewItem*>( theItem ) )
  {
    showPopup( contextMenuPopup( YACSGui_EditionTreeView::PortItem, aPIt->getSPort() ), thePoint );
  }
  else if ( dynamic_cast<YACSGui_LinkViewItem*>( theItem ) || dynamic_cast<YACSGui_ControlLinkViewItem*>( theItem ) )
  {
    showPopup( contextMenuPopup( YACSGui_EditionTreeView::LinkItem ), thePoint );
  }
  else if ( dynamic_cast<YACSGui_ContainerViewItem*>( theItem ) )
  {
    showPopup( contextMenuPopup( YACSGui_EditionTreeView::ContainerItem ), thePoint );
  }
  else if ( YACSGui_ComponentViewItem* aCIt = dynamic_cast<YACSGui_ComponentViewItem*>( theItem ) )
  {
    if ( aCIt->getComponent() && aCIt->getComponent()->getKind() == CORBAComponent::KIND )
      showPopup( contextMenuPopup( YACSGui_EditionTreeView::CorbaComponentItem ), thePoint );
    else
      showPopup( contextMenuPopup( YACSGui_EditionTreeView::ComponentItem ), thePoint );
  }
}

/*!
  \brief Builds tree for data types
  \param theSchemaItemL schema item
  \return "Data types" item
  */
YACSGui_LabelViewItem* YACSGui_EditionTreeView::buildDataTypesTree( 
  YACSGui_SchemaViewItem* theSchemaItem )
{
  return theSchemaItem->buildDataTypesTree();
}

//! Builds the tree view according to the schema content. Schema is used as the root object.
/*!
 */
void YACSGui_EditionTreeView::build()
{
  if ( !getProc() ) return;

  // Create a schema root object
  YACSGui_SchemaViewItem* aSchemaItem = new YACSGui_SchemaViewItem( this, 0, mySProc );
   
  YACSGui_LabelViewItem* aDataTypesItem = buildDataTypesTree( aSchemaItem );
  if ( !aDataTypesItem )
    return;

  // Create "Nodes" label if necessary
  YACSGui_LabelViewItem* aNodesItem = 0;
  list<Node*> aDirectNodeSet = getProc()->edGetDirectDescendants();
  //if (aDirectNodeSet.size() > 0)
  aNodesItem = new YACSGui_LabelViewItem( aSchemaItem, aDataTypesItem, tr( "NODES" ) );
  
  // Put all nodes under "Nodes" label
  if ( aNodesItem )
  {
    YACSGui_NodeViewItem* aNodeItem = 0;
    for ( list<Node*>::iterator it = aDirectNodeSet.begin(); it != aDirectNodeSet.end(); it++ )
      aNodeItem = displayNodeWithPorts( aNodesItem, aNodeItem, mySProc->getChild(*it) );
  }
  
  // Create "Links" label
  YACSGui_LabelViewItem* aLinksItem = new YACSGui_LabelViewItem( aSchemaItem, 
								 aNodesItem ? aNodesItem : aDataTypesItem,
								 tr( "LINKS" ) );
  
  // Put all links under "Links" label
  list<Node*> aNodeSet = getProc()->getAllRecursiveConstituents();
  for (  list<Node*>::iterator it = aNodeSet.begin(); it != aNodeSet.end(); it++ ) {
    Node* aCurNode = *it;
    if (!aCurNode) continue;
    
    set< pair< OutGate*,InGate* > > anInternalGateLinks;
    set< pair< OutPort*,InPort* > > anInternalLinks;
    if ( ComposedNode* aComposedNode = aCurNode->getFather() )
      if ( !dynamic_cast<Proc*>( aComposedNode ) )
	getAllInternalLinks( aComposedNode, anInternalGateLinks, anInternalLinks );

    // Control links
    QListViewItem* aLinkItem = 0;
    OutGate* anOutGate = aCurNode->getOutGate();
    if (anOutGate) {
      set<InGate *> anInGates = anOutGate->edSetInGate();
      set<InGate *>::iterator anInGatesIter = anInGates.begin();
      for(; anInGatesIter!=anInGates.end(); anInGatesIter++) {
	InGate* aCurInGate = *anInGatesIter;
	
	if ( anInternalGateLinks.find( make_pair< OutGate*,InGate* >(anOutGate, aCurInGate) ) == anInternalGateLinks.end() )
	{
	  pair<Node*,Node*> On2IN = make_pair( anOutGate->getNode(), aCurInGate->getNode() );
	  aLinkItem = new YACSGui_ControlLinkViewItem( aLinksItem, aLinkItem,
						       GuiContext::getCurrent()->_mapOfSubjectControlLink[On2IN] );
	}
      }
    }
      
    // Get list of current node out ports
    list<OutPort*> anOutPorts;
    
    if ( !dynamic_cast<ComposedNode*>( aCurNode ) )
      anOutPorts = aCurNode->getSetOfOutPort();
    else if ( ForEachLoop* aForEachLoopNode = dynamic_cast<ForEachLoop*>(aCurNode) ) {
      OutputPort* anOutputPort = aForEachLoopNode->edGetSamplePort();
      if (anOutputPort)
	anOutPorts.push_back(anOutputPort);
    }
      
    // Process out ports list of the node : Data links
    list<OutPort*>::iterator anOutPortsIter = anOutPorts.begin();
    QListViewItem* aDataLinkItem = aLinkItem;
    for ( ; anOutPortsIter != anOutPorts.end(); anOutPortsIter++ ) {
      OutPort* aCurOutPort = *anOutPortsIter;
      string aClassName = aCurOutPort->getNameOfTypeOfCurrentInstance();
      
      set<InPort*> anInPorts = aCurOutPort->edSetInPort();
      set<InPort*>::iterator anInPortsIter = anInPorts.begin();
      
      // Data links
      if ( !aClassName.compare(OutputPort::NAME) ) {
	if ( OutputPort* anOutputDFPort = dynamic_cast<OutputPort*>( aCurOutPort ) )
	  for ( ; anInPortsIter != anInPorts.end(); anInPortsIter++ ) {
	    InPort* aCurInPort = *anInPortsIter;
	    
	    pair< OutPort*,InPort* > anOIP = make_pair(anOutputDFPort, aCurInPort);
	    if ( anInternalLinks.find(anOIP) == anInternalLinks.end() )
	      aDataLinkItem = new YACSGui_LinkViewItem( aLinksItem, aDataLinkItem,
							GuiContext::getCurrent()->_mapOfSubjectLink[anOIP] );
	  }
      }
    } // out ports of the node iteration

    // Process out ports list of the node : Stream Links
    anOutPortsIter = anOutPorts.begin();
    QListViewItem* aStreamLinkItem = aDataLinkItem;
    for ( ; anOutPortsIter != anOutPorts.end(); anOutPortsIter++ ) {
      OutPort* aCurOutPort = *anOutPortsIter;
      string aClassName = aCurOutPort->getNameOfTypeOfCurrentInstance();
      
      set<InPort*> anInPorts = aCurOutPort->edSetInPort();
      set<InPort*>::iterator anInPortsIter = anInPorts.begin();
      
      // Stream links
      if ( !aClassName.compare(OutputDataStreamPort::NAME) || !aClassName.compare(OutputCalStreamPort::NAME) ) {
	if ( OutputDataStreamPort* anOutputDSPort = dynamic_cast<OutputDataStreamPort*>( aCurOutPort ) )
	  for ( ; anInPortsIter != anInPorts.end(); anInPortsIter++ ) {
	    InPort* aCurInPort = *anInPortsIter;
	    pair< OutPort*,InPort* > anOIP = make_pair(anOutputDSPort, aCurInPort);
	    if ( anInternalLinks.find(anOIP) == anInternalLinks.end() ) 
	      aStreamLinkItem = new YACSGui_LinkViewItem( aLinksItem, aStreamLinkItem,
							  GuiContext::getCurrent()->_mapOfSubjectLink[anOIP] );
	  }
      }
    } // out ports of the node iteration
    
  } // nodes of the proc iteration
  
  // Create "Containers" label if necessary
  YACSGui_LabelViewItem* aContainersItem = 0;
  //if ( !mySalomeComponentData.empty() || !myCorbaComponentData.empty() )
  aContainersItem = new YACSGui_LabelViewItem( aSchemaItem, aLinksItem, "Containers" );

  if ( aContainersItem )
  {
    // Put all SALOME containers under "Containers" label
    YACSGui_ContainerViewItem* aContainerItem = 0;
    for (map<string, Container*>::const_iterator itCont = getProc()->containerMap.begin();
	 itCont != getProc()->containerMap.end(); ++itCont)
    {
      aContainerItem = new YACSGui_ContainerViewItem( aContainersItem, aContainerItem,
						      GuiContext::getCurrent()->_mapOfSubjectContainer[(*itCont).second] );
      
      // get a list of service nodes (and therefore, components) inside this container
      Component2ServiceNodesMap aComponent2ServiceNodes;
      if ( mySalomeComponentData.find(dynamic_cast<SalomeContainer*>( (*itCont).second ))
	   != mySalomeComponentData.end() )
	aComponent2ServiceNodes = mySalomeComponentData[dynamic_cast<SalomeContainer*>( (*itCont).second )];

      YACSGui_ComponentViewItem* aComponentItem = 0;
      for (map<pair<string,int>, ComponentInstance*>::const_iterator itComp = getProc()->componentInstanceMap.begin();
	   itComp != getProc()->componentInstanceMap.end(); ++itComp)
      {
	bool aNeedToPublish = true;
	list<SubjectServiceNode*> aServiceNodes;
	if ( aComponent2ServiceNodes.find((*itComp).second) != aComponent2ServiceNodes.end() )
	  aServiceNodes = aComponent2ServiceNodes[(*itComp).second];
	else
	  // this component is not associated with any service node
	  // check this component have to be loaded in the current container (itCont) or not
	  if ( (*itComp).second->getContainer() != (*itCont).second )
	    aNeedToPublish = false;
	
	if ( !aNeedToPublish ) continue;

	SubjectComponent* aSComp = GuiContext::getCurrent()->_mapOfSubjectComponent[(*itComp).second];
	aComponentItem = new YACSGui_ComponentViewItem( aContainerItem, aComponentItem, aSComp );

	YACSGui_ReferenceViewItem* aRefNodeItem = 0;
       	for ( list<SubjectServiceNode*>::iterator itSNode = aServiceNodes.begin();
	      itSNode != aServiceNodes.end(); itSNode++ )
	  // Put a reference to the schemas node, which is used this component instance
	  aRefNodeItem = new YACSGui_ReferenceViewItem( aComponentItem, aRefNodeItem, (*itSNode)->getSubjectReference() );
      }
    }
    
    // Put all CORBA components under "Containers" label
    YACSGui_ComponentViewItem* aCComponentItem = 0;
    for ( Component2ServiceNodesMap::iterator itComp = myCorbaComponentData.begin();
	  itComp != myCorbaComponentData.end(); itComp++ )
    {
      SubjectComponent* aSComp = GuiContext::getCurrent()->_mapOfSubjectComponent[(*itComp).first];
      aCComponentItem = new YACSGui_ComponentViewItem( aContainersItem, 
						       aCComponentItem ? (QListViewItem*)aCComponentItem :
						                         (QListViewItem*)aContainerItem,
						       aSComp );

      YACSGui_ReferenceViewItem* aRefNodeItem = 0;
      list<SubjectServiceNode*> aServiceNodes = (*itComp).second;
      for ( list<SubjectServiceNode*>::iterator itSNode = aServiceNodes.begin();
	    itSNode != aServiceNodes.end(); itSNode++ )
      	// Put a reference to the schemas node, which is used this component instance
	aRefNodeItem = new YACSGui_ReferenceViewItem( aCComponentItem, aRefNodeItem, (*itSNode)->getSubjectReference() );
    }
  }
}

void YACSGui_EditionTreeView::fillContainerData( YACS::HMI::SubjectComposedNode* theSNode )
{
  if ( theSNode )
    if ( ComposedNode* aNode = dynamic_cast<ComposedNode*>(theSNode->getNode()) ) //if ( getProc() )
    { // find containers and components, which are used by Proc* service nodes
      list<Node*> aNodeSet = aNode->edGetDirectDescendants(); //getProc()->getAllRecursiveConstituents();
      for ( list<Node*>::iterator it = aNodeSet.begin(); it != aNodeSet.end(); it++ )
      {
	if ( ServiceNode* aSNode = dynamic_cast<ServiceNode*>( *it ) )
	{
	  if ( ComponentInstance* aComponent = aSNode->getComponent() )
	  {
	    if ( SubjectServiceNode* aSSNode = dynamic_cast<SubjectServiceNode*>( theSNode->getChild(aSNode) ) )
	    {
	      if ( aComponent->getKind() == CORBAComponent::KIND )
	      { // CORBA component
		if ( myCorbaComponentData.find( aComponent ) == myCorbaComponentData.end() )
		  myCorbaComponentData.insert( make_pair( aComponent, list<SubjectServiceNode*>( 1, aSSNode ) ) );
		else if ( std::find(myCorbaComponentData[ aComponent ].begin(), 
				    myCorbaComponentData[ aComponent ].end(), 
				    aSSNode) == myCorbaComponentData[ aComponent ].end() )
		{
		  myCorbaComponentData[ aComponent ].push_back( aSSNode );
		  DEBTRACE(">> CORBA ComponentInstance = "<<aComponent);
		}
	      }
	      else
	      { // SALOME or SALOME Python component
		if ( SalomeContainer* aContainer = dynamic_cast<SalomeContainer*>( aComponent->getContainer() ) )
		  if ( mySalomeComponentData.find( aContainer ) == mySalomeComponentData.end() )
		  {
		    Component2ServiceNodesMap aComp2Service;
		    aComp2Service.insert( make_pair( aComponent, list<SubjectServiceNode*>( 1, aSSNode ) ) );
		    mySalomeComponentData.insert( make_pair( aContainer, aComp2Service ) );
		  }
		  else if ( std::find( mySalomeComponentData[ aContainer ][ aComponent ].begin(),
				       mySalomeComponentData[ aContainer ][ aComponent ].end(),
				       aSSNode ) == mySalomeComponentData[ aContainer ][ aComponent ].end() )
		    mySalomeComponentData[ aContainer ][ aComponent ].push_back( aSSNode );
	      }
	    }
	  }
	}
	else if ( ComposedNode* aCNode = dynamic_cast<ComposedNode*>( *it ) )
	{
	  fillContainerData( dynamic_cast<SubjectComposedNode*>( theSNode->getChild(aCNode) ) );
	}
      }
    }
}

QPopupMenu* YACSGui_EditionTreeView::contextMenuPopup( const int theType, YACS::HMI::Subject* theSub )
{
  QPopupMenu* aPopup = new QPopupMenu( this );

  int anId;
  QPopupMenu* aCrNPopup = new QPopupMenu( this );
  aCrNPopup->insertItem( tr("POP_SALOME_SERVICE"),        myModule, SLOT(onSalomeServiceNode()) );
//   aCrNPopup->insertItem( tr("POP_CORBA_SERVICE"),         myModule, SLOT(onCorbaServiceNode()) );
  //aCrNPopup->insertItem( tr("POP_NODENODE_SERVICE"),      myModule, SLOT(onNodeNodeServiceNode()) );
  aCrNPopup->insertItem( tr("POP_CPP"),                   myModule, SLOT(onCppNode()) );
  anId = aCrNPopup->insertItem( tr("POP_SERVICE_INLINE"), myModule, SLOT(onServiceInlineNode()) );
  aCrNPopup->setItemEnabled ( anId, false );
  anId = aCrNPopup->insertItem( tr("POP_XML"),            myModule, SLOT(onXMLNode()) );
  aCrNPopup->setItemEnabled ( anId, false );
  aCrNPopup->insertItem( tr("POP_INLINE_SCRIPT"),         myModule, SLOT(onInlineScriptNode()) );
  aCrNPopup->insertItem( tr("POP_INLINE_FUNCTION"),       myModule, SLOT(onInlineFunctionNode()) );
  aCrNPopup->insertItem( tr("POP_IN_DATANODE"),           myModule, SLOT(onInDataNode()) );
  aCrNPopup->insertItem( tr("POP_OUT_DATANODE"),          myModule, SLOT(onOutDataNode()) );
  aCrNPopup->insertItem( tr("POP_IN_STUDYNODE"),          myModule, SLOT(onInStudyNode()) );
  aCrNPopup->insertItem( tr("POP_OUT_STUDYNODE"),         myModule, SLOT(onOutStudyNode()) );
  aCrNPopup->insertItem( tr("POP_BLOCK"),                 myModule, SLOT(onBlockNode()) );
  aCrNPopup->insertItem( tr("POP_FOR_LOOP"),              myModule, SLOT(onFORNode()) );
  aCrNPopup->insertItem( tr("POP_FOREACH_LOOP"),          myModule, SLOT(onFOREACHNode()) );
  aCrNPopup->insertItem( tr("POP_WHILE_LOOP"),            myModule, SLOT(onWHILENode()) );
  aCrNPopup->insertItem( tr("POP_SWITCH"),                myModule, SLOT(onSWITCHNode()) );
  //aCrNPopup->insertItem( tr("POP_FROM_LIBRARY"),          myModule, SLOT(onNodeFromLibrary()) );

  switch ( theType )
  {
  case YACSGui_EditionTreeView::DataTypeItem:
    {
      anId = aPopup->insertItem( tr("POP_EDIT"), myModule, SLOT(onEditDataTypes()) );
      aPopup->setItemEnabled ( anId, false );
      aPopup->insertItem( tr("POP_IMPORT_DATA_TYPE"), myModule, SLOT(onImportDataType()) );
    }
    break;
  case YACSGui_EditionTreeView::LabelNodesItem:
    {
      aPopup->insertItem(tr("POP_CREATE_NODE"), aCrNPopup);
    }
    break;
  case YACSGui_EditionTreeView::LabelContainersItem:
    {
      aPopup->insertItem( tr("POP_CREATE_CONTAINER_DEF"), myModule, SLOT(onNewContainer()) );
    }
    break;
  case YACSGui_TreeView::SchemaItem:
    {
      aPopup->insertItem( tr("POP_EXPORT"), myModule, SLOT(onExportSchema()) );
      aPopup->insertSeparator();

      anId = aPopup->insertItem( tr("POP_CREATE_DATA_TYPE"), myModule, SLOT(onCreateDataType()) );
      aPopup->setItemEnabled ( anId, false );
      aPopup->insertItem( tr("POP_CREATE_CONTAINER_DEF"), myModule, SLOT(onNewContainer()) );
      aPopup->insertSeparator();
      
      QPopupMenu* aCrNPopup = new QPopupMenu( this );
      aCrNPopup->insertItem( tr("POP_SALOME_SERVICE"), myModule, SLOT(onSalomeServiceNode()) );
      //aCrNPopup->insertItem( tr("POP_CORBA_SERVICE"), myModule, SLOT(onCorbaServiceNode()) );
      //aCrNPopup->insertItem( tr("POP_NODENODE_SERVICE"), myModule, SLOT(onNodeNodeServiceNode()) );
      aCrNPopup->insertItem( tr("POP_CPP"), myModule, SLOT(onCppNode()) );
      anId = aCrNPopup->insertItem( tr("POP_SERVICE_INLINE"), myModule, SLOT(onServiceInlineNode()) );
      aCrNPopup->setItemEnabled ( anId, false );
      anId = aCrNPopup->insertItem( tr("POP_XML"), myModule, SLOT(onXMLNode()) );
      aCrNPopup->setItemEnabled ( anId, false );
      aCrNPopup->insertItem( tr("POP_INLINE_SCRIPT"), myModule, SLOT(onInlineScriptNode()) );
      aCrNPopup->insertItem( tr("POP_INLINE_FUNCTION"), myModule, SLOT(onInlineFunctionNode()) );
      aCrNPopup->insertItem( tr("POP_IN_DATANODE"), myModule, SLOT(onInDataNode()) );
      aCrNPopup->insertItem( tr("POP_OUT_DATANODE"), myModule, SLOT(onOutDataNode()) );
      aCrNPopup->insertItem( tr("POP_IN_STUDYNODE"), myModule, SLOT(onInStudyNode()) );
      aCrNPopup->insertItem( tr("POP_OUT_STUDYNODE"), myModule, SLOT(onOutStudyNode()) );
      aCrNPopup->insertItem( tr("POP_BLOCK"), myModule, SLOT(onBlockNode()) );
      aCrNPopup->insertItem( tr("POP_FOR_LOOP"), myModule, SLOT(onFORNode()) );
      aCrNPopup->insertItem( tr("POP_FOREACH_LOOP"), myModule, SLOT(onFOREACHNode()) );
      aCrNPopup->insertItem( tr("POP_WHILE_LOOP"), myModule, SLOT(onWHILENode()) );
      aCrNPopup->insertItem( tr("POP_SWITCH"), myModule, SLOT(onSWITCHNode()) );
      anId = aCrNPopup->insertItem( tr("POP_FROM_LIBRARY"), myModule, SLOT(onNodeFromLibrary()) );
      aCrNPopup->setItemEnabled ( anId, false );
      
      aPopup->insertItem(tr("POP_CREATE_NODE"), aCrNPopup);
      aPopup->insertSeparator();
      
      aPopup->insertItem( tr("POP_EXECUTE"), myModule, SLOT(onCreateExecution(/*bool theIsFromTreeView*/)) );
      // TODO: if theIsFromTreeView = true, search SObject in Object Browser by name of the selected schema item in the tree view
      aPopup->insertSeparator();
      
      anId = aPopup->insertItem( tr("POP_COPY"), this, SLOT(onCopyItem()) );
      aPopup->setItemEnabled ( anId, false );
      anId = aPopup->insertItem( tr("POP_PASTE"), this, SLOT(onPasteItem()) );
      aPopup->setItemEnabled ( anId, false );
      //aPopup->insertItem( tr("POP_DELETE"), this, SLOT(onDeleteItem()) );
      aPopup->insertSeparator();
      
      anId = aPopup->insertItem( tr("POP_DISPLAY"), this, SLOT(onDisplaySchema()) );
      aPopup->setItemEnabled ( anId, false );
      anId = aPopup->insertItem( tr("POP_ERASE"), this, SLOT(onEraseSchema()) );
      aPopup->setItemEnabled ( anId, false );
    }
    break;
  case YACSGui_EditionTreeView::ContainerItem:
    {    
      QPopupMenu* aCrCPopup = new QPopupMenu( this );
      aCrCPopup->insertItem( tr("POP_SALOME_COMPONENT"), myModule, SLOT(onNewSalomeComponent()) );
      anId = aCrCPopup->insertItem( tr("POP_PYTHON_COMPONENT"), myModule, SLOT(onNewSalomePythonComponent()) );
      aCrCPopup->setItemEnabled ( anId, false );
      
      aPopup->insertItem(tr("POP_CREATE_COMPONENT"), aCrCPopup);
      aPopup->insertSeparator();
      
      anId = aPopup->insertItem( tr("POP_COPY"), this, SLOT(onCopyItem()) );
      aPopup->setItemEnabled ( anId, false );
      anId = aPopup->insertItem( tr("POP_PASTE"), this, SLOT(onPasteItem()) ); 
      aPopup->setItemEnabled ( anId, false );
      aPopup->insertItem( tr("POP_DELETE"), this, SLOT(onDeleteItem()) );
    }
    break;
  case YACSGui_EditionTreeView::ComponentItem:
    {
      QPopupMenu* aCrN2Popup = new QPopupMenu( this );
      aCrN2Popup->insertItem( tr("POP_SALOME_SERVICE"), myModule, SLOT(onSalomeServiceNode()) );
      anId = aCrN2Popup->insertItem( tr("POP_SERVICE_INLINE"), myModule, SLOT(onServiceInlineNode()) );
      aCrN2Popup->setItemEnabled ( anId, false );

      aPopup->insertItem(tr("POP_CREATE_NODE"), aCrN2Popup);
      aPopup->insertSeparator();
      
      anId = aPopup->insertItem( tr("POP_COPY"), this, SLOT(onCopyItem()) );
      aPopup->setItemEnabled ( anId, false );
      aPopup->insertItem( tr("POP_DELETE"), this, SLOT(onDeleteItem()) );
    }
    break;
  case YACSGui_EditionTreeView::CorbaComponentItem:
    {
      //aPopup->insertItem( tr("POP_CREATE_CORBA_SERVICE_NODE"), myModule, SLOT(onCorbaServiceNode()) );
      //aPopup->insertSeparator();
      
      anId = aPopup->insertItem( tr("POP_COPY"), this, SLOT(onCopyItem()) );
      aPopup->setItemEnabled ( anId, false );
      aPopup->insertItem( tr("POP_DELETE"), this, SLOT(onDeleteItem()) );
    }
    break;
  case YACSGui_TreeView::NodeItem:
  case YACSGui_EditionTreeView::ServiceNodeItem:
  case YACSGui_EditionTreeView::ComposedNodeItem:
  case YACSGui_EditionTreeView::LoopNodeItem:
    {
      //if ( theType == YACSGui_EditionTreeView::ServiceNodeItem )
      //{
      //	aPopup->insertItem( tr("POP_CREATE_NODE-NODE_SERVICE_NODE"), myModule, SLOT(onNodeNodeServiceNode()) );
      //	aPopup->insertSeparator();
      //}
      
      if ( theType == YACSGui_EditionTreeView::LoopNodeItem ||
	   theType == YACSGui_EditionTreeView::ComposedNodeItem )
      {
	// TODO: in slots we have to check what is selected: schema,
	//                                                   loop node (=> from ¨Create a body¨) or
	//                                                   another node (=> from ¨Create a loop¨).
	if ( theType == YACSGui_EditionTreeView::LoopNodeItem )
	  aPopup->insertItem(tr("POP_CREATE_BODY"), aCrNPopup);
	else if ( theType == YACSGui_EditionTreeView::ComposedNodeItem )
	  aPopup->insertItem(tr("POP_CREATE_NODE"), aCrNPopup);
	aPopup->insertSeparator();
      }

      QPopupMenu* aCrN2Popup = new QPopupMenu( this );
      aCrN2Popup->insertItem( tr("POP_FOR_LOOP"), myModule, SLOT(onFORNode()) );
      aCrN2Popup->insertItem( tr("POP_FOREACH_LOOP"), myModule, SLOT(onFOREACHNode()) );
      aCrN2Popup->insertItem( tr("POP_WHILE_LOOP"), myModule, SLOT(onWHILENode()) );
      
      aPopup->insertItem(tr("POP_CREATE_LOOP"), aCrN2Popup);
      aPopup->insertSeparator();

      anId = aPopup->insertItem( tr("POP_ADD_TO_LIBRARY"), this, SLOT(onAddToLibrary()) );
      aPopup->setItemEnabled ( anId, false );
      aPopup->insertSeparator();
      
      aPopup->insertItem( tr("POP_ADDCONTROLLINK"), this, SLOT(onAddLink()) );
      aPopup->insertSeparator();

      anId = aPopup->insertItem( tr("POP_COPY"), this, SLOT(onCopyItem()) );
      aPopup->setItemEnabled ( anId, false );
      aPopup->insertItem( tr("POP_DELETE"), this, SLOT(onDeleteItem()) );
      aPopup->insertSeparator();
      
      anId = aPopup->insertItem( tr("POP_DISPLAY"), this, SLOT(onDisplayNode()) );
      aPopup->setItemEnabled ( anId, false );
      anId = aPopup->insertItem( tr("POP_ERASE"), this, SLOT(onEraseNode()) );
      aPopup->setItemEnabled ( anId, false );
    }
    break;
  case YACSGui_EditionTreeView::PortItem:
    if ( SubjectDataPort* aSub = dynamic_cast<SubjectDataPort*>(theSub) )
    {
      if ( dynamic_cast<OutputPort*>( aSub->getPort() ) || dynamic_cast<OutputDataStreamPort*>( aSub->getPort() ) )
        {
      	aPopup->insertItem( tr("POP_ADDDATAFLOWLINK"), this, SLOT(onAddLink()) );
      	aPopup->insertItem( tr("POP_ADDDATALINK"), this, SLOT(onAddDataLink()) );
        }
	
    
      if ( !dynamic_cast<SubjectSalomeNode*>(aSub->getParent()) ) 
	// delete ports only for all nodes except SALOME service nodes
	// (change its ports only in the Input Panel when the method name is changed)
	if ( aSub->isDestructible() )
	{
	  aPopup->insertSeparator();
	  aPopup->insertItem( tr("POP_DELETE"), this, SLOT(onDeleteItem()) );
	}
    }
    break;
  case YACSGui_EditionTreeView::LinkItem:
    aPopup->insertItem( tr("POP_DELETE"), this, SLOT(onDeleteItem()) );
    break;
  default:
    break;
  }

  if ( aPopup->count() == 0 ) return 0;

  return aPopup;
}

void YACSGui_EditionTreeView::showPopup( QPopupMenu* thePopup, const QPoint thePos )
{
  if ( thePopup ) thePopup->exec(thePos);
}

void YACSGui_EditionTreeView::warnAboutSelectionChanged()
{
  DEBTRACE("YACSGui_EditionTreeView::warnAboutSelectionChanged");
  if ( !myModule ) return;

  YACSGui_InputPanel* anIP = myModule->getInputPanel();
  if ( !anIP ) return;

  bool aWarnToShow = true;
  bool aToApply=false;
  list<int> anIds = anIP->getVisiblePagesIds();
  for ( list<int>::iterator anIter = anIds.begin(); anIter != anIds.end(); ++anIter )
  {
    int anId = (*anIter);
    switch (anId)
    {
    case YACSGui_InputPanel::ContainerId:
      {
	if ( YACSGui_ContainerPage* aContPage = 
	     dynamic_cast<YACSGui_ContainerPage*>( anIP->getPage( YACSGui_InputPanel::ContainerId ) ) )
	  aContPage->checkModifications( aWarnToShow, aToApply );
      }
      break;
    case YACSGui_InputPanel::ComponentId:
      {
	if ( YACSGui_ComponentPage* aCompPage =
	     dynamic_cast<YACSGui_ComponentPage*>( anIP->getPage( YACSGui_InputPanel::ComponentId ) ) )
	  aCompPage->checkModifications( aWarnToShow, aToApply );
      }
      break;
    case YACSGui_InputPanel::SchemaId:
      {
	if ( YACSGui_SchemaPage* aSPage =
	     dynamic_cast<YACSGui_SchemaPage*>( anIP->getPage( YACSGui_InputPanel::SchemaId ) ) )
	  aSPage->checkModifications();
      }
      break;
    case YACSGui_InputPanel::InDataNodeId:
      {
	if ( YACSGui_DataNodePage* aINPage = 
	     dynamic_cast<YACSGui_DataNodePage*>( anIP->getPage( YACSGui_InputPanel::InDataNodeId ) ) )
	  aINPage->checkModifications();
      }
      break;
    case YACSGui_InputPanel::OutDataNodeId:
      {
	if ( YACSGui_DataNodePage* aINPage = 
	     dynamic_cast<YACSGui_DataNodePage*>( anIP->getPage( YACSGui_InputPanel::OutDataNodeId ) ) )
	  aINPage->checkModifications();
      }
      break;
    case YACSGui_InputPanel::InStudyNodeId:
      {
	if ( YACSGui_DataNodePage* aINPage = 
	     dynamic_cast<YACSGui_DataNodePage*>( anIP->getPage( YACSGui_InputPanel::InStudyNodeId ) ) )
	  aINPage->checkModifications();
      }
      break;
    case YACSGui_InputPanel::OutStudyNodeId:
      {
	if ( YACSGui_DataNodePage* aINPage = 
	     dynamic_cast<YACSGui_DataNodePage*>( anIP->getPage( YACSGui_InputPanel::OutStudyNodeId ) ) )
	  aINPage->checkModifications();
      }
      break;
    case YACSGui_InputPanel::InlineNodeId:
      {
	if ( YACSGui_InlineNodePage* aINPage = 
	     dynamic_cast<YACSGui_InlineNodePage*>( anIP->getPage( YACSGui_InputPanel::InlineNodeId ) ) )
	  aINPage->checkModifications();
      }
      break;
    case YACSGui_InputPanel::ServiceNodeId:
      {
	if ( YACSGui_ServiceNodePage* aSNPage = 
	     dynamic_cast<YACSGui_ServiceNodePage*>( anIP->getPage( YACSGui_InputPanel::ServiceNodeId ) ) )
	  aSNPage->checkModifications( aWarnToShow, aToApply );
      }
      break;
    case YACSGui_InputPanel::ForLoopNodeId:
      {
	if ( YACSGui_ForLoopNodePage* aFLNPage = 
	     dynamic_cast<YACSGui_ForLoopNodePage*>( anIP->getPage( YACSGui_InputPanel::ForLoopNodeId ) ) )
	  aFLNPage->checkModifications();
      }
      break;
    case YACSGui_InputPanel::ForEachLoopNodeId:
      {
	if ( YACSGui_ForEachLoopNodePage* aFELNPage = 
	     dynamic_cast<YACSGui_ForEachLoopNodePage*>( anIP->getPage( YACSGui_InputPanel::ForEachLoopNodeId ) ) )
	  aFELNPage->checkModifications();
      }
      break;
    case YACSGui_InputPanel::WhileLoopNodeId:
      {
	if ( YACSGui_WhileLoopNodePage* aWLNPage = 
	     dynamic_cast<YACSGui_WhileLoopNodePage*>( anIP->getPage( YACSGui_InputPanel::WhileLoopNodeId ) ) )
	  aWLNPage->checkModifications();
      }
      break;
    case YACSGui_InputPanel::SwitchNodeId:
      {
	if ( YACSGui_SwitchNodePage* aSNPage = 
	     dynamic_cast<YACSGui_SwitchNodePage*>( anIP->getPage( YACSGui_InputPanel::SwitchNodeId ) ) )
	  aSNPage->checkModifications();
      }
      break;
    case YACSGui_InputPanel::BlockNodeId:
      {
	if ( YACSGui_BlockNodePage* aBNPage = 
	     dynamic_cast<YACSGui_BlockNodePage*>( anIP->getPage( YACSGui_InputPanel::BlockNodeId ) ) )
	  aBNPage->checkModifications();
      }
      break;
    case YACSGui_InputPanel::LinkId:
      {
      }
      break;
    default:
      break;
    }
  }
}

void YACSGui_EditionTreeView::syncPageTypeWithSelection()
{
  DEBTRACE("YACSGui_EditionTreeView::syncPageTypeWithSelection");
  if ( !getProc() || !myModule ) return;

  YACSGui_InputPanel* anIP = myModule->getInputPanel();
  if ( !anIP ) return;

  // get the list of selected items
  list<QListViewItem*> aSelList = getSelected();
  
  QListViewItem* anItem = 0;
  
  // check if the current selection is a single selection
  if ( aSelList.size() == 1 ) anItem = aSelList.front();
  
  //if ( aSelList.size() > 0 ) warnAboutSelectionChanged();
    
  if(myModule->getGuiMode()==YACSGui_Module::NewMode) {
    DEBTRACE(">> Show Input Panel for a new Salome Service node");
    YACSGui_ServiceNodePage* aSNPage = 
      dynamic_cast<YACSGui_ServiceNodePage*>( anIP->getPage( YACSGui_InputPanel::ServiceNodeId ) );
    SubjectComposedNode* aSub = dynamic_cast<SubjectComposedNode*>(getSelectedSubject());
    DEBTRACE("aSub: " << aSub);
    if(!aSub)
      aSub = GuiContext::getCurrent()->getSubjectProc();
    DEBTRACE("aSub: " << aSub);
    if ( aSNPage )
      {
	list<int> aPagesIds;

	aSNPage->setSCNode( aSub );
	aSNPage->setSNode( 0 );
	anIP->setOn( true, YACSGui_InputPanel::ServiceNodeId );
	anIP->setMode( YACSGui_InputPanel::NewMode, YACSGui_InputPanel::ServiceNodeId );
	aPagesIds.push_back(YACSGui_InputPanel::ServiceNodeId);
	
	anIP->setExclusiveVisible( true, aPagesIds );
	anIP->show();
      }
  } 
  else if ( YACSGui_SchemaViewItem* aSchemaItem = dynamic_cast<YACSGui_SchemaViewItem*>( anItem ) )
  { // this is a schema view item
    // find or create a property page for a schema
    DEBTRACE(">> Show Input Panel for a given schema (Proc*)");

    YACSGui_SchemaPage* aSPage = dynamic_cast<YACSGui_SchemaPage*>( anIP->getPage( YACSGui_InputPanel::SchemaId ) );
    if ( aSPage )
    {
      aSPage->setSProc( aSchemaItem->getSProc() );
      anIP->setOn( true, YACSGui_InputPanel::SchemaId );
      anIP->setMode( YACSGui_InputPanel::EditMode, YACSGui_InputPanel::SchemaId );
      anIP->setExclusiveVisible( true,  list<int>(1,YACSGui_InputPanel::SchemaId) );
      string theErrorLog;
      Proc* aProc = getProc();
      // Check validity
      if(!aProc->isValid())
        {
          theErrorLog="--- YACS schema is not valid ---\n\n";
          theErrorLog += aProc->getErrorReport();
        }
      else
        {
          // Check consistency
          LinkInfo info(LinkInfo::ALL_DONT_STOP);
          aProc->checkConsistency(info);
          if (info.areWarningsOrErrors())
            theErrorLog = info.getGlobalRepr();
          else
            {
              theErrorLog = "--- No Validity Errors ---\n";
              theErrorLog += "--- No Consistency Errors ---\n";
            }
        }

      //Add initial logger info
      Logger* logger=getProc()->getLogger("parser");
      if (!logger->isEmpty())
        {
          theErrorLog += " --- Original file import log ---\n";
          theErrorLog += logger->getStr();  
        }

      aSPage->myErrorLog->setText(theErrorLog.c_str());
      anIP->show();
    }
  }
  else if ( YACSGui_NodeViewItem* aNodeItem = dynamic_cast<YACSGui_NodeViewItem*>( anItem ) )
  { // this is a node data object
    // the following method can be used if its needed:
    // YACSPrs_ElementaryNode* aNode = getGraph( dynamic_cast<Proc*>(aNodeItem->getNode()->getRootNode()) )->getItem( aNodeItem->getNode() );
    DEBTRACE(">> Show Input Panel for a given node (Node*)");

    Node* aNode = aNodeItem->getNode();
    DEBTRACE("Show Input Panel for a given node: " << aNode->getName());
    
    if ( ServiceNode* sNode = dynamic_cast<ServiceNode*>( aNode ) )
    {
      if ( sNode->getKind() == "XML" )
      {
	DEBTRACE(">>XML");
	/*
	YACSGui_ServiceInlineNodePage* aSINPage = 
	  dynamic_cast<YACSGui_ServiceInlineNodePage*>( anIP->getPage( YACSGui_InputPanel::ServiceInlineNodeId ) );
	if ( aSINPage )
	{
	  aSINPage->setSNode( aNodeItem->getSNode() );
	  
	  anIP->setOn( true, YACSGui_InputPanel::ServiceInlineNodeId );
	  anIP->setMode( YACSGui_InputPanel::EditMode, YACSGui_InputPanel::ServiceInlineNodeId );
	  anIP->setExclusiveVisible( true,  list<int>(1,YACSGui_InputPanel::ServiceInlineNodeId) );
	  anIP->show();
	}
	*/
      }
      else
      {
	DEBTRACE(">>Service");
	YACSGui_ServiceNodePage* aSNPage = 
	  dynamic_cast<YACSGui_ServiceNodePage*>( anIP->getPage( YACSGui_InputPanel::ServiceNodeId ) );
	if ( aSNPage )
	{
          DEBTRACE("---service node");
	  list<int> aPagesIds;

	  if ( !dynamic_cast<CORBANode*>(sNode) )
	  {
	    YACSGui_ContainerPage* aContPage = 
	      dynamic_cast<YACSGui_ContainerPage*>( anIP->getPage( YACSGui_InputPanel::ContainerId ) );
	    if ( aContPage )
	    {
	      SubjectContainer* aCont = 0;
	      if ( sNode->getComponent() && sNode->getComponent()->getContainer() )
		aCont = GuiContext::getCurrent()->_mapOfSubjectContainer[sNode->getComponent()->getContainer()];
	      aContPage->setSContainer(aCont);
	      anIP->setOn( false, YACSGui_InputPanel::ContainerId );
	      aPagesIds.push_back(YACSGui_InputPanel::ContainerId);
	    }
	  }
	    
	  YACSGui_ComponentPage* aCompPage = 
	    dynamic_cast<YACSGui_ComponentPage*>( anIP->getPage( YACSGui_InputPanel::ComponentId ) );
	  if ( aCompPage )
	  {
	    SubjectComponent* aComp = 0;
	    if ( sNode->getComponent() )
	      aComp = GuiContext::getCurrent()->_mapOfSubjectComponent[sNode->getComponent()];
	    aCompPage->setSComponent(aComp, false);
	    anIP->setOn( false, YACSGui_InputPanel::ComponentId );
	    aPagesIds.push_back(YACSGui_InputPanel::ComponentId);
	  }
	  
	  aSNPage->setSNode( aNodeItem->getSNode() );
	  anIP->setOn( true, YACSGui_InputPanel::ServiceNodeId );
	  anIP->setMode( YACSGui_InputPanel::EditMode, YACSGui_InputPanel::ServiceNodeId );
	  aPagesIds.push_back(YACSGui_InputPanel::ServiceNodeId);

	  anIP->setExclusiveVisible( true, aPagesIds );
	  anIP->show();
	}
      }
    }
    else if ( dynamic_cast<PresetNode*>( aNode ) )
    {
      DEBTRACE(">>PresetNode");
      YACSGui_DataNodePage* aINPage = 
	dynamic_cast<YACSGui_DataNodePage*>( anIP->getPage( YACSGui_InputPanel::InDataNodeId ) );
      DEBTRACE(aINPage);
      if ( aINPage )
      {
        YACS::HMI::SubjectNode* aSub = aNodeItem->getSNode();
	aINPage->setSNode( aSub );
	anIP->setOn( true, YACSGui_InputPanel::InDataNodeId );
	anIP->setMode( YACSGui_InputPanel::EditMode, YACSGui_InputPanel::InDataNodeId );
	anIP->setExclusiveVisible( true,  list<int>(1,YACSGui_InputPanel::InDataNodeId) );
	anIP->show();
      }
    }
    else if ( dynamic_cast<OutNode*>( aNode ) )
    {
      DEBTRACE(">>OutNode");
      YACSGui_DataNodePage* aINPage = 
	dynamic_cast<YACSGui_DataNodePage*>( anIP->getPage( YACSGui_InputPanel::OutDataNodeId ) );
      DEBTRACE(aINPage);
      if ( aINPage )
      {
        YACS::HMI::SubjectNode* aSub = aNodeItem->getSNode();
	aINPage->setSNode( aSub );
	anIP->setOn( true, YACSGui_InputPanel::OutDataNodeId );
	anIP->setMode( YACSGui_InputPanel::EditMode, YACSGui_InputPanel::OutDataNodeId );
	anIP->setExclusiveVisible( true,  list<int>(1,YACSGui_InputPanel::OutDataNodeId) );
	anIP->show();
      }
    }
    else if ( dynamic_cast<StudyInNode*>( aNode ) )
    {
      DEBTRACE("show StudyIn page");
      YACSGui_DataNodePage* aINPage = 
	dynamic_cast<YACSGui_DataNodePage*>( anIP->getPage( YACSGui_InputPanel::InStudyNodeId ) );
      DEBTRACE(aINPage);
      if ( aINPage )
      {
        YACS::HMI::SubjectNode* aSub = aNodeItem->getSNode();
	aINPage->setSNode( aSub );
	anIP->setOn( true, YACSGui_InputPanel::InStudyNodeId );
	anIP->setMode( YACSGui_InputPanel::EditMode, YACSGui_InputPanel::InStudyNodeId );
	anIP->setExclusiveVisible( true,  list<int>(1,YACSGui_InputPanel::InStudyNodeId) );
	anIP->show();
      }
    }
    else if ( dynamic_cast<StudyOutNode*>( aNode ) )
    {
      DEBTRACE(">>StudyOut");
      DEBTRACE("show StudyOut page");
      YACSGui_DataNodePage* aINPage = 
	dynamic_cast<YACSGui_DataNodePage*>( anIP->getPage( YACSGui_InputPanel::OutStudyNodeId ) );
      DEBTRACE(aINPage);
      if ( aINPage )
      {
        YACS::HMI::SubjectNode* aSub = aNodeItem->getSNode();
	aINPage->setSNode( aSub );
	anIP->setOn( true, YACSGui_InputPanel::OutStudyNodeId );
	anIP->setMode( YACSGui_InputPanel::EditMode, YACSGui_InputPanel::OutStudyNodeId );
	anIP->setExclusiveVisible( true,  list<int>(1,YACSGui_InputPanel::OutStudyNodeId) );
	anIP->show();
      }
    }
    else if ( dynamic_cast<InlineNode*>( aNode ) )
    {
      DEBTRACE(">>Inline");
      YACSGui_InlineNodePage* aINPage = 
	dynamic_cast<YACSGui_InlineNodePage*>( anIP->getPage( YACSGui_InputPanel::InlineNodeId ) );
      if ( aINPage )
      {
        YACS::HMI::SubjectNode* aSub = aNodeItem->getSNode();
	aINPage->setSNode( aSub );
	anIP->setOn( true, YACSGui_InputPanel::InlineNodeId );
	anIP->setMode( YACSGui_InputPanel::EditMode, YACSGui_InputPanel::InlineNodeId );
	anIP->setExclusiveVisible( true,  list<int>(1,YACSGui_InputPanel::InlineNodeId) );
	anIP->show();
      }
    }
    else if ( dynamic_cast<Switch*>( aNode ) )
    {
      DEBTRACE(">>Switch");
      YACSGui_SwitchNodePage* aSNPage = 
	dynamic_cast<YACSGui_SwitchNodePage*>( anIP->getPage( YACSGui_InputPanel::SwitchNodeId ) );
      if ( aSNPage )
      {
	aSNPage->setSNode( aNodeItem->getSNode() );
	anIP->setOn( true, YACSGui_InputPanel::SwitchNodeId );
	anIP->setMode( YACSGui_InputPanel::EditMode, YACSGui_InputPanel::SwitchNodeId );
	anIP->setExclusiveVisible( true,  list<int>(1,YACSGui_InputPanel::SwitchNodeId) );
	anIP->show();
      }
    }
    else if ( dynamic_cast<ForLoop*>( aNode ) )
    {
      DEBTRACE(">>For");
      YACSGui_ForLoopNodePage* aFLNPage = 
	dynamic_cast<YACSGui_ForLoopNodePage*>( anIP->getPage( YACSGui_InputPanel::ForLoopNodeId ) );
      if ( aFLNPage )
      {
	aFLNPage->setSNode( aNodeItem->getSNode() );
	anIP->setOn( true, YACSGui_InputPanel::ForLoopNodeId );
	anIP->setMode( YACSGui_InputPanel::EditMode, YACSGui_InputPanel::ForLoopNodeId );
	anIP->setExclusiveVisible( true,  list<int>(1,YACSGui_InputPanel::ForLoopNodeId) );
	anIP->show();
      }
    }
    else if ( dynamic_cast<ForEachLoop*>( aNode ) )
    {
      DEBTRACE(">>ForEach");
      YACSGui_ForEachLoopNodePage* aFELNPage = 
	dynamic_cast<YACSGui_ForEachLoopNodePage*>( anIP->getPage( YACSGui_InputPanel::ForEachLoopNodeId ) );
      if ( aFELNPage )
      {
	aFELNPage->setSNode( aNodeItem->getSNode() );
	anIP->setOn( true, YACSGui_InputPanel::ForEachLoopNodeId );
	anIP->setMode( YACSGui_InputPanel::EditMode, YACSGui_InputPanel::ForEachLoopNodeId );
	anIP->setExclusiveVisible( true,  list<int>(1,YACSGui_InputPanel::ForEachLoopNodeId) );
	anIP->show();
      }
    }  
    else if ( dynamic_cast<WhileLoop*>( aNode ) )
    {
      DEBTRACE(">>While");
      YACSGui_WhileLoopNodePage* aWLNPage = 
	dynamic_cast<YACSGui_WhileLoopNodePage*>( anIP->getPage( YACSGui_InputPanel::WhileLoopNodeId ) );
      if ( aWLNPage )
      {
	aWLNPage->setSNode( aNodeItem->getSNode() );
	anIP->setOn( true, YACSGui_InputPanel::WhileLoopNodeId );
	anIP->setMode( YACSGui_InputPanel::EditMode, YACSGui_InputPanel::WhileLoopNodeId );
	anIP->setExclusiveVisible( true,  list<int>(1,YACSGui_InputPanel::WhileLoopNodeId) );
	anIP->show();
      }
    }
    else if ( dynamic_cast<Bloc*>( aNode ) )
    {
      DEBTRACE(">>Bloc");
      YACSGui_BlockNodePage* aBNPage = 
	dynamic_cast<YACSGui_BlockNodePage*>( anIP->getPage( YACSGui_InputPanel::BlockNodeId ) );
      if ( aBNPage )
      {
	aBNPage->setSNode( aNodeItem->getSNode() );
	anIP->setOn( true, YACSGui_InputPanel::BlockNodeId );
	anIP->setMode( YACSGui_InputPanel::EditMode, YACSGui_InputPanel::BlockNodeId );
	anIP->setExclusiveVisible( true,  list<int>(1,YACSGui_InputPanel::BlockNodeId) );
	anIP->show();
      }
    }
  }
  else if ( YACSGui_LinkViewItem* aLinkItem = dynamic_cast<YACSGui_LinkViewItem*>( anItem ) )
  { // this is a link data object
    // YACS::ENGINE::Port* anOutPort = aLinkItem->getOutPort();
    // YACS::ENGINE::Port* anInPort = aLinkItem->getInPort();
    // the following method can be used if its needed:
    // to get link presentation here, we need to reimplement the ItemMap in the YACSGui_Graph :
    // typedef map< pair<void*, void*>, void*> ItemMap
    // and maps
    // map< pair<YACS::ENGINE::Node*,0>, YACSPrs_ElementaryNode*>
    // or
    // typedef map< pair<YACS::ENGINE::Port* anOutPort, YACS::ENGINE::Port* anInPort>, YACSPrs_Link*>
    // Therefore, it is needed to redefine YACSGui_Graph::getItem(...) method!
    DEBTRACE(">> Show Input Panel for a given link (<Port*,Port*>)");

    /*
    YACSGui_LinkNodePage* aLPage = 
      dynamic_cast<YACSGui_LinkNodePage*>( anIP->getPage( YACSGui_InputPanel::LinkId ) );
    if ( aLPage )
    {
      aLPage->setSLink( aLinkItem->getSLink() );
      anIP->setOn( true, YACSGui_InputPanel::LinkId );
      anIP->setExclusiveVisible( true,  list<int>(1,YACSGui_InputPanel::LinkId) );
      anIP->show();
    }
    */
    anIP->hide();
  }
  else if ( YACSGui_ContainerViewItem* aContItem = dynamic_cast<YACSGui_ContainerViewItem*>( anItem ) )
  { // this is a container data object
    // find or create a property page for a container
    DEBTRACE(">> Show Input Panel for a given container (SalomeContainer*>)");

    YACSGui_ContainerPage* aCPage = dynamic_cast<YACSGui_ContainerPage*>( anIP->getPage( YACSGui_InputPanel::ContainerId ) );
    if ( aCPage )
    {
      aCPage->setSContainer( aContItem->getSContainer() );
      anIP->setOn( true, YACSGui_InputPanel::ContainerId );
      anIP->setExclusiveVisible( true, list<int>(1,YACSGui_InputPanel::ContainerId) );
      anIP->show();
    }
  }
  else if ( YACSGui_ComponentViewItem* aCompItem = dynamic_cast<YACSGui_ComponentViewItem*>( anItem ) )
  { // this is a component data object
    // YACS::ENGINE::ComponentInstance* aComponent = aCompItem->getComponent();
    DEBTRACE(">> Show Input Panel for a given component (ComponentInstance*>)");

    YACSGui_ComponentPage* aCompPage = dynamic_cast<YACSGui_ComponentPage*>( anIP->getPage( YACSGui_InputPanel::ComponentId ) );
    if ( aCompPage )
    {
      aCompPage->setSComponent(aCompItem->getSComponent());
      anIP->setOn( true, YACSGui_InputPanel::ComponentId );
      anIP->setExclusiveVisible( true, list<int>(1,YACSGui_InputPanel::ComponentId) );
      anIP->show();
    }
  }
  else
    anIP->hide();
}

//! Private slot.  Shows the Input Panel with property pages acoording to the selected item.
/*!
 */
void YACSGui_EditionTreeView::onSelectionChanged()
{
  DEBTRACE("YACSGui_EditionTreeView::onSelectionChanged" << " " << myPreviousSelected << " " << itemToCheck);
  if ( !getProc() || !myModule ) return;
  if(itemToCheck && itemToCheck==myPreviousSelected)return;

  bool isNewSelection = true;
  bool isWarn = false;
  
  // get the list of selected items
  list<QListViewItem*> aSelList = getSelected();
  
  QListViewItem* anItem = 0;
  
  // check if the current selection is a single selection
  if ( aSelList.size() == 1 ) anItem = aSelList.front();
  
  DEBTRACE("YACSGui_EditionTreeView::onSelectionChanged");
  YACSGui_InputPanel* anIP = myModule->getInputPanel();
  if ( anItem && anIP )
  {
  DEBTRACE("YACSGui_EditionTreeView::onSelectionChanged");
    // check if the case selection of switch node is active
    if( ((QWidget*)anIP)->isVisible() && anIP->isVisible( YACSGui_InputPanel::SwitchNodeId ) )
    {
      DEBTRACE("SwitchNodeId visible");
      YACSGui_SwitchNodePage* aSNPage = dynamic_cast<YACSGui_SwitchNodePage*>( anIP->getPage( YACSGui_InputPanel::SwitchNodeId ) );
      if ( aSNPage && aSNPage->isSelectChild() )
      {
  DEBTRACE("YACSGui_EditionTreeView::onSelectionChanged");
	isWarn = false;
	if ( YACSGui_NodeViewItem* aNodeItem = dynamic_cast<YACSGui_NodeViewItem*>( anItem ) )
	{
	  if ( aSNPage->getNode() != aNodeItem->getNode() )
	  {
	    aSNPage->setChild(aNodeItem->getSNode());
	    
	    // change selection from the child node back to the switch node
	    bool block = signalsBlocked();
	    blockSignals( true );
	    setSelected( aNodeItem, false );
	    
	    QString aName = aSNPage->getNodeName();
	    QListViewItem* aSNItem = 0;
	    bool continueSearch = true;
	    while ( continueSearch )
	    {
	      aSNItem = findItem( aName, 0 );
	      if ( YACSGui_NodeViewItem* aSNVItem = dynamic_cast<YACSGui_NodeViewItem*>( aSNItem ) )
		if ( aSNPage->getNode() == aSNVItem->getNode() )
		  continueSearch = false;
	    }
	    if( aSNItem )
	      setSelected( aSNItem, true );
	    blockSignals( block );
	  }
	}
	else
	{ // remove selection from incompatible object (which is not a node)
	  setSelected( anItem, false );
	}
	return;
      }
    }
    
    // check if the child node selection of block node is active
    if( ((QWidget*)anIP)->isVisible() && anIP->isVisible( YACSGui_InputPanel::BlockNodeId ) )
    {
      DEBTRACE("BlockNodeId visible");
      YACSGui_BlockNodePage* aBNPage = dynamic_cast<YACSGui_BlockNodePage*>( anIP->getPage( YACSGui_InputPanel::BlockNodeId ) );
      if ( aBNPage && aBNPage->isSelectChild() )
      {
  DEBTRACE("YACSGui_EditionTreeView::onSelectionChanged");
	isWarn = false;
	if ( YACSGui_NodeViewItem* aNodeItem = dynamic_cast<YACSGui_NodeViewItem*>( anItem ) )
	{
  DEBTRACE("YACSGui_EditionTreeView::onSelectionChanged");
	  if ( aBNPage->getNode() != aNodeItem->getNode() )
	  {
  DEBTRACE("YACSGui_EditionTreeView::onSelectionChanged");
	    aBNPage->setChild(aNodeItem->getSNode());
	    
	    // change selection from the child node back to the block node
	    bool block = signalsBlocked();
	    blockSignals( true );
	    setSelected( aNodeItem, false );
	    
	    QString aName = aBNPage->getNodeName();
	    QListViewItem* aBNItem = 0;
	    bool continueSearch = true;
	    while ( continueSearch )
	    {
	      aBNItem = findItem( aName, 0 );
	      if ( YACSGui_NodeViewItem* aBNVItem = dynamic_cast<YACSGui_NodeViewItem*>( aBNItem ) )
		if ( aBNPage->getNode() == aBNVItem->getNode() )
		  continueSearch = false;
	    }
	    if( aBNItem )
	      setSelected( aBNItem, true );
	    blockSignals( block );
	  }
	}
	else
	{ // remove selection from incompatible object (which is not a node)
	  setSelected( anItem, false );
	}
	return;
      }
    }

  DEBTRACE("YACSGui_EditionTreeView::onSelectionChanged");
    // check if the data type selection is active
    if( ((QWidget*)anIP)->isVisible()
	&&
	( anIP->isVisible( YACSGui_InputPanel::InlineNodeId ) ) )
    {
      DEBTRACE("InlineNodeId visible");
      YACSGui_InlineNodePage* aSNPage = dynamic_cast<YACSGui_InlineNodePage*>( anIP->getPage( YACSGui_InputPanel::InlineNodeId ) );
      if ( aSNPage && aSNPage->isSelectDataType() )
      {
        DEBTRACE("aSNPage->isSelectDataType()");
       	if ( YACSGui_DataTypeItem* aDTItem = dynamic_cast<YACSGui_DataTypeItem*>( anItem ) )
	  aSNPage->setDataType(aDTItem->getSDataType()->getTypeCode());
	
	bool block = signalsBlocked();
	blockSignals( true );
	setSelected( anItem, false );
	QListViewItem* aSNItem = findItem( aSNPage->getNodeName(), 0 );
	if( aSNItem )
	  setSelected( aSNItem, true );
	blockSignals( block );
	
	return;
      }
    }
    DEBTRACE("YACSGui_EditionTreeView::onSelectionChanged");
    if (((QWidget*)anIP)->isVisible())
      {
	int pageId=-1;
        if(anIP->isVisible( YACSGui_InputPanel::InDataNodeId )) pageId = YACSGui_InputPanel::InDataNodeId;
        else if (anIP->isVisible( YACSGui_InputPanel::OutDataNodeId )) pageId = YACSGui_InputPanel::OutDataNodeId;
        else if (anIP->isVisible( YACSGui_InputPanel::InStudyNodeId )) pageId = YACSGui_InputPanel::InStudyNodeId;
        else if (anIP->isVisible( YACSGui_InputPanel::OutStudyNodeId )) pageId = YACSGui_InputPanel::OutStudyNodeId;
        if (pageId != -1)
          {
            DEBTRACE("dataNodeId visible");
            YACSGui_DataNodePage* aDNPage = dynamic_cast<YACSGui_DataNodePage*>( anIP->getPage( pageId ) );
            if ( aDNPage && aDNPage->isSelectDataType() )
              {
                DEBTRACE("aDNPage->isSelectDataType()");
                if ( YACSGui_DataTypeItem* aDTItem = dynamic_cast<YACSGui_DataTypeItem*>( anItem ) )
                  aDNPage->setDataType(aDTItem->getSDataType()->getTypeCode());
                
                bool block = signalsBlocked();
                blockSignals( true );
                setSelected( anItem, false );
                QListViewItem* aSNItem = findItem( aDNPage->getNodeName(), 0 );
                if( aSNItem )
                  setSelected( aSNItem, true );
                blockSignals( block );
                
                return;
              }
          }
      }
  }

  DEBTRACE("YACSGui_EditionTreeView::onSelectionChanged");

  //Check if input panel has been modified and not saved
  QListViewItem* previousItem = myPreviousSelected;
  QListViewItem* nextItem = anItem;

  if ( aSelList.size() == 0 ) // nothing selected, deselect previous
  {
    if ( myPreviousSelected )
    {
      myPreviousSelected = 0;
      isWarn = true;
    }
  }
  
  // creation of a link -->
  YACSGui_ViewItem* aVItem = 0;
  if ( anItem ) // an item selected
  {
    if ( anItem != myPreviousSelected ) // a new item is selected
    {
      if ( myPreviousSelected ) isWarn = true;
      myPreviousSelected = anItem;
      aVItem = dynamic_cast<YACSGui_ViewItem*>(anItem);
    }
    else
      isNewSelection = false;
  }
  else if ( myPreviousSelected ) // nothing selected, deselect previous (multiselection case)
  {
    anItem = myPreviousSelected;
    myPreviousSelected = 0;
    aVItem = dynamic_cast<YACSGui_ViewItem*>(anItem);
    isWarn = true;
  }

  itemToCheck=0;
  if ( isWarn )
    {
      itemToCheck=previousItem;
      try
        {
          warnAboutSelectionChanged();
          itemToCheck=0;
        }
      catch(YACS::Exception&)
        {
          myPreviousSelected=previousItem;
          if(nextItem)
            setSelected(nextItem,false);
          setSelected(myPreviousSelected,true);
          itemToCheck=0;
          return;
        }
    }
  //End of check

  if ( aVItem )
  {
    //aVItem->getSubject()->select(anItem->isSelected());
    if ( mySelectedSubjectOutPort )
    {
      if ( YACSGui_PortViewItem* aPItem = dynamic_cast<YACSGui_PortViewItem*>(aVItem) )
      {
	SubjectDataPort* aSubPort = aPItem->getSPort();
	if ( dynamic_cast<SubjectInputPort*>(aSubPort) || dynamic_cast<SubjectInputDataStreamPort*>(aSubPort) )
	{
	  if ( !SubjectDataPort::tryCreateLink( mySelectedSubjectOutPort, aSubPort ,control) )
	    SUIT_MessageBox::warn1(myModule->getApp()->desktop(), 
				   tr("WARNING"), 
				   GuiContext::getCurrent()->_lastErrorMessage,
				   tr("BUT_OK"));
	  else myModule->temporaryExport();
	  mySelectedSubjectOutPort = 0;
	}
      }
    }
    if ( mySelectedSubjectOutNode )
    {
      if ( YACSGui_NodeViewItem* aNItem = dynamic_cast<YACSGui_NodeViewItem*>(aVItem) )
      {
	if ( SubjectNode* aSubNode = aNItem->getSNode() )
	{
	  if ( !SubjectNode::tryCreateLink( mySelectedSubjectOutNode, aSubNode ) )
	    SUIT_MessageBox::warn1(myModule->getApp()->desktop(), 
				   tr("WARNING"), 
				   GuiContext::getCurrent()->_lastErrorMessage,
				   tr("BUT_OK"));
	  else myModule->temporaryExport();
	  mySelectedSubjectOutNode = 0;
	}
      }
    }
  }
  // creation of a link <--

  DEBTRACE("YACSGui_EditionTreeView::onSelectionChanged");

  if ( anIP ) // --- force edit mode to close the catalog, when click on tree or canvas item
    {
      myModule->setGuiMode(YACSGui_Module::EditMode);
      YACSGui_ServiceNodePage* aSNPage = dynamic_cast<YACSGui_ServiceNodePage*>(anIP->getPage(YACSGui_InputPanel::ServiceNodeId ));
      if (aSNPage) aSNPage->setMode(YACSGui_InputPanel::EditMode);
    }

  DEBTRACE("YACSGui_EditionTreeView::onSelectionChanged");
  if ( isNewSelection ) syncPageTypeWithSelection();
  DEBTRACE("YACSGui_EditionTreeView::onSelectionChanged");
  syncHMIWithSelection();
}

//! Private slot.  Selects the original node in the tree view if a reference node is double clicked.
/*!
 */
void YACSGui_EditionTreeView::onDblClick( QListViewItem* theItem )
{
  if ( YACSGui_ReferenceViewItem* aRNItem = dynamic_cast<YACSGui_ReferenceViewItem*>( theItem ) )
  {
    QListViewItemIterator anIt( this );
    for ( ; anIt.current(); anIt++ )
    {
      YACSGui_NodeViewItem* original = dynamic_cast<YACSGui_NodeViewItem*>( anIt.current() );
      if ( original && original->getNode() == aRNItem->getNode() )
      {
	setSelected( original, true );
	
	QListViewItem* p = original->parent();
	while( p )
	{
	  setOpen( p, true );
	  p = p->parent();
	}
	
	setCurrentItem( original );
	ensureItemVisible( original );
	emitSelectionChanged();
	//emit selectionChanged();
	
	break;
      }
    }
  }
}

void YACSGui_EditionTreeView::syncHMIWithSelection()
{
  // notify about selection other observers
  DEBTRACE("YACSGui_EditionTreeView::syncHMIWithSelection");

  std::list<QListViewItem*> aSelList = getSelected();

  // notify about selection
  list<QListViewItem*>::iterator it;
  for ( it = aSelList.begin(); it != aSelList.end(); ++it )
  {
    YACSGui_ViewItem* item = dynamic_cast<YACSGui_ViewItem*>( *it );
    if ( item )
    {
      DEBTRACE(item);
      YACS::HMI::Subject* aSub = getSubject( *it );
      if ( aSub )
      {
        DEBTRACE(aSub);
        DEBTRACE(aSub->getName());
        bool isBlocked = item->blockSelection( true );
        aSub->select( true );
        item->blockSelection( isBlocked );
        myLastSelected.erase( item );
        DEBTRACE("myLastSelected.erase " << item);
      }
    }
  }

  // notify about deselection 
  std::set< QListViewItem* >::iterator lIt;
  for ( lIt = myLastSelected.begin(); lIt != myLastSelected.end(); ++lIt )
  { 
    YACS::HMI::Subject* aSub = getSubject( *lIt );
    if ( aSub )
      {
        aSub->select( false );
        DEBTRACE("myLastSelected subject->select(false) " << *lIt);
      }
  }

  // fill last mySelected
  myLastSelected.clear();
  for ( it = aSelList.begin(); it != aSelList.end(); ++it )
    {
      myLastSelected.insert( *it );
      DEBTRACE("myLastSelected.insert " << *it);
    }
  myModule->updateViewer();
}

void YACSGui_EditionTreeView::emitSelectionChanged()
{
  emit selectionChanged();
}

//! Constructor.
/*!
 */
YACSGui_RunTreeView::YACSGui_RunTreeView( YACSGui_Module* theModule, YACS::HMI::SubjectProc* theSProc,
					  QWidget* theParent, const char* theName, WFlags theFlags )
  : YACSGui_TreeView( theModule, theSProc, theParent, theName, theFlags )
{
  if ( getProc() )
  {
    setCaption( tr( "RUN_TREE_VIEW" ) + QString( " of " ) + QString( getProc()->getName() ) );

    addColumn( "Nodes", 250 );
    addColumn( "State", 100 );

    build();

    connect( this, SIGNAL( clicked( QListViewItem* ) ), this, SLOT( onBreakpointClicked( QListViewItem* ) ) );
    connect( this, SIGNAL( selectionChanged() ), this, SLOT( onSelectionChanged() ) );
    connect( this, SIGNAL( contextMenuRequested( QListViewItem *, const QPoint& , int ) ),
             this, SLOT( onMenuRequested( QListViewItem *, const QPoint &, int ) ) );
  }
}

//! Destructor.
/*!
 */
YACSGui_RunTreeView::~YACSGui_RunTreeView()
{
}

void YACSGui_RunTreeView::onMenuRequested( QListViewItem* item, const QPoint & point, int col )
{
  DEBTRACE("lookForExecutor()");
  YACSGui_Executor* anExecutor = myModule->lookForExecutor();
  if ( !anExecutor ) return;

  if (YACSGui_ComposedNodeViewItem* anItem = dynamic_cast<YACSGui_ComposedNodeViewItem*>(item) )
    {
      anItem->popup(anExecutor,point);
    }
  else if(YACSGui_ElementaryNodeViewItem* anItem = dynamic_cast<YACSGui_ElementaryNodeViewItem*>(item))
    {
      anItem->popup(anExecutor,point);
    }
}

//! Udates the tree view starting from the given item.
/*!
 *  \param theItem - the item, which subtree have to be updated.
 *  \param theIsRecursive - the flag for recursive or not recursive update.
 *                          By default theIsRecursive = false.
 */
void YACSGui_RunTreeView::update( QListViewItem* theItem, const bool theIsRecursive )
{
  YACSGui_ComposedNodeViewItem* anItem = dynamic_cast<YACSGui_ComposedNodeViewItem*>( theItem );

  if ( !anItem )
    anItem = dynamic_cast<YACSGui_ComposedNodeViewItem*>( firstChild() ); // get a root item

  if ( anItem && anItem->listView() == this )
  {
    anItem->update( theIsRecursive );
    //triggerUpdate();
  }
}

//! Udates the tree view starting from the given item.
/*!
 *  \param theItem - the item, which subtree have to be updated.
 *  \param theIsRecursive - the flag for recursive or not recursive update.
 *                          By default theIsRecursive = false.
 */
void YACSGui_RunTreeView::update( QCheckListItem* theItem, const bool theIsRecursive )
{
  YACSGui_ElementaryNodeViewItem* anItem = dynamic_cast<YACSGui_ElementaryNodeViewItem*>( theItem );

  if ( !anItem ) return;

  if ( anItem->listView() == this )
  {
    anItem->update( theIsRecursive );
    //triggerUpdate();
  }
}

//! Builds the tree view according to the schema content. Schema is used as the root object.
/*!
 */
void YACSGui_RunTreeView::build()
{
  if ( !getProc() ) return;

  YACSGui_ComposedNodeViewItem* aRootItem = new YACSGui_ComposedNodeViewItem( this, QString( getProc()->getName() ), getProc() );
  myMapListViewItem[getProc()->getNumId()] = aRootItem;
  onNotifyStatus(YACS::NOTYETINITIALIZED);
  addTreeNode( aRootItem, getProc() );
}

void YACSGui_RunTreeView::addTreeNode( QListViewItem* theParent,
				       YACS::ENGINE::ComposedNode* theFather )
{
  list<Node*> aSetOfNodes = theFather->edGetDirectDescendants();
  for ( list<Node*>::iterator iter = aSetOfNodes.begin(); iter != aSetOfNodes.end(); iter++)
  {
    if (ElementaryNode* anElemNode = dynamic_cast<ElementaryNode*>(*iter) )
    {
      YACSGui_ElementaryNodeViewItem* item = new YACSGui_ElementaryNodeViewItem( theParent,
										 (*iter)->getQualifiedName(),
										 YACSGui_ElementaryNodeViewItem::CheckBox,
										 anElemNode );
      myMapListViewItem[(*iter)->getNumId()] = item;
    }
    else
    {
      YACSGui_ComposedNodeViewItem* item = new YACSGui_ComposedNodeViewItem( theParent,
									     (*iter)->getQualifiedName(),
									     (ComposedNode*)(*iter) );
      myMapListViewItem[(*iter)->getNumId()] = item;
      addTreeNode(item, (ComposedNode*)(*iter));
    }
  }
}

void YACSGui_RunTreeView::syncPageTypeWithSelection()
{
  DEBTRACE("YACSGui_RunTreeView::syncPageTypeWithSelection");
  if ( !getProc() || !myModule ) return;

  YACSGui_InputPanel* anIP = myModule->getInputPanel();
  if ( !anIP ) return;

  // get the list of selected items
  list<QListViewItem*> aSelList = getSelected();
  
  QListViewItem* anItem = 0;
  
  // check if the current selection is a single selection
  if ( aSelList.size() == 1 ) anItem = aSelList.front();
    
  if ( YACSGui_ComposedNodeViewItem* aCNItem = dynamic_cast<YACSGui_ComposedNodeViewItem*>( anItem ) )
  { 
    ComposedNode* aNode = aCNItem->getNode();
    if ( !dynamic_cast<Proc*>(aNode) )
    { // show Input Panel with page for composed node in the execution mode
     
      if ( dynamic_cast<Switch*>( aNode ) )
      {
	YACSGui_SwitchNodePage* aSNPage = 
	  dynamic_cast<YACSGui_SwitchNodePage*>( anIP->getPage( YACSGui_InputPanel::SwitchNodeId ) );
	if ( aSNPage )
	{
	  aSNPage->setSNode( GuiContext::getCurrent()->_mapOfSubjectNode[aNode] );
	  anIP->setOn( true, YACSGui_InputPanel::SwitchNodeId );
	  anIP->setMode( YACSGui_InputPanel::RunMode, YACSGui_InputPanel::SwitchNodeId );
	  anIP->setExclusiveVisible( true,  list<int>(1,YACSGui_InputPanel::SwitchNodeId) );
	  anIP->show();
	}
      }
      else if ( dynamic_cast<ForLoop*>( aNode ) )
      {
	YACSGui_ForLoopNodePage* aFLNPage = 
	  dynamic_cast<YACSGui_ForLoopNodePage*>( anIP->getPage( YACSGui_InputPanel::ForLoopNodeId ) );
	if ( aFLNPage )
	{
	  aFLNPage->setSNode( GuiContext::getCurrent()->_mapOfSubjectNode[aNode] );
	  anIP->setOn( true, YACSGui_InputPanel::ForLoopNodeId );
	  anIP->setMode( YACSGui_InputPanel::RunMode, YACSGui_InputPanel::ForLoopNodeId );
	  anIP->setExclusiveVisible( true,  list<int>(1,YACSGui_InputPanel::ForLoopNodeId) );
	  anIP->show();
	}
      }
      else if ( dynamic_cast<ForEachLoop*>( aNode ) )
      {
	YACSGui_ForEachLoopNodePage* aFELNPage = 
	  dynamic_cast<YACSGui_ForEachLoopNodePage*>( anIP->getPage( YACSGui_InputPanel::ForEachLoopNodeId ) );
	if ( aFELNPage )
	{
	  aFELNPage->setSNode( GuiContext::getCurrent()->_mapOfSubjectNode[aNode] );
	  anIP->setOn( true, YACSGui_InputPanel::ForEachLoopNodeId );
	  anIP->setMode( YACSGui_InputPanel::RunMode, YACSGui_InputPanel::ForEachLoopNodeId );
	  anIP->setExclusiveVisible( true,  list<int>(1,YACSGui_InputPanel::ForEachLoopNodeId) );
	  anIP->show();
	}
      }  
      else if ( dynamic_cast<WhileLoop*>( aNode ) )
      {
	YACSGui_WhileLoopNodePage* aWLNPage = 
	  dynamic_cast<YACSGui_WhileLoopNodePage*>( anIP->getPage( YACSGui_InputPanel::WhileLoopNodeId ) );
	if ( aWLNPage )
	{
	  aWLNPage->setSNode( GuiContext::getCurrent()->_mapOfSubjectNode[aNode] );
	  anIP->setOn( true, YACSGui_InputPanel::WhileLoopNodeId );
	  anIP->setMode( YACSGui_InputPanel::RunMode, YACSGui_InputPanel::WhileLoopNodeId );
	  anIP->setExclusiveVisible( true,  list<int>(1,YACSGui_InputPanel::WhileLoopNodeId) );
	  anIP->show();
	}
      }
      else if ( dynamic_cast<Bloc*>( aNode ) )
      {
	YACSGui_BlockNodePage* aBNPage = 
	  dynamic_cast<YACSGui_BlockNodePage*>( anIP->getPage( YACSGui_InputPanel::BlockNodeId ) );
	if ( aBNPage )
	{
	  aBNPage->setSNode( GuiContext::getCurrent()->_mapOfSubjectNode[aNode] );
	  anIP->setOn( true, YACSGui_InputPanel::BlockNodeId );
	  anIP->setMode( YACSGui_InputPanel::RunMode, YACSGui_InputPanel::BlockNodeId );
	  anIP->setExclusiveVisible( true,  list<int>(1,YACSGui_InputPanel::BlockNodeId) );
	  anIP->show();
	}
      }
    }
    else
    { // show Input Panel with page for a schema in the execution mode
      YACSGui_SchemaPage* aSPage = dynamic_cast<YACSGui_SchemaPage*>( anIP->getPage( YACSGui_InputPanel::SchemaId ) );
      if ( aSPage )
      {
	aSPage->setSProc( GuiContext::getCurrent()->getSubjectProc() );
	anIP->setOn( true, YACSGui_InputPanel::SchemaId );
	anIP->setMode( YACSGui_InputPanel::RunMode, YACSGui_InputPanel::SchemaId );
	anIP->setExclusiveVisible( true,  list<int>(1,YACSGui_InputPanel::SchemaId) );
        std::string  theErrorLog;
        DEBTRACE("lookForExecutor()");
        YACSGui_Executor* anExecutor = myModule->lookForExecutor();
        if ( anExecutor )
          {
            theErrorLog = anExecutor->getErrorReport(getProc());
          }
        aSPage->myErrorLog->setText(theErrorLog.c_str());
	anIP->show();
      }
    }
  }
  else if ( YACSGui_ElementaryNodeViewItem* aENItem = dynamic_cast<YACSGui_ElementaryNodeViewItem*>( anItem ) )
  { // show Input Panel with page for elementary node in the execution mode
    ElementaryNode* aNode = aENItem->getNode();

    if ( ServiceNode* sNode = dynamic_cast<ServiceNode*>( aNode ) )
    {
      if ( sNode->getKind() == "XML" )
      {
	/*
	YACSGui_ServiceInlineNodePage* aSINPage = 
	  dynamic_cast<YACSGui_ServiceInlineNodePage*>( anIP->getPage( YACSGui_InputPanel::ServiceInlineNodeId ) );
	if ( aSINPage )
	{
	  aSINPage->setSNode( GuiContext::getCurrent()->_mapOfSubjectNode[aNode] );
	  anIP->setOn( true, YACSGui_InputPanel::ServiceInlineNodeId );
	  anIP->setMode( YACSGui_InputPanel::RunMode, YACSGui_InputPanel::ServiceInlineNodeId );
	  anIP->setExclusiveVisible( true,  list<int>(1,YACSGui_InputPanel::ServiceInlineNodeId) );
	  anIP->show();
	}
	*/
      }
      else
      {
	YACSGui_ServiceNodePage* aSNPage = 
	  dynamic_cast<YACSGui_ServiceNodePage*>( anIP->getPage( YACSGui_InputPanel::ServiceNodeId ) );
	if ( aSNPage )
	{
	  aSNPage->setSNode( GuiContext::getCurrent()->_mapOfSubjectNode[aNode] );
	  anIP->setOn( true, YACSGui_InputPanel::ServiceNodeId );
	  anIP->setMode( YACSGui_InputPanel::RunMode, YACSGui_InputPanel::ServiceNodeId );
	  anIP->setExclusiveVisible( true,  list<int>(1,YACSGui_InputPanel::ServiceNodeId) );
	  anIP->show();
	}
      }
    }
    else if ( dynamic_cast<PresetNode*>( aNode ) )
    {
      YACSGui_DataNodePage* aINPage = 
	dynamic_cast<YACSGui_DataNodePage*>( anIP->getPage( YACSGui_InputPanel::InDataNodeId ) );
      if ( aINPage )
      {
	aINPage->setSNode( GuiContext::getCurrent()->_mapOfSubjectNode[aNode] );
	anIP->setOn( true, YACSGui_InputPanel::InDataNodeId );
	anIP->setMode( YACSGui_InputPanel::RunMode, YACSGui_InputPanel::InDataNodeId );
	anIP->setExclusiveVisible( true,  list<int>(1,YACSGui_InputPanel::InDataNodeId) );
	anIP->show();
      }
    }
    else if ( dynamic_cast<OutNode*>( aNode ) )
    {
      YACSGui_DataNodePage* aINPage = 
	dynamic_cast<YACSGui_DataNodePage*>( anIP->getPage( YACSGui_InputPanel::OutDataNodeId ) );
      if ( aINPage )
      {
	aINPage->setSNode( GuiContext::getCurrent()->_mapOfSubjectNode[aNode] );
	anIP->setOn( true, YACSGui_InputPanel::OutDataNodeId );
	anIP->setMode( YACSGui_InputPanel::RunMode, YACSGui_InputPanel::OutDataNodeId );
	anIP->setExclusiveVisible( true,  list<int>(1,YACSGui_InputPanel::OutDataNodeId) );
	anIP->show();
      }
    }
    else if ( dynamic_cast<StudyInNode*>( aNode ) )
    {
      YACSGui_DataNodePage* aINPage = 
	dynamic_cast<YACSGui_DataNodePage*>( anIP->getPage( YACSGui_InputPanel::InStudyNodeId ) );
      if ( aINPage )
      {
	aINPage->setSNode( GuiContext::getCurrent()->_mapOfSubjectNode[aNode] );
	anIP->setOn( true, YACSGui_InputPanel::InStudyNodeId );
	anIP->setMode( YACSGui_InputPanel::RunMode, YACSGui_InputPanel::InStudyNodeId );
	anIP->setExclusiveVisible( true,  list<int>(1,YACSGui_InputPanel::InStudyNodeId) );
	anIP->show();
      }
    }
    else if ( dynamic_cast<StudyOutNode*>( aNode ) )
    {
      YACSGui_DataNodePage* aINPage = 
	dynamic_cast<YACSGui_DataNodePage*>( anIP->getPage( YACSGui_InputPanel::OutStudyNodeId ) );
      if ( aINPage )
      {
	aINPage->setSNode( GuiContext::getCurrent()->_mapOfSubjectNode[aNode] );
	anIP->setOn( true, YACSGui_InputPanel::OutStudyNodeId );
	anIP->setMode( YACSGui_InputPanel::RunMode, YACSGui_InputPanel::OutStudyNodeId );
	anIP->setExclusiveVisible( true,  list<int>(1,YACSGui_InputPanel::OutStudyNodeId) );
	anIP->show();
      }
    }
    else if ( dynamic_cast<InlineNode*>( aNode ) )
    {
      YACSGui_InlineNodePage* aINPage = 
	dynamic_cast<YACSGui_InlineNodePage*>( anIP->getPage( YACSGui_InputPanel::InlineNodeId ) );
      if ( aINPage )
      {
	aINPage->setSNode( GuiContext::getCurrent()->_mapOfSubjectNode[aNode] );
	anIP->setOn( true, YACSGui_InputPanel::InlineNodeId );
	anIP->setMode( YACSGui_InputPanel::RunMode, YACSGui_InputPanel::InlineNodeId );
	anIP->setExclusiveVisible( true,  list<int>(1,YACSGui_InputPanel::InlineNodeId) );
	anIP->show();
      }
    }
  }
  else
    anIP->hide();
}

void YACSGui_RunTreeView::onSelectionChanged()
{
  DEBTRACE("YACSGui_RunTreeView::onSelectionChanged");
  YACSGui_InputPanel* anIP = myModule->getInputPanel();
  if ( !anIP ) return;
  list<int> anIds = anIP->getVisiblePagesIds();
  for ( list<int>::iterator anIter = anIds.begin(); anIter != anIds.end(); ++anIter )
  {
    int anId = (*anIter);
    switch (anId)
    {
    case YACSGui_InputPanel::SchemaId:
      {
	if ( YACSGui_SchemaPage* aSPage =
	     dynamic_cast<YACSGui_SchemaPage*>( anIP->getPage( YACSGui_InputPanel::SchemaId ) ) )
	  aSPage->checkModifications();
      }
      break;
    default:
      break;
    }
  }
  syncPageTypeWithSelection();
  syncHMIWithSelection();
}
void YACSGui_RunTreeView::removeLastSelected(YACSGui_ViewItem* v)
{
  myLastSelected.erase(v);
}

YACS::HMI::Subject* YACSGui_RunTreeView::getSubject( QListViewItem* theItem )
{
  Subject* aSub = 0;

  if ( YACSGui_ElementaryNodeViewItem* anItem = dynamic_cast<YACSGui_ElementaryNodeViewItem*>( theItem ) )
    aSub = anItem->getSubject();
  else if ( YACSGui_ComposedNodeViewItem* anCompItem = dynamic_cast<YACSGui_ComposedNodeViewItem*>( theItem ) )
    aSub = anCompItem->getSubject();  
  
  return aSub;
}

/*!
 *  If a breakpoint is set, the CORBA engine of YACS must be instanciated (findExecutor)
 *  to update the list of breakpoints.
 *  While there is no breakpoint, there is no need to instanciate the CORBA engine.
 */
void YACSGui_RunTreeView::onBreakpointClicked( QListViewItem* theItem )
{
  YACSGui_ElementaryNodeViewItem* anElemNodeItem = dynamic_cast<YACSGui_ElementaryNodeViewItem*>(theItem);
  if ( anElemNodeItem )
  {
    YACSGui_Executor* anExecutor = 0;

    if ( anElemNodeItem->isOn() )
    {
      DEBTRACE("findExecutor()");
      anExecutor = myModule->findExecutor();
      if ( !anExecutor ) return;
      if ( anExecutor->getCurrentExecMode() == YACS_ORB::CONTINUE )
	anExecutor->setBreakpointMode();

      myBreakpointSet.insert(anElemNodeItem->getNode()->getNumId());
    }
    else
      myBreakpointSet.erase(anElemNodeItem->getNode()->getNumId());

    list<string> aBreakpointList;
    for (set<int>::iterator pos = myBreakpointSet.begin(); pos != myBreakpointSet.end(); ++pos)
    {
      string aNodeName = 
	getProc()->getChildName(((YACSGui_ElementaryNodeViewItem*)(myMapListViewItem[*pos]))->getNode());
      aBreakpointList.push_back(aNodeName);
    }
    if (! aBreakpointList.empty())
      {
        anExecutor = myModule->findExecutor();
        assert(anExecutor);
        anExecutor->setBreakpointList(aBreakpointList);
      }
  }
}

void YACSGui_RunTreeView::onNotifyNodeStatus( int theNodeId, int theStatus )
{
  QListViewItem* anItem = myMapListViewItem[theNodeId];
  if (!anItem) return;
  YACSGui_ElementaryNodeViewItem* aNItem = dynamic_cast<YACSGui_ElementaryNodeViewItem*>(anItem);
  if (aNItem)
  {
    aNItem->setState(theStatus);
    return;
  }
  YACSGui_ComposedNodeViewItem* aCNItem = dynamic_cast<YACSGui_ComposedNodeViewItem*>(anItem);
  if (aCNItem)
  {
    aCNItem->setState(theStatus);
    return;
  }
  return;
}

void YACSGui_RunTreeView::onNotifyStatus( int theStatus )
{
  if ( YACSGui_ComposedNodeViewItem* aRootItem = dynamic_cast<YACSGui_ComposedNodeViewItem*>(myMapListViewItem[getProc()->getNumId()]) )
    {
      aRootItem->setStatus(theStatus);
      if(theStatus == YACS::FINISHED)
        setSelected(firstChild(),true);
    }
}

void YACSGui_RunTreeView::syncHMIWithSelection()
{
  DEBTRACE("YACSGui_RunTreeView::syncHMIWithSelection");
  // notify about selection other observers

  std::list<QListViewItem*> aSelList = getSelected();

  // notify about selection
  list<QListViewItem*>::iterator it;
  for ( it = aSelList.begin(); it != aSelList.end(); ++it )
  {
    if ( *it )
    {
      YACS::HMI::Subject* aSub = getSubject( *it );
      if ( aSub )
      {
        if ( YACSGui_ElementaryNodeViewItem* item = dynamic_cast<YACSGui_ElementaryNodeViewItem*>( *it ) )
        {
          bool isBlocked = item->blockSelection( true );
          aSub->select( true );
          item->blockSelection( isBlocked );
        }
        else if ( YACSGui_ComposedNodeViewItem* cItem = dynamic_cast<YACSGui_ComposedNodeViewItem*>( *it ) )
        {
          bool isBlocked = cItem->blockSelection( true );
          aSub->select( true );
          cItem->blockSelection( isBlocked );
        }
        else 
          continue;
        
        myLastSelected.erase( *it );
      }
    }
  }

  // notify about deselection 
  std::set< QListViewItem* >::iterator lIt;
  for ( lIt = myLastSelected.begin(); lIt != myLastSelected.end(); ++lIt )
  { 
    YACS::HMI::Subject* aSub = getSubject( *lIt );
    if ( aSub )
      aSub->select( false );
  }

  // fill last mySelected
  myLastSelected.clear();
  for ( it = aSelList.begin(); it != aSelList.end(); ++it )
    myLastSelected.insert( *it );

  myModule->updateViewer();
}

