// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include <Python.h>

#include "YACSPrs_ElementaryNode.h"

#include "YACSPrs_Link.h"
#include "YACSPrs_Def.h"

#include "SUIT_ResourceMgr.h"

#include <qpainter.h>
//#include <qvaluelist.h> // for states animation

#include <Bloc.hxx>
#include <InPort.hxx>
#include <OutPort.hxx>
#include <InputPort.hxx>
#include <OutputPort.hxx>
#include <TypeCode.hxx>
#include <AnyInputPort.hxx>
#include <ConditionInputPort.hxx>
#include <ForEachLoop.hxx> // for SeqAnyInputPort
#include <CORBAPorts.hxx>
#include <PythonPorts.hxx>
#include <XMLPorts.hxx>
#include <CalStreamPort.hxx>
#include <commandsProc.hxx>

using namespace YACS::ENGINE;
using namespace YACS::HMI;

using namespace std;
#include <sstream>
#include <string>

void drawText(QPainter& thePainter, const QString& theText, 
	      const QRect& theRect, int theHAlign = Qt::AlignAuto)
{
  int flags = theHAlign | Qt::AlignVCenter;
  QRect r(theRect.x() + TEXT_MARGIN, theRect.y(), 
	  theRect.width() - 2*TEXT_MARGIN, theRect.height());

  QWMatrix aMat = thePainter.worldMatrix();
  if (aMat.m11() != 1.0) { 
    // for scaled picture only
    QRect r1 = aMat.mapRect(r);
    QFont saved = thePainter.font();
    QFont f(saved);
    if (f.pointSize() == -1) {
      f.setPixelSize((int)(f.pixelSize()*aMat.m11()));
    }
    else {
      f.setPointSize((int)(f.pointSize()*aMat.m11()));
    }
    thePainter.save();
    QWMatrix m;
    thePainter.setWorldMatrix(m);
    thePainter.setFont(f);
    thePainter.drawText(r1, flags, theText);
    thePainter.setFont(saved);
    thePainter.restore();
  }
  else {
    thePainter.drawText(r, flags, theText);
  }
}

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

void toString(Any* theAny, QString& theValue)
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

/*!
 * =========================== YACSPrs_ElementaryNode ===========================
 !*/

YACSPrs_ElementaryNode::YACSPrs_ElementaryNode(SUIT_ResourceMgr* theMgr, QCanvas* theCanvas, YACS::HMI::SubjectNode* theSNode):
  QxGraph_ActiveItem(),
  QCanvasPolygonalItem(theCanvas),
  GuiObserver(),
  myMgr(theMgr),
  mySEngine(theSNode),
  myLabelLink(0),
  myHilightedPort(0),
  mySelectedPort(0)
{
  printf("YACSPrs_ElementaryNode::YACSPrs_ElementaryNode\n");
  if ( mySEngine ) mySEngine->attach(this);

  myWidth = NODE_WIDTH;
  if (2*(PORT_WIDTH+PORT_MARGIN) > myWidth)
    myWidth = 2*(PORT_WIDTH+PORT_MARGIN);

  myTitleHeight = TITLE_HEIGHT;
  myStatusHeight = TITLE_HEIGHT;
  myTimeHeight = TITLE_HEIGHT;
  myPortHeight = 2*PORT_MARGIN;
  myGateHeight = PORT_HEIGHT + 2*PORT_MARGIN;

  myStoreColor = myColor = myMgr->colorValue("QxGraph", "NodeBody", NODE_COLOR);
  myStoreSubColor = mySubColor = myMgr->colorValue( "QxGraph", "Title", TITLE_COLOR );

  QRect aTRect = getTitleRect();
  myXRnd = 50*aTRect.height()/aTRect.width();
  myYRnd = 50;

  myMoving = false;
  mySelfMoving = true;
  mySelected = false;

  myIsCheckAreaNeeded = true;

  myIsInBloc = false;
  myArea = QRect();

  myTime = QString("00:00:00");
  myTimeIteration = 0;
  myPercentage = -1.;

  myStarted = false;
  myFinished = false;

  /*QValueList<QPixmap> aPMList;
  aPMList.append(myStatePixmap);
  static QCanvasPixmapArray aPMArray(aPMList);
  mySprite = new QCanvasSprite( &aPMArray, theCanvas );
  //mySprite->setAnimated(true);
  mySprite->setZ(1);
  mySprite->show();*/

  myPointMaster = new YACSPrs_Hook(myMgr, canvas(), this, false, false, true);
  
  setZ(2);
  //updatePorts(); // create presentations for all ports of node, 
                 // must be called only in constructor of derived classes (because this is the base prs class)
                 // or createPrs() on demand and not in constructor!!!
  //moveBy(2*HOOKPOINT_SIZE,0); // move node only is subclasses, because moveBy() function calls
                                // updatePorts() method corresponding to this subclass
}

YACSPrs_ElementaryNode::~YACSPrs_ElementaryNode() 
{
  myPortList.setAutoDelete(true);
  myPortList.clear();

  if (myPointMaster) delete myPointMaster;

  mySEngine->detach(this);
  //delete mySEngine;
}

void YACSPrs_ElementaryNode::select( bool isSelected )
{
  printf(">> YACSPrs_ElementaryNode::select\n");
}

void YACSPrs_ElementaryNode::update( YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son)
{
  printf(">> YACSPrs_ElementaryNode::update\n");
  switch (event)
  {
  case RENAME:
    if ( canvas() )
    { 
      canvas()->setChanged(getTitleRect());
      canvas()->update();
    }
    break;
  case EDIT:
    switch (type)
    {
    case INPUTPORT:
    case OUTPUTPORT:
    case INPUTDATASTREAMPORT:
    case OUTPUTDATASTREAMPORT:
      {
	if ( canvas() )
	  if ( SubjectDataPort* aDP = dynamic_cast<SubjectDataPort*>(son) )
	    if ( YACSPrs_InOutPort* aPPrs = getPortPrs(aDP->getPort()) )
	    {
	      printf(">> In prs : EDIT\n");
	      aPPrs->update();
	      canvas()->update();
	    }
      }
      break;
    default:
      {
	int anOldHeight = maxHeight();
	updatePorts(true);
	
	bool needToCanvasUpdate = true;
	if ( anOldHeight != maxHeight() )
	{
	  //emit portsChanged();
	  
	  SubjectBloc* aParentSubj = dynamic_cast<SubjectBloc*>( mySEngine->getParent() );
	  if ( aParentSubj && !dynamic_cast<SubjectProc*>( aParentSubj ) )
	  {
	    aParentSubj->update( event, BLOC, mySEngine );
	    needToCanvasUpdate = false;
	  }
	}
	
	if ( needToCanvasUpdate && canvas() )
	{ 
	  QRect aRect((int)x(),(int)y(),maxWidth(),maxHeight());
	  canvas()->setChanged(aRect);
	  canvas()->update();
	}
      }
      break;
    }
    break;
  case ADD:
    switch (type)
    {
    case INPUTPORT:
    case OUTPUTPORT:
    case INPUTDATASTREAMPORT:
    case OUTPUTDATASTREAMPORT:
      {
	if ( SubjectDataPort* aDP = dynamic_cast<SubjectDataPort*>(son) )
	{
	  printf(">> In prs : ADD\n");
	  addPortPrs(aDP->getPort());
	}
      }
      break;
    default:
      break;
    }
    break;
  case REMOVE:
    switch (type)
    {
    case INPUTPORT:
    case OUTPUTPORT:
    case INPUTDATASTREAMPORT:
    case OUTPUTDATASTREAMPORT:
      {
	if ( SubjectDataPort* aDP = dynamic_cast<SubjectDataPort*>(son) )
	{
	  printf(">> In prs : REMOVE\n");
	  removePortPrs(aDP->getPort());
	}
      }
      break;
    default:
      break;
    }
    break;
  case UPDATE:
    {
      if ( canvas() )
      {
	printf(">> In prs : UPDATE\n");
	updatePorts();
	canvas()->update();
      }
    }
    break;
  case UP:
    switch (type)
    {
    case INPUTPORT:
    case OUTPUTPORT:
    case INPUTDATASTREAMPORT:
    case OUTPUTDATASTREAMPORT: 
      {
	if ( SubjectDataPort* aDP = dynamic_cast<SubjectDataPort*>(son) )
	{
	  printf(">> In prs : UP\n");
	  moveUp(aDP->getPort());
	}
      }
      break;
    default:
      break;
    }
    break;
  case DOWN:
    switch (type)
    {
    case INPUTPORT:
    case OUTPUTPORT:
    case INPUTDATASTREAMPORT:
    case OUTPUTDATASTREAMPORT: 
      {
	if ( SubjectDataPort* aDP = dynamic_cast<SubjectDataPort*>(son) )
	{
	  printf(">> In prs : DOWN\n");
	  moveDown(aDP->getPort());
	}
      }
      break;
    default:
      break;
    }
    break;
  default:
    GuiObserver::update(event, type, son);
  }
}

YACS::ENGINE::Node* YACSPrs_ElementaryNode::getEngine() const
{
  return ( mySEngine ? mySEngine->getNode() : 0 );
}

YACSPrs_InOutPort* YACSPrs_ElementaryNode::getPortPrs(YACS::ENGINE::Port* thePort)
{
  YACSPrs_InOutPort* aRetPort = 0;
  for (YACSPrs_Port* aPort = myPortList.first(); aPort; aPort = myPortList.next())
  {
    YACSPrs_InOutPort* anIOPort = dynamic_cast<YACSPrs_InOutPort*>( aPort );
    if ( anIOPort && anIOPort->getEngine() == thePort )
      aRetPort = anIOPort;
  }
  return aRetPort;
}

void YACSPrs_ElementaryNode::sortPorts()
{
  ElementaryNode* aNode = dynamic_cast<ElementaryNode*>(getEngine());
  if ( !aNode ) return;

  QPtrList<YACSPrs_Port> aSortedPortList;

  list<InPort*> anInPortsEngine = aNode->getSetOfInPort();
  list<InPort*>::iterator itIn = anInPortsEngine.begin();
  for ( ; itIn != anInPortsEngine.end(); itIn++ )
    aSortedPortList.append( getPortPrs(*itIn) );

  list<OutPort*> anOutPortsEngine = aNode->getSetOfOutPort();
  list<OutPort*>::iterator itOut = anOutPortsEngine.begin();
  for ( ; itOut != anOutPortsEngine.end(); itOut++ )
    aSortedPortList.append( getPortPrs(*itOut) );

  aSortedPortList.append( getPortPrs(aNode->getInGate()) );
  aSortedPortList.append( getPortPrs(aNode->getOutGate()) );

  myPortList = aSortedPortList;
}

void YACSPrs_ElementaryNode::moveUp(YACS::ENGINE::Port* thePort)
{
  if ( !thePort ) return;

  YACSPrs_InOutPort* aPortToMoveUp = getPortPrs(thePort);
  int anId = myPortList.find(aPortToMoveUp);
  if ( anId > -1 )
  {
    myPortList.take(anId);
    anId--;
    myPortList.insert(anId,aPortToMoveUp);
  }
}

