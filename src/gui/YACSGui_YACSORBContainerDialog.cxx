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
  Class : YACSGui_YACSORBContainerDialog
  Description :  Page for container properties
*/

YACSGui_YACSORBContainerDialog::YACSGui_YACSORBContainerDialog( QWidget* theParent, const char* theName, WFlags theFlags )
  : QDialog( theParent, theName, true, theFlags )
{
  setCaption(theName);

  QVBoxLayout* aTopLayout = new QVBoxLayout( this );
  aTopLayout->setSpacing( 6 );
  aTopLayout->setMargin( 11 );

  myPage = new ContainerPage(this);

  QGroupBox* aGrpBtns = new QGroupBox( this, "GroupButtons" );
  aGrpBtns->setColumnLayout(0, Qt::Vertical );
  aGrpBtns->layout()->setSpacing( 0 ); aGrpBtns->layout()->setMargin( 0 );
  aGrpBtns->setFrameStyle( QFrame::NoFrame );
  QHBoxLayout* aGrpBtnsLayout = new QHBoxLayout( aGrpBtns->layout() );
  aGrpBtnsLayout->setAlignment( Qt::AlignTop );
  aGrpBtnsLayout->setSpacing( 0 ); aGrpBtnsLayout->setMargin( 0 );

  QPushButton* okB = new QPushButton( tr( "BUT_OK" ), aGrpBtns );
  connect( okB, SIGNAL( clicked() ), this, SLOT( onApply() ) );

  aGrpBtnsLayout->addItem  ( new QSpacerItem( 5, 5, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
  aGrpBtnsLayout->addWidget( okB );
  aGrpBtnsLayout->addItem  ( new QSpacerItem( 5, 5, QSizePolicy::Expanding, QSizePolicy::Minimum ) );

  aTopLayout->addWidget(myPage);
  aTopLayout->addStretch(1);
  aTopLayout->addWidget(aGrpBtns);
  
  SalomeApp_Application* anApp = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
  if( anApp )
  {
    anApp->lcc()->preSet(myParams);
    myParams.container_name = "YACSServer";
    myParams.hostname = "localhost";
  }
  else
  {
    myParams.container_name = "YACSServer";
    myParams.hostname = "localhost";
    myParams.OS = "";
    myParams.mem_mb = 0;
    myParams.cpu_clock = 0;
    myParams.nb_proc_per_node = 0;
    myParams.nb_node = 0;
    myParams.isMPI = false;
    myParams.parallelLib = "";
    myParams.nb_component_nodes = 0;
  }

  myPage->showAdvanced->setChecked(false);
  onShowAdvanced();
  updateState();
  myPage->myHostName->setEditable(true);
  connect(myPage->showAdvanced, SIGNAL(clicked()), this, SLOT(onShowAdvanced()));
}

YACSGui_YACSORBContainerDialog::~YACSGui_YACSORBContainerDialog()
{
}

void YACSGui_YACSORBContainerDialog::fillHostNames() const
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

      myPage->myHostName->clear();
      myPage->myHostName->insertItem( QString("localhost") );
      for ( unsigned int i = 0; i < aMachineList->length(); i++ )
      {
	const char* aMachine = (*aMachineList)[i];
	myPage->myHostName->insertItem( QString(aMachine) );
      }
    }
}

void YACSGui_YACSORBContainerDialog::onShowAdvanced()
{
  if(myPage->showAdvanced->isChecked())
    myPage->groupBoxAdv->show();
  else
    myPage->groupBoxAdv->hide();
}

void YACSGui_YACSORBContainerDialog::updateState()
{
  fillHostNames();

  // Set container name
  if ( myPage->myExecutionName )
    myPage->myExecutionName->setText(QString(myParams.container_name));

  // Set host name
  if ( myPage->myHostName )
    myPage->myHostName->setCurrentText(QString(myParams.hostname));

  // Set mem_mb
  if ( myPage->myMemMb )
    myPage->myMemMb->setValue(myParams.mem_mb);

  // Set cpu_clock
  if ( myPage->myCpuClock )
    myPage->myCpuClock->setValue(myParams.cpu_clock);
  
  // Set nb_proc_per_node
  if ( myPage->myNbProcPerNode )
    myPage->myNbProcPerNode->setValue(myParams.nb_proc_per_node);

  // Set nb_node
  if ( myPage->myNbNode )
    myPage->myNbNode->setValue(myParams.nb_node);
  
  // Set MPI usage
  if ( myPage->myMpiUsage )
    myPage->myMpiUsage->setCurrentItem( myParams.isMPI ? 0 : 1 );

  // Set parallel library
  if ( myPage->myParallelLib )
    myPage->myParallelLib->setText(QString(myParams.parallelLib));

  // Set nb_component_nodes
  if ( myPage->myNbComponentNodes )
    myPage->myNbComponentNodes->setValue(myParams.nb_component_nodes);
}

void YACSGui_YACSORBContainerDialog::onApply()
{
  if ( myPage->myExecutionName )
    myParams.container_name = myPage->myExecutionName->text().latin1();

  if ( myPage->myHostName )
    myParams.hostname = myPage->myHostName->currentText().latin1();

  if ( myPage->myMemMb )
    myParams.mem_mb = myPage->myMemMb->value();

  if ( myPage->myCpuClock )
    myParams.cpu_clock = myPage->myCpuClock->value();
  
  if ( myPage->myNbProcPerNode )
    myParams.nb_proc_per_node = myPage->myNbProcPerNode->value();

  if ( myPage->myNbNode )
    myParams.nb_node = myPage->myNbNode->value();
  
  if ( myPage->myMpiUsage )
    myParams.isMPI = ( myPage->myMpiUsage->currentItem() ? false : true );

  if ( myPage->myParallelLib )
    myParams.parallelLib = myPage->myParallelLib->text().latin1();

  if ( myPage->myNbComponentNodes )
    myParams.nb_component_nodes = myPage->myNbComponentNodes->value();

  accept();
}

