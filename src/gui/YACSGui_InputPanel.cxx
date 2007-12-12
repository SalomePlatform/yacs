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

#include <PythonNode.hxx>

#include <YACSGui_InputPanel.h>
#include <YACSGui_Module.h>
#include <YACSGui_PlusMinusGrp.h>
#include <YACSGui_Table.h>
#include <YACSGui_Graph.h>
#include <YACSGui_Executor.h>

#include <YACSPrs_ElementaryNode.h>

#include <LightApp_Application.h>
#include <CAM_Application.h>
#include <SUIT_Session.h>
#include <SUIT_Application.h>
#include <SUIT_MessageBox.h>

#include <SALOME_ResourcesManager.hxx>
#include <SALOME_LifeCycleCORBA.hxx>

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

std::string portValueType( YACS::ENGINE::TypeCode* theTC )
{
  std::string aType = "";
  switch ( theTC->kind() )
    {
    case Double: aType = "double";     break;
    case Int:    aType = "int";        break;
    case String: aType = "string";     break;
    case Bool:   aType = "bool";       break;
    case Objref: aType = "objref"; 	   break;
    case Sequence: aType = "sequence"; break;
    case Array:    aType = "array";    break;
    case Struct:   aType = "struct";   break;
    default:                           break;
    }
  return aType;
}

/*
  Class : YACSGui_InputPanel::BtnStyle
  Description : Style for buttons with text aligned to the left border
*/

class YACSGui_InputPanel::BtnStyle : public QWindowsStyle
{
public:

  virtual void drawControl( ControlElement element,
			    QPainter *p,
			    const QWidget *widget,
			    const QRect &r,
			    const QColorGroup &cg,
			    SFlags how = Style_Default,
			    const QStyleOption& = QStyleOption::Default ) const;
};

void YACSGui_InputPanel::BtnStyle::drawControl( ControlElement element,
			                                            QPainter *p,
			                                            const QWidget *widget,
			                                            const QRect &r,
			                                            const QColorGroup &cg,
			                                            SFlags flags,
			                                            const QStyleOption& opt ) const
{
  if ( element != CE_PushButtonLabel )
    QWindowsStyle::drawControl( element, p, widget, r, cg, flags, opt );
  else
  {
#ifndef QT_NO_PUSHBUTTON
    const QPushButton *button = (const QPushButton *) widget;
    QRect ir = r;
    
    if ( button->isDown() || button->isOn() ) 
    {
      flags |= Style_Sunken;
      ir.moveBy(pixelMetric(PM_ButtonShiftHorizontal, widget),
        pixelMetric(PM_ButtonShiftVertical, widget));
    }

    int tf=AlignVCenter | ShowPrefix;
    if (!styleHint(SH_UnderlineAccelerator, widget, QStyleOption::Default, 0))
      tf |= NoAccel;
    
    tf |= AlignLeft;
    ir.setLeft( 10 );
    drawItem(p, ir, tf, cg,
      flags & Style_Enabled, button->pixmap(), button->text(),
      button->text().length(), &(cg.buttonText()) );

    // draw arrow
    PrimitiveElement aPE = button->isOn() ? PE_ArrowUp : PE_ArrowDown;
    int h = r.height();
    QRect aRect( r.right() - h, r.top(), h, h );
    SFlags aFlags = Style_Default | Style_Enabled; 
    drawPrimitive( aPE, p, aRect, cg, aFlags, QStyleOption( QColor( 0, 0, 255 ) ) );
    
    if (flags & Style_HasFocus)
      drawPrimitive( PE_FocusRect, p, subRect( SR_PushButtonFocusRect, widget ), cg, flags );
#endif
  }
}

QString getText( int theStatus )
{
  QString aText;
  switch (theStatus)
    {
    case UNDEFINED:    aText = "UNDEFINED";    break;
    case INITED:       aText = "INITED";       break;
    case TOLOAD:       aText = "TOLOAD";       break;
    case LOADED:       aText = "LOADED";       break;
    case TOACTIVATE:   aText = "TOACTIVATE";   break;
    case ACTIVATED:    aText = "ACTIVATED";    break;
    case DESACTIVATED: aText = "DESACTIVATED"; break;
    case DONE:         aText = "DONE";         break;
    case SUSPENDED:    aText = "SUSPENDED";    break;
    case LOADFAILED:   aText = "LOADFAILED";   break;
    case EXECFAILED:   aText = "EXECFAILED";   break;
    case PAUSE:        aText = "PAUSE";        break;
    case INTERNALERR:  aText = "INTERNALERR";  break;
    case DISABLED:     aText = "DISABLED";     break;
    case FAILED:       aText = "FAILED";       break;
    case ERROR:        aText = "ERROR";        break;
    default: break;
    }
  return aText;
}

QColor getColor( int theStatus )
{
  QColor aColor;
  switch (theStatus)
    {
    case UNDEFINED:    aColor = Qt::lightGray;     break;
    case INITED:       aColor = Qt::gray;          break;
    case TOLOAD:       aColor = Qt::darkYellow;    break;
    case LOADED:       aColor = Qt::darkMagenta;   break;
    case TOACTIVATE:   aColor = Qt::darkCyan;      break;
    case ACTIVATED:    aColor = Qt::darkBlue;      break;
    case DESACTIVATED: aColor = Qt::gray;          break;
    case DONE:         aColor = Qt::darkGreen;     break;
    case SUSPENDED:    aColor = Qt::gray;          break;
    case LOADFAILED:   aColor.setHsv(320,255,255); break;
    case EXECFAILED:   aColor.setHsv( 20,255,255); break;
    case PAUSE:        aColor.setHsv(180,255,255); break;
    case INTERNALERR:  aColor.setHsv(340,255,255); break;
    case DISABLED:     aColor.setHsv( 40,255,255); break;
    case FAILED:       aColor.setHsv( 20,255,255); break;
    case ERROR:        aColor.setHsv(  0,255,255); break;
    default:           aColor = Qt::lightGray;
    }
  return aColor;
}

/*
  Class       : YACSGui_InputPanel
  Description : Dockable window containing push buttons and corresponding widgets
*/