void YACSPrs_ElementaryNode::moveDown(YACS::ENGINE::Port* thePort)
{
  if ( !thePort ) return;

  YACSPrs_InOutPort* aPortToMoveDown = getPortPrs(thePort);
  int anId = myPortList.find(aPortToMoveDown);
  if ( anId > -1 )
  {
    myPortList.take(anId);
    anId++;
    myPortList.insert(anId,aPortToMoveDown);
  }
}

void YACSPrs_ElementaryNode::addPortPrs(YACS::ENGINE::Port* thePort)
{
  if ( !thePort ) return;

  ElementaryNode* aNode = dynamic_cast<ElementaryNode*>(getEngine());
  if ( !aNode ) return;

  int anIPNum = aNode->getNumberOfInputPorts();
  int anOPNum = aNode->getNumberOfOutputPorts();

  bool isInput = true;
  if ( dynamic_cast<OutPort*>(thePort) )
    isInput = false;

  bool aDisp = isVisible();
  if (aDisp) hide();

  QRect r = getBodyRect();
  int aPRectWidth = (int)(r.width()/2) - 2*PORT_MARGIN;
  if ( aPRectWidth < PORT_WIDTH ) aPRectWidth = PORT_WIDTH;
  
  int ix = r.x() + PORT_MARGIN + 1;
  int iy;
  if ( anIPNum > 1 )
    iy = r.y() + PORT_MARGIN + (PORT_HEIGHT+PORT_SPACE)*(anIPNum-1);
  else
    iy = r.y() + PORT_MARGIN;

  int ox = ix + aPRectWidth + 2*PORT_MARGIN;
  int oy;
  if ( anOPNum > 1 )
    oy = r.y() + PORT_MARGIN + (PORT_HEIGHT+PORT_SPACE)*(anOPNum-1);
  else
    oy = r.y() + PORT_MARGIN;

  YACSPrs_InOutPort* aPortPrs = new YACSPrs_InOutPort(myMgr, canvas(), thePort, this);
  aPortPrs->setPortRect(QRect( (isInput ? ix : ox), 
			       (isInput ? iy : oy),
			       aPRectWidth,
			       PORT_HEIGHT )); // commented
  aPortPrs->setColor(nodeSubColor());
  aPortPrs->setStoreColor(nodeSubColor());
  
  YACSPrs_InOutPort* aPort;
  for (aPort = (YACSPrs_InOutPort*)(myPortList.first()); 
       aPort;
       aPort = (YACSPrs_InOutPort*)(myPortList.next()))
  {
    if ( aPortPrs->isInput() && !aPort->isInput() )
    { 
      myPortList.insert( myPortList.find(aPort), aPortPrs );
      break;
    }
    if ( !aPortPrs->isInput() && aPort->isGate() )
    { 
      myPortList.insert( myPortList.find(aPort), aPortPrs );
      break;
    }
  }

  if ( (isInput && anIPNum > anOPNum) || (!isInput && anIPNum < anOPNum) )
    myPortHeight += PORT_HEIGHT+PORT_SPACE;
  
  updateGates(false);
  //updatePorts();

  if (myPointMaster)
  {
    QPoint aPnt = getConnectionMasterPoint();
    myPointMaster->setCoords(aPnt.x(), aPnt.y());
  }

  if (aDisp) show();

  if ( canvas() )
  { 
    canvas()->setChanged(getRect());
    canvas()->update();
  }
}

void YACSPrs_ElementaryNode::removePortPrs(YACS::ENGINE::Port* thePort)
{
  if ( YACSPrs_InOutPort* aPort = getPortPrs(thePort) )
  {
    ElementaryNode* aNode = dynamic_cast<ElementaryNode*>(getEngine());
    if ( !aNode ) return;
    
    int anIPNum = aNode->getNumberOfInputPorts();
    int anOPNum = aNode->getNumberOfOutputPorts();

    bool aDisp = isVisible();
    if (aDisp) hide();

    bool isInput = true;
    if ( dynamic_cast<OutPort*>(thePort) )
      isInput = false;

    if ( ElementaryNode* aNode = dynamic_cast<ElementaryNode*>(getEngine()) )
      if ( (isInput && anIPNum > anOPNum) || (!isInput && anIPNum < anOPNum) )
	myPortHeight -= PORT_HEIGHT+PORT_SPACE;

    myPortList.setAutoDelete(true);
    myPortList.remove(aPort);
    
    //updateGates(false);
    updatePorts();

    if (myPointMaster)
    {
      QPoint aPnt = getConnectionMasterPoint();
      myPointMaster->setCoords(aPnt.x(), aPnt.y());
    }

    if (aDisp) show();

    if ( canvas() )
    { 
      canvas()->setChanged(getRect());
      canvas()->update();
    }
  }
}

void YACSPrs_ElementaryNode::setCanvas(QCanvas* theCanvas)
{
  QCanvasItem::setCanvas(theCanvas);
  
  // set canvas to all ports
  YACSPrs_Port* aPort;
  for (aPort = myPortList.first(); aPort; aPort = myPortList.next())
    aPort->setCanvas(theCanvas);
  
  // set canvas to master point
  if (myPointMaster)
  {
    myPointMaster->setCanvas(theCanvas);
    QPoint aPnt = getConnectionMasterPoint();
    myPointMaster->setCoords(aPnt.x(), aPnt.y());
  }

  // set canvas to in/out points
  //myPointIn->setCanvas(theCanvas);
  //myPointOut->setCanvas(theCanvas);
}

void YACSPrs_ElementaryNode::removeLabelLink()
{ 
  if ( myLabelLink ) myLabelLink = 0;
}

void YACSPrs_ElementaryNode::updateLabelLink()
{
  if ( myLabelLink ) myLabelLink->moveByNode(this);
}

void YACSPrs_ElementaryNode::beforeMoving()
{ 
  myZ = z();
  setMoving(true);
  setZ(myZ+100);
}

void YACSPrs_ElementaryNode::afterMoving()
{
  setMoving(false);
  setZFromLinks(myZ);
  canvas()->update();
}

void YACSPrs_ElementaryNode::hilight(const QPoint& theMousePos, const bool toHilight)
{
  // process the hilighting for node and ports
  if (toHilight) 
    {
      if (getBodyRect().contains(theMousePos, true) || getGateRect().contains(theMousePos, true)) {
	// process ports
	QPtrList<YACSPrs_Port> aPortList = getPortList();
	for (YACSPrs_Port* aPort = aPortList.first(); aPort; aPort = aPortList.next()) {
	  QRect aPortRect = aPort->getPortRect();
	  aPortRect.setTop(aPortRect.top() - PORT_MARGIN);
	  aPortRect.setBottom(aPortRect.bottom() + PORT_MARGIN);
	  if (aPortRect.contains(theMousePos, true)) {
	    if ( aPort == myHilightedPort ) return;
	    
	    if ( myHilightedPort ) 
	      myHilightedPort->setColor(myHilightedPort->storeColor(), false, true);
	    else
	      setNodeColor( storeColor() );
	    
	    if ( aPort->isSelected()) {
	      myHilightedPort = 0;
	      return;
	    }
  
	    QColor aColor = myMgr->colorValue("YACSGui", "port_hilight_color", PORT_HILIGHT_COLOR);
	    aPort->setColor(aColor, false, true);
	    myHilightedPort = aPort;
	    return;
	  }
	}
      }
      
      if ( myHilightedPort ) {
	QColor aColor;
	if (isSelected()) {
	  aColor = nodeSubColor();
	  if (dynamic_cast<YACSPrs_LabelPort*>(myHilightedPort))
	    aColor = aColor.dark(140);
	}
	else
	  aColor = myHilightedPort->storeColor();
	
	myHilightedPort->setColor(aColor, false, true);
	myHilightedPort = 0;
      }
      
      // hilight node
      if (!isSelected()) {
	QColor aColor = myMgr->colorValue("YACSGui", "node_hilight_color", NODE_HILIGHT_COLOR);
	if (aColor !=  nodeColor())
	  setNodeColor(aColor);
      }
    }
  else
    {
      if ( myHilightedPort ) {
	myHilightedPort->setColor(myHilightedPort->storeColor(), false, true);
	myHilightedPort = 0;
      }
      else
	setNodeColor( storeColor() );
    }
}

void YACSPrs_ElementaryNode::select(const QPoint& theMousePos, const bool toSelect)
{
  if ( toSelect ) 
  {
    // unhilight the item under the mouse cursor
    hilight(theMousePos, false);

    bool isOnPort = false;
    if (getBodyRect().contains(theMousePos, true) || getGateRect().contains(theMousePos, true))
    {
      // process ports
      QPtrList<YACSPrs_Port> aPortList = getPortList();
      for (YACSPrs_Port* aPort = aPortList.first(); aPort; aPort = aPortList.next()) {
	if (aPort->getPortRect().contains(theMousePos, true)) {
	  isOnPort = true;
	  if ( aPort != mySelectedPort )
	  {
	    if ( mySelectedPort )
	    {
	      mySelectedPort->setSelected(false);
	      mySelectedPort->setColor(mySelectedPort->storeColor(), false, true, true);
	    }
	    else
	    {
	      setSelected(false);
	      setNodeSubColor( myStoreSubColor, true );
	    }
	    aPort->setSelected(true);
	    aPort->setColor(aPort->Color().dark(130), false, true, true);
	    mySelectedPort = aPort;
	  }
	  break;
	}
      }
    }
    
    if ( !isOnPort )
    {
      if ( mySelectedPort )
      {
	mySelectedPort->setSelected(false);
	mySelectedPort = 0;
      }
      
      if ( myStoreSubColor.dark(130) != mySubColor )
      {
	myStoreSubColor = mySubColor;

	setSelected(true);
	setNodeSubColor( nodeSubColor().dark(130), true );
      }
    }
  }
  else
  {
    if ( mySelectedPort ) {
      mySelectedPort->setSelected(false);
      mySelectedPort->setColor(mySelectedPort->storeColor(), false, true, true);
      mySelectedPort = 0;
    }
    else {
      setSelected(false);
      setNodeSubColor( myStoreSubColor, true );
    }
  }
}

void YACSPrs_ElementaryNode::showPopup(QWidget* theParent, QMouseEvent* theEvent, const QPoint& theMousePos)
{
}

