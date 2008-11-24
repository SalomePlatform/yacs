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
#include <YACSGui_QextScintilla.h>

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

#define NAMECOL 0
#define TYPECOL 1
#define VALUECOL 2
#define CLASSCOL 4
#define STUDYCOL 3

using namespace YACS;
using namespace YACS::ENGINE;
using namespace YACS::HMI;
using namespace std;


/*
  Class : YACSGui_InlineNodePage
  Description :  Page for inline function and inline service nodes properties
*/

YACSGui_InlineNodePage::YACSGui_InlineNodePage( QWidget* theParent, const char* theName, WFlags theFlags )
  : InlineNodePage( theParent, theName, theFlags ),
    YACSGui_NodePage(),
    myIsNeedToReorder(false)
{
  if ( !myInputPortsGroupBox || !myOutputPortsGroupBox ) return;

  // the possibility to search is hided according to the Num. 14 from NEWS file
  Search->hide();
  mySearch->hide();

  //#undef HAVE_QEXTSCINTILLA_H
#ifdef HAVE_QEXTSCINTILLA_H
  //  _myTextEdit = new QextScintilla( InPythonEditorGroupBox, "Python Editor" );
  _myTextEdit = new YACSGui_QextScintilla( InPythonEditorGroupBox, "Python Editor" );
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

  QString aPortTypes = QString("Data Flow"); //";Data Stream (BASIC);Data Stream (CALCIUM);Data Stream (PALM)");
  QString aValueTypes = QString("double;int;string;bool");

  // Input Ports table
  myInputPortsGroupBox->setTitle( tr("Input Ports") );
  myInputPortsGroupBox->HideBtn( YACSGui_PlusMinusGrp::SelectBtn );
  YACSGui_Table* aTable = myInputPortsGroupBox->Table();
  //aTable->setFixedHeight( 100 );
  //aTable->setNumCols( 4 );
  aTable->setNumCols( 3 );
  aTable->horizontalHeader()->setLabel( NAMECOL, tr( "Name" ) );
  //aTable->horizontalHeader()->setLabel( CLASSCOL, tr( "Class" ) );
  aTable->horizontalHeader()->setLabel( TYPECOL, tr( "Type" ) );
  aTable->horizontalHeader()->setLabel( VALUECOL, tr( "Value" ) );
  aTable->setColumnWidth(NAMECOL,58);
  aTable->setColumnWidth(TYPECOL,84);
  //aTable->setColumnWidth(CLASSCOL,80);
  aTable->setCellType( -1, NAMECOL, YACSGui_Table::String );
  //aTable->setCellType( -1, CLASSCOL, YACSGui_Table::Combo );
  aTable->setCellType( -1, TYPECOL, YACSGui_Table::ComboSelect );
  
  //aTable->setParams( 0, CLASSCOL, aPortTypes );
  aTable->setParams( 0, TYPECOL, aValueTypes );

  //aTable->setDefValue( 0, CLASSCOL, "Data Flow" );
  aTable->setDefValue( 0, TYPECOL, "double" );

  aTable->setEditorSync(true);

  connect( aTable, SIGNAL(valueChanged( int, int )), this, SLOT(onValueChanged( int, int )) );

  connect( aTable, SIGNAL(selectButtonClicked( const int, const int )), this, SLOT(onSelectDataType( const int, const int )) );
  
  myInputPortsGroupBox->EnableBtn( YACSGui_PlusMinusGrp::AllBtn );
  myInputPortsGroupBox->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );

  connect( myInputPortsGroupBox, SIGNAL(Inserted( const int )),  this, SLOT(onInserted( const int )) );
  connect( myInputPortsGroupBox, SIGNAL(MovedUp( const int )),   this, SLOT(onMovedUp( const int )) );
  connect( myInputPortsGroupBox, SIGNAL(MovedDown( const int )), this, SLOT(onMovedDown( const int )) );
  connect( myInputPortsGroupBox, SIGNAL(Added( const int )),     this, SLOT(onAdded( const int )) );
  connect( myInputPortsGroupBox, SIGNAL(Removed( const int )),   this, SLOT(onRemoved( const int )) );

  // Output Ports table  
  myOutputPortsGroupBox->setTitle( tr("Output Ports") );
  myOutputPortsGroupBox->HideBtn( YACSGui_PlusMinusGrp::SelectBtn );
  aTable = myOutputPortsGroupBox->Table();
  //aTable->setFixedHeight( 100 );
  aTable->setNumCols( 4 );
  aTable->horizontalHeader()->setLabel( NAMECOL, tr( "Name" ) );
  //aTable->horizontalHeader()->setLabel( CLASSCOL, tr( "Class" ) );
  aTable->horizontalHeader()->setLabel( TYPECOL, tr( "Type" ) );
  aTable->horizontalHeader()->setLabel( VALUECOL, tr( "Value" ) );
  aTable->horizontalHeader()->setLabel( STUDYCOL, tr( "Is in study" ) );
  aTable->setColumnWidth(NAMECOL,58);
  //aTable->setColumnWidth(CLASSCOL,84);
  aTable->setColumnWidth(TYPECOL,80);
  aTable->setCellType( -1, NAMECOL, YACSGui_Table::String );
  //aTable->setCellType( -1, CLASSCOL, YACSGui_Table::Combo );
  aTable->setCellType( -1, TYPECOL, YACSGui_Table::ComboSelect );
  //aTable->setCellType( -1, VALUECOL, YACSGui_Table::??? ); depends on combo box item choosen in the "Value type" column
  aTable->setCellType( -1, STUDYCOL, YACSGui_Table::Combo );

  //aTable->setParams( 0, CLASSCOL, aPortTypes );
  aTable->setParams( 0, TYPECOL, aValueTypes );
  aTable->setParams( 0, STUDYCOL, QString("Yes;No") );

  //aTable->setDefValue( 0, CLASSCOL, "Data Flow" );
  aTable->setDefValue( 0, TYPECOL, "double" );
  aTable->setDefValue( 0, STUDYCOL, QString("Yes") );

  aTable->setEditorSync(true);

  myPara = 0;
  myIndex = 0;

  connect( aTable, SIGNAL(valueChanged( int, int )), this, SLOT(onValueChanged( int, int )) );

  connect( aTable, SIGNAL(selectButtonClicked( const int, const int )), this, SLOT(onSelectDataType( const int, const int )) );
  
  myOutputPortsGroupBox->EnableBtn( YACSGui_PlusMinusGrp::AllBtn );
  myOutputPortsGroupBox->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );

  connect( myOutputPortsGroupBox, SIGNAL(Inserted( const int )),  this, SLOT(onInserted( const int )) );
  connect( myOutputPortsGroupBox, SIGNAL(MovedUp( const int )),   this, SLOT(onMovedUp( const int )) );
  connect( myOutputPortsGroupBox, SIGNAL(MovedDown( const int )), this, SLOT(onMovedDown( const int )) );
  connect( myOutputPortsGroupBox, SIGNAL(Added( const int )),     this, SLOT(onAdded( const int )) );
  connect( myOutputPortsGroupBox, SIGNAL(Removed( const int )),   this, SLOT(onRemoved( const int )) );

  connect( myNodeName, SIGNAL(textChanged( const QString& )), this, SLOT(onNodeNameChanged( const QString& )) );
  connect( mySearch, SIGNAL(returnPressed()), this, SLOT(onSearch()) );
}

YACSGui_InlineNodePage::~YACSGui_InlineNodePage()
{
  if (getInputPanel()) getInputPanel()->removePage(this);
}

void YACSGui_InlineNodePage::setSNode( YACS::HMI::SubjectNode* theSNode )
{
  if ( !theSNode ) return;

  if ( dynamic_cast<InlineNode*>( theSNode->getNode() ) )
  {
    YACSGui_NodePage::setSNode( theSNode );
    myIPList.clear();
    myOPList.clear();
  }
}

