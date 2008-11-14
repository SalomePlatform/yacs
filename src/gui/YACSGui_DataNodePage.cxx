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
#include <DataNode.hxx>
#include <StudyPorts.hxx>
#include <ServiceNode.hxx>
//#include <ServiceInlineNode.hxx>
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
#include <qtabwidget.h>

#include <sstream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

// #define SPACING 5
// #define MARGIN 5

using namespace YACS;
using namespace YACS::ENGINE;
using namespace YACS::HMI;
using namespace std;

YACSGui_DataNodePage::YACSGui_DataNodePage( int pageId,
                                            QWidget* theParent,
                                            const char* theName,
                                            WFlags theFlags)
  : DataNodePage( theParent, theName, theFlags ),
    YACSGui_NodePage(),
    myIsNeedToReorder(false),
    myPageId(pageId)
{
  if ( !myInputPortsGroupBox || !myOutputPortsGroupBox ) return;

  _isInput = true;
  _isStudy = false;
  _fileName = "";
  _studyName = "";
  _studyId = 0;
  switch (myPageId)
    {
    case YACSGui_InputPanel::InDataNodeId:
      {
        _isInput = true;
        _isStudy = false;
        myNodeType->setText("input data node");
        break;
      }
    case YACSGui_InputPanel::OutDataNodeId:
      {
        _isInput = false;
        _isStudy = false;
        myNodeType->setText("output data node");
        break;
      }
    case YACSGui_InputPanel::InStudyNodeId:
      {
        _isInput = true;
        _isStudy = true;
        myNodeType->setText("input study node");
        break;
      }
    case YACSGui_InputPanel::OutStudyNodeId:
      {
        _isInput = false;
        _isStudy = true;
        myNodeType->setText("output study node");
        break;
      }
    default:
      return;
    }

  QString aPortTypes = QString("Data Flow"); //";Data Stream (BASIC);Data Stream (CALCIUM);Data Stream (PALM)");
  QString aValueTypes = QString("double;int;string;bool");

  // Input Ports table
  myInputPortsGroupBox->setTitle( tr("Input Ports") );
  myInputPortsGroupBox->HideBtn( YACSGui_PlusMinusGrp::SelectBtn );
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
  aTable->setCellType( -1, 2, YACSGui_Table::ComboSelect );
  //aTable->setCellType( -1, 3, YACSGui_Table::??? ); depends on combo box item choosen in the "Value type" column
  
  aTable->setParams( 0, 1, aPortTypes );
  aTable->setParams( 0, 2, aValueTypes );

  aTable->setDefValue( 0, 1, "Data Flow");
  aTable->setDefValue( 0, 2, "double");

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
  aTable->setCellType( -1, 2, YACSGui_Table::ComboSelect );
  //aTable->setCellType( -1, 3, YACSGui_Table::??? ); depends on combo box item choosen in the "Value type" column
  aTable->setCellType( -1, 4, YACSGui_Table::Combo );

  aTable->setParams( 0, 1, aPortTypes );
  aTable->setParams( 0, 2, aValueTypes );
  aTable->setParams( 0, 4, QString("Yes;No") );

  aTable->setDefValue( 0, 1, "Data Flow");
  aTable->setDefValue( 0, 2, "double");
  aTable->setDefValue( 0, 4, QString("Yes") );

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

  if (_isInput)
    {
      ports->removePage(tab);
    }
  else
    {
      ports->removePage(tab_2);
    }
  if (_isStudy)
    {
      DataNodeFrameLayout->remove(FileGroupBox);
      FileGroupBox->hide();
    }
  else
    {
      DataNodeFrameLayout->remove(StudyGroupBox);
      StudyGroupBox->hide();
    }
}

YACSGui_DataNodePage::~YACSGui_DataNodePage()
{
  if (getInputPanel()) getInputPanel()->removePage(this);
}

void YACSGui_DataNodePage::setSNode( YACS::HMI::SubjectNode* theSNode )
{
  if ( !theSNode ) return;

  if ( dynamic_cast<DataNode*>( theSNode->getNode() ) )
  {
    YACSGui_NodePage::setSNode( theSNode );
    myIPList.clear();
    myOPList.clear();
  }
}

void YACSGui_DataNodePage::setMode( const YACSGui_InputPanel::PageMode theMode )
{
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
    aTable->setReadOnly( -1, 0, false );
    aTable->setReadOnly( -1, 1, false );
    aTable->setReadOnly( -1, 2, false );
//     aTable->setReadOnly( -1, 3, true );

    myOutputPortsGroupBox->ShowBtn();
    myOutputPortsGroupBox->HideBtn( YACSGui_PlusMinusGrp::SelectBtn );
    aTable = myOutputPortsGroupBox->Table();
    aTable->setReadOnly( -1, 0, false );
    aTable->setReadOnly( -1, 1, false );
    aTable->setReadOnly( -1, 2, false );
//     aTable->setReadOnly( -1, 3, false );

//     InPythonEditorGroupBox->show();
  } 
  else if ( myMode == YACSGui_InputPanel::RunMode )
  {
    myNodeName->setReadOnly(true);

    ExecutionGroupBox->show();

    EditPortsGroupBox->setTitle( tr( "Port List" ) );

    YACSGui_Table* aTable;

    myInputPortsGroupBox->HideBtn();
    aTable = myInputPortsGroupBox->Table();
    aTable->setReadOnly( -1, 0, true );
    aTable->setReadOnly( -1, 1, true );
    aTable->setReadOnly( -1, 2, true );
    
    myOutputPortsGroupBox->HideBtn();
    aTable = myOutputPortsGroupBox->Table();
    aTable->setReadOnly( -1, 0, true );
    aTable->setReadOnly( -1, 1, true );
    aTable->setReadOnly( -1, 2, true );

//     InPythonEditorGroupBox->hide();
  }
}

void YACSGui_DataNodePage::notifyNodeStatus( int theStatus )
{
  if ( myNodeState )
  {
    myNodeState->setPaletteForegroundColor(getColor(theStatus));
    myNodeState->setText(getText(theStatus));
  }
}

void YACSGui_DataNodePage::notifyNodeProgress()
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