QString YACSPrs_ElementaryNode::getToolTipText(const QPoint& theMousePos, QRect& theRect) const
{
  QString aText = QString("");
  
  if (getBodyRect().contains(theMousePos, true) || getGateRect().contains(theMousePos, true))
    {
      // process ports
      QPtrList<YACSPrs_Port> aPortList = getPortList();
      for (YACSPrs_Port* aPort = aPortList.first(); aPort; aPort = aPortList.next()) {
	if (aPort->getPortRect().contains(theMousePos, true)) {
	  YACSPrs_InOutPort* anIOPort = dynamic_cast<YACSPrs_InOutPort*>( aPort );
	  if ( anIOPort) {
	    aText += anIOPort->isInput()?QString("Input"):QString("Output");
	    if (!anIOPort->isGate()) {
	      if ( dynamic_cast<InputDataStreamPort*>(anIOPort->getEngine())
		   ||
		   dynamic_cast<OutputDataStreamPort*>(anIOPort->getEngine()) )
		aText += QString(" data stream");
	      aText += QString(" port \"") + anIOPort->getName() + QString("\", ");
	      aText += anIOPort->getType(true) + QString(", ");
	      aText += QString("value = ") + anIOPort->getValue();
	    }
	    else
	      aText += QString(" gate port");
	    
	    theRect = anIOPort->getPortRect();
	    return aText;
	  }
	  else if (YACSPrs_LabelPort* aLabelPort = dynamic_cast<YACSPrs_LabelPort*>( aPort )) {
	    aText += QString("Label port");
	    if ( YACS::ENGINE::Node* aSlaveNode = aLabelPort->getSlaveNode())
	      aText += QString(", connected to node \"") + aSlaveNode->getName() + QString("\"");
	    theRect = aLabelPort->getPortRect();
	    return aText;
	  }
	}  
      }
    }
  
  // info about node
  //if (getGateRect().contains(theMousePos, true))
  //  theRect = getGateRect();
  else if (getTitleRect().contains(theMousePos, true))
    theRect = getTitleRect();
  else if (getPixmapRect().contains(theMousePos, true))
    theRect = getPixmapRect();
  
  aText += QString("Name: %1\n").arg(getEngine()->getName());
  //aText += QString("Type: %1\n").arg(getEngine()->getType());
  return aText;
}

int YACSPrs_ElementaryNode::rtti() const
{
  return 0;//YACSPrs_Canvas::Rtti_ElementaryNode;
}

void YACSPrs_ElementaryNode::setVisible(bool b)
{
  QCanvasPolygonalItem::setVisible(b);

  // set visibility to all ports
  YACSPrs_Port* aPort;
  for (aPort = myPortList.first(); aPort; aPort = myPortList.next())
    if (aPort->isVisible()) aPort->setVisible(b);
  
  // set visibility to master point
  myPointMaster->setVisible(b);
  updateLabelLink();

  // set visibility to in/out points
  //myPointIn->setVisible(b);
  //myPointOut->setVisible(b);
}

QPointArray YACSPrs_ElementaryNode::areaPoints() const
{
  int w = width();
  int h = height()+1; // add pen width

  return constructAreaPoints(w,h);
}

QPointArray YACSPrs_ElementaryNode::maxAreaPoints() const
{
  int w = width();
  int h = maxHeight()+1; // add pen width

  return constructAreaPoints(w,h);
}

QPointArray YACSPrs_ElementaryNode::constructAreaPoints(int theW, int theH) const
{
  QPointArray aPnts(4);
  aPnts[0] = QPoint((int)x(), (int)y()) + QPoint(-NODEBOUNDARY_MARGIN,-NODEBOUNDARY_MARGIN);
  aPnts[1] = aPnts[0] + QPoint(theW, 0) + QPoint(NODEBOUNDARY_MARGIN,0);
  aPnts[2] = aPnts[1] + QPoint(0, theH) + QPoint(0,NODEBOUNDARY_MARGIN);
  aPnts[3] = aPnts[0] + QPoint(0, theH) + QPoint(0,NODEBOUNDARY_MARGIN);
  return aPnts;
}

void YACSPrs_ElementaryNode::moveBy(double dx, double dy) 
{
  // for constraint nodes' moving inside the Bloc-->
  if ( isCheckAreaNeeded() && !checkArea(dx,dy) ) return;
  // <--

  int aX = (int) (x()+dx);
  int aY = (int) (y()+dy);
  int xx = aX - (int)x();
  int yy = aY - (int)y();

  if ( canvas() )
  {  
    int w = aX + width() + GRAPH_MARGIN;
    int h = aY + height() + GRAPH_MARGIN;
    if (canvas()->width() > w) w = canvas()->width();
    if (canvas()->height() > h) h = canvas()->height();
    if (canvas()->width() < w || canvas()->height() < h)
      canvas()->resize(w, h);
  }

  // save new coordinates only if node is really moving...
  //if (isMoving()) {
  //  myNode->getEngine()->Coords(aX, aY);
  //}

  QCanvasPolygonalItem::moveBy(dx, dy);

  // update port's rectangle
  updatePorts();
  //YACSPrs_Port* aPort;
  //for (aPort = myPortList.first(); aPort; aPort = myPortList.next())
  //  aPort->moveBy(xx, yy);

  myPointMaster->moveBy(dx, dy);
  if ( myLabelLink ) myLabelLink->moveByNode(this, (int)dx, (int)dy);

  //if (!myCellPrs) {
  //  myPointIn->moveBy(dx, dy);
  //  myPointOut->moveBy(dx, dy);
  //}

  if ( isSelected() && canvas() && isMoving() )
  {        /* update changed areas on canvas only if a node moves itself ( not with
	      its Bloc node as a content ), in this case isMoving() returns true */
    QRect aRect = boundingRect();
    QPoint aShift(20,20);
    aRect.setTopLeft(aRect.topLeft()-aShift);
    aRect.setBottomRight(aRect.bottomRight()+aShift);
    canvas()->setChanged(aRect);
    canvas()->update();
  }
}

void YACSPrs_ElementaryNode::setZ(double z)
{
  QCanvasItem::setZ(z);

  // update port's 
  YACSPrs_Port* aPort;
  for (aPort = myPortList.first(); aPort; aPort = myPortList.next())
    aPort->setZ(z,isMoving());

  if ( myPointMaster ) myPointMaster->setZ(z);
}

void YACSPrs_ElementaryNode::setZFromLinks(double z)
{
  // find the maximum Z on all links of this node 
  double aMaxLinkZ = 0;
  YACSPrs_Port* aPort;
  for (aPort = myPortList.first(); aPort; aPort = myPortList.next())
  {
    list<YACSPrs_Link*> aLinks = aPort->getLinks();
    for(list<YACSPrs_Link*>::iterator it = aLinks.begin(); it != aLinks.end(); it++)
      if ( aMaxLinkZ < (*it)->getMyZ() ) aMaxLinkZ = (*it)->getMyZ();
  }

  double aMaxZ = ( z > aMaxLinkZ+1 ) ? z : aMaxLinkZ+1;

  QCanvasItem::setZ(aMaxZ);
  // update port's
  for (aPort = myPortList.first(); aPort; aPort = myPortList.next())
    aPort->setZ(aMaxZ,isMoving());

  if ( myPointMaster ) myPointMaster->setZ(aMaxZ);
}

//! Updates a node presentation during execution
/*!
 *  1) Update state of this node.
 *  2) Update ports values of this node.
 *  3) Update progress bar.
 */
void YACSPrs_ElementaryNode::update()
{
  printf("==> YACSPrs_ElementaryNode::update()\n");

  if ( !getEngine() ) return;

  // update state
  setState( getEngine()->getState() );

  // update ports values
  YACSPrs_Port* aPort = 0;
  for (aPort = myPortList.first(); aPort; aPort = myPortList.next())
  {
    YACSPrs_InOutPort* anIOPort = dynamic_cast<YACSPrs_InOutPort*>( aPort );
    if ( anIOPort && !anIOPort->isGate() )
    {
      printf("==> update port : %s\n",aPort->getName().latin1());
      anIOPort->update();
    }
  }
  
  // update progress bar
  nextTimeIteration();

  // update execution time
  updateExecTime();

  if ( canvas() ) {
    canvas()->setChanged(getRect());
    canvas()->update();
  }
}

//! Updates a node presentation during execution. !!! Use this function only for update original body node of ForEachLoop node.
/*!
 *  1) Update state of this node.
 *  2) Update ports values of this node.
 *  3) Update progress bar.
 *
 * \param theEngine : the engine of clone node from which we have to update presentation of this node.
 */
void YACSPrs_ElementaryNode::updateForEachLoopBody(YACS::ENGINE::Node* theEngine)
{
  printf("==> YACSPrs_ElementaryNode::updateForEachLoopBody()\n");
  
  if ( !getEngine() || !theEngine ) return;

  // update state
  setState( theEngine->getState() );

  // update ports values
  YACSPrs_Port* aPort = 0;
  for (aPort = myPortList.first(); aPort; aPort = myPortList.next())
  {
    YACSPrs_InOutPort* anIOPort = dynamic_cast<YACSPrs_InOutPort*>( aPort );
    if ( anIOPort && !anIOPort->isGate() ) {
      if ( anIOPort->isInput() )
	anIOPort->update( false, theEngine->getInPort(anIOPort->getName().latin1()) );
      else
	anIOPort->update( false, theEngine->getOutPort(anIOPort->getName().latin1()) );
    }
  }
  
  // update progress bar
  nextTimeIteration(theEngine);

  // update execution time
  updateExecTime(theEngine);

  myPercentage = getPercentage(theEngine);

  if ( canvas() ) {
    canvas()->setChanged(getRect());
    canvas()->update();
  }
}

void YACSPrs_ElementaryNode::updatePorts(bool theForce)
{
  bool aDisp = isVisible();
  if (aDisp) hide();

  if (theForce)
  {
    myPortHeight = 2*PORT_MARGIN;
    myPortList.setAutoDelete(true);
    myPortList.clear();
  }
  
  // iterates on all engine ports of the engine node getEngine(),
  // create (if not already exists) for each engine port YACSPrs_InOutPort object and
  // set the rectangle for it

  bool withCreate = false;
  if ( myPortList.isEmpty() ) withCreate = true;

  QRect r = getBodyRect();
  int aPRectWidth = (int)(r.width()/2) - 2*PORT_MARGIN;
  if ( aPRectWidth < PORT_WIDTH ) aPRectWidth = PORT_WIDTH;
  
  int ix = r.x() + PORT_MARGIN + 1;
  int iy = r.y() + PORT_MARGIN;// + 1;
  int ox = ix + aPRectWidth + 2*PORT_MARGIN;
  int oy = r.y() + PORT_MARGIN;// + 1;

  if ( withCreate )
  { // create (and update)

    // input ports
    list<InPort*> anInPortsEngine = getEngine()->getSetOfInPort();
    list<InPort*>::iterator anInPortsIter = anInPortsEngine.begin();
    int heightIncr = 0;
    for( ;anInPortsIter!=anInPortsEngine.end();anInPortsIter++)
      {
	YACSPrs_InOutPort* anInPort = new YACSPrs_InOutPort(myMgr, canvas(), *anInPortsIter, this);
	anInPort->setPortRect(QRect(ix, iy+heightIncr, aPRectWidth, PORT_HEIGHT));
	anInPort->setColor(nodeSubColor());
	anInPort->setStoreColor(nodeSubColor());
	myPortList.append(anInPort);
	heightIncr += PORT_HEIGHT+PORT_SPACE;
      }
    
    // output ports
    list<OutPort*> anOutPortsEngine = getEngine()->getSetOfOutPort();
    list<OutPort*>::iterator anOutPortsIter = anOutPortsEngine.begin();
    heightIncr = 0;
    for( ;anOutPortsIter!=anOutPortsEngine.end();anOutPortsIter++)
      {
	YACSPrs_InOutPort* anOutPort = new YACSPrs_InOutPort(myMgr, canvas(), *anOutPortsIter, this);
	anOutPort->setPortRect(QRect(ox, oy+heightIncr, aPRectWidth, PORT_HEIGHT));
	anOutPort->setColor(nodeSubColor());
	anOutPort->setStoreColor(nodeSubColor());
	myPortList.append(anOutPort);
	heightIncr += PORT_HEIGHT+PORT_SPACE;
      }
    
    int aPortsIncrement = 
      anInPortsEngine.size()>anOutPortsEngine.size()?anInPortsEngine.size():anOutPortsEngine.size();
    myPortHeight += PORT_HEIGHT*aPortsIncrement + (aPortsIncrement-1)*PORT_SPACE;
  }
  else
  { // only update
    YACSPrs_InOutPort* aPort;
    for (aPort = (YACSPrs_InOutPort*)(myPortList.first()); 
	 aPort;
	 aPort = (YACSPrs_InOutPort*)(myPortList.next()))
    { 
      if ( !aPort->isGate() )
      	if ( aPort->isInput() )
	{ // test input ports
	  aPort->setPortRect(QRect(ix, iy, aPRectWidth, PORT_HEIGHT), !mySelfMoving, myArea);
	  iy += PORT_HEIGHT+PORT_SPACE;
	}
	else
	{ // test output ports
	  aPort->setPortRect(QRect(ox, oy, aPRectWidth, PORT_HEIGHT), !mySelfMoving, myArea);
	  oy += PORT_HEIGHT+PORT_SPACE;
	}
    }
  }
    
  // can update gates only after body height will be defined
  updateGates(withCreate);

  if (theForce && myPointMaster)
  {
    QPoint aPnt = getConnectionMasterPoint();
    myPointMaster->setCoords(aPnt.x(), aPnt.y());
  }
    
  if (aDisp) show();
}

