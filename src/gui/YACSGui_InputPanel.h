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

#ifndef YACSGui_InputPanel_HeaderFile
#define YACSGui_InputPanel_HeaderFile

#include "yacsconfig.h"
#include "schemapage.h"
#include "containerpage.h"
#include "componentpage.h"
#include "inlinenodepage.h"
#include "forloopnodepage.h"
#include "foreachloopnodepage.h"
#include "whileloopnodepage.h"
#include "switchnodepage.h"
#include "blocknodepage.h"
#include "servicenodepage.h"

#include <SalomeContainer.hxx>
#include <ComponentInstance.hxx>
#include <Proc.hxx>
#include <guiObservers.hxx>

#include <qdockwindow.h>
#include <qlistview.h>
#include <qdialog.h>

#ifdef HAVE_QEXTSCINTILLA_H
#include <qextscintilla.h>
#include <qextscintillalexerpython.h>
#endif

class YACSGui_Module;
class YACSGui_PropertyPage;
class YACSGui_Table;

class QPushButton;
class QScrollView;
class QVBoxLayout;
class QLineEdit;
class QButtonGroup;
class QComboBox;

/*
  Class       : YACSGui_InputPanel
  Description : Dockable window containing push buttons and corresponding widgets
*/
class YACSGui_InputPanel : public QDockWindow
{
  Q_OBJECT

protected:

  class BtnStyle; 

public:
  
  enum { ContainerId=0, ComponentId, SchemaId, InlineNodeId, ServiceNodeId,
	 ForLoopNodeId, ForEachLoopNodeId, WhileLoopNodeId, SwitchNodeId, BlockNodeId,
	 LinkId, LastId = LinkId };

  typedef enum { EditMode, RunMode, NewMode } PageMode;

public: 
  YACSGui_InputPanel( YACSGui_Module* );
  virtual ~YACSGui_InputPanel();

  void                                setBtnText( const QString& theText, 
                                                  const int thePageId );

  QWidget*                            getPage( const int thePageId, const bool theWithCreate = true );
  YACSGui_Module*                     getModule() const { return myModule; }
  int                                 getPageId( QWidget* thePage ) const;
  int                                 insertPage( QWidget* thePage, 
                                                  const int thePageId = -1,
                                                  const int theBeforePageId = -1, 
                                                  const int theStretch = 0 ) const;
  void                                removePage( QWidget* thePage );

  virtual QSize                       sizeHint() const;

  bool                                isOn( const int ) const;
  void                                setOn( const bool, const int );

  bool                                isVisible( const int ) const;
  void                                setVisible( const bool , const int );
  void                                setExclusiveVisible( const bool , const std::list<int> );

  void                                setMode( const YACSGui_InputPanel::PageMode, const int );

  QSize                               getPrefferedSize();

public slots:
  void                                onNotifyNodeStatus( int theNodeId, int theStatus );
  void                                onNotifyInPortValues( int theNodeId,
							    std::map<std::string,std::string> theInPortName2Value );
  void                                onNotifyOutPortValues( int theNodeId,
							     std::map<std::string,std::string> theOutPortName2Value );
  void                                onApplyEnabled( bool );
  
signals:
  void                                Apply( const int theId );

private slots:
  void                                onMainBtn( bool );
  void                                onApply();
  void                                onClose();

private:
  void                                updateState();
  void                                createPage( const int thePageId );

private:
  QMap< int, QPushButton* >           myMainBtns;
  QMap< int, QWidget* >               myPages;

  QVBoxLayout*                        myMainLayout;
  QPushButton*                        myApplyBtn;
  QPushButton*                        myCancelBtn;

  YACSGui_Module*                     myModule;
};


//=================== rnv (to be removed) ==========================================================

/*
  Class : YACSGui_PropertyPage
  Description : This class is used for parameters of input panel.
                All pages of input panel must be derived from it
*/

class  YACSGui_PropertyPage : public QFrame
{
  Q_OBJECT

public:
                                      YACSGui_PropertyPage( QWidget*, YACSGui_Module* );
  virtual                             ~YACSGui_PropertyPage();

  QScrollView*                        getView() const { return myView; }

signals:

public slots:
  virtual void                        onApply();

protected:
  YACSGui_Module*                     myModule;
  QScrollView*                        myView;
};

//==================================================================================================