void YACSGui_InlineNodePage::setMode( const YACSGui_InputPanel::PageMode theMode )
{
  DEBTRACE("YACSGui_InlineNodePage::setMode " << theMode);
  if(myMode == theMode)
    return;

  YACSGui_NodePage::setMode(theMode);

  if ( myMode == YACSGui_InputPanel::EditMode )
  {
    myNodeName->setReadOnly(false);

    ExecutionGroupBox->hide();

    EditPortsGroupBox->setTitle( tr( "Edit Ports" ) );

    YACSGui_Table* aTable;
    
    myInputPortsGroupBox->ShowBtn();
    myInputPortsGroupBox->HideBtn( YACSGui_PlusMinusGrp::SelectBtn );
    aTable = myInputPortsGroupBox->Table();
    aTable->setReadOnly( -1, NAMECOL, false );
    aTable->setReadOnly( -1, TYPECOL, false );
    aTable->setReadOnly( -1, VALUECOL, false );

    myOutputPortsGroupBox->ShowBtn();
    myOutputPortsGroupBox->HideBtn( YACSGui_PlusMinusGrp::SelectBtn );
    aTable = myOutputPortsGroupBox->Table();
    aTable->setReadOnly( -1, NAMECOL, false );
    aTable->setReadOnly( -1, TYPECOL, false );
    aTable->setReadOnly( -1, VALUECOL, false );

    InPythonEditorGroupBox->show();
  } 
  else if ( myMode == YACSGui_InputPanel::RunMode )
  {
    myNodeName->setReadOnly(true);

    ExecutionGroupBox->show();

    EditPortsGroupBox->setTitle( tr( "Port List" ) );

    YACSGui_Table* aTable;

    myInputPortsGroupBox->HideBtn();
    aTable = myInputPortsGroupBox->Table();
    aTable->setReadOnly( -1, NAMECOL, true );
    aTable->setReadOnly( -1, TYPECOL, true );
    aTable->setReadOnly( -1, VALUECOL, true );
    
    myOutputPortsGroupBox->HideBtn();
    aTable = myOutputPortsGroupBox->Table();
    aTable->setReadOnly( -1, NAMECOL, true );
    aTable->setReadOnly( -1, TYPECOL, true );
    aTable->setReadOnly( -1, VALUECOL, true );

    InPythonEditorGroupBox->hide();
  }
}

void YACSGui_InlineNodePage::notifyNodeStatus( int theStatus )
{
  if ( myNodeState )
  {
    myNodeState->setPaletteForegroundColor(getColor(theStatus));
    myNodeState->setText(getText(theStatus));
  }
}

void YACSGui_InlineNodePage::notifyNodeProgress()
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

void YACSGui_InlineNodePage::notifyInPortValues( std::map<std::string,std::string> theInPortName2Value )
{
  DEBTRACE("YACSGui_InlineNodePage::notifyInPortValues");
  QStringList aValues;

  QStringList aPortNames;
  myInputPortsGroupBox->Table()->strings( NAMECOL, aPortNames );

  for ( QStringList::Iterator it = aPortNames.begin(); it != aPortNames.end(); ++it )
  {
    if ( (*it).isEmpty() ) continue;
    DEBTRACE((*it).latin1());

    map<string,string>::iterator it1 = theInPortName2Value.find( (*it).latin1() );
    if ( it1 != theInPortName2Value.end() )
      {
        DEBTRACE((*it1).second);
      aValues.append( QString((*it1).second.c_str()) );
      }
  }

  if ( aPortNames.count() == aValues.count() )
    myInputPortsGroupBox->Table()->setStrings( VALUECOL, aValues );
}

void YACSGui_InlineNodePage::notifyOutPortValues( std::map<std::string,std::string> theOutPortName2Value )
{
  DEBTRACE("YACSGui_InlineNodePage::notifyOutPortValues");
  QStringList aValues;

  QStringList aPortNames;
  myOutputPortsGroupBox->Table()->strings( NAMECOL, aPortNames );

  for ( QStringList::Iterator it = aPortNames.begin(); it != aPortNames.end(); ++it )
  {
    if ( (*it).isEmpty() ) continue;
    DEBTRACE((*it).latin1());

    map<string,string>::iterator it1 = theOutPortName2Value.find( (*it).latin1() );
    if ( it1 != theOutPortName2Value.end() )
      {
        DEBTRACE((*it1).second);
      aValues.append( QString((*it1).second.c_str()) );
      }
  }

  if ( aPortNames.count() == aValues.count() )
    myOutputPortsGroupBox->Table()->setStrings( VALUECOL, aValues );
}

void YACSGui_InlineNodePage::setDataType( YACS::ENGINE::TypeCode* theDataType )
{
  DEBTRACE("YACSGui_InlineNodePage::setDataType");
  if ( mySelectDataTypeFor.myRow < 0 ) return;

  YACSGui_Table* aTable = 0;
  if ( mySelectDataTypeFor.myIn )
  {
    aTable = myInputPortsGroupBox->Table();
    
    if ( myRow2DataTypeIn.find(mySelectDataTypeFor.myRow) != myRow2DataTypeIn.end() )
      myRow2DataTypeIn[mySelectDataTypeFor.myRow] = theDataType;
    else
      myRow2DataTypeIn.insert(make_pair(mySelectDataTypeFor.myRow,theDataType));
  }
  else
  {
    aTable = myOutputPortsGroupBox->Table();
    
    if ( myRow2DataTypeOut.find(mySelectDataTypeFor.myRow) != myRow2DataTypeOut.end() )
      myRow2DataTypeOut[mySelectDataTypeFor.myRow] = theDataType;
    else
      myRow2DataTypeOut.insert(make_pair(mySelectDataTypeFor.myRow,theDataType));
  }

  DEBTRACE("YACSGui_InlineNodePage::setDataType");
  if ( aTable )
  {
    // update the content of the cell according to the selected data type
    if ( YACSGui_TableComboSelectItem* aCSItem = 
    	 dynamic_cast<YACSGui_TableComboSelectItem*>(aTable->item( mySelectDataTypeFor.myRow, TYPECOL )) )
    {
      if( aCSItem->selectButton() && aCSItem->selectButton()->isOn() )
	aCSItem->selectButton()->setOn( false );

      QString aTypeName(theDataType->name());
      aCSItem->showText(aTypeName);
      aTable->setDefValue( mySelectDataTypeFor.myRow, TYPECOL, aTypeName );
    }
    
    mySelectDataTypeFor.myRow = -1;
  }
}

void YACSGui_InlineNodePage::checkModifications()
{
  DEBTRACE("YACSGui_InlineNodePage::checkModifications");
  if ( !getNode() ) return;

  // 1) check if the content of the page is really modified (in compare with the content of engine object)
  bool isModified = false;
  if ( myNodeName->text().compare(getNodeName()) != 0 ) isModified = true;

  if ( !isModified ) isModified = isPortsModified();
  if ( !isModified ) isModified = isPortsModified(false);

  if ( !isModified )
    // check the code modifications in the built-in Python code editor
    if ( InlineNode* anIN = dynamic_cast<InlineNode*>( getNode() ) )
      {
#ifdef HAVE_QEXTSCINTILLA_H
        QString theText = _myTextEdit->text();
#else
        QString theText = myTextEdit->text();
#endif
        //SCRUTE(theText);
        DEBTRACE(theText);
        //SCRUTE(anIN->getScript());
        DEBTRACE(anIN->getScript());
        if (theText.compare(QString(anIN->getScript())) != 0 ) isModified = true;
      }
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
      if ( getInputPanel() ) getInputPanel()->emitApply(YACSGui_InputPanel::InlineNodeId);
    }
    else
      updateState();
}

void YACSGui_InlineNodePage::onApply()
{
  DEBTRACE("YACSGui_InlineNodePage::onApply()");
  onApplyStatus="OK";
  // Rename a node
  if ( myNodeName )
    setNodeName( myNodeName->text() );
  
  bool withFilling = ( myIPList.isEmpty() || myOPList.isEmpty() );

  // Reset the list of input ports
  setInputPorts();
  
  // Reset the list of output ports
  setOutputPorts();

  // Reset Python function/script
  if ( InlineNode* anIN = dynamic_cast<InlineNode*>( getNode() ) )
    anIN->setScript( scriptText().isEmpty() ? string("") : scriptText().latin1() );
  if ( InlineFuncNode* anIFN = dynamic_cast<InlineFuncNode*>( getNode() ) )
  {
    QString aText = scriptText();
    int aFromId = aText.find( QString("\ndef ") ) + QString("\ndef ").length();
    int aToId = aText.find( QString("("), aFromId );
    aText = aText.left( aToId );
    QString aFname = aText.right( aText.length() - aFromId ).stripWhiteSpace();
    anIFN->setFname( aFname.isEmpty() ? string("") : aFname.latin1() );
  }

  if ( myIsNeedToReorder )
  {
    //orderPorts(withFilling);
    myIsNeedToReorder = false;
  }
  
  if(onApplyStatus=="OK")
    {
      updateState();
      resetStoredPortsMaps();
      resetDataTypeMaps();
      resetPLists();
    }

  updateBlocSize();
}