//=======================================================================
// name    : YACSGui_InputPanel
// Purpose : Constructor
//=======================================================================
YACSGui_InputPanel::YACSGui_InputPanel( YACSGui_Module* theModule )
  :  QDockWindow( QDockWindow::InDock, theModule && theModule->getApp() ? theModule->application()->desktop() : 0 ),
     myModule(theModule)
{
  QScrollView* scrollView = new QScrollView( this );

  // Create main frame and main layout
  QFrame* aMainFrame  = new QFrame( scrollView->viewport() );
  scrollView->addChild( aMainFrame );
  scrollView->setResizePolicy( QScrollView::AutoOneFit );

  myMainLayout = new QVBoxLayout( aMainFrame, MARGIN, SPACING);

  // create an empty property pages
  createPage( YACSGui_InputPanel::ContainerId );
  createPage( YACSGui_InputPanel::ComponentId );
  createPage( YACSGui_InputPanel::SchemaId );
  createPage( YACSGui_InputPanel::InlineNodeId );
  createPage( YACSGui_InputPanel::ServiceNodeId );
  createPage( YACSGui_InputPanel::ForLoopNodeId );
  createPage( YACSGui_InputPanel::ForEachLoopNodeId );
  createPage( YACSGui_InputPanel::WhileLoopNodeId );
  createPage( YACSGui_InputPanel::SwitchNodeId );
  createPage( YACSGui_InputPanel::BlockNodeId );
  createPage( YACSGui_InputPanel::LinkId );

  myMainLayout->addStretch( 1 );

  // Add "Apply" and "Close" buttons
  QGroupBox* aGrpBtns = new QGroupBox( aMainFrame, "GroupButtons" );
  aGrpBtns->setColumnLayout(0, Qt::Vertical );
  aGrpBtns->layout()->setSpacing( 0 ); aGrpBtns->layout()->setMargin( 0 );
  aGrpBtns->setFrameStyle( QFrame::NoFrame );
  QHBoxLayout* aGrpBtnsLayout = new QHBoxLayout( aGrpBtns->layout() );
  aGrpBtnsLayout->setAlignment( Qt::AlignTop );
  aGrpBtnsLayout->setSpacing( 0 ); aGrpBtnsLayout->setMargin( 0 );

  myApplyBtn = new QPushButton( tr( "APPLY_BTN" ), aGrpBtns );
  connect( myApplyBtn, SIGNAL( clicked() ), this, SLOT( onApply() ) );

  myCancelBtn = new QPushButton( tr( "CLOSE_BTN" ), aGrpBtns );
  connect( myCancelBtn, SIGNAL( clicked() ), this, SLOT( hide() ) );

  aGrpBtnsLayout->addWidget( myApplyBtn );
  aGrpBtnsLayout->addItem  ( new QSpacerItem( 5, 5, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
  aGrpBtnsLayout->addWidget( myCancelBtn );
  
  myMainLayout->addWidget( aGrpBtns );
  myMainLayout->addSpacing( SPACING );

  updateState();

  setWidget( scrollView );
  scrollView->show();
}

//=======================================================================
// name    : ~YACSGui_InputPanel
// Purpose : Destructor
//=======================================================================
YACSGui_InputPanel::~YACSGui_InputPanel()
{
}

//================================================================
// Function : setBtnText
// Purpose  : 
//================================================================
void YACSGui_InputPanel::setBtnText( const QString& theText, const int thePageId )
{
  if ( myMainBtns.contains( thePageId ) )
    myMainBtns[ thePageId ]->setText( theText );
}

//=======================================================================
// name    : getPage
// Purpose : 
//=======================================================================
QWidget* YACSGui_InputPanel::getPage( const int thePageId,
				      const bool theWithCreate )
{
  if ( myPages.contains( thePageId ) )
    return myPages[ thePageId ];
  else if ( theWithCreate )
  {
    createPage( thePageId );
    return myPages.contains( thePageId ) ? myPages[ thePageId ] : 0;
  }

  return 0;
}

//=============================================================================
// Function : getPageId
// Purpose  : 
//=============================================================================
int YACSGui_InputPanel::getPageId( QWidget* thePage ) const
{
  int anId = -1;
  if ( thePage != 0 )
  {
    QMap< int, QWidget* >::const_iterator anIter;
    for ( anIter = myPages.begin(); anIter != myPages.end(); ++anIter )
    {
      QWidget* aPage = anIter.data();
      if ( aPage == thePage )
      {
        anId = anIter.key();
        break;
      }
    }
  }

  return anId;
}

//=============================================================================
// Function : insertPage
// Purpose  : Insert page in input panel. If this is already present in input panel 
//            then it is not inserted. if theBeforePageId == -1 then page is added 
//            at the end. Method returns identifier of inserted page or -1 if page 
//            is not inserted.
//=============================================================================
int YACSGui_InputPanel::insertPage( QWidget*, 
				    const int thePageId,
				    const int theBeforePageId,
				    const int theStretch ) const
{
 
}

//=======================================================================
// name    : sizeHint
// Purpose : 
//=======================================================================
QSize YACSGui_InputPanel::sizeHint() const
{
  int w = QFontMetrics( font() ).width( QString("0"), 1 ) * 30;
  return QSize( w, QDockWindow::sizeHint().height() );
  //return QDockWindow::sizeHint();
}

//=======================================================================
// name    : isOn
// Purpose : 
//=======================================================================
bool YACSGui_InputPanel::isOn( const int i ) const
{
  bool res = false;
  if ( myMainBtns.contains( i ) )
    res = myMainBtns[i]->isOn();
  return res;
}

//=======================================================================
// name    : setOn
// Purpose : 
//=======================================================================
void YACSGui_InputPanel::setOn( const bool on, const int i )
{
  if ( myMainBtns.contains( i ) )
    myMainBtns[i]->setOn( on );
}

//=======================================================================
// name    : isVisible
// Purpose : 
//=======================================================================
bool YACSGui_InputPanel::isVisible( const int i ) const
{
  bool res = false;
  if ( myMainBtns.contains( i ) )
    res = myMainBtns[i]->isVisibleTo( myMainBtns[i]->parentWidget() );
  return res;
}

//=======================================================================
// name    : setVisible
// Purpose : 
//=======================================================================
void YACSGui_InputPanel::setVisible( const bool on, const int i )
{
  if ( myMainBtns.contains( i ) )
    myMainBtns[i]->setShown( on );
  if ( myPages.contains( i ) )
    myPages[i]->setShown( on && myMainBtns[i]->isOn() );
}

//=======================================================================
// name    : setExclusiveVisible
// Purpose : 
//=======================================================================
void YACSGui_InputPanel::setExclusiveVisible( const bool on, const std::list<int> ids )
{
  QMap< int, QWidget* >::const_iterator anIter;
  for ( anIter = myPages.begin(); anIter != myPages.end(); ++anIter )
  {
    int anId = anIter.key();
    
    bool aVisibility = on;
    if ( std::find(ids.begin(), ids.end(), anId) == ids.end() )
      aVisibility = !on;
    
    if ( myMainBtns.contains( anId ) ) myMainBtns[anId]->setShown( aVisibility );
    myPages[anId]->setShown( aVisibility && myMainBtns[anId]->isOn() );
  }
}

//=======================================================================
// name    : setMode
// Purpose : 
//=======================================================================
void YACSGui_InputPanel::setMode( const YACSGui_InputPanel::PageMode theMode, const int thePageId )
{
  switch( thePageId )
  {
  case SchemaId:
    {
      if ( YACSGui_SchemaPage* page = dynamic_cast<YACSGui_SchemaPage*>(myPages[thePageId]) )
      	page->setMode( theMode );
    }
    break;
  case InlineNodeId:
  case ServiceNodeId:
  case ForLoopNodeId:
  case ForEachLoopNodeId:
  case WhileLoopNodeId:
  case SwitchNodeId:
  case BlockNodeId:
    {
      if ( YACSGui_NodePage* page = dynamic_cast<YACSGui_NodePage*>(myPages[thePageId]) )
      	page->setMode( theMode );
    }
    break;
  default:
    break;
  }
}

//================================================================
// Function : getPrefferedSize
// Purpose  : 
//================================================================
QSize YACSGui_InputPanel::getPrefferedSize()
{
  setUpdatesEnabled( false );

  QMap< int, QWidget* >::const_iterator anIter;
  for ( anIter = myPages.begin(); anIter != myPages.end(); ++anIter )
    if ( isVisible(anIter.key()) )
      anIter.data()->updateGeometry();

  QSize aSize = this->sizeHint() * 1.44;
  setUpdatesEnabled( true );
  repaint();
  return aSize;
}

//=======================================================================
// name    : onMainBtn
// Purpose : SLOT. Called when main button ( i.e. button used for show/hide
//           windows of input panel clicked )
//=======================================================================
void YACSGui_InputPanel::onMainBtn( bool )
{
  updateState();
}

//=======================================================================
// name    : onApply
// Purpose : SLOT. Called when "Apply" button clicked. Emit signal with 
//           Corresponding id.
//=======================================================================
void YACSGui_InputPanel::onApply()
{
  QMap< int, QWidget* >::const_iterator anIter;
  for ( anIter = myPages.begin(); anIter != myPages.end(); ++anIter )
  {
    int anId = anIter.key();
    if ( isVisible(anId) ) 
    {
      switch(anId) {
      case ContainerId: {
        YACSGui_ContainerPage* page = dynamic_cast<YACSGui_ContainerPage*>(myPages[anId]);
        if(page)
          page->onApply();
        break;
      }
      case ComponentId: {
        YACSGui_ComponentPage* page = dynamic_cast<YACSGui_ComponentPage*>(myPages[anId]);
        if(page)
          page->onApply();
        break;
      }
      case SchemaId: {
        YACSGui_SchemaPage* page = dynamic_cast<YACSGui_SchemaPage*>(myPages[anId]);
        if(page)
          page->onApply();
        break;
      }
      case InlineNodeId: {
        YACSGui_InlineNodePage* page = dynamic_cast<YACSGui_InlineNodePage*>(myPages[anId]);
        if(page)
          page->onApply();
        break;
      }
      case ServiceNodeId: {
        YACSGui_ServiceNodePage* page = dynamic_cast<YACSGui_ServiceNodePage*>(myPages[anId]);
        if(page)
          page->onApply();
        break;
      }
      case ForLoopNodeId: {
        YACSGui_ForLoopNodePage* page = dynamic_cast<YACSGui_ForLoopNodePage*>(myPages[anId]);
        if(page)
          page->onApply();
        break;
      }
      case ForEachLoopNodeId: {
        YACSGui_ForEachLoopNodePage* page = dynamic_cast<YACSGui_ForEachLoopNodePage*>(myPages[anId]);
        if(page)
          page->onApply();
        break;
      }
      case WhileLoopNodeId: {
        YACSGui_WhileLoopNodePage* page = dynamic_cast<YACSGui_WhileLoopNodePage*>(myPages[anId]);
        if(page)
          page->onApply();
        break;
      }
      case SwitchNodeId: {
        YACSGui_SwitchNodePage* page = dynamic_cast<YACSGui_SwitchNodePage*>(myPages[anId]);
        if(page)
          page->onApply();
        break;
      }
      case BlockNodeId: {
        YACSGui_BlockNodePage* page = dynamic_cast<YACSGui_BlockNodePage*>(myPages[anId]);
        if(page)
          page->onApply();
        break;
      }
      // ...
      case LinkId: {
        YACSGui_LinkPage* page = dynamic_cast<YACSGui_LinkPage*>(myPages[anId]);
        if(page)
          page->onApply();
        break;
      }
      default:
        break;
      }
      emit Apply( anId );
    }
  }

  // export the modified schema (i.e. an active schema!) into
  // the temporary XML file, which will be used for create execution objects
  myModule->temporaryExport();
}

//=======================================================================
// name    : onNotifyNodeStatus
// Purpose : 
//=======================================================================
void YACSGui_InputPanel::onNotifyNodeStatus( int theNodeId, int theStatus )
{
  if ( Node::idMap.count(theNodeId) == 0 ) return;
  Node* aNode= Node::idMap[theNodeId];
  
  QMap< int, QWidget* >::const_iterator anIter;
  for ( anIter = myPages.begin(); anIter != myPages.end(); ++anIter )
  {
    int anId = anIter.key();
    if ( isVisible(anId) )
    {
      switch(anId) {
      case InlineNodeId: {
        YACSGui_InlineNodePage* page = dynamic_cast<YACSGui_InlineNodePage*>(myPages[anId]);
        if(page && page->getNode() == aNode)
          page->notifyNodeStatus(theStatus);
        break;
      }
      case ServiceNodeId: {
        YACSGui_ServiceNodePage* page = dynamic_cast<YACSGui_ServiceNodePage*>(myPages[anId]);
        if(page && page->getNode() == aNode)
          page->notifyNodeStatus(theStatus);
        break;
      }
      case ForLoopNodeId: {
        YACSGui_ForLoopNodePage* page = dynamic_cast<YACSGui_ForLoopNodePage*>(myPages[anId]);
        if(page && page->getNode() == aNode)
          page->notifyNodeStatus(theStatus);
        break;
      }
      case ForEachLoopNodeId: {
        YACSGui_ForEachLoopNodePage* page = dynamic_cast<YACSGui_ForEachLoopNodePage*>(myPages[anId]);
        if(page && page->getNode() == aNode)
          page->notifyNodeStatus(theStatus);
        break;
      }
      case WhileLoopNodeId: {
        YACSGui_WhileLoopNodePage* page = dynamic_cast<YACSGui_WhileLoopNodePage*>(myPages[anId]);
        if(page && page->getNode() == aNode)
          page->notifyNodeStatus(theStatus);
        break;
      }
      case SwitchNodeId: {
        YACSGui_SwitchNodePage* page = dynamic_cast<YACSGui_SwitchNodePage*>(myPages[anId]);
        if(page && page->getNode() == aNode)
          page->notifyNodeStatus(theStatus);
        break;
      }
      case BlockNodeId: {
        YACSGui_BlockNodePage* page = dynamic_cast<YACSGui_BlockNodePage*>(myPages[anId]);
        if(page && page->getNode() == aNode)
          page->notifyNodeStatus(theStatus);
        break;
      }
      default:
        break;
      }
    }
  }
}

//=======================================================================
// name    : onNotifyInPortValues
// Purpose : 
//=======================================================================
void YACSGui_InputPanel::onNotifyInPortValues( int theNodeId,
					       std::list<std::string> theInPortsNames,
					       std::list<std::string> theValues )
{
  if ( Node::idMap.count(theNodeId) == 0 ) return;
  Node* aNode= Node::idMap[theNodeId];
  
  QMap< int, QWidget* >::const_iterator anIter;
  for ( anIter = myPages.begin(); anIter != myPages.end(); ++anIter )
  {
    int anId = anIter.key();
    if ( isVisible(anId) )
    {
      switch(anId) {
      case InlineNodeId: {
        YACSGui_InlineNodePage* page = dynamic_cast<YACSGui_InlineNodePage*>(myPages[anId]);
        if(page && page->getNode() == aNode)
          page->notifyInPortValues(theInPortsNames,theValues);
        break;
      }
      case ServiceNodeId: {
        YACSGui_ServiceNodePage* page = dynamic_cast<YACSGui_ServiceNodePage*>(myPages[anId]);
        if(page && page->getNode() == aNode)
          page->notifyInPortValues(theInPortsNames,theValues);
        break;
      }
      case ForLoopNodeId: {
        YACSGui_ForLoopNodePage* page = dynamic_cast<YACSGui_ForLoopNodePage*>(myPages[anId]);
        if(page && page->getNode() == aNode)
          page->notifyInPortValues(theInPortsNames,theValues);
        break;
      }
      case ForEachLoopNodeId: {
        YACSGui_ForEachLoopNodePage* page = dynamic_cast<YACSGui_ForEachLoopNodePage*>(myPages[anId]);
        if(page && page->getNode() == aNode)
          page->notifyInPortValues(theInPortsNames,theValues);
        break;
      }
      case WhileLoopNodeId: {
        YACSGui_WhileLoopNodePage* page = dynamic_cast<YACSGui_WhileLoopNodePage*>(myPages[anId]);
        if(page && page->getNode() == aNode)
          page->notifyInPortValues(theInPortsNames,theValues);
        break;
      }
      case SwitchNodeId: {
        YACSGui_SwitchNodePage* page = dynamic_cast<YACSGui_SwitchNodePage*>(myPages[anId]);
        if(page && page->getNode() == aNode)
          page->notifyInPortValues(theInPortsNames,theValues);
        break;
      }
      case BlockNodeId: {
        YACSGui_BlockNodePage* page = dynamic_cast<YACSGui_BlockNodePage*>(myPages[anId]);
        if(page && page->getNode() == aNode)
          page->notifyInPortValues(theInPortsNames,theValues);
        break;
      }
      default:
        break;
      }
    }
  }
}

//=======================================================================
// name    : onNotifyOutPortValues
// Purpose : 
//=======================================================================
void YACSGui_InputPanel::onNotifyOutPortValues( int theNodeId,
						std::list<std::string> theOutPortsNames,
						std::list<std::string> theValues )
{
  if ( Node::idMap.count(theNodeId) == 0 ) return;
  Node* aNode= Node::idMap[theNodeId];
  
  QMap< int, QWidget* >::const_iterator anIter;
  for ( anIter = myPages.begin(); anIter != myPages.end(); ++anIter )
  {
    int anId = anIter.key();
    if ( isVisible(anId) )
    {
      switch(anId) {
      case InlineNodeId: {
        YACSGui_InlineNodePage* page = dynamic_cast<YACSGui_InlineNodePage*>(myPages[anId]);
        if(page && page->getNode() == aNode)
          page->notifyOutPortValues(theOutPortsNames,theValues);
        break;
      }
      case ServiceNodeId: {
        YACSGui_ServiceNodePage* page = dynamic_cast<YACSGui_ServiceNodePage*>(myPages[anId]);
        if(page && page->getNode() == aNode)
          page->notifyOutPortValues(theOutPortsNames,theValues);
        break;
      }
      case ForLoopNodeId: {
        YACSGui_ForLoopNodePage* page = dynamic_cast<YACSGui_ForLoopNodePage*>(myPages[anId]);
        if(page && page->getNode() == aNode)
          page->notifyOutPortValues(theOutPortsNames,theValues);
        break;
      }
      case ForEachLoopNodeId: {
        YACSGui_ForEachLoopNodePage* page = dynamic_cast<YACSGui_ForEachLoopNodePage*>(myPages[anId]);
        if(page && page->getNode() == aNode)
          page->notifyOutPortValues(theOutPortsNames,theValues);
        break;
      }
      case WhileLoopNodeId: {
        YACSGui_WhileLoopNodePage* page = dynamic_cast<YACSGui_WhileLoopNodePage*>(myPages[anId]);
        if(page && page->getNode() == aNode)
          page->notifyOutPortValues(theOutPortsNames,theValues);
        break;
      }
      case SwitchNodeId: {
        YACSGui_SwitchNodePage* page = dynamic_cast<YACSGui_SwitchNodePage*>(myPages[anId]);
        if(page && page->getNode() == aNode)
          page->notifyOutPortValues(theOutPortsNames,theValues);
        break;
      }
      case BlockNodeId: {
        YACSGui_BlockNodePage* page = dynamic_cast<YACSGui_BlockNodePage*>(myPages[anId]);
        if(page && page->getNode() == aNode)
          page->notifyOutPortValues(theOutPortsNames,theValues);
        break;
      }
      default:
        break;
      }
    }
  }
}

//=======================================================================
// name    : updateState
// Purpose : 
//=======================================================================
void YACSGui_InputPanel::updateState()
{
  QMap< int, QWidget* >::const_iterator anIter;
  for ( anIter = myPages.begin(); anIter != myPages.end(); ++anIter )
  {
    int anId = anIter.key();
    QPushButton* b = myMainBtns.contains( anId ) ? myMainBtns[ anId ] : 0;
    QWidget* page  = myPages.contains( anId ) ? myPages[ anId ] : 0;
    if ( b && page )
      page->setShown( b->isVisibleTo( b->parentWidget() ) && b->isOn() );
  }
}

//=======================================================================
// name    : createPage
// Purpose : 
//=======================================================================
void YACSGui_InputPanel::createPage( const int thePageId )
{
  // Create buttons and frames inside the main frame
  if ( !myMainLayout ) return;
  QFrame* aMainFrame = dynamic_cast<QFrame*>( myMainLayout->parent() );
  if ( !aMainFrame ) return;

  // Create a drop down button
  QPushButton* button = new QPushButton( aMainFrame );
    
  QWidget* page = 0;
  switch ( thePageId )
  {
  case ContainerId:
    page = new YACSGui_ContainerPage( aMainFrame );
    break;
  case ComponentId:
    page = new YACSGui_ComponentPage( aMainFrame );
    break;
  case SchemaId:
    page = new YACSGui_SchemaPage( aMainFrame );
    break;
  case InlineNodeId:
    page = new YACSGui_InlineNodePage( aMainFrame );
    break;
  case ServiceNodeId:
    page = new YACSGui_ServiceNodePage( aMainFrame );
    break;
  case ForLoopNodeId:
    page = new YACSGui_ForLoopNodePage( aMainFrame );
    break;
  case ForEachLoopNodeId:
    page = new YACSGui_ForEachLoopNodePage( aMainFrame );
    break;
  case WhileLoopNodeId:
    page = new YACSGui_WhileLoopNodePage( aMainFrame );
    break;
  case SwitchNodeId:
    page = new YACSGui_SwitchNodePage( aMainFrame );
    break;
  case BlockNodeId:
    page = new YACSGui_BlockNodePage( aMainFrame );
    break;
  case LinkId:
    break;
  default:
    break;
  }

  // Use only if the drop down button is created for each property page
  if ( !page )
  {
    delete button;
    return;
  }

  myMainLayout->insertWidget( -1, button );
  myMainLayout->insertWidget( -1, page );

  myMainBtns.insert( thePageId, button );
  myPages.insert( thePageId, page );
    
  // Set custom style for drawing buttons with text aligned to the left border
  static BtnStyle aStyle;
  button->setStyle( &aStyle );

  QFont aFont = button->font();
  aFont.setBold( true );
  button->setFont( aFont );
  button->setPaletteForegroundColor( QColor( 0, 0, 0 ) );
  button->setToggleButton( true );
  button->setOn(true);
  
  // Set titles for property pages as the text on the drop down button
  switch ( thePageId )
  {
  case ContainerId:
    myMainBtns[ContainerId]->setText(tr( "Container Properties" ));
    break;
  case ComponentId:
    myMainBtns[ComponentId]->setText(tr( "Component Properties" ));
    break;
  case SchemaId:
    myMainBtns[SchemaId]->setText( tr( "Schema Properties" ) );
    break;
  case InlineNodeId:
  case ServiceNodeId:
  case ForLoopNodeId:
  case ForEachLoopNodeId:
  case WhileLoopNodeId:
  case SwitchNodeId:
  case BlockNodeId:
    myMainBtns[thePageId]->setText( tr( "Node Properties" ) );
    break;
  case LinkId:
    break;
  default:
    break;
  }
  
  // Connect signals and slots
  if ( myMainBtns.contains( thePageId ) )
    connect( myMainBtns[ thePageId ], SIGNAL( toggled( bool ) ), SLOT( onMainBtn( bool ) ) );
}

/*
  Class : YACSGui_PropertyPage_InputPage
  Description : This class is used for parameters of input panel.
                All pages of input panel must be derived from it
*/

YACSGui_PropertyPage::YACSGui_PropertyPage( QWidget* theParent, YACSGui_Module* theModule )
  : QFrame( theParent ),
    myModule(theModule),
    myView(0)
{
  setLineWidth(2);
  setFrameStyle(QFrame::StyledPanel|QFrame::Raised);
}

YACSGui_PropertyPage::~YACSGui_PropertyPage()
{
}

void YACSGui_PropertyPage::onApply()
{
}

/*
  Class : YACSGui_ContainerPage
  Description :  Page for container properties
*/

YACSGui_ContainerPage::YACSGui_ContainerPage( QWidget* theParent, const char* theName, WFlags theFlags )
  : ContainerPage( theParent, theName, theFlags ),
    GuiObserver(),
    mySContainer( 0 )
{
}

YACSGui_ContainerPage::~YACSGui_ContainerPage()
{
  if ( mySContainer ) mySContainer->detach(this);
}

void YACSGui_ContainerPage::select( bool isSelected )
{
  printf(">> YACSGui_ContainerPage::select\n");
}

void YACSGui_ContainerPage::update( YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son)
{
  printf(">> YACSGui_ContainerPage::update\n");
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
}

YACS::ENGINE::Container* YACSGui_ContainerPage::getContainer() const
{
  return ( mySContainer ? mySContainer->getContainer() : 0 );
}

YACS::ENGINE::SalomeContainer* YACSGui_ContainerPage::getSalomeContainer() const
{
  return ( mySContainer ? dynamic_cast<SalomeContainer*>(mySContainer->getContainer()) : 0 );
}

QString YACSGui_ContainerPage::getContainerName() const
{
  return ( mySContainer ? QString( mySContainer->getName() ) : QString("") );
}

void YACSGui_ContainerPage::setContainerName( const QString& theName )
{
  if ( getContainer() )
  {
    getContainer()->setName( theName.latin1() );
    if ( getSalomeContainer() )
      getSalomeContainer()->setProperty( "container_name", theName.latin1() );
    mySContainer->update( RENAME, 0, mySContainer );
  }
}

YACSGui_InputPanel* YACSGui_ContainerPage::getInputPanel() const
{
  return dynamic_cast<YACSGui_InputPanel*>( parent()->parent()->parent()->parent() );
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
    return ( !aHN.compare(QString("")) ? QString("localhost") : aHN );
  }
  else 
    return QString("localhost");
}

void YACSGui_ContainerPage::setHostName( const QString& theHostName )
{
  if ( getSalomeContainer() )
    getSalomeContainer()->setProperty( "hostname", 
				       !theHostName.compare(QString("localhost")) ? QString("").latin1() :
				                                                    theHostName.latin1() );
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

void YACSGui_ContainerPage::updateState()
{
  // Set container name
  if ( myContainerName )
    myContainerName->setText(getContainerName());

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
  if ( myContainerName )
    setContainerName(myContainerName->text().latin1());

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

/*
  Class : YACSGui_ComponentPage
  Description :  Page for component properties
*/

YACSGui_ComponentPage::YACSGui_ComponentPage( QWidget* theParent, const char* theName, WFlags theFlags )
  : ComponentPage( theParent, theName, theFlags ),
    GuiObserver(),
    mySComponent( 0 ),
    myType( SALOME )
{
  connect( myContainerName, SIGNAL( activated( const QString& ) ),
	   this, SLOT( onContainerChanged( const QString& ) ) );
}

YACSGui_ComponentPage::~YACSGui_ComponentPage()
{
  if ( mySComponent ) mySComponent->detach(this);
}

void YACSGui_ComponentPage::select( bool isSelected )
{
  printf(">> YACSGui_ComponentPage::select\n");
}

void YACSGui_ComponentPage::update( YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son)
{
  printf(">> YACSGui_ComponentPage::update\n");
}

YACSGui_InputPanel* YACSGui_ComponentPage::getInputPanel() const
{
  return dynamic_cast<YACSGui_InputPanel*>( parent()->parent()->parent()->parent() );
}

void YACSGui_ComponentPage::setSComponent( YACS::HMI::SubjectComponent* theSComponent )
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
    updateState();
  }
}

YACS::ENGINE::ComponentInstance* YACSGui_ComponentPage::getComponent() const
{
  return ( mySComponent ? mySComponent->getComponent() : 0 );
}

QString YACSGui_ComponentPage::getComponentName() const
{
  return ( mySComponent ? QString( mySComponent->getName() ) : QString("") );
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

void YACSGui_ComponentPage::onApply()
{
  setContainer();

  if ( mySComponent ) mySComponent->update( EDIT, 0, mySComponent );
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
  // Set component name
  if ( myComponentName )
    myComponentName->setText( getComponentName() );
  
  // Set component type
  setComponentType();

  // Set container
  fillContainerNames();
  if ( myContainerName )
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
}

YACSGui_SchemaPage::~YACSGui_SchemaPage()
{
  if ( mySProc ) mySProc->detach(this);
}

void YACSGui_SchemaPage::select( bool isSelected )
{
  printf(">> YACSGui_SchemaPage::select\n");
}

void YACSGui_SchemaPage::update( YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son)
{
  printf(">> YACSGui_SchemaPage::update\n");
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
      getInputPanel()->getModule()->getDataModel()->updateItem( dynamic_cast<Proc*>(mySProc->getNode()),
								true );
  }
}

void YACSGui_SchemaPage::onApply()
{
  // Rename a schema
  if ( mySchemaName ) setSchemaName( mySchemaName->text() );

  // Change a view mode of a schema
  // ...
}

YACSGui_InputPanel* YACSGui_SchemaPage::getInputPanel() const
{
  return dynamic_cast<YACSGui_InputPanel*>( parent()->parent()->parent()->parent() );
}

void YACSGui_SchemaPage::setMode( const YACSGui_InputPanel::PageMode theMode )
{
  myMode = theMode;

  if ( !ViewModeButtonGroup || !NextToRunGroupBox || !mySchemaName ) return;

  if ( myMode == YACSGui_InputPanel::EditMode )
  {
    ViewModeButtonGroup->show();
    NextToRunGroupBox->hide();
    mySchemaName->setReadOnly(false);
  } 
  else if ( myMode == YACSGui_InputPanel::RunMode )
  {
    ViewModeButtonGroup->hide();
    NextToRunGroupBox->show();
    mySchemaName->setReadOnly(true);
  }

  if ( mySchemaName ) mySchemaName->setText( getSchemaName() );
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
  if ( YACSGui_Executor* anExecutor = getInputPanel()->getModule()->findExecutor() )
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
  if ( YACSGui_Executor* anExecutor = getInputPanel()->getModule()->findExecutor() )
    anExecutor->setNextStepList(nextStepList);
}

void YACSGui_SchemaPage::onNextStepClicked( QListViewItem* theItem )
{
  list<string> nextStepList;
  QListViewItemIterator it(myNextStepsListView);
  for ( ; it.current(); ++it )
    if (((QCheckListItem*)it.current())->isOn())
      nextStepList.push_back(it.current()->text(0));
  if ( YACSGui_Executor* anExecutor = getInputPanel()->getModule()->findExecutor() )
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
  // Set schema name
  if ( mySchemaName ) mySchemaName->setText( getSchemaName() );

  if ( myMode == YACSGui_InputPanel::RunMode && myNextStepsListView )
    myNextStepsListView->clear();
  
  // Set view mode of the given schema
  // the following method can be used if its needed:
  // YACSGui_Graph* aGraph = getInputPanel()->getModule()->getGraph( dynamic_cast<Proc*>(mySProc->getNode()) );
  // ...
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
	if( Loop* aLoop = dynamic_cast<Loop*>( aSNode->getNode() ) )
	{
	  notifyNodeCreateBody( son );
	}
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

void YACSGui_NodePage::setSNode( YACS::HMI::SubjectNode* theSNode )
{
  if ( !theSNode )
    return;

  if ( mySNode != theSNode )
  {
    if ( mySNode ) mySNode->detach(this); //detach from old node
    mySNode = theSNode;
    mySNode->attach(this); // attach to new node

    if ( getNode() )
    {
      notifyNodeStatus( getNode()->getState() );
      notifyNodeProgress();
    }
  }
  updateState();
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

void YACSGui_NodePage::updateState()
{
}

void YACSGui_NodePage::updateBlocSize()
{
  if ( SubjectBloc* aSB = dynamic_cast<SubjectBloc*>(mySNode->getParent()) )
    aSB->update( EDIT, 0, aSB );
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

/*
  Class : YACSGui_InlineNodePage
  Description :  Page for inline function and inline service nodes properties
*/

YACSGui_InlineNodePage::YACSGui_InlineNodePage( QWidget* theParent, const char* theName, WFlags theFlags )
  : InlineNodePage( theParent, theName, theFlags ),
    YACSGui_NodePage(),
    myIsNeedToReorder(false)
{
  if ( !myInputPortsGroupBox || !myOutputPortsGroupBox ) return;

  QString aPortTypes = QString("Data Flow"); //";Data Stream (BASIC);Data Stream (CALCIUM);Data Stream (PALM)");
  QString aValueTypes = QString("Double;Int;String;Bool;Objref;Sequence;Array;Struct");

  // Input Ports table
  myInputPortsGroupBox->setTitle( tr("Input Ports") );
  myInputPortsGroupBox->HideBtn( YACSGui_PlusMinusGrp::SelectBtn );
  YACSGui_Table* aTable = myInputPortsGroupBox->Table();
  aTable->setFixedHeight( 100 );
  aTable->setNumCols( 4 );
  aTable->horizontalHeader()->setLabel( 0, tr( "Port name" ) );
  aTable->horizontalHeader()->setLabel( 1, tr( "Port type" ) );
  aTable->horizontalHeader()->setLabel( 2, tr( "Value type" ) );
  aTable->horizontalHeader()->setLabel( 3, tr( "Value" ) );
  aTable->setCellType( -1, 0, YACSGui_Table::String );
  aTable->setCellType( -1, 1, YACSGui_Table::Combo );
  aTable->setCellType( -1, 2, YACSGui_Table::Combo );
  //aTable->setCellType( -1, 3, YACSGui_Table::??? ); depends on combo box item choosen in the "Value type" column
  
  aTable->setParams( 0, 1, aPortTypes );
  aTable->setParams( 0, 2, aValueTypes );

  connect( aTable, SIGNAL(valueChanged( int, int )), this, SLOT(onValueChanged( int, int )) );
  
  myInputPortsGroupBox->EnableBtn( YACSGui_PlusMinusGrp::AllBtn );
  myInputPortsGroupBox->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );

  connect( myInputPortsGroupBox, SIGNAL(Inserted( const int )),  this, SLOT(onInserted( const int )) );
  connect( myInputPortsGroupBox, SIGNAL(MovedUp( const int )),   this, SLOT(onMovedUp( const int )) );
  connect( myInputPortsGroupBox, SIGNAL(MovedDown( const int )), this, SLOT(onMovedDown( const int )) );
  connect( myInputPortsGroupBox, SIGNAL(Removed( const int )),   this, SLOT(onRemoved( const int )) );

  // Output Ports table  
  myOutputPortsGroupBox->setTitle( tr("Output Ports") );
  myOutputPortsGroupBox->HideBtn( YACSGui_PlusMinusGrp::SelectBtn );
  aTable = myOutputPortsGroupBox->Table();
  aTable->setFixedHeight( 100 );
  aTable->setNumCols( 5 );
  aTable->horizontalHeader()->setLabel( 0, tr( "Port name" ) );
  aTable->horizontalHeader()->setLabel( 1, tr( "Port type" ) );
  aTable->horizontalHeader()->setLabel( 2, tr( "Value type" ) );
  aTable->horizontalHeader()->setLabel( 3, tr( "Value" ) );
  aTable->horizontalHeader()->setLabel( 4, tr( "Is in study" ) );
  aTable->setCellType( -1, 0, YACSGui_Table::String );
  aTable->setCellType( -1, 1, YACSGui_Table::Combo );
  aTable->setCellType( -1, 2, YACSGui_Table::Combo );
  //aTable->setCellType( -1, 3, YACSGui_Table::??? ); depends on combo box item choosen in the "Value type" column
  aTable->setCellType( -1, 4, YACSGui_Table::Combo );

  aTable->setParams( 0, 1, aPortTypes );
  aTable->setParams( 0, 2, aValueTypes );
  aTable->setParams( 0, 4, QString("Yes;No") );
  aTable->setDefValue( 0, 4, QString("Yes") );

  connect( aTable, SIGNAL(valueChanged( int, int )), this, SLOT(onValueChanged( int, int )) );

  myOutputPortsGroupBox->EnableBtn( YACSGui_PlusMinusGrp::AllBtn );
  myOutputPortsGroupBox->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );

  connect( myOutputPortsGroupBox, SIGNAL(Inserted( const int )),  this, SLOT(onInserted( const int )) );
  connect( myOutputPortsGroupBox, SIGNAL(MovedUp( const int )),   this, SLOT(onMovedUp( const int )) );
  connect( myOutputPortsGroupBox, SIGNAL(MovedDown( const int )), this, SLOT(onMovedDown( const int )) );
  connect( myOutputPortsGroupBox, SIGNAL(Removed( const int )), this, SLOT(onRemoved( const int )) );

  connect( myNodeName, SIGNAL(textChanged( const QString& )), this, SLOT(onNodeNameChanged( const QString& )) );
  connect( mySearch, SIGNAL(returnPressed()), this, SLOT(onSearch()) );
}

YACSGui_InlineNodePage::~YACSGui_InlineNodePage()
{
}

void YACSGui_InlineNodePage::setSNode( YACS::HMI::SubjectNode* theSNode )
{
  if ( !theSNode ) return;

  if ( dynamic_cast<InlineNode*>( theSNode->getNode() ) )
  {
    YACSGui_NodePage::setSNode( theSNode );
    myIPList.clear();
    myOPList.clear();
  }
}

YACSGui_InputPanel* YACSGui_InlineNodePage::getInputPanel() const
{
  return dynamic_cast<YACSGui_InputPanel*>( parent()->parent()->parent()->parent() );
}

void YACSGui_InlineNodePage::setMode( const YACSGui_InputPanel::PageMode theMode )
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

    myOutputPortsGroupBox->ShowBtn();
    myOutputPortsGroupBox->HideBtn( YACSGui_PlusMinusGrp::SelectBtn );
    aTable = myOutputPortsGroupBox->Table();
    aTable->setReadOnly( -1, 0, false );
    aTable->setReadOnly( -1, 1, false );
    aTable->setReadOnly( -1, 2, false );

    InPythonEditorGroupBox->show();
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

    InPythonEditorGroupBox->hide();
  }
}

