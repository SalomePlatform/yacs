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
  Class : YACSGui_ServiceNodePage
  Description :  Page for SALOME and CORBA service nodes properties
*/

YACSGui_ServiceNodePage::YACSGui_ServiceNodePage( QWidget* theParent, const char* theName, WFlags theFlags )
  : ServiceNodePage( theParent, theName, theFlags ),
    YACSGui_NodePage(),
    myComponent( 0 ),
    mySCNode( 0 )
{
  // the possibility to search is hided according to the Num. 14 from NEWS file
  Search->hide();
  mySearch->hide();

  //#undef HAVE_QEXTSCINTILLA_H
#ifdef HAVE_QEXTSCINTILLA_H
  _myTextEdit = new QextScintilla( InPythonEditorGroupBox, "Python Editor" );
  _myTextEdit->setMinimumHeight(150);
  InPythonEditorGroupBoxLayout->insertWidget(InPythonEditorGroupBoxLayout->findWidget(myTextEdit),
                                             _myTextEdit );
  InPythonEditorGroupBoxLayout->remove(myTextEdit);
  delete myTextEdit;
  _myTextEdit->setUtf8(1);
  QextScintillaLexerPython *lex = new QextScintillaLexerPython(_myTextEdit);
  _myTextEdit->setLexer(lex);
  _myTextEdit->setBraceMatching(QextScintilla::SloppyBraceMatch);
  _myTextEdit->setAutoIndent(1);
  _myTextEdit->setIndentationWidth(4);
  _myTextEdit->setIndentationGuides(1);
  _myTextEdit->setIndentationsUseTabs(0);
  _myTextEdit->setAutoCompletionThreshold(2);
#endif

  if ( !myInputPortsGroupBox || !myOutputPortsGroupBox ) return;

  QString aPortTypes = QString("Data Flow;Data Stream (BASIC);Data Stream (CALCIUM);Data Stream (PALM)");
  QString aValueTypes = QString("double;int;string;bool;Objref;Sequence;Array;Struct");

  // Input Ports table
  myInputPortsGroupBox->setTitle( tr("Input Ports") );
  YACSGui_Table* aTable = myInputPortsGroupBox->Table();
  //aTable->setFixedHeight( 100 );
  aTable->setNumCols( 4 );
  aTable->horizontalHeader()->setLabel( 0, tr( "Name" ) );
  aTable->horizontalHeader()->setLabel( 1, tr( "Class" ) );
  aTable->horizontalHeader()->setLabel( 2, tr( "Type" ) );
  aTable->horizontalHeader()->setLabel( 3, tr( "Value" ) );
  aTable->setColumnWidth(0,58);
  aTable->setColumnWidth(1,84);
  aTable->setColumnWidth(2,80);
  aTable->setCellType( -1, 0, YACSGui_Table::String );
  aTable->setCellType( -1, 1, YACSGui_Table::Combo );
  aTable->setCellType( -1, 2, YACSGui_Table::Combo );
  //aTable->setCellType( -1, 3, YACSGui_Table::??? ); depends on combo box item choosen in the "Value type" column

  aTable->setParams( 0, 1, aPortTypes );
  aTable->setParams( 0, 2, aValueTypes );

  aTable->setEditorSync(true);

  connect( aTable, SIGNAL(valueChanged( int, int )), this, SLOT(onValueChanged( int, int )) );
  
  myInputPortsGroupBox->HideBtn( YACSGui_PlusMinusGrp::PlusBtn | YACSGui_PlusMinusGrp::MinusBtn |
				 YACSGui_PlusMinusGrp::InsertBtn | YACSGui_PlusMinusGrp::SelectBtn );
  myInputPortsGroupBox->EnableBtn( YACSGui_PlusMinusGrp::UpBtn | YACSGui_PlusMinusGrp::DownBtn );
  myInputPortsGroupBox->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );

  // Output Ports table  
  myOutputPortsGroupBox->setTitle( tr("Output Ports") );
  aTable = myOutputPortsGroupBox->Table();
  //aTable->setFixedHeight( 100 );
  aTable->setNumCols( 5 );
  aTable->horizontalHeader()->setLabel( 0, tr( "Name" ) );
  aTable->horizontalHeader()->setLabel( 1, tr( "Class" ) );
  aTable->horizontalHeader()->setLabel( 2, tr( "Type" ) );
  aTable->horizontalHeader()->setLabel( 3, tr( "Value" ) );
  aTable->horizontalHeader()->setLabel( 4, tr( "Is in study" ) );
  aTable->setColumnWidth(0,58);
  aTable->setColumnWidth(1,84);
  aTable->setColumnWidth(2,80);

  aTable->setCellType( -1, 0, YACSGui_Table::String );
  aTable->setCellType( -1, 1, YACSGui_Table::Combo );
  aTable->setCellType( -1, 2, YACSGui_Table::Combo );
  //aTable->setCellType( -1, 3, YACSGui_Table::??? ); depends on combo box item choosen in the "Value type" column
  aTable->setCellType( -1, 4, YACSGui_Table::Combo );

  aTable->setParams( 0, 1, aPortTypes );
  aTable->setParams( 0, 2, aValueTypes );
  aTable->setParams( 0, 4, QString("Yes;No") );
  aTable->setDefValue( 0, 4, QString("Yes") );

  aTable->setEditorSync(true);

  myPara = 0;
  myIndex = 0;

  connect( aTable, SIGNAL(valueChanged( int, int )), this, SLOT(onValueChanged( int, int )) );

  myOutputPortsGroupBox->HideBtn( YACSGui_PlusMinusGrp::PlusBtn | YACSGui_PlusMinusGrp::MinusBtn |
				  YACSGui_PlusMinusGrp::InsertBtn | YACSGui_PlusMinusGrp::SelectBtn );
  myOutputPortsGroupBox->EnableBtn( YACSGui_PlusMinusGrp::UpBtn | YACSGui_PlusMinusGrp::DownBtn );
  myOutputPortsGroupBox->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );

  myProcName = "";
  myProcRadioButton->setEnabled(false);

  connect( myNodeName, SIGNAL(textChanged( const QString& )), this, SLOT(onNodeNameChanged( const QString& )) );
  connect( myInstanceName, SIGNAL(activated( const QString& )), this, SLOT(onInstanceNameChanged( const QString& ) ) );
  connect( myBrowseButton, SIGNAL(clicked(  )), this, SLOT(onBrowse( )) );
  connect( this, SIGNAL( enableApply( bool ) ), getInputPanel(), SLOT( onApplyEnabled( bool ) ) );
  connect( CatalogTree, SIGNAL( clicked( QListViewItem* ) ), this, SLOT( onCatalogMethodClicked( QListViewItem* ) ) );
  connect( mySessionRadioButton, SIGNAL( stateChanged( int ) ), this, SLOT( onCatalogChanged( int ) ) );
  connect( myProcRadioButton, SIGNAL( stateChanged( int ) ), this, SLOT( onCatalogChanged( int ) ) );

  //connect( this, SIGNAL(visibilityChanged( bool )), this, SLOT(onVisibilityChanged( bool )) );
}

YACSGui_ServiceNodePage::~YACSGui_ServiceNodePage()
{
  if (getInputPanel()) getInputPanel()->removePage(this);
}

void YACSGui_ServiceNodePage::setSCNode( YACS::HMI::SubjectComposedNode* theSCNode )
{
  DEBTRACE("YACSGui_ServiceNodePage::setSCNode");
  if ( theSCNode && mySCNode != theSCNode )
    mySCNode = theSCNode;
}

void YACSGui_ServiceNodePage::setSNode( YACS::HMI::SubjectNode* theSNode )
{
  if ( !theSNode ) return;

  if ( YACS::ENGINE::ServiceNode* aServiceNode = dynamic_cast<ServiceNode*>( theSNode->getNode() ) )
  {
    myComponentName = QString::null;
    myComponent = aServiceNode->getComponent();
    YACSGui_NodePage::setSNode( theSNode );
  }
}

