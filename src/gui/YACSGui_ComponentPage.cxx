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

#define SPACING 5
#define MARGIN 5

using namespace YACS;
using namespace YACS::ENGINE;
using namespace YACS::HMI;
using namespace std;


/*
  Class : YACSGui_ComponentPage
  Description :  Page for component properties
*/

YACSGui_ComponentPage::YACSGui_ComponentPage( QWidget* theParent, const char* theName, WFlags theFlags )
  : ComponentPage( theParent, theName, theFlags ),
    GuiObserver(),
    mySComponent( 0 ),
    myType( SALOME ),
    myEditComp(true)
{
  connect( myContainerName, SIGNAL( activated( const QString& ) ),
	   this, SLOT( onContainerChanged( const QString& ) ) );
}

YACSGui_ComponentPage::~YACSGui_ComponentPage()
{
  if (getInputPanel()) getInputPanel()->removePage(this);

  if ( mySComponent ) mySComponent->detach(this);
}

void YACSGui_ComponentPage::select( bool isSelected )
{
  printf(">> YACSGui_ComponentPage::select\n");
}

void YACSGui_ComponentPage::update( YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son)
{
  printf(">> YACSGui_ComponentPage::update\n");
  switch (event)
  {
  case REMOVE:
    {
      if ( !type && !son && mySComponent )
      {
	mySComponent->detach(this);
	mySComponent = 0;
      }
    }
    break;
  default:
    GuiObserver::update(event, type, son);
  }
}

YACSGui_InputPanel* YACSGui_ComponentPage::getInputPanel() const
{
  SalomeApp_Application* anApp = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
  if( !anApp )
    return 0;

  YACSGui_Module* aModule = dynamic_cast<YACSGui_Module*>( anApp->activeModule() );
  if( !aModule )
    return 0;

  return aModule->getInputPanel();
}

void YACSGui_ComponentPage::setSComponent( YACS::HMI::SubjectComponent* theSComponent, bool isEdit )
{
  if( mySComponent != theSComponent)
  {
    if ( mySComponent ) mySComponent->detach(this); // detach from old component
    mySComponent = theSComponent;
    if ( mySComponent )
    {
      mySComponent->attach(this); // attach to new component
    
      if ( getComponent() && getComponent()->getKind() != CORBAComponent::KIND )
      {
	YACSGui_ContainerPage* aContPage = 
	  dynamic_cast<YACSGui_ContainerPage*>( getInputPanel()->getPage( YACSGui_InputPanel::ContainerId ) );
	if ( aContPage )
	  aContPage->setSContainer( GuiContext::getCurrent()->_mapOfSubjectContainer[getComponent()->getContainer()] );
      }
    }
  }
  myEditComp = isEdit;
  updateState();
}

YACS::ENGINE::ComponentInstance* YACSGui_ComponentPage::getComponent() const
{
  return ( mySComponent ? mySComponent->getComponent() : 0 );
}

QString YACSGui_ComponentPage::getComponentName() const
{
  return ( mySComponent ? QString( mySComponent->getComponent()->getCompoName() ) : QString("") );
}

QString YACSGui_ComponentPage::getInstanceName() const
{
  return ( mySComponent ? QString( mySComponent->getComponent()->getInstanceName() ) : QString("") );
}

void YACSGui_ComponentPage::setComponentType()
{
  if ( !getComponent() || !myComponentType ) return;
  
  if ( getComponent()->getKind() == SalomeComponent::KIND )
  {
    myComponentType->setText("SALOME");
    myType = SALOME;
  }
  else if ( getComponent()->getKind() == SalomePythonComponent::KIND )
  {
    myComponentType->setText("SALOME Python");
    myType = SALOMEPython;
  }
  else if ( getComponent()->getKind() == CppComponent::KIND )
  {
    myComponentType->setText("CPP");
    myType = CPP;
  }
  else if ( getComponent()->getKind() == CORBAComponent::KIND )
  {
    myComponentType->setText("CORBA");
    myType = CORBA;
  }
}

YACSGui_ComponentPage::ComponentType YACSGui_ComponentPage::getComponentType() const
{
  return myType;
}

QString YACSGui_ComponentPage::getContainerName() const
{
  QString aName;

  if ( !getComponent() ) return aName;

  if ( Container* aCont = getComponent()->getContainer() )
    aName = QString(aCont->getName());

  return aName;
}

void YACSGui_ComponentPage::setContainer()
{
  if ( !getComponent() ) return;
  
  QString aText = myContainerName->currentText();
  if ( aText.isEmpty() ) return;

  string aContName( aText.latin1() );

  Proc* aProc = GuiContext::getCurrent()->getProc();
  if ( !aProc ) return;

  if ( aProc->containerMap.count(aContName) )
    if ( Container* aCont = aProc->containerMap[aContName] )
      getComponent()->setContainer(aCont);
}

