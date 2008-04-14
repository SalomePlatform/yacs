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

#include <Python.h>
#include <PythonNode.hxx>

#include <SALOME_ResourcesManager.hxx>
#include <SALOME_LifeCycleCORBA.hxx>

#include <YACSGui_InputPanel.h>
#include <YACSGui_Module.h>
#include <YACSGui_PlusMinusGrp.h>
#include <YACSGui_Table.h>
#include <YACSGui_Graph.h>
#include <YACSGui_Executor.h>
#include <YACSGui_TreeView.h>
#include <YACSGui_TreeViewItem.h>
#include <YACSGui_LogViewer.h>

#include <YACSPrs_ElementaryNode.h>

#include <QxGraph_Canvas.h>

#include <LightApp_Application.h>
#include <CAM_Application.h>
#include <SUIT_Session.h>
#include <SUIT_Application.h>
#include <SUIT_MessageBox.h>
#include <LogWindow.h>

#include <InPort.hxx>
#include <OutPort.hxx>
#include <InputPort.hxx>
#include <OutputPort.hxx>
#include <CalStreamPort.hxx>
#include <ConditionInputPort.hxx>
#include <ForLoop.hxx>
#include <ForEachLoop.hxx>
#include <WhileLoop.hxx>
#include <Switch.hxx>
#include <CORBAPorts.hxx>
#include <PythonPorts.hxx>
#include <XMLPorts.hxx>
#include <InlineNode.hxx>
#include <ServiceNode.hxx>
#include <ServiceInlineNode.hxx>
#include <CORBANode.hxx>
#include <SalomePythonNode.hxx>
#include <CppNode.hxx>
#include <XMLNode.hxx>
#include <Exception.hxx>
#include <Catalog.hxx>
#include <ComponentDefinition.hxx>
#include <CORBAComponent.hxx>
#include <CppComponent.hxx>
#include <SalomeComponent.hxx>
#include <SalomePythonComponent.hxx>
#include <RuntimeSALOME.hxx>

#include <qwindowsstyle.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qcombobox.h>
#include <qtextedit.h>
#include <qtoolbutton.h>
#include <qspinbox.h>
#include <qprogressbar.h>
#include <qobjectlist.h>
#include <qfiledialog.h>
#include <qcheckbox.h>

#include <sstream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

#define SPACING 5
#define MARGIN 5

using namespace YACS;
using namespace YACS::ENGINE;
using namespace YACS::HMI;
using namespace std;

/*
  Class : YACSGui_BlockNodePage
  Description :  Page for block node properties
*/

YACSGui_BlockNodePage::YACSGui_BlockNodePage( QWidget* theParent, const char* theName, WFlags theFlags )
  : BlockNodePage( theParent, theName, theFlags ),
    YACSGui_NodePage(),
    myIsSelectChild(false)
{
  if ( myDirectChildrenGroupBox )
  {
    myDirectChildrenGroupBox->setTitle( tr("Direct Children") );
    myDirectChildrenGroupBox->HideBtn( YACSGui_PlusMinusGrp::InsertBtn |
				       YACSGui_PlusMinusGrp::UpBtn |
				       YACSGui_PlusMinusGrp::DownBtn );
    myDirectChildrenGroupBox->EnableBtn( YACSGui_PlusMinusGrp::SelectBtn, false );
    YACSGui_Table* aTable = myDirectChildrenGroupBox->Table();
    aTable->setFixedHeight( 100 );
    aTable->setNumCols( 1 );
    aTable->horizontalHeader()->setLabel( 0, tr( "Node name" ) );
    aTable->setCellType( -1, 0, YACSGui_Table::String );

    myDirectChildrenGroupBox->EnableBtn( YACSGui_PlusMinusGrp::AllBtn );
    myDirectChildrenGroupBox->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );

    myDirectChildrenGroupBox->Btn(YACSGui_PlusMinusGrp::SelectBtn)->setTextLabel(tr("SELECT_BTN_TOOLTIP"),true);
    myDirectChildrenGroupBox->Btn(YACSGui_PlusMinusGrp::PlusBtn)->setTextLabel(tr("PLUS_BTN_TOOLTIP"),true);
    myDirectChildrenGroupBox->Btn(YACSGui_PlusMinusGrp::MinusBtn)->setTextLabel(tr("MINUS_BTN_TOOLTIP"),true);

    connect( myNodeName, SIGNAL(textChanged( const QString& )), this, SLOT(onNodeNameChanged( const QString& )) );
    connect( myDirectChildrenGroupBox, SIGNAL(initSelection( const int, const int )),
	     this,                     SLOT(onInitSelection( const int, const int )) );
    connect( myDirectChildrenGroupBox->Table(), SIGNAL(itemCreated( const int, const int )),
	     this,                              SLOT(onNewItem( const int, const int )) );
    
    connect( myDirectChildrenGroupBox, SIGNAL(Added( const int )),     this, SLOT(onAdded( const int )) );
    connect( myDirectChildrenGroupBox, SIGNAL(Inserted( const int )),  this, SLOT(onInserted( const int )) );
    connect( myDirectChildrenGroupBox, SIGNAL(MovedUp( const int )),   this, SLOT(onMovedUp( const int )) );
    connect( myDirectChildrenGroupBox, SIGNAL(MovedDown( const int )), this, SLOT(onMovedDown( const int )) );
    connect( myDirectChildrenGroupBox, SIGNAL(Removed( const int )),   this, SLOT(onRemoved( const int )) );
  }

  // TODO: implement presentation of the block node in collapsed mode
  // temporary solution :
  myCollapsedRadioButton->setEnabled(false );
}