void YACSPrs_ElementaryNode::updateGates(bool theCreate)
{
  bool aDisp = isVisible();
  if (aDisp) hide();
  
  QRect aRect = getRect();
  QRect aBRect = getBodyRect();
  int aPRectWidth = (int)(aRect.width()/2) - 2*PORT_MARGIN;
 
  int ix = aBRect.left() + PORT_MARGIN + 1;
  int iy = aBRect.bottom() + PORT_MARGIN + 1;
  int ox = ix + aPRectWidth + 2*PORT_MARGIN;
  int oy = aBRect.bottom() + PORT_MARGIN + 1;

  if ( theCreate )
  { // create (and update)
    // input Gate
    YACSPrs_InOutPort* anInPort = new YACSPrs_InOutPort(myMgr,canvas(),getEngine()->getInGate(),this);
    anInPort->setPortRect(QRect(ix, iy,	aPRectWidth, PORT_HEIGHT));
    anInPort->setColor(nodeSubColor());
    anInPort->setStoreColor(nodeSubColor());
    myPortList.append(anInPort);

    // output Gate
    YACSPrs_InOutPort* anOutPort = new YACSPrs_InOutPort(myMgr,canvas(),getEngine()->getOutGate(),this);
    anOutPort->setPortRect(QRect(ox, oy, aPRectWidth, PORT_HEIGHT));
    anOutPort->setColor(nodeSubColor());
    anOutPort->setStoreColor(nodeSubColor());
    myPortList.append(anOutPort);
  }
  else
  { // only update
    YACSPrs_Port* aPort;
    for (aPort = myPortList.first(); aPort; aPort = myPortList.next())
    { 
      YACSPrs_InOutPort* anIOPort = dynamic_cast<YACSPrs_InOutPort*>( aPort );
      if ( anIOPort && anIOPort->isGate() )
      {
	if ( anIOPort->isInput() ) // test input ports
	  anIOPort->setPortRect(QRect(ix, iy, aPRectWidth, PORT_HEIGHT), !mySelfMoving, myArea);
	else // test output ports
	  anIOPort->setPortRect(QRect(ox, oy, aPRectWidth, PORT_HEIGHT), !mySelfMoving, myArea);
      }
    }
  }
  
  if (aDisp) show();
}

void YACSPrs_ElementaryNode::setNodeColor(const QColor& theColor)
{
  myColor = theColor;
  if ( canvas() )
  { 
    canvas()->setChanged(boundingRect());
    canvas()->update();
  }
}

void YACSPrs_ElementaryNode::setNodeSubColor(const QColor& theColor, bool theSelectionProcess)
{
  mySubColor = theColor;
  YACSPrs_Port* aPort;
  for (aPort = myPortList.first(); aPort; aPort = myPortList.next()) {
    if ( dynamic_cast<YACSPrs_LabelPort*>( aPort ) )
      aPort->setColor(theColor.dark(140));
    else
      aPort->setColor(theColor);
  }
  if ( canvas() )
  {
    QRect aRect;
    if ( theSelectionProcess )
    {
      aRect = boundingRect();
      QPoint aShift(20,20);
      aRect.setTopLeft(aRect.topLeft()-aShift);
      aRect.setBottomRight(aRect.bottomRight()+aShift);
    }
    else
      aRect = getRect();
    canvas()->setChanged(aRect);
    canvas()->update();
  }
}

int YACSPrs_ElementaryNode::height() const
{ 
  return NODE_MARGIN + getInfoHeight() + NODE_MARGIN + getBodyHeight() +
         getGateHeight();
}

int YACSPrs_ElementaryNode::maxWidth() const
{ 
  return boundingRect().width() + 4*HOOKPOINT_SIZE;
}

int YACSPrs_ElementaryNode::maxHeight() const
{ 
  return boundingRect().height() + ( myPointMaster ? myPointMaster->height()/2 : 0 );
}

int YACSPrs_ElementaryNode::getInfoHeight() const
{ 
  return myTitleHeight;
}

int YACSPrs_ElementaryNode::getStatusHeight() const
{
  return myStatusHeight;
}

int YACSPrs_ElementaryNode::getTimeHeight() const
{
  return myTimeHeight;
}

int YACSPrs_ElementaryNode::getBodyHeight() const
{
  return myPortHeight;
}

int YACSPrs_ElementaryNode::getGateHeight() const
{
  return myGateHeight;
}

QRect YACSPrs_ElementaryNode::getRect() const
{
  return QRect((int)x(), (int)y(), width(), height());
}

QRect YACSPrs_ElementaryNode::getTitleRect() const
{
  return QRect((int)x()+NODE_MARGIN, (int)y()+NODE_MARGIN, 
	       width()-2*NODE_MARGIN, getTitleHeight());
}

int YACSPrs_ElementaryNode::getPixMapHeight() const
{ 
  return PIXMAP_HEIGHT;
}

int YACSPrs_ElementaryNode::getPixMapWidth() const
{
  int width;
  int height = PIXMAP_HEIGHT;
  if ( !myStatePixmap.isNull() )
    width = myStatePixmap.width()*(height-2*PIXMAP_MARGIN)/myStatePixmap.height() + 2*PIXMAP_MARGIN;
  else
    width = height;
  return width;
}

QRect YACSPrs_ElementaryNode::getPixmapRect(bool theLeft, bool theService) const
{
  int x = (int)(theLeft ? getTitleRect().left() : getTitleRect().right() + NODE_MARGIN);
  int y = (int)getTitleRect().y() + (theService ? getTitleHeight() + ( getInfoHeight() - getTitleHeight() - getPixMapHeight() )/2 : 0 );
                                                                               /*          vertical   margin          */
  return QRect(x, y, getPixMapWidth(), getPixMapHeight());
}

QRect YACSPrs_ElementaryNode::getBodyRect() const
{
  return QRect((int)x(), ((int)y())+NODE_MARGIN+getInfoHeight()+NODE_MARGIN,
	       width(), getBodyHeight());
}

QRect YACSPrs_ElementaryNode::getGateRect() const
{
  return QRect((int)x(), ((int)y())+NODE_MARGIN+getInfoHeight()+NODE_MARGIN+
	       getBodyHeight(), 
	       width(), getGateHeight());
}

//! Increments time iteration.
/*!
 *  Note : use not null argument of this function only for update original body node of ForEachLoop node.
 *
 * \param theEngine : the engine of clone node from which we have to update presentation of this node.
 */
void YACSPrs_ElementaryNode::nextTimeIteration(YACS::ENGINE::Node* theEngine)
{
  bool nullifyOnToActivate = false;
  if ( !theEngine ) theEngine = getEngine();
  else nullifyOnToActivate = true;

  if ( theEngine
       &&
       ( theEngine->getState() == YACS::INITED /*|| theEngine->getEffectiveState() == YACS::INITED*/
	 ||
	 ( nullifyOnToActivate && ( theEngine->getState() == YACS::TOACTIVATE || theEngine->getEffectiveState() == YACS::TOACTIVATE) ) ) )
    myTimeIteration = 0.;
  else if ( theEngine &&
	    theEngine->getState() != YACS::INITED /*&& theEngine->getEffectiveState() != YACS::INITED*/ &&
	    !myFinished ) {
    const double aGoodTime = 0.02 * 100; // estimated time to run, s
    myTimeIteration += 1./aGoodTime;
  }
}

//! Returns the progress bar percentage.
/*!
 *  Note : use not null argument of this function only for update original body node of ForEachLoop node.
 *
 * \param theEngine : the engine of clone node from which we have to update presentation of this node.
 */
double YACSPrs_ElementaryNode::getPercentage(YACS::ENGINE::Node* theEngine) const
{
  bool nullifyOnToActivate = false;
  if ( !theEngine ) theEngine = getEngine();
  else nullifyOnToActivate = true;
    
  if ( !theEngine ) return 0.;

  if ( theEngine->getState() == YACS::INITED || /*theEngine->getEffectiveState() == YACS::INITED ||*/
       theEngine->getState() == YACS::TOLOAD || theEngine->getEffectiveState() == YACS::TOLOAD ||
       theEngine->getState() == YACS::DISABLED || theEngine->getEffectiveState() == YACS::DISABLED
       ||
       ( nullifyOnToActivate && ( theEngine->getState() == YACS::TOACTIVATE || theEngine->getEffectiveState() == YACS::TOACTIVATE) ) )
    return 0.;

  if ( theEngine->getState() == YACS::DONE )
    return 100.;

  // progress bar is manipulated at logarithmic scale:
  // iteration=1 -> 1%, iteration==goodtime -> 50%, iteration=infinite -> 99%
  return ( 0.499 + 99. * ( myTimeIteration * myTimeIteration / ( 1. + myTimeIteration * myTimeIteration ) ) );
}

//! Updates execution time.
/*!
 *  Note : use not null argument of this function only for update original body node of ForEachLoop node.
 *
 * \param theEngine : the engine of clone node from which we have to update presentation of this node.
 */