void YACSGui_ComponentPage::checkModifications( bool& theWarnToShow, bool& theToApply )
{
  if ( !getComponent() || !theWarnToShow && !theToApply ) return;

  // 1) check if the content of the page is really modified (in compare with the content of engine object)
  bool isModified = false;
  if (myContainerName->currentText().compare(getContainerName()) != 0 ) isModified = true;
    
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
	if ( getInputPanel() ) getInputPanel()->emitApply(YACSGui_InputPanel::ComponentId);
      }
      else
	theToApply = false;
    }
    else if ( theToApply )
    {
      onApply();
      if ( getInputPanel() ) getInputPanel()->emitApply(YACSGui_InputPanel::ComponentId);
    }
}

void YACSGui_ComponentPage::onApply()
{
  setContainer();

  if ( mySComponent ) {
    mySComponent->update( EDIT, 0, mySComponent );
    YACSGui_EditionTreeView * aETV = 
      dynamic_cast<YACSGui_EditionTreeView*>(getInputPanel()->getModule()->activeTreeView());
    if(!aETV) return;
    if(aETV->getSelected().empty()) return; 
    if ( YACSGui_ComponentViewItem* aComp = dynamic_cast<YACSGui_ComponentViewItem*>(aETV->getSelected().front()) ) {
      if ( aComp->getSComponent() == mySComponent ) {
	QListViewItem* aChild = aComp->firstChild();
	while( aChild ){
	  if(YACSGui_ReferenceViewItem* aRef = dynamic_cast<YACSGui_ReferenceViewItem*>(aChild)) {
	    SubjectServiceNode* aSSNode = dynamic_cast<SubjectServiceNode*>(aRef->getSReference()->getParent());
	    ServiceNode* sNode = dynamic_cast<ServiceNode*>(aSSNode->getNode());
	    if(aSSNode && sNode) {
	      YACSGui_InputPanel* anIP = getInputPanel();
	      YACSGui_ServiceNodePage* aSNPage = 
		dynamic_cast<YACSGui_ServiceNodePage*>( anIP->getPage( YACSGui_InputPanel::ServiceNodeId ) );
	      if( aSNPage ) {
		if ( !dynamic_cast<CORBANode*>(sNode) ) {
		  YACSGui_ContainerPage* aContPage = 
		    dynamic_cast<YACSGui_ContainerPage*>( anIP->getPage( YACSGui_InputPanel::ContainerId ) );
		  if ( aContPage ) {
		    SubjectContainer* aCont = 0;
		    if ( mySComponent->getComponent()->getContainer() )
		      aCont = GuiContext::getCurrent()->_mapOfSubjectContainer[mySComponent->getComponent()->getContainer()];
		    aContPage->setSContainer(aCont);
		    anIP->setOn( false, YACSGui_InputPanel::ContainerId );
		  }
		}
	    
		aSNPage->setSNode( aSSNode );
		anIP->setOn( true, YACSGui_InputPanel::ServiceNodeId );
		anIP->setMode( YACSGui_InputPanel::EditMode, YACSGui_InputPanel::ServiceNodeId );
		aSNPage->setComponent(mySComponent->getComponent());
		aSNPage->onApply();
	      }
	    }
	  }
	  aChild = aChild->nextSibling();
	}
      }
    }
  }
}

void YACSGui_ComponentPage::fillContainerNames()
{
  myContainerName->clear();
  
  if ( myType == CORBA )
    myContainerName->setEnabled(false);
  else
    myContainerName->setEnabled(true);

  Proc* aProc = GuiContext::getCurrent()->getProc();
  if ( !aProc ) return;

  for ( map<string,Container*>::iterator it = aProc->containerMap.begin(); it != aProc->containerMap.end(); it++ )
    myContainerName->insertItem( (*it).first );
}

void YACSGui_ComponentPage::updateState()
{
  myComponentName->setText( getComponentName() );
  myComponentName->setReadOnly(true);  

  myInstanceName->setText( getInstanceName() );
  myInstanceName->setReadOnly(true);  
  
  // Set component type
  setComponentType();

  // Set container
  fillContainerNames();
  myContainerName->setCurrentText( getContainerName() );
}

void YACSGui_ComponentPage::onContainerChanged( const QString& theContainerName )
{
  Proc* aProc = GuiContext::getCurrent()->getProc();
  if ( !aProc ) return;

  std::string aName = theContainerName.latin1();
  Container* aContainer = aProc->containerMap[ aName ];

  if ( getComponent() && getComponent()->getKind() != CORBAComponent::KIND )
  {
    YACSGui_ContainerPage* aContPage = 
      dynamic_cast<YACSGui_ContainerPage*>( getInputPanel()->getPage( YACSGui_InputPanel::ContainerId ) );
    if ( aContPage && aContPage->isVisible() )
      aContPage->setSContainer( GuiContext::getCurrent()->_mapOfSubjectContainer[aContainer] );
  }
}