void YACSGui_InlineNodePage::updateState()
{
  DEBTRACE("YACSGui_InlineNodePage::updateState()");
  // Clear input ports table and output ports tables, if they are not empty
  if ( myInputPortsGroupBox )
  {
    myInputPortsGroupBox->Table()->setNumRows( 0 );
    myInputPortsGroupBox->EnableBtn( YACSGui_PlusMinusGrp::AllBtn );
  }
  if ( myOutputPortsGroupBox )
  {
    myOutputPortsGroupBox->Table()->setNumRows( 0 );
    myOutputPortsGroupBox->EnableBtn( YACSGui_PlusMinusGrp::AllBtn );
  }

  // Set node name
  if ( myNodeName ) myNodeName->setText( getNodeName() );
  
  // Set node full name (read only)
  if ( myNodeFullName )
    myNodeFullName->setText( getNode()->getRootNode()->getChildName(getNode()) );
  
  // Set node type: inline function or inline script
  if ( dynamic_cast<PyFuncNode*>( getNode() ) )
    myNodeType->setText( tr( "INLINE_FUNCTION_NODE" ) ); // inline function node
  else if ( dynamic_cast<PythonNode*>( getNode() ) )
    myNodeType->setText( tr( "INLINE_SCRIPT_NODE" ) ); // inline script node

  // Fill the table of input ports of the node
  fillInputPortsTable();
  
  // Fill the table of output ports of the node
  fillOutputPortsTable();

  // Show the body (script/function) of inline node
  if ( InlineNode* anIN = dynamic_cast<InlineNode*>( getNode() ) )
    setScriptText( QString(anIN->getScript()) );
}

void YACSGui_InlineNodePage::onNodeNameChanged( const QString& theName )
{
  if ( myNodeFullName )
  {
    QString anOldName = myNodeFullName->text();
    int aRDotId = anOldName.findRev('.');
    myNodeFullName->setText( anOldName.replace( aRDotId+1, anOldName.length()-(aRDotId+1), theName ) );
  }
}

void YACSGui_InlineNodePage::onValueChanged( int theRow, int theCol )
{
  DEBTRACE("YACSGui_InlineNodePage::onValueChanged " << theRow << " " << theCol);
  if ( YACSGui_Table* aTable = ( YACSGui_Table* )sender() )
  {
    if ( theCol == TYPECOL ) // the value type of the port was changed
    {
      if( aTable == myInputPortsGroupBox->Table() )
        setValueCellValidator( aTable, theRow );

      if( aTable == myOutputPortsGroupBox->Table() )
	aTable->setReadOnly( theRow, VALUECOL, true );
    }
  }
}

void YACSGui_InlineNodePage::hideEvent( QHideEvent*)
{
  myPara = 0;
  myIndex = 0;
#ifdef HAVE_QEXTSCINTILLA_H
//   _myTextEdit->removeSelection(); // --- see if replacement ?
#else
  myTextEdit->removeSelection();
#endif
}

void YACSGui_InlineNodePage::onSearch()
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

void YACSGui_InlineNodePage::onInserted( const int theRow )
{
  DEBTRACE("onInserted " << theRow);
  if ( YACSGui_PlusMinusGrp* aGrpBox = ( YACSGui_PlusMinusGrp* )sender() )
    if ( myInputPortsGroupBox == aGrpBox )
    {
      // change the content of myRow2StoredInPorts
      //make a copy
      map<int,InPort*> aMap=myRow2StoredInPorts;
      myRow2StoredInPorts.clear();

      for (map<int,InPort*>::iterator it = aMap.begin() ; it != aMap.end(); it++ )
        {
          int aRow = (*it).first;
          InPort* aPort = (*it).second;
          if ( aRow < theRow )
            myRow2StoredInPorts[aRow]=aPort;
          else
            myRow2StoredInPorts[aRow+1]=aPort;
        }

      // change the content of myRow2DataTypeIn
      //make a copy
      map<int,TypeCode*> bMap=myRow2DataTypeIn;
      myRow2DataTypeIn.clear();

      for (map<int,TypeCode*>::iterator it = bMap.begin() ; it != bMap.end(); it++ )
        {
          int aRow = (*it).first;
          TypeCode* aTC = (*it).second;
          if ( aRow < theRow )
            myRow2DataTypeIn[aRow]=aTC;
          else
            myRow2DataTypeIn[aRow+1]=aTC;
        }
      myIsNeedToReorder = true;
    }
    else if ( myOutputPortsGroupBox == aGrpBox )
    {
      // change the content of myRow2StoredOutPorts
      //make a copy
      map<int,OutPort*> aMap=myRow2StoredOutPorts;
      myRow2StoredOutPorts.clear();

      for (map<int,OutPort*>::iterator it = aMap.begin() ; it != aMap.end(); it++ )
        {
          int aRow = (*it).first;
          OutPort* aPort = (*it).second;
          if ( aRow < theRow )
            myRow2StoredOutPorts[aRow]=aPort;
          else
            myRow2StoredOutPorts[aRow+1]=aPort;
        }

      // change the content of myRow2DataTypeOut
      //make a copy
      map<int,TypeCode*> bMap=myRow2DataTypeOut;
      myRow2DataTypeOut.clear();

      for (map<int,TypeCode*>::iterator it = bMap.begin() ; it != bMap.end(); it++ )
        {
          int aRow = (*it).first;
          TypeCode* aTC = (*it).second;
          if ( aRow < theRow )
            myRow2DataTypeOut[aRow]=aTC;
          else
            myRow2DataTypeOut[aRow+1]=aTC;
        }
      myIsNeedToReorder = true;
    }
}

void YACSGui_InlineNodePage::onMovedUp( const int theUpRow )
{
  DEBTRACE("YACSGui_InlineNodePage::onMovedUp " << theUpRow);
  if ( YACSGui_PlusMinusGrp* aGrpBox = ( YACSGui_PlusMinusGrp* )sender() )
    if ( myInputPortsGroupBox == aGrpBox )
    {
      // change the content of myRow2StoredInPorts
      InPort* aPortToMoveDown = 0;
      if ( myRow2StoredInPorts.find(theUpRow) != myRow2StoredInPorts.end() )
      {
	aPortToMoveDown = myRow2StoredInPorts[theUpRow];
	myRow2StoredInPorts.erase(theUpRow);
      }

      InPort* aPortToMoveUp = 0;
      if ( myRow2StoredInPorts.find(theUpRow+1) != myRow2StoredInPorts.end() )
      {
	aPortToMoveUp = myRow2StoredInPorts[theUpRow+1];
	myRow2StoredInPorts.erase(theUpRow+1);
      }
  
      if ( aPortToMoveUp ) myRow2StoredInPorts.insert( std::make_pair(theUpRow,aPortToMoveUp));
      if ( aPortToMoveDown ) myRow2StoredInPorts.insert( std::make_pair(theUpRow+1,aPortToMoveDown));    

      // change the content of myRow2DataTypeIn
      TypeCode* aTCToMoveDown = 0;
      if ( myRow2DataTypeIn.find(theUpRow) != myRow2DataTypeIn.end() )
      {
	aTCToMoveDown = myRow2DataTypeIn[theUpRow];
	myRow2DataTypeIn.erase(theUpRow);
      }

      TypeCode* aTCToMoveUp = 0;
      if ( myRow2DataTypeIn.find(theUpRow+1) != myRow2DataTypeIn.end() )
      {
	aTCToMoveUp = myRow2DataTypeIn[theUpRow+1];
	myRow2DataTypeIn.erase(theUpRow+1);
      }
  
      if ( aTCToMoveUp ) myRow2DataTypeIn.insert( std::make_pair(theUpRow,aTCToMoveUp));
      if ( aTCToMoveDown ) myRow2DataTypeIn.insert( std::make_pair(theUpRow+1,aTCToMoveDown));

      myIsNeedToReorder = true;
    }
    else if ( myOutputPortsGroupBox == aGrpBox )
    {
      // change the content of myRow2StoredOutPorts
      OutPort* aPortToMoveDown = 0;
      if ( myRow2StoredOutPorts.find(theUpRow) != myRow2StoredOutPorts.end() )
      {
	aPortToMoveDown = myRow2StoredOutPorts[theUpRow];
	myRow2StoredOutPorts.erase(theUpRow);
      }

      OutPort* aPortToMoveUp = 0;
      if ( myRow2StoredOutPorts.find(theUpRow+1) != myRow2StoredOutPorts.end() )
      {
	aPortToMoveUp = myRow2StoredOutPorts[theUpRow+1];
	myRow2StoredOutPorts.erase(theUpRow+1);
      }
  
      if ( aPortToMoveUp ) myRow2StoredOutPorts.insert( std::make_pair(theUpRow,aPortToMoveUp));
      if ( aPortToMoveDown ) myRow2StoredOutPorts.insert( std::make_pair(theUpRow+1,aPortToMoveDown));

      // change the content of myRow2DataTypeOut
      TypeCode* aTCToMoveDown = 0;
      if ( myRow2DataTypeOut.find(theUpRow) != myRow2DataTypeOut.end() )
      {
	aTCToMoveDown = myRow2DataTypeOut[theUpRow];
	myRow2DataTypeOut.erase(theUpRow);
      }

      TypeCode* aTCToMoveUp = 0;
      if ( myRow2DataTypeOut.find(theUpRow+1) != myRow2DataTypeOut.end() )
      {
	aTCToMoveUp = myRow2DataTypeOut[theUpRow+1];
	myRow2DataTypeOut.erase(theUpRow+1);
      }
  
      if ( aTCToMoveUp ) myRow2DataTypeOut.insert( std::make_pair(theUpRow,aTCToMoveUp));
      if ( aTCToMoveDown ) myRow2DataTypeOut.insert( std::make_pair(theUpRow+1,aTCToMoveDown));

      myIsNeedToReorder = true;
    }
}

