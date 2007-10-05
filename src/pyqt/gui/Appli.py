"""
"""
import sys,os
from qt import *
import Tree
import PanelManager
import Icons
import Items
import adapt
import Item
import pilot
import threading
import time

class Browser(QVBox):
  def __init__(self,parent,proc):
    QVBox.__init__(self,parent)
    self.hSplitter = QSplitter(self,"hSplitter")
    self.objectBrowser=Tree.Tree(self.hSplitter,self.onSelect)
    pp=adapt.adapt(proc,Item.Item)
    self.proc=proc
    self.pproc=pp
    self.objectBrowser.additem(pp)
    self.panelManager=PanelManager.PanelManager(self.hSplitter)
    self.executor=None
    self.resume=0
    self.thr=None

  def onSelect(self,item):
    #item is instance of Item.Item
    self.selected=item
    self.panelManager.setview(item)

  def run(self):
    if not self.executor:
      self.executor = pilot.ExecutorSwig()
    if self.thr and self.thr.isAlive():
      return
    #step by step execution mode
    self.executor.setExecMode(1)
    #execute it in a thread
    self.thr = threading.Thread(target=self.executor.RunW, args=(self.proc,0))
    #as a daemon (no need to join)
    self.thr.setDaemon(1)
    #start the thread
    self.thr.start()
    self.resume=1
    #wait pause
    time.sleep(0.1)
    self.executor.waitPause()
    #switch to continue execution mode
    self.executor.setExecMode(0)
    #resume it
    self.executor.resumeCurrentBreakPoint()
    self.resume=0

  def susp(self):
    """Suspend or resume an executing schema"""
    if not self.executor:
      return
    if not self.thr.isAlive():
      return

    if self.resume:
      #continue execution mode
      self.executor.setExecMode(0)
      #if finished stop it
      #resume it
      self.executor.resumeCurrentBreakPoint()
      self.resume=0
    else:
      #step by step execution mode
      self.executor.setExecMode(1)
      #self.executor.waitPause()
      self.resume=1

  def step(self):
    """Step on a paused schema"""
    if not self.executor:
      self.executor = pilot.ExecutorSwig()
    if not self.thr or not self.thr.isAlive():
      #start in step by step mode
      self.executor.setExecMode(1)
      self.thr = threading.Thread(target=self.executor.RunW, args=(self.proc,0))
      self.thr.setDaemon(1)
      self.thr.start()
      self.resume=1
      return

    #step by step execution mode
    self.resume=1
    self.executor.setExecMode(1)
    #if finished stop it
    #resume it
    self.executor.resumeCurrentBreakPoint()

  def stop(self):
    """Stop the schema"""
    if not self.executor:
      return
    if not self.thr.isAlive():
      return
    self.executor.setExecMode(1)
    self.executor.waitPause()
    self.executor.resumeCurrentBreakPoint()
    #self.executor.stopExecution()