void YACSGui_InlineNodePage::notifyNodeStatus( int theStatus )
{
  if ( myNodeState )
  {
    myNodeState->setPaletteForegroundColor(getColor(theStatus));
    myNodeState->setText(getText(theStatus));
  }
}

void YACSGui_InlineNodePage::notifyNodeProgress()
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

void YACSGui_InlineNodePage::notifyInPortValues( std::list<std::string> theInPortsNames,
						 std::list<std::string> theValues )
{
  QStringList aValues;

  QStringList aPortNames;
  myInputPortsGroupBox->Table()->strings( 0, aPortNames );

  int aRowId = 0;
  list<string>::iterator itN = theInPortsNames.begin();
  list<string>::iterator itV = theValues.begin();
  for ( ; itN!=theInPortsNames.end() && itV!=theValues.end(); itN++, itV++ )
  {
    if ( !aPortNames[aRowId].compare(QString(*itN)) )
      aValues.append(QString(*itV));
    else return;
    aRowId++;
  }
  
  myInputPortsGroupBox->Table()->setStrings( 3, aValues );
}

void YACSGui_InlineNodePage::notifyOutPortValues( std::list<std::string> theOutPortsNames,
						  std::list<std::string> theValues )
{
  QStringList aValues;

  QStringList aPortNames;
  myOutputPortsGroupBox->Table()->strings( 0, aPortNames );

  int aRowId = 0;
  list<string>::iterator itN = theOutPortsNames.begin();
  list<string>::iterator itV = theValues.begin();
  for ( ; itN!=theOutPortsNames.end() && itV!=theValues.end(); itN++, itV++ )
  {
    if ( !aPortNames[aRowId].compare(QString(*itN)) )
      aValues.append(QString(*itV));
    else return;
    aRowId++;
  }
  
  myOutputPortsGroupBox->Table()->setStrings( 3, aValues );
}

void YACSGui_InlineNodePage::onApply()
{
  // Rename a node
  if ( myNodeName )
    setNodeName( myNodeName->text() );
  
  bool withFilling = ( myIPList.isEmpty() || myOPList.isEmpty() );

  // Reset the list of input ports
  setInputPorts();
  
  // Reset the list of output ports
  setOutputPorts();

  // Reset Python function/script
  if ( InlineNode* anIN = dynamic_cast<InlineNode*>( getNode() ) )
    anIN->setScript( myTextEdit->text().latin1() );
  if ( InlineFuncNode* anIFN = dynamic_cast<InlineFuncNode*>( getNode() ) )
  {
    QString aText = myTextEdit->text();
    int aFromId = aText.find( QString("\ndef ") ) + QString("\ndef ").length();
    int aToId = aText.find( QString("("), aFromId );
    aText = aText.left( aToId );
    QString aFname = aText.right( aText.length() - aFromId ).stripWhiteSpace();
    anIFN->setFname( aFname.latin1() );
  }

  if ( myIsNeedToReorder )
  {
    orderPorts(withFilling);
    myIsNeedToReorder = false;
  }
  
  resetStoredPortsMaps();
  resetPLists();

  updateBlocSize();
}

void YACSGui_InlineNodePage::updateState()
{
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
  if ( InlineNode* anIN = dynamic_cast<InlineNode*>( getNode() ) )
  {
    myTextEdit->setText( QString(anIN->getScript()) );
  }
}

void YACSGui_InlineNodePage::onNodeNameChanged( const QString& theName )
{
  if ( myNodeFullName )
  {
    QString anOldName = myNodeFullName->text();
    int aRDotId = anOldName.findRev('.');
    myNodeFullName->setText( anOldName.replace( aRDotId+1, anOldName.length()-(aRDotId+1), theName ) );
  }
}

