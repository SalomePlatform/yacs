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

// Some useful functions. TODO: to be moved to Def.hxx file.

char * toString(CORBA::Any* theAny, QString& theRetStr)
{
  if ( !theAny ) theRetStr += QString("< ? >");
  else
  {
    CORBA::Any anAny = *theAny;
    if ( !anAny.value() ) theRetStr += QString("< ? >");
    else
    {
      ostringstream astr;
      const char * retstr;
      int startstr = 0;
      switch ( anAny.type()->kind() )
      {
	case CORBA::tk_string: {
	  anAny >>= retstr;
	  theRetStr += QString(retstr);
	  break;
	}
	case CORBA::tk_long: {
	  CORBA::Long l;
	  anAny >>= l;
	  astr << l << ends;
	  theRetStr += QString(astr.str());
	  break;
	}
	case CORBA::tk_double: {
	  double d;
	  anAny >>= d;
	  astr << d << ends;
	  //astr << setw(25) << setprecision(18) << d << ends;
	  QString aRetStr = QString(astr.str());
	  int i = 0;
	  while ( i < (int ) theRetStr.length() && theRetStr.at(i++) == ' ' ) {
	    startstr = i;
	  }
	  theRetStr += aRetStr.mid(startstr,aRetStr.length());
	  break;
	}
        case CORBA::tk_sequence: {
	  theRetStr += QString("[");

	  CORBA::Long aSeqLength = 0;
	  *(anAny.type()->parameter(1)) >>= aSeqLength;

	  if ( aSeqLength == 0 )
	  {
	    theRetStr += QString("   ]");
	    break;
	  }
	  
	  // TO DO : implement recursion for the sequence type
	  /*CORBA::TypeCode* aType;
	  *(anAny.type()->parameter(0)) >>= aType;
	  switch ( aType->kind() )
	  {
	    case CORBA::tk_string: {
	      printf("StringElem\n");
	      CORBA::StringSeq* aStringSeq;
	      anAny >>= aStringSeq;
	      for (int i=0; i < aSeqLength; i++)
	      {
		CORBA::Any anArg;
		anArg <<= aStringSeq[i];
		toString( &anArg, theRetStr );
		if ( i < aSeqLength-1 ) theRetStr += QString(",");
	      }
	      break;
	    }
	    case CORBA::tk_double: {
	      printf("DoubleElem\n");
	      CORBA::DoubleSeq* aDoubleSeq;
	      anAny >>= aDoubleSeq;
	      for (int i=0; i < aSeqLength; i++)
	      {
		CORBA::Any anArg;
		anArg <<= aDoubleSeq[i];
		toString( &anArg, theRetStr );
		if ( i < aSeqLength-1 ) theRetStr += QString(",");
	      }
	      break;
	    }
  	    case CORBA::tk_sequence: {
	      printf("SequenceElem\n");
	      CORBA::Any* aSequenceSeq;
	      anAny >>= aSequenceSeq;
	      for (int i=0; i < aSeqLength; i++)
	      {
		CORBA::Any anArg;
		anArg <<= aSequenceSeq[i];
		toString( &anArg, theRetStr );
		if ( i < aSeqLength-1 ) theRetStr += QString(",");
	      }
	      break;
	    }
	    default: {
	      printf("DefaultElem\n");
	      theRetStr += QString("< ? >");
	      break;
	    }
	  }*/
	  theRetStr += QString("]");
	  break;
	}
	case CORBA::tk_objref: {
	  /*CORBA::Object_ptr obj;
	  try {
	    anAny >>= (CORBA::Any::to_object ) obj;
	    theRetStr += QString( _Orb->object_to_string( obj ) );
	  }
	  catch ( ... ) {
	    theRetStr += QString("object_to_string catched ");
	  }*/
	  theRetStr += QString("Objref");
	  break;
	}
	default: {
	  theRetStr += QString("< ? >");
	  break;
	}
      }
    }
  }
}