class Appli(QMainWindow):
  """
      Appli()
      Cree la fenetre principale de l'interface utilisateur
  """
  def __init__(self):
    QMainWindow.__init__(self)
    self.createWidgets()
    self.initActions()
    self.initMenus()
    self.initToolbar()
    self.initStatusbar()
    self.initYACS()

  def createWidgets(self):
    self.tabWidget = QTabWidget(self)
    self.currentPanel=None
    self.connect(self.tabWidget, SIGNAL('currentChanged(QWidget *)'),self.handlePanelChanged)
    self.setCentralWidget(self.tabWidget)
    self.resize(800,600)

  def handlePanelChanged(self,panel):
    self.currentPanel=panel

  def initActions(self):
    self.actions = []

    self.newAct=QAction('New', QIconSet(Icons.get_image("new")), '&New',
                          QKeySequence("CTRL+N"),self)
    self.newAct.setStatusTip('Open an empty editor window')
    self.newAct.setWhatsThis( """<b>New</b>"""
            """<p>An empty editor window will be created.</p>""")
    self.newAct.connect(self.newAct,SIGNAL('activated()'), self.handleFile)
    self.actions.append(self.newAct)

    self.prefAct=QAction('Preferences',QIconSet(Icons.get_image("configure.png")),'&Preferences...',
                           0, self)
    self.prefAct.setStatusTip('Set the prefered configuration')
    self.prefAct.setWhatsThis("""<b>Preferences</b>"""
                              """<p>Set the configuration items of the application"""
                              """ with your prefered values.</p>""")
    self.prefAct.connect(self.prefAct,SIGNAL('activated()'), self.handlePreferences)
    self.actions.append(self.prefAct)

    self.runAct=QAction('Run',QIconSet(Icons.get_image("run.png")),'&Run',0,self)
    self.runAct.connect(self.runAct,SIGNAL('activated()'), self.run)
    self.runAct.setStatusTip('Run the selected schema')
    self.actions.append(self.runAct)

    self.suspAct=QAction('Suspend/resume',QIconSet(Icons.get_image("suspend-resume.gif")),'&Suspend/resume',0,self)
    self.suspAct.connect(self.suspAct,SIGNAL('activated()'), self.susp)
    self.suspAct.setStatusTip('Suspend/resume the selected schema')
    self.actions.append(self.suspAct)

    self.stepAct=QAction('Step',QIconSet(Icons.get_image("steps.png")),'&Step',0,self)
    self.stepAct.connect(self.stepAct,SIGNAL('activated()'), self.step)
    self.stepAct.setStatusTip('Step the selected schema')
    self.actions.append(self.stepAct)

    self.stopAct=QAction('Stop',QIconSet(Icons.get_image("kill.png")),'&Stop',0,self)
    self.stopAct.connect(self.stopAct,SIGNAL('activated()'), self.stop)
    self.stopAct.setStatusTip('Stop the selected schema')
    self.actions.append(self.stopAct)

  def initMenus(self):
    menubar = self.menuBar()

    #menu file
    self.fileMenu=QPopupMenu(self)
    self.newAct.addTo(self.fileMenu)
    self.fileMenu.insertItem("&Open", self.openFile)
    self.fileMenu.insertItem("&Open Salome", self.openSalomeFile)
    menubar.insertItem('&File',self.fileMenu)

    #menu settings
    self.settingsMenu = QPopupMenu(self)
    menubar.insertItem('&Settings', self.settingsMenu)
    self.settingsMenu.insertTearOffHandle()
    self.prefAct.addTo(self.settingsMenu)

    #menu Edit
    self.editMenu = QPopupMenu(self)
    self.editMenu.insertItem("&Add node", self.addNode)
    menubar.insertItem('&Edit', self.editMenu)

    #menu Canvas
    #sous menu layout
    self.layoutMenu = QPopupMenu(self)
    self.layoutMenu.insertItem("&Left Right", self.LR)
    self.canvasMenu = QPopupMenu(self)
    self.canvasMenu.insertItem("&Zoom in", self.zoomIn)
    self.canvasMenu.insertItem("Zoom &out", self.zoomOut)
    self.canvasMenu.insertItem("Layout", self.layoutMenu)
    self.canvasMenu.insertItem("&Update", self.updateCanvas)
    menubar.insertItem('&Canvas', self.canvasMenu)

    #menu window
    self.windowMenu = QPopupMenu(self)
    menubar.insertItem('&Window', self.windowMenu)
    self.connect(self.windowMenu, SIGNAL('aboutToShow()'), self.handleWindowMenu)

    #menu help
    self.help=QPopupMenu(self)
    menubar.insertItem('&Help',self.help)
    self.help.insertItem('&About',self.about,Qt.Key_F1)
    self.help.insertItem('About &Qt',self.aboutQt)

  def initYACS(self):
    import pilot
    import loader
    import salomeloader
    self.runtime= pilot.getRuntime()
    self.loader = loader.YACSLoader()
    self.executor = pilot.ExecutorSwig()
    self.salomeloader=salomeloader.SalomeLoader()

  def openSalomeFile(self):
    fn = QFileDialog.getOpenFileName(QString.null,QString.null,self)
    if fn.isEmpty():
      self.statusBar().message('Loading aborted',2000)
      return
    fileName = str(fn)
    proc=self.salomeloader.load(fileName)

    panel=Browser(self.tabWidget,proc)
    self.currentPanel=panel
    self.tabWidget.addTab( panel,os.path.basename(fileName))
    self.tabWidget.showPage(panel)

  def openFile(self):
    fn = QFileDialog.getOpenFileName(QString.null,QString.null,self)
    if fn.isEmpty():
      self.statusBar().message('Loading aborted',2000)
      return
    fileName = str(fn)
    proc=self.loader.load(fileName)

    panel=Browser(self.tabWidget,proc)
    self.currentPanel=panel
    self.tabWidget.addTab( panel,os.path.basename(fileName))
    self.tabWidget.showPage(panel)

  def LR(self):
    if self.currentPanel.selected and isinstance(self.currentPanel.selected,Items.ItemComposedNode):
      self.currentPanel.selected.layout("LR")

  def updateCanvas(self):
    if self.currentPanel.selected:#item selected
      if isinstance(self.currentPanel.selected,Items.ItemComposedNode):
        #on peut updater
        self.currentPanel.selected.editor.updateCanvas()

  def addNode(self):
    if self.currentPanel.selected:#item selected
      if isinstance(self.currentPanel.selected,Items.ItemComposedNode):
        #on peut ajouter un noeud
        self.currentPanel.selected.addNode()

  def zoomIn(self):
    if self.currentPanel.selected:#item selected
      if isinstance(self.currentPanel.selected,Items.ItemComposedNode):
        #on peut zoomer
        self.currentPanel.selected.editor.zoomIn()

  def zoomOut(self):
    if self.currentPanel.selected:#item selected
      if isinstance(self.currentPanel.selected,Items.ItemComposedNode):
        #on peut dezoomer
        self.currentPanel.selected.editor.zoomOut()

  def handleFile(self):
    pass

  def handlePreferences(self):
    pass

  def handleWindowMenu(self):
    pass

  def about(self):
    QMessageBox.about(self,'YACS browser GUI', 'YACS browser GUI')

  def aboutQt(self):
    QMessageBox.aboutQt(self,'YACS browser GUI')

  def run(self):
    if self.currentPanel:
      self.currentPanel.run()

  def susp(self):
    if self.currentPanel:
      self.currentPanel.susp()

  def step(self):
    if self.currentPanel:
      self.currentPanel.step()

  def stop(self):
    if self.currentPanel:
      self.currentPanel.stop()

  def initToolbar(self):
    tb = QToolBar(self)
    self.newAct.addTo(tb)
    self.runAct.addTo(tb)
    self.suspAct.addTo(tb)
    self.stepAct.addTo(tb)
    self.stopAct.addTo(tb)
    self.toolbars={}
    self.toolbars['File']=tb

  def initStatusbar(self):
    sb = self.statusBar()
    self.SBfile=QLabel(sb)
    sb.addWidget(self.SBfile)
    QWhatsThis.add(self.SBfile,
                   """<p>Partie de la statusbar qui donne le nom"""
                   """du fichier courant. </p>""")
    self.SBfile.setText("")


if __name__ == "__main__":
  from Item import Item
  app = QApplication(sys.argv)
  t=Appli()
  t.objectBrowser.additem(Item("item1"))
  n=t.objectBrowser.additem(Item("item2"))
  n.additem(Item("item3"))
  app.setMainWidget(t)
  t.show()
  app.exec_loop()


