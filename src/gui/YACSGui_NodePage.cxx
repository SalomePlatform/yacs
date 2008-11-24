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
#include <YACSPrs_toString.h>

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
#include <StudyPorts.hxx>
#include <XMLPorts.hxx>
#include <InlineNode.hxx>
#include <DataNode.hxx>
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
#include <TypeConversions.hxx>

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
  Class : YACSGui_NodePage
  Description :  Basic page for node properties
*/

YACSGui_NodePage::YACSGui_NodePage()
  : GuiObserver(),
    mySNode( 0 ),
    myMode(YACSGui_InputPanel::RunMode),
    myType( Unknown ) //?
{
  _destructible=false;
  mySelectDataTypeFor.myRow = -1;
}

YACSGui_NodePage::~YACSGui_NodePage()
{
  DEBTRACE("YACSGui_NodePage::~YACSGui_NodePage");
}

void YACSGui_NodePage::decrementSubjects(YACS::HMI::Subject *subject)
{
  DEBTRACE("YACSGui_NodePage::decrementSubjects");
  mySNode=0;
  YACS::HMI::GuiObserver::decrementSubjects(subject);
}

void YACSGui_NodePage::select( bool isSelected )
{
  DEBTRACE(">> YACSGui_NodePage::select");
}

void YACSGui_NodePage::update( YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son)
{
  DEBTRACE(">> YACSGui_NodePage::update " << event << " " << type);
  switch (event)
  {
  case ADD:
    {
      // check if the parent of the son is a loop
      if( SubjectNode* aSNode = dynamic_cast<SubjectNode*>( son->getParent() ) )
      {
	if ( dynamic_cast<Loop*>( aSNode->getNode() ) || dynamic_cast<ForEachLoop*>( aSNode->getNode() ) )
	  notifyNodeCreateBody( son );
	else if ( dynamic_cast<Switch*>( aSNode->getNode() ) || dynamic_cast<Bloc*>( aSNode->getNode() ) )
	  notifyNodeCreateNode( son );
      }
    }
    break;
  case REMOVE:
    {
    }
    break;
  case UPDATEPROGRESS:
    notifyNodeProgress();
    break;
  default:
    GuiObserver::update(event, type, son);
  }
}

YACSGui_InputPanel* YACSGui_NodePage::getInputPanel() const
{
  SalomeApp_Application* anApp = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
  if( !anApp )
    return 0;

  YACSGui_Module* aModule = dynamic_cast<YACSGui_Module*>( anApp->activeModule() );
  if( !aModule )
    return 0;

  return aModule->getInputPanel();
}

