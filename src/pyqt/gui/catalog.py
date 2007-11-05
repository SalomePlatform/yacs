
from qt import *
import browser_session
import browser_catalog
import pilot

class CatalogTool(QMainWindow):
  """
      CatalogTool()
      Main window of Catalog Tool
  """
  def __init__(self,parent=None):
    QMainWindow.__init__(self,parent)
    self.appli=parent
    self.createWidgets()
    self.initActions()
    self.initMenus()
    self.initToolbar()
    self.initStatusbar()
    cata=pilot.getRuntime().getBuiltinCatalog()
    self.register(cata,"Builtin Catalog")

  def createWidgets(self):
    self.tabWidget = QTabWidget(self)
    #self.currentPanel=None
    #self.connect(self.tabWidget, SIGNAL('currentChanged(QWidget *)'),self.handlePanelChanged)
    self.setCentralWidget(self.tabWidget)
    self.resize(800,600)

  def initActions(self):
    self.actions = []
    self.impSessionAct=QAction('Import from session...',0,self)
    self.impSessionAct.connect(self.impSessionAct,SIGNAL('activated()'), self.importFromSession)
    self.actions.append(self.impSessionAct)
    self.impProcAct=QAction('Import from proc...',0,self)
    self.impProcAct.connect(self.impProcAct,SIGNAL('activated()'), self.importFromProc)
    self.actions.append(self.impProcAct)

  def initMenus(self):
    menubar = self.menuBar()

    #menu import
    self.importMenu=QPopupMenu(self)
    self.impSessionAct.addTo(self.importMenu)
    self.impProcAct.addTo(self.importMenu)
    menubar.insertItem('&Import',self.importMenu)

  def importFromSession(self):
    browser_session.MainBrowser(self,self.appli).show()
    return

  def importFromProc(self):
    fn = QFileDialog.getOpenFileName(QString.null,QString.null,self)
    if fn.isEmpty():
      self.statusBar().message('Loading aborted',2000)
      return
    filename = str(fn)
    cata=pilot.getRuntime().loadCatalog("proc",filename)
    print cata
    print cata._nodeMap
    for name,node in cata._nodeMap.items():
      print name,node
    self.register(cata,filename)

  def register(self,cata,name):
    """Add a catalog in the catalog tool list"""
    panel=browser_catalog.Browser(self,appli=self.appli)
    panel.setCata(cata)
    self.tabWidget.addTab( panel,name)
    self.tabWidget.showPage(panel)

  def initToolbar(self):
    #tb = QToolBar(self)
    #self.importAct.addTo(tb)
    self.toolbars={}
    #self.toolbars['Import']=tb

  def initStatusbar(self):
    sb = self.statusBar()
    self.SBfile=QLabel(sb)
    sb.addWidget(self.SBfile)
    QWhatsThis.add(self.SBfile, """<p>Message""")
    self.SBfile.setText("")