void YACSGui_InlineNodePage::onValueChanged( int theRow, int theCol )
{
  if ( YACSGui_Table* aTable = ( YACSGui_Table* )sender() )
  {
    if ( theCol == 1 ) // the port type was changed
    {
      if( aTable == myInputPortsGroupBox->Table() )
	aTable->setReadOnly( theRow, 3, false );
    }

    if ( theCol == 2 ) // the value type of the port was changed
    {
      QString aVT = aTable->item( theRow, theCol )->text();
      if ( !aVT.compare(QString("Double")) )
	aTable->setCellType( theRow, 3, YACSGui_Table::Double );
      else if ( !aVT.compare(QString("Int")) )
	aTable->setCellType( theRow, 3, YACSGui_Table::Int );
      else if ( !aVT.compare(QString("Bool")) )
      {
	aTable->setCellType( theRow, 3, YACSGui_Table::Combo );
	aTable->setParams( theRow, 3, QString("True;False") );
      }
      else // i.e. "String" or "Objref" or "Sequence" or "Array" or "Struct"
	aTable->setCellType( theRow, 3, YACSGui_Table::String );

      if( aTable == myOutputPortsGroupBox->Table() )
	aTable->setReadOnly( theRow, 3, true );
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

void YACSGui_InlineNodePage::onSearch()
{

}

void YACSGui_InlineNodePage::onInserted( const int theRow )
{
  if ( YACSGui_PlusMinusGrp* aGrpBox = ( YACSGui_PlusMinusGrp* )sender() )
    if ( myInputPortsGroupBox == aGrpBox )
    {
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
      myIsNeedToReorder = true;
    }
    else if ( myOutputPortsGroupBox == aGrpBox )
    {
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
      myIsNeedToReorder = true;
    }
}

void YACSGui_InlineNodePage::onMovedUp( const int theUpRow )
{
  if ( YACSGui_PlusMinusGrp* aGrpBox = ( YACSGui_PlusMinusGrp* )sender() )
    if ( myInputPortsGroupBox == aGrpBox )
    {
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

      myIsNeedToReorder = true;
    }
    else if ( myOutputPortsGroupBox == aGrpBox )
    {
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

      myIsNeedToReorder = true;
    }
}

void YACSGui_InlineNodePage::onMovedDown( const int theDownRow )
{
  if ( YACSGui_PlusMinusGrp* aGrpBox = ( YACSGui_PlusMinusGrp* )sender() )
    if ( myInputPortsGroupBox == aGrpBox )
    {
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

      myIsNeedToReorder = true;
    }
    else if ( myOutputPortsGroupBox == aGrpBox )
    {
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
      
      myIsNeedToReorder = true;
    }
}

void YACSGui_InlineNodePage::onRemoved( const int theRow )
{
  if ( YACSGui_PlusMinusGrp* aGrpBox = ( YACSGui_PlusMinusGrp* )sender() )
    if ( myInputPortsGroupBox == aGrpBox )
    {
      if ( myRow2StoredInPorts.find(theRow) != myRow2StoredInPorts.end() )
      	myRow2StoredInPorts.erase(theRow);
      
      map<int,InPort*>::iterator it = myRow2StoredInPorts.begin();
      for ( ; it != myRow2StoredInPorts.end(); it++ )
      {
	int aRow = (*it).first;
	InPort* aPort = (*it).second;
	
	if ( aRow > theRow )
	{
	  myRow2StoredInPorts.insert( std::make_pair(aRow-1,aPort) );
	  myRow2StoredInPorts.erase(aRow);
	}
      }
    }
    else if ( myOutputPortsGroupBox == aGrpBox )
    {
      if ( myRow2StoredOutPorts.find(theRow) != myRow2StoredOutPorts.end() )
      	myRow2StoredOutPorts.erase(theRow);

      map<int,OutPort*>::iterator it = myRow2StoredOutPorts.begin();
      for ( ; it != myRow2StoredOutPorts.end(); it++ )
      {
	int aRow = (*it).first;
	OutPort* aPort = (*it).second;
	
	if ( aRow > theRow )
	{
	  myRow2StoredOutPorts.insert( std::make_pair(aRow-1,aPort) );
	  myRow2StoredOutPorts.erase(aRow);
	} 
      }
    }
}

void YACSGui_InlineNodePage::fillInputPortsTable()
{
  if ( !getNode() ) return;

  myRow2StoredInPorts.clear();

  QStringList aPortNames;
  QStringList aPortTypes;
  QStringList aValueTypes;
  QStringList aValues;

  QValueList<bool> aReadOnlyFlags;

  YACSGui_Table* aTable = myInputPortsGroupBox->Table();
  QStringList aPTCB = aTable->Params( 0, 1 );

  int aRowId = 0;
  list<InPort*> anInPortsEngine = getNode()->getSetOfInPort();
  list<InPort*>::iterator anInPortsIter = anInPortsEngine.begin();
  for( ;anInPortsIter!=anInPortsEngine.end();anInPortsIter++)
  {
    InPort* anInPort = *anInPortsIter;

    myRow2StoredInPorts.insert( std::make_pair(aRowId,anInPort) );

    // Collect port names in the list
    aPortNames.append( anInPort->getName() );

    // Port type
    if ( InputPort* anInputP = dynamic_cast<InputPort*>(anInPort) )
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

  // Fill "Value" column
  aTable->setStrings( 3, aValues, true );

  // Set "Value" column read only (for linked ports)
  for ( int i = 0, n = aTable->numRows(); i < n; i++ )
    aTable->setReadOnly( i, 3, aReadOnlyFlags[ i ] );

  if ( !aPortNames.empty() ) myInputPortsGroupBox->EnableBtn( YACSGui_PlusMinusGrp::AllBtn );
}

void YACSGui_InlineNodePage::fillOutputPortsTable()
{
  if ( !getNode() ) return;

  myRow2StoredOutPorts.clear();

  QStringList aPortNames;
  QStringList aPortTypes;
  QStringList aValueTypes;
  QStringList aValues;
  QStringList anIsInStudy;
  
  YACSGui_Table* aTable = myOutputPortsGroupBox->Table();
  QStringList aPTCB = aTable->Params( 0, 1 );

  int aRowId = 0;
  list<OutPort*> anOutPortsEngine = getNode()->getSetOfOutPort();
  list<OutPort*>::iterator anOutPortsIter = anOutPortsEngine.begin();
  for( ;anOutPortsIter!=anOutPortsEngine.end();anOutPortsIter++)
  {
    OutPort* anOutPort = *anOutPortsIter;

    myRow2StoredOutPorts.insert( std::make_pair(aRowId,anOutPort) );

    // Collect port names in the list
    aPortNames.append( anOutPort->getName() );
    
    // Port type
    if ( OutputPort* anOutputP = dynamic_cast<OutputPort*>(anOutPort) )
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

  // Fill "Value" column
  aTable->setStrings( 3, aValues, true );

  // Fill "Is in study" column
  aTable->setStrings( 4, anIsInStudy, true );

  // Set "Value" column read only
  aTable->setReadOnly( -1, 3, true );

  if ( !aPortNames.empty() ) myOutputPortsGroupBox->EnableBtn( YACSGui_PlusMinusGrp::AllBtn );
}

QString YACSGui_InlineNodePage::getPortType( YACS::ENGINE::Port* thePort ) const
{
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
    if (aDType != NONE)
      aType = aVTCB[aDType-1];
  }
  
  return aType;
}

bool YACSGui_InlineNodePage::isStored( YACS::ENGINE::Port* thePort )
{
  if ( InPort* anIP = dynamic_cast<InPort*>(thePort) )
  {
    map<int,InPort*>::iterator it = myRow2StoredInPorts.begin();
    for ( ; it != myRow2StoredInPorts.end(); it++ )
      if ( (*it).second == thePort ) return true;
    return false;
  }
  else if ( OutPort* anOP = dynamic_cast<OutPort*>(thePort) )
  {
    map<int,OutPort*>::iterator it = myRow2StoredOutPorts.begin();
    for ( ; it != myRow2StoredOutPorts.end(); it++ )
      if ( (*it).second == thePort ) return true;
    return false;
  }
  return true; // the gate ports are kept (not re-create)
}

void YACSGui_InlineNodePage::resetStoredPortsMaps()
{
  myRow2StoredInPorts.clear();

  QStringList aIPortNames;
  myInputPortsGroupBox->Table()->strings( 0, aIPortNames );

  int aRowId = 0;
  for ( QStringList::Iterator it = aIPortNames.begin(); it != aIPortNames.end(); ++it )
  {
    try {
      if ( InputPort* aIDP = getNode()->getInputPort((*it).latin1()) )
	myRow2StoredInPorts.insert( std::make_pair(aRowId,aIDP) );
    }
    catch (YACS::Exception& ex) {
      if ( InputDataStreamPort* aIDSP = getNode()->getInputDataStreamPort((*it).latin1()) )
	myRow2StoredInPorts.insert( std::make_pair(aRowId,aIDSP) );
    }
    aRowId++;
  }

  myRow2StoredOutPorts.clear();

  QStringList aOPortNames;
  myOutputPortsGroupBox->Table()->strings( 0, aOPortNames );

  aRowId = 0;
  for ( QStringList::Iterator it = aOPortNames.begin(); it != aOPortNames.end(); ++it )
  {
    try {
      if ( OutputPort* aODP = getNode()->getOutputPort((*it).latin1()) )
	myRow2StoredOutPorts.insert( std::make_pair(aRowId,aODP) );
    }
    catch (YACS::Exception& ex) {
      if ( OutputDataStreamPort* aODSP = getNode()->getOutputDataStreamPort((*it).latin1()) )
	myRow2StoredOutPorts.insert( std::make_pair(aRowId,aODSP) );
    }
    aRowId++;
  }
}

void YACSGui_InlineNodePage::resetPLists()
{
  resetIPLists();
  resetOPLists();
}

void YACSGui_InlineNodePage::resetIPLists()
{
  myIPList.clear();

  QStringList aIPortNames;
  myInputPortsGroupBox->Table()->strings( 0, aIPortNames );

  for ( QStringList::Iterator it = aIPortNames.begin(); it != aIPortNames.end(); ++it )
    try {
      if ( InPort* aIDP = getNode()->getInputPort((*it).latin1()) )
	myIPList.append(aIDP);
    }
    catch (YACS::Exception& ex) {
      if ( InPort* aIDSP = getNode()->getInputDataStreamPort((*it).latin1()) )
	myIPList.append(aIDSP);
    }
}

void YACSGui_InlineNodePage::resetOPLists()
{
  myOPList.clear();

  QStringList aOPortNames;
  myOutputPortsGroupBox->Table()->strings( 0, aOPortNames );

  for ( QStringList::Iterator it = aOPortNames.begin(); it != aOPortNames.end(); ++it )
    try {
      if ( OutPort* aODP = getNode()->getOutputPort((*it).latin1()) )
	myOPList.append(aODP);
    }
    catch (YACS::Exception& ex) {
      if ( OutPort* aODSP = getNode()->getOutputDataStreamPort((*it).latin1()) )
	myOPList.append(aODSP);
    }
}

void YACSGui_InlineNodePage::orderPorts( bool withFilling )
{
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

void YACSGui_InlineNodePage::setInputPorts()
{
  if ( !myInputPortsGroupBox ) return;

  ElementaryNode* aNode = dynamic_cast<ElementaryNode*>( getNode() );
  if ( !aNode ) return;

  // remove old ports
  list<InPort*> anInPortsEngine = aNode->getSetOfInPort();
  list<InPort*>::iterator anInPortsIter = anInPortsEngine.begin();
  for( ;anInPortsIter!=anInPortsEngine.end();anInPortsIter++)
    if ( !isStored(*anInPortsIter) )
    {
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
  for ( QStringList::Iterator it = aPortNames.begin(); it != aPortNames.end(); ++it )
  {
    if ( aPortNames.contains(*it) > 1 )
    {
      std::string what("The node ");
      what += mySNode->getName();
      what += " contains two or more "; 
      what+=(*it).latin1();
      what+=" input ports";
      throw Exception(what);
    }
    
    // retrieve a type name of the input port as a string
    std::string aType = "";
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
    
    TypeCode* aTC = 0;
    if ( aType.empty() )
    {
      aTC = createTypeCode( DynType(aTable->intValueCombo( 2, aRowId ) + 1), aTable, aRowId );
      if ( !aTC ) return;
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
	if ( aType.empty() )
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
	// initialize new created input data port
	switch ( aTC->kind() )
	{
	case Double:
	  aIDP->edInit( aValues[aRowId].toDouble() );
	  break;
	case Int:
	  aIDP->edInit( aValues[aRowId].toInt() );
	  break;
	case String:
	  aIDP->edInit( aValues[aRowId].latin1() );
	  break;
	case Bool:
	  aIDP->edInit( aValues[aRowId].compare( aTable->Params(aRowId,3)[0] ) ? false : true );
	  break;
	case Objref:
	  aIDP->edInit( "" ); // TODO : create an Any* with corresponding type and initialize with it
	  break;
	case Sequence:
	  aIDP->edInit( "" ); // TODO : create an Any* (i.e. SequenceAny*) with corresponding type and initialize with it
	  break;
	case Array:
	  aIDP->edInit( "" ); // TODO : create an Any* (i.e. ArrayAny*) with corresponding type and initialize with it
	  break;
	case Struct:
	  aIDP->edInit( "" ); // TODO : create an Any* with corresponding type and initialize with it
	  break;
	default:
	  break;
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
	if ( aIDSP = dynamic_cast<InputDataStreamPort*>(myRow2StoredInPorts[aRowId]) )
	{
	  aIDSP->setName(*it);
	  aTC = aIDSP->edGetType();
	  // EDIT event will be emitted => only update port prs
	}
      }
      else
      { // ---- create port ----
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
    
    aRowId++;
  }

  //resetIPLists();
}

void YACSGui_InlineNodePage::setOutputPorts()
{
  if ( !myOutputPortsGroupBox ) return;

  ElementaryNode* aNode = dynamic_cast<ElementaryNode*>( getNode() );
  if ( !aNode ) return;

  // remove old ports
  list<OutPort*> anOutPortsEngine = aNode->getSetOfOutPort();
  list<OutPort*>::iterator anOutPortsIter = anOutPortsEngine.begin();
  for( ;anOutPortsIter!=anOutPortsEngine.end();anOutPortsIter++)
    if ( !isStored(*anOutPortsIter) )
    {
      mySNode->update( REMOVE,
		       ( dynamic_cast<OutputPort*>(*anOutPortsIter) ? OUTPUTPORT : OUTPUTDATASTREAMPORT ),
		       GuiContext::getCurrent()->_mapOfSubjectDataPort[*anOutPortsIter] );
      myOPList.take(myOPList.find(*anOutPortsIter));

      //aNode->edRemovePort(*anOutPortsIter);
      // remove port subject (and all subject links connected to this port)
      mySNode->destroy( GuiContext::getCurrent()->_mapOfSubjectDataPort[*anOutPortsIter] );
    }
  
  // read output data and output data stream ports from the table
  YACSGui_Table* aTable = myOutputPortsGroupBox->Table();
  
  QStringList aPortNames;
  QStringList aValues;

  aTable->strings( 0, aPortNames );

  YACS::ENGINE::Catalog* aCatalog = YACS::ENGINE::getSALOMERuntime()->getBuiltinCatalog();

  int aRowId = 0;
  for ( QStringList::Iterator it = aPortNames.begin(); it != aPortNames.end(); ++it )
  {
    if ( aPortNames.contains(*it) > 1 )
    {
      std::string what("The node ");
      what += mySNode->getName();
      what += " contains two or more "; 
      what+=(*it).latin1();
      what+=" output ports";
      throw Exception(what);
    }
    
    // retrieve a type name of the input port as a string
    std::string aType = "";
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
    
    TypeCode* aTC = 0;
    if ( aType.empty() )
    {
      aTC = createTypeCode( DynType(aTable->intValueCombo( 2, aRowId ) + 1), aTable, aRowId );
      if ( !aTC ) return;
    }

    GuiEvent anEvent = EDIT;
    if ( aTable->intValueCombo( 1, aRowId ) == 0 )           // Data Flow port
    {
      OutputPort* aODP = 0;
      if ( myRow2StoredOutPorts.find(aRowId) != myRow2StoredOutPorts.end() )
      { // ---- update port ----
	if ( aODP = dynamic_cast<OutputPort*>(myRow2StoredOutPorts[aRowId]) )
	{
	  aODP->setName(*it);
	  aTC = aODP->edGetType();
	  // EDIT event will be emitted => only update port prs
	}
      }
      else
      { // ---- create port ----
	if ( aType.empty() )
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
	  aODP = dynamic_cast<OutputPort*>( dynamic_cast<SubjectElementaryNode*>(mySNode)->addOutputPort( aCatalog, aType, *it )->getPort() );
	  aTC = aODP->edGetType();
	}

	// ADD event will be emitted from hmi after subject creation => re-create port prs
	anEvent = ADD;
	
	myOPList.append(aODP);
      }

      if ( aODP && anEvent == EDIT )
	mySNode->update( EDIT, OUTPUTPORT, GuiContext::getCurrent()->_mapOfSubjectDataPort[aODP]);
      
    }

    //else if ( aTable->intValueCombo( 1, aRowId ) == 1 )    // Data Stream (BASIC) port
    //  aNode->edAddOutputDataStreamPort( *it, aTC );

    else if ( aTable->intValueCombo( 1, aRowId ) == 2 )      // Data Stream (CALCIUM) port
    {
      OutputDataStreamPort* aODSP = 0;
      if ( myRow2StoredOutPorts.find(aRowId) != myRow2StoredOutPorts.end() )
      { // ---- update port ----
	if ( aODSP = dynamic_cast<OutputDataStreamPort*>(myRow2StoredOutPorts[aRowId]) )
	{
	  aODSP->setName(*it);
	  aTC = aODSP->edGetType();
	  // EDIT event will be emitted => only update port prs
	}
      }
      else
      { // ---- create port ----
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

    aRowId++;
  }

  //resetOPLists();
}

YACS::ENGINE::TypeCode* YACSGui_InlineNodePage::createTypeCode( YACS::ENGINE::DynType theType,
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

/*
  Class : YACSGui_ServiceNodePage
  Description :  Page for SALOME and CORBA service nodes properties
*/

YACSGui_ServiceNodePage::YACSGui_ServiceNodePage( QWidget* theParent, const char* theName, WFlags theFlags )
  : ServiceNodePage( theParent, theName, theFlags ),
    YACSGui_NodePage(),
    myComponent( 0 ),
    myMethodChanged( false )
{
  connect( myMethodName, SIGNAL( activated( const QString& ) ),
	   this, SLOT( onMethodChanged( const QString& ) ) );

  if ( !myInputPortsGroupBox || !myOutputPortsGroupBox ) return;

  QString aPortTypes = QString("Data Flow;Data Stream (BASIC);Data Stream (CALCIUM);Data Stream (PALM)");
  QString aValueTypes = QString("Double;Int;String;Bool;Objref;Sequence;Array;Struct");

  // Input Ports table
  myInputPortsGroupBox->setTitle( tr("Input Ports") );
  YACSGui_Table* aTable = myInputPortsGroupBox->Table();
  aTable->setFixedHeight( 100 );
  aTable->setNumCols( 4 );
  aTable->horizontalHeader()->setLabel( 0, tr( "Port name" ) );
  aTable->horizontalHeader()->setLabel( 1, tr( "Port type" ) );
  aTable->horizontalHeader()->setLabel( 2, tr( "Value type" ) );
  aTable->horizontalHeader()->setLabel( 3, tr( "Value" ) );
  aTable->setCellType( -1, 0, YACSGui_Table::String );
  aTable->setCellType( -1, 1, YACSGui_Table::Combo );
  aTable->setCellType( -1, 2, YACSGui_Table::Combo );
  //aTable->setCellType( -1, 3, YACSGui_Table::??? ); depends on combo box item choosen in the "Value type" column

  aTable->setParams( 0, 1, aPortTypes );
  aTable->setParams( 0, 2, aValueTypes );

  connect( aTable, SIGNAL(valueChanged( int, int )), this, SLOT(onValueChanged( int, int )) );
  
  myInputPortsGroupBox->HideBtn( YACSGui_PlusMinusGrp::PlusBtn | YACSGui_PlusMinusGrp::MinusBtn |
				 YACSGui_PlusMinusGrp::InsertBtn | YACSGui_PlusMinusGrp::SelectBtn );
  myInputPortsGroupBox->EnableBtn( YACSGui_PlusMinusGrp::UpBtn | YACSGui_PlusMinusGrp::DownBtn );
  myInputPortsGroupBox->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );

  // Output Ports table  
  myOutputPortsGroupBox->setTitle( tr("Output Ports") );
  aTable = myOutputPortsGroupBox->Table();
  aTable->setFixedHeight( 100 );
  aTable->setNumCols( 5 );
  aTable->horizontalHeader()->setLabel( 0, tr( "Port name" ) );
  aTable->horizontalHeader()->setLabel( 1, tr( "Port type" ) );
  aTable->horizontalHeader()->setLabel( 2, tr( "Value type" ) );
  aTable->horizontalHeader()->setLabel( 3, tr( "Value" ) );
  aTable->horizontalHeader()->setLabel( 4, tr( "Is in study" ) );
  aTable->setCellType( -1, 0, YACSGui_Table::String );
  aTable->setCellType( -1, 1, YACSGui_Table::Combo );
  aTable->setCellType( -1, 2, YACSGui_Table::Combo );
  //aTable->setCellType( -1, 3, YACSGui_Table::??? ); depends on combo box item choosen in the "Value type" column
  aTable->setCellType( -1, 4, YACSGui_Table::Combo );

  aTable->setParams( 0, 1, aPortTypes );
  aTable->setParams( 0, 2, aValueTypes );
  aTable->setParams( 0, 4, QString("Yes;No") );
  aTable->setDefValue( 0, 4, QString("Yes") );

  connect( aTable, SIGNAL(valueChanged( int, int )), this, SLOT(onValueChanged( int, int )) );

  myOutputPortsGroupBox->HideBtn( YACSGui_PlusMinusGrp::PlusBtn | YACSGui_PlusMinusGrp::MinusBtn |
				  YACSGui_PlusMinusGrp::InsertBtn | YACSGui_PlusMinusGrp::SelectBtn );
  myOutputPortsGroupBox->EnableBtn( YACSGui_PlusMinusGrp::UpBtn | YACSGui_PlusMinusGrp::DownBtn );
  myOutputPortsGroupBox->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );

  connect( myNodeName, SIGNAL(textChanged( const QString& )), this, SLOT(onNodeNameChanged( const QString& )) );

  //connect( this, SIGNAL(visibilityChanged( bool )), this, SLOT(onVisibilityChanged( bool )) );
}

YACSGui_ServiceNodePage::~YACSGui_ServiceNodePage()
{
}

void YACSGui_ServiceNodePage::setSNode( YACS::HMI::SubjectNode* theSNode )
{
  if ( !theSNode ) return;

  if ( YACS::ENGINE::ServiceNode* aServiceNode = dynamic_cast<ServiceNode*>( theSNode->getNode() ) )
  {
    myComponentName = QString::null;
    myComponent = aServiceNode->getComponent();
    YACSGui_NodePage::setSNode( theSNode );
  }
}

void YACSGui_ServiceNodePage::setComponent( YACS::ENGINE::ComponentInstance* theComponent )
{
  if ( !theComponent ) return;

  std::string aComponentName = theComponent->getName();
  if( myComponentName != aComponentName )
  {
    myComponentName = aComponentName;
    myComponent = theComponent;
    updateState();

    YACSGui_ComponentPage* aCompPage = 
      dynamic_cast<YACSGui_ComponentPage*>( getInputPanel()->getPage( YACSGui_InputPanel::ComponentId ) );
    if ( aCompPage )
      aCompPage->setSComponent( GuiContext::getCurrent()->_mapOfSubjectComponent[theComponent] );
  }

  if( aComponentName != "" && myComponentButton && myComponentButton->isOn() )
    myComponentButton->setOn( false );
}

bool YACSGui_ServiceNodePage::isSelectComponent() const
{
  return ( myComponentButton ? myComponentButton->isOn() : false );
}

YACSGui_InputPanel* YACSGui_ServiceNodePage::getInputPanel() const
{
  SalomeApp_Application* anApp = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
  if( !anApp )
    return 0;

  YACSGui_Module* aModule = dynamic_cast<YACSGui_Module*>( anApp->activeModule() );
  if( !aModule )
    return 0;

  return aModule->getInputPanel();
}

void YACSGui_ServiceNodePage::setMode( const YACSGui_InputPanel::PageMode theMode )
{
  YACSGui_NodePage::setMode(theMode);

  if ( myMode == YACSGui_InputPanel::EditMode )
  {
    myNodeName->setReadOnly(false);
    myComponentButton->show();
    myMethodName->setEnabled(true);

    ExecutionGroupBox->hide();

    myInputPortsGroupBox->ShowBtn( YACSGui_PlusMinusGrp::UpBtn | YACSGui_PlusMinusGrp::DownBtn );
    myOutputPortsGroupBox->ShowBtn( YACSGui_PlusMinusGrp::UpBtn | YACSGui_PlusMinusGrp::DownBtn );
  } 
  else if ( myMode == YACSGui_InputPanel::RunMode )
  {
    myNodeName->setReadOnly(true);
    myComponentButton->hide();
    myMethodName->setEnabled(false);

    ExecutionGroupBox->show();

    myInputPortsGroupBox->HideBtn();
    myOutputPortsGroupBox->HideBtn();
  }
}

void YACSGui_ServiceNodePage::notifyNodeStatus( int theStatus )
{
  if ( myNodeState )
  {
    myNodeState->setPaletteForegroundColor(getColor(theStatus));
    myNodeState->setText(getText(theStatus));
  }
}

void YACSGui_ServiceNodePage::notifyNodeProgress()
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

void YACSGui_ServiceNodePage::notifyInPortValues( std::list<std::string> theInPortsNames,
						  std::list<std::string> theValues )
{
  QStringList aValues;

  QStringList aPortNames;
  myInputPortsGroupBox->Table()->strings( 0, aPortNames );

  int aRowId = 0;
  list<string>::iterator itN = theInPortsNames.begin();
  list<string>::iterator itV = theValues.begin();
  for ( ; itN!=theInPortsNames.end() && itV!=theValues.end(); itN++, itV++ )
  {
    if ( !aPortNames[aRowId].compare(QString(*itN)) )
      aValues.append(QString(*itV));
    else return;
    aRowId++;
  }
  
  myInputPortsGroupBox->Table()->setStrings( 3, aValues );
}

void YACSGui_ServiceNodePage::notifyOutPortValues( std::list<std::string> theOutPortsNames,
						   std::list<std::string> theValues )
{
  QStringList aValues;

  QStringList aPortNames;
  myOutputPortsGroupBox->Table()->strings( 0, aPortNames );

  int aRowId = 0;
  list<string>::iterator itN = theOutPortsNames.begin();
  list<string>::iterator itV = theValues.begin();
  for ( ; itN!=theOutPortsNames.end() && itV!=theValues.end(); itN++, itV++ )
  {
    if ( !aPortNames[aRowId].compare(QString(*itN)) )
      aValues.append(QString(*itV));
    else return;
    aRowId++;
  }
  
  myOutputPortsGroupBox->Table()->setStrings( 3, aValues );
}

void YACSGui_ServiceNodePage::onApply()
{
  // Rename a node
  if ( myNodeName )
    setNodeName( myNodeName->text() );

  // Reset method
  if ( myMethodName )
  {
    if ( ServiceNode* aServiceNode = dynamic_cast<ServiceNode*>( getNode() ) )
    {
      //printf( "Component - %s\n", myComponent->getName().c_str() );
      //printf( "Method - %s\n", myMethodName->currentText().latin1() );

      SubjectComponent* aPrevSComponent = 0;
      if ( ComponentInstance* aPrevComponent = aServiceNode->getComponent() )
	aPrevSComponent = GuiContext::getCurrent()->_mapOfSubjectComponent[aPrevComponent];

      if ( !aServiceNode->getComponent() )
      {
      	SubjectServiceNode* aSSNode = dynamic_cast<SubjectServiceNode*>(mySNode);
	SubjectComponent* aSComp = GuiContext::getCurrent()->_mapOfSubjectComponent[myComponent];
	if ( aSSNode && aSComp ) aSSNode->associateToComponent(aSComp);
      }
      else if ( aServiceNode->getComponent() != myComponent )
      {
	aServiceNode->setComponent( myComponent );
	if ( aPrevSComponent )
	  aPrevSComponent->update( EDIT, REFERENCE, mySNode );
      }
      
      if ( aServiceNode->getComponent()
	   &&
	   myMethodName->currentText().compare(QString(aServiceNode->getMethod())) != 0
	   &&
	   !myMethodName->currentText().isEmpty() )
      {
	aServiceNode->setMethod( myMethodName->currentText().latin1() );
	myMethodChanged = true;
      }
    }
  }

  // Reset the list of input ports
  setInputPorts();
  
  // Reset the list of output ports
  setOutputPorts();

  if ( myMethodChanged ) myMethodChanged = false;

  // Reset Python function/script
  // ...

  mySNode->update( EDIT, 0, mySNode );

  updateBlocSize();
}

void YACSGui_ServiceNodePage::updateState()
{
  // Clear input ports table and output ports tables, if they are not empty
  if ( myInputPortsGroupBox )
  {
    myInputPortsGroupBox->Table()->setNumRows( 0 );
    myInputPortsGroupBox->EnableBtn( YACSGui_PlusMinusGrp::UpBtn | YACSGui_PlusMinusGrp::DownBtn );
  }
  if ( myOutputPortsGroupBox )
  {
    myOutputPortsGroupBox->Table()->setNumRows( 0 );
    myOutputPortsGroupBox->EnableBtn( YACSGui_PlusMinusGrp::UpBtn | YACSGui_PlusMinusGrp::DownBtn );
  }

  // Set node name
  if ( myNodeName ) myNodeName->setText( getNodeName() );
  
  // Set node full name (read only)
  if ( myNodeFullName )
    myNodeFullName->setText( getNode()->getRootNode()->getChildName(getNode()) );
  
  // Set node type: SALOME or CORBA service
  if ( dynamic_cast<SalomeNode*>( getNode() ) )
    myNodeType->setText( tr( "SALOME_SERVICE_NODE" ) ); // SALOME service node
  else if ( dynamic_cast<CORBANode*>( getNode() ) )
    myNodeType->setText( tr( "CORBA_SERVICE_NODE" ) ); // CORBA service node
  //else if ( dynamic_cast<NodeNode*>( getNode() ) )
  //  myNodeType->setText( tr( "NODENODE_SERVICE_NODE" ) ); // node-node service node
  else if ( dynamic_cast<CppNode*>( getNode() ) )
    myNodeType->setText( tr( "CPP_NODE" ) ); // CPP node
  else if ( dynamic_cast<SalomePythonNode*>( getNode() ) )
    myNodeType->setText( tr( "SERVICE_INLINE_NODE" ) ); // service inline node
  else if ( dynamic_cast<XmlNode*>( getNode() ) )
    myNodeType->setText( tr( "XML_NODE" ) ); // XML node

  // Set component instance and services
  if( myComponentDefinition && myMethodName )
    updateServices();

  // Fill the table of input ports of the node
  //fillInputPortsTable( getNode() );
  
  // Fill the table of output ports of the node
  //fillOutputPortsTable( getNode() );
}

void YACSGui_ServiceNodePage::onVisibilityChanged( bool visible )
{
  if( !visible && myComponentButton && myComponentButton->isOn() )
    myComponentButton->setOn( false );
}

void YACSGui_ServiceNodePage::onNodeNameChanged( const QString& theName )
{
  if ( myNodeFullName )
  {
    QString anOldName = myNodeFullName->text();
    int aRDotId = anOldName.findRev('.');
    myNodeFullName->setText( anOldName.replace( aRDotId+1, anOldName.length()-(aRDotId+1), theName ) );
  }
}

void YACSGui_ServiceNodePage::onMethodChanged( const QString& theMethodName )
{
  if( myComponentDefinition && myMethodName )
    updateServices( theMethodName );
}

void YACSGui_ServiceNodePage::onValueChanged( int theRow, int theCol )
{
  if ( YACSGui_Table* aTable = ( YACSGui_Table* )sender() )
    if ( theCol == 2 ) // the value type of the port was changed
    {
      QString aVT = aTable->item( theRow, theCol )->text();
      if ( !aVT.compare(QString("Double")) )
	aTable->setCellType( theRow, 3, YACSGui_Table::Double );
      else if ( !aVT.compare(QString("Int")) )
	aTable->setCellType( theRow, 3, YACSGui_Table::Int );
      else if ( !aVT.compare(QString("Bool")) )
      {
	aTable->setCellType( theRow, 3, YACSGui_Table::Combo );
	aTable->setParams( theRow, 3, QString("True;False") );
      }
      else // i.e. "String" or "Objref" or "Sequence" or "Array" or "Struct"
	aTable->setCellType( theRow, 3, YACSGui_Table::String );
    }
}

void YACSGui_ServiceNodePage::updateServices( const QString& theCurrent )
{
  YACSGui_Module* aModule = getInputPanel()->getModule();
  YACS::ENGINE::Catalog* aCatalog = aModule->getCatalog();
  if( !aCatalog )
    return;
      
  if ( !myComponent
       ||
       ( myComponent && aCatalog && aCatalog->_componentMap.count(myComponent->getName()) == 0 ) )
  {
    if ( myComponentDefinition ) myComponentDefinition->clear();
    if ( myMethodName ) myMethodName->clear();
    return;
  }
  
  bool isForced = theCurrent.isNull();

  YACS::ENGINE::ServiceNode* aServiceNode = dynamic_cast<ServiceNode*>( getNode() );
  if( !aServiceNode )
    return;

  if( myComponentName.isNull() )
    myComponentName = aServiceNode->getComponent()->getName();

  ComponentDefinition* aCompoDef = aCatalog->_componentMap[ myComponentName.latin1() ];
  if( !aCompoDef )
    return;

  if( isForced )
  {
    myComponentDefinition->setText( myComponentName );
    myMethodName->clear();
  }

  QString aRefName = isForced ? QString( aServiceNode->getMethod() ) : theCurrent;

  std::map<std::string,ServiceNode*> aServiceMap = aCompoDef->_serviceMap;
  std::map<std::string,ServiceNode*>::const_iterator it = aServiceMap.begin();
  std::map<std::string,ServiceNode*>::const_iterator itEnd = aServiceMap.end();

  int aCurrentIndex = 0;
  YACS::ENGINE::Node* aCurrentNode = (*it).second;

  for( int index = 0; it != itEnd; it++, index++ )
  {
    std::string aService = (*it).first;
    if( !strcmp( aRefName.latin1(), (*it).first.c_str() ) )
    {
      aCurrentIndex = index;
      aCurrentNode = (*it).second;
    }

    if( isForced )
      myMethodName->insertItem( QString( aService ) );
  }

  if( isForced )
    myMethodName->setCurrentItem( aCurrentIndex );

  fillInputPortsTable( aCurrentNode );
  fillOutputPortsTable( aCurrentNode );
}

void YACSGui_ServiceNodePage::fillInputPortsTable( YACS::ENGINE::Node* theNode )
{
  if( !theNode )
    return;

  QStringList aPortNames;
  QStringList aPortTypes;
  QStringList aValueTypes;
  QStringList aValues;

  QValueList<bool> aReadOnlyFlags;

  YACSGui_Table* aTable = myInputPortsGroupBox->Table();
  QStringList aPTCB = aTable->Params( 0, 1 );

  list<InPort*> anInPortsEngine = theNode->getSetOfInPort();
  list<InPort*>::iterator anInPortsIter = anInPortsEngine.begin();
  for( ;anInPortsIter!=anInPortsEngine.end();anInPortsIter++)
  {
    InPort* anInPort = *anInPortsIter;

    // Collect port names in the list
    aPortNames.append( anInPort->getName() );
    
    // Port type
    if ( InputPort* anInputP = dynamic_cast<InputPort*>(anInPort) )
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
  }

  // Fill "Port name" column
  aTable->setStrings( 0, aPortNames, true );

  // Fill "Port type" column
  aTable->setStrings( 1, aPortTypes, true );

  // Fill "Value type" column
  aTable->setStrings( 2, aValueTypes, true );

  // Fill "Value" column
  aTable->setStrings( 3, aValues, true );

  // Set all columns read only (except "Value" column)
  aTable->setReadOnly( -1, 0, true );
  aTable->setReadOnly( -1, 1, true );
  aTable->setReadOnly( -1, 2, true );

  // Set "Value" column read only (for linked ports)
  for ( int i = 0, n = aTable->numRows(); i < n; i++ )
    aTable->setReadOnly( i, 3, aReadOnlyFlags[ i ] );

  if ( aPortNames.count() > 1 )
    myInputPortsGroupBox->EnableBtn( YACSGui_PlusMinusGrp::UpBtn | YACSGui_PlusMinusGrp::DownBtn );

}

void YACSGui_ServiceNodePage::fillOutputPortsTable( YACS::ENGINE::Node* theNode )
{
  if( !theNode )
    return;

  QStringList aPortNames;
  QStringList aPortTypes;
  QStringList aValueTypes;
  QStringList aValues;
  QStringList anIsInStudy;
  
  YACSGui_Table* aTable = myOutputPortsGroupBox->Table();
  QStringList aPTCB = aTable->Params( 0, 1 );

  list<OutPort*> anOutPortsEngine = theNode->getSetOfOutPort();
  list<OutPort*>::iterator anOutPortsIter = anOutPortsEngine.begin();
  for( ;anOutPortsIter!=anOutPortsEngine.end();anOutPortsIter++)
  {
    OutPort* anOutPort = *anOutPortsIter;

    // Collect port names in the list
    aPortNames.append( anOutPort->getName() );
    
    // Port type
    if ( OutputPort* anOutputP = dynamic_cast<OutputPort*>(anOutPort) )
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
  }

  // Fill "Port name" column
  aTable->setStrings( 0, aPortNames, true );

  // Fill "Port type" column
  aTable->setStrings( 1, aPortTypes, true );

  // Fill "Value type" column
  aTable->setStrings( 2, aValueTypes, true );

  // Fill "Value" column
  aTable->setStrings( 3, aValues, true );

  // Fill "Is in study" column
  aTable->setStrings( 4, anIsInStudy, true );

  // Set all columns read only
  aTable->setReadOnly( -1, 0, true );
  aTable->setReadOnly( -1, 1, true );
  aTable->setReadOnly( -1, 2, true );
  aTable->setReadOnly( -1, 3, true );

  if ( aPortNames.count() > 1 )
    myOutputPortsGroupBox->EnableBtn( YACSGui_PlusMinusGrp::UpBtn | YACSGui_PlusMinusGrp::DownBtn );
}

QString YACSGui_ServiceNodePage::getPortType( YACS::ENGINE::Port* thePort ) const
{
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
    if (aDType != NONE)
      aType = aVTCB[aDType-1];
  }
  
  return aType;
}

void YACSGui_ServiceNodePage::setInputPorts()
{
  if ( !myInputPortsGroupBox ) return;

  ServiceNode* aNode = dynamic_cast<ServiceNode*>( getNode() );
  if ( !aNode ) return;

  if ( myMethodChanged )
  {
    // remove old ports
    list<InPort*> anInPortsEngine = aNode->getSetOfInPort();
    list<InPort*>::iterator anInPortsIter = anInPortsEngine.begin();
    for( ;anInPortsIter!=anInPortsEngine.end();anInPortsIter++)
    {
      mySNode->update( REMOVE,
		       ( dynamic_cast<InputPort*>(*anInPortsIter) ? INPUTPORT : INPUTDATASTREAMPORT ),
		       GuiContext::getCurrent()->_mapOfSubjectDataPort[*anInPortsIter] );
      
      //aNode->edRemovePort(*anInPortsIter);
      // remove port subject (and all subject links connected to this port)
      mySNode->destroy( GuiContext::getCurrent()->_mapOfSubjectDataPort[*anInPortsIter] );
    }
    
    // create a set of new input ports
    YACS::ENGINE::Catalog* aCatalog = getInputPanel()->getModule()->getCatalog();
    if ( aCatalog && aNode->getComponent()
	 &&
	 aCatalog->_componentMap.count(aNode->getComponent()->getName()) )
    {
      ComponentDefinition* aCompodef = aCatalog->_componentMap[aNode->getComponent()->getName()];
      if ( aCompodef && aCompodef->_serviceMap.count(aNode->getMethod()) )
      	if ( ServiceNode* aNodeToClone = aCompodef->_serviceMap[aNode->getMethod()] )
	{
	  list<InPort*> anInPortsClone = aNodeToClone->getSetOfInPort();
	  list<InPort*>::iterator anInPortsIter = anInPortsClone.begin();
	  for( ;anInPortsIter!=anInPortsClone.end();anInPortsIter++)
	    if ( InputPort* anIP = dynamic_cast<InputPort*>(*anInPortsIter) ) // Data Flow port
	      dynamic_cast<SubjectElementaryNode*>(mySNode)->addInputPort( aCatalog,
									   portValueType(anIP->edGetType()),
									   anIP->getName() );
	    else if ( InputDataStreamPort* anIDSP = dynamic_cast<InputDataStreamPort*>(*anInPortsIter) ) // Data Stream port
	      dynamic_cast<SubjectElementaryNode*>(mySNode)->addIDSPort( aCatalog,
									 portValueType(anIDSP->edGetType()),
									 anIDSP->getName() );
	}
    }
  }
  
  // read input data and input data stream ports from the table
  YACSGui_Table* aTable = myInputPortsGroupBox->Table();
  
  QStringList aPortNames;
  QStringList aValues;

  aTable->strings( 0, aPortNames );
  aTable->strings( 3, aValues );

  // initialize created input ports with values
  int aRowId = 0;
  for ( QStringList::Iterator it = aPortNames.begin(); it != aPortNames.end(); ++it )
  {
    if ( aTable->intValueCombo( 1, aRowId ) == 0 ) // Data Flow port
    {
      InputPort* aIDP;
      try {
	aIDP = aNode->getInputPort(*it);
      }
      catch (YACS::Exception& ex) {
	continue;
      }
      if ( !aIDP )
	continue;

      TypeCode* aTC = aIDP->edGetType();
      if ( !aTC )
	continue;

      // initialize new created input data port
      switch ( aTC->kind() )
      {
      case Double:
	aIDP->edInit( aValues[aRowId].toDouble() );
	break;
      case Int:
	aIDP->edInit( aValues[aRowId].toInt() );
	break;
      case String:
	aIDP->edInit( aValues[aRowId].latin1() );
	break;
      case Bool:
	aIDP->edInit( aValues[aRowId].compare( aTable->Params(aRowId,3)[0] ) ? false : true );
	break;
      case Objref:
	aIDP->edInit( "" ); // TODO : create an Any* with corresponding type and initialize with it
	break;
      case Sequence:
	aIDP->edInit( "" ); // TODO : create an Any* (i.e. SequenceAny*) with corresponding type and initialize with it
	break;
      case Array:
	aIDP->edInit( "" ); // TODO : create an Any* (i.e. ArrayAny*) with corresponding type and initialize with it
	break;
      case Struct:
	aIDP->edInit( "" ); // TODO : create an Any* with corresponding type and initialize with it
	break;
      default:
	break;
      }

      mySNode->update( EDIT, INPUTPORT, GuiContext::getCurrent()->_mapOfSubjectDataPort[aIDP]);
    }
    //else if ( aTable->intValueCombo( 1, aRowId ) == 1 )    // Data Stream (BASIC) port
    //  // TODO : set value for input data stream port
    //else if ( aTable->intValueCombo( 1, aRowId ) == 2 )    // Data Stream (CALCIUM) port
    //  // TODO : set value for input data stream port
    //else if ( aTable->intValueCombo( 1, aRowId ) == 3 )    // Data Stream (PALM) port
    //  // TODO : set value for input data stream port
    
    aRowId++;
  }
}

void YACSGui_ServiceNodePage::setOutputPorts()
{
  if ( !myOutputPortsGroupBox ) return;

  ServiceNode* aNode = dynamic_cast<ServiceNode*>( getNode() );
  if ( !aNode ) return;

  if ( myMethodChanged )
  {
    // remove old ports
    list<OutPort*> anOutPortsEngine = aNode->getSetOfOutPort();
    list<OutPort*>::iterator anOutPortsIter = anOutPortsEngine.begin();
    for( ;anOutPortsIter!=anOutPortsEngine.end();anOutPortsIter++)
    {
      mySNode->update( REMOVE,
		       ( dynamic_cast<OutputPort*>(*anOutPortsIter) ? OUTPUTPORT : OUTPUTDATASTREAMPORT ),
		       GuiContext::getCurrent()->_mapOfSubjectDataPort[*anOutPortsIter] );
      
      //aNode->edRemovePort(*anOutPortsIter);
      // remove port subject (and all subject links connected to this port)
      mySNode->destroy( GuiContext::getCurrent()->_mapOfSubjectDataPort[*anOutPortsIter] );
    }
    
    // create a set of new input ports
    YACS::ENGINE::Catalog* aCatalog = getInputPanel()->getModule()->getCatalog();
    if ( aCatalog && aNode->getComponent()
	 &&
	 aCatalog->_componentMap.count(aNode->getComponent()->getName()) )
    {
      ComponentDefinition* aCompodef = aCatalog->_componentMap[aNode->getComponent()->getName()];
      if ( aCompodef && aCompodef->_serviceMap.count(aNode->getMethod()) )
      	if ( ServiceNode* aNodeToClone = aCompodef->_serviceMap[aNode->getMethod()] )
	{
	  list<OutPort*> anOutPortsClone = aNodeToClone->getSetOfOutPort();
	  list<OutPort*>::iterator anOutPortsIter = anOutPortsClone.begin();
	  for( ;anOutPortsIter!=anOutPortsClone.end();anOutPortsIter++)
	    if ( OutputPort* anOP = dynamic_cast<OutputPort*>(*anOutPortsIter) ) // Data Flow port
	      dynamic_cast<SubjectElementaryNode*>(mySNode)->addOutputPort( aCatalog,
									    portValueType(anOP->edGetType()),
									    anOP->getName() );
	    else if ( OutputDataStreamPort* anODSP = dynamic_cast<OutputDataStreamPort*>(*anOutPortsIter) ) // Data Stream port
	      dynamic_cast<SubjectElementaryNode*>(mySNode)->addODSPort( aCatalog,
									 portValueType(anODSP->edGetType()),
									 anODSP->getName() );
	}
    }
  }

  // read output data and output data stream ports from the table
  YACSGui_Table* aTable = myOutputPortsGroupBox->Table();
  
  QStringList aPortNames;
  aTable->strings( 0, aPortNames );

  int aRowId = 0;
  for ( QStringList::Iterator it = aPortNames.begin(); it != aPortNames.end(); ++it )
  {
    // TODO : Remember "Is in study" flag for output port
    // ...

    aRowId++;
  }
}

YACS::ENGINE::TypeCode* YACSGui_ServiceNodePage::createTypeCode( YACS::ENGINE::DynType theType,
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
}

void YACSGui_ForLoopNodePage::setSNode( YACS::HMI::SubjectNode* theSNode )
{
  if ( !theSNode ) return;

  if ( dynamic_cast<ForLoop*>( theSNode->getNode() ) )
    YACSGui_NodePage::setSNode( theSNode );
}

YACSGui_InputPanel* YACSGui_ForLoopNodePage::getInputPanel() const
{
  return dynamic_cast<YACSGui_InputPanel*>( parent()->parent()->parent()->parent() );
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

void YACSGui_ForLoopNodePage::notifyInPortValues( std::list<std::string> theInPortsNames,
						  std::list<std::string> theValues )
{
  //printf("==> ForLoopNodePage : Size of theInPortsNames : %d\n",theInPortsNames.size());
  //printf("==> ForLoopNodePage : Size of theValues : %d\n",theValues.size());

  ForLoop* aForLoopNode = dynamic_cast<ForLoop*>( getNode() );
  if ( !aForLoopNode ) return;

  if ( theInPortsNames.size() == 1 && theValues.size() == 1 )
  {
    // nsteps port
    QString aName(aForLoopNode->edGetNbOfTimesInputPort()->getName());
    QString aGivenName( theInPortsNames.front() );
    if ( !aGivenName.compare(aName) && myNbTimesInputPortValue )
    {
      QString aValue(theValues.front());
      if ( !aValue.compare(QString("< ? >")) )
	myNbTimesInputPortValue->setValue( myNbTimesInputPortValue->minValue() );
      else
	myNbTimesInputPortValue->setValue( aValue.toInt() );
    }
  }
}

void YACSGui_ForLoopNodePage::notifyOutPortValues( std::list<std::string> theOutPortsNames,
						   std::list<std::string> theValues )
{
  //printf("==> ForLoopNodePage : Size of theOutPortsNames : %d\n",theOutPortsNames.size());
  //printf("==> ForLoopNodePage : Size of theValues : %d\n",theValues.size());
}

void YACSGui_ForLoopNodePage::notifyNodeCreateBody( YACS::HMI::Subject* theSubject )
{
  if( myLoopBodyNodeName )
  {
    QString aBodyName = theSubject->getName();
    myLoopBodyNodeName->setText( aBodyName );
  }
}

void YACSGui_ForLoopNodePage::onApply()
{
  // Rename a node
  if ( myNodeName ) setNodeName( myNodeName->text() );
  
  // Reset number of times input port value
  if ( YACS::ENGINE::ForLoop* aForLoopNode = dynamic_cast<ForLoop*>( getNode() ) )
  {
    if( YACS::ENGINE::InputPort* aPort = aForLoopNode->edGetNbOfTimesInputPort() )
    {
      int aValue = myNbTimesInputPortValue->value();
      aPort->edInit( aValue );
    }
  }

  // Reset the view mode
  // ...

  mySNode->update( EDIT, 0, mySNode );

  updateBlocSize();
}

void YACSGui_ForLoopNodePage::updateState()
{
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
      std::set<Node*> aNodes = aForLoopNode->edGetDirectDescendants();
      std::set<Node*>::iterator aNodesIt = aNodes.begin();
      Node* aNode = *aNodesIt;
      if( aNode )
      {
	QString aBodyName = aNode->getName();
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

/*
  Class : YACSGui_ForEachLoopNodePage
  Description :  Page for FOREACH node properties
*/

YACSGui_ForEachLoopNodePage::YACSGui_ForEachLoopNodePage( QWidget* theParent, const char* theName, WFlags theFlags )
  : ForEachLoopNodePage( theParent, theName, theFlags ),
    YACSGui_NodePage()
{
  connect( myDataPortToDispatchValue, SIGNAL( textChanged( const QString& ) ),
	   this, SLOT( onSeqOfSamplesChanged( const QString& ) ) );

  // TODO: implement presentation of the FOR node in collapsed mode
  // temporary solution :
  myCollapsedRadioButton->setEnabled(false );

  connect( myNodeName, SIGNAL(textChanged( const QString& )), this, SLOT(onNodeNameChanged( const QString& )) );
}

YACSGui_ForEachLoopNodePage::~YACSGui_ForEachLoopNodePage()
{
}

void YACSGui_ForEachLoopNodePage::setSNode( YACS::HMI::SubjectNode* theSNode )
{
  if ( !theSNode ) return;

  if ( dynamic_cast<ForEachLoop*>( theSNode->getNode() ) )
    YACSGui_NodePage::setSNode( theSNode );
}

YACSGui_InputPanel* YACSGui_ForEachLoopNodePage::getInputPanel() const
{
  return dynamic_cast<YACSGui_InputPanel*>( parent()->parent()->parent()->parent() );
}

void YACSGui_ForEachLoopNodePage::setMode( const YACSGui_InputPanel::PageMode theMode )
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

void YACSGui_ForEachLoopNodePage::notifyNodeStatus( int theStatus )
{
  if ( myNodeState )
  {
    myNodeState->setPaletteForegroundColor(getColor(theStatus));
    myNodeState->setText(getText(theStatus));
  }
}

void YACSGui_ForEachLoopNodePage::notifyNodeProgress()
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

void YACSGui_ForEachLoopNodePage::notifyInPortValues( std::list<std::string> theInPortsNames,
						      std::list<std::string> theValues )
{
  //printf("==> ForEachLoopNodePage : Size of theInPortsNames : %d\n",theInPortsNames.size());
  //printf("==> ForEachLoopNodePage : Size of theValues : %d\n",theValues.size());

  ForEachLoop* aForEachLoopNode = dynamic_cast<ForEachLoop*>( getNode() );
  if ( !aForEachLoopNode ) return;

  if ( theInPortsNames.size() == 2 && theValues.size() == 2 )
  {
    QString aName1 = QString(aForEachLoopNode->edGetNbOfBranchesPort()->getName());
    QString aName2 = QString(aForEachLoopNode->edGetSeqOfSamplesPort()->getName());
    QString aGivenName, aValue;
    
    list<string>::iterator itN = theInPortsNames.begin();
    list<string>::iterator itV = theValues.begin();
    for ( ; itN!=theInPortsNames.end() && itV!=theValues.end(); itN++, itV++ )
    {
      aGivenName = QString(*itN);
      aValue = QString(*itV);
      
      if ( !aGivenName.compare(aName1) && myNbBranchesInputPortValue )
      {	// nbBranches port
	if ( !aValue.compare(QString("< ? >")) )
	  myNbBranchesInputPortValue->setValue( myNbBranchesInputPortValue->minValue() );
	else
	  myNbBranchesInputPortValue->setValue( aValue.toInt() );
      }

      if ( !aGivenName.compare(aName2) && myDataPortToDispatchValue )
      { // SmplsCollection port
	myDataPortToDispatchValue->setText( aValue );
      }
    }
  }
}

void YACSGui_ForEachLoopNodePage::notifyOutPortValues( std::list<std::string> theOutPortsNames,
						       std::list<std::string> theValues )
{
  //printf("==> ForEachLoopNodePage : Size of theOutPortsNames : %d\n",theOutPortsNames.size());
  //printf("==> ForEachLoopNodePage : Size of theValues : %d\n",theValues.size());
  // SmplPrt port (no gui control for it in the property page!)
  //...
}

void YACSGui_ForEachLoopNodePage::onApply()
{
  // Rename a node
  if ( myNodeName ) setNodeName( myNodeName->text() );
  
  // Reset number of brances input port value and data port to dispatch value
  if ( YACS::ENGINE::ForEachLoop* aForEachLoopNode = dynamic_cast<ForEachLoop*>( getNode() ) )
  {
    if( YACS::ENGINE::InputPort* aBranchesPort = aForEachLoopNode->edGetNbOfBranchesPort() )
    {
      int aValue = myNbBranchesInputPortValue->value();
      aBranchesPort->edInit( aValue );
    }

    if( YACS::ENGINE::InputPort* aSamplesPort = aForEachLoopNode->edGetSeqOfSamplesPort() )
    {
      QString aValue = myDataPortToDispatchValue->text();
      aSamplesPort->edInit( aValue.latin1() );
    }
  }

  // Reset the view mode
  // ...

  mySNode->update( EDIT, 0, mySNode );

  updateBlocSize();
}

void YACSGui_ForEachLoopNodePage::updateState()
{
  // Set node name
  if ( myNodeName ) myNodeName->setText( getNodeName() );

  // Set node full name (read only)
  if ( myNodeFullName )
    myNodeFullName->setText( getNode()->getRootNode()->getChildName(getNode()) );

  // Set number of brances input port value, data port to dispatch value and loop body node name
  if ( YACS::ENGINE::ForEachLoop* aForEachLoopNode = dynamic_cast<ForEachLoop*>( getNode() ) )
  {
    if( YACS::ENGINE::InputPort* aBranchesPort = aForEachLoopNode->edGetNbOfBranchesPort() )
    {
      bool ok;
      int aValue = getPortValue( aBranchesPort ).toInt( &ok );
      if( ok && aValue > 1 )
	myNbBranchesInputPortValue->setValue( aValue );
    }

    if( YACS::ENGINE::InputPort* aSamplesPort = aForEachLoopNode->edGetSeqOfSamplesPort() )
    {
      QString aValue = getPortValue( aSamplesPort );
      myDataPortToDispatchValue->setText( aValue );
    }

    if( myLoopBodyNodeName )
    {
      std::set<Node*> aNodes = aForEachLoopNode->edGetDirectDescendants();
      std::set<Node*>::iterator aNodesIt = aNodes.begin();
      Node* aNode = *aNodesIt;
      if( aNode )
      {
	QString aBodyName = aNode->getName();
	myLoopBodyNodeName->setText( aBodyName );
      }
    }
  }
  
  // Set view mode of the given FOREACH node
  // the following method can be used if its needed:
  // YACSGui_Graph* aGraph = getInputPanel()->getModule()->getGraph( myProc );
  // ...
}

void YACSGui_ForEachLoopNodePage::onNodeNameChanged( const QString& theName )
{
  if ( myNodeFullName )
  {
    QString anOldName = myNodeFullName->text();
    int aRDotId = anOldName.findRev('.');
    myNodeFullName->setText( anOldName.replace( aRDotId+1, anOldName.length()-(aRDotId+1), theName ) );
  }
}

void YACSGui_ForEachLoopNodePage::onSeqOfSamplesChanged( const QString& theText )
{
  /*
  QString aText = theText;
  if( aText.left( 1 ) != "[" )
    aText.prepend( "[" );
    
  if( aText.right( 1 ) != "]" )
    aText.append( "]" );

  if( aText != theText )
    myDataPortToDispatchValue->setText( aText );
  */
}

/*
  Class : YACSGui_WhileLoopNodePage
  Description :  Page for WHILE node properties
*/

YACSGui_WhileLoopNodePage::YACSGui_WhileLoopNodePage( QWidget* theParent, const char* theName, WFlags theFlags )
  : WhileLoopNodePage( theParent, theName, theFlags ),
    YACSGui_NodePage()
{
  // TODO: implement presentation of the FOR node in collapsed mode
  // temporary solution :
  myCollapsedRadioButton->setEnabled(false );

  connect( myNodeName, SIGNAL(textChanged( const QString& )), this, SLOT(onNodeNameChanged( const QString& )) );
}

YACSGui_WhileLoopNodePage::~YACSGui_WhileLoopNodePage()
{
}

void YACSGui_WhileLoopNodePage::setSNode( YACS::HMI::SubjectNode* theSNode )
{
  if ( !theSNode ) return;

  if ( dynamic_cast<WhileLoop*>( theSNode->getNode() ) )
    YACSGui_NodePage::setSNode( theSNode );
}

YACSGui_InputPanel* YACSGui_WhileLoopNodePage::getInputPanel() const
{
  return dynamic_cast<YACSGui_InputPanel*>( parent()->parent()->parent()->parent() );
}

void YACSGui_WhileLoopNodePage::setMode( const YACSGui_InputPanel::PageMode theMode )
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

void YACSGui_WhileLoopNodePage::notifyNodeStatus( int theStatus )
{
  if ( myNodeState )
  {
    myNodeState->setPaletteForegroundColor(getColor(theStatus));
    myNodeState->setText(getText(theStatus));
  }
}

void YACSGui_WhileLoopNodePage::notifyNodeProgress()
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

void YACSGui_WhileLoopNodePage::notifyInPortValues( std::list<std::string> theInPortsNames,
						    std::list<std::string> theValues )
{
  //printf("==> WhileLoopNodePage : Size of theInPortsNames : %d\n",theInPortsNames.size());
  //printf("==> WhileLoopNodePage : Size of theValues : %d\n",theValues.size());

  WhileLoop* aWhileLoopNode = dynamic_cast<WhileLoop*>( getNode() );
  if ( !aWhileLoopNode ) return;

  if ( theInPortsNames.size() == 1 && theValues.size() == 1 )
  {
    QString aName(aWhileLoopNode->edGetConditionPort()->getName());
    QString aGivenName( theInPortsNames.front() );
    if ( !aGivenName.compare(aName) && myCondInputPortValue )
    {
      QString aValue(theValues.front());
      int anId = 0;
      if ( !aValue.compare(QString("< ? >")) ||
	   aValue.compare(QString("false")) ||
	   !aValue.toInt() )
	anId = 1;
      myCondInputPortValue->setCurrentItem(anId);
    }
  }
}

void YACSGui_WhileLoopNodePage::notifyOutPortValues( std::list<std::string> theOutPortsNames,
						     std::list<std::string> theValues )
{
  //printf("==> WhileLoopNodePage : Size of theOutPortsNames : %d\n",theOutPortsNames.size());
  //printf("==> WhileLoopNodePage : Size of theValues : %d\n",theValues.size());
}

void YACSGui_WhileLoopNodePage::notifyNodeCreateBody( YACS::HMI::Subject* theSubject )
{
  if( myLoopBodyNodeName )
  {
    QString aBodyName = theSubject->getName();
    myLoopBodyNodeName->setText( aBodyName );
  }
}

void YACSGui_WhileLoopNodePage::onApply()
{
  // Rename a node
  if ( myNodeName ) setNodeName( myNodeName->text() );
  
  // Reset condition input port value
  if ( YACS::ENGINE::WhileLoop* aWhileLoopNode = dynamic_cast<WhileLoop*>( getNode() ) )
  {
    if( YACS::ENGINE::InputPort* aPort = aWhileLoopNode->edGetConditionPort() )
    {
      bool aValue = myCondInputPortValue->currentItem() == 0 ? true : false;
      aPort->edInit( aValue );
    }
  }
  
  // Reset the view mode
  // ...

  mySNode->update( EDIT, 0, mySNode );

  updateBlocSize();
}

void YACSGui_WhileLoopNodePage::updateState()
{
  // Set node name
  if ( myNodeName ) myNodeName->setText( getNodeName() );

  // Set node full name (read only)
  if ( myNodeFullName )
    myNodeFullName->setText( getNode()->getRootNode()->getChildName(getNode()) );
  
  // Set condition input port value and loop body node name
  if ( YACS::ENGINE::WhileLoop* aWhileLoopNode = dynamic_cast<WhileLoop*>( getNode() ) )
  {
    if( YACS::ENGINE::InputPort* aPort = aWhileLoopNode->edGetConditionPort() )
    {
      QString aValue = getPortValue( aPort );
      int anIndex = aValue == "True" ? 0 : 1;
      myCondInputPortValue->setCurrentItem( anIndex );
    }

    if( myLoopBodyNodeName )
    {
      std::set<Node*> aNodes = aWhileLoopNode->edGetDirectDescendants();
      std::set<Node*>::iterator aNodesIt = aNodes.begin();
      Node* aNode = *aNodesIt;
      if( aNode )
      {
	QString aBodyName = aNode->getName();
	myLoopBodyNodeName->setText( aBodyName );
      }
    }
  }
  
  // Set view mode of the given WHILE node
  // the following method can be used if its needed:
  // YACSGui_Graph* aGraph = getInputPanel()->getModule()->getGraph( myProc );
  // ...
}

void YACSGui_WhileLoopNodePage::onNodeNameChanged( const QString& theName )
{
  if ( myNodeFullName )
  {
    QString anOldName = myNodeFullName->text();
    int aRDotId = anOldName.findRev('.');
    myNodeFullName->setText( anOldName.replace( aRDotId+1, anOldName.length()-(aRDotId+1), theName ) );
  }
}

/*
  Class : YACSGui_SwitchNodePage
  Description :  Page for SWITCH node properties
*/

YACSGui_SwitchNodePage::YACSGui_SwitchNodePage( QWidget* theParent, const char* theName, WFlags theFlags )
  : SwitchNodePage( theParent, theName, theFlags ),
    YACSGui_NodePage(),
    myIsSelectChild(false)
{
  if ( mySwitchCasesGroupBox )
  {
    mySwitchCasesGroupBox->setTitle( tr("SWITCH Cases") );
    mySwitchCasesGroupBox->HideBtn( YACSGui_PlusMinusGrp::InsertBtn |
				    YACSGui_PlusMinusGrp::UpBtn |
				    YACSGui_PlusMinusGrp::DownBtn );
    mySwitchCasesGroupBox->EnableBtn( YACSGui_PlusMinusGrp::SelectBtn, false );
    YACSGui_Table* aTable = mySwitchCasesGroupBox->Table();
    aTable->setFixedHeight( 100 );
    aTable->setNumCols( 2 );
    aTable->horizontalHeader()->setLabel( 0, tr( "Case id" ) );
    aTable->horizontalHeader()->setLabel( 1, tr( "Node name" ) );
    aTable->setCellType( -1, 0, YACSGui_Table::String );
    aTable->setCellType( -1, 1, YACSGui_Table::String );

    connect( aTable,     SIGNAL(valueChanged( int, int )), this, SLOT(onValueChanged( int, int )) );

    mySwitchCasesGroupBox->EnableBtn( YACSGui_PlusMinusGrp::AllBtn );
    mySwitchCasesGroupBox->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );

    connect( myNodeName, SIGNAL(textChanged( const QString& )), this, SLOT(onNodeNameChanged( const QString& )) );
    connect( mySwitchCasesGroupBox, SIGNAL(initSelection( const int, const int )),
	     this,                  SLOT(onInitSelection( const int, const int )) );
    connect( mySwitchCasesGroupBox->Table(), SIGNAL(itemCreated( const int, const int )),
	     this,                           SLOT(onNewItem( const int, const int )) );
    
    connect( mySwitchCasesGroupBox, SIGNAL(Added( const int )),     this, SLOT(onAdded( const int )) );
    connect( mySwitchCasesGroupBox, SIGNAL(Inserted( const int )),  this, SLOT(onInserted( const int )) );
    connect( mySwitchCasesGroupBox, SIGNAL(MovedUp( const int )),   this, SLOT(onMovedUp( const int )) );
    connect( mySwitchCasesGroupBox, SIGNAL(MovedDown( const int )), this, SLOT(onMovedDown( const int )) );
    connect( mySwitchCasesGroupBox, SIGNAL(Removed( const int )),   this, SLOT(onRemoved( const int )) );
  }

  // TODO: implement presentation of the switch node in collapsed mode
  // temporary solution :
  myCollapsedRadioButton->setEnabled(false );
}

YACSGui_SwitchNodePage::~YACSGui_SwitchNodePage()
{
  myRow2ChildMap.clear();
  myRowsOfSelectedChildren.clear();
}

void YACSGui_SwitchNodePage::setSNode( YACS::HMI::SubjectNode* theSNode )
{
  if ( !theSNode ) return;

  if ( dynamic_cast<Switch*>( theSNode->getNode() ) )
    YACSGui_NodePage::setSNode( theSNode );
}

YACSGui_InputPanel* YACSGui_SwitchNodePage::getInputPanel() const
{
  return dynamic_cast<YACSGui_InputPanel*>( parent()->parent()->parent()->parent() );
}

void YACSGui_SwitchNodePage::setChild( YACS::HMI::SubjectNode* theChildNode )
{
  if ( !theChildNode ) return;

  if ( !mySwitchCasesGroupBox ) return;
  YACSGui_Table* aTable = mySwitchCasesGroupBox->Table();

  myIsSelectChild = false;
  mySwitchCasesGroupBox->setOffSelect();
  mySwitchCasesGroupBox->EnableBtn( YACSGui_PlusMinusGrp::SelectBtn, false );

  // sets the child node name into table's cell
  QStringList aChildNames;
  aTable->strings( 1, aChildNames );
  aChildNames[aTable->currentRow()] = theChildNode->getNode()->getName();
  aTable->setStrings( 1, aChildNames );

  // add theChildNode into the map of children of this switch node
  if ( myRow2ChildMap.find(aTable->currentRow()) == myRow2ChildMap.end() )
    myRow2ChildMap.insert( std::make_pair( aTable->currentRow(), theChildNode ) );
  else
    myRow2ChildMap[aTable->currentRow()] = theChildNode;

  bool anExist = false;
  std::list<int>::iterator it = myRowsOfSelectedChildren.begin();
  for ( ; it != myRowsOfSelectedChildren.end(); it++ )
    if ( *it == aTable->currentRow() )
    {
      anExist = true;
      break;
    }
  if ( !anExist )
    myRowsOfSelectedChildren.push_back( aTable->currentRow() );
}

bool YACSGui_SwitchNodePage::isSelectChild() const
{
  return myIsSelectChild;
}

void YACSGui_SwitchNodePage::setMode( const YACSGui_InputPanel::PageMode theMode )
{
  YACSGui_NodePage::setMode(theMode);

  if ( myMode == YACSGui_InputPanel::EditMode )
  {
    myNodeName->setReadOnly(false);
    
    mySwitchCasesGroupBox->ShowBtn( YACSGui_PlusMinusGrp::SelectBtn |
				    YACSGui_PlusMinusGrp::PlusBtn |
				    YACSGui_PlusMinusGrp::MinusBtn );
    YACSGui_Table* aTable = mySwitchCasesGroupBox->Table();
    aTable->setReadOnly( -1, 0, false );

    ViewModeButtonGroup->show();

    ExecutionGroupBox->hide();
  } 
  else if ( myMode == YACSGui_InputPanel::RunMode )
  {
    myNodeName->setReadOnly(true);
    
    mySwitchCasesGroupBox->HideBtn();
    YACSGui_Table* aTable = mySwitchCasesGroupBox->Table();
    aTable->setReadOnly( -1, 0, true );

    ViewModeButtonGroup->hide();

    ExecutionGroupBox->show();
  }
}

void YACSGui_SwitchNodePage::notifyNodeStatus( int theStatus )
{
  if ( myNodeState )
  {
    myNodeState->setPaletteForegroundColor(getColor(theStatus));
    myNodeState->setText(getText(theStatus));
  }
}

void YACSGui_SwitchNodePage::notifyNodeProgress()
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

void YACSGui_SwitchNodePage::notifyInPortValues( std::list<std::string> theInPortsNames,
						 std::list<std::string> theValues )
{
  //printf("==> SwitchNodePage : Size of theInPortsNames : %d\n",theInPortsNames.size());
  //printf("==> SwitchNodePage : Size of theValues : %d\n",theValues.size());

  Switch* aSwitchNode = dynamic_cast<Switch*>( getNode() );
  if ( !aSwitchNode ) return;

  if ( theInPortsNames.size() == 1 && theValues.size() == 1 )
  {
    // select port
    QString aName(aSwitchNode->edGetConditionPort()->getName());
    QString aGivenName( theInPortsNames.front() );
    // (no gui control for it in the property page!) : will be implemented as a combobox
    if ( !aGivenName.compare(aName) /*&& mySelectPortValue*/ )
    {
      QString aValue(theValues.front());
      //if ( !aValue.compare(QString("< ? >")) )
      //	mySelectPortValue->setCurrentItem( /* default case */ );
      //else
      //	mySelectPortValue->setCurrentItem( aValue.toInt() );
    }
  }
}

void YACSGui_SwitchNodePage::notifyOutPortValues( std::list<std::string> theOutPortsNames,
						  std::list<std::string> theValues )
{
  //printf("==> SwitchNodePage : Size of theOutPortsNames : %d\n",theOutPortsNames.size());
  //printf("==> SwitchNodePage : Size of theValues : %d\n",theValues.size());
}

void YACSGui_SwitchNodePage::onApply()
{
  // Rename a node
  if ( myNodeName && getNode() )
    getNode()->setName( myNodeName->text().latin1() );
  
  // To remove a new child nodes from its old places in the tree view
  std::list<int>::iterator it = myRowsOfSelectedChildren.begin();
  for ( ; it != myRowsOfSelectedChildren.end(); it++ )
  {
    SubjectNode* aNewSelectedChild = myRow2ChildMap[*it];
    Node* aNode = aNewSelectedChild->getNode();

    if( SubjectSwitch* aSSwitch = dynamic_cast<SubjectSwitch*>( aNewSelectedChild->getParent() ) )
    {
      Switch* aSwitch = dynamic_cast<Switch*>( aSSwitch->getNode() );

      Node* aDefaultNode = 0;
      try {
	aDefaultNode = aSwitch->getChildByShortName(Switch::DEFAULT_NODE_NAME);
      }
      catch (YACS::Exception& ex) {}

      if( aDefaultNode && aDefaultNode == aNode )
	continue; // do not remove a default node - keep it for copying
    }

    try {
      getNode()->getChildByName( aNode->getName() );
    }
    catch (YACS::Exception& ex) {
      // block is not contain nodes with the same name as aNode
      aNewSelectedChild->getParent()->update( REMOVE,
					      ProcInvoc::getTypeOfNode( aNewSelectedChild->getNode() ),
					      aNewSelectedChild );
    }
  }
  
  // Reset the list of switch cases
  setSwitchCases();
  
  // Reset the view mode
  // ...

  mySNode->update( EDIT, 0, mySNode );
  
  // clear for next selections
  myRowsOfSelectedChildren.clear();

  updateBlocSize();
}

void YACSGui_SwitchNodePage::updateState()
{
  myRow2ChildMap.clear();
  myRowsOfSelectedChildren.clear();

  // Clear input ports table and output ports tables, if they are not empty
  if ( mySwitchCasesGroupBox ) {
    mySwitchCasesGroupBox->Table()->setNumRows( 0 );
    mySwitchCasesGroupBox->EnableBtn( YACSGui_PlusMinusGrp::AllBtn );
  }

  // Set node name
  if ( myNodeName ) myNodeName->setText( getNodeName() );

  // Set node full name (read only)
  if ( myNodeFullName )
    myNodeFullName->setText( getNode()->getRootNode()->getChildName(getNode()) );
  
  // Fill the table of child nodes of the switch
  SubjectSwitch* aSNode = dynamic_cast<SubjectSwitch*>(mySNode);
  if ( aSNode && mySwitchCasesGroupBox )
  {
    if( Switch* aSwitch = dynamic_cast<Switch*>(getNode()) )
    {
      set<Node*> aChildren = dynamic_cast<Switch*>(getNode())->edGetDirectDescendants();
      if ( !aChildren.empty() )
      {
	set<Node*>::iterator aChildIt = aChildren.begin();

	Node* aDefaultNode = 0;
	try {
	  aDefaultNode = aSwitch->getChildByShortName(Switch::DEFAULT_NODE_NAME);
	}
	catch (YACS::Exception& ex) {
	  SUIT_MessageBox::warn1(getInputPanel()->getModule()->getApp()->desktop(),
				 tr("WRN_WARNING"),
				 QString("Switch has no a default node!"),
				 tr("BUT_OK"));
	}
	
	int aMinCaseId, aMaxCaseId;
	aMinCaseId = aMaxCaseId = aSwitch->getRankOfNode(*aChildIt);
	
	list<Node*> aCaseNodes;
	
	for( ; aChildIt != aChildren.end(); aChildIt++ )
	{
	  Node* aNode = *aChildIt;
	  if( aNode == aDefaultNode )
	    continue;
	  
	  int aRank = aSwitch->getRankOfNode( aNode );
	  if( aMinCaseId >= aRank )
	  {
	    aCaseNodes.push_front( aNode );
	    aMinCaseId = aRank;
	  }
	  else if( aMinCaseId < aRank && aMaxCaseId > aRank )
	  {
	    list<Node*>::iterator aCaseNodesIt = aCaseNodes.begin();
	    for( list<Node*>::iterator anIt = aCaseNodesIt;
		 anIt++ != aCaseNodes.end();
		 aCaseNodesIt++, anIt = aCaseNodesIt )
	    {
	      if ( aRank >= aSwitch->getRankOfNode( *aCaseNodesIt ) &&
		   aSwitch->getRankOfNode( aNode ) <= aSwitch->getRankOfNode( *anIt ) )
	      {
		aCaseNodes.insert( anIt, aNode );
		break;
	      }
	    }
	  }
	  else if( aMaxCaseId <= aRank )
	  {
	    aCaseNodes.push_back( aNode );
	    aMaxCaseId = aRank;
	  }
	}
	aCaseNodes.push_back( aDefaultNode );
	
	QStringList aChildIds;
	QStringList aChildNames;
	
	list<Node*>::iterator anIt = aCaseNodes.begin();
	list<Node*>::iterator anItEnd = aCaseNodes.end();
	for( int aRow = 0; anIt != anItEnd; anIt++, aRow++ )
	{
	  QString anId, aName;
	  Node* aNode = *anIt;
	  
	  if( aNode == aDefaultNode )
	    anId = Switch::DEFAULT_NODE_NAME;
	  else
	    anId = QString::number( aSwitch->getRankOfNode( aNode ) );
	  aChildIds.append( anId );
	  
	  // Collect child names in the list
	  aName = aNode->getName();
	  aChildNames.append( aName );
	  
	  if ( myRow2ChildMap.find(aRow) == myRow2ChildMap.end() )
	    myRow2ChildMap.insert( std::make_pair( aRow, aSNode->getChild(aNode) ) );
	  else
	    myRow2ChildMap[aRow] = aSNode->getChild(aNode);
	}
	
	// Fill "Node name" column
	mySwitchCasesGroupBox->Table()->setStrings( 0, aChildIds, true );
	mySwitchCasesGroupBox->Table()->setStrings( 1, aChildNames, true );
	
	if ( !aChildNames.empty() ) mySwitchCasesGroupBox->EnableBtn( YACSGui_PlusMinusGrp::AllBtn );
      }
    }
  }
  
  // Set view mode of the given switch node
  // the following method can be used if its needed:
  // YACSGui_Graph* aGraph = getInputPanel()->getModule()->getGraph( myProc );
  // ...
}

void YACSGui_SwitchNodePage::onNodeNameChanged( const QString& theName )
{
  if ( myNodeFullName )
  {
    QString anOldName = myNodeFullName->text();
    int aRDotId = anOldName.findRev('.');
    myNodeFullName->setText( anOldName.replace( aRDotId+1, anOldName.length()-(aRDotId+1), theName ) );
  }
}

void YACSGui_SwitchNodePage::onValueChanged( int theRow, int theCol )
{
  if ( YACSGui_Table* aTable = ( YACSGui_Table* )sender() )
  {
    if ( theCol == 0 )
    {
      QString anId = aTable->item( theRow, theCol )->text();
      QStringList anIds;
      aTable->strings( 0, anIds );
      if( anIds.contains( anId ) > 1 )
	SUIT_MessageBox::warn1(getInputPanel()->getModule()->getApp()->desktop(),
			       tr("WRN_WARNING"),
			       QString("Switch is already contains a node with such id."),
			       tr("BUT_OK"));
    }
  }
}

void YACSGui_SwitchNodePage::onInitSelection( const int theRow, const int theCol )
{
  myIsSelectChild = true;
}

void YACSGui_SwitchNodePage::onNewItem( const int theRow, const int theCol )
{
  if ( mySwitchCasesGroupBox && theCol == 1 ) // set "node name" column read only
    mySwitchCasesGroupBox->Table()->setReadOnly( theRow, theCol, true );
}

void YACSGui_SwitchNodePage::onAdded( const int theRow )
{
  SubjectNode* aNode = 0;
  myRow2ChildMap.insert( std::make_pair( theRow, aNode ) );
}

void YACSGui_SwitchNodePage::onInserted( const int theRow )
{
  SubjectNode* aNode = 0;

  if ( myRow2ChildMap.find(theRow) == myRow2ChildMap.end() )
    myRow2ChildMap.insert( std::make_pair( theRow, aNode ) );
  else
  {    
    SubjectNode* aStartNodeToMoveDown = myRow2ChildMap[theRow];
    myRow2ChildMap[theRow] = aNode;
    
    for ( int aRow = myRow2ChildMap.size()-1; aRow > theRow+1; aRow-- )
      myRow2ChildMap[aRow] = myRow2ChildMap[aRow-1];
    
    myRow2ChildMap[theRow+1] = aStartNodeToMoveDown;
  }
}

void YACSGui_SwitchNodePage::onMovedUp( const int theUpRow )
{
  SubjectNode* aNodeToMoveDown = myRow2ChildMap[theUpRow];
  SubjectNode* aNodeToMoveUp = myRow2ChildMap[theUpRow+1];
  
  myRow2ChildMap[theUpRow] = aNodeToMoveUp;
  myRow2ChildMap[theUpRow+1] = aNodeToMoveDown;
  
}

void YACSGui_SwitchNodePage::onMovedDown( const int theDownRow )
{
  SubjectNode* aNodeToMoveUp = myRow2ChildMap[theDownRow];
  SubjectNode* aNodeToMoveDown = myRow2ChildMap[theDownRow-1];
  
  myRow2ChildMap[theDownRow] = aNodeToMoveDown;
  myRow2ChildMap[theDownRow-1] = aNodeToMoveUp;
}

void YACSGui_SwitchNodePage::onRemoved( const int theRow )
{
  /*
  SubjectNode* aSNodeToRaise = myRow2ChildMap[theRow];
  Node* aNodeToRaise = aSNodeToRaise->getNode();

  Bloc* aBloc = dynamic_cast<Bloc*>( getNode() );
  if ( !aBloc ) return;

  // remove aNodeToRaise node from the children list (if it is present in it)
  set<Node*> aChildren = aBloc->edGetDirectDescendants();
  set<Node*>::iterator aChildIter = aChildren.begin();
  for( ;aChildIter!=aChildren.end();aChildIter++)
  {
    if ( *aChildIter == aNodeToRaise )
    {
      // get schema engine object
      Proc* aProc = dynamic_cast<Proc*>(aBloc->getRootNode());
      if ( !aProc ) return;
      
      // get schema subject
      //YACSGui_Graph* aGraph = getInputPanel()->getModule()->getGraph( aProc );
      //if ( !aGraph ) return;
      //SubjectProc* aSProc = aGraph->getContext()->getSubjectProc();
      SubjectProc* aSProc = GuiContext::getCurrent()->getSubjectProc()
      if ( !aSProc ) return;
      
      aBloc->edRemoveChild(aNodeToRaise);
      
      try {
	aProc->edAddChild(aNodeToRaise); // raise the removed child to the Proc level
	aSNodeToRaise->reparent(aSProc);
      }
      catch (YACS::Exception& ex) {
	if ( SUIT_MessageBox::warn2(getInputPanel()->getModule()->getApp()->desktop(),
				    tr("WRN_WARNING"),
				    QString(ex.what())+QString("\nThe child node will be deleted. Do you want to continue?"),
				    tr("BUT_YES"), tr("BUT_NO"), 0, 1, 1) == 0 )
	{
	  // delete the old child node
	  if ( YACSGui_Graph* aGraph = getInputPanel()->getModule()->getGraph( aProc ) )
	    if ( YACSPrs_ElementaryNode* anItem = aGraph->getItem(aNodeToRaise) )
	    {
	      anItem->hide();
	      aGraph->removeItem(anItem);
	      delete anItem;
	    }
	  //aNodeToRaise = 0;	
	  delete aNodeToRaise;
	}
	else
	{
	  aBloc->edAddChild(aNodeToRaise);
	  return;
	}
      }
      break;
    }
  }
  */

  if ( theRow == myRow2ChildMap.size()-1 )
    myRow2ChildMap.erase(theRow);
  else
  {
    for ( int aRow = theRow; aRow < myRow2ChildMap.size()-1; aRow++ )
      myRow2ChildMap[aRow] = myRow2ChildMap[aRow+1];
    myRow2ChildMap.erase(myRow2ChildMap.size()-1);
  }
}

void YACSGui_SwitchNodePage::setSwitchCases()
{
  if ( !mySwitchCasesGroupBox ) return;

  SubjectSwitch* aSNode = dynamic_cast<SubjectSwitch*>(mySNode);
  if ( !aSNode ) return;

  Switch* aSwitch = dynamic_cast<Switch*>( getNode() );
  if ( !aSwitch ) return;

  // get schema engine object
  Proc* aProc = dynamic_cast<Proc*>(aSwitch->getRootNode());
  if ( !aProc ) return;

  // get schema subject
  SubjectProc* aSProc = GuiContext::getCurrent()->getSubjectProc();
  if ( !aSProc ) return;
  
  // get gui graph
  YACSGui_Graph* aGraph = getInputPanel()->getModule()->getGraph( aProc );
  if ( !aGraph ) return;

  //printf("myRow2ChildMap:\n");
  //for( map<int,SubjectNode*>::iterator aChildIt = myRow2ChildMap.begin(); aChildIt != myRow2ChildMap.end(); aChildIt++ )
  //  printf( "%d - %s\n", (*aChildIt).first, ((*aChildIt).second)->getNode()->getName().c_str() );

  // remove old children
  Node* aRemovedOldChild;
  set<Node*> aChildren = aSwitch->edGetDirectDescendants();
  set<Node*>::iterator aChildIter = aChildren.begin();
  for( ;aChildIter!=aChildren.end();aChildIter++)
  {
    bool isNeedToRemove = true;
    map<int,SubjectNode*>::iterator aChildIt = myRow2ChildMap.begin();
    for( ;aChildIt!=myRow2ChildMap.end();aChildIt++)
      if ( (*aChildIt).second->getNode() == (*aChildIter) )
      {
	isNeedToRemove = false;
	break;
      }
    if ( !isNeedToRemove ) continue;

    int anId;
    Node* aDefaultNode = 0;
    try {
      aDefaultNode = aSwitch->getChildByShortName(Switch::DEFAULT_NODE_NAME);
    }
    catch (YACS::Exception& ex) {
      SUIT_MessageBox::warn1(getInputPanel()->getModule()->getApp()->desktop(),
			     tr("WRN_WARNING"),
			     QString("Switch has no a default node!"),
			     tr("BUT_OK"));
    }
    if( aDefaultNode && (*aChildIter) == aDefaultNode )
      anId = Switch::ID_FOR_DEFAULT_NODE;
    else
      anId = aSwitch->getRankOfNode( *aChildIter );

    //aSwitch->edRemoveChild(*aChildIter); // the _father of the removed node will be 0 
    aSwitch->edReleaseCase( anId );
    aRemovedOldChild = *aChildIter;
    SubjectNode* aSChild = aSNode->getChild(*aChildIter);

    try {
      aProc->edAddChild(*aChildIter); // raise the removed child to the Proc level
      aSChild->reparent(aSProc);
    }
    catch (YACS::Exception& ex)
    {
      if ( SUIT_MessageBox::warn2(getInputPanel()->getModule()->getApp()->desktop(),
				  tr("WRN_WARNING"),
				  QString(ex.what())+QString("\nThe child node will be deleted. Do you want to continue?"),
				  tr("BUT_YES"), tr("BUT_NO"), 0, 1, 1) == 0 )
      {
	// delete the old child node
	// 1) delete 2D presentation
	aGraph->deletePrs(aSChild);
	// 2) delete corresponding tree view item
	aSNode->update(REMOVE, ProcInvoc::getTypeOfNode( aSChild->getNode() ), aSChild);
	// 3) delete corresponding subject
	aSNode->destroy(aSChild);
      }
      else
      {
	//aSwitch->edAddChild(aRemovedOldChild);
	updateState();
	return;
      }
    }
  }

  // read new children nodes from the table
  YACSGui_Table* aTable = mySwitchCasesGroupBox->Table();
  
  QStringList aChildIds, aChildNames;
  aTable->strings( 0, aChildIds );
  aTable->strings( 1, aChildNames );

  bool isNeedToUpdate = false;
  map<int,SubjectNode*>::iterator aChildIt = myRow2ChildMap.begin();
  for( ;aChildIt!=myRow2ChildMap.end();aChildIt++)
  {
    SubjectNode* aSChild = (*aChildIt).second;
    Node* aChild = aSChild->getNode();
    if( aChild->getFather() == aSwitch )
      continue;

    int aRow = aChildNames.findIndex( QString(aChild->getName()) );
    if ( aRow != -1 )
    {
      int aNewId;
      QString aStr = aChildIds[ aRow ];
      if( aStr == Switch::DEFAULT_NODE_NAME )
	aNewId = Switch::ID_FOR_DEFAULT_NODE;
      else
	aNewId = aStr.toInt();

      if ( Switch* aFather = dynamic_cast<Switch*>(aChild->getFather()) )
      {
	int anId;
	Node* aFatherDefaultNode = 0;
	try {
	  aFatherDefaultNode = aFather->getChildByShortName(Switch::DEFAULT_NODE_NAME);
	}
	catch (YACS::Exception& ex) {}

	if( aFatherDefaultNode && aChild == aFatherDefaultNode )
	{
	  anId = Switch::ID_FOR_DEFAULT_NODE;

	  // If the child is a default node of the another switch - just copy it to the current switch
	  YACS::ENGINE::Catalog* catalog = YACS::ENGINE::getSALOMERuntime()->getBuiltinCatalog();

	  std::string compo;
	  std::string type;
	  if( YACS::ENGINE::ServiceNode* aServiceNode = dynamic_cast<ServiceNode*>( aChild ) )
	  {
	    if ( aServiceNode->getComponent() )
	      compo = aServiceNode->getComponent()->getName();
	    type = aServiceNode->getMethod();
	  }
	  else
	    type = getInputPanel()->getModule()->getNodeType( ProcInvoc::getTypeOfNode(aChild) );

	  std::string name = aChild->getName();
	  if( aSNode->addNode( catalog, compo, type, name, aNewId, true ) )
	  {
	    Node* aNewChild;
	    set<Node*> aNewChildren = aSwitch->edGetDirectDescendants();
	    set<Node*>::iterator aNewChildIt = aNewChildren.begin();
	    set<Node*>::iterator aNewChildItEnd = aNewChildren.end();
	    for( ; aNewChildIt != aNewChildItEnd; aNewChildIt++ )
	      if( (*aNewChildIt)->getName() == name )
	      {
		aNewChild = (*aNewChildIt);
		break;
	      }

	    aGraph->update( aNewChild, dynamic_cast<SubjectSwitch*>( getSNode() ) );
	    aGraph->show();
	  }
	}
	else
	  anId = aFather->getRankOfNode( aChild );

	if( anId == Switch::ID_FOR_DEFAULT_NODE )
	  continue;

	aFather->edReleaseCase( anId );
      
	try {
	  aSwitch->edSetNode(aNewId, aChild);
	  aSChild->reparent(mySNode);
	}
	catch (YACS::Exception& ex)
	{
	  SUIT_MessageBox::warn1(getInputPanel()->getModule()->getApp()->desktop(),
				 tr("WRN_WARNING"),
				 QString(ex.what())+QString("\nRename, please, a new child node and try to add it into the switch once more."),
				 tr("BUT_OK"));
	  
	  aFather->edSetNode(anId, aChild);
	  isNeedToUpdate = true;
	}
      }
      else if ( Bloc* aFather = dynamic_cast<Bloc*>(aChild->getFather()) )
      {
	aFather->edRemoveChild(aChild);
      
	try {
	  aSwitch->edSetNode(aNewId, aChild);
	  aSChild->reparent(mySNode);
	}
	catch (YACS::Exception& ex)
	{
	  SUIT_MessageBox::warn1(getInputPanel()->getModule()->getApp()->desktop(),
				 tr("WRN_WARNING"),
				 QString(ex.what())+QString("\nRename, please, a new child node and try to add it into the switch once more."),
				 tr("BUT_OK"));
	  
	  aFather->edAddChild(aChild);
	  isNeedToUpdate = true;
	}
      }
    }
  }
  
  if ( isNeedToUpdate ) updateState();
     
  // update the block node presentation
  aGraph->update( aSwitch, dynamic_cast<SubjectComposedNode*>( getSNode()->getParent() ) );
}

/*
  Class : YACSGui_BlockNodePage
  Description :  Page for block node properties
*/

YACSGui_BlockNodePage::YACSGui_BlockNodePage( QWidget* theParent, const char* theName, WFlags theFlags )
  : BlockNodePage( theParent, theName, theFlags ),
    YACSGui_NodePage(),
    myIsSelectChild(false)
{
  if ( myDirectChildrenGroupBox )
  {
    myDirectChildrenGroupBox->setTitle( tr("Direct Children") );
    myDirectChildrenGroupBox->HideBtn( YACSGui_PlusMinusGrp::InsertBtn |
				       YACSGui_PlusMinusGrp::UpBtn |
				       YACSGui_PlusMinusGrp::DownBtn );
    myDirectChildrenGroupBox->EnableBtn( YACSGui_PlusMinusGrp::SelectBtn, false );
    YACSGui_Table* aTable = myDirectChildrenGroupBox->Table();
    aTable->setFixedHeight( 100 );
    aTable->setNumCols( 1 );
    aTable->horizontalHeader()->setLabel( 0, tr( "Node name" ) );
    aTable->setCellType( -1, 0, YACSGui_Table::String );

    myDirectChildrenGroupBox->EnableBtn( YACSGui_PlusMinusGrp::AllBtn );
    myDirectChildrenGroupBox->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );

    connect( myNodeName, SIGNAL(textChanged( const QString& )), this, SLOT(onNodeNameChanged( const QString& )) );
    connect( myDirectChildrenGroupBox, SIGNAL(initSelection( const int, const int )),
	     this,                     SLOT(onInitSelection( const int, const int )) );
    connect( myDirectChildrenGroupBox->Table(), SIGNAL(itemCreated( const int, const int )),
	     this,                              SLOT(onNewItem( const int, const int )) );
    
    connect( myDirectChildrenGroupBox, SIGNAL(Added( const int )),     this, SLOT(onAdded( const int )) );
    connect( myDirectChildrenGroupBox, SIGNAL(Inserted( const int )),  this, SLOT(onInserted( const int )) );
    connect( myDirectChildrenGroupBox, SIGNAL(MovedUp( const int )),   this, SLOT(onMovedUp( const int )) );
    connect( myDirectChildrenGroupBox, SIGNAL(MovedDown( const int )), this, SLOT(onMovedDown( const int )) );
    connect( myDirectChildrenGroupBox, SIGNAL(Removed( const int )),   this, SLOT(onRemoved( const int )) );
  }

  // TODO: implement presentation of the block node in collapsed mode
  // temporary solution :
  myCollapsedRadioButton->setEnabled(false );
}

YACSGui_BlockNodePage::~YACSGui_BlockNodePage()
{
  myRow2ChildMap.clear();
  myRowsOfSelectedChildren.clear();
}

void YACSGui_BlockNodePage::setSNode( YACS::HMI::SubjectNode* theSNode )
{
  if ( !theSNode ) return;

  if ( dynamic_cast<Bloc*>( theSNode->getNode() ) )
    YACSGui_NodePage::setSNode( theSNode );
}

YACSGui_InputPanel* YACSGui_BlockNodePage::getInputPanel() const
{
  return dynamic_cast<YACSGui_InputPanel*>( parent()->parent()->parent()->parent() );
}

void YACSGui_BlockNodePage::setChild( YACS::HMI::SubjectNode* theChildNode )
{
  if ( !theChildNode ) return;

  if ( !myDirectChildrenGroupBox ) return;
  YACSGui_Table* aTable = myDirectChildrenGroupBox->Table();

  myIsSelectChild = false;
  myDirectChildrenGroupBox->setOffSelect();
  myDirectChildrenGroupBox->EnableBtn( YACSGui_PlusMinusGrp::SelectBtn, false );

  // sets the child node name into table´s cell
  QStringList aChildNames;
  aTable->strings( 0, aChildNames );
  aChildNames[aTable->currentRow()] = theChildNode->getNode()->getName();
  aTable->setStrings( 0, aChildNames );

  // add theChildNode into the map of children of this block node
  if ( myRow2ChildMap.find(aTable->currentRow()) == myRow2ChildMap.end() )
    myRow2ChildMap.insert( std::make_pair( aTable->currentRow(), theChildNode ) );
  else
    myRow2ChildMap[aTable->currentRow()] = theChildNode;

  bool anExist = false;
  std::list<int>::iterator it = myRowsOfSelectedChildren.begin();
  for ( ; it != myRowsOfSelectedChildren.end(); it++ )
    if ( *it == aTable->currentRow() )
    {
      anExist = true;
      break;
    }
  if ( !anExist )
    myRowsOfSelectedChildren.push_back( aTable->currentRow() );
}

bool YACSGui_BlockNodePage::isSelectChild() const
{
  return myIsSelectChild;
}

void YACSGui_BlockNodePage::setMode( const YACSGui_InputPanel::PageMode theMode )
{
  YACSGui_NodePage::setMode(theMode);

  if ( myMode == YACSGui_InputPanel::EditMode )
  {
    myNodeName->setReadOnly(false);
    
    myDirectChildrenGroupBox->ShowBtn( YACSGui_PlusMinusGrp::SelectBtn |
				       YACSGui_PlusMinusGrp::PlusBtn |
				       YACSGui_PlusMinusGrp::MinusBtn );
    YACSGui_Table* aTable = myDirectChildrenGroupBox->Table();
    aTable->setReadOnly( -1, 0, false );

    ViewModeButtonGroup->show();

    ExecutionGroupBox->hide();
  } 
  else if ( myMode == YACSGui_InputPanel::RunMode )
  {
    myNodeName->setReadOnly(true);
    
    myDirectChildrenGroupBox->HideBtn();
    YACSGui_Table* aTable = myDirectChildrenGroupBox->Table();
    aTable->setReadOnly( -1, 0, true );

    ViewModeButtonGroup->hide();

    ExecutionGroupBox->show();
  }
}

void YACSGui_BlockNodePage::notifyNodeStatus( int theStatus )
{
  if ( myNodeState )
  {
    myNodeState->setPaletteForegroundColor(getColor(theStatus));
    myNodeState->setText(getText(theStatus));
  }
}

void YACSGui_BlockNodePage::notifyNodeProgress()
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

void YACSGui_BlockNodePage::notifyInPortValues( std::list<std::string> theInPortsNames,
						std::list<std::string> theValues )
{
  //printf("==> BlockNodePage : Size of theInPortsNames : %d\n",theInPortsNames.size());
  //printf("==> BlockNodePage : Size of theValues : %d\n",theValues.size());
}

void YACSGui_BlockNodePage::notifyOutPortValues( std::list<std::string> theOutPortsNames,
						 std::list<std::string> theValues )
{
  //printf("==> BlockNodePage : Size of theOutPortsNames : %d\n",theOutPortsNames.size());
  //printf("==> BlockNodePage : Size of theValues : %d\n",theValues.size());
}

void YACSGui_BlockNodePage::onApply()
{
  // Rename a node
  if ( myNodeName && getNode() )
    getNode()->setName( myNodeName->text().latin1() );

  // To remove a new child nodes from its old places in the tree view
  std::list<int>::iterator it = myRowsOfSelectedChildren.begin();
  for ( ; it != myRowsOfSelectedChildren.end(); it++ )
  {
    SubjectNode* aNewSelectedChild = myRow2ChildMap[*it];
    Node* aNode = aNewSelectedChild->getNode();
    try {
      getNode()->getChildByName( aNode->getName() );
    }
    catch (YACS::Exception& ex) {
      // block is not contain nodes with the same name as aNode
      aNewSelectedChild->getParent()->update( REMOVE,
					      ProcInvoc::getTypeOfNode( aNewSelectedChild->getNode() ),
					      aNewSelectedChild );
    }
  }
  
  // Reset the list of direct children
  setDirectChildren();
  
  // Reset the view mode
  // ...

  mySNode->update( EDIT, 0, mySNode );
  
  // clear for next selections
  myRowsOfSelectedChildren.clear();

  updateBlocSize();
}

void YACSGui_BlockNodePage::updateState()
{
  myRow2ChildMap.clear();
  myRowsOfSelectedChildren.clear();

  // Clear input ports table and output ports tables, if they are not empty
  if ( myDirectChildrenGroupBox ) {
    myDirectChildrenGroupBox->Table()->setNumRows( 0 );
    myDirectChildrenGroupBox->EnableBtn( YACSGui_PlusMinusGrp::AllBtn );
  }

  // Set node name
  if ( myNodeName ) myNodeName->setText( getNodeName() );

  // Set node full name (read only)
  if ( myNodeFullName )
    myNodeFullName->setText( getNode()->getRootNode()->getChildName(getNode()) );
  
  // Fill the table of child nodes of the block
  SubjectBloc* aSNode = dynamic_cast<SubjectBloc*>(mySNode);
  if ( aSNode && dynamic_cast<Bloc*>(getNode()) && myDirectChildrenGroupBox )
  {
    QStringList aChildNames;
    
    set<Node*> aChildren = dynamic_cast<Bloc*>(getNode())->edGetDirectDescendants();
    set<Node*>::iterator aChildIt = aChildren.begin();
    int aRow = 0;
    for( ;aChildIt!=aChildren.end();aChildIt++)
    {
      // Collect child names in the list
      aChildNames.append( (*aChildIt)->getName() );
      if ( myRow2ChildMap.find(aRow) == myRow2ChildMap.end() )
	myRow2ChildMap.insert( std::make_pair( aRow, aSNode->getChild(*aChildIt) ) );
      else
	myRow2ChildMap[aRow] = aSNode->getChild(*aChildIt);
      aRow++;
    }

    // Fill "Node name" column
    myDirectChildrenGroupBox->Table()->setStrings( 0, aChildNames, true );

    if ( !aChildNames.empty() ) myDirectChildrenGroupBox->EnableBtn( YACSGui_PlusMinusGrp::AllBtn );
  }

  // Set view mode of the given block node
  // the following method can be used if its needed:
  // YACSGui_Graph* aGraph = getInputPanel()->getModule()->getGraph( myProc );
  // ...
}

void YACSGui_BlockNodePage::onNodeNameChanged( const QString& theName )
{
  if ( myNodeFullName )
  {
    QString anOldName = myNodeFullName->text();
    int aRDotId = anOldName.findRev('.');
    myNodeFullName->setText( anOldName.replace( aRDotId+1, anOldName.length()-(aRDotId+1), theName ) );
  }
}

void YACSGui_BlockNodePage::onInitSelection( const int theRow, const int theCol )
{
  myIsSelectChild = true;
}

void YACSGui_BlockNodePage::onNewItem( const int theRow, const int theCol )
{
  if ( myDirectChildrenGroupBox )
    myDirectChildrenGroupBox->Table()->setReadOnly( theRow, theCol, true );
}

void YACSGui_BlockNodePage::onAdded( const int theRow )
{
  SubjectNode* aNode = 0;
  myRow2ChildMap.insert( std::make_pair( theRow, aNode ) );
}

void YACSGui_BlockNodePage::onInserted( const int theRow )
{
  SubjectNode* aNode = 0;

  if ( myRow2ChildMap.find(theRow) == myRow2ChildMap.end() )
    myRow2ChildMap.insert( std::make_pair( theRow, aNode ) );
  else
  {    
    SubjectNode* aStartNodeToMoveDown = myRow2ChildMap[theRow];
    myRow2ChildMap[theRow] = aNode;
    
    for ( int aRow = myRow2ChildMap.size()-1; aRow > theRow+1; aRow-- )
      myRow2ChildMap[aRow] = myRow2ChildMap[aRow-1];
    
    myRow2ChildMap[theRow+1] = aStartNodeToMoveDown;
  }
}

void YACSGui_BlockNodePage::onMovedUp( const int theUpRow )
{
  SubjectNode* aNodeToMoveDown = myRow2ChildMap[theUpRow];
  SubjectNode* aNodeToMoveUp = myRow2ChildMap[theUpRow+1];
  
  myRow2ChildMap[theUpRow] = aNodeToMoveUp;
  myRow2ChildMap[theUpRow+1] = aNodeToMoveDown;
  
}

void YACSGui_BlockNodePage::onMovedDown( const int theDownRow )
{
  SubjectNode* aNodeToMoveUp = myRow2ChildMap[theDownRow];
  SubjectNode* aNodeToMoveDown = myRow2ChildMap[theDownRow-1];
  
  myRow2ChildMap[theDownRow] = aNodeToMoveDown;
  myRow2ChildMap[theDownRow-1] = aNodeToMoveUp;
}

void YACSGui_BlockNodePage::onRemoved( const int theRow )
{
  /*
  SubjectNode* aSNodeToRaise = myRow2ChildMap[theRow];
  Node* aNodeToRaise = aSNodeToRaise->getNode();

  Bloc* aBloc = dynamic_cast<Bloc*>( getNode() );
  if ( !aBloc ) return;

  // remove aNodeToRaise node from the children list (if it is present in it)
  set<Node*> aChildren = aBloc->edGetDirectDescendants();
  set<Node*>::iterator aChildIter = aChildren.begin();
  for( ;aChildIter!=aChildren.end();aChildIter++)
  {
    if ( *aChildIter == aNodeToRaise )
    {
      // get schema engine object
      Proc* aProc = dynamic_cast<Proc*>(aBloc->getRootNode());
      if ( !aProc ) return;
      
      // get schema subject
      //YACSGui_Graph* aGraph = getInputPanel()->getModule()->getGraph( aProc );
      //if ( !aGraph ) return;
      //SubjectProc* aSProc = aGraph->getContext()->getSubjectProc();
      SubjectProc* aSProc = GuiContext::getCurrent()->getSubjectProc()
      if ( !aSProc ) return;
      
      aBloc->edRemoveChild(aNodeToRaise);
      
      try {
	aProc->edAddChild(aNodeToRaise); // raise the removed child to the Proc level
	aSNodeToRaise->reparent(aSProc);
      }
      catch (YACS::Exception& ex) {
	if ( SUIT_MessageBox::warn2(getInputPanel()->getModule()->getApp()->desktop(),
				    tr("WRN_WARNING"),
				    QString(ex.what())+QString("\nThe child node will be deleted. Do you want to continue?"),
				    tr("BUT_YES"), tr("BUT_NO"), 0, 1, 1) == 0 )
	{
	  // delete the old child node
	  if ( YACSGui_Graph* aGraph = getInputPanel()->getModule()->getGraph( aProc ) )
	    if ( YACSPrs_ElementaryNode* anItem = aGraph->getItem(aNodeToRaise) )
	    {
	      anItem->hide();
	      aGraph->removeItem(anItem);
	      delete anItem;
	    }
	  //aNodeToRaise = 0;	
	  delete aNodeToRaise;
	}
	else
	{
	  aBloc->edAddChild(aNodeToRaise);
	  return;
	}
      }
      break;
    }
  }
  */

  if ( theRow == myRow2ChildMap.size()-1 )
    myRow2ChildMap.erase(theRow);
  else
  {
    for ( int aRow = theRow; aRow < myRow2ChildMap.size()-1; aRow++ )
      myRow2ChildMap[aRow] = myRow2ChildMap[aRow+1];
    myRow2ChildMap.erase(myRow2ChildMap.size()-1);
  }
}

void YACSGui_BlockNodePage::setDirectChildren()
{
  if ( !myDirectChildrenGroupBox ) return;

  SubjectBloc* aSNode = dynamic_cast<SubjectBloc*>(mySNode);
  if ( !aSNode ) return;

  Bloc* aBloc = dynamic_cast<Bloc*>( getNode() );
  if ( !aBloc ) return;

  // get schema engine object
  Proc* aProc = dynamic_cast<Proc*>(aBloc->getRootNode());
  if ( !aProc ) return;

  // get schema subject
  SubjectProc* aSProc = GuiContext::getCurrent()->getSubjectProc();
  if ( !aSProc ) return;
  
  // get gui graph
  YACSGui_Graph* aGraph = getInputPanel()->getModule()->getGraph( aProc );
  if ( !aGraph ) return;

  // remove old children
  Node* aRemovedOldChild;
  set<Node*> aChildren = aBloc->edGetDirectDescendants();
  set<Node*>::iterator aChildIter = aChildren.begin();
  for( ;aChildIter!=aChildren.end();aChildIter++)
  {
    bool isNeedToRemove = true;
    map<int,SubjectNode*>::iterator aChildIt = myRow2ChildMap.begin();
    for( ;aChildIt!=myRow2ChildMap.end();aChildIt++)
      if ( (*aChildIt).second->getNode() == (*aChildIter) )
      {
	isNeedToRemove = false;
	break;
      }
    if ( !isNeedToRemove ) continue;

    aBloc->edRemoveChild(*aChildIter); // the _father of the removed node will be 0 
    aRemovedOldChild = *aChildIter;
    SubjectNode* aSChild = aSNode->getChild(*aChildIter);

    try {
      aProc->edAddChild(*aChildIter); // raise the removed child to the Proc level
      aSChild->reparent(aSProc);
    }
    catch (YACS::Exception& ex)
    {
      if ( SUIT_MessageBox::warn2(getInputPanel()->getModule()->getApp()->desktop(),
				  tr("WRN_WARNING"),
				  QString(ex.what())+QString("\nThe child node will be deleted. Do you want to continue?"),
				  tr("BUT_YES"), tr("BUT_NO"), 0, 1, 1) == 0 )
      {
	// delete the old child node
	// 1) delete 2D presentation
	aGraph->deletePrs(aSChild);
	// 2) delete corresponding tree view item
	aSNode->update(REMOVE, ProcInvoc::getTypeOfNode( aSChild->getNode() ), aSChild);
	// 3) delete corresponding subject
	aSNode->destroy(aSChild);
      }
      else
      {
	aBloc->edAddChild(aRemovedOldChild);
	updateState();
	return;
      }
    }
  }
  
  // read new children nodes from the table
  YACSGui_Table* aTable = myDirectChildrenGroupBox->Table();
  
  QStringList aChildNames;
  aTable->strings( 0, aChildNames );

  bool isNeedToUpdate = false;
  map<int,SubjectNode*>::iterator aChildIt = myRow2ChildMap.begin();
  for( ;aChildIt!=myRow2ChildMap.end();aChildIt++)
  {
    SubjectNode* aSChild = (*aChildIt).second;
    Node* aChild = aSChild->getNode();
    if ( aChildNames.find( QString(aChild->getName()) ) != aChildNames.end() )
    {
      if ( Bloc* aFather = dynamic_cast<Bloc*>(aChild->getFather()) )
      {
	aFather->edRemoveChild(aChild);
      
	try {
	  aBloc->edAddChild(aChild);
	  aSChild->reparent(mySNode);
	}
	catch (YACS::Exception& ex)
	{
	  SUIT_MessageBox::warn1(getInputPanel()->getModule()->getApp()->desktop(),
				 tr("WRN_WARNING"),
				 QString(ex.what())+QString("\nRename, please, a new child node and try to add it into the block once more."),
				 tr("BUT_OK"));
	  
	  aFather->edAddChild(aChild);
	  isNeedToUpdate = true;
	}
      }
    }
  }
  
  if ( isNeedToUpdate ) updateState();
     
  // update the block node presentation
  aGraph->update( aBloc, dynamic_cast<SubjectComposedNode*>( getSNode()->getParent() ) );
}

/*
  Class : YACSGui_LinkPage
  Description :  Page for link properties
*/

YACSGui_LinkPage::YACSGui_LinkPage( QWidget* theParent,  YACSGui_Module* theModule )
  : YACSGui_PropertyPage( theParent, theModule ),
    myOut( 0 ),
    myIn( 0 )
{
  
}

YACSGui_LinkPage::~YACSGui_LinkPage()
{
}

void YACSGui_LinkPage::onApply()
{
}