void YACSGui_ServiceNodePage::setComponent( YACS::ENGINE::ComponentInstance* theComponent )
{
  DEBTRACE("YACSGui_ServiceNodePage::setComponent");
  if (!theComponent) return;
  ServiceNode* aServiceNode = dynamic_cast<ServiceNode*>(getNode());
  if (!aServiceNode) return;
  ComponentInstance* compo = aServiceNode->getComponent();
  if (!compo) return;
  
  std::string currentInstName = compo->getInstanceName();
  std::string anInstanceName = theComponent->getInstanceName();
  
  if( currentInstName != anInstanceName )
  {
    myComponentName = theComponent->getCompoName();
    myInstanceName->setCurrentText(theComponent->getInstanceName());
    myComponent = theComponent;
    updateState();

    YACSGui_ComponentPage* aCompPage = 
      dynamic_cast<YACSGui_ComponentPage*>( getInputPanel()->getPage( YACSGui_InputPanel::ComponentId ) );
    if ( aCompPage )
      aCompPage->setSComponent( GuiContext::getCurrent()->_mapOfSubjectComponent[theComponent], false );
  }
}

void YACSGui_ServiceNodePage::setMode( const YACSGui_InputPanel::PageMode theMode )
{
  DEBTRACE("YACSGui_ServiceNodePage::setMode");
  YACSGui_NodePage::setMode(theMode);

  if ( myMode == YACSGui_InputPanel::NewMode )
  {
    DEBTRACE("new mode");
    CatalogGroupBox->show();
    PortListGroupBox->hide();
    InPythonEditorGroupBox->hide();
    NameGroupBox->hide();
//     ComponentDefinitionLabel->hide();
//     myComponentDefinition->hide();
//     MethodName->hide();
//     myMethodName->hide();
    ExecutionGroupBox->hide();
    stringstream aName;
    string aType = getInputPanel()->getModule()->getNodeType(SALOMENODE);
    aName << aType << GuiContext::getCurrent()->getNewId(SALOMENODE);
    myNodeName->setText(aName.str());
    myNodeFullName->setText(aName.str());
    getInputPanel()->onApplyEnabled(false);
    mySNode = 0;
    updateState();
  }
  else
  {
    DEBTRACE("not new mode");
    CatalogGroupBox->hide();
    PortListGroupBox->show();
    NameGroupBox->show();
//     ComponentDefinitionLabel->show();
//     myComponentDefinition->show();
//     MethodName->show();
//     myMethodName->show();
    getInputPanel()->onApplyEnabled(true);
  }

  myMethodName->setReadOnly(true);

  if ( myMode == YACSGui_InputPanel::EditMode )
  {
    DEBTRACE("edit mode");
    myNodeName->setReadOnly(false);
    ExecutionGroupBox->hide();

    myInputPortsGroupBox->ShowBtn( YACSGui_PlusMinusGrp::UpBtn | YACSGui_PlusMinusGrp::DownBtn );
    myOutputPortsGroupBox->ShowBtn( YACSGui_PlusMinusGrp::UpBtn | YACSGui_PlusMinusGrp::DownBtn );
  } 
  else if ( myMode == YACSGui_InputPanel::RunMode )
  {
    DEBTRACE("run mode");
    myNodeName->setReadOnly(true);
    ExecutionGroupBox->show();

    myInputPortsGroupBox->HideBtn();
    myOutputPortsGroupBox->HideBtn();
  }
}

void YACSGui_ServiceNodePage::notifyNodeStatus( int theStatus )
{
  if ( myNodeState )
  {
    myNodeState->setPaletteForegroundColor(getColor(theStatus));
    myNodeState->setText(getText(theStatus));
  }
}