YACSGui_BlockNodePage::~YACSGui_BlockNodePage()
{
  if (getInputPanel()) getInputPanel()->removePage(this);

  myRow2ChildMap.clear();
  myRowsOfSelectedChildren.clear();
}

void YACSGui_BlockNodePage::setSNode( YACS::HMI::SubjectNode* theSNode )
{
  if ( !theSNode ) return;

  if ( dynamic_cast<Bloc*>( theSNode->getNode() ) )
    YACSGui_NodePage::setSNode( theSNode );
}

void YACSGui_BlockNodePage::setChild( YACS::HMI::SubjectNode* theChildNode )
{
  if ( !theChildNode ) return;

  if ( !myDirectChildrenGroupBox ) return;
  YACSGui_Table* aTable = myDirectChildrenGroupBox->Table();

  myIsSelectChild = false;
  myDirectChildrenGroupBox->setOffSelect();
  myDirectChildrenGroupBox->EnableBtn( YACSGui_PlusMinusGrp::SelectBtn, false );

  // sets the child node name into tables cell
  QStringList aChildNames;
  aTable->strings( 0, aChildNames );
  aChildNames[aTable->currentRow()] = theChildNode->getNode()->getName();
  aTable->setStrings( 0, aChildNames );

  // add theChildNode into the map of children of this block node
  if ( myRow2ChildMap.find(aTable->currentRow()) == myRow2ChildMap.end() )
    myRow2ChildMap.insert( std::make_pair( aTable->currentRow(), theChildNode ) );
  else
    myRow2ChildMap[aTable->currentRow()] = theChildNode;

  bool anExist = false;
  std::list<int>::iterator it = myRowsOfSelectedChildren.begin();
  for ( ; it != myRowsOfSelectedChildren.end(); it++ )
    if ( *it == aTable->currentRow() )
    {
      anExist = true;
      break;
    }
  if ( !anExist )
    myRowsOfSelectedChildren.push_back( aTable->currentRow() );
}

bool YACSGui_BlockNodePage::isSelectChild() const
{
  return myIsSelectChild;
}

void YACSGui_BlockNodePage::setMode( const YACSGui_InputPanel::PageMode theMode )
{
  YACSGui_NodePage::setMode(theMode);

  if ( myMode == YACSGui_InputPanel::EditMode )
  {
    myNodeName->setReadOnly(false);
    
    myDirectChildrenGroupBox->ShowBtn( YACSGui_PlusMinusGrp::SelectBtn |
				       YACSGui_PlusMinusGrp::PlusBtn |
				       YACSGui_PlusMinusGrp::MinusBtn );
    YACSGui_Table* aTable = myDirectChildrenGroupBox->Table();
    aTable->setReadOnly( -1, 0, false );

    ViewModeButtonGroup->show();

    ExecutionGroupBox->hide();
  } 
  else if ( myMode == YACSGui_InputPanel::RunMode )
  {
    myNodeName->setReadOnly(true);
    
    myDirectChildrenGroupBox->HideBtn();
    YACSGui_Table* aTable = myDirectChildrenGroupBox->Table();
    aTable->setReadOnly( -1, 0, true );

    ViewModeButtonGroup->hide();

    ExecutionGroupBox->show();
  }
}