void YACSGui_NodePage::setSNode( YACS::HMI::SubjectNode* theSNode )
{
  DEBTRACE("YACSGui_NodePage::setSNode ");
  if ( !theSNode )
    return;
  //DEBTRACE(theSNode->getName());

  bool isNeedToUpdatePortValues = false;
  if ( mySNode != theSNode )
  {
    if ( mySNode ) mySNode->detach(this); //detach from old node
    mySNode = theSNode;
    mySNode->attach(this); // attach to new node

    // specify the node type
    Node* aNode = getNode();
    if ( dynamic_cast<PyFuncNode*>(aNode) )
      myType = InlineFunction;
    else if ( dynamic_cast<PythonNode*>(aNode) )
      myType = InlineScript;
    else if ( dynamic_cast<SalomeNode*>(aNode) )
      myType = SALOMEService;
    else if ( dynamic_cast<CORBANode*>(aNode) )
      myType = CORBAService;
    else if ( dynamic_cast<CppNode*>(aNode) )
      myType = CPPNode;
    else if ( dynamic_cast<SalomePythonNode*>(aNode) )
      myType = ServiceInline;
    else if ( dynamic_cast<XmlNode*>(aNode) )
      myType = XMLNode;
    else if ( dynamic_cast<DataNode*>(aNode) )
      myType = DATANode;
    else if ( dynamic_cast<ForLoop*>(aNode) )
      myType = FOR;
    else if ( dynamic_cast<ForEachLoop*>(aNode) )
      myType = FOREACH;
    else if ( dynamic_cast<WhileLoop*>(aNode) )
      myType = WHILE;
    else if ( dynamic_cast<Switch*>(aNode) )
      myType = SWITCH;
    else if ( dynamic_cast<Bloc*>(aNode) )
      myType = Block;
    
    if ( aNode )
    {
      notifyNodeStatus( getNode()->getState() );
      notifyNodeProgress();
    }

    isNeedToUpdatePortValues = true;
  }
  
  updateState();

  DEBTRACE("NeedToUpdatePortValues? " << isNeedToUpdatePortValues << " " << myType);
  if ( (isNeedToUpdatePortValues || myType == SALOMEService || myType == InlineFunction|| myType == InlineScript
        || myType == XMLNode ||myType==DATANode) && getNode() )
  {
  DEBTRACE("YACSGui_NodePage::setSNode ");
    Proc* aProc = dynamic_cast<Proc*>(getNode()->getRootNode());
    if ( !aProc ) return;
    
    SalomeApp_Application* anApp = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
    if( !anApp ) return;
    
    YACSGui_Module* aModule = dynamic_cast<YACSGui_Module*>( anApp->activeModule() );
    if( !aModule ) return;
    
    YACSGui_Graph* aGraph = aModule->getGraph( aProc );
    if ( !aGraph ) return;
    
    if ( YACSPrs_ElementaryNode* anItem = aGraph->getItem(getNode()) )
    {
      DEBTRACE("elementary node");
      //MESSAGE("elementary node");
      map<string,string> anInPortName2Value;
      list<InPort*> IPs = getNode()->getSetOfInPort();
      list<InPort*>::iterator itIP = IPs.begin();
      DEBTRACE("Input ports");
      for ( ; itIP!=IPs.end(); itIP++ )
      {
        InPort* aPort = *itIP;
        if ( !aPort )
          continue;

        YACSPrs_InOutPort* aPortPrs = anItem->getPortPrs( aPort );
        if ( !aPortPrs )
          continue;

        QString aCurrVal = aPortPrs->getCurrentValue();
        if ( aCurrVal.isNull() )
          continue; 
        
        anInPortName2Value.insert( 
          make_pair( aPort->getName(), string( aCurrVal.latin1() ) ) );

	  //printf(">> Name = %s, Value = %s\n",
	  // (*itIP)->getName().c_str(),
	  // anItem->getPortPrs(*itIP)->getCurrentValue().latin1());
	
      }
      DEBTRACE("Notify");
      notifyInPortValues( anInPortName2Value );
            
      map<string,string> anOutPortName2Value;
      list<OutPort*> OPs = getNode()->getSetOfOutPort();
      list<OutPort*>::iterator itOP = OPs.begin();
      DEBTRACE("Output ports");
      for ( ; itOP!=OPs.end(); itOP++ )
      {
        OutPort* aPort = *itOP;
        if ( !aPort )
          continue;

        YACSPrs_InOutPort* aPortPrs = anItem->getPortPrs( aPort );
        if ( !aPortPrs )
          continue;

        QString aCurrVal = aPortPrs->getCurrentValue();
        if ( aCurrVal.isNull() )
          continue; 

	  anOutPortName2Value.insert( 
            make_pair( aPort->getName(), string( aCurrVal.latin1() ) ) );

	  //printf(">> Name = %s, Value = %s\n",
	  // (*itOP)->getName().c_str(),
	  // anItem->getPortPrs(*itOP)->getCurrentValue().latin1());
      }
      DEBTRACE("Notify");
      notifyOutPortValues( anOutPortName2Value );
    }
  }
}

YACS::ENGINE::Node* YACSGui_NodePage::getNode() const
{
  return ( mySNode ? mySNode->getNode() : 0 );
}

QString YACSGui_NodePage::getNodeName() const
{
  return ( mySNode ? QString( mySNode->getName() ) : QString("") );
}

void YACSGui_NodePage::setNodeName( const QString& theName )
{
  if ( getNode() )
  {
    try
      {
        getNode()->setName( theName.latin1() );
      }
    catch (YACS::Exception& ex)
      {
        SUIT_MessageBox::error1(getInputPanel()->getModule()->getApp()->desktop(),
                                QObject::tr("ERROR"),
                                ex.what(),
                                QObject::tr("BUT_OK"));
        onApplyStatus="ERROR";
        return;
      }
    mySNode->update( RENAME, 0, mySNode );
  }
}

void YACSGui_NodePage::setMode( const YACSGui_InputPanel::PageMode theMode )
{
  myMode = theMode;
}

void YACSGui_NodePage::setValueCellValidator( YACSGui_Table* theTable, int theRow )
{
  DEBTRACE("YACSGui_NodePage::setValueCellValidator " << theRow);
  QString aVT = theTable->item( theRow, 2 )->text();
  //DEBTRACE(aVT.latin1());
  if ( !aVT.compare(QString("double")) )
    theTable->setCellType( theRow, 3, YACSGui_Table::Double );
  else if ( !aVT.compare(QString("int")) )
    theTable->setCellType( theRow, 3, YACSGui_Table::Int );
  else if ( !aVT.compare(QString("bool")) )
  {
    theTable->setCellType( theRow, 3, YACSGui_Table::Combo );
    theTable->setParams( theRow, 3, QString("True;False") );
  }
  else // i.e. "string" or "Objref" or "Sequence" or "Array" or "Struct"
    theTable->setCellType( theRow, 3, YACSGui_Table::String );
}

bool YACSGui_NodePage::isSelectDataType()
{
  if ( mySelectDataTypeFor.myRow >= 0 ) return true;
  return false;
}

void YACSGui_NodePage::updateState()
{
}