/*
  Class : YACSGui_YACSORBContainerDialog
  Description : Dialog for container properties used for loading YACSGui ORAB engine
*/
class YACSGui_YACSORBContainerDialog : public QDialog
{
  Q_OBJECT

public:
                                      YACSGui_YACSORBContainerDialog( QWidget* theParent = 0,
								      const char* theName = 0,
								      WFlags theFlags = 0 );
  virtual                             ~YACSGui_YACSORBContainerDialog();

  Engines::MachineParameters          getParams() const { return myParams; }

public slots:
  virtual void                        onApply();

protected slots:
  void                                onShowAdvanced();
  void                                fillHostNames() const; 

private:
  void                                updateState();
  Engines::MachineParameters          myParams;
  ContainerPage*                      myPage;
};

//==================================================================================================

/*
  Class : YACSGui_ContainerPage
  Description : Page for container properties
*/
class YACSGui_ContainerPage : public ContainerPage, public YACS::HMI::GuiObserver
{
  Q_OBJECT

public:
                                      YACSGui_ContainerPage( QWidget* theParent = 0,
							     const char* theName = 0,
							     WFlags theFlags = 0 );
  virtual                             ~YACSGui_ContainerPage();

  virtual void                        select( bool isSelected );
  virtual void                        update( YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son);
  
  YACSGui_InputPanel*                 getInputPanel() const;

  void                                setSContainer( YACS::HMI::SubjectContainer* theSContainer );

  YACS::ENGINE::Container*            getContainer() const;
  YACS::ENGINE::SalomeContainer*      getSalomeContainer() const;

  QString                             getContainerName() const;
  void                                setContainerName( const QString& );
  
  void                                fillHostNames() const;
  QString                             getHostName() const;
  void                                setHostName( const QString& );

  int                                 getMemMb() const;
  void                                setMemMb(const int);

  int                                 getCpuClock() const;
  void                                setCpuClock(const int);

  int                                 getNbProcPerNode() const;
  void                                setNbProcPerNode(const int);

  int                                 getNbNode() const;
  void                                setNbNode(const int);

  bool                                getMpiUsage() const;
  void                                setMpiUsage(const bool);

  QString                             getParallelLib() const;
  void                                setParallelLib( const QString& );

  int                                 getNbComponentNodes() const;
  void                                setNbComponentNodes(const int);
  
public slots:
  virtual void                        onApply();

protected slots:
  void                                onShowAdvanced(); 

private:
  void                                updateState();
  YACS::HMI::SubjectContainer*        mySContainer;
};

/*
  Class : YACSGui_ComponentPage
  Description : Page for component properties
*/
class YACSGui_ComponentPage : public ComponentPage, public YACS::HMI::GuiObserver
{
  Q_OBJECT

public: 
  typedef enum { SALOME, SALOMEPython, CPP, CORBA } ComponentType;

public:
                                      YACSGui_ComponentPage( QWidget* theParent = 0,
							     const char* theName = 0,
							     WFlags theFlags = 0 );
  virtual                             ~YACSGui_ComponentPage();

  virtual void                        select( bool isSelected );
  virtual void                        update( YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son);

  YACSGui_InputPanel*                 getInputPanel() const;

  void                                setSComponent( YACS::HMI::SubjectComponent* theSComponent, bool isEdit = true );

  YACS::ENGINE::ComponentInstance*    getComponent() const;

  QString                             getComponentName() const;

  void                                setComponentType();
  ComponentType                       getComponentType() const;

  QString                             getContainerName() const;
  void                                setContainer();

public slots:
  virtual void                        onApply(); 

protected slots:
  void                                onContainerChanged( const QString& );

private:
  void                                fillComponentNames();
  void                                fillContainerNames();
  void                                updateState();
  YACS::HMI::SubjectComponent*        mySComponent;
  ComponentType                       myType;
  bool                                myEditComp;
};

/*
  Class : YACSGui_SchemaPage
  Description : Page for schema properties
*/
class YACSGui_SchemaPage : public SchemaPage, public YACS::HMI::GuiObserver
{
  Q_OBJECT

public:
                                      YACSGui_SchemaPage( QWidget* theParent = 0,
							  const char* theName = 0,
							  WFlags theFlags = 0 );
  virtual                             ~YACSGui_SchemaPage();

  virtual void                        select( bool isSelected );
  virtual void                        update( YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son);
  
  void                                setSProc( YACS::HMI::SubjectProc* theProc );

  QString                             getSchemaName() const;
  void                                setSchemaName( const QString& );

