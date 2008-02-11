
#include "RuntimeSALOME.hxx"
#include "Proc.hxx"

#include "mainempty.h"
#include "editTree.h"
#include "editCanvas.h"
#include "guiObservers.hxx"
#include "commandsProc.hxx"
#include "guiContext.hxx"
#include "nodeEdition.h"
#include "parsers.hxx"
#include "Logger.hxx"
#include "LinkInfo.hxx"
#include "VisitorSaveSalomeSchema.hxx"

#include "SALOME_NamingService.hxx"
#include "SALOME_ModuleCatalog.hxx"
#include "SALOME_ModuleCatalog.hh"

#include <qapplication.h>
#include <qfiledialog.h>
#include <qdockwindow.h>
#include <qtoolbox.h>
#include <qcolor.h>
#include <qcanvas.h>
#include <qwidget.h>
#include <qwidgetstack.h>
#include <qlayout.h>

#include <sstream>
#include <iostream>
#include <stdexcept>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"
using namespace std;

void AttachDebugger()
{
  if(getenv ("DEBUGGER"))
    {
      std::stringstream exec;
      exec << "$DEBUGGER guidemo " << getpid() << "&";
      std::cerr << exec.str() << std::endl;
      system(exec.str().c_str());
      while(1);
    }
}

void terminateHandler(void)
{
  std::cerr << "Terminate: not managed exception !"  << std::endl;
  AttachDebugger();
}

void unexpectedHandler(void)
{
  std::cerr << "Unexpected: unexpected exception !"  << std::endl;
  AttachDebugger();
}

class myMainform: public mainform
{
public:
  myMainform(QWidget* parent = 0, const char* name = 0, WFlags fl = 0);
  virtual ~myMainform();
  virtual void fileExit();
  virtual void fileOpen();
  virtual void fileNew();
  virtual void fileSave();
  virtual void fileSaveAs();
  virtual void fileCheckLoad();
  virtual void addTree(Qt::Dock pos);
  virtual void setCanvas();
  virtual void setStackOfWidgets();
protected:
  virtual void load(const QString &f);
  YACS::ENGINE::Proc* _proc;
  YACS::YACSLoader *_loader;
};


myMainform::myMainform(QWidget* parent, const char* name, WFlags fl)
  : mainform(parent, name, fl)
{
  YACS::ENGINE::RuntimeSALOME::setRuntime();
  _loader = new YACS::YACSLoader();
  _loader->registerProcCataLoader();
  YACS::HMI::GuiContext* context = new YACS::HMI::GuiContext();
  YACS::HMI::GuiContext::setCurrent(context);
  setMinimumWidth(1260);
  setMinimumHeight(800);
  setCanvas();
  addTree(Qt::DockLeft);
  addTree(Qt::DockRight);
  //  addTree(Qt::DockLeft);
  setStackOfWidgets();

  try
    {
      YACS::ENGINE::RuntimeSALOME* runTime = YACS::ENGINE::getSALOMERuntime();
      CORBA::ORB_ptr orb = runTime->getOrb();
      if (orb)
        {
          SALOME_NamingService namingService(orb);
          CORBA::Object_ptr obj = namingService.Resolve("/Kernel/ModulCatalog");
          SALOME_ModuleCatalog::ModuleCatalog_var aModuleCatalog =
            SALOME_ModuleCatalog::ModuleCatalog::_narrow(obj);
          if (! CORBA::is_nil(aModuleCatalog))
            {
              DEBTRACE("SALOME_ModuleCatalog::ModuleCatalog found");
              std::string anIOR = orb->object_to_string( aModuleCatalog );
              YACS::ENGINE::Catalog* aCatalog = runTime->loadCatalog( "session", anIOR );
              YACS::HMI::GuiContext::getCurrent()->setSessionCatalog(aCatalog);
              std::map< std::string, YACS::ENGINE::ComponentDefinition * >::iterator it;
              for (it = aCatalog->_componentMap.begin();it != aCatalog->_componentMap.end(); ++it)
                DEBTRACE("Component: " <<(*it).first);
            }
        }
    }
  catch(ServiceUnreachable& e)
    {
      DEBTRACE("Caught Exception. "<<e);
    }
}

myMainform::~myMainform()
{
  delete _proc;
}

void myMainform::fileNew()
{
  YACS::ENGINE::RuntimeSALOME* runTime = YACS::ENGINE::getSALOMERuntime();
  _proc = runTime->createProc("newSchema");
  YACS::HMI::GuiContext::getCurrent()->setProc(_proc);
}

void myMainform::setCanvas()
{
  QCanvas * canvas = YACS::HMI::GuiContext::getCurrent()->getCanvas();
  YACS::HMI::EditCanvas *editor
    = new YACS::HMI::EditCanvas(YACS::HMI::GuiContext::getCurrent(), canvas, this);
  canvas->setBackgroundColor(QColor(204,237,239));
  setCentralWidget(editor);
}

