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
#include <DataNode.hxx>
#include <PresetNode.hxx>
#include <OutNode.hxx>
#include <StudyNodes.hxx>
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


QColor getColor( int theStatus )
{
  QColor aColor;
  switch (theStatus)
    {
    case UNDEFINED:    aColor = Qt::lightGray;     break;
    case INVALID:      aColor = Qt::red;           break;
    case READY:        aColor = Qt::green;         break;
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

std::string portValueType( YACS::ENGINE::TypeCode* theTC )
{
  std::string aType = "";
  switch ( theTC->kind() )
    {
    case Double: aType = "double";     break;
    case Int:    aType = "int";        break;
    case String: aType = "string";     break;
    case Bool:   aType = "bool";       break;
    case Objref: aType = "objref";     break;
    case Sequence: aType = "sequence"; break;
    case Array:    aType = "array";    break;
    case Struct:   aType = "struct";   break;
    default:                           break;
    }
  return aType;
}

QString getText( int theStatus )
{
  QString aText;
  switch (theStatus)
    {
    case UNDEFINED:    aText = "UNDEFINED";    break;
    case INVALID:      aText = "INVALID";      break;
    case READY:        aText = "READY";        break;
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
     myModule(theModule),
     myApplyBtn(0),
     myCancelBtn(0)
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
  createPage( YACSGui_InputPanel::InDataNodeId );
  createPage( YACSGui_InputPanel::OutDataNodeId );
  createPage( YACSGui_InputPanel::InStudyNodeId );
  createPage( YACSGui_InputPanel::OutStudyNodeId );
  createPage( YACSGui_InputPanel::InlineNodeId );
  createPage( YACSGui_InputPanel::ServiceNodeId );
  createPage( YACSGui_InputPanel::ForLoopNodeId );
  createPage( YACSGui_InputPanel::ForEachLoopNodeId );
  createPage( YACSGui_InputPanel::WhileLoopNodeId );
  createPage( YACSGui_InputPanel::SwitchNodeId );
  createPage( YACSGui_InputPanel::BlockNodeId );
  createPage( YACSGui_InputPanel::LinkId );
  createPage( YACSGui_InputPanel::DataTypeId );

  //  myMainLayout->addStretch( 1 );

  // Add "Apply" and "Close" buttons
  QGroupBox* aGrpBtns = new QGroupBox( aMainFrame, "GroupButtons" );
  aGrpBtns->setColumnLayout(0, Qt::Vertical );
  aGrpBtns->layout()->setSpacing( 0 ); aGrpBtns->layout()->setMargin( 0 );
  aGrpBtns->setFrameStyle( QFrame::NoFrame );
  //aGrpBtns->setFrameStyle( QFrame::StyledPanel );
  QHBoxLayout* aGrpBtnsLayout = new QHBoxLayout( aGrpBtns->layout() );
  aGrpBtnsLayout->setAlignment( Qt::AlignTop );
  aGrpBtnsLayout->setSpacing( 0 ); aGrpBtnsLayout->setMargin( 0 );

  myApplyBtn = new QPushButton( tr( "APPLY_BTN" ), aGrpBtns );
  connect( myApplyBtn, SIGNAL( clicked() ), this, SLOT( onApply() ) );

  myCancelBtn = new QPushButton( tr( "CLOSE_BTN" ), aGrpBtns );
  connect( myCancelBtn, SIGNAL( clicked() ), this, SLOT( onClose() ) );

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
  myMainBtns.clear();
  myPages.clear();
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

  QWidget* aWid = 0;
  return aWid;
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
// name    : removePage
// Purpose : 
//=======================================================================
void YACSGui_InputPanel::removePage( QWidget* thePage )
{
  if ( !thePage ) return;

  int anId = getPageId(thePage);

  if ( myMainBtns.find(anId) != myMainBtns.end() )
  {
    QPushButton* aBtn = myMainBtns[anId];
    myMainBtns.erase(anId);
    delete aBtn;
  }
    
  if ( myPages.find(anId) != myPages.end() )
    myPages.erase(anId);
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
  DEBTRACE("YACSGui_InputPanel::isVisible " << i);
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
// name    : getVisiblePagesIds
// Purpose : 
//=======================================================================
std::list<int> YACSGui_InputPanel::getVisiblePagesIds()
{
  list<int> aRetList;

  for ( QMap< int, QWidget* >::const_iterator anIter = myPages.begin(); anIter != myPages.end(); ++anIter )
  {
    int anId = anIter.key();
    if ( isVisible(anId) ) aRetList.push_back(anId);
  }

  return aRetList;
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
  case InDataNodeId:
  case OutDataNodeId:
  case InStudyNodeId:
  case OutStudyNodeId:
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
  case DataTypeId:
    {
      if ( YACSGui_DataTypePage* page = dynamic_cast<YACSGui_DataTypePage*>(myPages[thePageId]) )
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

//================================================================
// Function : emitApply
// Purpose  : 
//================================================================
void YACSGui_InputPanel::emitApply( const int theId )
{
  emit Apply( theId );
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
// name    : onApplyEnabled
// Purpose : SLOT. Called when "Apply" button should be disabled or enabled.
//=======================================================================
void YACSGui_InputPanel::onApplyEnabled( bool enable )
{
  myApplyBtn->setEnabled(enable);
}

//=======================================================================
// name    : onApply
// Purpose : SLOT. Called when "Apply" button clicked. Emit signal with 
//           Corresponding id.
//=======================================================================
void YACSGui_InputPanel::onApply()
{
  DEBTRACE("YACSGui_InputPanel::onApply");
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
      case InDataNodeId:
      case OutDataNodeId: 
      case InStudyNodeId:
      case OutStudyNodeId: {
  DEBTRACE("YACSGui_InputPanel::onApply");
        YACSGui_DataNodePage* page = dynamic_cast<YACSGui_DataNodePage*>(myPages[anId]);
        if(page)
          page->onApply();
        break;
      }
      case InlineNodeId: {
  DEBTRACE("YACSGui_InputPanel::onApply");
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
      case DataTypeId: {
        YACSGui_DataTypePage* page = dynamic_cast<YACSGui_DataTypePage*>(myPages[anId]);
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
// name    : onClose
// Purpose : SLOT. Called when "Close" button clicked.
//=======================================================================
void YACSGui_InputPanel::onClose()
{
  hide();
  if(getModule()->getGuiMode() == YACSGui_Module::NewMode) {
    getModule()->setGuiMode(YACSGui_Module::EditMode);
    if(getModule()->activeTreeView())
      getModule()->activeTreeView()->syncPageTypeWithSelection();
  }
  onApplyEnabled(true);
}

//=======================================================================
// name    : onNotifyNodeStatus
// Purpose : 
//=======================================================================
void YACSGui_InputPanel::onNotifyNodeStatus( int theNodeId, int theStatus )
{
  DEBTRACE("YACSGui_InputPanel::onNotifyNodeStatus: " << theNodeId << " " << theStatus);
  if ( Node::idMap.count(theNodeId) == 0 ) return;
  Node* aNode= Node::idMap[theNodeId];
  
  QMap< int, QWidget* >::const_iterator anIter;
  for ( anIter = myPages.begin(); anIter != myPages.end(); ++anIter )
  {
    int anId = anIter.key();
    if ( isVisible(anId) )
    {
      switch(anId) {
      case InDataNodeId:
      case OutDataNodeId:
      case InStudyNodeId:
      case OutStudyNodeId: {
        YACSGui_DataNodePage* page = dynamic_cast<YACSGui_DataNodePage*>(myPages[anId]);
        if(page && page->getNode() == aNode)
          page->notifyNodeStatus(theStatus);
        break;
      }
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
					       std::map<std::string,std::string> theInPortName2Value )
{
  DEBTRACE("onNotifyInPortValues: " << theNodeId);
  if ( Node::idMap.count(theNodeId) == 0 ) return;
  Node* aNode= Node::idMap[theNodeId];
  
  QMap< int, QWidget* >::const_iterator anIter;
  for ( anIter = myPages.begin(); anIter != myPages.end(); ++anIter )
  {
    int anId = anIter.key();
    if ( isVisible(anId) )
    {
      switch(anId) {
      case InDataNodeId:
      case OutDataNodeId:
      case InStudyNodeId:
      case OutStudyNodeId: {
        YACSGui_DataNodePage* page = dynamic_cast<YACSGui_DataNodePage*>(myPages[anId]);
        if(page && page->getNode() == aNode)
          page->notifyInPortValues(theInPortName2Value);
        break;
      }
      case InlineNodeId: {
        YACSGui_InlineNodePage* page = dynamic_cast<YACSGui_InlineNodePage*>(myPages[anId]);
        if(page && page->getNode() == aNode)
          page->notifyInPortValues(theInPortName2Value);
        break;
      }
      case ServiceNodeId: {
        YACSGui_ServiceNodePage* page = dynamic_cast<YACSGui_ServiceNodePage*>(myPages[anId]);
        if(page && page->getNode() == aNode)
          page->notifyInPortValues(theInPortName2Value);
        break;
      }
      case ForLoopNodeId: {
        YACSGui_ForLoopNodePage* page = dynamic_cast<YACSGui_ForLoopNodePage*>(myPages[anId]);
        if(page && page->getNode() == aNode)
          page->notifyInPortValues(theInPortName2Value);
        break;
      }
      case ForEachLoopNodeId: {
        YACSGui_ForEachLoopNodePage* page = dynamic_cast<YACSGui_ForEachLoopNodePage*>(myPages[anId]);
        if(page && page->getNode() == aNode)
          page->notifyInPortValues(theInPortName2Value);
        break;
      }
      case WhileLoopNodeId: {
        YACSGui_WhileLoopNodePage* page = dynamic_cast<YACSGui_WhileLoopNodePage*>(myPages[anId]);
        if(page && page->getNode() == aNode)
          page->notifyInPortValues(theInPortName2Value);
        break;
      }
      case SwitchNodeId: {
        YACSGui_SwitchNodePage* page = dynamic_cast<YACSGui_SwitchNodePage*>(myPages[anId]);
        if(page && page->getNode() == aNode)
          page->notifyInPortValues(theInPortName2Value);
        break;
      }
      case BlockNodeId: {
        YACSGui_BlockNodePage* page = dynamic_cast<YACSGui_BlockNodePage*>(myPages[anId]);
        if(page && page->getNode() == aNode)
          page->notifyInPortValues(theInPortName2Value);
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
						std::map<std::string,std::string> theOutPortName2Value )
{
  DEBTRACE("YACSGui_InputPanel::onNotifyOutPortValues" << theNodeId);
  if ( Node::idMap.count(theNodeId) == 0 ) return;
  Node* aNode= Node::idMap[theNodeId];
  
  QMap< int, QWidget* >::const_iterator anIter;
  for ( anIter = myPages.begin(); anIter != myPages.end(); ++anIter )
  {
    int anId = anIter.key();
    if ( isVisible(anId) )
    {
      switch(anId) {
      case InDataNodeId:
      case OutDataNodeId:
      case InStudyNodeId:
      case OutStudyNodeId: {
        YACSGui_DataNodePage* page = dynamic_cast<YACSGui_DataNodePage*>(myPages[anId]);
        DEBTRACE("Notify");
        if(page && page->getNode() == aNode)
          page->notifyOutPortValues(theOutPortName2Value);
        break;
      }
      case InlineNodeId: {
        YACSGui_InlineNodePage* page = dynamic_cast<YACSGui_InlineNodePage*>(myPages[anId]);
        DEBTRACE("Notify");
        if(page && page->getNode() == aNode)
          page->notifyOutPortValues(theOutPortName2Value);
        break;
      }
      case ServiceNodeId: {
        YACSGui_ServiceNodePage* page = dynamic_cast<YACSGui_ServiceNodePage*>(myPages[anId]);
        DEBTRACE("Notify");
        if(page && page->getNode() == aNode)
          page->notifyOutPortValues(theOutPortName2Value);
        break;
      }
      case ForLoopNodeId: {
        YACSGui_ForLoopNodePage* page = dynamic_cast<YACSGui_ForLoopNodePage*>(myPages[anId]);
        if(page && page->getNode() == aNode)
          page->notifyOutPortValues(theOutPortName2Value);
        break;
      }
      case ForEachLoopNodeId: {
        YACSGui_ForEachLoopNodePage* page = dynamic_cast<YACSGui_ForEachLoopNodePage*>(myPages[anId]);
        if(page && page->getNode() == aNode)
          page->notifyOutPortValues(theOutPortName2Value);
        break;
      }
      case WhileLoopNodeId: {
        YACSGui_WhileLoopNodePage* page = dynamic_cast<YACSGui_WhileLoopNodePage*>(myPages[anId]);
        if(page && page->getNode() == aNode)
          page->notifyOutPortValues(theOutPortName2Value);
        break;
      }
      case SwitchNodeId: {
        YACSGui_SwitchNodePage* page = dynamic_cast<YACSGui_SwitchNodePage*>(myPages[anId]);
        if(page && page->getNode() == aNode)
          page->notifyOutPortValues(theOutPortName2Value);
        break;
      }
      case BlockNodeId: {
        YACSGui_BlockNodePage* page = dynamic_cast<YACSGui_BlockNodePage*>(myPages[anId]);
        DEBTRACE("Notify");
        if(page && page->getNode() == aNode)
          page->notifyOutPortValues(theOutPortName2Value);
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
  DEBTRACE("YACSGui_InputPanel::updateState()");
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
  DEBTRACE("YACSGui_InputPanel::createPage");
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
  case InDataNodeId:
  case OutDataNodeId:
  case InStudyNodeId:
  case OutStudyNodeId:
    page = new YACSGui_DataNodePage(thePageId, aMainFrame );
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
  case DataTypeId:
    page = new YACSGui_DataTypePage( aMainFrame );
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

  if ( myApplyBtn && myCancelBtn )
  {
    int anIndex;
    if ( thePageId == ContainerId ) anIndex = 0;
    if ( thePageId == ComponentId ) anIndex = 2;
    else anIndex = myMainLayout->findWidget(myApplyBtn->parentWidget()) - 1;
    
    myMainLayout->insertWidget( anIndex, page );
    myMainLayout->insertWidget( anIndex, button );
  }
  else
  {
    myMainLayout->insertWidget( -1, button );
    myMainLayout->insertWidget( -1, page );
  }
  
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
  case InDataNodeId:
  case OutDataNodeId:
  case InStudyNodeId:
  case OutStudyNodeId:
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
  case DataTypeId:
    myMainBtns[thePageId]->setText( tr( "Data Type Properties" ) );
    break;
  default:
    break;
  }
  
  // Connect signals and slots
  if ( myMainBtns.contains( thePageId ) )
    connect( myMainBtns[ thePageId ], SIGNAL( toggled( bool ) ), SLOT( onMainBtn( bool ) ) );
}