void YACSPrs_ElementaryNode::updateExecTime(YACS::ENGINE::Node* theEngine)
{
  bool nullifyOnToActivate = false;
  if ( !theEngine ) theEngine = getEngine();
  else nullifyOnToActivate = true;
    
  if ( !theEngine ) return;

  if ( theEngine->getState() == YACS::DISABLED || theEngine->getEffectiveState() == YACS::DISABLED )
  {
    myTime = QString("00:00:00");
    return;
  }

  if ( theEngine->getState() == YACS::INITED /*|| theEngine->getEffectiveState() == YACS::INITED*/
       ||
       ( nullifyOnToActivate && ( theEngine->getState() == YACS::TOACTIVATE || theEngine->getEffectiveState() == YACS::TOACTIVATE) ) )
  {
    myTime = QString("00:00:00");
    myStarted = false;
    if ( theEngine->getState() == YACS::INITED ) return;
  }

  if ( !myTime.compare(QString("00:00:00")) && !myStarted ) {
    myStartTime.start();
    myStarted = true;
    myFinished = false;
  }

  if ( theEngine->getState() != YACS::INITED && theEngine->getState() != YACS::DONE && 
       theEngine->getState() != YACS::FAILED && theEngine->getState() != YACS::ERROR )
  {
    int aMS = myStartTime.elapsed();
    int aH = aMS/3600000;
    int aM = aMS/60000 - aH*60;
    int aS = aMS/1000 - aH*3600 - aM*60;
    QTime aT(aH,aM,aS);
    myTime = aT.toString();
    return;
  }

  if ( !myFinished && ( theEngine->getState() == YACS::DONE ||
			theEngine->getState() == YACS::FAILED ||
			theEngine->getState() == YACS::ERROR ) )
  {
    int aMS = myStartTime.elapsed();
    int aH = aMS/3600000;
    int aM = aMS/60000 - aH*60;
    int aS = aMS/1000 - aH*3600 - aM*60;
    QTime aT(aH,aM,aS);
    myTime = aT.toString();
    myFinished = true;
    return;
  }
}

/*!
  Set state and update pixmap
*/
void YACSPrs_ElementaryNode::setState(YACS::StatesForNode theState)
{
  switch ( theState )
    {
    case YACS::INITED:
      myStatus = QString("Inited");
      myStatePixmap = myMgr->loadPixmap( "YACSPrs", QObject::tr( "ICON_STATUS_NO" ));
      break;
    case YACS::TOLOAD:
      myStatus = QString("To Load");
      myStatePixmap = myMgr->loadPixmap( "YACSPrs", QObject::tr( "ICON_STATUS_WAITING" ));
      break;
    case YACS::LOADED:
      myStatus = QString("Loaded");
      myStatePixmap = myMgr->loadPixmap( "YACSPrs", QObject::tr( "ICON_STATUS_WAITING" ));
      break;
    case YACS::TOACTIVATE:
      myStatus = QString("To Activate");
      myStatePixmap = myMgr->loadPixmap( "YACSPrs", QObject::tr( "ICON_STATUS_WAITING" ));
      break;
    case YACS::ACTIVATED:
      myStatus = QString("Activated");
      myStatePixmap = myMgr->loadPixmap( "YACSPrs", QObject::tr( "ICON_STATUS_RUNNING" ));
      break;
    case YACS::DESACTIVATED:
      myStatus = QString("Desactivated");
      myStatePixmap = myMgr->loadPixmap( "YACSPrs", QObject::tr( "ICON_STATUS_DONE" ));
      break;
    case YACS::DONE:
      myStatus = QString("Done");
      myStatePixmap = myMgr->loadPixmap( "YACSPrs", QObject::tr( "ICON_STATUS_DONE" ));
      break;
    case YACS::SUSPENDED:
    case YACS::LOADFAILED:
    case YACS::EXECFAILED:
    case YACS::PAUSE:
      break;
    case YACS::DISABLED:
      myStatus = QString("Disabled");
      myStatePixmap = myMgr->loadPixmap( "YACSPrs", QObject::tr( "ICON_DISABLED" ));
      break;
    case YACS::INTERNALERR:
      myStatus = QString("Internal Error");
      myStatePixmap = myMgr->loadPixmap( "YACSPrs", QObject::tr( "ICON_STATUS_ABORTED" ));
      break;
    case YACS::FAILED:
      myStatus = QString("Failed");
      myStatePixmap = myMgr->loadPixmap( "YACSPrs", QObject::tr( "ICON_STATUS_ABORTED" ));
      break;
    case YACS::ERROR:
      myStatus = QString("Error");
      myStatePixmap = myMgr->loadPixmap( "YACSPrs", QObject::tr( "ICON_STATUS_ABORTED" ));
      break;
    default:
      break;
    }
}

void YACSPrs_ElementaryNode::draw(QPainter& thePainter)
{
  thePainter.setPen(pen());
  thePainter.setBrush(nodeColor());
  drawShape(thePainter);
}

void YACSPrs_ElementaryNode::drawShape(QPainter& thePainter)
{
  drawFrame(thePainter);
  drawTitle(thePainter);
  drawPort(thePainter);
  drawGate(thePainter);
}
  
void YACSPrs_ElementaryNode::drawTitleShape(QPainter& thePainter)
{
  thePainter.drawRoundRect(getTitleRect(),myXRnd,myYRnd);
  drawText(thePainter, QString(getEngine()->getName()), getTitleRect(), Qt::AlignHCenter);
}

void YACSPrs_ElementaryNode::drawTitle(QPainter& thePainter)
{
  QBrush saved = thePainter.brush();
  QBrush br( mySubColor );
  thePainter.setBrush(br);
  drawTitleShape(thePainter);
  thePainter.setBrush(saved);
}

void YACSPrs_ElementaryNode::drawPort(QPainter& thePainter)
{
  QRect r = getBodyRect();
  r.setHeight(r.height()+1);

  thePainter.drawRect(r);
  int x0 = (r.left() + r.right())/2;
  thePainter.drawLine(x0, r.top(), x0, r.bottom());
  //if (getStreamHeight() > 0) {
  //  int y0 = r.top() + getPortHeight();
  //  thePainter.drawLine(r.left(), y0, r.right(), y0);
  //}

  int aRRectWidth = (x0 - r.left() - 2*PORT_MARGIN - 2*PORT_SPACE)/3;
  QRect aTRect = getTitleRect();
  int aXRnd = aTRect.width()*myXRnd/aRRectWidth;
  int aYRnd = aTRect.height()*myYRnd/PORT_HEIGHT;

  YACSPrs_Port* aPort;
  for (aPort = myPortList.first(); aPort; aPort = myPortList.next())
  {
    aPort->draw(thePainter, aXRnd, aYRnd);
  } 
}

void YACSPrs_ElementaryNode::drawGate(QPainter& thePainter)
{
  QRect aRect = getRect();
  QRect aBRect = getBodyRect();
  int x0 = (aRect.left() + aRect.right())/2;
  thePainter.drawLine(x0, aBRect.bottom(), x0, aRect.bottom());

  int aRRectWidth = x0 - aRect.left() - 2*PORT_MARGIN;
  QRect aTRect = getTitleRect();
  int aXRnd = aTRect.width()*myXRnd/aRRectWidth;
  int aYRnd = aTRect.height()*myYRnd/PORT_HEIGHT;
  
  YACSPrs_Port* aPort;
  for (aPort = myPortList.first(); aPort; aPort = myPortList.next())
  {
    if ( (YACSPrs_InOutPort*)aPort && ((YACSPrs_InOutPort*)aPort)->isGate() )
      aPort->draw(thePainter,aXRnd,aYRnd);
  } 
}

void YACSPrs_ElementaryNode::drawFrame(QPainter& thePainter) 
{
  QRect aRect = getRect();
  QRect aTRect = getTitleRect();
  int aXRnd = aTRect.width()*myXRnd/aRect.width();
  int aYRnd = aTRect.height()*myYRnd/aRect.height();
  thePainter.drawRoundRect(aRect,aXRnd,aYRnd);

  // draw bounding nodes' polygon if node is currently selected
  if ( isSelected() ) drawBoundary(thePainter,2);
}

void YACSPrs_ElementaryNode::drawBoundary(QPainter& thePainter, int theRightBottomPointIndex)
{
  QPointArray anArr = areaPoints();
  int aSize = anArr.size();
  QPoint aFirstP = *(anArr.begin());
  anArr.putPoints(aSize, 1, aFirstP.x(), aFirstP.y());

  thePainter.drawPolyline(anArr,0,theRightBottomPointIndex+1);
    
  QPoint aRPM( getConnectionMasterPoint().x() + (myPointMaster?myPointMaster->width()/2:0),
	       anArr[theRightBottomPointIndex].y() );
  QPoint aLPM( getConnectionMasterPoint().x() - (myPointMaster?myPointMaster->width()/2:0),
	       anArr[theRightBottomPointIndex].y() );
  thePainter.drawLine(anArr[theRightBottomPointIndex],aRPM);
  thePainter.drawLine(aLPM,anArr[theRightBottomPointIndex+1]);
  
  thePainter.drawPolyline(anArr,theRightBottomPointIndex+1,aSize-theRightBottomPointIndex);
}

QPoint YACSPrs_ElementaryNode::getConnectionMasterPoint()
{
  QRect aRect = getRect();
  return QPoint(aRect.left()+aRect.width()/2, aRect.bottom());
}

void YACSPrs_ElementaryNode::setMasterPointColor(QColor theColor)
{
  if ( myPointMaster ) myPointMaster->setBrush(theColor);
}

bool YACSPrs_ElementaryNode::checkArea(double dx, double dy)
{
  // for constraint nodes' moving inside the Bloc-->
  if ( !myIsInBloc ) return true;

  QRect aRect = boundingRect();
  aRect.moveBy((int)dx, (int)dy);
  aRect.setRect(aRect.x()-2*HOOKPOINT_SIZE, aRect.y(), maxWidth(), maxHeight());
  if ( myArea.isValid() && myArea.contains(aRect) )//,true) )
    return true;
  return false;
  // <--
}

/*!
 * =========================== YACSPrs_Port ===========================
 !*/

YACSPrs_Port::YACSPrs_Port( SUIT_ResourceMgr* theMgr, QCanvas* theCanvas, YACSPrs_ElementaryNode* theNode ):
  myMgr(theMgr),
  myCanvas(theCanvas),
  myPoint(0),
  myNode(theNode)
{
  myName = QString("");
  
  myVisible = true;
  myStoreColor = myColor = QColor();

  mySelected = false;
}

YACSPrs_Port::~YACSPrs_Port()
{
  printf(">> YACSPrs_Port::~YACSPrs_Port()\n");
  if (myPoint) delete myPoint;

  for(list<YACSPrs_Link*>::iterator it = myLinks.begin(); it != myLinks.end(); it++) {
    if ( YACSPrs_PortLink* aPL = dynamic_cast<YACSPrs_PortLink*>( *it ) ) {
      //delete aPL;
      //aPL->getInputPort()->removeLink(aPL);
      aPL->setInputPort(0);

      //aPL->getOutputPort()->removeLink(aPL);
      aPL->setOutputPort(0);
    }
    else if ( YACSPrs_LabelLink* aLL = dynamic_cast<YACSPrs_LabelLink*>( *it ) ) {
      //delete aLL;
      //aLL->getOutputPort()->setSlaveNode(0);
      aLL->setOutputPort(0);

      //aLL->getSlaveNode()->removeLabelLink();
      aLL->setSlaveNode(0);
    }
  }
  
  //for(list<YACSPrs_Link*>::iterator it = myLinks.begin(); it != myLinks.end(); ++it) {
  //  delete (*it);
  //}
  //myLinks.clear();
}