void myMainform::addTree(Qt::Dock pos)
{
  QDockWindow *dw = new QDockWindow( QDockWindow::InDock, this );
  dw->setResizeEnabled( TRUE );
  dw->setVerticalStretchable( TRUE );
  addDockWindow( dw, pos );
  setDockEnabled( dw, DockTop, FALSE );
  setDockEnabled( dw, DockBottom, FALSE );
  dw->setCloseMode( QDockWindow::Never );

  YACS::HMI::editTree *dbtree =
    new YACS::HMI::editTree(YACS::HMI::GuiContext::getCurrent(), dw);
  dw->setWidget(dbtree);
  dw->setCaption( tr("edit tree"));
}

void myMainform::setStackOfWidgets()
{
  QDockWindow *dw = new QDockWindow( QDockWindow::InDock, this );
  dw->setResizeEnabled( TRUE );
  dw->setVerticalStretchable( TRUE );
  addDockWindow( dw, DockRight );
  setDockEnabled( dw, DockTop, FALSE );
  setDockEnabled( dw, DockBottom, FALSE );
  dw->setCloseMode( QDockWindow::Never );

  QWidgetStack *ws = new QWidgetStack(dw);
  YACS::HMI::GuiContext::getCurrent()->setWidgetStack(ws);
  QWidget* WStackPage = new QWidget( ws, "WStackPage" );
  QHBoxLayout* wiEditionsLayout = new QHBoxLayout( this, 11, 6, "wiEditionsLayout"); 
  ws->addWidget( WStackPage, 0 );
  wiEditionsLayout->addWidget( ws );

  dw->setWidget(ws);
  dw->setCaption( tr("edit stack"));

  YACS::HMI::Subject* context = YACS::HMI::GuiContext::getCurrent();
  YACS::HMI::NodeEdition* rootEdit = new YACS::HMI::NodeEdition(context,
                                                                ws,
                                                                context->getName().c_str());
}

void myMainform::fileOpen()
{
  QString fn = QFileDialog::getOpenFileName( QString::null,
                                             tr( "XML-Files (*.xml);;All Files (*)" ),
                                             this,
                                             "load YACS scheme file dialog",
                                             "Choose a filename to load"  );
  if ( !fn.isEmpty() )
    {
      DEBTRACE("***************************************************************************");
      DEBTRACE("file loaded : " <<fn.latin1());
      DEBTRACE("***************************************************************************");
      _proc = _loader->load(fn.latin1());
      YACS::ENGINE::Logger* logger= _proc->getLogger("parser");
      if(!logger->isEmpty())
        {
          DEBTRACE(logger->getStr());
        }
      YACS::HMI::GuiContext::getCurrent()->setProc(_proc);
    }
}

void myMainform::fileExit()
{
  close();
}

void myMainform::fileSave()
{
  fileSaveAs();
}

void myMainform::fileSaveAs()
{
  QString fn = QFileDialog::getSaveFileName(QString::null,
                                           tr( "XML-Files (*.xml)" ),
                                           this,
                                           "save YACS scheme file dialog",
                                           "Choose a filename to save under" );
  if ( !fn.isEmpty() )
    {
      YACS::ENGINE::Proc* proc = YACS::HMI::GuiContext::getCurrent()->getProc();
      YACS::ENGINE::LinkInfo info(YACS::ENGINE::LinkInfo::ALL_DONT_STOP);
      proc->checkConsistency(info);
      if (info.areWarningsOrErrors())
        DEBTRACE(info.getGlobalRepr());
      YACS::ENGINE::VisitorSaveSalomeSchema vss(proc);
      vss.openFileSchema(fn.latin1());
      proc->accept(&vss);
      vss.closeFileSchema();      
    }
}

/*!
 * Select several xml files in a directory to check load and delete of all the files.
 * Mainly used do test delete mechanism.
 */
void myMainform::fileCheckLoad()
{
  QStringList files = QFileDialog::getOpenFileNames(tr( "XML-Files (*.xml)" ),
                                                    QString::null,
                                                    this,
                                                    "check load on samples xml files",
                                                    "Select one or more files to load" );
  QStringList list = files;
  QStringList::Iterator it = list.begin();
  while( it != list.end() )
    {
      QString fn = *it;
      if ( !fn.isEmpty() )
        {
          DEBTRACE("***************************************************************************");
          DEBTRACE("file loaded : " <<fn.latin1());
          DEBTRACE("***************************************************************************");
          _proc = _loader->load(fn.latin1());
          YACS::ENGINE::Logger* logger= _proc->getLogger("parser");
          if(!logger->isEmpty())
            {
              DEBTRACE(logger->getStr());
            }
          YACS::HMI::GuiContext::getCurrent()->setProc(_proc);
        }
      ++it;
    }
 }

/*!
 * Not used any more ?
 */
void myMainform::load(const QString &f)
{
  if (!QFile::exists(f))
    return;
  QFile file(f);
  if (!file.open(IO_ReadOnly))
    return;
  QTextStream ts(&file);
  QString txt = ts.read();
 }

int main( int argc, char **argv )
{
  if(getenv ("DEBUGGER"))
    {
//       setsig(SIGSEGV,&Handler);
      set_terminate(&terminateHandler);
      set_unexpected(&unexpectedHandler);
    }
    QApplication a( argc, argv );
    myMainform *myAppli =new myMainform();
    myAppli->show();
    a.connect( &a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()) );
    return a.exec();
}