  int                                 getViewMode() const;
  void                                setViewMode( const int );
  YACSGui_InputPanel*                 getInputPanel() const;

  void                                setMode( const YACSGui_InputPanel::PageMode );

public slots:
  virtual void                        onAddAllNextToRun();
  virtual void                        onRemoveAllNextToRun();
  virtual void                        onNextStepClicked( QListViewItem* theItem ); 

  virtual void                        onNotifyNextSteps( std::list<std::string> theNextSteps );

  virtual void                        onApply();

private:
  void                                updateState();

  YACS::HMI::SubjectProc*             mySProc;
  YACSGui_InputPanel::PageMode        myMode;

  std::map<QCheckListItem*, std::string> myMapNextSteps;
};

/*
  Class : YACSGui_NodePage
  Description : Basic page for node properties
*/
class YACSGui_NodePage : public YACS::HMI::GuiObserver
{
public: 
  typedef enum { SALOMEService, CORBAService, CPPNode, ServiceInline, XMLNode,
		 InlineFunction, InlineScript,
		 FOR, FOREACH, WHILE, SWITCH, Block, Unknown } NodeType;

  typedef enum { UnknownType,
		 Input, SeqAnyInput, AnyInput, ConditionInput, InputCorba,  InputPy, InputXml,
                 InputBasicStream, InputCalStream, InputPalmStream, InputDataStream,
                 Output, AnyOutput, OutputCorba, OutputPy, OutputXml,
		 OutputBasicStream, OutputCalStream, OutputPalmStream, OutputDataStream } PortType;

public:
                                      YACSGui_NodePage();
				      ~YACSGui_NodePage();
				      
  virtual void                        select( bool isSelected );
  virtual void                        update( YACS::HMI::GuiEvent event, int type, YACS::HMI::Subject* son);

  YACSGui_InputPanel*                 getInputPanel() const;
  
  virtual void                        setSNode( YACS::HMI::SubjectNode* theSNode );

  YACS::ENGINE::Node*                 getNode() const;
  YACS::HMI::SubjectNode*             getSNode() const { return mySNode; }

  QString                             getNodeName() const;
  void                                setNodeName( const QString& );

  virtual void                        setMode( const YACSGui_InputPanel::PageMode );

  void                                setValueCellValidator( YACSGui_Table* theTable, int theRow );

  virtual void                        notifyNodeStatus( int theStatus ) {}
  virtual void                        notifyNodeProgress() {}
  virtual void                        notifyInPortValues( std::map<std::string,std::string> theInPortName2Value ) {}
  virtual void                        notifyOutPortValues( std::map<std::string,std::string> theOutPortName2Value ) {}
  virtual void                        notifyNodeCreateBody( YACS::HMI::Subject* theSubject ) {}
  virtual void                        notifyNodeCreateNode( YACS::HMI::Subject* theSubject ) {}

protected:
  virtual void                        updateState();
  void                                updateBlocSize();

  PortType                            getDataPortType( YACS::ENGINE::Port* ) const;
  QString                             getPortValue( YACS::ENGINE::Port* ) const;

  YACS::HMI::SubjectNode*             mySNode;
  NodeType                            myType;
  YACSGui_InputPanel::PageMode        myMode;
};

/*
  Class : YACSGui_InlineNodePage
  Description : Page for inline function and inline service nodes properties
*/
class YACSGui_InlineNodePage : public InlineNodePage, public YACSGui_NodePage
{
  Q_OBJECT

public:
                                      YACSGui_InlineNodePage( QWidget* theParent = 0,
							      const char* theName = 0,
							      WFlags theFlags = 0 );
  virtual                             ~YACSGui_InlineNodePage();

  virtual void                        setSNode( YACS::HMI::SubjectNode* theSNode );

  virtual void                        setMode( const YACSGui_InputPanel::PageMode );

  virtual void                        notifyNodeStatus( int theStatus );
  virtual void                        notifyNodeProgress();
  virtual void                        notifyInPortValues( std::map<std::string,std::string> theInPortName2Value );
  virtual void                        notifyOutPortValues( std::map<std::string,std::string> theOutPortName2Value );

public slots:
  virtual void                        onApply();

protected:
  virtual void                        updateState();
  virtual void                        hideEvent( QHideEvent* );