void toString(PyObject* theObject, QString& theRetStr)
{
  if ( !theObject ) theRetStr += QString("< ? >");

  ostringstream aStr;
  if ( PyString_CheckExact(theObject) )
    theRetStr += QString( PyString_AsString(theObject) );
  else if ( PyLong_CheckExact(theObject) )
  {
    long aVal = PyLong_AsLong(theObject);
    aStr << aVal << ends;
    theRetStr += QString( aStr.str() );
  }
  else if ( PyInt_CheckExact(theObject) )
  {
    long aVal = PyInt_AsLong(theObject);
    aStr << aVal << ends;
    theRetStr += QString( aStr.str() );
  }
  else if ( PyBool_Check(theObject) )
    theRetStr += QString( (theObject == Py_True) ? "true" : "false" );
  else if ( PyFloat_CheckExact(theObject) )
  {
    double aVal = PyFloat_AsDouble(theObject);
    aStr << aVal << ends;
    theRetStr += QString( aStr.str() );
  }
  else if ( PyList_CheckExact(theObject) )
  {
    theRetStr += QString("[");
    for (int i=0; i < PyList_Size(theObject); i++)
    {
      toString( PyList_GetItem(theObject, i), theRetStr );
      if ( i < PyList_Size(theObject)-1 ) theRetStr += QString(",");
    }
    theRetStr += QString("]");
  }
  //else if ( ... ) // objref case
  else
    theRetStr += QString("< ? >");
}

void toString(YACS::ENGINE::Any* theAny, QString& theValue)
{
  if ( !theAny ) theValue += QString("< ? >");
  else if ( theAny->getType() )
  {
    DynType aKind = theAny->getType()->kind();
    switch (aKind)
      {
      case Double:
	theValue += QString::number(theAny->getDoubleValue());
	break;
      case Int:
	theValue += QString::number(theAny->getIntValue());
	break;
      case String:
	theValue += QString(theAny->getStringValue());
	break;
      case Bool:
	theValue += QString(theAny->getBoolValue()?"true":"false");
	break;
      case Objref:
	theValue += QString("Objref"); /// ?
	break;
      case Sequence: {
	SequenceAny* aSeqAny = dynamic_cast<SequenceAny*>( theAny );
	if ( aSeqAny )
	{
	  theValue += QString("[");
	  for (int i=0; i < aSeqAny->size(); i++)
	  {
	    toString( (*aSeqAny)[i], theValue );
	    if ( i < aSeqAny->size()-1 ) theValue += QString(",");
	  }
	  theValue += QString("]");
	}
	break;
      }
      case NONE:
      default:
	theValue += QString("");
	break;
      }
  }
}

/*
  Class : YACSGui_NodePage
  Description :  Basic page for node properties
*/

YACSGui_NodePage::YACSGui_NodePage()
  : GuiObserver(),
    mySNode( 0 ),
    myType( Unknown ) //?
{
  mySelectDataTypeFor.myRow = -1;
}

YACSGui_NodePage::~YACSGui_NodePage()
{
  if ( mySNode ) mySNode->detach(this);
}

void YACSGui_NodePage::select( bool isSelected )
{
  printf(">> YACSGui_NodePage::select\n");
}

void YACSGui_NodePage::update( YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son)
{
  printf(">> YACSGui_NodePage::update\n");
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
      if ( !type && !son && mySNode )
      {
	mySNode->detach(this);
	mySNode = 0;
      }
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
  if ( !theSNode )
    return;

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

  if ( (isNeedToUpdatePortValues || myType == SALOMEService) && getNode() )
  {
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
      map<string,string> anInPortName2Value;
      list<InPort*> IPs = getNode()->getSetOfInPort();
      list<InPort*>::iterator itIP = IPs.begin();
      //printf("Input ports\n");
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
      //printf("--------\n\n");
      //printf("Notify\n");
      notifyInPortValues( anInPortName2Value );
            
      map<string,string> anOutPortName2Value;
      list<OutPort*> OPs = getNode()->getSetOfOutPort();
      list<OutPort*>::iterator itOP = OPs.begin();
      //printf("Output ports\n");
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
      //printf("--------\n\n");
      //printf("Notify\n");
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
    getNode()->setName( theName.latin1() );
    mySNode->update( RENAME, 0, mySNode );
  }
}

