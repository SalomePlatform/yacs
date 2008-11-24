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
  Class : YACSGui_ForLoopNodePage
  Description :  Page for FOR node properties
*/

YACSGui_ForLoopNodePage::YACSGui_ForLoopNodePage( QWidget* theParent, const char* theName, WFlags theFlags )
  : ForLoopNodePage( theParent, theName, theFlags ),
    YACSGui_NodePage()
{
  // TODO: implement presentation of the FOR node in collapsed mode
  // temporary solution :
  myCollapsedRadioButton->setEnabled(false );

  connect( myNodeName, SIGNAL(textChanged( const QString& )), this, SLOT(onNodeNameChanged( const QString& )) );
}

YACSGui_ForLoopNodePage::~YACSGui_ForLoopNodePage()
{
  if (getInputPanel()) getInputPanel()->removePage(this);
}

void YACSGui_ForLoopNodePage::setSNode( YACS::HMI::SubjectNode* theSNode )
{
  if ( !theSNode ) return;

  if ( dynamic_cast<ForLoop*>( theSNode->getNode() ) )
    YACSGui_NodePage::setSNode( theSNode );
}

void YACSGui_ForLoopNodePage::setMode( const YACSGui_InputPanel::PageMode theMode )
{
  YACSGui_NodePage::setMode(theMode);

  if ( myMode == YACSGui_InputPanel::EditMode )
  {
    myNodeName->setReadOnly(false);
    ViewModeButtonGroup->show();

    ExecutionGroupBox->hide();
  } 
  else if ( myMode == YACSGui_InputPanel::RunMode )
  {
    myNodeName->setReadOnly(true);
    ViewModeButtonGroup->hide();

    ExecutionGroupBox->show();
  }
}

void YACSGui_ForLoopNodePage::notifyNodeStatus( int theStatus )
{
  if ( myNodeState )
  {
    myNodeState->setPaletteForegroundColor(getColor(theStatus));
    myNodeState->setText(getText(theStatus));
  }
}

void YACSGui_ForLoopNodePage::notifyNodeProgress()
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

void YACSGui_ForLoopNodePage::notifyInPortValues( std::map<std::string,std::string> theInPortName2Value )
{
  DEBTRACE("==> ForLoopNodePage : Size of theInPortName2Value : " << theInPortName2Value.size());
  
  ForLoop* aForLoopNode = dynamic_cast<ForLoop*>( getNode() );
  if ( !aForLoopNode ) return;

  if ( theInPortName2Value.size() == 1 )
  {
    // nsteps port
    QString aName(aForLoopNode->edGetNbOfTimesInputPort()->getName());
    map<string,string>::iterator it = theInPortName2Value.begin();
    QString aGivenName( (*it).first );
    if ( !aGivenName.compare(aName) && myNbTimesInputPortValue )
    {
      QString aValue( (*it).second );
      if ( !aValue.compare(QString("< ? >")) )
	myNbTimesInputPortValue->setValue( myNbTimesInputPortValue->minValue() );
      else
	myNbTimesInputPortValue->setValue( aValue.toInt() );
    }
  }
}

void YACSGui_ForLoopNodePage::notifyOutPortValues( std::map<std::string,std::string> theOutPortName2Value )
{
  DEBTRACE("==> ForLoopNodePage : " << theOutPortName2Value.size());
}

void YACSGui_ForLoopNodePage::notifyNodeCreateBody( YACS::HMI::Subject* theSubject )
{
  DEBTRACE("YACSGui_ForLoopNodePage::notifyNodeCreateBody");
  if( myLoopBodyNodeName )
  {
    QString aBodyName = theSubject->getName();
    myLoopBodyNodeName->setText( aBodyName );
  }
}

void YACSGui_ForLoopNodePage::checkModifications()
{
  if ( !getNode() ) return;

  // 1) check if the content of the page is really modified (in compare with the content of engine object)
  bool isModified = false;

  if ( myNodeName->text().compare(getNodeName()) != 0 ) isModified = true;
  else if ( YACS::ENGINE::ForLoop* aForLoopNode = dynamic_cast<ForLoop*>( getNode() ) )
    if( YACS::ENGINE::InputPort* aPort = aForLoopNode->edGetNbOfTimesInputPort() )
    {
      QString val=getPortValue( aPort );
      if ( !val.compare(QString("< ? >")) )
        {
          if(myNbTimesInputPortValue->value() != myNbTimesInputPortValue->minValue()) isModified = true;
        }
      else if(myNbTimesInputPortValue->value() != val.toInt()) isModified = true;
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
      if ( getInputPanel() ) getInputPanel()->emitApply(YACSGui_InputPanel::ForLoopNodeId);
    }
    else
      updateState();
}

void YACSGui_ForLoopNodePage::onApply()
{
  DEBTRACE("YACSGui_ForLoopNodePage::onApply");
  onApplyStatus="OK";

  // Rename a node
  if ( myNodeName ) setNodeName( myNodeName->text() );
  
  // Reset number of times input port value
  if ( YACS::ENGINE::ForLoop* aForLoopNode = dynamic_cast<ForLoop*>( getNode() ) )
  {
    if( YACS::ENGINE::InputPort* aPort = aForLoopNode->edGetNbOfTimesInputPort() )
    {
      int aValue = myNbTimesInputPortValue->value();
      aPort->edInit( aValue );
      mySNode->update( EDIT, INPUTPORT, GuiContext::getCurrent()->_mapOfSubjectDataPort[aPort] );
    }
  }

  // Reset the view mode
  // ...
  if(onApplyStatus=="OK")
    updateState();

  updateBlocSize();
}

void YACSGui_ForLoopNodePage::updateState()
{
  DEBTRACE("YACSGui_ForLoopNodePage::updateState");
  // Set node name
  if ( myNodeName ) myNodeName->setText( getNodeName() );

  // Set node full name (read only)
  if ( myNodeFullName )
    myNodeFullName->setText( getNode()->getRootNode()->getChildName(getNode()) );

  // Set number of times input port value and loop body node name
  if ( YACS::ENGINE::ForLoop* aForLoopNode = dynamic_cast<ForLoop*>( getNode() ) )
  {
    if( YACS::ENGINE::InputPort* aPort = aForLoopNode->edGetNbOfTimesInputPort() )
    {
      bool ok;
      int aValue = getPortValue( aPort ).toInt( &ok );
      if( ok && aValue > 1 )
	myNbTimesInputPortValue->setValue( aValue );
    }

    if( myLoopBodyNodeName )
    {
      std::list<Node*> aNodes = aForLoopNode->edGetDirectDescendants();
      if ( !aNodes.empty() )
      {
	QString aBodyName;
	
	std::list<Node*>::iterator aNodesIt = aNodes.begin();
	Node* aNode = *aNodesIt;
	if( aNode )
	  aBodyName = aNode->getName();
	
	myLoopBodyNodeName->setText( aBodyName );
      }
    }
  }

  // Set view mode of the given FOR node
  // the following method can be used if its needed:
  // YACSGui_Graph* aGraph = getInputPanel()->getModule()->getGraph( myProc );
  // ...
}

void YACSGui_ForLoopNodePage::onNodeNameChanged( const QString& theName )
{
  if ( myNodeFullName )
  {
    QString anOldName = myNodeFullName->text();
    int aRDotId = anOldName.findRev('.');
    myNodeFullName->setText( anOldName.replace( aRDotId+1, anOldName.length()-(aRDotId+1), theName ) );
  }
}

