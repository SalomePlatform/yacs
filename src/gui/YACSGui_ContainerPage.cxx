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
  Class : YACSGui_ContainerPage
  Description :  Page for container properties
*/

YACSGui_ContainerPage::YACSGui_ContainerPage( QWidget* theParent, const char* theName, WFlags theFlags )
  : ContainerPage( theParent, theName, theFlags ),
    GuiObserver(),
    mySContainer( 0 )
{
  showAdvanced->setChecked(false);
  onShowAdvanced();
  connect(showAdvanced, SIGNAL(clicked()), this, SLOT(onShowAdvanced()));
}

YACSGui_ContainerPage::~YACSGui_ContainerPage()
{
  if (getInputPanel()) getInputPanel()->removePage(this);

  if ( mySContainer ) mySContainer->detach(this);
}

void YACSGui_ContainerPage::select( bool isSelected )
{
  printf(">> YACSGui_ContainerPage::select\n");
}

void YACSGui_ContainerPage::update( YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son)
{
  printf(">> YACSGui_ContainerPage::update\n");
  switch (event)
  {
  case REMOVE:
    {
      if ( !type && !son && mySContainer )
      {
	mySContainer->detach(this);
	mySContainer = 0;
      }
    }
    break;
  default:
    GuiObserver::update(event, type, son);
  }
}

void YACSGui_ContainerPage::setSContainer(YACS::HMI::SubjectContainer* theSContainer )
{
  if ( mySContainer != theSContainer )
  {
    if ( mySContainer ) mySContainer->detach(this); //detach from old container
    mySContainer = theSContainer;
    if ( mySContainer ) mySContainer->attach(this); // attach to new container

    fillHostNames();
    updateState();
  }

  if ( theSContainer )
  {
    // set "Container Name" fields to read only if the user is editing a service node
    ServiceNode* aService = 0;
    if ( YACSGui_EditionTreeView * aETV = 
	 dynamic_cast<YACSGui_EditionTreeView*>(getInputPanel()->getModule()->activeTreeView()) )
      {
        std::list<QListViewItem*> aList = aETV->getSelected();
        if (!aList.empty())
          if ( YACSGui_NodeViewItem* aNode = dynamic_cast<YACSGui_NodeViewItem*>(aList.front()) )
            aService = dynamic_cast<ServiceNode*>(aNode->getNode());
      }
    
    if ( aService )
      {
        myDefinitionName->setReadOnly(true);
        myExecutionName->setReadOnly(true);
      }
    else
      {
        myDefinitionName->setReadOnly(false);
        myExecutionName->setReadOnly(false);
      }
  }
}

YACS::ENGINE::Container* YACSGui_ContainerPage::getContainer() const
{
  return ( mySContainer ? mySContainer->getContainer() : 0 );
}

YACS::ENGINE::SalomeContainer* YACSGui_ContainerPage::getSalomeContainer() const
{
  return ( mySContainer ? dynamic_cast<SalomeContainer*>(mySContainer->getContainer()) : 0 );
}

QString YACSGui_ContainerPage::getDefinitionName() const
{
  return ( mySContainer ? QString( mySContainer->getName() ) : QString("") );
}

bool YACSGui_ContainerPage::setDefinitionName( const QString& theName )
{
  bool ret = false;
  bool alreadyExists = false;
  if ( getContainer() )
    {
      GuiContext* aContext = GuiContext::getCurrent();
      map<Container*, SubjectContainer*>::const_iterator it;
      for(it = aContext->_mapOfSubjectContainer.begin(); it !=aContext->_mapOfSubjectContainer.end(); ++it)
        {
          if ( QString::compare(theName, (*it).first->getName().c_str()) == 0)
            {
              alreadyExists =true;
              break;
            }
        }
      }
  if (!alreadyExists)
  {
    getContainer()->setName( theName.latin1() );
    mySContainer->update( RENAME, 0, mySContainer );
    ret = true;
  }
  return ret;
}

QString YACSGui_ContainerPage::getExecutionName() const
{
  QString name = "";
  if (getSalomeContainer())
    {
      string nc = getSalomeContainer()->getProperty("container_name");
      name = nc.c_str();
    } 
  return name;
}

void YACSGui_ContainerPage::setExecutionName( const QString& theName )
{
  if ( getSalomeContainer() )
    {
      getSalomeContainer()->setProperty( "container_name", theName.latin1() );
      mySContainer->update( RENAME, 0, mySContainer );
    }
}