  QString                             scriptText() const;
  void                                setScriptText( const QString& );

protected slots:
  void                                onNodeNameChanged( const QString& );
  void                                onValueChanged( int, int );
  void                                onSearch();
  void                                onInserted( const int theRow );
  void                                onMovedUp( const int theUpRow );
  void                                onMovedDown( const int theDownRow );
  void                                onRemoved( const int theRow );
  
protected:
#ifdef HAVE_QEXTSCINTILLA_H
  QextScintilla *_myTextEdit;
#endif

private:
  void                                fillInputPortsTable();
  void                                fillOutputPortsTable();
  QString                             getPortType( YACS::ENGINE::Port* ) const;
  void                                setInputPorts();
  void                                setOutputPorts();
  YACS::ENGINE::TypeCode*             createTypeCode( YACS::ENGINE::DynType, YACSGui_Table*, int );
  bool                                isStored( YACS::ENGINE::Port* );
  void                                resetStoredPortsMaps();
  void                                resetPLists();
  void                                resetIPLists();
  void                                resetOPLists();
  void                                orderPorts( bool withFilling = false );

  std::map<int,YACS::ENGINE::InPort*>  myRow2StoredInPorts;
  std::map<int,YACS::ENGINE::OutPort*> myRow2StoredOutPorts;
  QPtrList<YACS::ENGINE::InPort>       myIPList;
  QPtrList<YACS::ENGINE::OutPort>      myOPList;
  bool                                 myIsNeedToReorder;
  int                                  myPara;
  int                                  myIndex;
};

/*
  Class : YACSGui_ServiceNodePage
  Description : Page for SALOME and CORBA service nodes properties
*/
class YACSGui_ServiceNodePage : public ServiceNodePage, public YACSGui_NodePage
{
  Q_OBJECT

public:
                                      YACSGui_ServiceNodePage( QWidget* theParent = 0,
							       const char* theName = 0,
							       WFlags theFlags = 0 );
  virtual                             ~YACSGui_ServiceNodePage();

  virtual void                        setSCNode( YACS::HMI::SubjectComposedNode* theSCNode );
  virtual void                        setSNode( YACS::HMI::SubjectNode* theSNode );

  virtual void                        setComponent( YACS::ENGINE::ComponentInstance* theComponent );

  bool                                isSelectComponent() const;

  virtual void                        setMode( const YACSGui_InputPanel::PageMode );

  virtual void                        notifyNodeStatus( int theStatus );
  virtual void                        notifyNodeProgress();
  virtual void                        notifyInPortValues( std::map<std::string,std::string> theInPortName2Value );
  virtual void                        notifyOutPortValues( std::map<std::string,std::string> theOutPortName2Value );

signals:
  void                                enableApply( bool );

public slots:
  virtual void                        onApply();
 
protected:
  virtual void                        updateState();
  virtual void                        hideEvent( QHideEvent* );

  QString                             scriptText() const;
  void                                setScriptText( const QString& );

protected slots:
  void                                onVisibilityChanged( bool );
  void                                onNodeNameChanged( const QString& );
  void                                onMethodChanged( const QString& );
  void                                onValueChanged( int, int );
  void                                onSearch();
  void                                onBrowse( );
  void                                onCatalogMethodClicked( QListViewItem* );
  void                                onCatalogChanged( int );
  
private:
  void                                updateServices( const QString& = QString::null );
  void                                fillInputPortsTable( YACS::ENGINE::Node* );
  void                                fillOutputPortsTable( YACS::ENGINE::Node* );
  QString                             getPortType( YACS::ENGINE::Port* ) const;
  void                                setInputPorts();
  void                                setOutputPorts();
  YACS::ENGINE::TypeCode*             createTypeCode( YACS::ENGINE::DynType, YACSGui_Table*, int );
  void                                buildTree( YACS::ENGINE::Node* theNode, QListViewItem* theParent );

 protected:
#ifdef HAVE_QEXTSCINTILLA_H
  QextScintilla *_myTextEdit;
#endif

private:
  YACS::ENGINE::ComponentInstance*    myComponent;
  QString                             myComponentName;
  bool                                myMethodChanged;
  YACS::HMI::SubjectComposedNode*     mySCNode;
  std::map<QListViewItem*,std::pair<std::string,std::string> > myServiceMap;
  std::map<QListViewItem*, YACS::ENGINE::Node* > myProcNodesMap;
  std::string                         myProcName;
  int                                 myPara;
  int                                 myIndex;
};