void YACSGui_InlineNodePage::onMovedDown( const int theDownRow )
{
  if ( YACSGui_PlusMinusGrp* aGrpBox = ( YACSGui_PlusMinusGrp* )sender() )
    if ( myInputPortsGroupBox == aGrpBox )
    {
      // change the content of myRow2StoredInPorts
      InPort* aPortToMoveUp = 0;
      if ( myRow2StoredInPorts.find(theDownRow) != myRow2StoredInPorts.end() )
      {
	aPortToMoveUp = myRow2StoredInPorts[theDownRow];
	myRow2StoredInPorts.erase(theDownRow);
      }

      InPort* aPortToMoveDown = 0;
      if ( myRow2StoredInPorts.find(theDownRow-1) != myRow2StoredInPorts.end() )
      {
	aPortToMoveDown = myRow2StoredInPorts[theDownRow-1];
	myRow2StoredInPorts.erase(theDownRow-1);
      }
      
      if ( aPortToMoveDown ) myRow2StoredInPorts.insert( std::make_pair(theDownRow,aPortToMoveDown));
      if ( aPortToMoveUp ) myRow2StoredInPorts.insert( std::make_pair(theDownRow-1,aPortToMoveUp));

      // change the content of myRow2DataTypeIn
      TypeCode* aTCToMoveUp = 0;
      if ( myRow2DataTypeIn.find(theDownRow) != myRow2DataTypeIn.end() )
      {
	aTCToMoveUp = myRow2DataTypeIn[theDownRow];
	myRow2DataTypeIn.erase(theDownRow);
      }

      TypeCode* aTCToMoveDown = 0;
      if ( myRow2DataTypeIn.find(theDownRow-1) != myRow2DataTypeIn.end() )
      {
	aTCToMoveDown = myRow2DataTypeIn[theDownRow-1];
	myRow2DataTypeIn.erase(theDownRow-1);
      }
      
      if ( aTCToMoveDown ) myRow2DataTypeIn.insert( std::make_pair(theDownRow,aTCToMoveDown));
      if ( aTCToMoveUp ) myRow2DataTypeIn.insert( std::make_pair(theDownRow-1,aTCToMoveUp));

      myIsNeedToReorder = true;
    }
    else if ( myOutputPortsGroupBox == aGrpBox )
    {
      // change the content of myRow2StoredOutPorts
      OutPort* aPortToMoveUp = 0;
      if ( myRow2StoredOutPorts.find(theDownRow) != myRow2StoredOutPorts.end() )
      {
	aPortToMoveUp = myRow2StoredOutPorts[theDownRow];
	myRow2StoredOutPorts.erase(theDownRow);
      }

      OutPort* aPortToMoveDown = 0;
      if ( myRow2StoredOutPorts.find(theDownRow-1) != myRow2StoredOutPorts.end() )
      {
	aPortToMoveDown = myRow2StoredOutPorts[theDownRow-1];
	myRow2StoredOutPorts.erase(theDownRow-1);
      }
      
      if ( aPortToMoveDown ) myRow2StoredOutPorts.insert( std::make_pair(theDownRow,aPortToMoveDown));
      if ( aPortToMoveUp ) myRow2StoredOutPorts.insert( std::make_pair(theDownRow-1,aPortToMoveUp));

      // change the content of myRow2DataTypeOut
      TypeCode* aTCToMoveUp = 0;
      if ( myRow2DataTypeOut.find(theDownRow) != myRow2DataTypeOut.end() )
      {
	aTCToMoveUp = myRow2DataTypeOut[theDownRow];
	myRow2DataTypeOut.erase(theDownRow);
      }

      TypeCode* aTCToMoveDown = 0;
      if ( myRow2DataTypeOut.find(theDownRow-1) != myRow2DataTypeOut.end() )
      {
	aTCToMoveDown = myRow2DataTypeOut[theDownRow-1];
	myRow2DataTypeOut.erase(theDownRow-1);
      }
      
      if ( aTCToMoveDown ) myRow2DataTypeOut.insert( std::make_pair(theDownRow,aTCToMoveDown));
      if ( aTCToMoveUp ) myRow2DataTypeOut.insert( std::make_pair(theDownRow-1,aTCToMoveUp));
      
      myIsNeedToReorder = true;
    }
}

void YACSGui_InlineNodePage::onRemoved( const int theRow )
{
  DEBTRACE("YACSGui_InlineNodePage::onRemoved: " << theRow);
  if ( YACSGui_PlusMinusGrp* aGrpBox = ( YACSGui_PlusMinusGrp* )sender() )
    if ( myInputPortsGroupBox == aGrpBox )
    {
      // change the content of myRow2StoredInPorts
      int aRow;
      DEBTRACE(myRow2StoredInPorts.size());
      for(aRow=theRow;aRow<myRow2StoredInPorts.size()-1;aRow++)
        {
          DEBTRACE("myRow2StoredInPorts["<<aRow<<"]="<< myRow2StoredInPorts[aRow+1]);
          myRow2StoredInPorts[aRow]=myRow2StoredInPorts[aRow+1];
        }
      DEBTRACE("erase last row: " << aRow);
      myRow2StoredInPorts.erase(aRow);

      // change the content of myRow2DataTypeIn
      DEBTRACE(myRow2DataTypeIn.size());
      for(aRow=theRow;aRow<myRow2DataTypeIn.size()-1;aRow++)
        {
          DEBTRACE("myRow2DataTypeIn["<<aRow<<"]="<< myRow2DataTypeIn[aRow+1]);
          myRow2DataTypeIn[aRow]=myRow2DataTypeIn[aRow+1];
        }
      DEBTRACE("erase last row: " << aRow);
      myRow2DataTypeIn.erase(aRow);
    }
    else if ( myOutputPortsGroupBox == aGrpBox )
    {
      // change the content of myRow2StoredOutPorts
      int aRow;
      DEBTRACE(myRow2StoredOutPorts.size());
      for(aRow=theRow;aRow<myRow2StoredOutPorts.size()-1;aRow++)
        {
          DEBTRACE("myRow2StoredOutPorts["<<aRow<<"]="<< myRow2StoredOutPorts[aRow+1]);
          myRow2StoredOutPorts[aRow]=myRow2StoredOutPorts[aRow+1];
        }
      DEBTRACE("erase last row: " << aRow);
      myRow2StoredOutPorts.erase(aRow);

      // change the content of myRow2DataTypeOut
      DEBTRACE(myRow2DataTypeOut.size());
      for(aRow=theRow;aRow<myRow2DataTypeOut.size()-1;aRow++)
        {
          DEBTRACE("myRow2DataTypeOut["<<aRow<<"]="<< myRow2DataTypeOut[aRow+1]);
          myRow2DataTypeOut[aRow]=myRow2DataTypeOut[aRow+1];
        }
      DEBTRACE("erase last row: " << aRow);
      myRow2DataTypeOut.erase(aRow);
    }
}

void YACSGui_InlineNodePage::onAdded( const int theRow )
{
  DEBTRACE("YACSGui_InlineNodePage::onAdded: " << theRow);
  if ( YACSGui_PlusMinusGrp* aGrpBox = ( YACSGui_PlusMinusGrp* )sender() )
    if ( myInputPortsGroupBox == aGrpBox )
    {
      myInputPortsGroupBox->Table()->setDefValue( theRow, TYPECOL, "" );
      myInputPortsGroupBox->Table()->setDefValue( theRow, VALUECOL, "" );
      myInputPortsGroupBox->Table()->item( theRow, TYPECOL )->setText("double");
      myInputPortsGroupBox->Table()->item( theRow, VALUECOL )->setText("");
      setValueCellValidator( myInputPortsGroupBox->Table() , theRow );
    }
    else if ( myOutputPortsGroupBox == aGrpBox )
    {
      myOutputPortsGroupBox->Table()->setDefValue( theRow, TYPECOL, "" );
      myOutputPortsGroupBox->Table()->item( theRow, TYPECOL )->setText("double");
      myOutputPortsGroupBox->Table()->item( theRow, VALUECOL )->setText("");
      myOutputPortsGroupBox->Table()->setReadOnly( theRow, VALUECOL,true );
    }
}