void YACSGui_BlockNodePage::notifyNodeStatus( int theStatus )
{
  if ( myNodeState )
  {
    myNodeState->setPaletteForegroundColor(getColor(theStatus));
    myNodeState->setText(getText(theStatus));
  }
}

void YACSGui_BlockNodePage::notifyNodeProgress()
{
  if ( myProgressBar )
  {
    Proc* aProc = dynamic_cast<Proc*>(getNode()->getRootNode());
    if ( !aProc ) return;

    YACSGui_Graph* aGraph = getInputPanel()->getModule()->getGraph( aProc );
    if ( !aGraph ) return;

    if ( YACSPrs_ElementaryNode* anItem = aGraph->getItem(getNode()) )
    {
      int aProgress = (int)( ( (anItem->getStoredPercentage() < 0) ? anItem->getPercentage() :
			                                             anItem->getStoredPercentage() ) );
      myProgressBar->setProgress(aProgress);
    }
  }
}

void YACSGui_BlockNodePage::notifyInPortValues( std::map<std::string,std::string> theInPortName2Value )
{
  DEBTRACE("==> BlockNodePage : Size of theInPortName2Value : " << theInPortName2Value.size());
}

void YACSGui_BlockNodePage::notifyOutPortValues( std::map<std::string,std::string> theOutPortName2Value )
{
  DEBTRACE("==> BlockNodePage : Size of theOutPortName2Value : " << theOutPortName2Value.size());
}

void YACSGui_BlockNodePage::notifyNodeCreateNode( YACS::HMI::Subject* theSubject )
{
  updateState();
}

void YACSGui_BlockNodePage::checkModifications()
{
  if ( !getNode() ) return;

  Bloc* aBloc = dynamic_cast<Bloc*>(getNode());
  if ( !aBloc ) return;

  // 1) check if the content of the page is really modified (in compare with the content of engine object)
  bool isModified = false;

  if ( myNodeName->text().compare(getNodeName()) != 0 ) isModified = true;
  else
  {
    // check child nodes
    YACSGui_Table* aTable = myDirectChildrenGroupBox->Table();
    if ( !aTable ) return;
    
    QStringList aChildNames;
    aTable->strings( 0, aChildNames );

    list<Node*> aChildren = aBloc->edGetDirectDescendants();

    if ( aChildNames.count() != aChildren.size() ) isModified = true;
    else
    {
      int aRowId = 0;
      for ( QStringList::Iterator it = aChildNames.begin(); it != aChildNames.end(); ++it )
      {
	Node* aChild = 0;
	if ( SubjectNode* aSChild = myRow2ChildMap[aRowId] )
	  aChild = aSChild->getNode();

	bool isContains = false;
        for (list<Node*>::iterator itn = aChildren.begin(); itn != aChildren.end(); ++itn)
          if (aChild == (*itn))
          {
	    isContains = true;
	    break;
	  }
	if (!isContains)
	{
	  isModified = true;
	  break;
	}

	aRowId++;
      }
    }
  }
  
  //if ( !isModified )
  //  TODO: compare view mode: expanded or collapsed (not yet in use)    
  
  // 2) if yes, show a warning message: Apply or Cancel
  if ( isModified )
    if ( SUIT_MessageBox::warn2(getInputPanel()->getModule()->getApp()->desktop(),
				tr("WRN_WARNING"),
				tr("APPLY_CANCEL_MODIFICATIONS"),
				tr("BUT_YES"), tr("BUT_NO"), 0, 1, 0) == 0 )
    {
      onApply();
      if ( getInputPanel() ) getInputPanel()->emitApply(YACSGui_InputPanel::InlineNodeId);
    }
}

