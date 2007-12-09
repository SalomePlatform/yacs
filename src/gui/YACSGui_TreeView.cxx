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

#include <OutputDataStreamPort.hxx>
#include <CalStreamPort.hxx>

#include <ServiceNode.hxx>
#include <InlineNode.hxx>
#include <Switch.hxx>
#include <ForLoop.hxx>
#include <ForEachLoop.hxx>
#include <WhileLoop.hxx>
#include <Bloc.hxx>

#include <ComponentInstance.hxx>
#include <CORBAComponent.hxx>

#include <qpopupmenu.h>

#include <iostream.h> //for debug only

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
    set<Node *> constituents = theNode->edGetDirectDescendants();
    for(set<Node *>::iterator iter=constituents.begin();iter!=constituents.end();iter++)
    {
      // fill control links: check links going from output gate port of each direct child node
      OutGate* anOutGate = ( *iter )->getOutGate();
      if (anOutGate) {
	set<InGate *> anInGates = anOutGate->edSetInGate();
	set<InGate *>::iterator anInGatesIter = anInGates.begin();
	for(; anInGatesIter!=anInGates.end(); anInGatesIter++) {
	  InGate* aCurInGate = *anInGatesIter;
	  if ( constituents.find(aCurInGate->getNode()) != constituents.end() )
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
    set<Node *> constituents = theNode->edGetDirectDescendants();
    list<OutPort *> temp;
    for(set<Node *>::iterator iter=constituents.begin();iter!=constituents.end();iter++)
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
	  if ( constituents.find(aCurInPort->getNode()) != constituents.end() )
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
    set<Node *> constituents = theNode->edGetDirectDescendants();
    list<OutPort *> temp;
    for(set<Node *>::iterator iter=constituents.begin();iter!=constituents.end();iter++)
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
	  if ( constituents.find(aCurInPort->getNode()) != constituents.end() )
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
	  if ( constituents.find(aCurInGate->getNode()) != constituents.end() )
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
    
    addColumn( "Object" );
    
    fillContainerData(mySProc);
    build();

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

  if ( !theSNode ) {
    printf(">> Null theSNode\n");
    return aNodeItem;
  }

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
    set<Node*> aChildNodeSet = aComposedNode->edGetDirectDescendants();
    set<Node*>::const_iterator aChildIter = aChildNodeSet.begin();
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
      anInPortItem = new YACSGui_PortViewItem( theNodeItem, anInPortItem, *InPortsIter );
  }
  else if ( ForLoop* aForLoopNode = dynamic_cast<ForLoop*>( aNode ) ) {
    InputPort* anInputPort = aForLoopNode->edGetNbOfTimesInputPort();
    if (anInputPort)
      anInPortItem = new YACSGui_PortViewItem( theNodeItem, aSubNodeItem, anInputPort );
  }
  else if ( Switch* aSwitchNode = dynamic_cast<Switch*>( aNode ) ) {
    InputPort* anInputPort = aSwitchNode->edGetConditionPort();
    if (anInputPort)
      anInPortItem = new YACSGui_PortViewItem( theNodeItem, aSubNodeItem, anInputPort );
  }
  else if ( WhileLoop* aWhileLoopNode = dynamic_cast<WhileLoop*>( aNode ) ) {
    InputPort* anInputPort = aWhileLoopNode->edGetConditionPort();
    if (anInputPort)
      anInPortItem = new YACSGui_PortViewItem( theNodeItem, aSubNodeItem, anInputPort );
  }
  else if ( ForEachLoop* aForEachLoopNode = dynamic_cast<ForEachLoop*>( aNode ) ) {
    InputPort* anInputPort1 = aForEachLoopNode->edGetNbOfBranchesPort();
    if (anInputPort1)
      anInPortItem = new YACSGui_PortViewItem( theNodeItem, aSubNodeItem, anInputPort1 );
    InputPort* anInputPort2 = aForEachLoopNode->edGetSeqOfSamplesPort();
    if (anInputPort2)
      anInPortItem = new YACSGui_PortViewItem( theNodeItem, anInPortItem, anInputPort2 );
  }
  
  InGate* anInGate = aNode->getInGate();
  if (anInGate)
    anInPortItem = new YACSGui_PortViewItem( theNodeItem, anInPortItem, anInGate );
  
  // output ports
  
  QListViewItem* anOutPortItem = anInPortItem;
  if (!aComposedNode) {
    list<OutPort *> anOutPorts = aNode->getSetOfOutPort();
          
    list<OutPort *>::iterator OutPortsIter = anOutPorts.begin();
    for(; OutPortsIter != anOutPorts.end(); OutPortsIter++)
      anOutPortItem = new YACSGui_PortViewItem( theNodeItem, anOutPortItem, *OutPortsIter );
  }
  else if ( ForEachLoop* aForEachLoopNode = dynamic_cast<ForEachLoop*>( aNode ) ) {
    OutputPort* anOutputPort = aForEachLoopNode->edGetSamplePort();
    if (anOutputPort)
      anOutPortItem = new YACSGui_PortViewItem( theNodeItem, anInPortItem, anOutputPort );
  }
    
  OutGate* anOutGate = aNode->getOutGate();
  if (anOutGate)
    anOutPortItem = new YACSGui_PortViewItem( theNodeItem, anOutPortItem, anOutGate );
  
  if (aComposedNode)
  {
    set< pair< OutGate*,InGate* > > anInternalGateLinks;
    set< pair< OutPort*,InPort* > > anInternalLinks;
    getAllInternalLinks( aComposedNode, anInternalGateLinks, anInternalLinks );

    // Create internal links

    // Create "Links" label
    YACSGui_LabelViewItem* aLinksItem = new YACSGui_LabelViewItem( theNodeItem, anOutPortItem, "Links" );

    // Control links
    QListViewItem* aControlLinkItem = 0;
    set< pair< OutGate*,InGate* > >::iterator aGateLinksIter = anInternalGateLinks.begin();
    for ( ; aGateLinksIter != anInternalGateLinks.end(); aGateLinksIter++ )
      aControlLinkItem = new YACSGui_LinkViewItem( aLinksItem, aControlLinkItem,
						   (*aGateLinksIter).first, (*aGateLinksIter).second );
    
    // Process out ports list of the node : Data links
    QListViewItem* aDataLinkItem = aControlLinkItem;
    set< pair< OutPort*,InPort* > >::iterator aLinksIter = anInternalLinks.begin();
    for ( ; aLinksIter != anInternalLinks.end(); aLinksIter++ ) {
      OutPort* aCurOutPort = (*aLinksIter).first;
      string aClassName = aCurOutPort->getNameOfTypeOfCurrentInstance();
      
      InPort* aCurInPort = (*aLinksIter).second;
	
      // Data links
      if ( !aClassName.compare(OutputPort::NAME) ) {
	OutputPort* anOutputDFPort = dynamic_cast<OutputPort*>( aCurOutPort );
	if ( anOutputDFPort )
	  aDataLinkItem = new YACSGui_LinkViewItem( aLinksItem, aDataLinkItem, anOutputDFPort, aCurInPort);
      }
    } 

    // Process out ports list of the node : Stream links
    QListViewItem* aStreamLinkItem = aDataLinkItem;
    aLinksIter = anInternalLinks.begin();
    for ( ; aLinksIter != anInternalLinks.end(); aLinksIter++ ) {
      OutPort* aCurOutPort = (*aLinksIter).first;
      string aClassName = aCurOutPort->getNameOfTypeOfCurrentInstance();
      
      InPort* aCurInPort = (*aLinksIter).second;
      
      // Stream links
      if ( !aClassName.compare(OutputDataStreamPort::NAME) || !aClassName.compare(OutputCalStreamPort::NAME) ) {
	OutputDataStreamPort* anOutputDSPort = dynamic_cast<OutputDataStreamPort*>( aCurOutPort );
	if ( anOutputDSPort ) 
	  aStreamLinkItem = new YACSGui_LinkViewItem( aLinksItem, aStreamLinkItem, anOutputDSPort, aCurInPort);
      }
    }
  }

}

/*!
  Method returns a subject of the selected tree viewitem if the selection is a single selection or 0, otherwise
*/
YACS::HMI::Subject* YACSGui_EditionTreeView::getSelectedSubject()
{
  Subject* aSub = 0;

  std::list<QListViewItem*> aSelList = getSelected();
  QListViewItem* anItem = 0;
  
  // check if the current selection is a single selection
  if ( aSelList.size() == 1 ) anItem = aSelList.front();
   
  if ( YACSGui_ReferenceViewItem* aRefItem = dynamic_cast<YACSGui_ReferenceViewItem*>( anItem ) )
    aSub = aRefItem->getSReference();
  else if ( YACSGui_NodeViewItem* aNodeItem = dynamic_cast<YACSGui_NodeViewItem*>( anItem ) )
    aSub = aNodeItem->getSNode();
  else if ( YACSGui_SchemaViewItem* aSchemaItem = dynamic_cast<YACSGui_SchemaViewItem*>( anItem ) )
    aSub = aSchemaItem->getSProc();
  else if ( YACSGui_ContainerViewItem* aContItem = dynamic_cast<YACSGui_ContainerViewItem*>( anItem ) )
    aSub = aContItem->getSContainer();
  else if ( YACSGui_ComponentViewItem* aCompItem = dynamic_cast<YACSGui_ComponentViewItem*>( anItem ) )
    aSub = aCompItem->getSComponent();

  return aSub;
}

//! Public slot.  Creates data type for the schema.
/*!
 */
void YACSGui_EditionTreeView::onCreateDataType()
{
  
}

//! Public slot.  Adds the selected node into the library.
/*!
 */
void YACSGui_EditionTreeView::onAddToLibrary()
{

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
  printf("===>> YACSGui_EditionTreeView::onContextMenuRequested\n");
  if ( dynamic_cast<YACSGui_SchemaViewItem*>( theItem ) )
  {
    showPopup( contextMenuPopup( YACSGui_TreeView::SchemaItem ), thePoint );
  }
  else if ( YACSGui_NodeViewItem* aNIt = dynamic_cast<YACSGui_NodeViewItem*>( theItem ) )
  {
    if ( dynamic_cast<ServiceNode*>( aNIt->getNode() ) )
      showPopup( contextMenuPopup( YACSGui_EditionTreeView::ServiceNodeItem ), thePoint );
    else if ( dynamic_cast<Loop*>( aNIt->getNode() ) || dynamic_cast<ForEachLoop*>( aNIt->getNode() ) )
      showPopup( contextMenuPopup( YACSGui_EditionTreeView::LoopNodeItem ), thePoint );
    else
      showPopup( contextMenuPopup( YACSGui_TreeView::NodeItem ), thePoint );
  }
  else if ( dynamic_cast<YACSGui_PortViewItem*>( theItem ) )
  {
    showPopup( contextMenuPopup( YACSGui_EditionTreeView::PortItem ), thePoint );
  }
  else if ( dynamic_cast<YACSGui_LinkViewItem*>( theItem ) )
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

//! Builds the tree view according to the schema content. Schema is used as the root object.
/*!
 */
void YACSGui_EditionTreeView::build()
{
  if ( !getProc() ) return;

  // Create a schema root object
  YACSGui_SchemaViewItem* aSchemaItem = new YACSGui_SchemaViewItem( this, 0, mySProc );
   
  // Create "Data Types" label
  YACSGui_LabelViewItem* aDataTypesItem = new YACSGui_LabelViewItem( aSchemaItem, 0, "Data Types" );

  // Create "Simple" label
  YACSGui_LabelViewItem* aSimpleItem = new YACSGui_LabelViewItem( aDataTypesItem, 0, "Simple" );

  // Create "Double" data type view item

  // Create "Int" data type view item

  // Create "String" data type view item

  // Create "Bool" data type view item

  // Create "Objref" label
  YACSGui_LabelViewItem* anObjrefItem = new YACSGui_LabelViewItem( aDataTypesItem, aSimpleItem, "Objref" );

  // Create "Sequence" label
  YACSGui_LabelViewItem* aSequenceItem = new YACSGui_LabelViewItem( aDataTypesItem, anObjrefItem, "Sequence" );

  // Create "Array" label
  YACSGui_LabelViewItem* anArrayItem = new YACSGui_LabelViewItem( aDataTypesItem, aSequenceItem, "Array" );

  // Create "Struct" label
  YACSGui_LabelViewItem* aStructItem = new YACSGui_LabelViewItem( aDataTypesItem, anArrayItem, "Struct" );

  // Create "Nodes" label if necessary
  YACSGui_LabelViewItem* aNodesItem = 0;
  set<Node*> aDirectNodeSet = getProc()->edGetDirectDescendants();
  //if (aDirectNodeSet.size() > 0)
  aNodesItem = new YACSGui_LabelViewItem( aSchemaItem, aDataTypesItem, "Nodes" );
  
  // Put all nodes under "Nodes" label
  if ( aNodesItem )
  {
    YACSGui_NodeViewItem* aNodeItem = 0;
    for ( set<Node*>::iterator it = aDirectNodeSet.begin(); it != aDirectNodeSet.end(); it++ )
      aNodeItem = displayNodeWithPorts( aNodesItem, aNodeItem, mySProc->getChild(*it) );
  }
  
  // Create "Links" label
  YACSGui_LabelViewItem* aLinksItem = new YACSGui_LabelViewItem( aSchemaItem, 
								 aNodesItem ? aNodesItem : aDataTypesItem,
								 "Links" );
  
  // Put all links under "Links" label
  set<Node*> aNodeSet = getProc()->getAllRecursiveConstituents();
  for ( set<Node*>::iterator it = aNodeSet.begin(); it != aNodeSet.end(); it++ ) {
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
	  aLinkItem = new YACSGui_LinkViewItem( aLinksItem, aLinkItem, anOutGate, aCurInGate);
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
	    if ( anInternalLinks.find( make_pair< OutPort*,InPort* >(anOutputDFPort, aCurInPort) ) == anInternalLinks.end() )
	      aDataLinkItem = new YACSGui_LinkViewItem( aLinksItem, aDataLinkItem, anOutputDFPort, aCurInPort);
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
	    if ( anInternalLinks.find( make_pair< OutPort*,InPort* >(anOutputDSPort, aCurInPort) ) == anInternalLinks.end() ) 
	      aStreamLinkItem = new YACSGui_LinkViewItem( aLinksItem, aStreamLinkItem, anOutputDSPort, aCurInPort);
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
      for ( Component2ServiceNodesMap::iterator itComp = aComponent2ServiceNodes.begin();
	    itComp != aComponent2ServiceNodes.end(); itComp++ )
      {
	SubjectComponent* aSComp = GuiContext::getCurrent()->_mapOfSubjectComponent[(*itComp).first];
	aComponentItem = new YACSGui_ComponentViewItem( aContainerItem, aComponentItem, aSComp );

	YACSGui_ReferenceViewItem* aRefNodeItem = 0;
	list<SubjectServiceNode*> aServiceNodes = (*itComp).second;
	for ( list<SubjectServiceNode*>::iterator itSNode = aServiceNodes.begin();
	      itSNode != aServiceNodes.end(); itSNode++ )
	  // Put a reference to the schemas node, which is used this component instance
	  aRefNodeItem = new YACSGui_ReferenceViewItem( aComponentItem, aRefNodeItem, (*itSNode)->getSubjectReference(aSComp) );
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
	aRefNodeItem = new YACSGui_ReferenceViewItem( aCComponentItem, aRefNodeItem, (*itSNode)->getSubjectReference(aSComp) );
    }
  }
}

void YACSGui_EditionTreeView::fillContainerData( YACS::HMI::SubjectComposedNode* theSNode )
{
  if ( theSNode )
    if ( ComposedNode* aNode = dynamic_cast<ComposedNode*>(theSNode->getNode()) ) //if ( getProc() )
    { // find containers and components, which are used by Proc* service nodes
      set<Node*> aNodeSet = aNode->edGetDirectDescendants(); //getProc()->getAllRecursiveConstituents();
      for ( set<Node*>::iterator it = aNodeSet.begin(); it != aNodeSet.end(); it++ )
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
		  cout<<">> CORBA ComponentInstance = "<<aComponent<<endl;
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

QPopupMenu* YACSGui_EditionTreeView::contextMenuPopup( const int theType )
{
  QPopupMenu* aPopup = new QPopupMenu( this );

  switch ( theType )
  {
  case YACSGui_TreeView::SchemaItem:
    {
      aPopup->insertItem( tr("POP_EXPORT"), myModule, SLOT(onExportSchema()) );
      aPopup->insertSeparator();
      
      aPopup->insertItem( tr("POP_CREATE_DATA_TYPE"), this, SLOT(onCreateDataType()) );
      aPopup->insertItem( tr("POP_CREATE_CONTAINER_DEF"), myModule, SLOT(onNewContainer()) );
      aPopup->insertSeparator();
      
      QPopupMenu* aCrNPopup = new QPopupMenu( this );
      aCrNPopup->insertItem( tr("POP_INLINE_SCRIPT"), myModule, SLOT(onInlineScriptNode()) );
      aCrNPopup->insertItem( tr("POP_INLINE_FUNCTION"), myModule, SLOT(onInlineFunctionNode()) );
      aCrNPopup->insertItem( tr("POP_BLOCK"), myModule, SLOT(onBlockNode()) );
      aCrNPopup->insertItem( tr("POP_FOR_LOOP"), myModule, SLOT(onFORNode()) );
      aCrNPopup->insertItem( tr("POP_FOREACH_LOOP"), myModule, SLOT(onFOREACHNode()) );
      aCrNPopup->insertItem( tr("POP_WHILE_LOOP"), myModule, SLOT(onWHILENode()) );
      aCrNPopup->insertItem( tr("POP_SWITCH"), myModule, SLOT(onSWITCHNode()) );
      aCrNPopup->insertItem( tr("POP_FROM_LIBRARY"), myModule, SLOT(onNodeFromLibrary()) );
      
      aPopup->insertItem(tr("POP_CREATE_NODE"), aCrNPopup);
      aPopup->insertSeparator();
      
      aPopup->insertItem( tr("POP_EXECUTE"), myModule, SLOT(onCreateExecution(/*bool theIsFromTreeView*/)) );
      // TODO: if theIsFromTreeView = true, search SObject in Object Browser by name of the selected schema item in the tree view
      aPopup->insertSeparator();
      
      aPopup->insertItem( tr("POP_COPY"), this, SLOT(onCopyItem()) );
      aPopup->insertItem( tr("POP_PASTE"), this, SLOT(onPasteItem()) );
      aPopup->insertItem( tr("POP_DELETE"), this, SLOT(onDeleteItem()) );
      aPopup->insertSeparator();
      
      aPopup->insertItem( tr("POP_DISPLAY"), this, SLOT(onDisplaySchema()) );
      aPopup->insertItem( tr("POP_ERASE"), this, SLOT(onEraseSchema()) );
    }
    break;
  case YACSGui_EditionTreeView::ContainerItem:
    {    
      QPopupMenu* aCrCPopup = new QPopupMenu( this );
      aCrCPopup->insertItem( tr("POP_SALOME_COMPONENT"), myModule, SLOT(onNewSalomeComponent()) );
      aCrCPopup->insertItem( tr("POP_PYTHON_COMPONENT"), myModule, SLOT(onNewSalomePythonComponent()) );
      
      aPopup->insertItem(tr("POP_CREATE_COMPONENT"), aCrCPopup);
      aPopup->insertSeparator();
      
      aPopup->insertItem( tr("POP_COPY"), this, SLOT(onCopyItem()) );
      aPopup->insertItem( tr("POP_PASTE"), this, SLOT(onPasteItem()) ); 
      aPopup->insertItem( tr("POP_DELETE"), this, SLOT(onDeleteItem()) );
    }
    break;
  case YACSGui_EditionTreeView::ComponentItem:
    {
      aPopup->insertItem( tr("POP_CREATE_SALOME_SERVICE_NODE"), myModule, SLOT(onSalomeServiceNode()) );
      aPopup->insertItem( tr("POP_CREATE_SERVICE_INLINE_NODE"), myModule, SLOT(onServiceInlineNode()) );
      aPopup->insertSeparator();
      
      aPopup->insertItem( tr("POP_COPY"), this, SLOT(onCopyItem()) );
      aPopup->insertItem( tr("POP_DELETE"), this, SLOT(onDeleteItem()) );
    }
    break;
  case YACSGui_EditionTreeView::CorbaComponentItem:
    {
      aPopup->insertItem( tr("POP_CREATE_CORBA_SERVICE_NODE"), myModule, SLOT(onCorbaServiceNode()) );
      aPopup->insertSeparator();
      
      aPopup->insertItem( tr("POP_COPY"), this, SLOT(onCopyItem()) );
      aPopup->insertItem( tr("POP_DELETE"), this, SLOT(onDeleteItem()) );
    }
    break;
  case YACSGui_TreeView::NodeItem:
  case YACSGui_EditionTreeView::ServiceNodeItem:
  case YACSGui_EditionTreeView::LoopNodeItem:
    {
      if ( theType == YACSGui_EditionTreeView::ServiceNodeItem )
      {
	aPopup->insertItem( tr("POP_CREATE_NODE-NODE_SERVICE_NODE"), myModule, SLOT(onNodeNodeServiceNode()) );
	aPopup->insertSeparator();
      }
      
      if ( theType == YACSGui_EditionTreeView::LoopNodeItem )
      {
	QPopupMenu* aCrNPopup = new QPopupMenu( this );
	// TODO: in slots we have to check what is selected: schema,
	//                                                   loop node (=> from ¨Create a body¨) or
	//                                                   another node (=> from ¨Create a loop¨).
	aCrNPopup->insertItem( tr("POP_INLINE_SCRIPT"), myModule, SLOT(onInlineScriptNode()) );
	aCrNPopup->insertItem( tr("POP_INLINE_FUNCTION"), myModule, SLOT(onInlineFunctionNode()) );
	aCrNPopup->insertItem( tr("POP_BLOCK"), myModule, SLOT(onBlockNode()) );
	aCrNPopup->insertItem( tr("POP_FOR_LOOP"), myModule, SLOT(onFORNode()) );
	aCrNPopup->insertItem( tr("POP_FOREACH_LOOP"), myModule, SLOT(onFOREACHNode()) );
	aCrNPopup->insertItem( tr("POP_WHILE_LOOP"), myModule, SLOT(onWHILENode()) );
	aCrNPopup->insertItem( tr("POP_SWITCH"), myModule, SLOT(onSWITCHNode()) );
	aCrNPopup->insertItem( tr("POP_FROM_LIBRARY"), myModule, SLOT(onNodeFromLibrary()) );
	
	aPopup->insertItem(tr("POP_CREATE_BODY"), aCrNPopup);
	aPopup->insertSeparator();
      }
      else
      {
	QPopupMenu* aCrNPopup = new QPopupMenu( this );
	aCrNPopup->insertItem( tr("POP_FOR_LOOP"), myModule, SLOT(onFORNode()) );
	aCrNPopup->insertItem( tr("POP_FOREACH_LOOP"), myModule, SLOT(onFOREACHNode()) );
	aCrNPopup->insertItem( tr("POP_WHILE_LOOP"), myModule, SLOT(onWHILENode()) );
	
	aPopup->insertItem(tr("POP_CREATE_LOOP"), aCrNPopup);
	aPopup->insertSeparator();
      }
      
      aPopup->insertItem( tr("POP_ADD_TO_LIBRARY"), this, SLOT(onAddToLibrary()) );
      aPopup->insertSeparator();
      
      aPopup->insertItem( tr("POP_COPY"), this, SLOT(onCopyItem()) );
      aPopup->insertItem( tr("POP_DELETE"), this, SLOT(onDeleteItem()) );
      aPopup->insertSeparator();
      
      aPopup->insertItem( tr("POP_DISPLAY"), this, SLOT(onDisplayNode()) );
      aPopup->insertItem( tr("POP_ERASE"), this, SLOT(onEraseNode()) );
    }
    break;
  case YACSGui_EditionTreeView::PortItem:
    aPopup->insertItem( tr("POP_DELETE"), this, SLOT(onDeleteItem()) );
    break;
  case YACSGui_EditionTreeView::LinkItem:
    aPopup->insertItem( tr("POP_DELETE"), this, SLOT(onDeleteItem()) );
    break;
  default:
    break;
  }

  return aPopup;
}

void YACSGui_EditionTreeView::showPopup( QPopupMenu* thePopup, const QPoint thePos )
{
  thePopup->exec(thePos);
}

void YACSGui_EditionTreeView::syncPageTypeWithSelection()
{
  if ( !getProc() || !myModule ) return;

  YACSGui_InputPanel* anIP = myModule->getInputPanel();
  if ( !anIP ) return;

  // get the list of selected items
  list<QListViewItem*> aSelList = getSelected();
  
  QListViewItem* anItem = 0;
  
  // check if the current selection is a single selection
  if ( aSelList.size() == 1 ) anItem = aSelList.front();
    
  if ( YACSGui_SchemaViewItem* aSchemaItem = dynamic_cast<YACSGui_SchemaViewItem*>( anItem ) )
  { // this is a schema view item
    // find or create a property page for a schema
    printf(">> Show Input Panel for a given schema (Proc*)\n");

    YACSGui_SchemaPage* aSPage = dynamic_cast<YACSGui_SchemaPage*>( anIP->getPage( YACSGui_InputPanel::SchemaId ) );
    if ( aSPage )
    {
      aSPage->setSProc( aSchemaItem->getSProc() );
      anIP->setOn( true, YACSGui_InputPanel::SchemaId );
      anIP->setMode( YACSGui_InputPanel::EditMode, YACSGui_InputPanel::SchemaId );
      anIP->setExclusiveVisible( true,  list<int>(1,YACSGui_InputPanel::SchemaId) );
      anIP->show();
    }
  }
  else if ( YACSGui_NodeViewItem* aNodeItem = dynamic_cast<YACSGui_NodeViewItem*>( anItem ) )
  { // this is a node data object
    // the following method can be used if its needed:
    // YACSPrs_ElementaryNode* aNode = getGraph( dynamic_cast<Proc*>(aNodeItem->getNode()->getRootNode()) )->getItem( aNodeItem->getNode() );
    printf(">> Show Input Panel for a given node (Node*)\n");

    Node* aNode = aNodeItem->getNode();
    
    if ( ServiceNode* sNode = dynamic_cast<ServiceNode*>( aNode ) )
    {
      if ( sNode->getKind() == "XML" )
      {
	printf(">>XML\n");
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
	printf(">>Service\n");
	YACSGui_ServiceNodePage* aSNPage = 
	  dynamic_cast<YACSGui_ServiceNodePage*>( anIP->getPage( YACSGui_InputPanel::ServiceNodeId ) );
	if ( aSNPage )
	{
	  list<int> aPagesIds;

	  YACSGui_ContainerPage* aContPage = 
	    dynamic_cast<YACSGui_ContainerPage*>( anIP->getPage( YACSGui_InputPanel::ContainerId ) );
	  if ( aContPage )
	  {
	    aContPage->setSContainer( GuiContext::getCurrent()->_mapOfSubjectContainer[sNode->getComponent()->getContainer()] );
	    anIP->setOn( false, YACSGui_InputPanel::ContainerId );
	    aPagesIds.push_back(YACSGui_InputPanel::ContainerId);
	  }
	    
	  YACSGui_ComponentPage* aCompPage = 
	    dynamic_cast<YACSGui_ComponentPage*>( anIP->getPage( YACSGui_InputPanel::ComponentId ) );
	  if ( aCompPage )
	  {
	    aCompPage->setSComponent( GuiContext::getCurrent()->_mapOfSubjectComponent[sNode->getComponent()] );
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
    else if ( dynamic_cast<InlineNode*>( aNode ) )
    {
      printf(">>Inline\n");
      YACSGui_InlineNodePage* aINPage = 
	dynamic_cast<YACSGui_InlineNodePage*>( anIP->getPage( YACSGui_InputPanel::InlineNodeId ) );
      if ( aINPage )
      {
	aINPage->setSNode( aNodeItem->getSNode() );
	anIP->setOn( true, YACSGui_InputPanel::InlineNodeId );
	anIP->setMode( YACSGui_InputPanel::EditMode, YACSGui_InputPanel::InlineNodeId );
	anIP->setExclusiveVisible( true,  list<int>(1,YACSGui_InputPanel::InlineNodeId) );
	anIP->show();
      }
    }
    else if ( dynamic_cast<Switch*>( aNode ) )
    {
      printf(">>Switch\n");
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
      printf(">>For\n");
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
      printf(">>ForEach\n");
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
      printf(">>While\n");
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
      printf(">>Bloc\n");
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
    printf(">> Show Input Panel for a given link (<Port*,Port*>)\n");

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
    printf(">> Show Input Panel for a given container (SalomeContainer*>)\n");

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
    printf(">> Show Input Panel for a given component (ComponentInstance*>)\n");

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
  printf("===>> YACSGui_EditionTreeView::onSelectionChanged\n");
  
  if ( !getProc() || !myModule ) return;

  YACSGui_InputPanel* anIP = myModule->getInputPanel();
  if ( !anIP ) return;

  // get the list of selected items
  list<QListViewItem*> aSelList = getSelected();
  
  QListViewItem* anItem = 0;
  
  // check if the current selection is a single selection
  if ( aSelList.size() == 1 ) anItem = aSelList.front();

  // check if the component instance selection is active
  if( ((QWidget*)anIP)->isVisible() && anIP->isVisible( YACSGui_InputPanel::ServiceNodeId ) )
  {
    YACSGui_ServiceNodePage* aSNPage = dynamic_cast<YACSGui_ServiceNodePage*>( anIP->getPage( YACSGui_InputPanel::ServiceNodeId ) );
    if ( aSNPage && aSNPage->isSelectComponent() )
    {
      if ( YACSGui_ComponentViewItem* aCompItem = dynamic_cast<YACSGui_ComponentViewItem*>( anItem ) )
      {
	printf(">> Set component instance for Service Node page (%s)\n", aCompItem->getComponent()->getName().c_str());
	aSNPage->setComponent(aCompItem->getComponent());
	printf(">> Set component instance for Service Node page (%s)\n", aCompItem->getComponent()->getName().c_str());

	// change selection from the component instance to the service node
	bool block = signalsBlocked();
	blockSignals( true );
	setSelected( aCompItem, false );
	QListViewItem* aSNItem = findItem( aSNPage->getNodeName(), 0 );
	if( aSNItem )
	  setSelected( aSNItem, true );
	blockSignals( block );
      }
      else
      { // remove selection from incompatible object (which is not component instance)
	setSelected( anItem, false );
      }

      return;
    }
  }

  // check if the case selection of switch node is active
  if( ((QWidget*)anIP)->isVisible() && anIP->isVisible( YACSGui_InputPanel::SwitchNodeId ) )
  {
    YACSGui_SwitchNodePage* aSNPage = dynamic_cast<YACSGui_SwitchNodePage*>( anIP->getPage( YACSGui_InputPanel::SwitchNodeId ) );
    printf(">> aSNPage->isSelectChild() = %d\n",aSNPage->isSelectChild());
    if ( aSNPage && aSNPage->isSelectChild() )
    {
      if ( YACSGui_NodeViewItem* aNodeItem = dynamic_cast<YACSGui_NodeViewItem*>( anItem ) )
      {
	printf(">> Set child node for Switch Node page\n");
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
    YACSGui_BlockNodePage* aBNPage = dynamic_cast<YACSGui_BlockNodePage*>( anIP->getPage( YACSGui_InputPanel::BlockNodeId ) );
    printf(">> aBNPage->isSelectChild() = %d\n",aBNPage->isSelectChild());
    if ( aBNPage && aBNPage->isSelectChild() )
    {
      if ( YACSGui_NodeViewItem* aNodeItem = dynamic_cast<YACSGui_NodeViewItem*>( anItem ) )
      {
	printf(">> Set child node for Block Node page\n");
	if ( aBNPage->getNode() != aNodeItem->getNode() )
	{
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

  syncPageTypeWithSelection();
    
  //else
  //{
  //  anIP->hide();

    // check if the current selection contains two ports => create link operation had to be added in the popup
    /*if ( aSelList.size() == 2 )
    {
      anItem = aSelList.front();
      QListViewItem* anItem2 = aSelList.back();
    }*/

  //}
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
	emit selectionChanged();
	
	break;
      }
    }
  }
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

    addColumn( "Nodes" );
    addColumn( "State" );

    build();

    connect( this, SIGNAL( clicked( QListViewItem* ) ), this, SLOT( onBreakpointClicked( QListViewItem* ) ) );
    connect( this, SIGNAL( selectionChanged() ), this, SLOT( onSelectionChanged() ) );
  }
}

//! Destructor.
/*!
 */
YACSGui_RunTreeView::~YACSGui_RunTreeView()
{
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
  set<Node*> aSetOfNodes = theFather->edGetDirectDescendants();
  for ( set<Node*>::iterator iter = aSetOfNodes.begin(); iter != aSetOfNodes.end(); iter++)
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
  printf("===>> YACSGui_RunTreeView::onSelectionChanged\n");
  syncPageTypeWithSelection();
}

void YACSGui_RunTreeView::onBreakpointClicked( QListViewItem* theItem )
{
  printf("YACSGui_RunTreeView::onBreakpointClicked");
  YACSGui_ElementaryNodeViewItem* anElemNodeItem = dynamic_cast<YACSGui_ElementaryNodeViewItem*>(theItem);
  if ( anElemNodeItem )
  {
    printf("=> click on node ",anElemNodeItem->getNode()->getQualifiedName().c_str());

    YACSGui_Executor* anExecutor = myModule->findExecutor();
    if ( !anExecutor ) return;

    if ( anElemNodeItem->isOn() )
    {
      if ( anExecutor->getCurrentExecMode() == YACSGui_ORB::CONTINUE )
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
      printf("=> nodeName = %s\n",aNodeName.c_str());
      aBreakpointList.push_back(aNodeName);
    }
    anExecutor->setBreakpointList(aBreakpointList);
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
    aRootItem->setStatus(theStatus);
}