void YACSGui_InlineNodePage::onSelectDataType( const int theRow, const int theCol )
{
  DEBTRACE("YACSGui_InlineNodePage::onSelectDataType");
  if ( theCol == TYPECOL && theRow >= 0 ) // the value type selection
  {
    if ( YACSGui_Table* aTable = ( YACSGui_Table* )sender() )
    {
      YACSGui_TableComboSelectItem* aCSItem = dynamic_cast<YACSGui_TableComboSelectItem*>(aTable->item( theRow, theCol ));
      if ( aCSItem && aCSItem->selectButton() && !aCSItem->selectButton()->isOn() )
      {
	mySelectDataTypeFor.myRow = -1;
	return;
      }

      if ( aTable == myInputPortsGroupBox->Table() )
      {
	if ( !mySelectDataTypeFor.myIn && mySelectDataTypeFor.myRow >= 0 )
	  if ( YACSGui_TableComboSelectItem* aCSPrevItem =
	       dynamic_cast<YACSGui_TableComboSelectItem*>(aTable->item( mySelectDataTypeFor.myRow, theCol )) )
	    if ( aCSPrevItem->selectButton() ) aCSPrevItem->selectButton()->setOn(false);
	
	mySelectDataTypeFor.myIn = true;
	mySelectDataTypeFor.myRow = theRow;
      }
      else if ( aTable == myOutputPortsGroupBox->Table() )
      {
	if ( mySelectDataTypeFor.myIn && mySelectDataTypeFor.myRow >= 0 )
	  if ( YACSGui_TableComboSelectItem* aCSPrevItem =
	       dynamic_cast<YACSGui_TableComboSelectItem*>(aTable->item( mySelectDataTypeFor.myRow, theCol )) )
	    if ( aCSPrevItem->selectButton() ) aCSPrevItem->selectButton()->setOn(false);
	
	mySelectDataTypeFor.myIn = false;
	mySelectDataTypeFor.myRow = theRow;
      }
    }
  }
}

void YACSGui_InlineNodePage::fillInputPortsTable()
{
  DEBTRACE("YACSGui_InlineNodePage::fillInputPortsTable()");
  if ( !getNode() ) return;

  myRow2StoredInPorts.clear();
  myRow2DataTypeIn.clear();

  QStringList aPortNames;
  QStringList aValueTypes;
  QStringList aValues;

  QValueList<bool> aReadOnlyFlags;

  YACSGui_Table* aTable = myInputPortsGroupBox->Table();

  int aRowId = 0;
  list<InPort*> anInPortsEngine = getNode()->getSetOfInPort();
  list<InPort*>::iterator anInPortsIter = anInPortsEngine.begin();
  for( ;anInPortsIter!=anInPortsEngine.end();anInPortsIter++)
  {
    InPort* anInPort = *anInPortsIter;

    myRow2StoredInPorts.insert( std::make_pair(aRowId,anInPort) );
    myRow2DataTypeIn[aRowId]=anInPort->edGetType();

    // Collect port names in the list
    aPortNames.append( anInPort->getName() );

    // Value type
    aValueTypes.append( getPortType( anInPort ) );

    // Value
    aValues.append( getPortValue( anInPort ) );

    if(anInPort->edGetNumberOfLinks() > 0)
      aReadOnlyFlags.append( true);
    else
      aReadOnlyFlags.append( false);

    aRowId++;
  }

  // Fill "Port name" column
  aTable->setStrings( NAMECOL, aPortNames, true );

  QStringList aIPortNames;
  aTable->strings( NAMECOL, aIPortNames);
  // Fill "Port type" column
  //aTable->setStrings( CLASSCOL, aPortTypes, true );

  // Fill "Value type" column
  aTable->setStrings( TYPECOL, aValueTypes, true );

  // set validator for input cell
  for (int irow=0; irow < aTable->numRows(); irow++)
    setValueCellValidator( aTable, irow);

  // Fill "Value" column
  aTable->setStrings( VALUECOL, aValues, true );

  // Set "Value" column read only (for linked ports)
  for ( int i = 0, n = aTable->numRows(); i < n; i++ )
  {
    aTable->setDefValue( i, TYPECOL, aValueTypes[i] );
    aTable->setReadOnly( i, TYPECOL, aReadOnlyFlags[ i ] );
    aTable->setReadOnly( i, VALUECOL, aReadOnlyFlags[ i ] );
  }

  if ( !aPortNames.empty() ) myInputPortsGroupBox->EnableBtn( YACSGui_PlusMinusGrp::AllBtn );
}

void YACSGui_InlineNodePage::fillOutputPortsTable()
{
  DEBTRACE("YACSGui_InlineNodePage::fillOutputPortsTable()");
  if ( !getNode() ) return;

  myRow2StoredOutPorts.clear();
  myRow2DataTypeOut.clear();

  QStringList aPortNames;
  QStringList aValueTypes;
  QStringList aValues;
  QStringList anIsInStudy;
  QValueList<bool> aReadOnlyFlags;
  
  YACSGui_Table* aTable = myOutputPortsGroupBox->Table();

  int aRowId = 0;
  list<OutPort*> anOutPortsEngine = getNode()->getSetOfOutPort();
  list<OutPort*>::iterator anOutPortsIter = anOutPortsEngine.begin();
  for( ;anOutPortsIter!=anOutPortsEngine.end();anOutPortsIter++)
  {
    OutPort* anOutPort = *anOutPortsIter;

    myRow2StoredOutPorts.insert( std::make_pair(aRowId,anOutPort) );
    myRow2DataTypeOut[aRowId]=anOutPort->edGetType();

    // Collect port names in the list
    aPortNames.append( anOutPort->getName() );
    
    // Value type
    aValueTypes.append( getPortType( anOutPort ) );

    // Value
    aValues.append( getPortValue( anOutPort ) );

    if(anOutPort->edGetNumberOfOutLinks() > 0)
      aReadOnlyFlags.append( true);
    else
      aReadOnlyFlags.append( false);

    // Is in study
    // There is no a such parameter either in engine or GUI at the current moment.
    // TODO: we need to implement it at GUI side.
    // As a temporary solution we set the default value
    anIsInStudy.append( aTable->defValue( 0, STUDYCOL ) );

    aRowId++;
  }

  // Fill "Port name" column
  aTable->setStrings( NAMECOL, aPortNames, true );

  // Fill "Port type" column
  //aTable->setStrings( CLASSCOL, aPortTypes, true );

  // Fill "Value type" column
  aTable->setStrings( TYPECOL, aValueTypes, true );

  // Fill "Value" column
  aTable->setStrings( VALUECOL, aValues, true );

  // Fill "Is in study" column
  aTable->setStrings( STUDYCOL, anIsInStudy, true );

  // Set "Value" column read only
  for ( int i = 0, n = aTable->numRows(); i < n; i++ )
  {
    aTable->setDefValue( i, TYPECOL, aValueTypes[i] );
    aTable->setReadOnly( i, VALUECOL, true );
    aTable->setReadOnly( i, TYPECOL, aReadOnlyFlags[ i ] );
  }

  if ( !aPortNames.empty() ) myOutputPortsGroupBox->EnableBtn( YACSGui_PlusMinusGrp::AllBtn );
}

QString YACSGui_InlineNodePage::getPortType( YACS::ENGINE::Port* thePort ) const
{
  QString aType;

  YACSGui_Table* aTable = 0;
  if ( dynamic_cast<InPort*>(thePort) )
    aTable = myInputPortsGroupBox->Table();
  else if ( dynamic_cast<OutPort*>(thePort) )
    aTable = myOutputPortsGroupBox->Table();

  if ( !aTable ) return aType;

  QStringList aVTCB = aTable->Params( 0, TYPECOL );

  if ( DataPort* aDataPort = dynamic_cast<DataPort*>(thePort) )
  {
    DynType aDType = aDataPort->edGetType()->kind();
    if ( aDType > NONE && aDType < Objref )
      aType = aVTCB[aDType-1];
    else
      aType = QString(aDataPort->edGetType()->name());
  }
  
  return aType;
}

bool YACSGui_InlineNodePage::isStored( YACS::ENGINE::Port* thePort )
{
  DEBTRACE("YACSGui_InlineNodePage::isStored " << thePort);
  if ( InPort* anIP = dynamic_cast<InPort*>(thePort) )
  {
    map<int,InPort*>::iterator it = myRow2StoredInPorts.begin();
    for ( ; it != myRow2StoredInPorts.end(); it++ )
      if ( (*it).second == thePort ) return true;
    return false;
  }
  else if ( OutPort* anOP = dynamic_cast<OutPort*>(thePort) )
  {
    map<int,OutPort*>::iterator it = myRow2StoredOutPorts.begin();
    for ( ; it != myRow2StoredOutPorts.end(); it++ )
      if ( (*it).second == thePort ) return true;
    return false;
  }
  return true; // the gate ports are kept (not re-create)
}