void YACSGui_BlockNodePage::onApply()
{
  // Rename a node
  if ( myNodeName && getNode() )
    getNode()->setName( myNodeName->text().latin1() );

  // To remove a new child nodes from its old places in the tree view
  std::list<int>::iterator it = myRowsOfSelectedChildren.begin();
  for ( ; it != myRowsOfSelectedChildren.end(); it++ )
  {
    SubjectNode* aNewSelectedChild = myRow2ChildMap[*it];
    if( !aNewSelectedChild )
      continue;
    Node* aNode = aNewSelectedChild->getNode();
    try {
      getNode()->getChildByName( aNode->getName() );
    }
    catch (YACS::Exception& ex) {
      // block is not contain nodes with the same name as aNode
      // get schema engine object
      Proc* aProc = GuiContext::getCurrent()->getProc();
      if ( !aProc ) return;
      // get gui graph
      YACSGui_Graph* aGraph = getInputPanel()->getModule()->getGraph( aProc );
      if ( !aGraph ) return;

      aNewSelectedChild->getParent()->detach(aGraph); // to remove only a node view item in the tree, node presentation is still alive
	
      aNewSelectedChild->getParent()->update( REMOVE,
					      ProcInvoc::getTypeOfNode( aNewSelectedChild->getNode() ),
					      aNewSelectedChild );
      
      aNewSelectedChild->getParent()->attach(aGraph); // to remove only a node view item in the tree, node presentation is still alive
    }
  }
  
  // Reset the list of direct children
  setDirectChildren();
  
  // Reset the view mode
  // ...

  mySNode->update( EDIT, 0, mySNode );
  
  // clear for next selections
  myRowsOfSelectedChildren.clear();

  updateBlocSize();
}

void YACSGui_BlockNodePage::updateState()
{
  myRow2ChildMap.clear();
  myRowsOfSelectedChildren.clear();

  // Clear input ports table and output ports tables, if they are not empty
  if ( myDirectChildrenGroupBox ) {
    myDirectChildrenGroupBox->Table()->setNumRows( 0 );
    myDirectChildrenGroupBox->EnableBtn( YACSGui_PlusMinusGrp::AllBtn );
  }

  // Set node name
  if ( myNodeName ) myNodeName->setText( getNodeName() );

  // Set node full name (read only)
  if ( myNodeFullName )
    myNodeFullName->setText( getNode()->getRootNode()->getChildName(getNode()) );
  
  // Fill the table of child nodes of the block
  SubjectBloc* aSNode = dynamic_cast<SubjectBloc*>(mySNode);
  if ( aSNode && dynamic_cast<Bloc*>(getNode()) && myDirectChildrenGroupBox )
  {
    QStringList aChildNames;
    
    list<Node*> aChildren = dynamic_cast<Bloc*>(getNode())->edGetDirectDescendants();
    list<Node*>::iterator aChildIt = aChildren.begin();
    int aRow = 0;
    for( ;aChildIt!=aChildren.end();aChildIt++)
    {
      // Collect child names in the list
      aChildNames.append( (*aChildIt)->getName() );
      if ( myRow2ChildMap.find(aRow) == myRow2ChildMap.end() )
	myRow2ChildMap.insert( std::make_pair( aRow, aSNode->getChild(*aChildIt) ) );
      else
	myRow2ChildMap[aRow] = aSNode->getChild(*aChildIt);
      aRow++;
    }

    // Fill "Node name" column
    myDirectChildrenGroupBox->Table()->setStrings( 0, aChildNames, true );

    if ( !aChildNames.empty() ) myDirectChildrenGroupBox->EnableBtn( YACSGui_PlusMinusGrp::AllBtn );
  }

  // Set view mode of the given block node
  // the following method can be used if its needed:
  // YACSGui_Graph* aGraph = getInputPanel()->getModule()->getGraph( myProc );
  // ...
}

void YACSGui_BlockNodePage::onNodeNameChanged( const QString& theName )
{
  if ( myNodeFullName )
  {
    QString anOldName = myNodeFullName->text();
    int aRDotId = anOldName.findRev('.');
    myNodeFullName->setText( anOldName.replace( aRDotId+1, anOldName.length()-(aRDotId+1), theName ) );
  }
}

void YACSGui_BlockNodePage::onInitSelection( const int theRow, const int theCol )
{
  myIsSelectChild = true;
}

void YACSGui_BlockNodePage::onNewItem( const int theRow, const int theCol )
{
  if ( myDirectChildrenGroupBox )
    myDirectChildrenGroupBox->Table()->setReadOnly( theRow, theCol, true );
}

void YACSGui_BlockNodePage::onAdded( const int theRow )
{
  SubjectNode* aNode = 0;
  myRow2ChildMap.insert( std::make_pair( theRow, aNode ) );
}