/*
  Class : YACSGui_ForLoopNodePage
  Description : Page for FOR node properties
*/
class YACSGui_ForLoopNodePage : public ForLoopNodePage, public YACSGui_NodePage
{
  Q_OBJECT

public:
                                      YACSGui_ForLoopNodePage( QWidget* theParent = 0,
							       const char* theName = 0,
							       WFlags theFlags = 0 );
  virtual                             ~YACSGui_ForLoopNodePage();

  virtual void                        setSNode( YACS::HMI::SubjectNode* theSNode );

  virtual void                        setMode( const YACSGui_InputPanel::PageMode );

  virtual void                        notifyNodeStatus( int theStatus );
  virtual void                        notifyNodeProgress();
  virtual void                        notifyInPortValues( std::map<std::string,std::string> theInPortName2Value );
  virtual void                        notifyOutPortValues( std::map<std::string,std::string> theOutPortName2Value );
  virtual void                        notifyNodeCreateBody( YACS::HMI::Subject* theSubject );

public slots:
  virtual void                        onApply();

protected:
  virtual void                        updateState();

protected slots:
  void                                onNodeNameChanged( const QString& );
};

/*
  Class : YACSGui_ForEachLoopNodePage
  Description : Page for FOR node properties
*/
class YACSGui_ForEachLoopNodePage : public ForEachLoopNodePage, public YACSGui_NodePage
{
  Q_OBJECT

public:
                                      YACSGui_ForEachLoopNodePage( QWidget* theParent = 0,
								   const char* theName = 0,
								   WFlags theFlags = 0 );
  virtual                             ~YACSGui_ForEachLoopNodePage();

  virtual void                        setSNode( YACS::HMI::SubjectNode* theSNode );

  virtual void                        setMode( const YACSGui_InputPanel::PageMode );

  virtual void                        notifyNodeStatus( int theStatus );
  virtual void                        notifyNodeProgress();
  virtual void                        notifyInPortValues( std::map<std::string,std::string> theInPortName2Value );
  virtual void                        notifyOutPortValues( std::map<std::string,std::string> theOutPortName2Value );
  virtual void                        notifyNodeCreateBody( YACS::HMI::Subject* theSubject );

public slots:
  virtual void                        onApply();

protected:
  virtual void                        updateState();

protected slots:
  void                                onNodeNameChanged( const QString& );
  void                                onSeqOfSamplesChanged( const QString& );
};

/*
  Class : YACSGui_WhileLoopNodePage
  Description : Page for WHILE node properties
*/
class YACSGui_WhileLoopNodePage : public WhileLoopNodePage, public YACSGui_NodePage
{
  Q_OBJECT

public:
                                      YACSGui_WhileLoopNodePage( QWidget* theParent = 0,
								 const char* theName = 0,
								 WFlags theFlags = 0 );
  virtual                             ~YACSGui_WhileLoopNodePage();

  virtual void                        setSNode( YACS::HMI::SubjectNode* theSNode );

  virtual void                        setMode( const YACSGui_InputPanel::PageMode );

  virtual void                        notifyNodeStatus( int theStatus );
  virtual void                        notifyNodeProgress();
  virtual void                        notifyInPortValues( std::map<std::string,std::string> theInPortName2Value );
  virtual void                        notifyOutPortValues( std::map<std::string,std::string> theOutPortName2Value );
  virtual void                        notifyNodeCreateBody( YACS::HMI::Subject* theSubject );

public slots:
  virtual void                        onApply();

protected:
  virtual void                        updateState();

protected slots:
  void                                onNodeNameChanged( const QString& );
};

/*
  Class : YACSGui_SwitchNodePage
  Description : Page for SWITCH node properties
*/
class YACSGui_SwitchNodePage : public SwitchNodePage, public YACSGui_NodePage
{
  Q_OBJECT

public:
                                      YACSGui_SwitchNodePage( QWidget* theParent = 0,
							      const char* theName = 0,
							      WFlags theFlags = 0 );
  virtual                             ~YACSGui_SwitchNodePage();

  virtual void                        setSNode( YACS::HMI::SubjectNode* theSNode );

  void                                setChild( YACS::HMI::SubjectNode* theChildNode );

  bool                                isSelectChild() const;

  virtual void                        setMode( const YACSGui_InputPanel::PageMode );