void YACSGui_ServiceNodePage::notifyNodeProgress()
{
  if ( myProgressBar )
  {
    if (!getNode()) return;
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

void YACSGui_ServiceNodePage::notifyInPortValues( std::map<std::string,std::string> theInPortName2Value )
{
  DEBTRACE("YACSGui_ServiceNodePage::notifyInPortValues");
  QStringList aValues;

  QStringList aPortNames;
  myInputPortsGroupBox->Table()->strings( 0, aPortNames );

  for ( QStringList::Iterator it = aPortNames.begin(); it != aPortNames.end(); ++it )
  {
    if ( (*it).isEmpty() ) continue;

    map<string,string>::iterator it1 = theInPortName2Value.find( (*it).latin1() );
    if ( it1 != theInPortName2Value.end() )
      aValues.append( QString((*it1).second.c_str()) );
  }

  if ( aPortNames.count() == aValues.count() )
    myInputPortsGroupBox->Table()->setStrings( 3, aValues );
}

void YACSGui_ServiceNodePage::notifyOutPortValues( std::map<std::string,std::string> theOutPortName2Value )
{
  DEBTRACE("YACSGui_ServiceNodePage::notifyOutPortValues");
  QStringList aValues;

  QStringList aPortNames;
  myOutputPortsGroupBox->Table()->strings( 0, aPortNames );

  for ( QStringList::Iterator it = aPortNames.begin(); it != aPortNames.end(); ++it )
  {
    if ( (*it).isEmpty() ) continue;

    map<string,string>::iterator it1 = theOutPortName2Value.find( (*it).latin1() );
    if ( it1 != theOutPortName2Value.end() )
      aValues.append( QString((*it1).second.c_str()) );
  }

  if ( aPortNames.count() == aValues.count() )
    myOutputPortsGroupBox->Table()->setStrings( 3, aValues );
}

void YACSGui_ServiceNodePage::checkModifications( bool& theWarnToShow, bool& theToApply )
{
  DEBTRACE("YACSGui_ServiceNodePage::checkModifications " << theWarnToShow <<","<<theToApply);
  if ( !getNode() || !theWarnToShow && !theToApply ) return;

  // 1) check if the content of the page is really modified (in compare with the content of engine object)
  bool isModified = false;
  ServiceNode* aServiceNode = dynamic_cast<ServiceNode*>( getNode() );
  if ( !aServiceNode ) return;

  if ( myNodeName->text().compare(getNodeName()) != 0) isModified = true;

  if ( !isModified )
  {
    if (aServiceNode->getComponent())
      if(QString::compare(myInstanceName->currentText(),aServiceNode->getComponent()->getInstanceName()) != 0)
        isModified = true;
  }

  if ( !isModified )
  {
    // read input data and input data stream ports values from the table
    YACSGui_Table* aTable = myInputPortsGroupBox->Table();
    aTable->stopEdit(true);
    
    QStringList aPortNames;
    QStringList aValues;
    
    aTable->strings( 0, aPortNames );
    aTable->strings( 3, aValues );
    
    int aRowId = 0;
    for ( QStringList::Iterator it = aPortNames.begin(); it != aPortNames.end(); ++it )
    {
      InPort* anInPort = aServiceNode->getInPort(*it);
      QString value=aValues[aRowId];
      if(value.isNull())
        value="< ? >";
      DEBTRACE(getPortValue( anInPort ) << "," <<value);

      if ( anInPort && getPortValue( anInPort ).compare(value) != 0 )
      {
	isModified = true;
	break;
      }
      aRowId++;
    }
  }

  //if ( !isModified )
  //  TODO: compare "Is in study" fields (not yet in use)

  if ( !isModified  && (myType == ServiceInline || myType == XMLNode) )
  {
    // check the code modifications in the built-in Python code editor
    QString aScript;
    if ( ServiceInlineNode* aSIN = dynamic_cast<ServiceInlineNode*>( getNode() ) ) aScript = QString(aSIN->getScript());
    else if ( XmlNode* aXN = dynamic_cast<XmlNode*>( getNode() ) ) aScript = QString(aXN->getScript());

#ifdef HAVE_QEXTSCINTILLA_H
    QString theText = _myTextEdit->text();
#else
    QString theText = myTextEdit->text();
#endif
    if ( theText.compare(aScript) != 0 ) isModified = true;
  }
    
  // 2) if yes, show a warning message: Apply or Cancel
  if ( isModified )
    if ( theWarnToShow )
    {
      theWarnToShow = false;
      if ( SUIT_MessageBox::warn2(getInputPanel()->getModule()->getApp()->desktop(),
				  tr("WRN_WARNING"),
				  tr("APPLY_CANCEL_MODIFICATIONS"),
				  tr("BUT_YES"), tr("BUT_NO"), 0, 1, 0) == 0 )
      {
	onApply();
	theToApply = true;
	if ( getInputPanel() ) getInputPanel()->emitApply(YACSGui_InputPanel::ServiceNodeId);
      }
      else
	theToApply = false;
    }
    else if ( theToApply )
    {
      onApply();
      if ( getInputPanel() ) getInputPanel()->emitApply(YACSGui_InputPanel::ServiceNodeId);
    }
}

void YACSGui_ServiceNodePage::onCatalogChanged( int )
{
  DEBTRACE("YACSGui_ServiceNodePage::onCatalogChanged");
  getInputPanel()->onApplyEnabled(false);

  // generate name of node if session catalog is active one. Name for proc catalog
  // is generated when node is selected in catalog view
  /*if ( !myProcRadioButton->isChecked() )
  {
    stringstream aName;
    string aType = getInputPanel()->getModule()->getNodeType( SALOMENODE );
    aName << aType << GuiContext::getCurrent()->getNewId( SALOMENODE );
    myNodeName->setText(aName.str());
  }*/
  
  updateState();
}

void YACSGui_ServiceNodePage::onCatalogMethodClicked( QListViewItem* it )
{
    if ( it )
  {
    if ( !myProcRadioButton->isChecked() )
    {
      // session catalog is used
      std::pair<std::string, std::string> compoServ = myServiceMap[it];
      getInputPanel()->onApplyEnabled(!compoServ.first.empty() && !compoServ.second.empty());
    }
    else 
    {
      // This block of code is commented because service nodes can be creatwed here only

      // proc catalog is used. Apply button should be enabled if selected item is not
      // "Nodes" or "Composed nodes" one.
      QListViewItem* aParent = it->parent();
      getInputPanel()->onApplyEnabled( aParent != 0 );

      // At the time service nodes can be created only so there is no point in 
      // regenerating default node name in accordance with its type
      // Update name
      if ( myProcNodesMap.count( it ) )
        {
          YACS::ENGINE::Node* aNode = myProcNodesMap[ it ];
          if ( aNode )
            {
              YACS::HMI::TypeOfElem aTypeId = ProcInvoc::getTypeOfNode( aNode );
              stringstream aName;
              string aType = getInputPanel()->getModule()->getNodeType( aTypeId );
              aName << aType << GuiContext::getCurrent()->getNewId( aTypeId );
              myNodeName->setText( aName.str() );
              myNodeFullName->setText( aName.str() );
              myNodeType->setText( aType.c_str() );
            }
        }
    }
  }
}

void YACSGui_ServiceNodePage::onBrowse()
{
  DEBTRACE("YACSGui_ServiceNodePage::onBrowse");
  QString fn = QFileDialog::getOpenFileName( QString::null,
					     tr( "XML-Files (*.xml);;All Files (*)" ),
					     this,
					     "load YACS scheme file dialog as catalog",
					     "Choose a filename to load"  );
  if ( fn.isEmpty() )
    return;
  
  myProcName = fn.latin1();
  
  myProcRadioButton->setEnabled(true);
  myProcRadioButton->setChecked(true);

  getInputPanel()->onApplyEnabled(false);
  updateState();
}

void YACSGui_ServiceNodePage::onApply()
{
  DEBTRACE("YACSGui_ServiceNodePage::onApply");
  if ( myMode == YACSGui_InputPanel::NewMode )
  {
    // --- NewMode: import a node or composed node of any type from catalog ---------

    GuiContext* aContext = GuiContext::getCurrent();
    if ( !aContext ) return;

    YACSGui_Module* aModule = getInputPanel()->getModule();
    if ( !aModule ) return;

    YACSGui_EditionTreeView * aETV = dynamic_cast<YACSGui_EditionTreeView*>(aModule->activeTreeView());
    if ( !aETV ) return;

    SubjectProc* aSchema = aContext->getSubjectProc();
    DEBTRACE("YACSGui_ServiceNodePage::onApply: "<< aSchema);

    YACS::ENGINE::Catalog* aCatalog = 0;
    if ( myProcRadioButton->isChecked() )
      {
        aCatalog = YACS::ENGINE::getSALOMERuntime()->loadCatalog( "proc", myProcName.c_str() );
        std::string errors=aCatalog->getErrors();
        if(errors != "")
          {

            std::string msg="The imported file ";
            msg=msg+myProcName+" has errors. Some nodes can be uncompletely built\n";
            LogViewer* log=new LogViewer(msg+errors,getInputPanel()->getModule()->getApp()->desktop(),"dialog",WDestructiveClose);
            log->show();
            getInputPanel()->getModule()->getApp()->logWindow()->putMessage(msg+errors );
          }
      }
    else
      aCatalog = aModule->getCatalog();
    
    QListViewItem* anItem = CatalogTree->selectedItem();

    SubjectServiceNode* aNewSNode = 0;
    if ( aSchema && aCatalog && anItem && mySCNode )
    {
      string compo;
      string service;
      YACS::HMI::TypeOfElem aTypeId = UNKNOWN;
      if ( !myProcRadioButton->isChecked() ) 
      {
        if ( myServiceMap.find( anItem ) == myServiceMap.end() )
          return;
         
        pair<string, string> compoServ = myServiceMap[anItem];
        compo = compoServ.first;
        service = compoServ.second;
        aTypeId = SALOMENODE;
      }
      else 
      {
        if ( anItem->text( 0 ).isNull() )
          return;

        compo = "";
        service = anItem->text( 0 ).latin1();

        if ( !myProcNodesMap.count( anItem ) )
          return; // nonsense

        YACS::ENGINE::Node* aNode = myProcNodesMap[ anItem ];
        
        // Add node in catalog map if necessary. In the other case it will not be
        // found in CommandAddNodeFromCatalog method
        if ( !aCatalog->_nodeMap.count( service ) && 
             !aCatalog->_composednodeMap.count( service ) )
        {
          aCatalog->_nodeMap[ service ] = aNode;
        }

        YACS::ENGINE::ServiceNode* aServiceNode = 
          dynamic_cast< YACS::ENGINE::ServiceNode* >( aNode );
        if ( aServiceNode )
          aServiceNode->setComponent( 0 );

         aTypeId = ProcInvoc::getTypeOfNode( aNode );
      }

      myComponentName = compo;
      myMethodName->setText( service );

      Proc* aProc = aContext->getProc();
      DEBTRACE("YACSGui_ServiceNodePage::onApply: " << aProc);
      if ( !aProc ) return;

      YACSGui_Graph* aGraph = aModule->getGraph( aProc );
      if ( !aGraph ) return;

      stringstream aName;
      string aType = aModule->getNodeType( aTypeId );
      aName << aType << aContext->getNewId( aTypeId );

      // ---> block for correct node (engine - subject - presentation) creation
      SubjectNode* aCreatedSNode = 0;
      if ( mySCNode )
      {
	if ( SubjectSwitch* aSwitch = dynamic_cast<SubjectSwitch*>(mySCNode) )
	{
	  map<int, SubjectNode*> bodyMap = aSwitch->getBodyMap();
	  int aSwCase = 0;
	  if (bodyMap.empty()) aSwCase = 1;
	  else
	  {
	    map<int, SubjectNode*>::reverse_iterator rit = bodyMap.rbegin();
	    aSwCase = (*rit).first + 1;
	  }
	  aCreatedSNode = aSwitch->addNode( aCatalog, compo, service, aName.str(), aSwCase);
	}
	else if ( SubjectForLoop* aForLoop = dynamic_cast<SubjectForLoop*>(mySCNode) )
	{
	  aCreatedSNode = aModule->createBody( SALOMENODE, mySCNode, aCatalog, compo, service );
	}
	else if ( SubjectForEachLoop* aForEachLoop = dynamic_cast<SubjectForEachLoop*>(mySCNode) )
	{
	  aCreatedSNode = aModule->createBody( SALOMENODE, mySCNode, aCatalog, compo, service );
	}
	else if ( SubjectWhileLoop* aWhileLoop = dynamic_cast<SubjectWhileLoop*>(mySCNode) )
	{
	  aCreatedSNode = aModule->createBody( SALOMENODE, mySCNode, aCatalog, compo, service );
	}
	else if ( SubjectProc* aSchema = dynamic_cast<SubjectProc*>(mySCNode) )
	{
          DEBTRACE("YACSGui_ServiceNodePage::onApply: " << aSchema);
	  aCreatedSNode = aSchema->addNode( aCatalog, compo, service, aName.str() );
	}
	else if ( SubjectBloc* aBloc = dynamic_cast<SubjectBloc*>(mySCNode) )
	{
	  aCreatedSNode = aBloc->addNode( aCatalog, compo, service, aName.str() );
	  if( SubjectComposedNode* aParent = dynamic_cast<SubjectComposedNode*>(aBloc->getParent()) )
	    aGraph->update(aBloc->getNode(), aParent);
	}
      }
      else
      {  
	aCreatedSNode = aSchema->addNode( aCatalog, compo, service, aName.str() );
      }
      // <---

      if ( SubjectBloc* aSB = dynamic_cast<SubjectBloc*>(mySCNode) )
	aSB->update( EDIT, 0, aSB );
      
      aNewSNode = dynamic_cast<SubjectServiceNode*>(aCreatedSNode);
      ServiceNode* aNewNode =0;
      if ( aNewSNode )
        aNewNode = dynamic_cast<ServiceNode*>(aNewSNode->getNode());
      ComponentInstance* aComp = 0;
      if ( aNewNode )
        aComp = aNewNode->getComponent();
      SubjectComponent* aSComp = 0;
      if ( aComp ) // find the component subject in the current GuiContext
        if ( aContext->_mapOfSubjectComponent.find(aComp) != aContext->_mapOfSubjectComponent.end() )
          aSComp = aContext->_mapOfSubjectComponent[aComp];
      
      if ( aSComp )
        {
          SubjectContainer* aSCont = 0;
          SalomeContainer* aCont = dynamic_cast<SalomeContainer*>(aComp->getContainer());
          if ( !aCont )
            { // container is not found
              // 1) try to find the first container with the suitable type (i.e. Salome container)
              //    in the current GuiContext
              map<Container*, SubjectContainer*>::iterator itCont = aContext->_mapOfSubjectContainer.begin();
              for ( ; itCont!=aContext->_mapOfSubjectContainer.end(); itCont++ )
                {
                  SalomeContainer* aSalomeCont = dynamic_cast<SalomeContainer*>( (*itCont).first );
                  if ( aSalomeCont )
                    {
                      aSCont = (*itCont).second;
                      aCont = aSalomeCont;
                      break;
                    }
                }

              if ( !aSCont )
                {
                  // 2) create a new default Salome container and subject for it
                  string aDefContainer = "DefaultContainer";
                  if ( !aProc->containerMap.count(aDefContainer) )
                    {
                      aSCont = aSchema->addContainer(aDefContainer);
                      aCont = dynamic_cast<SalomeContainer*>(aSCont->getContainer());
                    }
                }
            }
          else
            { //container is found => try to get its subject from the current GuiContext
              if ( aContext->_mapOfSubjectContainer.find(aCont) != aContext->_mapOfSubjectContainer.end() )
                aSCont = aContext->_mapOfSubjectContainer[aCont];
            }
	
          if ( aSCont )
            {
              YACSGui_ContainerViewItem* aContainerItem = 0;

              // get the "Containers" label view item
              YACSGui_LabelViewItem* aContainersL = 
                dynamic_cast<YACSGui_LabelViewItem*>(aETV->firstChild()->firstChild()->nextSibling()->nextSibling()->nextSibling());
              if ( !aContainersL || aContainersL->text(0).compare(QString("Containers")) ) return;
          
              YACSGui_ContainerViewItem* anAfter = 0;
              if ( QListViewItem* aChild = aContainersL->firstChild() )
                {
                  while( aChild )
                    {
                      if ( anAfter = dynamic_cast<YACSGui_ContainerViewItem*>(aChild) )
                        if ( anAfter->getContainer() == aCont )
                          {
                            aContainerItem = anAfter;
                            break;
                          }
                      aChild = aChild->nextSibling();
                    }
                }

              if ( aContainerItem )
                {
                  aContainerItem->addComponentItem(aSComp);
                  aSComp->associateToContainer( aSCont );
                  aNewSNode->associateToComponent(aSComp);
                }
            }
        }

      aModule->temporaryExport();
    }  

    // --- commented to avoid to close the NewMode panel    
//     aModule->setGuiMode(YACSGui_Module::EditMode);
//     setMode(YACSGui_InputPanel::EditMode);
    if ( aNewSNode ) setSNode(aNewSNode);

    if ( aModule->activeTreeView() )
      aModule->activeTreeView()->syncPageTypeWithSelection();

    return;
  }

  // --- not NewMode: edition of an existing Service Node -----------

  // Rename a node
  if ( myNodeName )
    setNodeName( myNodeName->text() );

  if ( ServiceNode* aServiceNode = dynamic_cast<ServiceNode*>( getNode() ) )
  {
    SubjectServiceNode* aSSNode = dynamic_cast<SubjectServiceNode*>(mySNode);
    // change component
    if (ComponentInstance* aPrevComponent = aServiceNode->getComponent())
      {
	SubjectComponent* aPrevSComponent = GuiContext::getCurrent()->_mapOfSubjectComponent[aPrevComponent];

        if(QString::compare(myInstanceName->currentText(),aServiceNode->getComponent()->getInstanceName()) != 0)
          {
            ComponentInstance* compo = 0;
            Proc* aProc = GuiContext::getCurrent()->getProc();
            if (aProc)
              {
                map<pair<string,int>,ComponentInstance*>::const_iterator it = aProc->componentInstanceMap.begin();
                for(; it!=aProc->componentInstanceMap.end(); ++it)
                  {
                    ComponentInstance *inst=(*it).second;
                    if (! QString::compare(inst->getInstanceName(), myInstanceName->currentText())) // if same instance name
                      {
                        compo = inst;
                        break;
                      }
                  }
              }
            if (compo)
              {
                SubjectComponent* aSComp = GuiContext::getCurrent()->_mapOfSubjectComponent[compo];
                if ( aSSNode && aSComp ) aSSNode->associateToComponent(aSComp);
                setComponent(compo);
              }
          }
      }
    else
      {
	SubjectComponent* aSComp = GuiContext::getCurrent()->_mapOfSubjectComponent[myComponent];
	if ( aSSNode && aSComp ) aSSNode->associateToComponent(aSComp);
      }
  }

  // Reset the list of input ports
  setInputPorts();
  
  // Reset the list of output ports
  setOutputPorts();

  // Reset Python function/script
  string aScript = scriptText().isEmpty() ? string("") : scriptText().latin1();
  if ( ServiceInlineNode* aSIN = dynamic_cast<ServiceInlineNode*>( getNode() ) ) aSIN->setScript( aScript );
  else if ( XmlNode* aXN = dynamic_cast<XmlNode*>( getNode() ) ) aXN->setScript( aScript );

  updateBlocSize();
}

void YACSGui_ServiceNodePage::buildTree( YACS::ENGINE::Node* theNode,
                                         QListViewItem* nodeLabel,
                                         QListViewItem* compoNodeLabel )
{
  if ( !theNode )
    return;

  // --- Create item corresponding to the node
  QString aName( theNode->getName().c_str() );
  if ( aName.isNull())
    return;

  QListViewItem* theParent = nodeLabel;
  if (YACS::ENGINE::ComposedNode* aCompNode = dynamic_cast<YACS::ENGINE::ComposedNode*>(theNode))
    {
      theParent = compoNodeLabel;
      std::list<Node*> aNodes = aCompNode->edGetDirectDescendants();
      std::list<Node*>::iterator anIter;
      for ( anIter = aNodes.begin(); anIter != aNodes.end(); ++anIter )
        buildTree( *anIter, nodeLabel, compoNodeLabel );
    }

  QListViewItem* anItem = new QListViewItem( theParent, aName );
  anItem->setPixmap( 0, YACSGui_NodeViewItem::icon( theNode ) );
  myProcNodesMap[ anItem ] = theNode;
}


void YACSGui_ServiceNodePage::updateState()
{
  DEBTRACE("YACSGui_ServiceNodePage::updateState");
  if ( myMode == YACSGui_InputPanel::NewMode )
    {
      //DEBTRACE("New mode");
      //myNodeType->setText( tr( "SALOME_SERVICE_NODE" ) ); // SALOME service node
      CatalogTree->clear();
      CatalogTree->setRootIsDecorated( TRUE );

      YACS::ENGINE::Catalog *_currentCatalog =0;
      if ( myProcRadioButton->isChecked() )
        {
          _currentCatalog = YACS::ENGINE::getSALOMERuntime()->loadCatalog( "proc", myProcName.c_str() );
          std::string errors=_currentCatalog->getErrors();
          if(errors != "")
            {
              std::string msg="The imported file ";
              msg=msg+myProcName+" has errors. Some nodes can be uncompletely built\n";
              LogViewer* log=new LogViewer(msg+errors,getInputPanel()->getModule()->getApp()->desktop(),"dialog",WDestructiveClose);
              log->show();
              getInputPanel()->getModule()->getApp()->logWindow()->putMessage(msg+errors );
            }
        }
      else
        _currentCatalog = getInputPanel()->getModule()->getCatalog();

      if ( _currentCatalog )
        {
          myProcNodesMap.clear();

          // --- nodes and composed nodes
          std::map< std::string, Node* >& aNodeMap = _currentCatalog->_nodeMap;
          std::map< std::string, ComposedNode* >& aCompMap = _currentCatalog->_composednodeMap;
          QListViewItem* aNodeRoot = new QListViewItem( CatalogTree, tr( "NODES" ) );
          QListViewItem* aCompRoot = new QListViewItem( CatalogTree, tr( "COMPOSED_NODES" ) );

          if ( aCompMap.size() )
            {
              std::map< std::string, ComposedNode* >::iterator cIter;
              for ( cIter = aCompMap.begin(); cIter != aCompMap.end(); ++cIter )
                {
                  YACS::ENGINE::Node* aNode = cIter->second;
                  buildTree( aNode, aNodeRoot, aCompRoot );
                }
            }
          if ( aNodeMap.size() )
            {
              std::map< std::string, Node* >::iterator nIter;
              for ( nIter = aNodeMap.begin(); nIter != aNodeMap.end(); ++nIter )
                {
                  YACS::ENGINE::Node* aNode = nIter->second;
                  buildTree( aNode, aNodeRoot, aCompRoot );
                }
            }

          // --- composed nodes

          // --- component and services
          map<string, YACS::ENGINE::ComponentDefinition*>::const_iterator itComp; 
          for (itComp = _currentCatalog->_componentMap.begin();
               itComp != _currentCatalog->_componentMap.end();
               ++itComp)
            {
              string compoName = (*itComp).first;
              YACS::ENGINE::ComponentDefinition* compo = (*itComp).second;
              QListViewItem *item = new QListViewItem(CatalogTree, compoName.c_str());
              myServiceMap[item] = pair<string,string>(compoName,"");
              map<string, YACS::ENGINE::ServiceNode*>::iterator itServ;
              for (itServ = compo->_serviceMap.begin(); itServ != compo->_serviceMap.end(); ++itServ)
                {
                  string serviceName = (*itServ).first;
                  YACS::ENGINE::ServiceNode* service = (*itServ).second;
                  QListViewItem *sitem = new QListViewItem(item, serviceName.c_str());
                  myServiceMap[sitem] = pair<string,string>(compoName,serviceName);
                }
            }
        }
      return;
    }
  //DEBTRACE("not new mode");

  if ( myType != SALOMEService )
  {
    //DEBTRACE("myType != SALOMEService");
    myInputPortsGroupBox->ShowBtn( YACSGui_PlusMinusGrp::AllBtn );
    myInputPortsGroupBox->HideBtn( YACSGui_PlusMinusGrp::SelectBtn );
    myInputPortsGroupBox->EnableBtn( YACSGui_PlusMinusGrp::UpBtn | YACSGui_PlusMinusGrp::DownBtn |
				     YACSGui_PlusMinusGrp::PlusBtn | YACSGui_PlusMinusGrp::MinusBtn |
				     YACSGui_PlusMinusGrp::InsertBtn );

    myOutputPortsGroupBox->ShowBtn( YACSGui_PlusMinusGrp::AllBtn );
    myOutputPortsGroupBox->HideBtn( YACSGui_PlusMinusGrp::SelectBtn );
    myOutputPortsGroupBox->EnableBtn( YACSGui_PlusMinusGrp::UpBtn | YACSGui_PlusMinusGrp::DownBtn |
				      YACSGui_PlusMinusGrp::PlusBtn | YACSGui_PlusMinusGrp::MinusBtn |
				      YACSGui_PlusMinusGrp::InsertBtn );
  }
  else
  {
    //DEBTRACE("myType == SALOMEService");
    myInputPortsGroupBox->HideBtn( YACSGui_PlusMinusGrp::PlusBtn | YACSGui_PlusMinusGrp::MinusBtn |
				   YACSGui_PlusMinusGrp::InsertBtn | YACSGui_PlusMinusGrp::SelectBtn );
    myInputPortsGroupBox->EnableBtn( YACSGui_PlusMinusGrp::UpBtn | YACSGui_PlusMinusGrp::DownBtn );

    myOutputPortsGroupBox->HideBtn( YACSGui_PlusMinusGrp::PlusBtn | YACSGui_PlusMinusGrp::MinusBtn |
				    YACSGui_PlusMinusGrp::InsertBtn | YACSGui_PlusMinusGrp::SelectBtn );
    myOutputPortsGroupBox->EnableBtn( YACSGui_PlusMinusGrp::UpBtn | YACSGui_PlusMinusGrp::DownBtn );
  }

  if ( (myType == ServiceInline || myType == XMLNode) && InPythonEditorGroupBox )
  {
    // show built-in Python code editor
    InPythonEditorGroupBox->show();
    connect( mySearch, SIGNAL(returnPressed()), this, SLOT(onSearch()) );
  }
  else if ( InPythonEditorGroupBox )
  {
    // hide built-in Python code editor
    InPythonEditorGroupBox->hide();
    disconnect( mySearch, SIGNAL(returnPressed()), this, SLOT(onSearch()) );
  }
  
  // Clear input ports table and output ports tables, if they are not empty
  if ( myInputPortsGroupBox )
  {
    myInputPortsGroupBox->Table()->setNumRows( 0 );
    myInputPortsGroupBox->EnableBtn( YACSGui_PlusMinusGrp::UpBtn | YACSGui_PlusMinusGrp::DownBtn );
  }
  if ( myOutputPortsGroupBox )
  {
    myOutputPortsGroupBox->Table()->setNumRows( 0 );
    myOutputPortsGroupBox->EnableBtn( YACSGui_PlusMinusGrp::UpBtn | YACSGui_PlusMinusGrp::DownBtn );
  }
  
  // Set node name
  if ( myNodeName ) myNodeName->setText( getNodeName() );
  
  // Set node full name (read only)
  if ( myNodeFullName )
    myNodeFullName->setText( getNode()->getRootNode()->getChildName(getNode()) );
  
  // Set node type: SALOME or CORBA service
  switch (myType)
  {
  case SALOMEService:
    myNodeType->setText( tr( "SALOME_SERVICE_NODE" ) ); break;
  case CORBAService:
    myNodeType->setText( tr( "CORBA_SERVICE_NODE" ) );  break;
  case CPPNode:
    myNodeType->setText( tr( "CPP_NODE" ) );            break;
  case ServiceInline:
    myNodeType->setText( tr( "SERVICE_INLINE_NODE" ) ); break;
  case XMLNode:
    myNodeType->setText( tr( "XML_NODE" ) );            break;
  default:                                              break;
  }

  // Set component instance and services
  if( myComponentDefinition && myMethodName )
    updateServices();

  // --- always fill the table seems safer (salome nodes)
  // if ( myType == CORBAService || myType == CPPNode /*|| myType == ServiceInline || myType == XMLNode*/ )
  {
    // Fill the table of input ports of the node
    fillInputPortsTable( getNode() );
    
    // Fill the table of output ports of the node
    fillOutputPortsTable( getNode() );
  }
}

void YACSGui_ServiceNodePage::onNodeNameChanged( const QString& theName )
{
  if ( myNodeFullName )
  {
    QString anOldName = myNodeFullName->text();
    int aRDotId = anOldName.findRev('.');
    myNodeFullName->setText( anOldName.replace( aRDotId+1, anOldName.length()-(aRDotId+1), theName ) );
  }
}

void YACSGui_ServiceNodePage::onInstanceNameChanged( const QString& theInstanceName )
{
  Proc* aProc = GuiContext::getCurrent()->getProc();
  if ( !aProc ) return;

  ComponentInstance* compo = 0;
  map<pair<string,int>,ComponentInstance*>::const_iterator it = aProc->componentInstanceMap.begin();
  for(; it!=aProc->componentInstanceMap.end(); ++it)
    {
      ComponentInstance *inst=(*it).second;
      if (! QString::compare(inst->getInstanceName(), theInstanceName)) // if same instance name
        {
          compo = inst;
          break;
        }
    }
  if (!compo) return;
  
  YACSGui_ComponentPage* aCompPage = 
    dynamic_cast<YACSGui_ComponentPage*>( getInputPanel()->getPage( YACSGui_InputPanel::ComponentId ) );
  if ( aCompPage )
    aCompPage->setSComponent( GuiContext::getCurrent()->_mapOfSubjectComponent[compo], false );
}

void YACSGui_ServiceNodePage::onValueChanged( int theRow, int theCol )
{
  DEBTRACE("YACSGui_ServiceNodePage::onValueChanged " << theRow << "," << theCol);
  if ( YACSGui_Table* aTable = ( YACSGui_Table* )sender() )
    if ( theCol == 2 ) // the value type of the port was changed
      setValueCellValidator( aTable, theRow );
}

void YACSGui_ServiceNodePage::hideEvent( QHideEvent* )
{
  myPara = 0;
  myIndex = 0;
#ifdef HAVE_QEXTSCINTILLA_H
//   _myTextEdit->removeSelection(); // --- see if replacement ?
#else
  myTextEdit->removeSelection();
#endif
}

void YACSGui_ServiceNodePage::onSearch()
{
#ifdef HAVE_QEXTSCINTILLA_H
  if ( _myTextEdit && mySearch )
  {
    if ( _myTextEdit->findFirst( mySearch->text(), true, false, true, &myPara, &myIndex ) )
    {
      _myTextEdit->setSelection( myPara, myIndex, myPara, myIndex + mySearch->text().length() );
      myIndex += mySearch->text().length();
    } else {
      myPara = 0;
      myIndex = 0;
    }
  }
#else
  if ( myTextEdit && mySearch )
  {
    if ( myTextEdit->find( mySearch->text(), true, false, true, &myPara, &myIndex ) )
    {
      myTextEdit->setSelection( myPara, myIndex, myPara, myIndex + mySearch->text().length() );
      myIndex += mySearch->text().length();
    } else {
      myPara = 0;
      myIndex = 0;
    }
  }
#endif
}

void YACSGui_ServiceNodePage::updateServices()
{
  if ( !myComponent )
  {
    if ( myComponentDefinition ) myComponentDefinition->clear();
    if ( myMethodName ) myMethodName->clear();
    return;
  }

  YACS::ENGINE::ServiceNode* aServiceNode = dynamic_cast<ServiceNode*>( getNode() );
  if( !aServiceNode ) return;

  if( myComponentName.isNull() )
    myComponentName = aServiceNode->getComponent()->getCompoName();

  myComponentDefinition->setText( myComponentName );
  myMethodName->setText( aServiceNode->getMethod() );
  fillInstanceNames();
  myInstanceName->setCurrentText( aServiceNode->getComponent()->getInstanceName() );

  fillInputPortsTable( aServiceNode );
  fillOutputPortsTable( aServiceNode );
}

void YACSGui_ServiceNodePage::fillInstanceNames()
{
  myInstanceName->clear();
  YACS::ENGINE::ServiceNode* aServiceNode = dynamic_cast<ServiceNode*>( getNode() );
  if( !aServiceNode ) return;
  ComponentInstance* compo = aServiceNode->getComponent();
  if (!compo) return;

  if (compo->getKind() == "Salome")
    myInstanceName->setEnabled(true);
  else
    myInstanceName->setEnabled(false);  

  Proc* aProc = GuiContext::getCurrent()->getProc();
  if ( !aProc ) return;

  map<pair<string,int>,ComponentInstance*>::const_iterator it = aProc->componentInstanceMap.begin();
  for(; it!=aProc->componentInstanceMap.end(); ++it)
    {
      ComponentInstance *inst=(*it).second;
      QString aComponentName = inst->getCompoName();
      if (! QString::compare(compo->getCompoName(), aComponentName)) // if same component name
        myInstanceName->insertItem( QString(inst->getInstanceName()));
    }
}

void YACSGui_ServiceNodePage::fillInputPortsTable( YACS::ENGINE::Node* theNode )
{
  DEBTRACE("YACSGui_ServiceNodePage::fillInputPortsTable");
  if( !theNode )
    return;

  QStringList aPortNames;
  QStringList aPortTypes;
  QStringList aValueTypes;
  QStringList aValues;

  QValueList<bool> aReadOnlyFlags;

  YACSGui_Table* aTable = myInputPortsGroupBox->Table();
  QStringList aPTCB = aTable->Params( 0, 1 );

  list<InPort*> anInPortsEngine = theNode->getSetOfInPort();
  list<InPort*>::iterator anInPortsIter = anInPortsEngine.begin();
  for( ;anInPortsIter!=anInPortsEngine.end();anInPortsIter++)
  {
    InPort* anInPort = *anInPortsIter;

    // Collect port names in the list
    aPortNames.append( anInPort->getName() );
    
    // Port type
    if ( InputPort* anInputP = dynamic_cast<InputPort*>(anInPort) )
    {
      aPortTypes.append( aPTCB[0] );

      int nbLinks = anInPort->edGetNumberOfLinks();
      aReadOnlyFlags.append( nbLinks > 0 );
    }
    //else if ( InputBasicStreamPort* aBasicStreamP = dynamic_cast<InputBasicStreamPort*>(anInPort) )
    //{ // there is no possibility to identify BASIC data stream port in engine now => take default value from combo box
    //  aPortTypes.append( aPTCB[1] ); 
    //}
    else if ( InputCalStreamPort* aCalStreamP = dynamic_cast<InputCalStreamPort*>(anInPort) )
    {
      aPortTypes.append( aPTCB[2] );
      aReadOnlyFlags.append( true );
    }
    //else if ( InputPalmStreamPort* aPalmStreamP = dynamic_cast<InputPalmStreamPort*>(anInPort) )
    //{ // there is no possibility to identify PALM data stream port in engine now => take default value from combo box
    //  aPortTypes.append( aPTCB[3] );
    //}
    else if ( InputDataStreamPort* aCalStreamP = dynamic_cast<InputDataStreamPort*>(anInPort) )
    {
      aPortTypes.append( aPTCB[2] );
      aReadOnlyFlags.append( true );
    }
    else
    { // take default value from combo box
      aPortTypes.append( aPTCB[0] );
      aReadOnlyFlags.append( true );
    }

    // Value type
    aValueTypes.append( getPortType( anInPort ) );

    // Value
    aValues.append( getPortValue( anInPort ) );
  }

  // Fill "Port name" column
  aTable->setStrings( 0, aPortNames, true );

  // Fill "Port type" column
  aTable->setStrings( 1, aPortTypes, true );

  // Fill "Value type" column
  aTable->setStrings( 2, aValueTypes, true );

  if ( myType == SALOMEService )
    // Set the valid cell type for the input ports values
    for ( int i=0; i<aTable->numRows(); i++ )
      setValueCellValidator( aTable, i );

  // Fill "Value" column
  aTable->setStrings( 3, aValues, true );

  if ( myType == SALOMEService )
  {
    // Set all columns read only (except "Value" column)
    aTable->setReadOnly( -1, 0, true );
    aTable->setReadOnly( -1, 1, true );
    aTable->setReadOnly( -1, 2, true );
  }

  // Set "Value" column read only (for linked ports)
  for ( int i = 0, n = aTable->numRows(); i < n; i++ )
  {
    aTable->setDefValue( i, 2, aValueTypes[i] );
    aTable->setReadOnly( i, 3, aReadOnlyFlags[ i ] );
  }
  
  if ( aPortNames.count() > 1 )
    myInputPortsGroupBox->EnableBtn( YACSGui_PlusMinusGrp::UpBtn | YACSGui_PlusMinusGrp::DownBtn );

}

void YACSGui_ServiceNodePage::fillOutputPortsTable( YACS::ENGINE::Node* theNode )
{
  if( !theNode )
    return;

  QStringList aPortNames;
  QStringList aPortTypes;
  QStringList aValueTypes;
  QStringList aValues;
  QStringList anIsInStudy;
  
  YACSGui_Table* aTable = myOutputPortsGroupBox->Table();
  QStringList aPTCB = aTable->Params( 0, 1 );

  list<OutPort*> anOutPortsEngine = theNode->getSetOfOutPort();
  list<OutPort*>::iterator anOutPortsIter = anOutPortsEngine.begin();
  for( ;anOutPortsIter!=anOutPortsEngine.end();anOutPortsIter++)
  {
    OutPort* anOutPort = *anOutPortsIter;

    // Collect port names in the list
    aPortNames.append( anOutPort->getName() );
    
    // Port type
    if ( OutputPort* anOutputP = dynamic_cast<OutputPort*>(anOutPort) )
    {
      aPortTypes.append( aPTCB[0] );
    }
    //else if ( OutputBasicStreamPort* aBasicStreamP = dynamic_cast<OutputBasicStreamPort*>(anOutPort) )
    //{ // there is no possibility to identify BASIC data stream port in engine now => take default value from combo box
    //  aPortTypes.append( aPTCB[1] ); 
    //}
    else if ( OutputCalStreamPort* aCalStreamP = dynamic_cast<OutputCalStreamPort*>(anOutPort) )
    {
      aPortTypes.append( aPTCB[2] );
    }
    //else if ( OutputPalmStreamPort* aPalmStreamP = dynamic_cast<OutputPalmStreamPort*>(anOutPort) )
    //{ // there is no possibility to identify PALM data stream port in engine now => take default value from combo box
    //  aPortTypes.append( aPTCB[3] );
    //}
    else if ( OutputDataStreamPort* aCalStreamP = dynamic_cast<OutputDataStreamPort*>(anOutPort) )
    {
      aPortTypes.append( aPTCB[2] );
    }
    else
    { // take default value from combo box
      aPortTypes.append( aPTCB[0] );
    }
      
    // Value type
    aValueTypes.append( getPortType( anOutPort ) );

    // Value
    aValues.append( getPortValue( anOutPort ) );

    // Is in study
    // There is no a such parameter either in engine or GUI at the current moment.
    // TODO: we need to implement it at GUI side.
    // As a temporary solution we set the default value
    anIsInStudy.append( aTable->defValue( 0, 4 ) );
  }

  // Fill "Port name" column
  aTable->setStrings( 0, aPortNames, true );

  // Fill "Port type" column
  aTable->setStrings( 1, aPortTypes, true );

  // Fill "Value type" column
  aTable->setStrings( 2, aValueTypes, true );

  // Fill "Value" column
  aTable->setStrings( 3, aValues, true );

  // Fill "Is in study" column
  aTable->setStrings( 4, anIsInStudy, true );

  if ( myType == SALOMEService )
  {
    // Set all columns read only
    aTable->setReadOnly( -1, 0, true );
    aTable->setReadOnly( -1, 1, true );
    aTable->setReadOnly( -1, 2, true );
    aTable->setReadOnly( -1, 3, true );
  }

  for ( int i = 0, n = aTable->numRows(); i < n; i++ )
    aTable->setDefValue( i, 2, aValueTypes[i] );

  if ( aPortNames.count() > 1 )
    myOutputPortsGroupBox->EnableBtn( YACSGui_PlusMinusGrp::UpBtn | YACSGui_PlusMinusGrp::DownBtn );
}

QString YACSGui_ServiceNodePage::getPortType( YACS::ENGINE::Port* thePort ) const
{
  QString aType;

  YACSGui_Table* aTable = 0;
  if ( dynamic_cast<InPort*>(thePort) )
    aTable = myInputPortsGroupBox->Table();
  else if ( dynamic_cast<OutPort*>(thePort) )
    aTable = myOutputPortsGroupBox->Table();

  if ( !aTable ) return aType;

  QStringList aVTCB = aTable->Params( 0, 2 );

  if ( DataPort* aDataPort = dynamic_cast<DataPort*>(thePort) )
  {
    DynType aDType = aDataPort->edGetType()->kind();
    if ( aDType > NONE && aDType < Objref ) //if (aDType != NONE)
      aType = aVTCB[aDType-1];
    else
      aType = QString(aDataPort->edGetType()->name());
    //SCRUTE(aType);
  }
  
  return aType;
}

void YACSGui_ServiceNodePage::setInputPorts()
{
  DEBTRACE("YACSGui_ServiceNodePage::setInputPorts");
  if ( !myInputPortsGroupBox ) return;

  ServiceNode* aNode = dynamic_cast<ServiceNode*>( getNode() );
  if ( !aNode ) return;

  myInputPortsGroupBox->Table()->stopEdit(true);

  if ( myType != SALOMEService )
  {
    // remove old ports
    list<InPort*> anInPortsEngine = aNode->getSetOfInPort();
    list<InPort*>::iterator anInPortsIter = anInPortsEngine.begin();
    for( ;anInPortsIter!=anInPortsEngine.end();anInPortsIter++)
    {
      mySNode->update( REMOVE,
		       ( dynamic_cast<InputPort*>(*anInPortsIter) ? INPUTPORT : INPUTDATASTREAMPORT ),
		       GuiContext::getCurrent()->_mapOfSubjectDataPort[*anInPortsIter] );
      
      //aNode->edRemovePort(*anInPortsIter);
      // remove port subject (and all subject links connected to this port)
      mySNode->destroy( GuiContext::getCurrent()->_mapOfSubjectDataPort[*anInPortsIter] );
    }
    
    // create a set of new input ports
    YACS::ENGINE::Catalog* aCatalog = getInputPanel()->getModule()->getCatalog();
    if ( aCatalog && aNode->getComponent()
	 &&
	 aCatalog->_componentMap.count(aNode->getComponent()->getCompoName()) )
    {
      ComponentDefinition* aCompodef = aCatalog->_componentMap[aNode->getComponent()->getCompoName()];
      if ( aCompodef && aCompodef->_serviceMap.count(aNode->getMethod()) )
      	if ( ServiceNode* aNodeToClone = aCompodef->_serviceMap[aNode->getMethod()] )
	{
	  list<InPort*> anInPortsClone = aNodeToClone->getSetOfInPort();
	  list<InPort*>::iterator anInPortsIter = anInPortsClone.begin();
	  for( ;anInPortsIter!=anInPortsClone.end();anInPortsIter++)
	    if ( InputPort* anIP = dynamic_cast<InputPort*>(*anInPortsIter) ) // Data Flow port
	      dynamic_cast<SubjectElementaryNode*>(mySNode)->addInputPort( aCatalog,
									   portValueType(anIP->edGetType()),
									   anIP->getName() );
	    else if ( InputDataStreamPort* anIDSP = dynamic_cast<InputDataStreamPort*>(*anInPortsIter) ) // Data Stream port
	      dynamic_cast<SubjectElementaryNode*>(mySNode)->addIDSPort( aCatalog,
									 portValueType(anIDSP->edGetType()),
									 anIDSP->getName() );
	}
    }
  }
  
  // read input data and input data stream ports from the table
  YACSGui_Table* aTable = myInputPortsGroupBox->Table();
  
  QStringList aPortNames;
  QStringList aValues;

  aTable->strings( 0, aPortNames );
  aTable->strings( 3, aValues );

  // initialize created input ports with values
  int aRowId = 0;
  for ( QStringList::Iterator it = aPortNames.begin(); it != aPortNames.end(); ++it,++aRowId )
  {
    if ( (*it).isEmpty() ) continue;

    if ( aTable->intValueCombo( 1, aRowId ) == 0 ) // Data Flow port
    {
      InputPort* aIDP;
      try {
	aIDP = aNode->getInputPort(*it);
      }
      catch (YACS::Exception& ex) {
	continue;
      }
      if ( !aIDP )
	continue;

      TypeCode* aTC = aIDP->edGetType();
      if ( !aTC )
	continue;

#ifdef _DEVDEBUG_
        if(!aValues[aRowId].isNull())
          std::cerr << aValues[aRowId] << std::endl;
        else
          std::cerr << "Qnull" << std::endl;
#endif

      if( aValues[aRowId] == "< ? >")
        continue;

      // initialize new created input data port
      double d;
      bool ok;
      switch ( aTC->kind() )
      {
      case Double:
        d=aValues[aRowId].toDouble(&ok);
        if(ok)
	  aIDP->edInit(d);
	break;
      case Int:
	aIDP->edInit( aValues[aRowId].toInt() );
	break;
      case String:
	aIDP->edInit( aValues[aRowId].latin1() );
	break;
      case Bool:
	aIDP->edInit( aValues[aRowId].compare( aTable->Params(aRowId,3)[0] ) ? false : true );
	break;
      case Objref:
	//aIDP->edInit( "" ); // TODO : create an Any* with corresponding type and initialize with it
	break;
      case Sequence:
	//aIDP->edInit( "" ); // TODO : create an Any* (i.e. SequenceAny*) with corresponding type and initialize with it
	break;
      case Array:
	//aIDP->edInit( "" ); // TODO : create an Any* (i.e. ArrayAny*) with corresponding type and initialize with it
	break;
      case Struct:
	//aIDP->edInit( "" ); // TODO : create an Any* with corresponding type and initialize with it
	break;
      default:
	break;
      }

      mySNode->update( EDIT, INPUTPORT, GuiContext::getCurrent()->_mapOfSubjectDataPort[aIDP]);
    }
    //else if ( aTable->intValueCombo( 1, aRowId ) == 1 )    // Data Stream (BASIC) port
    //  // TODO : set value for input data stream port
    //else if ( aTable->intValueCombo( 1, aRowId ) == 2 )    // Data Stream (CALCIUM) port
    //  // TODO : set value for input data stream port
    //else if ( aTable->intValueCombo( 1, aRowId ) == 3 )    // Data Stream (PALM) port
    //  // TODO : set value for input data stream port
    
  }
}

void YACSGui_ServiceNodePage::setOutputPorts()
{
  DEBTRACE("YACSGui_ServiceNodePage::setOutputPorts");
  if ( !myOutputPortsGroupBox ) return;

  ServiceNode* aNode = dynamic_cast<ServiceNode*>( getNode() );
  if ( !aNode ) return;

  myOutputPortsGroupBox->Table()->stopEdit(true);

  if ( myType != SALOMEService )
  {
    // remove old ports
    list<OutPort*> anOutPortsEngine = aNode->getSetOfOutPort();
    list<OutPort*>::iterator anOutPortsIter = anOutPortsEngine.begin();
    for( ;anOutPortsIter!=anOutPortsEngine.end();anOutPortsIter++)
    {
      mySNode->update( REMOVE,
		       ( dynamic_cast<OutputPort*>(*anOutPortsIter) ? OUTPUTPORT : OUTPUTDATASTREAMPORT ),
		       GuiContext::getCurrent()->_mapOfSubjectDataPort[*anOutPortsIter] );
      
      //aNode->edRemovePort(*anOutPortsIter);
      // remove port subject (and all subject links connected to this port)
      mySNode->destroy( GuiContext::getCurrent()->_mapOfSubjectDataPort[*anOutPortsIter] );
    }
    
    // create a set of new input ports
    YACS::ENGINE::Catalog* aCatalog = getInputPanel()->getModule()->getCatalog();
    if ( aCatalog && aNode->getComponent()
	 &&
	 aCatalog->_componentMap.count(aNode->getComponent()->getCompoName()) )
    {
      ComponentDefinition* aCompodef = aCatalog->_componentMap[aNode->getComponent()->getCompoName()];
      if ( aCompodef && aCompodef->_serviceMap.count(aNode->getMethod()) )
      	if ( ServiceNode* aNodeToClone = aCompodef->_serviceMap[aNode->getMethod()] )
	{
	  list<OutPort*> anOutPortsClone = aNodeToClone->getSetOfOutPort();
	  list<OutPort*>::iterator anOutPortsIter = anOutPortsClone.begin();
	  for( ;anOutPortsIter!=anOutPortsClone.end();anOutPortsIter++)
	    if ( OutputPort* anOP = dynamic_cast<OutputPort*>(*anOutPortsIter) ) // Data Flow port
	      dynamic_cast<SubjectElementaryNode*>(mySNode)->addOutputPort( aCatalog,
									    portValueType(anOP->edGetType()),
									    anOP->getName() );
	    else if ( OutputDataStreamPort* anODSP = dynamic_cast<OutputDataStreamPort*>(*anOutPortsIter) ) // Data Stream port
	      dynamic_cast<SubjectElementaryNode*>(mySNode)->addODSPort( aCatalog,
									 portValueType(anODSP->edGetType()),
									 anODSP->getName() );
	}
    }
  }

  // read output data and output data stream ports from the table
  YACSGui_Table* aTable = myOutputPortsGroupBox->Table();
  
  QStringList aPortNames;
  aTable->strings( 0, aPortNames );

  int aRowId = 0;
  for ( QStringList::Iterator it = aPortNames.begin(); it != aPortNames.end(); ++it )
  {
    if ( (*it).isEmpty() ) continue;

    // TODO : Remember "Is in study" flag for output port
    // ...

    aRowId++;
  }
}

YACS::ENGINE::TypeCode* YACSGui_ServiceNodePage::createTypeCode( YACS::ENGINE::DynType theType,
								 YACSGui_Table* theTable,
								 int theRowId )
{
  TypeCode* aTC = 0;

  switch (theType)
  {
  case Double:
  case Int:
  case String:
  case Bool:
    aTC = new TypeCode(theType);
    break;
  case Objref:
    {
      if ( theTable->intValueCombo( 1, theRowId ) == 2 ) // Data Stream (CALCIUM) port
	aTC = new TypeCodeObjref("CALCIUM","CALCIUM");
      else if ( theTable->intValueCombo( 1, theRowId ) == 1 ) // Data Stream (BASIC) port
	aTC = new TypeCodeObjref("BASIC","BASIC");
      else if ( theTable->intValueCombo( 1, theRowId ) == 3 ) // Data Stream (PALM) port
	aTC = new TypeCodeObjref("PALM","PALM");
      else
	aTC = new TypeCodeObjref("","");
    }
    break;
  case Sequence:
    aTC = new TypeCodeSeq("","",(new TypeCode(Double)));
    break;
  case Array:
    aTC = new TypeCodeArray("","",(new TypeCode(Double)),0);
    break;
  case Struct:
    aTC = new TypeCodeStruct("","");
    break;
  default:
    break;
  }

  return aTC;
}

QString YACSGui_ServiceNodePage::scriptText() const
{
#ifdef HAVE_QEXTSCINTILLA_H
  return _myTextEdit->text();
#else
  return myTextEdit->text();
#endif
}

void YACSGui_ServiceNodePage::setScriptText( const QString& txt )
{
#ifdef HAVE_QEXTSCINTILLA_H
  _myTextEdit->setText( txt );
#else
  myTextEdit->setText( txt );
#endif
}
