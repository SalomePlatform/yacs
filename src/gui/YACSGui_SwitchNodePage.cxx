//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D, OPEN CASCADE
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
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
  Class : YACSGui_SwitchNodePage
  Description :  Page for SWITCH node properties
*/

YACSGui_SwitchNodePage::YACSGui_SwitchNodePage( QWidget* theParent, const char* theName, WFlags theFlags )
  : SwitchNodePage( theParent, theName, theFlags ),
    YACSGui_NodePage(),
    myIsSelectChild(false)
{
  if ( mySwitchCasesGroupBox )
  {
    mySwitchCasesGroupBox->setTitle( tr("SWITCH Cases") );
    mySwitchCasesGroupBox->HideBtn( YACSGui_PlusMinusGrp::InsertBtn |
				    YACSGui_PlusMinusGrp::UpBtn |
				    YACSGui_PlusMinusGrp::DownBtn );
    mySwitchCasesGroupBox->EnableBtn( YACSGui_PlusMinusGrp::SelectBtn, false );
    YACSGui_Table* aTable = mySwitchCasesGroupBox->Table();
    aTable->setFixedHeight( 100 );
    aTable->setNumCols( 2 );
    aTable->horizontalHeader()->setLabel( 0, tr( "Case id" ) );
    aTable->horizontalHeader()->setLabel( 1, tr( "Node name" ) );
    aTable->setCellType( -1, 0, YACSGui_Table::String );
    aTable->setCellType( -1, 1, YACSGui_Table::String );

    connect( aTable,     SIGNAL(valueChanged( int, int )), this, SLOT(onValueChanged( int, int )) );

    mySwitchCasesGroupBox->EnableBtn( YACSGui_PlusMinusGrp::AllBtn );
    mySwitchCasesGroupBox->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );

    mySwitchCasesGroupBox->Btn(YACSGui_PlusMinusGrp::SelectBtn)->setTextLabel(tr("SELECT_BTN_TOOLTIP"),true);
    mySwitchCasesGroupBox->Btn(YACSGui_PlusMinusGrp::PlusBtn)->setTextLabel(tr("PLUS_BTN_TOOLTIP"),true);
    mySwitchCasesGroupBox->Btn(YACSGui_PlusMinusGrp::MinusBtn)->setTextLabel(tr("MINUS_BTN_TOOLTIP"),true);

    connect( myNodeName, SIGNAL(textChanged( const QString& )), this, SLOT(onNodeNameChanged( const QString& )) );
    connect( mySwitchCasesGroupBox, SIGNAL(initSelection( const int, const int )),
	     this,                  SLOT(onInitSelection( const int, const int )) );
    connect( mySwitchCasesGroupBox->Table(), SIGNAL(itemCreated( const int, const int )),
	     this,                           SLOT(onNewItem( const int, const int )) );
    
    connect( mySwitchCasesGroupBox, SIGNAL(Added( const int )),     this, SLOT(onAdded( const int )) );
    connect( mySwitchCasesGroupBox, SIGNAL(Inserted( const int )),  this, SLOT(onInserted( const int )) );
    connect( mySwitchCasesGroupBox, SIGNAL(MovedUp( const int )),   this, SLOT(onMovedUp( const int )) );
    connect( mySwitchCasesGroupBox, SIGNAL(MovedDown( const int )), this, SLOT(onMovedDown( const int )) );
    connect( mySwitchCasesGroupBox, SIGNAL(Removed( const int )),   this, SLOT(onRemoved( const int )) );
  }

  // TODO: implement presentation of the switch node in collapsed mode
  // temporary solution :
  myCollapsedRadioButton->setEnabled(false );
}

YACSGui_SwitchNodePage::~YACSGui_SwitchNodePage()
{
  if (getInputPanel()) getInputPanel()->removePage(this);

  myRow2ChildMap.clear();
  myRowsOfSelectedChildren.clear();
}