YACSGui_InputPanel* YACSGui_ContainerPage::getInputPanel() const
{
  SalomeApp_Application* anApp = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
  if( !anApp )
    return 0;

  YACSGui_Module* aModule = dynamic_cast<YACSGui_Module*>( anApp->activeModule() );
  if( !aModule )
    return 0;

  return aModule->getInputPanel();
}

void YACSGui_ContainerPage::fillHostNames() const
{
  SalomeApp_Application* anApp = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
  SALOME_NamingService* namingService = anApp->namingService();

  CORBA::Object_var obj = namingService->Resolve(SALOME_ResourcesManager::_ResourcesManagerNameInNS);
  if ( !CORBA::is_nil(obj) )
    if ( Engines::ResourcesManager_var aResManager=Engines::ResourcesManager::_narrow(obj) )
    {
      Engines::CompoList aCompoList ;
      Engines::MachineParameters params;
      anApp->lcc()->preSet(params);

      Engines::MachineList* aMachineList = aResManager->GetFittingResources( params, aCompoList );

      myHostName->clear();
      myHostName->insertItem( QString("localhost") );
      for ( unsigned int i = 0; i < aMachineList->length(); i++ )
      {
	const char* aMachine = (*aMachineList)[i];
	myHostName->insertItem( QString(aMachine) );
      }
    }
}

QString YACSGui_ContainerPage::getHostName() const
{
  if ( getSalomeContainer() )
  {
    QString aHN(getSalomeContainer()->_params.hostname);
    return aHN;
  }
  else 
    return QString("");
}

void YACSGui_ContainerPage::setHostName( const QString& theHostName )
{
  if ( getSalomeContainer() )
    getSalomeContainer()->setProperty( "hostname", theHostName.latin1() );
}

int YACSGui_ContainerPage::getMemMb() const
{
  return ( getSalomeContainer() ? getSalomeContainer()->_params.mem_mb : 0 );
}

void YACSGui_ContainerPage::setMemMb(const int theMem) 
{
  if ( getSalomeContainer() )
    getSalomeContainer()->setProperty( "mem_mb", QString("%1").arg(theMem).latin1() );
}

int YACSGui_ContainerPage::getCpuClock() const
{
  return ( getSalomeContainer() ? getSalomeContainer()->_params.cpu_clock : 0 );
}

void YACSGui_ContainerPage::setCpuClock(const int theClock) 
{
  if ( getSalomeContainer() )
    getSalomeContainer()->setProperty( "cpu_clock", QString("%1").arg(theClock).latin1() );
}

int YACSGui_ContainerPage::getNbProcPerNode() const
{
  return ( getSalomeContainer() ? getSalomeContainer()->_params.nb_proc_per_node : 0 );
}

void YACSGui_ContainerPage::setNbProcPerNode(const int theNbProc) 
{
  if ( getSalomeContainer() )
    getSalomeContainer()->setProperty( "nb_proc_per_node", QString("%1").arg(theNbProc).latin1() );
}

int YACSGui_ContainerPage::getNbNode() const
{
  return ( getSalomeContainer() ? getSalomeContainer()->_params.nb_node : 0 );
}

void YACSGui_ContainerPage::setNbNode(const int theNbNode) 
{
  if ( getSalomeContainer() )
    getSalomeContainer()->setProperty( "nb_node", QString("%1").arg(theNbNode).latin1() );
}

bool YACSGui_ContainerPage::getMpiUsage() const
{
  return ( getSalomeContainer() ? getSalomeContainer()->_params.isMPI : false );
}

void YACSGui_ContainerPage::setMpiUsage(const bool on)
{
  if ( getSalomeContainer() )
    getSalomeContainer()->setProperty( "isMPI", on ? "true" : "false" );
}

QString YACSGui_ContainerPage::getParallelLib() const
{
  return ( getSalomeContainer() ? QString(getSalomeContainer()->_params.parallelLib) : QString("") );
}

void YACSGui_ContainerPage::setParallelLib( const QString& theLib)
{
  if ( getSalomeContainer() )
    getSalomeContainer()->setProperty( "parallelLib", theLib.latin1() );
}

int YACSGui_ContainerPage::getNbComponentNodes() const
{
  return ( getSalomeContainer() ? getSalomeContainer()->_params.nb_component_nodes : 0 );
}

