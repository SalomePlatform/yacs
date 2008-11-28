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
  Class : YACSGui_SchemaPage
  Description :  Page for schema properties
*/

YACSGui_SchemaPage::YACSGui_SchemaPage( QWidget* theParent, const char* theName, WFlags theFlags )
  : SchemaPage( theParent, theName, theFlags ),
    GuiObserver(),
    mySProc( 0 ),
    myMode( YACSGui_InputPanel::EditMode )
{
  DEBTRACE("YACSGui_SchemaPage::YACSGui_SchemaPage");
  myErrorLog->setReadOnly(true);
  // temporary disabled, because of data flow and data stream views are not yet implemented
  myDataFlowMode_2->setEnabled(false);
  myDataStreamMode_2->setEnabled(false);

  SalomeApp_Application* anApp = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
  if( anApp )
  {
    anApp->lcc()->preSet(myParams);
  }
  else
  {
    myParams.OS = "";
    myParams.mem_mb = 0;
    myParams.cpu_clock = 0;
    myParams.nb_proc_per_node = 0;
    myParams.nb_node = 0;
    myParams.isMPI = false;
    myParams.parallelLib = "";
    myParams.nb_component_nodes = 0;
  }
  myParams.container_name = "YACSServer";
  myParams.hostname = "localhost";
  myExecutionName->setText(QString(myParams.container_name));
  myHostName->setCurrentText(QString(myParams.hostname));

  fillHostNames();
}

YACSGui_SchemaPage::~YACSGui_SchemaPage()
{
  if (getInputPanel()) getInputPanel()->removePage(this);
}

void YACSGui_SchemaPage::select( bool isSelected )
{
  DEBTRACE("YACSGui_SchemaPage::select");
}

void YACSGui_SchemaPage::update( YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son)
{
  DEBTRACE("YACSGui_SchemaPage::update");
}

void YACSGui_SchemaPage::setSProc( YACS::HMI::SubjectProc* theSProc )
{ 
  if ( theSProc && mySProc != theSProc )
  {
    if ( mySProc ) mySProc->detach(this); //detach from old schema
    mySProc = theSProc;
    mySProc->attach(this); // attach to new schema
    updateState();
  }
}

YACS::ENGINE::Proc* YACSGui_SchemaPage::getProc() const
{
  return ( mySProc ? dynamic_cast<Proc*>(mySProc->getNode()) : 0 );
}

QString YACSGui_SchemaPage::getSchemaName() const
{
  return ( mySProc ? QString( mySProc->getName() ) : QString("") );
}

void YACSGui_SchemaPage::setSchemaName( const QString& theName )
{
  if ( mySProc && myMode == YACSGui_InputPanel::EditMode ) {
    mySProc->getNode()->setName( theName.latin1() );
    mySProc->update( RENAME, 0, mySProc );

    if(getInputPanel())
      getInputPanel()->getModule()->getDataModel()->updateItem( getProc(), true );
  }
}

void YACSGui_SchemaPage::onApply()
{
  DEBTRACE("YACSGui_SchemaPage::onApply");
  // Rename a schema
  if ( mySchemaName ) setSchemaName( mySchemaName->text() );

  // set container parameters for YACS Engine 
  if ( myMode == YACSGui_InputPanel::RunMode )
    if (GuiContext *context = GuiContext::getCurrent())
      {
        pair<string,string> yc(myExecutionName->text().latin1(), myHostName->currentText().latin1());
        context->setYACSContainer(yc);
        myParams.container_name = myExecutionName->text().latin1();
        myParams.hostname = myHostName->currentText().latin1();
      }

  // Change a view mode of a schema
  QButton* aSelBtn = ViewModeButtonGroup->selected();
  if ( aSelBtn && getInputPanel() )
    if ( aSelBtn == myFullMode_2 )
      getInputPanel()->getModule()->onFullView();
    else if ( aSelBtn == myControlMode_2 )
      getInputPanel()->getModule()->onControlView();
    //else if ( aSelBtn == myDataFlowMode_2 )
    //else if ( aSelBtn == myDataStreamMode_2 )
}

YACSGui_InputPanel* YACSGui_SchemaPage::getInputPanel() const
{
  DEBTRACE("YACSGui_SchemaPage::getInputPanel");
  SalomeApp_Application* anApp = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
  if( !anApp )
    return 0;

  YACSGui_Module* aModule = dynamic_cast<YACSGui_Module*>( anApp->activeModule() );
  if( !aModule )
    return 0;

  return aModule->getInputPanel();
}

void YACSGui_SchemaPage::setMode( const YACSGui_InputPanel::PageMode theMode )
{
  DEBTRACE("YACSGui_SchemaPage::setMode");
  myMode = theMode;

  if ( !ViewModeButtonGroup || !NextToRunGroupBox || !mySchemaName ) return;

  if ( myMode == YACSGui_InputPanel::EditMode )
  {
    ViewModeButtonGroup->show();
    ContainerGroupBox->hide();
    NextToRunGroupBox->hide();
    mySchemaName->setReadOnly(false);
  } 
  else if ( myMode == YACSGui_InputPanel::RunMode )
  {
    ViewModeButtonGroup->hide();
    ContainerGroupBox->show();
    NextToRunGroupBox->show();
    mySchemaName->setReadOnly(true);
    // --- when YACS engine created, its parameters are not modifiable
    if (getInputPanel()->getModule()->lookForExecutor())
      {
        myExecutionName->setReadOnly(true);
        myHostName->setEnabled(false);
      }
    else
      {
        myExecutionName->setReadOnly(false);
        myHostName->setEnabled(true);
      }
  }

  if ( mySchemaName ) mySchemaName->setText( getSchemaName() );
}