void YACSGui_SwitchNodePage::setSNode( YACS::HMI::SubjectNode* theSNode )
{
  if ( !theSNode ) return;

  if ( dynamic_cast<Switch*>( theSNode->getNode() ) )
    YACSGui_NodePage::setSNode( theSNode );
}

void YACSGui_SwitchNodePage::setChild( YACS::HMI::SubjectNode* theChildNode )
{
  if ( !theChildNode ) return;

  if ( !mySwitchCasesGroupBox ) return;
  YACSGui_Table* aTable = mySwitchCasesGroupBox->Table();

  myIsSelectChild = false;
  mySwitchCasesGroupBox->setOffSelect();
  mySwitchCasesGroupBox->EnableBtn( YACSGui_PlusMinusGrp::SelectBtn, false );

  // sets the child node name into table's cell
  QStringList aChildNames;
  aTable->strings( 1, aChildNames );
  aChildNames[aTable->currentRow()] = theChildNode->getNode()->getName();
  aTable->setStrings( 1, aChildNames );

  // add theChildNode into the map of children of this switch node
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

bool YACSGui_SwitchNodePage::isSelectChild() const
{
  return myIsSelectChild;
}

void YACSGui_SwitchNodePage::setMode( const YACSGui_InputPanel::PageMode theMode )
{
  YACSGui_NodePage::setMode(theMode);

  if ( myMode == YACSGui_InputPanel::EditMode )
  {
    myNodeName->setReadOnly(false);
    
    mySwitchCasesGroupBox->ShowBtn( YACSGui_PlusMinusGrp::SelectBtn |
				    YACSGui_PlusMinusGrp::PlusBtn |
				    YACSGui_PlusMinusGrp::MinusBtn );
    YACSGui_Table* aTable = mySwitchCasesGroupBox->Table();
    aTable->setReadOnly( -1, 0, false );

    ViewModeButtonGroup->show();

    ExecutionGroupBox->hide();
  } 
  else if ( myMode == YACSGui_InputPanel::RunMode )
  {
    myNodeName->setReadOnly(true);
    
    mySwitchCasesGroupBox->HideBtn();
    YACSGui_Table* aTable = mySwitchCasesGroupBox->Table();
    aTable->setReadOnly( -1, 0, true );

    ViewModeButtonGroup->hide();

    ExecutionGroupBox->show();
  }
}

void YACSGui_SwitchNodePage::notifyNodeStatus( int theStatus )
{
  if ( myNodeState )
  {
    myNodeState->setPaletteForegroundColor(getColor(theStatus));
    myNodeState->setText(getText(theStatus));
  }
}

void YACSGui_SwitchNodePage::notifyNodeProgress()
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

void YACSGui_SwitchNodePage::notifyInPortValues( std::map<std::string,std::string> theInPortName2Value )
{
  //printf("==> SwitchNodePage : Size of theInPortName2Value : %d\n",theInPortName2Value.size());

  Switch* aSwitchNode = dynamic_cast<Switch*>( getNode() );
  if ( !aSwitchNode ) return;

  if ( theInPortName2Value.size() == 1 )
  {
    // select port
    QString aName(aSwitchNode->edGetConditionPort()->getName());
    map<string,string>::iterator it = theInPortName2Value.begin();
    QString aGivenName( (*it).first );
    if ( !aGivenName.compare(aName) && mySelectInputPortValue )
    {
      QString aValue( (*it).second );
      if ( !aValue.compare(QString("< ? >")) )
        mySelectInputPortValue->setValue( mySelectInputPortValue->minValue() );
      else
        mySelectInputPortValue->setValue( aValue.toInt() );
    }
  }
}

void YACSGui_SwitchNodePage::notifyOutPortValues( std::map<std::string,std::string> theOutPortName2Value )
{
  //printf("==> SwitchNodePage : Size of theOutPortName2Value : %d\n",theOutPortName2Value.size());
}

void YACSGui_SwitchNodePage::notifyNodeCreateNode( YACS::HMI::Subject* theSubject )
{
  updateState();
}

void YACSGui_SwitchNodePage::checkModifications()
{
  DEBTRACE("YACSGui_SwitchNodePage::checkModifications");
  if ( !getNode() ) return;

  Switch* aSwitch = dynamic_cast<Switch*>(getNode());
  if ( !aSwitch ) return;

  // 1) check if the content of the page is really modified (in compare with the content of engine object)
  bool isModified = false;

  if ( myNodeName->text().compare(getNodeName()) != 0 ) isModified = true;
  else if( YACS::ENGINE::InputPort* aPort = aSwitch->edGetConditionPort() )
  {
    QString val=getPortValue( aPort );
    if ( !val.compare(QString("< ? >")) )
      {
         if(mySelectInputPortValue->value() != mySelectInputPortValue->minValue()) isModified = true;
      }
    else if(mySelectInputPortValue->value() != val.toInt()) isModified = true;
  }

  if ( !isModified )
  {
    // check switch cases nodes
    YACSGui_Table* aTable = mySwitchCasesGroupBox->Table();
    if ( !aTable ) return;
    
    QStringList aChildIds, aChildNames;
    aTable->strings( 0, aChildIds );
    aTable->strings( 1, aChildNames );

    std::list<Node*> aCaseNodes = aSwitch->edGetDirectDescendants();
    if ( aChildIds.count() != aCaseNodes.size() ) isModified = true;
    else
    {
      std::list<Node*>::iterator anIter;
      std::map<int,Node*> aCase2Node;
      for ( anIter = aCaseNodes.begin(); anIter != aCaseNodes.end(); ++anIter )
	aCase2Node.insert(make_pair(aSwitch->getRankOfNode(*anIter),(*anIter)));

      int aRowId = 0;
      for ( QStringList::Iterator it = aChildIds.begin(); it != aChildIds.end(); ++it )
      {
	int aCaseId;
	if( (*it).compare(Switch::DEFAULT_NODE_NAME) == 0 )
	  aCaseId = Switch::ID_FOR_DEFAULT_NODE;
	else
	  aCaseId = (*it).toInt();

	map<int,Node*>::iterator iter = aCase2Node.find(aCaseId);
	if ( iter == aCase2Node.end() )
	{
	  isModified = true;
	  break;
	}
	else
	{ 
	  Node* aChild = 0;
	  if ( SubjectNode* aSChild = myRow2ChildMap[aRowId] )
	    aChild = aSChild->getNode();

	  if ( (*iter).second != aChild )
	  {
	    isModified = true;
	    break;
	  }
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
      if(onApplyStatus=="ERROR")
        throw Exception("Error in checkModifications");
      if ( getInputPanel() ) getInputPanel()->emitApply(YACSGui_InputPanel::SwitchNodeId);
    }
    else
      updateState();
}

void YACSGui_SwitchNodePage::onApply()
{
  DEBTRACE("YACSGui_SwitchNodePage::onApply");
  onApplyStatus="OK";
  // Rename a node
  if ( myNodeName && getNode() )
    getNode()->setName( myNodeName->text().latin1() );
  
  // Reset select input port value
  if ( YACS::ENGINE::Switch* aSwitchNode = dynamic_cast<Switch*>( getNode() ) )
  {
    if( YACS::ENGINE::InputPort* aPort = aSwitchNode->edGetConditionPort() )
    {
      int aValue = mySelectInputPortValue->value();
      aPort->edInit( aValue );
      mySNode->update( EDIT, INPUTPORT, GuiContext::getCurrent()->_mapOfSubjectDataPort[aPort] );
    }
  }

  // To remove a new child nodes from its old places in the tree view
  std::list<int>::iterator it = myRowsOfSelectedChildren.begin();
  for ( ; it != myRowsOfSelectedChildren.end(); it++ )
  {
    SubjectNode* aNewSelectedChild = myRow2ChildMap[*it];
    Node* aNode = aNewSelectedChild->getNode();

    if( SubjectSwitch* aSSwitch = dynamic_cast<SubjectSwitch*>( aNewSelectedChild->getParent() ) )
    {
      Switch* aSwitch = dynamic_cast<Switch*>( aSSwitch->getNode() );

      Node* aDefaultNode = 0;
      try {
	aDefaultNode = aSwitch->getChildByShortName(Switch::DEFAULT_NODE_NAME);
      }
      catch (YACS::Exception& ex) {}

      if( aDefaultNode && aDefaultNode == aNode )
	continue; // do not remove a default node - keep it for copying
    }

  }
  
  // Reset the list of switch cases
  setSwitchCases();
  
  updateLabelPorts();

  if(onApplyStatus=="OK")
    updateState();

  updateBlocSize();
}

void YACSGui_SwitchNodePage::updateState()
{
  DEBTRACE("YACSGui_SwitchNodePage::updateState");
  myRow2ChildMap.clear();
  myRowsOfSelectedChildren.clear();

  // Clear input ports table and output ports tables, if they are not empty
  if ( mySwitchCasesGroupBox ) {
    mySwitchCasesGroupBox->Table()->setNumRows( 0 );
    mySwitchCasesGroupBox->EnableBtn( YACSGui_PlusMinusGrp::AllBtn );
  }

  // Set node name
  if ( myNodeName ) myNodeName->setText( getNodeName() );

  // Set node full name (read only)
  if ( myNodeFullName )
    myNodeFullName->setText( getNode()->getRootNode()->getChildName(getNode()) );
  
  // Fill the table of child nodes of the switch
  SubjectSwitch* aSNode = dynamic_cast<SubjectSwitch*>(mySNode);
  if ( aSNode && mySwitchCasesGroupBox )
  {
    if( Switch* aSwitch = dynamic_cast<Switch*>(getNode()) )
    {
      if( YACS::ENGINE::InputPort* aPort = aSwitch->edGetConditionPort() )
      {
	bool ok;
	int aValue = getPortValue( aPort ).toInt( &ok );
	if( ok )
	mySelectInputPortValue->setValue( aValue );
      }

      list<Node*> aChildren = dynamic_cast<Switch*>(getNode())->edGetDirectDescendants();
      if ( !aChildren.empty() )
      {
	list<Node*>::iterator aChildIt = aChildren.begin();

	Node* aDefaultNode = 0;
	try {
	  aDefaultNode = aSwitch->getChildByShortName(Switch::DEFAULT_NODE_NAME);
	}
	catch (YACS::Exception& ex) {
          //ignore it
	}
	
	int aMinCaseId, aMaxCaseId;
	aMinCaseId = aMaxCaseId = aSwitch->getRankOfNode(*aChildIt);
	
	list<Node*> aCaseNodes;
	
	for( ; aChildIt != aChildren.end(); aChildIt++ )
	{
	  Node* aNode = *aChildIt;
	  if( aNode == aDefaultNode )
	    continue;
	  
	  int aRank = aSwitch->getRankOfNode( aNode );
	  if( aMinCaseId >= aRank )
	  {
	    aCaseNodes.push_front( aNode );
	    aMinCaseId = aRank;
	  }
	  else if( aMinCaseId < aRank && aMaxCaseId > aRank )
	  {
	    list<Node*>::iterator aCaseNodesIt = aCaseNodes.begin();
	    for( list<Node*>::iterator anIt = aCaseNodesIt;
		 anIt++ != aCaseNodes.end();
		 aCaseNodesIt++, anIt = aCaseNodesIt )
	    {
	      if ( aRank >= aSwitch->getRankOfNode( *aCaseNodesIt ) &&
		   aSwitch->getRankOfNode( aNode ) <= aSwitch->getRankOfNode( *anIt ) )
	      {
		aCaseNodes.insert( anIt, aNode );
		break;
	      }
	    }
	  }
	  else if( aMaxCaseId <= aRank )
	  {
	    aCaseNodes.push_back( aNode );
	    aMaxCaseId = aRank;
	  }
	}
	aCaseNodes.push_back( aDefaultNode );
	
	QStringList aChildIds;
	QStringList aChildNames;
	
	list<Node*>::iterator anIt = aCaseNodes.begin();
	list<Node*>::iterator anItEnd = aCaseNodes.end();
	for( int aRow = 0; anIt != anItEnd; anIt++, aRow++ )
	{
	  QString anId, aName;
	  Node* aNode = *anIt;

	  if ( !aNode ) continue;
	  
	  if( aNode == aDefaultNode )
	    anId = Switch::DEFAULT_NODE_NAME;
	  else
	    anId = QString::number( aSwitch->getRankOfNode( aNode ) );
	  aChildIds.append( anId );
	  
	  // Collect child names in the list
	  aName = aNode->getName();
	  aChildNames.append( aName );
	  
	  if ( myRow2ChildMap.find(aRow) == myRow2ChildMap.end() )
	    myRow2ChildMap.insert( std::make_pair( aRow, aSNode->getChild(aNode) ) );
	  else
	    myRow2ChildMap[aRow] = aSNode->getChild(aNode);
	}
	
	// Fill "Node name" column
	mySwitchCasesGroupBox->Table()->setStrings( 0, aChildIds, true );
	mySwitchCasesGroupBox->Table()->setStrings( 1, aChildNames, true );
	
	if ( !aChildNames.empty() ) mySwitchCasesGroupBox->EnableBtn( YACSGui_PlusMinusGrp::AllBtn );
      }
    }
  }
  
  // Set view mode of the given switch node
  // the following method can be used if its needed:
  // YACSGui_Graph* aGraph = getInputPanel()->getModule()->getGraph( myProc );
  // ...
}

void YACSGui_SwitchNodePage::updateLabelPorts()
{
  if( Proc* aProc = dynamic_cast<Proc*>(getNode()->getRootNode()) )
  {
    if( YACSGui_Graph* aGraph = getInputPanel()->getModule()->getGraph( aProc ) )
    {
      if ( YACSPrs_ElementaryNode* aNodePrs = aGraph->getItem( getNode() ) )
      {
	QPtrList<YACSPrs_Port> aPorts = aNodePrs->getPortList();
	for (YACSPrs_Port* aPort = aPorts.first(); aPort; aPort = aPorts.next())
	{
	  if ( YACSPrs_LabelPort* aLabelPort = dynamic_cast<YACSPrs_LabelPort*>( aPort ) )
	    aGraph->update( aLabelPort );
	}
      }
      aGraph->getCanvas()->update();
    }
  }
}

void YACSGui_SwitchNodePage::onNodeNameChanged( const QString& theName )
{
  if ( myNodeFullName )
  {
    QString anOldName = myNodeFullName->text();
    int aRDotId = anOldName.findRev('.');
    myNodeFullName->setText( anOldName.replace( aRDotId+1, anOldName.length()-(aRDotId+1), theName ) );
  }
}

void YACSGui_SwitchNodePage::onValueChanged( int theRow, int theCol )
{
  if ( YACSGui_Table* aTable = ( YACSGui_Table* )sender() )
  {
    if ( theCol == 0 )
    {
      QString anId = aTable->item( theRow, theCol )->text();
      QStringList anIds;
      aTable->strings( 0, anIds );
      if( anIds.contains( anId ) > 1 )
	SUIT_MessageBox::warn1(getInputPanel()->getModule()->getApp()->desktop(),
			       tr("WRN_WARNING"),
			       QString("Switch is already contains a node with such id."),
			       tr("BUT_OK"));
    }
  }
}

void YACSGui_SwitchNodePage::onInitSelection( const int theRow, const int theCol )
{
  myIsSelectChild = true;
}

void YACSGui_SwitchNodePage::onNewItem( const int theRow, const int theCol )
{
  if ( mySwitchCasesGroupBox && theCol == 1 ) // set "node name" column read only
    mySwitchCasesGroupBox->Table()->setReadOnly( theRow, theCol, true );
}

void YACSGui_SwitchNodePage::onAdded( const int theRow )
{
  SubjectNode* aNode = 0;
  myRow2ChildMap.insert( std::make_pair( theRow, aNode ) );
}

void YACSGui_SwitchNodePage::onInserted( const int theRow )
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

void YACSGui_SwitchNodePage::onMovedUp( const int theUpRow )
{
  SubjectNode* aNodeToMoveDown = myRow2ChildMap[theUpRow];
  SubjectNode* aNodeToMoveUp = myRow2ChildMap[theUpRow+1];
  
  myRow2ChildMap[theUpRow] = aNodeToMoveUp;
  myRow2ChildMap[theUpRow+1] = aNodeToMoveDown;
  
}

void YACSGui_SwitchNodePage::onMovedDown( const int theDownRow )
{
  SubjectNode* aNodeToMoveUp = myRow2ChildMap[theDownRow];
  SubjectNode* aNodeToMoveDown = myRow2ChildMap[theDownRow-1];
  
  myRow2ChildMap[theDownRow] = aNodeToMoveDown;
  myRow2ChildMap[theDownRow-1] = aNodeToMoveUp;
}

void YACSGui_SwitchNodePage::onRemoved( const int theRow )
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

void YACSGui_SwitchNodePage::setSwitchCases()
{
  if ( !mySwitchCasesGroupBox ) return;

  SubjectSwitch* aSNode = dynamic_cast<SubjectSwitch*>(mySNode);
  if ( !aSNode ) return;

  Switch* aSwitch = dynamic_cast<Switch*>( getNode() );
  if ( !aSwitch ) return;

  // get schema engine object
  Proc* aProc = dynamic_cast<Proc*>(aSwitch->getRootNode());
  if ( !aProc ) return;

  // get schema subject
  SubjectProc* aSProc = GuiContext::getCurrent()->getSubjectProc();
  if ( !aSProc ) return;
  
  // get gui graph
  YACSGui_Graph* aGraph = getInputPanel()->getModule()->getGraph( aProc );
  if ( !aGraph ) return;

  //printf("myRow2ChildMap:\n");
  //for( map<int,SubjectNode*>::iterator aChildIt = myRow2ChildMap.begin(); aChildIt != myRow2ChildMap.end(); aChildIt++ )
  //  printf( "%d - %s\n", (*aChildIt).first, ((*aChildIt).second)->getNode()->getName().c_str() );

  // remove old children
  Node* aRemovedOldChild;
  list<Node*> aChildren = aSwitch->edGetDirectDescendants();
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

    int anId;
    Node* aDefaultNode = 0;
    try {
      aDefaultNode = aSwitch->getChildByShortName(Switch::DEFAULT_NODE_NAME);
    }
    catch (YACS::Exception& ex) {
    }
    if( aDefaultNode && (*aChildIter) == aDefaultNode )
      anId = Switch::ID_FOR_DEFAULT_NODE;
    else
      anId = aSwitch->getRankOfNode( *aChildIter );

    SubjectNode* aSChild = aSNode->getChild(*aChildIter);
    aSChild->removeExternalLinks();
    //aSwitch->edRemoveChild(*aChildIter); // the _father of the removed node will be 0 
    aSwitch->edReleaseCase( anId );
    aRemovedOldChild = *aChildIter;

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
	aSNode->destroy(aSChild);
      }
      else
      {
	//aSwitch->edAddChild(aRemovedOldChild);
	updateState();
	return;
      }
    }
  }

  // read new children nodes from the table
  YACSGui_Table* aTable = mySwitchCasesGroupBox->Table();
  
  QStringList aChildIds, aChildNames;
  aTable->strings( 0, aChildIds );
  aTable->strings( 1, aChildNames );

  bool isNeedToUpdate = false;
  map<int,SubjectNode*>::iterator aChildIt = myRow2ChildMap.begin();
  for( ;aChildIt!=myRow2ChildMap.end();aChildIt++)
  {
    SubjectNode* aSChild = (*aChildIt).second;
    if(!aSChild)
      {
        SUIT_MessageBox::error1(getInputPanel()->getModule()->getApp()->desktop(),
                                tr("ERROR"),"incorrect node" , tr("BUT_OK"));
        onApplyStatus="ERROR";
      }

    Node* aChild = aSChild->getNode();
    if( aChild->getFather() == aSwitch )
      continue;

    int aRow = aChildNames.findIndex( QString(aChild->getName()) );
    if ( aRow != -1 )
    {
      int aNewId;
      QString aStr = aChildIds[ aRow ];
      if( aStr == Switch::DEFAULT_NODE_NAME )
	aNewId = Switch::ID_FOR_DEFAULT_NODE;
      else
	aNewId = aStr.toInt();

      if ( Switch* aFather = dynamic_cast<Switch*>(aChild->getFather()) )
      {
	int anId;
	Node* aFatherDefaultNode = 0;
	try {
	  aFatherDefaultNode = aFather->getChildByShortName(Switch::DEFAULT_NODE_NAME);
	}
	catch (YACS::Exception& ex) {}

	if( aFatherDefaultNode && aChild == aFatherDefaultNode )
	{
	  anId = Switch::ID_FOR_DEFAULT_NODE;

	  // If the child is a default node of the another switch - just copy it to the current switch
          YACS::ENGINE::Catalog* catalog = getInputPanel()->getModule()->getCatalog();

	  std::string compo;
	  std::string type;
	  if( YACS::ENGINE::ServiceNode* aServiceNode = dynamic_cast<ServiceNode*>( aChild ) )
	  {
	    if ( aServiceNode->getComponent() )
	      compo = aServiceNode->getComponent()->getCompoName();
	    type = aServiceNode->getMethod();
	  }
	  else
	    type = getInputPanel()->getModule()->getNodeType( ProcInvoc::getTypeOfNode(aChild) );

	  std::string name = aChild->getName();
	  if( aSNode->addNode( catalog, compo, type, name, aNewId, true ) )
	  {
	    Node* aNewChild;
	    list<Node*> aNewChildren = aSwitch->edGetDirectDescendants();
	    list<Node*>::iterator aNewChildIt = aNewChildren.begin();
	    list<Node*>::iterator aNewChildItEnd = aNewChildren.end();
	    for( ; aNewChildIt != aNewChildItEnd; aNewChildIt++ )
	      if( (*aNewChildIt)->getName() == name )
	      {
		aNewChild = (*aNewChildIt);
		break;
	      }

	    aGraph->update( aNewChild, dynamic_cast<SubjectSwitch*>( getSNode() ) );
	    aGraph->show();
	  }
	}
	else
	  anId = aFather->getRankOfNode( aChild );

	if( anId == Switch::ID_FOR_DEFAULT_NODE )
	  continue;

        aSChild->removeExternalLinks();
	aFather->edReleaseCase( anId );
      
	try {
	  aSwitch->edSetNode(aNewId, aChild);
	  aSChild->reparent(mySNode);
	}
	catch (YACS::Exception& ex)
	{
	  SUIT_MessageBox::warn1(getInputPanel()->getModule()->getApp()->desktop(),
				 tr("WRN_WARNING"),
				 QString(ex.what())+QString("\nRename, please, a new child node and try to add it into the switch once more."),
				 tr("BUT_OK"));
	  
	  aFather->edSetNode(anId, aChild);
	  isNeedToUpdate = true;
	}
      }
      else if ( Bloc* aFather = dynamic_cast<Bloc*>(aChild->getFather()) )
      {
        aSChild->removeExternalLinks();
	aFather->edRemoveChild(aChild);
      
	try {
	  aSwitch->edSetNode(aNewId, aChild);
	  aSChild->reparent(mySNode);
	}
	catch (YACS::Exception& ex)
	{
	  SUIT_MessageBox::warn1(getInputPanel()->getModule()->getApp()->desktop(),
				 tr("WRN_WARNING"),
				 QString(ex.what())+QString("\nRename, please, a new child node and try to add it into the switch once more."),
				 tr("BUT_OK"));
	  
	  aFather->edAddChild(aChild);
	  isNeedToUpdate = true;
	}
      }
    }
  }
}