void YACSGui_BlockNodePage::onInserted( const int theRow )
{
  SubjectNode* aNode = 0;

  if ( myRow2ChildMap.find(theRow) == myRow2ChildMap.end() )
    myRow2ChildMap.insert( std::make_pair( theRow, aNode ) );
  else
  {    
    SubjectNode* aStartNodeToMoveDown = myRow2ChildMap[theRow];
    myRow2ChildMap[theRow] = aNode;
    
    for ( int aRow = myRow2ChildMap.size()-1; aRow > theRow+1; aRow-- )
      myRow2ChildMap[aRow] = myRow2ChildMap[aRow-1];
    
    myRow2ChildMap[theRow+1] = aStartNodeToMoveDown;
  }
}

void YACSGui_BlockNodePage::onMovedUp( const int theUpRow )
{
  SubjectNode* aNodeToMoveDown = myRow2ChildMap[theUpRow];
  SubjectNode* aNodeToMoveUp = myRow2ChildMap[theUpRow+1];
  
  myRow2ChildMap[theUpRow] = aNodeToMoveUp;
  myRow2ChildMap[theUpRow+1] = aNodeToMoveDown;
  
}

void YACSGui_BlockNodePage::onMovedDown( const int theDownRow )
{
  SubjectNode* aNodeToMoveUp = myRow2ChildMap[theDownRow];
  SubjectNode* aNodeToMoveDown = myRow2ChildMap[theDownRow-1];
  
  myRow2ChildMap[theDownRow] = aNodeToMoveDown;
  myRow2ChildMap[theDownRow-1] = aNodeToMoveUp;
}

void YACSGui_BlockNodePage::onRemoved( const int theRow )
{
  /*
  SubjectNode* aSNodeToRaise = myRow2ChildMap[theRow];
  Node* aNodeToRaise = aSNodeToRaise->getNode();

  Bloc* aBloc = dynamic_cast<Bloc*>( getNode() );
  if ( !aBloc ) return;

  // remove aNodeToRaise node from the children list (if it is present in it)
  set<Node*> aChildren = aBloc->edGetDirectDescendants();
  set<Node*>::iterator aChildIter = aChildren.begin();
  for( ;aChildIter!=aChildren.end();aChildIter++)
  {
    if ( *aChildIter == aNodeToRaise )
    {
      // get schema engine object
      Proc* aProc = dynamic_cast<Proc*>(aBloc->getRootNode());
      if ( !aProc ) return;
      
      // get schema subject
      //YACSGui_Graph* aGraph = getInputPanel()->getModule()->getGraph( aProc );
      //if ( !aGraph ) return;
      //SubjectProc* aSProc = aGraph->getContext()->getSubjectProc();
      SubjectProc* aSProc = GuiContext::getCurrent()->getSubjectProc()
      if ( !aSProc ) return;
      
      aBloc->edRemoveChild(aNodeToRaise);
      
      try {
	aProc->edAddChild(aNodeToRaise); // raise the removed child to the Proc level
	aSNodeToRaise->reparent(aSProc);
      }
      catch (YACS::Exception& ex) {
	if ( SUIT_MessageBox::warn2(getInputPanel()->getModule()->getApp()->desktop(),
				    tr("WRN_WARNING"),
				    QString(ex.what())+QString("\nThe child node will be deleted. Do you want to continue?"),
				    tr("BUT_YES"), tr("BUT_NO"), 0, 1, 1) == 0 )
	{
	  // delete the old child node
	  if ( YACSGui_Graph* aGraph = getInputPanel()->getModule()->getGraph( aProc ) )
	    if ( YACSPrs_ElementaryNode* anItem = aGraph->getItem(aNodeToRaise) )
	    {
	      anItem->hide();
	      aGraph->removeItem(anItem);
	      delete anItem;
	    }
	  //aNodeToRaise = 0;	
	  delete aNodeToRaise;
	}
	else
	{
	  aBloc->edAddChild(aNodeToRaise);
	  return;
	}
      }
      break;
    }
  }
  */

  if ( theRow == myRow2ChildMap.size()-1 )
    myRow2ChildMap.erase(theRow);
  else
  {
    for ( int aRow = theRow; aRow < myRow2ChildMap.size()-1; aRow++ )
      myRow2ChildMap[aRow] = myRow2ChildMap[aRow+1];
    myRow2ChildMap.erase(myRow2ChildMap.size()-1);
  }
}