  virtual void                        notifyNodeStatus( int theStatus );
  virtual void                        notifyNodeProgress();
  virtual void                        notifyInPortValues( std::map<std::string,std::string> theInPortName2Value );
  virtual void                        notifyOutPortValues( std::map<std::string,std::string> theOutPortName2Value );
  virtual void                        notifyNodeCreateNode( YACS::HMI::Subject* theSubject );

public slots:
  virtual void                        onApply();
  
protected:
  virtual void                        updateState();

protected slots:
  void                                onNodeNameChanged( const QString& );
  void                                onValueChanged( int, int );
  void                                onInitSelection( const int theRow, const int theCol ); 
  void                                onNewItem( const int theRow, const int theCol );
  void                                onAdded( const int theRow );
  void                                onInserted( const int theRow );
  void                                onMovedUp( const int theUpRow );
  void                                onMovedDown( const int theDownRow );
  void                                onRemoved( const int theRow );

private:
  void                                updateLabelPorts();
  void                                setSwitchCases();

  bool                                  myIsSelectChild;
  std::map<int,YACS::HMI::SubjectNode*> myRow2ChildMap;
  std::list<int>                        myRowsOfSelectedChildren; // for children selected from tree view
};

/*
  Class : YACSGui_BlockNodePage
  Description : Page for block node properties
*/
class YACSGui_BlockNodePage : public BlockNodePage, public YACSGui_NodePage
{
  Q_OBJECT

public:
                                      YACSGui_BlockNodePage( QWidget* theParent = 0,
							     const char* theName = 0,
							     WFlags theFlags = 0 );
  virtual                             ~YACSGui_BlockNodePage();

  virtual void                        setSNode( YACS::HMI::SubjectNode* theSNode );

  void                                setChild( YACS::HMI::SubjectNode* theChildNode );

  bool                                isSelectChild() const;

  virtual void                        setMode( const YACSGui_InputPanel::PageMode );

  virtual void                        notifyNodeStatus( int theStatus );
  virtual void                        notifyNodeProgress();
  virtual void                        notifyInPortValues( std::map<std::string,std::string> theInPortName2Value );
  virtual void                        notifyOutPortValues( std::map<std::string,std::string> theOutPortName2Value );
  virtual void                        notifyNodeCreateNode( YACS::HMI::Subject* theSubject );

public slots:
  virtual void                        onApply();
  
protected:
  virtual void                        updateState();

protected slots:
  void                                onNodeNameChanged( const QString& );
  void                                onInitSelection( const int theRow, const int theCol ); 
  void                                onNewItem( const int theRow, const int theCol );
  void                                onAdded( const int theRow );
  void                                onInserted( const int theRow );
  void                                onMovedUp( const int theUpRow );
  void                                onMovedDown( const int theDownRow );
  void                                onRemoved( const int theRow );

private:
  void                                setDirectChildren();

  bool                                  myIsSelectChild;
  std::map<int,YACS::HMI::SubjectNode*> myRow2ChildMap;
  std::list<int>                        myRowsOfSelectedChildren; // for children selected from tree view
};

/*class YACSGui_NodePage : public YACSGui_PropertyPage
{
  Q_OBJECT

public: 
  typedef enum { SALOMEService, CORBAService, NodeNode, ServiceInline, XML,
		 InlineFunction, InlineScript,
		 FOR, FOREACH, WHILE, SWITCH, Block } NodeType;

public:
                                      YACSGui_NodePage( QWidget* theParent, YACSGui_Module* );
  virtual                             ~YACSGui_NodePage();

  void                                setNode( YACS::ENGINE::Node* theNode );

  QString                             getNodeName() const;
  void                                setNodeName( const QString& );

public slots:
  virtual void                        onApply();

private:
  void                                updateState();

  QLineEdit*                          myNodeName;

  YACS::ENGINE::Node*                 myNode;
  NodeType                            myType;
};
*/

/*
  Class : YACSGui_LinkPage
  Description : Page for link properties
*/
class YACSGui_LinkPage : public YACSGui_PropertyPage
{
  Q_OBJECT

public: 
  typedef enum { Control, DataFlow, DataStream } LinkType;

public:
                                      YACSGui_LinkPage( QWidget* theParent, YACSGui_Module* );
  virtual                             ~YACSGui_LinkPage();

  void                                setLink( YACS::ENGINE::Port* theOut, YACS::ENGINE::Port* theIn );

  QString                             getLinkName() const;

public slots:
  virtual void                        onApply();

private:
  void                                updateState();

  YACS::ENGINE::Port*                 myOut;
  YACS::ENGINE::Port*                 myIn;
  LinkType                            myType;
};

#endif