void YACSPrs_Port::setCanvas(QCanvas* theCanvas)
{ 
  myCanvas = theCanvas;
  
  // set canvas to hook
  if (myPoint) myPoint->setCanvas(theCanvas);
}

void YACSPrs_Port::updateLinks(bool theMoveInternalLinkPoints, QRect theArea)
{
  for(list<YACSPrs_Link*>::iterator it = myLinks.begin(); it != myLinks.end(); it++)
    (*it)->moveByPort(this, theMoveInternalLinkPoints, theArea);
}

QString YACSPrs_Port::getName() const
{
  return myName;
}

void YACSPrs_Port::setVisible(bool b)
{
  if (myPoint) myPoint->setVisible(b);
  if (b) updateLinks();
}

void YACSPrs_Port::setColor(const QColor& theColor, bool theUpdatePointColor,
			    bool theUpdate, bool theSelectionProcess)
{
  myColor = theColor;
  if (myCanvas && theUpdate)
  {
    QRect aRect = getPortRect();
    if ( theSelectionProcess )
    {
      QPoint aShift(PORT_MARGIN/2,PORT_MARGIN/2);
      aRect.setTopLeft(aRect.topLeft()-aShift);
      aRect.setBottomRight(aRect.bottomRight()+aShift);
    }
    myCanvas->setChanged(aRect);
    myCanvas->update();
  }
}

void YACSPrs_Port::moveBy(int dx, int dy)
{
  for(list<YACSPrs_Link*>::iterator it = myLinks.begin(); it != myLinks.end(); it++)
    (*it)->moveByPort(this, dx, dy);
}

void YACSPrs_Port::setZ(double z, bool storeOldZ)
{
  if (myPoint) myPoint->setZ(z);
  for(list<YACSPrs_Link*>::iterator it = myLinks.begin(); it != myLinks.end(); it++) {
    if ( storeOldZ ) (*it)->setMyZ((*it)->z());
    (*it)->setZ(z-2);
  }
}

double YACSPrs_Port::z()
{
  if (myPoint) return myPoint->z();
  return 0;
}

/*!
 * =========================== YACSPrs_LabelPort ===========================
 !*/

YACSPrs_LabelPort::YACSPrs_LabelPort( SUIT_ResourceMgr* theMgr, QCanvas* theCanvas, YACS::ENGINE::Node* theNode, YACSPrs_ElementaryNode* theNodePrs,
				      const bool& theSwitch, const int& theId ):
  YACSPrs_Port(theMgr, theCanvas, theNodePrs),
  mySlaveNode(theNode)
{
  if ( theSwitch )
    myName = QString("Case ") + QString::number(theId);
  else
    myName = QString("Body");
  
  myStoreColor = myColor = LABELPORT_COLOR;
  
  myPoint = new YACSPrs_Hook(theMgr, theCanvas, this, false, false);
}

YACSPrs_LabelPort::~YACSPrs_LabelPort()
{
}

void YACSPrs_LabelPort::setPortRect(const QRect& theRect, bool theMoveInternalLinkPoints, QRect theArea)
{
  myNameRect = theRect;
  myNameRect.setWidth(theRect.width()-3);
  
  QPoint aPnt = getConnectionPoint();
  if (myPoint) {
    myPoint->setCoords(aPnt.x(), aPnt.y());
    if (myVisible) myPoint->show();
  }
  updateLinks(theMoveInternalLinkPoints, theArea);
}

QRect YACSPrs_LabelPort::getPortRect() const
{
  return myNameRect;
}

void YACSPrs_LabelPort::setColor(const QColor& theColor, bool theUpdatePointColor,
				 bool theUpdate, bool theSelectionProcess)
{
  if ( theUpdatePointColor ) myPoint->setColor( theColor );
  YACSPrs_Port::setColor(theColor, theUpdatePointColor, theUpdate, theSelectionProcess);
}

void YACSPrs_LabelPort::moveBy(int dx, int dy)
{
  myNameRect.moveBy(dx, dy);
  if (myPoint) myPoint->moveBy(dx, dy);
  
  YACSPrs_Port::moveBy(dx, dy);
}

QPoint YACSPrs_LabelPort::getConnectionPoint() const
{
  int x, y;
  x = myNameRect.right() + PORT_MARGIN + 2 + 3*HOOKPOINT_SIZE/2;
  y = (int)(myNameRect.top() + myNameRect.bottom())/2;
  return QPoint(x, y);
}

void YACSPrs_LabelPort::draw(QPainter& thePainter, int theXRnd, int theYRnd)
{
  QBrush savedB = thePainter.brush();
  QBrush br( myColor );
  thePainter.setBrush(br);

  QPen savedP = thePainter.pen();
  thePainter.setPen(thePainter.brush().color().dark(140));
  thePainter.drawRoundRect(myNameRect,theXRnd,theYRnd);

  thePainter.setPen(Qt::white);
  drawText(thePainter, myName, myNameRect, Qt::AlignHCenter);
 
  thePainter.setPen(savedP);
  thePainter.setBrush(savedB);

  // draw bounding rectangle of the port if it is currently selected
  if ( isSelected() )
  {
    QBrush savedB = thePainter.brush();
    thePainter.setBrush(Qt::NoBrush);

    QRect aRect = getPortRect();
    aRect.setTopLeft(aRect.topLeft()-QPoint(PORT_MARGIN/2,PORT_MARGIN/2));
    aRect.setBottomRight(aRect.bottomRight()+QPoint(PORT_MARGIN/2,PORT_MARGIN/2));
    thePainter.drawRect(aRect);
    
    thePainter.setBrush(savedB);
  }
}

/*!
 * =========================== YACSPrs_InOutPort ===========================
 !*/

YACSPrs_InOutPort::YACSPrs_InOutPort( SUIT_ResourceMgr* theMgr, QCanvas* theCanvas, YACS::ENGINE::Port* thePort, YACSPrs_ElementaryNode* theNodePrs ):
  YACSPrs_Port(theMgr, theCanvas, theNodePrs),
  myEngine(thePort)
{
  myInput = false;
  myGate = false;

  InPort* anInPort = dynamic_cast<InPort*>(myEngine);
  if (anInPort) myInput = true;
  else {
    InGate* anInGate = dynamic_cast< InGate*>(myEngine);
    if (anInGate) myGate = myInput = true;
    else {
      OutGate* anOutGate = dynamic_cast< OutGate*>(myEngine);
      if (anOutGate) myGate = true;
    }
  }

  myName = getName();
  
  if ( !myGate ) {
    myType = getType();
    myValue = getValue();
  }

  myColor = myMgr->colorValue( "QxGraph", "Title", TITLE_COLOR );

  myPoint = new YACSPrs_Hook(theMgr, theCanvas, this, myInput, myGate);
}

YACSPrs_InOutPort::~YACSPrs_InOutPort()
{
}

//! Updates a port presentation during execution.
/*!
 *  Note : use not null second argument of this function only for update original body node of ForEachLoop node.
 *
 * \param theEngine : the engine of port of clone node from which we have to update value of this port.
 */
void YACSPrs_InOutPort::update(bool theForce, YACS::ENGINE::Port* theEngine)
{
  QString aNewName = getName();
  if (theForce || myName.compare(aNewName) != 0) {
    myName = aNewName;
    if (myCanvas) myCanvas->setChanged(myNameRect);
  }
  if ( !myGate )
  {  
    QString aNewType = getType();
    QString aNewValue = getValue(theEngine);
    if (theForce || myType.compare(aNewType) != 0) {
      myType = aNewType;
      if (myCanvas) myCanvas->setChanged(myTypeRect);
    }
    if (theForce || myValue.compare(aNewValue) != 0) {
      myValue = aNewValue;
      if (myCanvas) myCanvas->setChanged(myValueRect);
    }
  }
}

bool YACSPrs_InOutPort::isHilight() const
{
  //Port aPort = myPort->getEngine();
  bool b = false;
  /*if (!aPort->IsGate()) {
    if (aPort->IsInput()) {
      if (aPort->HasInput() && !aPort->IsLinked())
	b = true;
    }
    else if (myPort->inherits("YACSGui_PortOut")) {
      YACSGui_PortOut* aPortOut = (YACSGui_PortOut*) myPort;
      if (aPortOut->isInStudy())
	b = true;
    }
    }*/
  return b;
}

bool YACSPrs_InOutPort::isAlert() const
{
  bool b = false;
  /*Port aPort = myPort->getEngine();
  if (!aPort->IsGate()) {
    if (aPort->IsInput() && !aPort->HasInput() && !aPort->IsLinked())
      b = true;
      }*/
  return b;
}

QString YACSPrs_InOutPort::getName() const
{
  QString aName;
  if ( !myGate )
  {
    DataPort* aDataPort = dynamic_cast<DataPort*>(myEngine);
    if ( aDataPort )
      aName = QString(aDataPort->getName());
  }
  else
    aName = QString("Gate");
  return aName;
}

QString YACSPrs_InOutPort::getType(const bool forToolTip) const
{
  QString aType;
  if ( !myGate )
  {
    DataPort* aDataPort = dynamic_cast<DataPort*>(myEngine);
    if (aDataPort) {
      DynType aDType = aDataPort->edGetType()->kind();
      if (aDType == Objref) {
	TypeCodeObjref* aRefTypeCode = dynamic_cast<TypeCodeObjref*>(aDataPort->edGetType());
	if (aRefTypeCode) {
	  aType = aRefTypeCode->name();
	  if (aType.isEmpty()) 
	    aType = QString(TypeCode::getKindRepr( aDType ));
	  if (forToolTip)
	    aType += QString(" (obj. reference)");
	}
      }
      else if (aDType == Sequence) {
	TypeCodeSeq* aSeqTypeCode = dynamic_cast<TypeCodeSeq*>(aDataPort->edGetType());
	if (aSeqTypeCode) {
	  aType = aSeqTypeCode->name();
	  if (aType.isEmpty()) 
	    aType = QString(TypeCode::getKindRepr( aDType ));
	  if (forToolTip)
	    aType += QString(" (seq. of %1)").arg(aSeqTypeCode->contentType()->name());
	}
      }
      else
	aType = QString( TypeCode::getKindRepr( aDType ) );
    }
  }
  return aType;
}

//! Returns a port value.
/*!
 *  Note : use not null argument of this function only for update original body node of ForEachLoop node.
 *
 * \param theEngine : the engine of port of clone node from which we have to get value of this port.
 */