void YACSGui_BlockNodePage::setDirectChildren()
{
  if ( !myDirectChildrenGroupBox ) return;

  SubjectBloc* aSNode = dynamic_cast<SubjectBloc*>(mySNode);
  if ( !aSNode ) return;

  Bloc* aBloc = dynamic_cast<Bloc*>( getNode() );
  if ( !aBloc ) return;

  // get schema engine object
  Proc* aProc = dynamic_cast<Proc*>(aBloc->getRootNode());
  if ( !aProc ) return;

  // get schema subject
  SubjectProc* aSProc = GuiContext::getCurrent()->getSubjectProc();
  if ( !aSProc ) return;
  
  // get gui graph
  YACSGui_Graph* aGraph = getInputPanel()->getModule()->getGraph( aProc );
  if ( !aGraph ) return;

  // remove old children
  Node* aRemovedOldChild;
  list<Node*> aChildren = aBloc->edGetDirectDescendants();
  list<Node*>::iterator aChildIter = aChildren.begin();
  for( ;aChildIter!=aChildren.end();aChildIter++)
  {
    bool isNeedToRemove = true;
    map<int,SubjectNode*>::iterator aChildIt = myRow2ChildMap.begin();
    for( ;aChildIt!=myRow2ChildMap.end();aChildIt++)
      if ( (*aChildIt).second->getNode() == (*aChildIter) )
      {
	isNeedToRemove = false;
	break;
      }
    if ( !isNeedToRemove ) continue;

    aBloc->edRemoveChild(*aChildIter); // the _father of the removed node will be 0 
    aRemovedOldChild = *aChildIter;
    SubjectNode* aSChild = aSNode->getChild(*aChildIter);

    try {
      aProc->edAddChild(*aChildIter); // raise the removed child to the Proc level
      aSChild->reparent(aSProc);
    }
    catch (YACS::Exception& ex)
    {
      if ( SUIT_MessageBox::warn2(getInputPanel()->getModule()->getApp()->desktop(),
				  tr("WRN_WARNING"),
				  QString(ex.what())+QString("\nThe child node will be deleted. Do you want to continue?"),
				  tr("BUT_YES"), tr("BUT_NO"), 0, 1, 1) == 0 )
      {
	// delete the old child node
	// 1) delete corresponding tree view item
	aSNode->update(REMOVE, ProcInvoc::getTypeOfNode( aSChild->getNode() ), aSChild);
	// 2) delete 2D presentation
	aGraph->removeNode( aSChild->getNode() );
	// 3) delete corresponding subject
	aSNode->destroy(aSChild);
      }
      else
      {
	aBloc->edAddChild(aRemovedOldChild);
	updateState();
	return;
      }
    }
  }
  
  // read new children nodes from the table
  YACSGui_Table* aTable = myDirectChildrenGroupBox->Table();
  
  QStringList aChildNames;
  aTable->strings( 0, aChildNames );

  bool isNeedToUpdate = false;
  map<int,SubjectNode*>::iterator aChildIt = myRow2ChildMap.begin();
  for( ;aChildIt!=myRow2ChildMap.end();aChildIt++)
  {
    SubjectNode* aSChild = (*aChildIt).second;
    if( !aSChild )
      continue;
    Node* aChild = aSChild->getNode();
    if ( aChildNames.find( QString(aChild->getName()) ) != aChildNames.end() )
    {
      if ( Bloc* aFather = dynamic_cast<Bloc*>(aChild->getFather()) )
      {
	aFather->edRemoveChild(aChild);
      
	try {
	  aBloc->edAddChild(aChild);
	  aSChild->reparent(mySNode);
	}
	catch (YACS::Exception& ex)
	{
	  SUIT_MessageBox::warn1(getInputPanel()->getModule()->getApp()->desktop(),
				 tr("WRN_WARNING"),
				 QString(ex.what())+QString("\nRename, please, a new child node and try to add it into the block once more."),
				 tr("BUT_OK"));
	  
	  aFather->edAddChild(aChild);
	  isNeedToUpdate = true;
	}
      }
    }
  }
  
  if ( isNeedToUpdate ) updateState();
     
  // update the block node presentation
  aGraph->update( aBloc, dynamic_cast<SubjectComposedNode*>( getSNode()->getParent() ) );
}