void YACSGui_InlineNodePage::resetStoredPortsMaps()
{
  DEBTRACE("YACSGui_InlineNodePage::resetStoredPortsMaps");
  myRow2StoredInPorts.clear();
  myRow2DataTypeIn.clear();

  QStringList aIPortNames;
  myInputPortsGroupBox->Table()->strings( NAMECOL, aIPortNames );

  int aRowId = 0;
  for ( QStringList::Iterator it = aIPortNames.begin(); it != aIPortNames.end(); ++it )
  {
    if ( (*it).isEmpty() ) continue;

    try {
      if ( InputPort* aIDP = getNode()->getInputPort((*it).latin1()) )
        {
          DEBTRACE("store port: " << aRowId << " " << aIDP);
          myRow2StoredInPorts[aRowId]=aIDP;
          myRow2DataTypeIn[aRowId]=aIDP->edGetType();
        }
    }
    catch (YACS::Exception& ex) {
        std::cerr << "INTERNAL ERROR. Unknown port " << (*it).latin1() << std::endl;
    }
    aRowId++;
  }

  myRow2StoredOutPorts.clear();
  myRow2DataTypeOut.clear();

  QStringList aOPortNames;
  myOutputPortsGroupBox->Table()->strings( NAMECOL, aOPortNames );

  aRowId = 0;
  for ( QStringList::Iterator it = aOPortNames.begin(); it != aOPortNames.end(); ++it )
  {
    if ( (*it).isEmpty() ) continue;

    try {
      if ( OutputPort* aODP = getNode()->getOutputPort((*it).latin1()) )
        {
          DEBTRACE("store port: " << aRowId << " " << aODP);
          myRow2StoredOutPorts.insert( std::make_pair(aRowId,aODP) );
          myRow2DataTypeOut[aRowId]=aODP->edGetType();
        }
    }
    catch (YACS::Exception& ex) {
        std::cerr << "INTERNAL ERROR. Unknown port " << (*it).latin1() << std::endl;
    }
    aRowId++;
  }
}

void YACSGui_InlineNodePage::resetDataTypeMaps()
{
  DEBTRACE("YACSGui_InlineNodePage::resetDataTypeMaps");
//Reset is done in resetStoredPortsMaps
//  myRow2DataTypeIn.clear();
//  myRow2DataTypeOut.clear();
}

void YACSGui_InlineNodePage::resetPLists()
{
  DEBTRACE("YACSGui_InlineNodePage::resetPLists");
  resetIPLists();
  resetOPLists();
}

void YACSGui_InlineNodePage::resetIPLists()
{
  DEBTRACE("YACSGui_InlineNodePage::resetIPLists");
  myIPList.clear();

  QStringList aIPortNames;
  myInputPortsGroupBox->Table()->strings( NAMECOL, aIPortNames );

  for ( QStringList::Iterator it = aIPortNames.begin(); it != aIPortNames.end(); ++it )
  {
    if ( (*it).isEmpty() ) continue;

    try {
      if ( InPort* aIDP = getNode()->getInputPort((*it).latin1()) )
        myIPList.append(aIDP);
    }
    catch (YACS::Exception& ex) {
        std::cerr << "INTERNAL ERROR. Unknown port " << (*it).latin1() << std::endl;
    }
  }
}

void YACSGui_InlineNodePage::resetOPLists()
{
  DEBTRACE("YACSGui_InlineNodePage::resetOPLists");
  myOPList.clear();

  QStringList aOPortNames;
  myOutputPortsGroupBox->Table()->strings( NAMECOL, aOPortNames );

  for ( QStringList::Iterator it = aOPortNames.begin(); it != aOPortNames.end(); ++it )
  {
    if ( (*it).isEmpty() ) continue;

    try {
      if ( OutPort* aODP = getNode()->getOutputPort((*it).latin1()) )
        myOPList.append(aODP);
    }
    catch (YACS::Exception& ex) {
        std::cerr << "INTERNAL ERROR. Unknown port " << (*it).latin1() << std::endl;
    }
  }
}

void YACSGui_InlineNodePage::orderPorts( bool withFilling )
{
  ElementaryNode* aNode = dynamic_cast<ElementaryNode*>( getNode() );
  if ( !aNode ) return;

  QStringList aPortNames;
  int aRowId;

  // order input ports according to the list of ports names in the table
  myInputPortsGroupBox->Table()->strings( NAMECOL, aPortNames );

  if ( withFilling || myIPList.isEmpty() )
  { // copy the list of engine ports into the ptr list
    myIPList.clear();
    list<InPort*> anInPortsEngine = aNode->getSetOfInPort();
    list<InPort*>::iterator anInPortsIter = anInPortsEngine.begin();
    for( ;anInPortsIter!=anInPortsEngine.end();anInPortsIter++)
      myIPList.append(*anInPortsIter);
  }

  aRowId = 0;
  for ( QStringList::Iterator it = aPortNames.begin(); it != aPortNames.end(); ++it )
  {
    if ( (*it).isEmpty() ) continue;

    InPort* anIP = myIPList.at(aRowId);
    if ( !anIP ) return;

    if ( (*it).compare( QString(anIP->getName()) ) ) // diff. order
    {
      bool isDataStream = false;
      InPort* anIPToMove = 0;
      if ( dynamic_cast<InputPort*>(anIP) )
	anIPToMove = aNode->getInputPort((*it).latin1());
      else if ( dynamic_cast<InputDataStreamPort*>(anIP) )
      {
	anIPToMove = aNode->getInputDataStreamPort((*it).latin1());
	isDataStream = true;
      }

      int engineId = -1;
      if ( anIPToMove )
	engineId = myIPList.find( anIPToMove );

      if ( engineId > -1 )
      {
	if ( aRowId < engineId ) // it is needed to move up engine port
	  for ( int i=aRowId; i<engineId; i++ )
	    mySNode->update( UP, 
			     ( !isDataStream ? INPUTPORT : INPUTDATASTREAMPORT ),
			     GuiContext::getCurrent()->_mapOfSubjectDataPort[anIPToMove] );
      	
	if ( aRowId > engineId ) // it is needed to move down engine port
	  for ( int i=engineId; i<aRowId; i++ )
	    mySNode->update( DOWN, 
			     ( !isDataStream ? INPUTPORT : INPUTDATASTREAMPORT ),
			     GuiContext::getCurrent()->_mapOfSubjectDataPort[anIPToMove] );
      }
    }
    
    aRowId++;
  }

  aPortNames.clear();

  // order output ports according to the list of ports names in the table
  myOutputPortsGroupBox->Table()->strings( NAMECOL, aPortNames );

  if ( withFilling || myOPList.isEmpty() )
  { // copy the list of engine ports into the ptr list
    myOPList.clear();
    list<OutPort*> anOutPortsEngine = aNode->getSetOfOutPort();
    list<OutPort*>::iterator anOutPortsIter = anOutPortsEngine.begin();
    for( ;anOutPortsIter!=anOutPortsEngine.end();anOutPortsIter++)
      myOPList.append(*anOutPortsIter);
  }

  aRowId = 0;
  for ( QStringList::Iterator it = aPortNames.begin(); it != aPortNames.end(); ++it )
  {
    if ( (*it).isEmpty() ) continue;

    OutPort* anOP = myOPList.at(aRowId);
    if ( !anOP ) return;

    if ( (*it).compare( QString(anOP->getName()) ) ) // diff. order
    {
      bool isDataStream = false;
      OutPort* anOPToMove = 0;
      if ( dynamic_cast<OutputPort*>(anOP) )
	anOPToMove = aNode->getOutputPort((*it).latin1());
      else if ( dynamic_cast<OutputDataStreamPort*>(anOP) )
      {
	anOPToMove = aNode->getOutputDataStreamPort((*it).latin1());
	isDataStream = true;
      }

      int engineId = -1;
      if ( anOPToMove )
	engineId = myOPList.find( anOPToMove );

      if ( engineId > -1 )
      {
	if ( aRowId < engineId ) // it is needed to move up engine port
	  for ( int i=aRowId; i<engineId; i++ )
	    mySNode->update( UP, 
			     ( !isDataStream ? OUTPUTPORT : OUTPUTDATASTREAMPORT ),
			     GuiContext::getCurrent()->_mapOfSubjectDataPort[anOPToMove] );
	
	if ( aRowId > engineId ) // it is needed to move down engine port
	  for ( int i=engineId; i<aRowId; i++ )
	    mySNode->update( DOWN, 
			     ( !isDataStream ? OUTPUTPORT : OUTPUTDATASTREAMPORT ),
			     GuiContext::getCurrent()->_mapOfSubjectDataPort[anOPToMove] );
      }
    }
    
    aRowId++;
  }

  mySNode->update( UPDATE, 0, mySNode );
}