void YACSGui_NodePage::updateBlocSize()
{
  if ( SubjectBloc* aSB = dynamic_cast<SubjectBloc*>(mySNode->getParent()) )
  {
    //aSB->update( EDIT, 0, aSB );

    Proc* aProc = dynamic_cast<Proc*>(getNode()->getRootNode());
    if ( !aProc ) return;

    YACSGui_Graph* aGraph = getInputPanel()->getModule()->getGraph( aProc );
    if ( !aGraph ) return;

    Bloc* aFather = dynamic_cast<Bloc*>(aSB->getNode());
    if ( !aFather ) return;

    bool isNeedToArrange = aGraph->isNeededToIncreaseBlocSize(aFather);
    while ( isNeedToArrange && aFather && !dynamic_cast<Proc*>(aFather) )
    {
      aGraph->arrangeNodesWithinBloc(aFather);
      aFather = dynamic_cast<Bloc*>(aFather->getFather());
      isNeedToArrange = aGraph->isNeededToIncreaseBlocSize(aFather);
    }
    aGraph->getCanvas()->update();
  }
}

YACSGui_NodePage::PortType YACSGui_NodePage::getDataPortType(YACS::ENGINE::Port* thePort) const
{
  DEBTRACE("YACSGui_NodePage::getDataPortType" );
  if ( dynamic_cast<DataPort*>(thePort) )
  {
    if ( dynamic_cast<InPort*>(thePort) )
    {
      if ( SeqAnyInputPort* aSeqAnyP = dynamic_cast<SeqAnyInputPort*>(thePort) )
      {
 	return SeqAnyInput;
      }
      else if ( AnyInputPort* anAnyP = dynamic_cast<AnyInputPort*>(thePort) )
      { 
	return AnyInput;
      }
      else if ( ConditionInputPort* aConditionP = dynamic_cast<ConditionInputPort*>(thePort) )
      { 
	return ConditionInput;
      }
      else if ( InputCorbaPort* aCorbaP = dynamic_cast<InputCorbaPort*>(thePort) )
      {
	return InputCorba;
      }
      else if ( InputPyPort* aPyP = dynamic_cast<InputPyPort*>(thePort) )
      { 
	return InputPy;
      }
      else if ( InputXmlPort* aXmlP = dynamic_cast<InputXmlPort*>(thePort) )
      {
	return InputXml;
      }
      //else if ( InputBasicStreamPort* aBasicStreamP = dynamic_cast<InputBasicStreamPort*>(thePort) )
      //{ // there is no possibility to identify BASIC data stream port in engine now => take default value from combo box
      //}
      else if ( InputCalStreamPort* aCalStreamP = dynamic_cast<InputCalStreamPort*>(thePort) )
      { 
	return InputCalStream;
      }
      //else if ( InputPalmStreamPort* aPalmStreamP = dynamic_cast<InputPalmStreamPort*>(thePort) )
      //{ // there is no possibility to identify PALM data stream port in engine now => take default value from combo box
      //}
      else if ( InputDataStreamPort* aDataStreamP = dynamic_cast<InputDataStreamPort*>(thePort) )
      { 
	return InputDataStream;
      }
      else if ( InputPort* anInputP = dynamic_cast<InputPort*>(thePort) )
      { // one of previous types is not detected
	return Input;
      }
    }
    else if ( dynamic_cast<OutPort*>(thePort) )
    {
      if ( AnyOutputPort* anAnyP = dynamic_cast<AnyOutputPort*>(thePort) )
      {
	return AnyOutput;
      }
      else if ( OutputCorbaPort* aCorbaP = dynamic_cast<OutputCorbaPort*>(thePort) )
      {
	return OutputCorba;
      }
      else if ( OutputPyPort* aPyP = dynamic_cast<OutputPyPort*>(thePort) )
      {
	return OutputPy;
      }
      else if ( OutputXmlPort* aXmlP  = dynamic_cast<OutputXmlPort*>(thePort))
      {
	return OutputXml;
      }
      //else if ( OutputBasicStreamPort* aBasicStreamP = dynamic_cast<OutputBasicStreamPort*>(thePort) )
      //{ // there is no possibility to identify BASIC data stream port in engine now => take default value from combo box
      //}
      else if ( OutputCalStreamPort* aCalStreamP = dynamic_cast<OutputCalStreamPort*>(thePort) )
      {
	return OutputCalStream;
      }
      //else if ( OutputPalmStreamPort* aPalmStreamP = dynamic_cast<OutputPalmStreamPort*>(thePort) )
      //{ // there is no possibility to identify PALM data stream port in engine now => take default value from combo box
      //}
      else if ( OutputDataStreamPort* aCalStreamP = dynamic_cast<OutputDataStreamPort*>(thePort) )
      {
	return OutputDataStream;
      }
      else if ( OutputPort* anOutputP = dynamic_cast<OutputPort*>(thePort) )
      { // one of previous types is not detected
	return Output;
      }
    }
  }
  return UnknownType;
}

//! Returns value of port as a string
/*!
 *
 */
QString YACSGui_NodePage::getPortValue(YACS::ENGINE::Port* thePort) const
{
  DEBTRACE("YACSGui_NodePage::getPortValue" );
  QString aValue;

  if(dynamic_cast<DataStreamPort*>(thePort))
    return "data stream";

  PortType aType = getDataPortType(thePort);
  if(DataPort* aDataPort = dynamic_cast<DataPort*>(thePort))
    {
      aValue=aDataPort->getAsString();
      if(aValue=="None") aValue="< ? >";
    }

  return aValue;
}