QString YACSPrs_InOutPort::getValue(YACS::ENGINE::Port* theEngine) const
{
  if ( !theEngine ) theEngine = myEngine;

  QString aValue;
  if ( !myGate )
  {
    // variables to store ports values
    Any* anAny = 0;
    CORBA::Any* aCorbaAny = 0;
    
    if ( myInput )
    {
      if ( SeqAnyInputPort* aSeqAnyP = dynamic_cast<SeqAnyInputPort*>(theEngine) )
      { // check if theEngine is SeqAnyInputPort
	//printf("SeqAnyInputPort : %s\n",getName().latin1());
	anAny = aSeqAnyP->getValue();
	if ( !anAny ) aValue = QString("[ ? ]");
	else toString(anAny, aValue);
      }
      else if ( AnyInputPort* anAnyP = dynamic_cast<AnyInputPort*>(theEngine) )
      { // check if theEngine is AnyInputPort
	//printf("AnyInputPort : %s\n",getName().latin1());
	toString(anAnyP->getValue(), aValue);
      }
      else if ( ConditionInputPort* aConditionP = dynamic_cast<ConditionInputPort*>(theEngine) )
      { // check if theEngine is ConditionInputPort
	//printf("ConditionInputPort : %s\n",getName().latin1());
	aValue = QString( aConditionP->getValue() ? "true" : "false" );
      }
      else if ( InputCorbaPort* aCorbaP = dynamic_cast<InputCorbaPort*>(theEngine) )
      { // check if theEngine is InputCorbaPort
	//printf("InputCorbaPort : %s\n",getName().latin1());
	toString( aCorbaP->getAny(), aValue );
      }
      else if ( InputPyPort* aPyP = dynamic_cast<InputPyPort*>(theEngine) )
      { // check if theEngine is InputPyPort
	//printf("InputPyPort : %s\n",getName().latin1());
	toString( aPyP->getPyObj(), aValue );
      }
      else if ( InputXmlPort* aXmlP = dynamic_cast<InputXmlPort*>(theEngine) )
      { // check if theEngine is InputXmlPort
	//printf("InputXmlPort : %s\n",getName().latin1());
      }
      else if ( InputCalStreamPort* aCalStreamP = dynamic_cast<InputCalStreamPort*>(theEngine) )
      { // check if theEngine is InputCalStreamPort
	//printf("InputCalStreamPort : %s\n",getName().latin1());
	aValue = QString("data stream");
      }
      else if ( InputDataStreamPort* aDataStreamP = dynamic_cast<InputDataStreamPort*>(theEngine) )
      { // check if theEngine is InputDataStreamPort
	//printf("InputDataStreamPort : %s\n",getName().latin1());
	aValue = QString("data stream");
      }
      else if ( InputPort* anInputP = dynamic_cast<InputPort*>(theEngine) )
      { // check if theEngine is InputPort
	//printf("InputPort : %s\n",getName().latin1());
      }
    }
    else
    {
      // this case was implemented only for the initial output ports values (before execution finished)
      
      if ( AnyOutputPort* anAnyP = dynamic_cast<AnyOutputPort*>(theEngine) )
      { // check if theEngine is AnyOutputPort
	//printf("AnyOutputPort : %s\n",getName().latin1());
	toString(anAnyP->getValue(), aValue);
      }
      else if ( OutputCorbaPort* aCorbaP = dynamic_cast<OutputCorbaPort*>(theEngine) )
      { // check if theEngine is OutputCorbaPort
	//printf("OutputCorbaPort : %s\n",getName().latin1());
	toString( aCorbaP->getAny(), aValue );
      }
      else if ( OutputPyPort* aPyP = dynamic_cast<OutputPyPort*>(theEngine) )
      { // check if theEngine is OutputPyPort
	//printf("OutputPyPort : %s\n",getName().latin1());
	toString( aPyP->get(), aValue );
      }
      else if ( OutputXmlPort* aXmlP  = dynamic_cast<OutputXmlPort*>(theEngine))
      { // check if theEngine is OutputXmlPort
	//printf("OutputXmlPort : %s\n",getName().latin1());
      }
      else if ( OutputCalStreamPort* aCalStreamP = dynamic_cast<OutputCalStreamPort*>(theEngine) )
      { // check if theEngine is OutputCalStreamPort
	//printf("OutputCalStreamPort : %s\n",getName().latin1());
	aValue = QString("data stream");
      }
      else if ( OutputDataStreamPort* aCalStreamP = dynamic_cast<OutputDataStreamPort*>(theEngine) )
      { // check if theEngine is OutputDataStreamPort
	//printf("OutputDataStreamPort : %s\n",getName().latin1());
	aValue = QString("data stream");
      }
      else if ( OutputPort* anOutputP = dynamic_cast<OutputPort*>(theEngine) )
      { // check if theEngine is OutputPort
	//printf("OutputPort : %s\n",getName().latin1());
      }
    }
  }
  return aValue;
}

int YACSPrs_InOutPort::getAlignment() const
{
  int a = Qt::AlignAuto;
  if ( myGate && myInput )
    a = Qt::AlignLeft;
  else if ( myGate && !myInput )
    a = Qt::AlignRight;
  return a;
}

void YACSPrs_InOutPort::setPortRect(const QRect& theRect, bool theMoveInternalLinkPoints, QRect theArea)
{
  if ( !myGate )
  {  
    int aRectWidth = (theRect.right() - theRect.left() - 2*(PORT_SPACE))/3;
    myNameRect = QRect(theRect.left(), theRect.top(),
		       aRectWidth,PORT_HEIGHT);
    myTypeRect = QRect(myNameRect.right()+PORT_SPACE, theRect.top(),
		       aRectWidth,PORT_HEIGHT);
    myValueRect = QRect(myTypeRect.right()+PORT_SPACE, theRect.top(),
			aRectWidth,PORT_HEIGHT);
  }
  else
  {
    myNameRect = theRect;
    myNameRect.setWidth(theRect.width()-3);
  }
  
  QPoint aPnt = getConnectionPoint();
  if (myPoint) {
    myPoint->setCoords(aPnt.x(), aPnt.y());
    if (myVisible) myPoint->show();
  }

  updateLinks(theMoveInternalLinkPoints,theArea);
}

QRect YACSPrs_InOutPort::getPortRect() const
{
  if ( !myGate )
    return QRect(myNameRect.left(), myNameRect.top(),
		 myNameRect.width()*3+2*PORT_SPACE, PORT_HEIGHT);
  else
    return myNameRect;
}

void YACSPrs_InOutPort::moveBy(int dx, int dy)
{
  myNameRect.moveBy(dx, dy);
  if ( !myGate )
  {  
    myTypeRect.moveBy(dx, dy);
    myValueRect.moveBy(dx, dy);
  }
  if (myPoint) myPoint->moveBy(dx, dy);

  YACSPrs_Port::moveBy(dx, dy);
}

void YACSPrs_InOutPort::setZ(double z, bool storeOldZ)
{
  if (myPoint) myPoint->setZ(z);
  for(list<YACSPrs_Link*>::iterator it = myLinks.begin(); it != myLinks.end(); it++) {
    if ( storeOldZ ) (*it)->setMyZ((*it)->z());
    
    YACSPrs_PortLink* aPortLink = dynamic_cast<YACSPrs_PortLink*>( *it );
    if ( aPortLink )
    {
      if ( myInput && aPortLink->getOutputPort() )
	aPortLink->setZ( ( z > aPortLink->getOutputPort()->z() ) ? z-2 : aPortLink->getOutputPort()->z()-2 );
      if ( !myInput && aPortLink->getInputPort() )
	aPortLink->setZ( ( z > aPortLink->getInputPort()->z() ) ? z-2 : aPortLink->getInputPort()->z()-2 );
    }
  }
}

QPoint YACSPrs_InOutPort::getConnectionPoint() const
{
  int x, y;
  if ( myGate )
  {
    if ( myInput )
      x = myNameRect.left() - PORT_MARGIN - 1;
    else
      x = myNameRect.right() + PORT_MARGIN + 3;
  }
  else
  {
    if ( myInput )
      x = myNameRect.left() - PORT_MARGIN - 1 - 3*HOOKPOINT_SIZE/2;
    else
      x = myValueRect.right() + PORT_MARGIN + 2 + 3*HOOKPOINT_SIZE/2;
  }
  y = (int)(myNameRect.top() + myNameRect.bottom())/2;
  return QPoint(x, y);
}

void YACSPrs_InOutPort::draw(QPainter& thePainter, int theXRnd, int theYRnd)
{
  QFont savedF = thePainter.font();
  QFont f(savedF);
  f.setBold(isHilight());
  thePainter.setFont(f);
  
  QBrush savedB = thePainter.brush();
  QBrush br( myColor );
  thePainter.setBrush(br);

  QPen savedP = thePainter.pen();
  if ( myType.compare(QString("Sequence")) )
    thePainter.setPen(thePainter.brush().color().dark(140));

  // port name
  thePainter.drawRoundRect(myNameRect,theXRnd,theYRnd);
  if ( !myGate )
  {
    // port type
    thePainter.drawRoundRect(myTypeRect,theXRnd,theYRnd);
    // port value
    thePainter.drawRoundRect(myValueRect,theXRnd,theYRnd);
  }

  /*if (myPort->isStream())
    thePainter.setPen(QColor(128, 64, 0));
    else */
  if (isHilight())
    thePainter.setPen(Qt::black);
  else if (isAlert())
    thePainter.setPen(Qt::red.dark(120));
  else
    thePainter.setPen(Qt::white);

  drawText(thePainter, myName, myNameRect, getAlignment());
  if ( !myGate )
  {
    drawText(thePainter, myType, myTypeRect, getAlignment());
    drawText(thePainter, myValue, myValueRect, getAlignment());
  }
 
  thePainter.setPen(savedP);
  thePainter.setFont(savedF);
  thePainter.setBrush(savedB);

  // draw bounding rectangle of the port if it is currently selected
  if ( isSelected() )
  {
    QBrush savedB = thePainter.brush();
    thePainter.setBrush(Qt::NoBrush);

    QRect aRect = getPortRect();
    aRect.setTopLeft(aRect.topLeft()-QPoint(PORT_MARGIN/2,PORT_MARGIN/2));
    if ( myGate )
      aRect.setBottomRight(aRect.bottomRight()+QPoint(PORT_MARGIN/2,PORT_MARGIN/2));
    else
      aRect.setBottomRight(aRect.bottomRight()+QPoint(PORT_MARGIN/2-2,PORT_MARGIN/2));
    thePainter.drawRect(aRect);
    
    thePainter.setBrush(savedB);
  }
}

/*!
 * =========================== YACSPrs_Hook ===========================
 !*/

YACSPrs_Hook::YACSPrs_Hook(SUIT_ResourceMgr* theMgr,
			   QCanvas* theCanvas, 
			   YACSPrs_ElementaryNode* theNode,
			   const bool& theIn,
			   const bool& theGate,
			   const bool& theMaster):
  QCanvasEllipse(HOOKPOINT_SIZE, HOOKPOINT_SIZE, theCanvas),
  myMgr(theMgr), myNodePrs(theNode), myPortPrs(0), myIn(theIn), myGate(theGate), myMaster(theMaster), myLine(0), mySelected(false)
{
  init(theCanvas);
}