void YACSGui_InlineNodePage::setInputPorts()
{
  DEBTRACE("YACSGui_InlineNodePage::setInputPorts()");
  if ( !myInputPortsGroupBox ) return;

  ElementaryNode* aNode = dynamic_cast<ElementaryNode*>( getNode() );
  if ( !aNode ) return;

  myInputPortsGroupBox->Table()->stopEdit(true);

  // remove old ports
  list<InPort*> anInPortsEngine = aNode->getSetOfInPort();
  list<InPort*>::iterator anInPortsIter = anInPortsEngine.begin();
  for( ;anInPortsIter!=anInPortsEngine.end();anInPortsIter++)
    if ( !isStored(*anInPortsIter) )
    {
      DEBTRACE("remove port " << *anInPortsIter);
      myIPList.take(myIPList.find(*anInPortsIter));
      mySNode->destroy( GuiContext::getCurrent()->_mapOfSubjectDataPort[*anInPortsIter] );
    }
  
  // read input data and input data stream ports from the table
  YACSGui_Table* aTable = myInputPortsGroupBox->Table();
  
  QStringList aPortNames;
  QStringList aValues;

  aTable->strings( NAMECOL, aPortNames );
  aTable->strings( VALUECOL, aValues );

  std::list<InputPort*> myPorts;

  int aRowId = 0;
  for ( QStringList::Iterator it = aPortNames.begin(); it != aPortNames.end(); ++it,++aRowId)
  {
    DEBTRACE(aRowId << " port name " << *it);
    if ( (*it).isEmpty() )
      {
        SUIT_MessageBox::warn1(getInputPanel()->getModule()->getApp()->desktop(),
                               tr("WRN_WARNING"),
                               "an input port has no name",
                               tr("BUT_OK"));
        onApplyStatus="ERROR";
        continue;
      }

    if ( aPortNames.contains(*it) > 1 )
    {
      std::string what("The node ");
      what += mySNode->getName();
      what += " contains two or more "; 
      what+=(*it).latin1();
      what+=" input ports";
      SUIT_MessageBox::warn1(getInputPanel()->getModule()->getApp()->desktop(),
                             tr("WRN_WARNING"),
                             what,
                             tr("BUT_OK"));
      onApplyStatus="ERROR";
      continue;
    }
    
    // retrieve a type name of the input port as a string
    std::string aType = "";

    if (QTableItem* anItem = aTable->item( aRowId, TYPECOL ) )
    {
      if ( QComboBox* aCB = dynamic_cast<QComboBox*>(aTable->cellWidget( aRowId, TYPECOL )) )
      {
	if ( aCB->lineEdit() )
	  aType = aCB->lineEdit()->text().latin1();
      }
      else
	aType = anItem->text().latin1();
    }
    
    DEBTRACE(aType);
    TypeCode* aTC = 0;
    try
      {
        aTC=GuiContext::getCurrent()->getProc()->getTypeCode(aType);
      }
    catch(YACS::Exception&)
      {
        std::cerr << "Problem with data types initialization" << std::endl;
        continue;
      }

    GuiEvent anEvent = EDIT;
      DEBTRACE("Data flow port");
      InputPort* aIDP = 0;
      if ( myRow2StoredInPorts.find(aRowId) != myRow2StoredInPorts.end() )
      { // ---- update port ----
        DEBTRACE("Update port");
	if ( aIDP = dynamic_cast<InputPort*>(myRow2StoredInPorts[aRowId]) )
	{
	  aIDP->setName(*it);
          //change type if needed
	  TypeCode* theTC = aIDP->edGetType();
          if(!aTC->isEquivalent(theTC))
            {
	      aIDP->edSetType(aTC);
	      aIDP->edRemoveManInit();
            }
	}
      }
      else
      { // ---- create port ----
        DEBTRACE("Create port");
        try
          {
            DEBTRACE(*it << " " << aTC);
            SubjectElementaryNode* sen=dynamic_cast<SubjectElementaryNode*>(mySNode);
            SubjectInputPort* sip=sen->edAddInputPort(*it,aTC);
            DataPort* dp=sip->getPort();
            aIDP=dynamic_cast<InputPort*>(dp);
          }
        catch(YACS::Exception& ex)
          {
            SUIT_MessageBox::warn1(getInputPanel()->getModule()->getApp()->desktop(), tr("WRN_WARNING"),
                             ex.what(), tr("BUT_OK"));
            onApplyStatus="ERROR";
            continue;
          }

	myIPList.append(aIDP);
        myRow2StoredInPorts[aRowId]=aIDP;
        myRow2DataTypeIn[aRowId]=aTC;
      }

      if ( aIDP )
        myPorts.push_back(aIDP);

      if ( aIDP )
      {
#ifdef _DEVDEBUG_
        if(!aValues[aRowId].isNull())
          std::cerr << aValues[aRowId] << std::endl;
        else
          std::cerr << "Qnull" << std::endl;
#endif

	// initialize new created input data port
        double d;
        int ii;
        bool ok;
	switch ( aTC->kind() )
	{
	case Double:
          d=aValues[aRowId].toDouble(&ok);
          if(ok)
	    aIDP->edInit(d);
	  anEvent = EDIT;
	  break;
	case Int:
	  ii= aValues[aRowId].toInt(&ok) ;
          if(ok)
	    aIDP->edInit(ii);
	  anEvent = EDIT;
	  break;
	case String:
          if(aValues[aRowId] != "< ? >")
	    aIDP->edInit( aValues[aRowId].isEmpty() ? "" : aValues[aRowId].latin1() );
	  anEvent = EDIT;
	  break;
	case Bool:
          if(aValues[aRowId] != "< ? >")
	    aIDP->edInit( aValues[aRowId].compare( aTable->Params(aRowId,VALUECOL)[0] ) ? false : true );
	  anEvent = EDIT;
	  break;
	default:
          PyObject* ob;
          if(aValues[aRowId].isEmpty()||aValues[aRowId] == "< ? >")
            break;

          try
            {
              ob=YACS::ENGINE::getSALOMERuntime()->convertStringToPyObject(aValues[aRowId]);
            }
          catch(Exception& ex)
            {
              SUIT_MessageBox::error1(getInputPanel()->getModule()->getApp()->desktop(), 
                                      tr("ERROR"),ex.what() , tr("BUT_OK"));
              onApplyStatus="ERROR";
              break;
            }
          PyGILState_STATE gstate = PyGILState_Ensure();
          try
            {
              aIDP->edInit("Python",ob);
	      anEvent = EDIT;
            }
          catch(Exception& ex)
            {
              SUIT_MessageBox::error1(getInputPanel()->getModule()->getApp()->desktop(), 
                                      tr("ERROR"),ex.what() , tr("BUT_OK"));
              onApplyStatus="ERROR";
            }
          Py_DECREF(ob);
          PyGILState_Release(gstate);
	  break;
	}
      
	if ( anEvent == EDIT )
	  mySNode->update( EDIT, INPUTPORT, GuiContext::getCurrent()->_mapOfSubjectDataPort[aIDP]);
      }
  }

  //Reorder ports
  try
    {
      aNode->edOrderInputPorts(myPorts);
      mySNode->update( UPDATE, 0, mySNode );
    }
  catch(YACS::Exception& ex)
    {
      std::cerr << ex.what() << std::endl;
    }
}

