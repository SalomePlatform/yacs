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
  Class : YACSGui_DataTypePage
  Description : Page for data type import
*/

YACSGui_DataTypePage::YACSGui_DataTypePage( QWidget* theParent, const char* theName, WFlags theFlags )
  : DataTypePage( theParent, theName, theFlags ),
    GuiObserver(),
    mySDataType( 0 )
{
  myProcName = "";
  myProcRadioButton->setEnabled(false);

  connect( myBrowseButton, SIGNAL(clicked(  )), this, SLOT(onBrowse( )) );
  connect( this, SIGNAL( enableApply( bool ) ), getInputPanel(), SLOT( onApplyEnabled( bool ) ) );
  connect( CatalogTree, SIGNAL( clicked( QListViewItem* ) ), this, SLOT( onCatalogDataTypeClicked( QListViewItem* ) ) );
  connect( mySessionRadioButton, SIGNAL( stateChanged( int ) ), this, SLOT( onCatalogChanged( int ) ) );
  connect( myProcRadioButton, SIGNAL( stateChanged( int ) ), this, SLOT( onCatalogChanged( int ) ) );
}

YACSGui_DataTypePage::~YACSGui_DataTypePage()
{
  if (getInputPanel()) getInputPanel()->removePage(this);

  if ( mySDataType ) mySDataType->detach(this);
}

void YACSGui_DataTypePage::select( bool isSelected )
{
  DEBTRACE(">> YACSGui_DataTypePage::select");
}

void YACSGui_DataTypePage::update( YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son)
{
  DEBTRACE(">> YACSGui_DataTypePage::update");
}

void YACSGui_DataTypePage::setSDataType( YACS::HMI::SubjectDataType* theSDataType )
{ 
  if ( theSDataType && mySDataType != theSDataType )
  {
    if ( mySDataType ) mySDataType->detach(this); //detach from old schema
    mySDataType = theSDataType;
    mySDataType->attach(this); // attach to new schema
  }
  updateState();
}

YACS::ENGINE::TypeCode* YACSGui_DataTypePage::getTypeCode() const
{
  return ( mySDataType ? dynamic_cast<TypeCode*>(mySDataType->getTypeCode()) : 0 );
}

QString YACSGui_DataTypePage::getDataTypeName() const
{
  return ( mySDataType ? QString( mySDataType->getName() ) : QString("") );
}

YACSGui_InputPanel* YACSGui_DataTypePage::getInputPanel() const
{
  SalomeApp_Application* anApp = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
  if( !anApp )
    return 0;

  YACSGui_Module* aModule = dynamic_cast<YACSGui_Module*>( anApp->activeModule() );
  if( !aModule )
    return 0;

  return aModule->getInputPanel();
}

void YACSGui_DataTypePage::setMode( const YACSGui_InputPanel::PageMode theMode )
{
  myMode = theMode;

  if ( myMode == YACSGui_InputPanel::NewMode )
  {
    CatalogGroupBox->show();
    getInputPanel()->onApplyEnabled(false);
    updateState();
  }
  else if ( myMode == YACSGui_InputPanel::EditMode )
  {
    CatalogGroupBox->hide();
    getInputPanel()->onApplyEnabled(true);
  }
}