void YACSGui_SchemaPage::checkModifications()
{
  DEBTRACE("YACSGui_SchemaPage::checkModifications");
  if ( !getProc() ) return;

  // 1) check if the content of the page is really modified (in compare with the content of engine object)
  bool isModified = false;

  if ( myMode == YACSGui_InputPanel::RunMode )
    {
      if (GuiContext *context = GuiContext::getCurrent())
        {
          DEBTRACE(isModified);
          QString contname(myParams.container_name);
          QString hostname(myParams.hostname);
          if (myExecutionName->text().compare(contname) != 0 ) isModified = true;
          if (myHostName->currentText().compare(hostname) != 0 ) isModified = true;
          DEBTRACE(isModified);
        }
    }
  else
    {
      if ( mySchemaName->text().compare(getSchemaName()) != 0 ) isModified = true;

      if ( !isModified )
        if ( mySProc && getInputPanel() )
          if ( Proc* aProc = getProc() )
            {
              // get gui graph
              YACSGui_Graph* aGraph = getInputPanel()->getModule()->getGraph(aProc);
              if ( !aGraph ) return;
              
              int aViewMode = aGraph->getDMode();
              if ( myFullMode_2->isOn() && aViewMode != YACSGui_Graph::FullId
                   ||
                   myControlMode_2->isOn() && aViewMode != YACSGui_Graph::ControlId
                   ||
                   myDataFlowMode_2->isOn() && aViewMode != YACSGui_Graph::DataflowId
                   ||
                   myDataStreamMode_2->isOn() && aViewMode != YACSGui_Graph::DataStreamId ) isModified = true;
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
        if ( getInputPanel() ) getInputPanel()->emitApply(YACSGui_InputPanel::SchemaId);
      }
    else updateState();
}

void YACSGui_SchemaPage::onAddAllNextToRun()
{
  list<string> nextStepList;
  QListViewItemIterator it(myNextStepsListView);
  while(it.current())
  {
    ((QCheckListItem*)it.current())->setOn(true);
    nextStepList.push_back(it.current()->text(0));
    ++it;
  }
  if ( YACSGui_Executor* anExecutor = getInputPanel()->getModule()->lookForExecutor() )
    anExecutor->setNextStepList(nextStepList);
}

void YACSGui_SchemaPage::onRemoveAllNextToRun()
{
  list<string> nextStepList;
  QListViewItemIterator it(myNextStepsListView);
  while(it.current())
  {
    ((QCheckListItem*)it.current())->setOn(false);
    ++it;
  }
  if ( YACSGui_Executor* anExecutor = getInputPanel()->getModule()->lookForExecutor() )
    anExecutor->setNextStepList(nextStepList);
}

void YACSGui_SchemaPage::onNextStepClicked( QListViewItem* theItem )
{
  list<string> nextStepList;
  QListViewItemIterator it(myNextStepsListView);
  for ( ; it.current(); ++it )
    if (((QCheckListItem*)it.current())->isOn())
      nextStepList.push_back(it.current()->text(0));
  if ( YACSGui_Executor* anExecutor = getInputPanel()->getModule()->lookForExecutor() )
    anExecutor->setNextStepList(nextStepList);
}

void YACSGui_SchemaPage::onNotifyNextSteps( std::list<std::string> theNextSteps )
{
  myNextStepsListView->clear();
  myMapNextSteps.clear();
  myNextStepsListView->setRootIsDecorated(true);
  while (! theNextSteps.empty())
  {
    QCheckListItem *item = new QCheckListItem(myNextStepsListView,
					      theNextSteps.front().c_str(),
					      QCheckListItem::CheckBox );
    myMapNextSteps[item] = theNextSteps.front();
    item->setOn(true);
    theNextSteps.pop_front();
  }
}

void YACSGui_SchemaPage::updateState()
{
  DEBTRACE("YACSGui_SchemaPage::updateState");
  // Set schema name
  if ( mySchemaName ) mySchemaName->setText( getSchemaName() );

  if ( myMode == YACSGui_InputPanel::RunMode )
    if (GuiContext *context = GuiContext::getCurrent())
      {
        myParams.container_name = context->getYACSCont().first.c_str();
        myParams.hostname = context->getYACSCont().second.c_str();
        myExecutionName->setText(QString(myParams.container_name));
        myHostName->setCurrentText(QString(myParams.hostname));
      }

  if ( myMode == YACSGui_InputPanel::RunMode && myNextStepsListView )
    myNextStepsListView->clear();
  
  // Set view mode of the given schema
  if ( mySProc )
    if ( Proc* aProc = getProc() )
    {
      // get gui graph
      YACSGui_Graph* aGraph = getInputPanel()->getModule()->getGraph(aProc);
      if ( !aGraph ) return;

      int aViewMode = aGraph->getDMode();
      if ( aViewMode == YACSGui_Graph::FullId && !myFullMode_2->isOn() )
	myFullMode_2->toggle();
      else if ( aViewMode == YACSGui_Graph::ControlId && !myControlMode_2->isOn() )
	myControlMode_2->toggle();
      else if ( aViewMode == YACSGui_Graph::DataflowId && !myDataFlowMode_2->isOn() )
	myDataFlowMode_2->toggle();
      else if ( aViewMode == YACSGui_Graph::DataStreamId && !myDataStreamMode_2->isOn() )
	myDataStreamMode_2->toggle();
    }
}

void YACSGui_SchemaPage::fillHostNames() const
{
  SalomeApp_Application* anApp = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
  if ( !anApp ) return;

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