void YACSGui_InlineNodePage::setOutputPorts()
{
  DEBTRACE("YACSGui_InlineNodePage::setOutputPorts()");
  if ( !myOutputPortsGroupBox ) return;

  ElementaryNode* aNode = dynamic_cast<ElementaryNode*>( getNode() );
  if ( !aNode ) return;

  myOutputPortsGroupBox->Table()->stopEdit(true);

  // remove old ports
  list<OutPort*> anOutPortsEngine = aNode->getSetOfOutPort();
  list<OutPort*>::iterator anOutPortsIter = anOutPortsEngine.begin();
  for( ;anOutPortsIter!=anOutPortsEngine.end();anOutPortsIter++)
    if ( !isStored(*anOutPortsIter) )
    {
      myOPList.take(myOPList.find(*anOutPortsIter));
      mySNode->destroy( GuiContext::getCurrent()->_mapOfSubjectDataPort[*anOutPortsIter] );
    }
  
  // read output data and output data stream ports from the table
  YACSGui_Table* aTable = myOutputPortsGroupBox->Table();
  
  QStringList aPortNames;
  QStringList aValues;

  aTable->strings( NAMECOL, aPortNames );

  std::list<OutputPort*> myPorts;

  int aRowId = 0;
  for ( QStringList::Iterator it = aPortNames.begin(); it != aPortNames.end(); ++it,++aRowId )
  {
    if ( (*it).isEmpty() ) 
      {
        SUIT_MessageBox::warn1(getInputPanel()->getModule()->getApp()->desktop(),
                               tr("WRN_WARNING"),
                               "an output port has no name",
                               tr("BUT_OK"));
        onApplyStatus="ERROR";
        continue;
      }

    if ( aPortNames.contains(*it) > 1 )
    {
      std::string what("The node ");
      what += mySNode->getName();
      what += " contains two or more "; 
      what+=(*it).latin1();
      what+=" output ports";
      SUIT_MessageBox::warn1(getInputPanel()->getModule()->getApp()->desktop(),
                             tr("WRN_WARNING"),
                             what,
                             tr("BUT_OK"));
      onApplyStatus="ERROR";
      continue;
    }
    
    // retrieve a type name of the input port as a string
    std::string aType = "";

    if (QTableItem* anItem = aTable->item( aRowId, TYPECOL ) )
    {
      if ( QComboBox* aCB = dynamic_cast<QComboBox*>(aTable->cellWidget( aRowId, TYPECOL )) )
      {
	if ( aCB->lineEdit() )
	  aType = aCB->lineEdit()->text().latin1();
      }
      else
	aType = anItem->text().latin1();
    }

    DEBTRACE(aType);
    TypeCode* aTC = 0;
    try
      {
        aTC=GuiContext::getCurrent()->getProc()->getTypeCode(aType);
      }
    catch(YACS::Exception&)
      {
        std::cerr << "Problem with data types initialization" << std::endl;
        continue;
      }
    
    GuiEvent anEvent = EDIT;
      OutputPort* aODP = 0;
      if ( myRow2StoredOutPorts.find(aRowId) != myRow2StoredOutPorts.end() )
      { // ---- update port ----
        DEBTRACE("---- update port ---- " << *it);
	if ( aODP = dynamic_cast<OutputPort*>(myRow2StoredOutPorts[aRowId]) )
	{
	  aODP->setName(*it);
          //change type if needed
	  TypeCode* theTC = aODP->edGetType();
          if(!aTC->isEquivalent(theTC))
            {
	      aODP->edSetType(aTC);
            }
	}
      }
      else
      { // ---- create port ----
        DEBTRACE("---- create port ---- " << *it);
        try
          {
            DEBTRACE(*it << " " << aTC);
            SubjectElementaryNode* sen=dynamic_cast<SubjectElementaryNode*>(mySNode);
            SubjectOutputPort* sop=sen->edAddOutputPort(*it,aTC);
            DataPort* dp=sop->getPort();
            aODP=dynamic_cast<OutputPort*>(dp);
          }
        catch(YACS::Exception& ex)
          {
            SUIT_MessageBox::warn1(getInputPanel()->getModule()->getApp()->desktop(), tr("WRN_WARNING"),
                             ex.what(), tr("BUT_OK"));
            onApplyStatus="ERROR";
            continue;
          }
	myOPList.append(aODP);
        myRow2StoredOutPorts[aRowId]=aODP;
        myRow2DataTypeOut[aRowId]=aTC;
      }

      if ( aODP )
        myPorts.push_back(aODP);

      if ( aODP && anEvent == EDIT )
	mySNode->update( EDIT, OUTPUTPORT, GuiContext::getCurrent()->_mapOfSubjectDataPort[aODP]);
  }

  //Reorder ports
  try
    {
      aNode->edOrderOutputPorts(myPorts);
      mySNode->update( UPDATE, 0, mySNode );
    }
  catch(YACS::Exception& ex)
    {
      std::cerr << ex.what() << std::endl;
    }
}

YACS::ENGINE::TypeCode* YACSGui_InlineNodePage::createTypeCode( YACS::ENGINE::DynType theType,
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
      if ( theTable->intValueCombo( CLASSCOL, theRowId ) == 2 ) // Data Stream (CALCIUM) port
	aTC = new TypeCodeObjref("CALCIUM","CALCIUM");
      else if ( theTable->intValueCombo( CLASSCOL, theRowId ) == 1 ) // Data Stream (BASIC) port
	aTC = new TypeCodeObjref("BASIC","BASIC");
      else if ( theTable->intValueCombo( CLASSCOL, theRowId ) == 3 ) // Data Stream (PALM) port
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

QString YACSGui_InlineNodePage::scriptText() const
{
#ifdef HAVE_QEXTSCINTILLA_H
  return _myTextEdit->text();
#else
  return myTextEdit->text();
#endif
}

void YACSGui_InlineNodePage::setScriptText( const QString& txt )
{
#ifdef HAVE_QEXTSCINTILLA_H
  _myTextEdit->setText( txt );
#else
  myTextEdit->setText( txt );
#endif
}

bool YACSGui_InlineNodePage::isPortsModified( bool theInput )
{
  bool isModified = false;
  
  if ( !getNode() ) return isModified;
  // read data and data stream ports from the table
  YACSGui_Table* aTable = 0;
  if ( theInput ) aTable = myInputPortsGroupBox->Table();
  else aTable = myOutputPortsGroupBox->Table();
  if ( !aTable ) return isModified;
  aTable->stopEdit(true);
    
  QStringList aPortNames;
  aTable->strings( NAMECOL, aPortNames );

  if ( ( theInput ? aPortNames.count() != getNode()->getNumberOfInputPorts() :
	            aPortNames.count() != getNode()->getNumberOfOutputPorts() ) ) isModified = true;
  else
  {
    QStringList aValueTypes;
    QStringList aValues;
    
    aTable->strings( TYPECOL, aValueTypes );
    aTable->strings( VALUECOL, aValues );
      
    int aRowId = 0;
    for ( QStringList::Iterator it = aPortNames.begin(); it != aPortNames.end(); ++it )
    {
      Port* aPort = 0;
      try {
	aPort = ( theInput ? (Port*)(getNode()->getInPort(*it)) : (Port*)(getNode()->getOutPort(*it)) );
      }
      catch (...) {}
      
      if ( aPort )
      {
        DEBTRACE("isPortsModified: " << getPortValue( aPort ) << " " << getPortType( aPort ) );
        QString value=aValues[aRowId];
        QString valtype=aValueTypes[aRowId];
        if(value.isNull())
          value="< ? >";
        if(valtype.isNull())
          valtype="< ? >";
        DEBTRACE(value<<","<<valtype);

	// check value type and value (for input ports only) of the port
	if ( theInput && getPortValue( aPort ).compare(value) != 0
	     ||
	     getPortType( aPort ).compare(valtype) != 0 )
	{
	  isModified = true;
	  break;
	}
	
      }
      else // this case means that we renamed any port or added a new one (i.e. check port name)
      {
	isModified = true;
	break;
      }
      aRowId++;
    }
  }
  return isModified;
}

void YACSGui_InlineNodePage::setValueCellValidator( YACSGui_Table* theTable, int theRow )
{
  DEBTRACE("YACSGui_NodePage::setValueCellValidator " << theRow);
  QString aVT = theTable->item( theRow, TYPECOL )->text();
  QString aValue = theTable->item( theRow, VALUECOL )->text();
  theTable->setDefValue( theRow, VALUECOL, aValue );
  if ( !aVT.compare(QString("double")) )
    theTable->setCellType( theRow, VALUECOL, YACSGui_Table::Double );
  else if ( !aVT.compare(QString("int")) )
    theTable->setCellType( theRow, VALUECOL, YACSGui_Table::Int );
  else if ( !aVT.compare(QString("bool")) )
  {
    theTable->setCellType( theRow, VALUECOL, YACSGui_Table::Combo );
    theTable->setParams( theRow, VALUECOL, QString("True;False") );
  }
  else // i.e. "string" or "Objref" or "Sequence" or "Array" or "Struct"
    theTable->setCellType( theRow, VALUECOL, YACSGui_Table::String );
}

void YACSGui_InlineNodePage::removePort(YACS::HMI::SubjectDataPort* thePort)
{
  DEBTRACE("YACSGui_InlineNodePage::removePort");
  if ( !getNode() ) return;
  DataPort* dp=thePort->getPort();
  YACSGui_Table* aTable = 0;
  if(dynamic_cast<InPort*>(dp))
    {
      aTable = myInputPortsGroupBox->Table();
    }
  else
    {
      aTable = myOutputPortsGroupBox->Table();
    }
  if ( !aTable ) return;
  QStringList aPortNames;
  aTable->strings( NAMECOL, aPortNames );
  int theRow;
  theRow = aPortNames.findIndex(dp->getName());
  DEBTRACE(theRow);
  if(theRow==-1)return;//already removed
  aTable->removeRow(theRow);
}