void YACSGui_DataNodePage::notifyInPortValues( std::map<std::string,std::string> theInPortName2Value )
{
  DEBTRACE("YACSGui_DataNodePage::notifyInPortValues" );
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

void YACSGui_DataNodePage::notifyOutPortValues( std::map<std::string,std::string> theOutPortName2Value )
{
  DEBTRACE("YACSGui_DataNodePage::notifyOutPortValues" );
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

void YACSGui_DataNodePage::setDataType( YACS::ENGINE::TypeCode* theDataType )
{
  DEBTRACE("YACSGui_DataNodePage::setDataType: " << theDataType->name());
  if ( mySelectDataTypeFor.myRow < 0 ) return;

  YACSGui_Table* aTable = 0;
  if ( mySelectDataTypeFor.myIn )
  {
    aTable = myInputPortsGroupBox->Table();
    
    if ( myRow2DataTypeIn.find(mySelectDataTypeFor.myRow) != myRow2DataTypeIn.end() )
      myRow2DataTypeIn[mySelectDataTypeFor.myRow] = theDataType;
    else
      myRow2DataTypeIn.insert(make_pair(mySelectDataTypeFor.myRow,theDataType));
    DEBTRACE(theDataType->name());
  }
  else
  {
    aTable = myOutputPortsGroupBox->Table();
    
    if ( myRow2DataTypeOut.find(mySelectDataTypeFor.myRow) != myRow2DataTypeOut.end() )
      myRow2DataTypeOut[mySelectDataTypeFor.myRow] = theDataType;
    else
      myRow2DataTypeOut.insert(make_pair(mySelectDataTypeFor.myRow,theDataType));
    DEBTRACE(theDataType->name());
  }

  if ( aTable )
  {
    // update the content of the cell according to the selected data type
    if ( YACSGui_TableComboSelectItem* aCSItem = 
    	 dynamic_cast<YACSGui_TableComboSelectItem*>(aTable->item( mySelectDataTypeFor.myRow, 2 )) )
    {
      if( aCSItem->selectButton() && aCSItem->selectButton()->isOn() )
	aCSItem->selectButton()->setOn( false );

      QString aTypeName(theDataType->name());
      aCSItem->showText(aTypeName);
      aTable->setDefValue( mySelectDataTypeFor.myRow, 2, aTypeName );
    }
    
    mySelectDataTypeFor.myRow = -1;
  }
}

void YACSGui_DataNodePage::checkModifications()
{
  DEBTRACE("YACSGui_DataNodePage::checkModifications");
  if ( !getNode() ) return;

  // 1) check if the content of the page is really modified (in compare with the content of engine object)
  bool isModified = false;
  if ( myNodeName->text().compare(getNodeName()) != 0 ) isModified = true;
  DEBTRACE(isModified);

  if ( !isModified ) isModified = isPortsModified();
  DEBTRACE(isModified);
  if ( !isModified ) isModified = isPortsModified(false);
  DEBTRACE(isModified);

  if ( !isModified )
    {
      if (_isStudy)
        {
          if (QString::compare(_studyName, myStudyName->text()) != 0) isModified = true;
          else if (_studyId != myStudyId->value()) isModified = true;
        }
      else
        {
          if (QString::compare(_fileName,myFileName->text()) != 0) isModified = true;
        }
    }

  // 2) if yes, show a warning message: Apply or Cancel
  if ( isModified )
    if ( SUIT_MessageBox::warn2(getInputPanel()->getModule()->getApp()->desktop(),
				tr("WRN_WARNING"),
				tr("APPLY_CANCEL_MODIFICATIONS"),
				tr("BUT_YES"), tr("BUT_NO"), 0, 1, 0) == 0 )
    {
      onApply();
      if ( getInputPanel() ) getInputPanel()->emitApply(YACSGui_InputPanel::DataNodeId);
    }
}

void YACSGui_DataNodePage::onApply()
{
  DEBTRACE("YACSGui_DataNodePage::onApply");
  // Rename a node
  if ( myNodeName )
    setNodeName( myNodeName->text() );
  
  bool withFilling = ( myIPList.isEmpty() || myOPList.isEmpty() );

  // Reset the list of input ports
  setInputPorts();
  
  // Reset the list of output ports
  setOutputPorts();

  if ( myIsNeedToReorder )
  {
    orderPorts(withFilling);
    myIsNeedToReorder = false;
  }

  // set file or study
  if (_isStudy)
    {
      _studyName =  myStudyName->text();
      _studyId = myStudyId->value();
    }
  else
    {
      _fileName = myFileName->text();
    }
  setProperties();
  
  resetStoredPortsMaps();
  resetDataTypeMaps();
  resetPLists();

  updateBlocSize();
}

//! Update widget state from node state
/*!
 *
 */
void YACSGui_DataNodePage::updateState()
{
  DEBTRACE("YACSGui_DataNodePage::updateState()");
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
//   if ( DataNode* anIN = dynamic_cast<DataNode*>( getNode() ) )
//     setScriptText( QString(anIN->getScript()) );
}

void YACSGui_DataNodePage::onNodeNameChanged( const QString& theName )
{
  if ( myNodeFullName )
  {
    QString anOldName = myNodeFullName->text();
    int aRDotId = anOldName.findRev('.');
    myNodeFullName->setText( anOldName.replace( aRDotId+1, anOldName.length()-(aRDotId+1), theName ) );
  }
}

void YACSGui_DataNodePage::onValueChanged( int theRow, int theCol )
{
  DEBTRACE("YACSGui_DataNodePage::onValueChanged "<< theRow << " " << theCol);
  if ( YACSGui_Table* aTable = ( YACSGui_Table* )sender() )
  {
    if ( theCol == 1 ) // the port type was changed
    {
      if( aTable == myInputPortsGroupBox->Table() )
	aTable->setReadOnly( theRow, 3, true );
    }

    if ( theCol == 2 ) // the value type of the port was changed
    {
      setValueCellValidator( aTable, theRow );

      if( aTable == myOutputPortsGroupBox->Table() )
	aTable->setReadOnly( theRow, 3, false );
    }

    if ( theCol == 1 || theCol == 2 ) // port type or value type was changed
    {
      if ( myInputPortsGroupBox->Table() == aTable )
      	if ( myRow2StoredInPorts.find(theRow) != myRow2StoredInPorts.end() )
	  myRow2StoredInPorts.erase(theRow);

      if ( myOutputPortsGroupBox->Table() == aTable )
      	if ( myRow2StoredOutPorts.find(theRow) != myRow2StoredOutPorts.end() )
	  myRow2StoredOutPorts.erase(theRow);
    }
  }
}

void YACSGui_DataNodePage::hideEvent( QHideEvent*)
{
  myPara = 0;
  myIndex = 0;
}

void YACSGui_DataNodePage::onInserted( const int theRow )
{
  DEBTRACE("YACSGui_DataNodePage::onInserted: " << theRow);
  if ( YACSGui_PlusMinusGrp* aGrpBox = ( YACSGui_PlusMinusGrp* )sender() )
    if ( myInputPortsGroupBox == aGrpBox )
    {
      // change the content of myRow2StoredInPorts
      if ( myRow2StoredInPorts.size() == 0 )
        return;

      if ( myRow2StoredInPorts.size() == 1 )
      {
	if ( theRow == 0 )
	{
	  InPort* aPort = myRow2StoredInPorts[0];
	  myRow2StoredInPorts.insert( std::make_pair(1,aPort) );
	  myRow2StoredInPorts.erase(0);
	} 
      }
      else 
      {
	map<int,InPort*>::iterator it = myRow2StoredInPorts.end();
	map<int,InPort*>::iterator itBeforeBegin = myRow2StoredInPorts.begin()--;
	for ( it--; it != itBeforeBegin; it-- )
	{
	  int aRow = (*it).first;
	  InPort* aPort = (*it).second;
	  
	  if ( aRow >= theRow )
	  {
	    myRow2StoredInPorts.insert( std::make_pair(aRow+1,aPort) );
	    myRow2StoredInPorts.erase(aRow);
	  } 
	}
      }

      // change the content of myRow2DataTypeIn
      if ( myRow2DataTypeIn.size() == 0 )
        return;

      if ( myRow2DataTypeIn.size() == 1 )
      {
	if ( theRow == 0 )
	{
	  TypeCode* aTC = myRow2DataTypeIn[0];
	  myRow2DataTypeIn.insert( std::make_pair(1,aTC) );
	  myRow2DataTypeIn.erase(0);
	} 
      }
      else 
      {
	map<int,TypeCode*>::iterator it = myRow2DataTypeIn.end();
	map<int,TypeCode*>::iterator itBeforeBegin = myRow2DataTypeIn.begin()--;
	for ( it--; it != itBeforeBegin; it-- )
	{
	  int aRow = (*it).first;
	  TypeCode* aTC = (*it).second;
	  
	  if ( aRow >= theRow )
	  {
	    myRow2DataTypeIn.insert( std::make_pair(aRow+1,aTC) );
	    myRow2DataTypeIn.erase(aRow);
	  } 
	}
      }

      myIsNeedToReorder = true;
    }
    else if ( myOutputPortsGroupBox == aGrpBox )
    {
      // change the content of myRow2StoredOutPorts
      if ( myRow2StoredOutPorts.size() == 0 )
        return;

      if ( myRow2StoredOutPorts.size() == 1 )
      {
	if ( theRow == 0 )
	{
	  OutPort* aPort = myRow2StoredOutPorts[0];
	  myRow2StoredOutPorts.insert( std::make_pair(1,aPort) );
	  myRow2StoredOutPorts.erase(0);
	} 
      }
      else
      {
	map<int,OutPort*>::iterator it = myRow2StoredOutPorts.end();
	map<int,OutPort*>::iterator itBeforeBegin = myRow2StoredOutPorts.begin()--;
	for ( it--; it != itBeforeBegin; it-- )
	{
	  int aRow = (*it).first;
	  OutPort* aPort = (*it).second;
	  
	  if ( aRow >= theRow )
	  {
	    myRow2StoredOutPorts.insert( std::make_pair(aRow+1,aPort) );
	    myRow2StoredOutPorts.erase(aRow);
	  } 
	}
      }

      // change the content of myRow2DataTypeOut
      if ( myRow2DataTypeOut.size() == 0 )
        return;

      if ( myRow2DataTypeOut.size() == 1 )
      {
	if ( theRow == 0 )
	{
	  TypeCode* aTC = myRow2DataTypeOut[0];
	  myRow2DataTypeOut.insert( std::make_pair(1,aTC) );
	  myRow2DataTypeOut.erase(0);
          DEBTRACE(aTC->name());
	} 
      }
      else
      {
	map<int,TypeCode*>::iterator it = myRow2DataTypeOut.end();
	map<int,TypeCode*>::iterator itBeforeBegin = myRow2DataTypeOut.begin()--;
	for ( it--; it != itBeforeBegin; it-- )
	{
	  int aRow = (*it).first;
	  TypeCode* aTC = (*it).second;
	  
	  if ( aRow >= theRow )
	  {
	    myRow2DataTypeOut.insert( std::make_pair(aRow+1,aTC) );
	    myRow2DataTypeOut.erase(aRow);
            DEBTRACE(aTC->name());
	  } 
	}
      }

      myIsNeedToReorder = true;
    }
}

void YACSGui_DataNodePage::onMovedUp( const int theUpRow )
{
  DEBTRACE("YACSGui_DataNodePage::onMovedUp: " << theUpRow);
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

void YACSGui_DataNodePage::onMovedDown( const int theDownRow )
{
  DEBTRACE("YACSGui_DataNodePage::onMovedDown: " << theDownRow);
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

void YACSGui_DataNodePage::onRemoved( const int theRow )
{
  DEBTRACE("YACSGui_DataNodePage::onRemoved " << theRow);
  if ( YACSGui_PlusMinusGrp* aGrpBox = ( YACSGui_PlusMinusGrp* )sender() )
    if ( myInputPortsGroupBox == aGrpBox )
    {
      DEBTRACE("Change input ports");
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
      DEBTRACE("Change output ports");
      // change the content of myRow2StoredOutPorts
      int aRow;
      DEBTRACE(myRow2StoredOutPorts.size());
      for(aRow=theRow;aRow<myRow2StoredOutPorts.size()-1;aRow++)
        {
          DEBTRACE("myRow2StoredOutPorts["<<aRow<<"]="<< myRow2StoredOutPorts[aRow+1]);
          myRow2StoredOutPorts[aRow]=myRow2StoredOutPorts[aRow+1];
        }
      DEBTRACE("erase last row: " << aRow);

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

void YACSGui_DataNodePage::onAdded( const int theRow )
{
  DEBTRACE("YACSGui_DataNodePage::onAdded: " << theRow);
  if ( YACSGui_PlusMinusGrp* aGrpBox = ( YACSGui_PlusMinusGrp* )sender() )
    if ( myInputPortsGroupBox == aGrpBox )
    {
      myInputPortsGroupBox->Table()->setDefValue( theRow, 2, "" );
      myInputPortsGroupBox->Table()->item( theRow, 2 )->setText("double");
    }
    else if ( myOutputPortsGroupBox == aGrpBox )
    {
      myOutputPortsGroupBox->Table()->setDefValue( theRow, 2, "" );
      myOutputPortsGroupBox->Table()->item( theRow, 2 )->setText("double");
    }
}

void YACSGui_DataNodePage::onSelectDataType( const int theRow, const int theCol )
{
  DEBTRACE("YACSGui_DataNodePage::onSelectDataType: " << theRow << " " << theCol);
  if ( theCol == 2 && theRow >= 0 ) // the value type selection
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

//! Update input port table state from node state
/*!
 *
 */
void YACSGui_DataNodePage::fillInputPortsTable()
{
  DEBTRACE("YACSGui_DataNodePage::fillInputPortsTable()");
  if ( !getNode() ) return;

  myRow2StoredInPorts.clear();
  myRow2DataTypeIn.clear();

  QStringList aPortNames;
  QStringList aPortTypes;
  QStringList aValueTypes;
  QStringList aValues;

  QValueList<bool> aReadOnlyFlags;

  YACSGui_Table* aTable = myInputPortsGroupBox->Table();
  QStringList aPTCB = aTable->Params( 0, 1 );

  bool isStudyNode=false;
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

    // Port type
    if ( InputStudyPort* anInputP = dynamic_cast<InputStudyPort*>(anInPort) )
      {
        isStudyNode=true;
        aPortTypes.append( aPTCB[0] );
        aReadOnlyFlags.append( false );
      }
    else if ( InputPort* anInputP = dynamic_cast<InputPort*>(anInPort) )
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
    else
    { // take default value from combo box
      aPortTypes.append( aPTCB[0] );
      aReadOnlyFlags.append( true );
    }

    // Value type
    aValueTypes.append( getPortType( anInPort ) );

    // Value
    aValues.append( getPortValue( anInPort ) );

    aRowId++;
  }

  // Fill "Port name" column
  aTable->setStrings( 0, aPortNames, true );

  // Fill "Port type" column
  aTable->setStrings( 1, aPortTypes, true );

  // Fill "Value type" column
  aTable->setStrings( 2, aValueTypes, true );

  //Do not set validators for study as all values are strings whatever is the type
  if(isStudyNode)
    for (int irow=0; irow < aTable->numRows(); irow++)
      aTable->setCellType( irow, 3, YACSGui_Table::String );

  // Fill "Value" column
  aTable->setStrings( 3, aValues, true );

  // Set "Value" column read only (for linked ports)
  // no read only for study node only for data node
  for ( int i = 0, n = aTable->numRows(); i < n; i++ )
    {
      aTable->setDefValue( i, 2, aValueTypes[i] );
      aTable->setReadOnly( i, 3, aReadOnlyFlags[i]  );
    }

  if ( !aPortNames.empty() ) myInputPortsGroupBox->EnableBtn( YACSGui_PlusMinusGrp::AllBtn );
}

//! Update output port table state from node state
/*!
 *
 */
void YACSGui_DataNodePage::fillOutputPortsTable()
{
  DEBTRACE("YACSGui_DataNodePage::fillOutputPortsTable()" );
  if ( !getNode() ) return;

  myRow2StoredOutPorts.clear();
  myRow2DataTypeOut.clear();

  QStringList aPortNames;
  QStringList aPortTypes;
  QStringList aValueTypes;
  QStringList aValues;
  QStringList anIsInStudy;
  
  YACSGui_Table* aTable = myOutputPortsGroupBox->Table();
  QStringList aPTCB = aTable->Params( 0, 1 );

  bool isStudyNode=false;
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
    
    // Port type
    if ( OutputStudyPort* anOutputP = dynamic_cast<OutputStudyPort*>(anOutPort) )
      {
        //Output study port
        aPortTypes.append( aPTCB[0] );
        isStudyNode=true;
      }
    else if ( OutputPort* anOutputP = dynamic_cast<OutputPort*>(anOutPort) )
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

    aRowId++;
  }

  // Fill "Port name" column
  aTable->setStrings( 0, aPortNames, true );

  // Fill "Port type" column
  aTable->setStrings( 1, aPortTypes, true );

  // Fill "Value type" column
  aTable->setStrings( 2, aValueTypes, true );

  //Do not set validators for study as all values are strings whatever is the type
  for (int irow=0; irow < aTable->numRows(); irow++)
    if(isStudyNode == false)
      setValueCellValidator( aTable, irow);
    else
      aTable->setCellType( irow, 3, YACSGui_Table::String );

  // Fill "Value" column
  aTable->setStrings( 3, aValues, true );

  // Fill "Is in study" column
  aTable->setStrings( 4, anIsInStudy, true );

  // Set "Value" column read write
  DEBTRACE("ReadOnly");
  for ( int i = 0, n = aTable->numRows(); i < n; i++ )
  {
    aTable->setDefValue( i, 2, aValueTypes[i] );
    aTable->setReadOnly( i, 3, false );
  }
 
  if ( !aPortNames.empty() ) myOutputPortsGroupBox->EnableBtn( YACSGui_PlusMinusGrp::AllBtn );
  DEBTRACE("End of fillOutputPorts");
}

QString YACSGui_DataNodePage::getPortType( YACS::ENGINE::Port* thePort ) const
{
  DEBTRACE("YACSGui_DataNodePage::getPortType");
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
    if ( aDType > NONE && aDType < Objref )
      aType = aVTCB[aDType-1];
    else
      aType = QString(aDataPort->edGetType()->name());
  }
  
  return aType;
}

bool YACSGui_DataNodePage::isStored( YACS::ENGINE::Port* thePort )
{
  DEBTRACE("YACSGui_DataNodePage::isStored: " << thePort);
  if ( InPort* anIP = dynamic_cast<InPort*>(thePort) )
  {
    map<int,InPort*>::iterator it = myRow2StoredInPorts.begin();
    for ( ; it != myRow2StoredInPorts.end(); it++ )
      {
        DEBTRACE("isStored ? " << thePort << " " << (*it).second);
        if ( (*it).second == thePort ) return true;
      }
    return false;
  }
  else if ( OutPort* anOP = dynamic_cast<OutPort*>(thePort) )
  {
    map<int,OutPort*>::iterator it = myRow2StoredOutPorts.begin();
    for ( ; it != myRow2StoredOutPorts.end(); it++ )
      {
        DEBTRACE("isStored ? " << thePort << " " << (*it).second);
        if ( (*it).second == thePort ) return true;
      }
    return false;
  }
  return true; // the gate ports are kept (not re-create)
}

void YACSGui_DataNodePage::resetStoredPortsMaps()
{
  DEBTRACE("YACSGui_DataNodePage::resetStoredPortsMaps");
  myRow2StoredInPorts.clear();
  myRow2DataTypeIn.clear();

  QStringList aIPortNames;
  myInputPortsGroupBox->Table()->strings( 0, aIPortNames );

  int aRowId = 0;
  for ( QStringList::Iterator it = aIPortNames.begin(); it != aIPortNames.end(); ++it )
  {
    DEBTRACE((*it).latin1());
    if ( (*it).isEmpty() ) continue;

    try {
      if ( InputPort* aIDP = getNode()->getInputPort((*it).latin1()) )
        {
          DEBTRACE("store port: " << aRowId << " " << aIDP);
	  myRow2StoredInPorts.insert( std::make_pair(aRowId,aIDP) );
          myRow2DataTypeIn[aRowId]=aIDP->edGetType();
        }
    }
    catch (YACS::Exception& ex) {
      try {
        if ( InputDataStreamPort* aIDSP = getNode()->getInputDataStreamPort((*it).latin1()) )
          {
            DEBTRACE("store port: " << aRowId << " " << aIDSP);
            myRow2StoredInPorts.insert( std::make_pair(aRowId,aIDSP) );
            myRow2DataTypeIn[aRowId]=aIDSP->edGetType();
          }
      }
      catch (YACS::Exception& ex) {
        std::cerr << "INTERNAL ERROR. Unknown port " << (*it).latin1() << std::endl;
      }
    }
    aRowId++;
  }

  myRow2StoredOutPorts.clear();
  myRow2DataTypeOut.clear();

  QStringList aOPortNames;
  myOutputPortsGroupBox->Table()->strings( 0, aOPortNames );

  aRowId = 0;
  for ( QStringList::Iterator it = aOPortNames.begin(); it != aOPortNames.end(); ++it )
  {
    DEBTRACE((*it).latin1());
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
      try {
        if ( OutputDataStreamPort* aODSP = getNode()->getOutputDataStreamPort((*it).latin1()) )
          {
            DEBTRACE("store port: " << aRowId << " " << aODSP);
            myRow2StoredOutPorts.insert( std::make_pair(aRowId,aODSP) );
            myRow2DataTypeOut[aRowId]=aODSP->edGetType();
          }
      }
      catch (YACS::Exception& ex) {
        std::cerr << "INTERNAL ERROR. Unknown port " << (*it).latin1() << std::endl;
      }
    }
    aRowId++;
  }
}

void YACSGui_DataNodePage::resetDataTypeMaps()
{
  DEBTRACE("YACSGui_DataNodePage::resetDataTypeMaps");
  //myRow2DataTypeIn.clear();
  //myRow2DataTypeOut.clear();
}

void YACSGui_DataNodePage::resetPLists()
{
  DEBTRACE("YACSGui_DataNodePage::resetPLists");
  resetIPLists();
  resetOPLists();
}

void YACSGui_DataNodePage::resetIPLists()
{
  DEBTRACE("YACSGui_DataNodePage::resetIPLists");
  myIPList.clear();

  QStringList aIPortNames;
  myInputPortsGroupBox->Table()->strings( 0, aIPortNames );

  for ( QStringList::Iterator it = aIPortNames.begin(); it != aIPortNames.end(); ++it )
  {
    DEBTRACE((*it).latin1());
    if ( (*it).isEmpty() ) continue;

    try {
      if ( InPort* aIDP = getNode()->getInputPort((*it).latin1()) )
	myIPList.append(aIDP);
    }
    catch (YACS::Exception& ex) {
      try {
        if ( InPort* aIDSP = getNode()->getInputDataStreamPort((*it).latin1()) )
          myIPList.append(aIDSP);
      }
      catch (YACS::Exception& ex) {
        std::cerr << "INTERNAL ERROR. Unknown port " << (*it).latin1() << std::endl;
      }
    }
  }
}

void YACSGui_DataNodePage::resetOPLists()
{
  DEBTRACE("YACSGui_DataNodePage::resetOPLists");
  myOPList.clear();

  QStringList aOPortNames;
  myOutputPortsGroupBox->Table()->strings( 0, aOPortNames );

  for ( QStringList::Iterator it = aOPortNames.begin(); it != aOPortNames.end(); ++it )
  {
    DEBTRACE((*it).latin1());
    if ( (*it).isEmpty() ) continue;

    try {
      if ( OutPort* aODP = getNode()->getOutputPort((*it).latin1()) )
	myOPList.append(aODP);
    }
    catch (YACS::Exception& ex) {
      try {
        if ( OutPort* aODSP = getNode()->getOutputDataStreamPort((*it).latin1()) )
          myOPList.append(aODSP);
      }
      catch (YACS::Exception& ex) {
        std::cerr << "INTERNAL ERROR. Unknown port " << (*it).latin1() << std::endl;
      }
    }
  }
}

void YACSGui_DataNodePage::orderPorts( bool withFilling )
{
  DEBTRACE("YACSGui_DataNodePage::orderPorts: " << withFilling);
  ElementaryNode* aNode = dynamic_cast<ElementaryNode*>( getNode() );
  if ( !aNode ) return;

  QStringList aPortNames;
  int aRowId;

  // order input ports according to the list of ports names in the table
  myInputPortsGroupBox->Table()->strings( 0, aPortNames );

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
  myOutputPortsGroupBox->Table()->strings( 0, aPortNames );

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

void YACSGui_DataNodePage::setProperties()
{
  DEBTRACE("YACSGui_DataNodePage::setProperties");
  ElementaryNode* aNode = dynamic_cast<ElementaryNode*>( getNode() );
  if ( !aNode ) return;
  DataNode* aDataNode = dynamic_cast<DataNode*>( getNode() );
  if ( !aDataNode ) return;
  if (_isStudy)
    {
      string aStudyName(_studyName.latin1());
      if (! _studyName.isEmpty()) aDataNode->setRef(aStudyName);
      if (_studyId) 
        {
          std::stringstream theStudyId;
          theStudyId << _studyId;
          aDataNode->setProperty("StudyID", theStudyId.str());
        }
    }
  else
    {
      string aFileName(_fileName.latin1());
      if (! _fileName.isEmpty()) aDataNode->setRef(aFileName);
    }
}

void YACSGui_DataNodePage::setInputPorts()
{
  DEBTRACE("YACSGui_DataNodePage::setInputPorts");
  if ( !myInputPortsGroupBox ) return;

  ElementaryNode* aNode = dynamic_cast<ElementaryNode*>( getNode() );
  if ( !aNode ) return;
  DataNode* aDataNode = dynamic_cast<DataNode*>( getNode() );
  if ( !aDataNode ) return;

  myInputPortsGroupBox->Table()->stopEdit(true);

  // remove old ports
  DEBTRACE("Remove old ports");
  list<InPort*> anInPortsEngine = aNode->getSetOfInPort();
  list<InPort*>::iterator anInPortsIter = anInPortsEngine.begin();
  for( ;anInPortsIter!=anInPortsEngine.end();anInPortsIter++)
    if ( !isStored(*anInPortsIter) )
    {
      DEBTRACE("Port not stored: " << *anInPortsIter);
      mySNode->update( REMOVE,
		       ( dynamic_cast<InputPort*>(*anInPortsIter) ? INPUTPORT : INPUTDATASTREAMPORT ),
		       GuiContext::getCurrent()->_mapOfSubjectDataPort[*anInPortsIter] );
      myIPList.take(myIPList.find(*anInPortsIter));

      //aNode->edRemovePort(*anInPortsIter);
      // remove port subject (and all subject links connected to this port)
      mySNode->destroy( GuiContext::getCurrent()->_mapOfSubjectDataPort[*anInPortsIter] );
    }
  
  // read input data and input data stream ports from the table
  YACSGui_Table* aTable = myInputPortsGroupBox->Table();
  
  QStringList aPortNames;
  QStringList aValues;

  aTable->strings( 0, aPortNames );
  aTable->strings( 3, aValues );

  YACS::ENGINE::Catalog* aCatalog = YACS::ENGINE::getSALOMERuntime()->getBuiltinCatalog();

  int aRowId = 0;
  for ( QStringList::Iterator it = aPortNames.begin(); it != aPortNames.end(); ++it,++aRowId )
  {
    if ( (*it).isEmpty() ) continue;

    if ( aPortNames.contains(*it) > 1 )
    {
      std::string what("The node ");
      what += mySNode->getName();
      what += " contains two or more "; 
      what+=(*it).latin1();
      what+=" input ports";
      INFOS("=========="<<what);
      SUIT_MessageBox::warn1(getInputPanel()->getModule()->getApp()->desktop(),
                             tr("WRN_WARNING"),
                             what,
                             tr("BUT_OK"));
      continue;
    }
    
    // retrieve a type name of the input port as a string
    std::string aType = "";

    if (QTableItem* anItem = aTable->item( aRowId, 2 ) )
    {
      if ( QComboBox* aCB = dynamic_cast<QComboBox*>(aTable->cellWidget( aRowId, 2 )) )
      {
	if ( aCB->lineEdit() )
	  aType = aCB->lineEdit()->text().latin1();
      }
      else
	aType = anItem->text().latin1();
    }
    
    TypeCode* aTC = 0;
    bool toSwitch = true;
    if ( !aType.empty() )
    {
      QStringList aL = aTable->Params( aRowId, 2 );
      if ( aL.findIndex( QString(aType) ) == -1 ) toSwitch = false;
    }

    if ( toSwitch )
    {
      switch ( DynType(aTable->intValueCombo( 2, aRowId ) + 1) )
      {
      case Double:
	aType = "double";
	break;
      case Int:
	aType = "int";
	break;
      case String:
	aType = "string";
	break;
      case Bool:
	aType = "bool";
	break;
      case Objref:
	//aType = "objref";
	break;
      case Sequence:
	//aType = "sequence";
	break;
      case Array:
	//aType = "array";
	break;
      case Struct:
	//aType = "struct";
	break;
      default:
	break;
      }
    }
    else
    {
      if ( myRow2DataTypeIn.find(aRowId) != myRow2DataTypeIn.end() )
	aTC = myRow2DataTypeIn[aRowId]->clone();
      if ( !aTC )
        {
          std::cerr << "Problem with data types initialization" << std::endl;
          continue;
        }
    }

    GuiEvent anEvent = EDIT;
    if ( aTable->intValueCombo( 1, aRowId ) == 0 ) // Data Flow port
    {
      InputPort* aIDP = 0;
      if ( myRow2StoredInPorts.find(aRowId) != myRow2StoredInPorts.end() )
      { // ---- update port ----
	if ( aIDP = dynamic_cast<InputPort*>(myRow2StoredInPorts[aRowId]) )
	{
	  aIDP->setName(*it);
	  aTC = aIDP->edGetType();
	  // EDIT event will be emitted => only update port prs
	}
      }
      else
      { // ---- create port ----
	if ( !toSwitch )
	{
	  aIDP = aNode->edAddInputPort( *it, aTC );
	  // here we need also to create a port subject
	  // this is a temporary solution solution, bcause of SessionCataLoader use loadTypesOld(...)
	  // method instead of a new loadTypes(...) method => we can not retrieve all value types
	  // from the _typeMap of the Catalog ("objref","sequence","array","struct" are not available from _typeMap now)
	  // => we need to change SubjectNode::addSubjectInputPort(..) to public method
	  mySNode->addSubjectInputPort( aIDP, *it );
	}
	else
	{
	  aIDP = dynamic_cast<InputPort*>( dynamic_cast<SubjectElementaryNode*>(mySNode)->addInputPort( aCatalog, aType, *it )->getPort() );
	  aTC = aIDP->edGetType();
	}

	// ADD event will be emitted from hmi after subject creation => re-create port prs
	anEvent = ADD;
	
	myIPList.append(aIDP);
      }

      if ( aIDP )
      {
	// initialize new input data port
        if(aValues[aRowId].isNull() || aValues[aRowId] == "< ? >")
          continue;

        string val;
	switch ( aTC->kind() )
	{
        case Double:
          {
            if(_isStudy)
              val=aValues[aRowId].latin1();
            else
              {
                stringstream sval;
                double vald = aValues[aRowId].toDouble();
                sval << vald;
                val = sval.str();
              }
            aDataNode->setData(aIDP, val );
            anEvent = EDIT;
            DEBTRACE(val);
            break;
          }
        case Int:
          {
            if(_isStudy)
              val=aValues[aRowId].latin1();
            else
              {
                stringstream sval;
                int vali = aValues[aRowId].toInt();
                sval << vali;
                val = sval.str();
              }
            aDataNode->setData(aIDP, val );
            anEvent = EDIT;
            DEBTRACE(val);
            break;
          }
	case String:
          {
            val = "";
            if (! aValues[aRowId].isNull()) 
              val = aValues[aRowId].latin1();
            aDataNode->setData(aIDP, val );
            anEvent = EDIT;
            DEBTRACE(val);
            break;
          }
        case Bool:
          {
            if(_isStudy)
              val=aValues[aRowId].latin1();
            else
              val = (aValues[aRowId].compare( aTable->Params(aRowId,3)[0] ) ? "0" : "1" );
            aDataNode->setData(aIDP, val );
            anEvent = EDIT;
            DEBTRACE(val);
            break;
          }
          //case Objref:
	  //aIDP->edInit( "" ); // TODO : create an Any* with corresponding type and initialize with it
	  //anEvent = EDIT;
	  //break;
          //case Sequence:
	  //aIDP->edInit( "" ); // TODO : create an Any* (i.e. SequenceAny*) with corresponding type and initialize with it
	  //anEvent = EDIT;
	  //break;
          //case Array:
	  //aIDP->edInit( "" ); // TODO : create an Any* (i.e. ArrayAny*) with corresponding type and initialize with it
	  //anEvent = EDIT;
	  //break;
          //case Struct:
	  //aIDP->edInit( "" ); // TODO : create an Any* with corresponding type and initialize with it
	  //anEvent = EDIT;
	  //break;
	default:
          {
            //DEBTRACE("Problem, default type -------------------------------------------------");
            if(_isStudy)
              {
                string val = aValues[aRowId].latin1();
                aDataNode->setData(aIDP, val );
                anEvent = EDIT;
                DEBTRACE(val);
              }
            // When Apply clicked directly after edition, abort on following line
//             string val = aValues[aRowId].latin1();
//             aDataNode->setData(aIDP, val );
//             anEvent = EDIT;
            break;
          }
	}
      
	if ( anEvent == EDIT )
	  mySNode->update( EDIT, INPUTPORT, GuiContext::getCurrent()->_mapOfSubjectDataPort[aIDP]);
	//else if ( anEvent == ADD )
	//  mySNode->update( ADD, INPUTPORT, GuiContext::getCurrent()->_mapOfSubjectDataPort[aIDP]);
      }
    }

    //else if ( aTable->intValueCombo( 1, aRowId ) == 1 )    // Data Stream (BASIC) port
    //  aNode->edAddInputDataStreamPort( *it, aTC );
    //  // TODO : initialize new created input data stream port

    else if ( aTable->intValueCombo( 1, aRowId ) == 2 )      // Data Stream (CALCIUM) port
    {
      InputDataStreamPort* aIDSP = 0;
      if ( myRow2StoredInPorts.find(aRowId) != myRow2StoredInPorts.end() )
      { // ---- update port ----
        DEBTRACE("---- update port ---- " << *it);
	if ( aIDSP = dynamic_cast<InputDataStreamPort*>(myRow2StoredInPorts[aRowId]) )
	{
	  aIDSP->setName(*it);
	  aTC = aIDSP->edGetType();
	  // EDIT event will be emitted => only update port prs
	}
      }
      else
      { // ---- create port ----
        DEBTRACE("---- create port ---- " << *it);
	if ( aType.empty() )
	{
	  aIDSP = aNode->edAddInputDataStreamPort( *it, aTC );
	  // here we need also to create a port subject
	  // this is a temporary solution solution, bcause of SessionCataLoader use loadTypesOld(...)
	  // method instead of a new loadTypes(...) method => we can not retrieve all value types
	  // from the _typeMap of the Catalog ("objref","sequence","array","struct" are not available from _typeMap now)
	  // => we need to change SubjectNode::addSubjectIDSPort(..) to public method
	  mySNode->addSubjectIDSPort( aIDSP, *it );
	}
	else
	{
	  aIDSP = dynamic_cast<InputDataStreamPort*>( dynamic_cast<SubjectElementaryNode*>(mySNode)->addIDSPort( aCatalog, aType, *it )->getPort() );
	  aTC = aIDSP->edGetType();
	}

	// ADD event will be emitted => re-create port prs
	anEvent = ADD;

	myIPList.append(aIDSP);
      }

      // TODO : initialize new created input data stream port

      if ( aIDSP && anEvent == EDIT )
	mySNode->update( EDIT, INPUTDATASTREAMPORT, GuiContext::getCurrent()->_mapOfSubjectDataPort[aIDSP]);
      //else if ( anEvent == ADD )
      //  mySNode->update( ADD, INPUTDATASTREAMPORT, GuiContext::getCurrent()->_mapOfSubjectDataPort[aIDSP]);
    }

    //else if ( aTable->intValueCombo( 1, aRowId ) == 3 )    // Data Stream (PALM) port
    //  aNode->edAddInputDataStreamPort( *it, aTC );
    //  // TODO : initialize new created input data stream port
    
  }

  //resetIPLists();
}

void YACSGui_DataNodePage::setOutputPorts()
{
  DEBTRACE("YACSGui_DataNodePage::setOutputPorts");
  if ( !myOutputPortsGroupBox ) return;

  ElementaryNode* aNode = dynamic_cast<ElementaryNode*>( getNode() );
  if ( !aNode ) return;
  DataNode* aDataNode = dynamic_cast<DataNode*>( getNode() );
  if ( !aDataNode ) return;

  myOutputPortsGroupBox->Table()->stopEdit(true);

  // remove old ports
  list<OutPort*> anOutPortsEngine = aNode->getSetOfOutPort();
  list<OutPort*>::iterator anOutPortsIter = anOutPortsEngine.begin();
  for( ;anOutPortsIter!=anOutPortsEngine.end();anOutPortsIter++)
    if ( !isStored(*anOutPortsIter) )
    {
      DEBTRACE("Port not stored: " << *anOutPortsIter);
      mySNode->update( REMOVE,
		       ( dynamic_cast<OutputPort*>(*anOutPortsIter) ? OUTPUTPORT : OUTPUTDATASTREAMPORT ),
		       GuiContext::getCurrent()->_mapOfSubjectDataPort[*anOutPortsIter] );
      myOPList.take(myOPList.find(*anOutPortsIter));

      //aNode->edRemovePort(*anOutPortsIter);
      // remove port subject (and all subject links connected to this port)
      mySNode->destroy( GuiContext::getCurrent()->_mapOfSubjectDataPort[*anOutPortsIter] );
    }
  
  DEBTRACE("YACSGui_DataNodePage::setOutputPorts");
  // read output data and output data stream ports from the table
  YACSGui_Table* aTable = myOutputPortsGroupBox->Table();
  
  QStringList aPortNames;
  QStringList aValues;

  aTable->strings( 0, aPortNames );
  aTable->strings( 3, aValues );

  YACS::ENGINE::Catalog* aCatalog = YACS::ENGINE::getSALOMERuntime()->getBuiltinCatalog();
  DEBTRACE("YACSGui_DataNodePage::setOutputPorts");

  int aRowId = 0;
  for ( QStringList::Iterator it = aPortNames.begin(); it != aPortNames.end(); ++it,++aRowId )
  {
    DEBTRACE(aRowId << " " << *it);
    if ( (*it).isEmpty() ) continue;

    if ( aPortNames.contains(*it) > 1 )
    {
      std::string what("The node ");
      what += mySNode->getName();
      what += " contains two or more "; 
      what+=(*it).latin1();
      what+=" output ports";
      INFOS("=========="<<what);
      DEBTRACE("=========="<<what);
      SUIT_MessageBox::warn1(getInputPanel()->getModule()->getApp()->desktop(),
                             tr("WRN_WARNING"),
                             what,
                             tr("BUT_OK"));
      continue;
    }
    
    // retrieve a type name of the input port as a string
    std::string aType = "";

    if (QTableItem* anItem = aTable->item( aRowId, 2 ) )
    {
      if ( QComboBox* aCB = dynamic_cast<QComboBox*>(aTable->cellWidget( aRowId, 2 )) )
      {
	if ( aCB->lineEdit() )
	  aType = aCB->lineEdit()->text().latin1();
      }
      else
	aType = anItem->text().latin1();
    }
    DEBTRACE(aType);
    
    TypeCode* aTC = 0;
    bool toSwitch = true;
    if ( !aType.empty() )
    {
      QStringList aL = aTable->Params( aRowId, 2 );
      if ( aL.findIndex( QString(aType) ) == -1 ) toSwitch = false;
    }
    DEBTRACE(toSwitch);
    if ( toSwitch )
    {
      DEBTRACE("switch branch " << DynType(aTable->intValueCombo( 2, aRowId ) + 1));
      switch ( DynType(aTable->intValueCombo( 2, aRowId ) + 1) )
      {
      case Double:
	aType = "double";
	break;
      case Int:
	aType = "int";
	break;
      case String:
	aType = "string";
	break;
      case Bool:
	aType = "bool";
	break;
      case Objref:
	//aType = "objref";
	break;
      case Sequence:
	//aType = "sequence";
	break;
      case Array:
	//aType = "array";
	break;
      case Struct:
	//aType = "struct";
	break;
      default:
	break;
      }
      DEBTRACE(aType);
    }
    else
    {
      DEBTRACE("no switch branch " << myRow2DataTypeOut.size());
      if ( myRow2DataTypeOut.find(aRowId) != myRow2DataTypeOut.end() )
        {
          DEBTRACE(myRow2DataTypeOut[aRowId]);
          aTC = myRow2DataTypeOut[aRowId]->clone();
        }
      if ( !aTC )
        {
          std::cerr << "Problem with initialization of data types" << std::endl;
          continue;
        }
      DEBTRACE(aTC);
    }
    
  DEBTRACE("YACSGui_DataNodePage::setOutputPorts " << aTable->intValueCombo( 1, aRowId ));

    GuiEvent anEvent = EDIT;
    if ( aTable->intValueCombo( 1, aRowId ) == 0 )           // Data Flow port
    {
      DEBTRACE("Data Flow");
      OutputPort* aODP = 0;
      if ( myRow2StoredOutPorts.find(aRowId) != myRow2StoredOutPorts.end() )
      { // ---- update port ----
        DEBTRACE("---- update port ---- " << *it);
	if ( aODP = dynamic_cast<OutputPort*>(myRow2StoredOutPorts[aRowId]) )
	{
	  aODP->setName(*it);
	  aTC = aODP->edGetType();
	  // EDIT event will be emitted => only update port prs
	}
      }
      else
      { // ---- create port ----
        DEBTRACE("---- create port ---- " << *it);
	if ( !toSwitch )
	{
	  aODP = aNode->edAddOutputPort( *it, aTC );
	  // here we need also to create a port subject
	  // this is a temporary solution solution, bcause of SessionCataLoader use loadTypesOld(...)
	  // method instead of a new loadTypes(...) method => we can not retrieve all value types
	  // from the _typeMap of the Catalog ("objref","sequence","array","struct" are not available from _typeMap now)
	  // => we need to change SubjectNode::addSubjectOutputPort(..) to public method
	  mySNode->addSubjectOutputPort( aODP, *it );
	}
	else
	{
          DEBTRACE("from catalog");
	  aODP = dynamic_cast<OutputPort*>( dynamic_cast<SubjectElementaryNode*>(mySNode)->addOutputPort( aCatalog, aType, *it )->getPort() );
	  aTC = aODP->edGetType();
	}

	// ADD event will be emitted from hmi after subject creation => re-create port prs
	anEvent = ADD;
	
	myOPList.append(aODP);
      }

      if ( aODP )
        {
          DEBTRACE("initialize ODP")

	// initialize new created input data port
        string val;
	switch ( aTC->kind() )
	{
	case Double:
          {
            if (aValues[aRowId].isNull()) 
              break;
            if(_isStudy)
              val=aValues[aRowId].latin1();
            else
              {
                stringstream sval;
                double vald = aValues[aRowId].toDouble();
                sval << vald;
                val = sval.str();
              }
            aDataNode->setData(aODP, val );
            anEvent = EDIT;
            DEBTRACE(val);
            break;
          }
	case Int:
          {
            if (aValues[aRowId].isNull()) 
              break;
            if(_isStudy)
              val=aValues[aRowId].latin1();
            else
              {
                stringstream sval;
                int vali = aValues[aRowId].toInt();
                sval << vali;
                val = sval.str();
              }
            aDataNode->setData(aODP, val );
            anEvent = EDIT;
            DEBTRACE(val);
            break;
          }
	case String:
          {
            if (aValues[aRowId].isNull()) 
              break;
            val = "";
            if (! aValues[aRowId].isNull()) 
              val = aValues[aRowId].latin1();
            aDataNode->setData(aODP, val );
            anEvent = EDIT;
            DEBTRACE(val);
            break;
          }
	case Bool:
          {
            if (aValues[aRowId].isNull()) 
              break;
            if(_isStudy)
              val=aValues[aRowId].latin1();
            else
              val = (aValues[aRowId].compare( aTable->Params(aRowId,3)[0] ) ? "0" : "1" );
            aDataNode->setData(aODP, val );
            anEvent = EDIT;
            DEBTRACE(val);
            break;
          }
          //case Objref:
	  //aODP->edInit( "" ); // TODO : create an Any* with corresponding type and initialize with it
	  //anEvent = EDIT;
	  //break;
          //case Sequence:
	  //aODP->edInit( "" ); // TODO : create an Any* (i.e. SequenceAny*) with corresponding type and initialize with it
	  //anEvent = EDIT;
	  //break;
          //case Array:
	  //aODP->edInit( "" ); // TODO : create an Any* (i.e. ArrayAny*) with corresponding type and initialize with it
	  //anEvent = EDIT;
	  //break;
          //case Struct:
	  //aODP->edInit( "" ); // TODO : create an Any* with corresponding type and initialize with it
	  //anEvent = EDIT;
	  //break;
	default:
          {
            //DEBTRACE("Problem, default type -------------------------------------------------" );
            if (aValues[aRowId].isNull()) 
              break;
            if(_isStudy)
              {
                val = aValues[aRowId].latin1();
                aDataNode->setData(aODP, val );
                anEvent = EDIT;
                DEBTRACE(val);
              }
//             string val = aValues[aRowId].latin1();
//             aDataNode->setData(aODP, val );
//             anEvent = EDIT;
//             break;
          }
	}
      
        if ( anEvent == EDIT )
          mySNode->update( EDIT, OUTPUTPORT, GuiContext::getCurrent()->_mapOfSubjectDataPort[aODP]);
        }
    }

    //else if ( aTable->intValueCombo( 1, aRowId ) == 1 )    // Data Stream (BASIC) port
    //  aNode->edAddOutputDataStreamPort( *it, aTC );

    else if ( aTable->intValueCombo( 1, aRowId ) == 2 )      // Data Stream (CALCIUM) port
    {
      DEBTRACE("datastream ports");
      OutputDataStreamPort* aODSP = 0;
      if ( myRow2StoredOutPorts.find(aRowId) != myRow2StoredOutPorts.end() )
      { // ---- update port ----
        DEBTRACE("---- update port ---- " << *it);
	if ( aODSP = dynamic_cast<OutputDataStreamPort*>(myRow2StoredOutPorts[aRowId]) )
	{
	  aODSP->setName(*it);
	  aTC = aODSP->edGetType();
	  // EDIT event will be emitted => only update port prs
	}
      }
      else
      { // ---- create port ----
        DEBTRACE("---- create port ---- " << *it);
	if ( aType.empty() )
	{
	  aODSP = aNode->edAddOutputDataStreamPort( *it, aTC );
	  // here we need also to create a port subject
	  // this is a temporary solution solution, bcause of SessionCataLoader use loadTypesOld(...)
	  // method instead of a new loadTypes(...) method => we can not retrieve all value types
	  // from the _typeMap of the Catalog ("objref","sequence","array","struct" are not available from _typeMap now)
	  // => we need to change SubjectNode::addSubjectODSPort(..) to public method
	  mySNode->addSubjectODSPort( aODSP, *it );
	}
	else
	{
	  aODSP = dynamic_cast<OutputDataStreamPort*>( dynamic_cast<SubjectElementaryNode*>(mySNode)->addODSPort( aCatalog, aType, *it )->getPort() );
	  aTC = aODSP->edGetType();
	}

	// ADD event will be emitted => re-create port prs
	anEvent = ADD;
	
	myOPList.append(aODSP);
      }

      if ( aODSP && anEvent == EDIT )
	mySNode->update( EDIT, OUTPUTDATASTREAMPORT, GuiContext::getCurrent()->_mapOfSubjectDataPort[aODSP]);
      
    }

    //else if ( aTable->intValueCombo( 1, aRowId ) == 3 )    // Data Stream (PALM) port
    //  aNode->edAddOutputDataStreamPort( *it, aTC );
    
    // TODO : Remember "Is in study" flag for output pirt
    // ...

  }

  //resetOPLists();
}

YACS::ENGINE::TypeCode* YACSGui_DataNodePage::createTypeCode( YACS::ENGINE::DynType theType,
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

bool YACSGui_DataNodePage::isPortsModified( bool theInput )
{
  DEBTRACE("YACSGui_DataNodePage::isPortsModified: " << theInput);
  bool isModified = false;
  
  if ( !getNode() ) return isModified;
  // read data and data stream ports from the table
  YACSGui_Table* aTable = 0;
  if ( theInput ) aTable = myInputPortsGroupBox->Table();
  else aTable = myOutputPortsGroupBox->Table();
  if ( !aTable ) return isModified;
  aTable->stopEdit(true);
    
  QStringList aPortNames;
  aTable->strings( 0, aPortNames );

  if ( ( theInput ? aPortNames.count() != getNode()->getNumberOfInputPorts() :
                    aPortNames.count() != getNode()->getNumberOfOutputPorts() ) ) isModified = true;
  else
  {
    DEBTRACE("YACSGui_DataNodePage::isPortsModified: " << isModified);
    QStringList aPortTypes;
    QStringList aValueTypes;
    QStringList aValues;
    
    aTable->strings( 1, aPortTypes );
    aTable->strings( 2, aValueTypes );
    aTable->strings( 3, aValues );
      
    QStringList aPTCB = aTable->Params( 0, 1 );
    
    int aRowId = 0;
    for ( QStringList::Iterator it = aPortNames.begin(); it != aPortNames.end(); ++it )
    {
      DEBTRACE(*it);
      Port* aPort = 0;
      try {
	aPort = ( theInput ? (Port*)(getNode()->getInPort(*it)) : (Port*)(getNode()->getOutPort(*it)) );
      }
      catch (...) {}
      
      if ( aPort )
      {
        // check value type and value of the port
        DEBTRACE("YACSGui_DataNodePage::isPortsModified: " << getPortValue( aPort ) << " " << getPortType( aPort ) );
        QString value=aValues[aRowId];
        QString valtype=aValueTypes[aRowId];
        if(value.isNull())
          value="";
        if(valtype.isNull())
          valtype="";
        DEBTRACE(value<<","<<valtype);

        if ( getPortValue( aPort ).compare(value) != 0
             ||
             getPortType( aPort ).compare(valtype) != 0 )
        {
	  isModified = true;
          DEBTRACE("YACSGui_DataNodePage::isPortsModified: " << isModified);
	  break;
	}
	
	// check port type
	QString aPortType = aPTCB[0];
	if ( dynamic_cast<DataStreamPort*>(aPort) )
	  aPortType = aPTCB[2];
	
	if ( aPortType.compare(aPortTypes[aRowId]) != 0 )
	{
	  isModified = true;
          DEBTRACE("YACSGui_DataNodePage::isPortsModified: " << isModified);
	  break;
	}

	//if ( !theInput ) 
	//  TODO: compare "Is in study" fields (not yet in use)
      }
      else // this case means that we renamed any port or added a new one (i.e. check port name)
      {
	isModified = true;
        DEBTRACE("YACSGui_DataNodePage::isPortsModified: " << isModified);
	break;
      }
      aRowId++;
    }
  }
  return isModified;
}