void YACSGui_ContainerPage::setNbComponentNodes(const int theNbComponentNodes)
{
  if ( getSalomeContainer() )
    getSalomeContainer()->setProperty( "nb_component_nodes", QString("%1").arg(theNbComponentNodes).latin1() );
}

void YACSGui_ContainerPage::checkModifications( bool& theWarnToShow, bool& theToApply )
{
  if ( !getContainer() || !theWarnToShow && !theToApply ) return;

  // 1) check if the content of the page is really modified (in compare with the content of engine object)
  bool isModified = false;
  if ( myDefinitionName->text().compare(getDefinitionName()) != 0
       ||
       myExecutionName->text().compare(getExecutionName()) != 0
       ||
       myHostName->currentText().compare(getHostName()) != 0
       ||
       myMemMb->value() != getMemMb()
       ||
       myCpuClock->value() != getCpuClock()
       ||
       myNbProcPerNode->value() != getNbProcPerNode()
       ||
       myNbNode->value() != getNbNode()
       ||
       ( myMpiUsage->currentItem() ? false : true ) != getMpiUsage()
       ||
       myParallelLib->text().compare(getParallelLib()) != 0
       ||
       myNbComponentNodes->value() != getNbComponentNodes() ) isModified = true;
    
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
	if ( getInputPanel() ) getInputPanel()->emitApply(YACSGui_InputPanel::ContainerId);
      }
      else
	theToApply = false;
    }
    else if ( theToApply )
    {
      onApply();
      if ( getInputPanel() ) getInputPanel()->emitApply(YACSGui_InputPanel::ContainerId);
    }
}

void YACSGui_ContainerPage::onShowAdvanced()
{
  if(showAdvanced->isChecked()) {
    groupBoxAdv->show();
  } else {
    groupBoxAdv->hide();
  }
}

void YACSGui_ContainerPage::updateState()
{
  // Set container name

  if ( myDefinitionName )
    {
      QString defName = getDefinitionName();
      if (QString::compare(defName, "DefaultContainer") == 0)
        myDefinitionName->setReadOnly(true);
      else
        myDefinitionName->setReadOnly(false);
      myDefinitionName->setText(defName);
      
    }

  if ( myExecutionName )
    myExecutionName->setText(getExecutionName());

  // Set host name
  if ( myHostName )
    myHostName->setCurrentText(getHostName());

  // Set mem_mb
  if ( myMemMb )
    myMemMb->setValue(getMemMb());

  // Set cpu_clock
  if ( myCpuClock )
    myCpuClock->setValue(getCpuClock());
  
  // Set nb_proc_per_node
  if ( myNbProcPerNode )
    myNbProcPerNode->setValue(getNbProcPerNode());

  // Set nb_node
  if ( myNbNode )
    myNbNode->setValue(getNbNode());
  
  // Set MPI usage
  if ( myMpiUsage )
    myMpiUsage->setCurrentItem( getMpiUsage() ? 0 : 1 );

  // Set parallel library
  if ( myParallelLib )
    myParallelLib->setText(getParallelLib());

  // Set nb_component_nodes
  if ( myNbComponentNodes )
    myNbComponentNodes->setValue(getNbComponentNodes());
}

void YACSGui_ContainerPage::onApply()
{
  if ( myDefinitionName )
    if (! setDefinitionName(myDefinitionName->text().latin1()))
      if (getContainer()) myDefinitionName->setText(getContainer()->getName());

  if ( myExecutionName )
    setExecutionName(myExecutionName->text().latin1());

  if ( myHostName )
    setHostName(myHostName->currentText().latin1());

  if ( myMemMb )
    setMemMb(myMemMb->value());

  if ( myCpuClock )
    setCpuClock(myCpuClock->value());
  
  if ( myNbProcPerNode )
    setNbProcPerNode(myNbProcPerNode->value());

  if ( myNbNode )
    setNbNode(myNbNode->value());
  
  if ( myMpiUsage )
    setMpiUsage( myMpiUsage->currentItem() ? false : true );

  if ( myParallelLib )
    setParallelLib(myParallelLib->text().latin1());

  if ( myNbComponentNodes )
    setNbComponentNodes(myNbComponentNodes->value());

  //mySContainer->update( EDIT, 0, mySContainer );
}