void YACSGui_NodePage::setMode( const YACSGui_InputPanel::PageMode theMode )
{
  myMode = theMode;
}

void YACSGui_NodePage::setValueCellValidator( YACSGui_Table* theTable, int theRow )
{
  QString aVT = theTable->item( theRow, 2 )->text();
  if ( !aVT.compare(QString("Double")) )
    theTable->setCellType( theRow, 3, YACSGui_Table::Double );
  else if ( !aVT.compare(QString("Int")) )
    theTable->setCellType( theRow, 3, YACSGui_Table::Int );
  else if ( !aVT.compare(QString("Bool")) )
  {
    theTable->setCellType( theRow, 3, YACSGui_Table::Combo );
    theTable->setParams( theRow, 3, QString("True;False") );
  }
  else // i.e. "String" or "Objref" or "Sequence" or "Array" or "Struct"
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

QString YACSGui_NodePage::getPortValue(YACS::ENGINE::Port* thePort) const
{
  QString aValue;

  PortType aType = getDataPortType(thePort);

  if( aType == Input )
  {
  }
  else if( aType == SeqAnyInput )
  {
    SeqAnyInputPort* aSeqAnyP = dynamic_cast<SeqAnyInputPort*>(thePort);
    Any* anAny = aSeqAnyP->getValue();
    if ( !anAny ) aValue = QString("[ ? ]");
    else toString(anAny, aValue);
  }
  else if( aType == AnyInput )
  {
    AnyInputPort* anAnyP = dynamic_cast<AnyInputPort*>(thePort);
    toString(anAnyP->getValue(), aValue);
  }
  else if( aType == ConditionInput )
  {
    ConditionInputPort* aConditionP = dynamic_cast<ConditionInputPort*>(thePort);
    aValue = QString( aConditionP->getValue() ? "True" : "False" );
  }
  else if( aType == InputCorba )
  {
    InputCorbaPort* aCorbaP = dynamic_cast<InputCorbaPort*>(thePort);
    toString( aCorbaP->getAny(), aValue );
  }
  else if( aType == InputPy )
  {
    InputPyPort* aPyP = dynamic_cast<InputPyPort*>(thePort);
    toString( aPyP->getPyObj(), aValue );
  }
  else if( aType == InputXml )
  {
    aValue = QString("xml");
  }
  else if( aType == InputBasicStream )
  {
  }
  else if( aType == InputCalStream )
  {
    aValue = QString("data stream");
  }
  else if( aType == InputPalmStream )
  {
  }
  else if( aType == InputDataStream )
  {
    aValue = QString("data stream");
  }
  else if( aType == Output )
  {
  }
  else if( aType == AnyOutput )
  {
    AnyOutputPort* anAnyP = dynamic_cast<AnyOutputPort*>(thePort);
    toString(anAnyP->getValue(), aValue);
  }
  else if( aType == OutputCorba )
  {
    OutputCorbaPort* aCorbaP = dynamic_cast<OutputCorbaPort*>(thePort);
    toString( aCorbaP->getAny(), aValue );
  }
  else if( aType == OutputPy )
  {
    OutputPyPort* aPyP = dynamic_cast<OutputPyPort*>(thePort);
    toString( aPyP->get(), aValue );
  }
  else if( aType == OutputXml )
  {
    aValue = QString("xml");
  }
  else if( aType == OutputBasicStream )
  {
  }
  else if( aType == OutputCalStream )
  {
    aValue = QString("data stream");
  }
  else if( aType == OutputPalmStream )
  {
  }
  else if( aType == OutputDataStream )
  {
    aValue = QString("data stream");
  }

  return aValue;
}