void YACSGui_DataTypePage::onApply()
{
  if ( myMode == YACSGui_InputPanel::NewMode )
  {
    GuiContext* aContext = GuiContext::getCurrent();
    if ( !aContext ) return;

    YACSGui_Module* aModule = getInputPanel()->getModule();
    if ( !aModule ) return;

    YACSGui_EditionTreeView * aETV = dynamic_cast<YACSGui_EditionTreeView*>(aModule->activeTreeView());
    if ( !aETV ) return;

    SubjectProc* aSchema = aContext->getSubjectProc();

    YACS::ENGINE::Catalog* aCatalog = 0;
    if ( myProcRadioButton->isChecked() )
    {
      aCatalog = YACS::ENGINE::getSALOMERuntime()->loadCatalog( "proc", myProcName.c_str() );
      std::string errors=aCatalog->getErrors();
      if ( errors != "" )
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

    if ( aSchema && aCatalog && anItem )
    {
      string data_type;
      if ( !myProcRadioButton->isChecked() )
      {
        if ( myDataTypeMap.find( anItem ) == myDataTypeMap.end() )
          return;
         
        data_type = myDataTypeMap[anItem];
      }
      else 
      {
        if ( anItem->text( 0 ).isNull() )
          return;

	if ( !myProcDataTypesMap.count( anItem ) )
          return;

	data_type = anItem->text( 0 ).latin1();
        YACS::ENGINE::TypeCode* aTC = myProcDataTypesMap[ anItem ];
        
        // Add data type in catalog map if necessary. In the other case it will not be
        // found in CommandAddDataTypeFromCatalog method
        if ( aCatalog->_typeMap.find(data_type) == aCatalog->_typeMap.end() )
	  aCatalog->_typeMap[ data_type ] = aTC;
      }

      // data type ( engine - subject ) creation
      SubjectDataType* aCreatedDT = aSchema->addDataType( aCatalog, data_type );
      if ( !aCreatedDT )
      {
	SUIT_MessageBox::warn1(aModule->getApp()->desktop(), 
			       tr("WARNING"), 
			       GuiContext::getCurrent()->_lastErrorMessage,
			       tr("BUT_OK"));
      }
      else aModule->temporaryExport();
    }
    
    //aModule->setGuiMode(YACSGui_Module::EditMode);
    setMode(YACSGui_InputPanel::EditMode);

    if ( aModule->activeTreeView() )
      aModule->activeTreeView()->syncPageTypeWithSelection();

    return;
  }

}

void YACSGui_DataTypePage::onBrowse( )
{
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

void YACSGui_DataTypePage::onCatalogDataTypeClicked( QListViewItem* it )
{
  if ( it )
    if ( !myProcRadioButton->isChecked() )
    {
      // session catalog is used
      std::string aDT = myDataTypeMap[it];
      getInputPanel()->onApplyEnabled(!aDT.empty());
    }
    else 
    {
      // proc catalog is used. Apply button should be enabled if selected item is a data type.
      QListViewItem* aParent = it->parent();
      getInputPanel()->onApplyEnabled( aParent != 0 );
    }
}

void YACSGui_DataTypePage::onCatalogChanged( int )
{
  getInputPanel()->onApplyEnabled(false);
  updateState();
}

void YACSGui_DataTypePage::updateState()
{
  if ( myMode == YACSGui_InputPanel::NewMode )
  {
    CatalogTree->clear();
    CatalogTree->setRootIsDecorated( TRUE );
    QListViewItem* aDTRoot = new QListViewItem( CatalogTree, tr( "DATA_TYPES" ) );

    YACS::ENGINE::Catalog *_currentCatalog;
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

      if ( _currentCatalog )
      {
        myProcDataTypesMap.clear();
	buildTree( _currentCatalog, aDTRoot );
      }
    }
    else
    {
      _currentCatalog = getInputPanel()->getModule()->getCatalog();
      if (_currentCatalog) buildTree( _currentCatalog, aDTRoot );
    }

    return;
  }
}

void YACSGui_DataTypePage::buildTree( YACS::ENGINE::Catalog* theCatalog, QListViewItem* theParent )
{
  if ( !theCatalog ) return;

  map<string, TypeCode*> shortList;

  std::map<std::string,TypeCode*>::iterator anIter;
  for ( anIter = theCatalog->_typeMap.begin(); anIter != theCatalog->_typeMap.end(); ++anIter )
  {
    TypeCode* aTC = anIter->second;
    if ( !aTC ) continue;
    string aName( aTC->name() );
    if ( aName.empty()) continue;
    shortList[aName] = aTC;
  }

  for ( anIter = shortList.begin(); anIter != shortList.end(); ++anIter )
  {
    TypeCode* aTC = anIter->second;

    // Create item corresponding to the data type
    QString aName( aTC->name() );
    QListViewItem* anItem = new QListViewItem( theParent, aName );
    anItem->setPixmap( 0, YACSGui_DataTypeItem::icon() );

    if ( myProcRadioButton->isChecked() ) myProcDataTypesMap[ anItem ] = aTC;
    else myDataTypeMap[ anItem ] = aTC->name();
  }
}