YACSPrs_Hook::YACSPrs_Hook(SUIT_ResourceMgr* theMgr,
			   QCanvas* theCanvas, 
			   YACSPrs_Port* thePort,
			   const bool& theIn,
			   const bool& theGate,
			   const bool& theMaster):
  QCanvasEllipse(HOOKPOINT_SIZE, HOOKPOINT_SIZE, theCanvas),
  myMgr(theMgr), myNodePrs(0), myPortPrs(thePort), myIn(theIn), myGate(theGate), myMaster(theMaster), myLine(0), mySelected(false)
{
  init(theCanvas);
}

void YACSPrs_Hook::init(QCanvas* theCanvas)
{
  if ( myMaster )
  {
    setSize(HOOKPOINTMASTER_SIZE, HOOKPOINTMASTER_SIZE/2);
    setAngles(180*16,180*16);
  }
  else if ( myGate )
  {
    setSize(HOOKPOINTGATE_SIZE, HOOKPOINTGATE_SIZE);
    if ( myIn ) setAngles(90*16,180*16);
    else setAngles(270*16,180*16);
  }
  else
  {
    myLine = new QCanvasLine(theCanvas);
    myLine->setPen(QPen(HOOK_COLOR, 2));
  }
  
  setBrush(HOOK_COLOR);
  setZ(1);
}

YACSPrs_Hook::~YACSPrs_Hook()
{
  hide();
  if ( myLine ) {
    delete myLine;
    myLine = 0;
  }
}

void YACSPrs_Hook::setCanvas(QCanvas* theCanvas)
{
  QCanvasItem::setCanvas(theCanvas);
  
  // set canvas to line
  if (myLine) myLine->setCanvas(theCanvas);
}

void YACSPrs_Hook::hilight(const QPoint& theMousePos, const bool toHilight)
{
  if ( mySelected ) return;

  // process the hilighting for hook:
  QColor aHookHilightColor = myMgr->colorValue("YACSGui", "hook_hilight_color", HOOK_HILIGHT_COLOR);
  QColor aHookDrawColor = myMgr->colorValue("YACSGui", "hook_color", HOOK_COLOR);
  QColor aDefinedColor = toHilight?aHookHilightColor:aHookDrawColor;

  if (myNodePrs) {
    YACSPrs_LabelLink* aLabelLink = myNodePrs->getLabelLink(); 
    if (aLabelLink) {
      YACSPrs_LabelPort* aLabelPort = aLabelLink->getOutputPort();
      QColor aColor = toHilight?aHookHilightColor:aLabelPort->storeColor();
      setColor(aColor);
      aLabelLink->setHilighted(toHilight);
      aLabelPort->GetHook()->setColor(aColor);
    }
    else
      setColor(aDefinedColor);
  }
  else if (YACSPrs_LabelPort* aLabelPort = dynamic_cast<YACSPrs_LabelPort*>(myPortPrs))
    setColor(toHilight?aHookHilightColor:aLabelPort->storeColor());
  else
    setColor(aDefinedColor);
  
  // hilight items connected to the hook
  if (myPortPrs)
   {
      std::list<YACSPrs_Link*> aLinks = myPortPrs->getLinks();
      if ( aLinks.size() < 1 ) {
	if ( canvas() ) canvas()->update();
	return;
      }
      std::list<YACSPrs_Link*>::iterator it = aLinks.begin();
      for(; it != aLinks.end(); it++) {
	YACSPrs_Link* aLink = *it;
	aLink->setHilighted(toHilight);
	if (YACSPrs_LabelLink* aLabelLink = dynamic_cast<YACSPrs_LabelLink*>(aLink)) {
	  if (YACSPrs_ElementaryNode* aSlaveNode = aLabelLink->getSlaveNode())
	    aSlaveNode->setMasterPointColor(toHilight?aHookHilightColor:myPortPrs->storeColor());
	}
	else if (YACSPrs_PortLink* aPortLink =  dynamic_cast<YACSPrs_PortLink*>(aLink)) {
	  
	  if (aPortLink->getInputPort() != myPortPrs)
	    aPortLink->getInputPort()->GetHook()->setColor(aDefinedColor);
	  else
	    aPortLink->getOutputPort()->GetHook()->setColor(aDefinedColor);
	}
      }
    }
  
  if ( canvas() ) canvas()->update();
}

void YACSPrs_Hook::select(const QPoint& theMousePos, const bool toSelect)
{
  // unhilight the item under the mouse cursor
  hilight(theMousePos, false);

  // process the hilighting for hook:
  mySelected = toSelect;
  QColor aDefinedColor = toSelect?HOOK_SELECT_COLOR:HOOK_COLOR;

  if (myNodePrs) {
    YACSPrs_LabelLink* aLabelLink = myNodePrs->getLabelLink(); 
    if (aLabelLink) {
      YACSPrs_LabelPort* aLabelPort = aLabelLink->getOutputPort();
      QColor aColor = toSelect?HOOK_SELECT_COLOR:aLabelPort->storeColor();
      setColor(aColor);
      aLabelLink->setSelected(toSelect);
      aLabelPort->GetHook()->setSelected(toSelect);
      aLabelPort->GetHook()->setColor(aColor);
    }
    else
      setColor(aDefinedColor);
  }
  else if (YACSPrs_LabelPort* aLabelPort = dynamic_cast<YACSPrs_LabelPort*>(myPortPrs))
    setColor(toSelect?HOOK_SELECT_COLOR:aLabelPort->storeColor());
  else
    setColor(aDefinedColor);
  
  // select items connected to the hook
  if (myPortPrs)
  {
    std::list<YACSPrs_Link*> aLinks = myPortPrs->getLinks();
    if ( aLinks.size() < 1 ) return;
    std::list<YACSPrs_Link*>::iterator it = aLinks.begin();
    for(; it != aLinks.end(); it++) {
      YACSPrs_Link* aLink = *it;
      aLink->setSelected(toSelect);
      if (YACSPrs_LabelLink* aLabelLink = dynamic_cast<YACSPrs_LabelLink*>(aLink)) {
	if (YACSPrs_ElementaryNode* aSlaveNode = aLabelLink->getSlaveNode()) {
	  if ( aSlaveNode->getMasterPoint() ) aSlaveNode->getMasterPoint()->setSelected(toSelect);
	  aSlaveNode->setMasterPointColor(toSelect?HOOK_SELECT_COLOR:myPortPrs->storeColor());
	}
      }
      else if (YACSPrs_PortLink* aPortLink =  dynamic_cast<YACSPrs_PortLink*>(aLink)) {
	
	if (aPortLink->getInputPort() != myPortPrs) {
	  aPortLink->getInputPort()->GetHook()->setSelected(toSelect);
	  aPortLink->getInputPort()->GetHook()->setColor(aDefinedColor);
	}
	else {
	  aPortLink->getOutputPort()->GetHook()->setSelected(toSelect);
	  aPortLink->getOutputPort()->GetHook()->setColor(aDefinedColor);
	}
      }
    }
  }

  if ( canvas() ) canvas()->update();
}

void YACSPrs_Hook::showPopup(QWidget* theParent, QMouseEvent* theEvent, const QPoint& theMousePos)
{
}

QObject* YACSPrs_Hook::getObject() const
{
  /*QObject* anObj = 0;
  if ( myNodePrs )
    anObj = myNodePrs->getNode();
  else if ( myPortPrs )
    anObj = myPortPrs->getPort();
    return anObj;*/
}

void YACSPrs_Hook::setCoords(int x, int y)
{
  move(x,y);
  if (myLine) { // for not Gate port only
    myLine->move(0, 0);
    myLine->setPoints(x+(myIn?3*HOOKPOINT_SIZE/2:-3*HOOKPOINT_SIZE/2), y, x, y);
    myTrPoint = QPoint(myLine->startPoint().x()+(myIn?-HOOKPOINT_SIZE/2:HOOKPOINT_SIZE/2),
		       myLine->startPoint().y()-HOOKPOINT_SIZE/2);
    //                  myTrPoint
    //                     x
    //                     |\
    //       input port  __|_\  output port
    //                     | /
    //                     |/
  }
}

void YACSPrs_Hook::setColor(const QColor& theColor)
{
  setBrush(theColor);
  if(myLine)
    myLine->setPen(QPen(theColor, 2));
}

void YACSPrs_Hook::setVisible(bool b)
{
  QCanvasEllipse::setVisible(b);
  if (myLine) myLine->setVisible(b);
}

void YACSPrs_Hook::moveBy(double dx, double dy)
{
  QCanvasEllipse::moveBy(dx, dy);
  if (myLine)
  {
    myLine->moveBy(dx, dy);
    myTrPoint.setX((int)(myTrPoint.x()+dx));
    myTrPoint.setY((int)(myTrPoint.y()+dy));
  }  
}

void YACSPrs_Hook::setZ(double z)
{
  QCanvasEllipse::setZ(z);
  if (myLine) myLine->setZ(z);
}

int YACSPrs_Hook::rtti() const
{
  return 0;//YACSPrs_Canvas::Rtti_Hook;
}

QPointArray YACSPrs_Hook::areaPoints() const
{
  if ( myGate || myMaster ) return QCanvasEllipse::areaPoints();
  else
  {
    int w = width() + ( myLine ? ( myIn ? -1 : 1 )*(myLine->endPoint().x()-myLine->startPoint().x()) : 0 );
    int h = height()+1; // add pen width
    
    QPointArray aPnts(4);
    aPnts[0] = QPoint((int)x(), (int)y()) + QPoint( myIn ? -HOOKPOINT_SIZE/2 : HOOKPOINT_SIZE/2 , -HOOKPOINT_SIZE/2);
    aPnts[1] = aPnts[0] + QPoint( myIn ? w : -w , 0);
    aPnts[2] = aPnts[1] + QPoint(0, h);
    aPnts[3] = aPnts[0] + QPoint(0, h);
    return aPnts;
  }
}

void YACSPrs_Hook::drawShape(QPainter& thePainter)
{
  QCanvasEllipse::drawShape(thePainter);

  if ( myMaster )
  {
    QPen savedP = thePainter.pen();
    thePainter.setPen(Qt::white);
    QRect aRectText((int)(x()-width()/2), (int)y(), width(), height()/2);
    drawText(thePainter, QString("Master"), aRectText, Qt::AlignHCenter);
    thePainter.setPen(savedP);
  }
  else if ( myGate )
  { // draw line
    //thePainter.drawLine( (int)x(), (int)(y()-HOOKPOINTGATE_SIZE/2),
    //			 (int)x(), (int)(y()+HOOKPOINTGATE_SIZE/2) );
  }
  else
  { //draw triangle
    if ( myLine )
    {
      QPointArray aPA(3);
      aPA.putPoints(0, 3, myTrPoint.x(),myTrPoint.y(), 
		          myTrPoint.x()+HOOKPOINT_SIZE/2,myTrPoint.y()+HOOKPOINT_SIZE/2, 
		          myTrPoint.x(),myTrPoint.y()+HOOKPOINT_SIZE);
      thePainter.drawPolygon( aPA );
    }
  }
}
