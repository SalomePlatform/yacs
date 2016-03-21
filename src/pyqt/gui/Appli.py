# Copyright (C) 2006-2016  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

"""
"""
import sys,os
from qt import *
import Tree
import PanelManager
import BoxManager
import Icons
import Items
import adapt
import Item
import logview
import pilot
import threading
import time
import CONNECTOR
import catalog
import traceback
import glob

class ErrorEvent(QCustomEvent):
  def __init__(self,caption,msg):
    QCustomEvent.__init__(self,8888)
    self.caption=caption
    self.msg=msg
  def process(self,parent):
    QMessageBox.warning(parent,self.caption,self.msg)

class Runner(threading.Thread):
  def __init__(self,parent,executor,proc):
    threading.Thread.__init__(self)
    self.parent=parent
    self.executor=executor
    self.proc=proc

  def run(self):
    try:
      self.executor.RunW(self.proc,0)
    except ValueError,ex:
      #traceback.print_exc()
      QApplication.postEvent(self.parent, ErrorEvent('YACS execution error',str(ex)))

class Browser(QVBox):
  def __init__(self,parent,proc):
    QVBox.__init__(self,parent)
    pp=Item.adapt(proc)
    self.proc=proc
    self.pproc=pp
    self.hSplitter = QSplitter(self,"hSplitter")
    self.objectBrowser=Tree.Tree(self.hSplitter,self.onSelect,self.onDblSelect)
    self.objectBrowser.additem(pp)
    self.panelManager=PanelManager.PanelManager(self.hSplitter)
    self.panelManager.setRootItem(pp)
    self.boxManager=BoxManager.BoxManager(self.hSplitter)
    self.boxManager.setRootItem(pp)
    self.selected=None
    self.executor=None
    self.resume=0
    self.thr=None
    self.log=logview.LogView()

  def view_log(self):
    self.log.text.setText(self.proc.getLogger("parser").getStr())
    self.log.show()

  def onDblSelect(self,item):
    #item is instance of Item.Item
    pass

  def onSelect(self,item):
    #item is instance of Item.Item
    self.selected=item

  def customEvent(self,ev):
    if ev.type() == 8888:
      ev.process(self)

  def run(self):
    if not self.executor:
      self.executor = pilot.ExecutorSwig()
    if self.thr and self.thr.isAlive():
      return
    #continue execution mode
    self.executor.setExecMode(0)
    #execute it in a thread
    self.thr = Runner(self, self.executor, self.proc)
    #as a daemon (no need to join)
    self.thr.setDaemon(1)
    #start the thread
    self.thr.start()
    time.sleep(0.1)
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
      #resume it
      self.executor.resumeCurrentBreakPoint()
      self.resume=0
    else:
      #step by step execution mode
      self.executor.setExecMode(1)
      self.resume=1

  def step(self):
    """Step on a paused schema"""
    if not self.executor:
      self.executor = pilot.ExecutorSwig()
    if not self.thr or not self.thr.isAlive():
      #start in step by step mode
      self.executor.setExecMode(1)
      self.thr = Runner(self, self.executor, self.proc)
      self.thr.setDaemon(1)
      self.thr.start()
      self.resume=1
      return

    #step by step execution mode
    self.resume=1
    self.executor.setExecMode(1)
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
    self.newAct.connect(self.newAct,SIGNAL('activated()'), self.newProc)
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

    self.cataToolAct=QAction('Catalog Tool',0,self,"catatool")
    self.cataToolAct.connect(self.cataToolAct,SIGNAL('activated()'), self.cata_tool)
    self.actions.append(self.cataToolAct)

  def initMenus(self):
    menubar = self.menuBar()

    #menu file
    self.fileMenu=QPopupMenu(self)
    self.newAct.addTo(self.fileMenu)
    self.fileMenu.insertItem("&Open", self.openFile)
    self.fileMenu.insertItem("&Open Salome", self.openSalomeFile)
    self.loadersMenu = QPopupMenu(self)
    self.fileMenu.insertItem("Loaders", self.loadersMenu)
    self.loaders=[]
    for file in glob.glob("/local/chris/SALOME2/SUPERV/YACS/BR_CC/YACS_SRC/src/pyqt/*loader.py"):
      d,f=os.path.split(file)
      name=f[:-9]
      def call_loader(event,obj=self,file=file):
        obj.openFileWithLoader(file)
      self.loaders.append(call_loader)
      self.loadersMenu.insertItem(name, call_loader)
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
    self.layoutMenu.insertItem("Right Left", self.RL)
    self.layoutMenu.insertItem("Top Bottom", self.TB)
    self.layoutMenu.insertItem("Bottom Top", self.BT)
    self.canvasMenu = QPopupMenu(self)
    self.canvasMenu.insertItem("&Zoom in", self.zoomIn)
    self.canvasMenu.insertItem("Zoom &out", self.zoomOut)
    self.canvasMenu.insertItem("Layout", self.layoutMenu)
    self.canvasMenu.insertItem("Ortholinks", self.orthoLinks)
    self.canvasMenu.insertItem("Clearlinks", self.clearLinks)
    self.canvasMenu.insertItem("&Update", self.updateCanvas)
    menubar.insertItem('&Canvas', self.canvasMenu)

    #menu window
    self.windowMenu = QPopupMenu(self)
    self.cataToolAct.addTo(self.windowMenu)
    self.windowMenu.insertItem("&Log", self.view_log)
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
    self.loader.registerProcCataLoader()

  def openSalomeFile(self):
    fn = QFileDialog.getOpenFileName(QString.null,QString.null,self)
    if fn.isEmpty():
      self.statusBar().message('Loading aborted',2000)
      return
    fileName = str(fn)
    proc=self.salomeloader.load(fileName)
    logger=proc.getLogger("parser")
    if logger.hasErrors():
      self.logFile=logview.LogView()
      self.logFile.text.setText(logger.getStr())
      self.logFile.show()

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
    logger=proc.getLogger("parser")
    if logger.hasErrors():
      self.logFile=logview.LogView()
      self.logFile.text.setText(logger.getStr())
      self.logFile.show()

    panel=Browser(self.tabWidget,proc)
    self.currentPanel=panel
    self.tabWidget.addTab( panel,os.path.basename(fileName))
    self.tabWidget.showPage(panel)

  def newProc(self):
    r=pilot.getRuntime()
    proc=r.createProc("pr")
    panel=Browser(self.tabWidget,proc)
    self.currentPanel=panel
    self.tabWidget.addTab( panel,proc.getName())
    self.tabWidget.showPage(panel)

  def openFileWithLoader(self,file):
    d,f=os.path.split(file)
    sys.path.insert(0,d)
    module=__import__(os.path.splitext(f)[0])
    del sys.path[0]
    loader=module.Loader()

    fn = QFileDialog.getOpenFileName(QString.null,QString.null,self)
    if fn.isEmpty():
      self.statusBar().message('Loading aborted',2000)
      return
    fileName = str(fn)
    proc=loader.load(fileName)
    logger=proc.getLogger("parser")
    if logger.hasErrors():
      self.logFile=logview.LogView()
      self.logFile.text.setText(logger.getStr())
      self.logFile.show()

    panel=Browser(self.tabWidget,proc)
    self.currentPanel=panel
    self.tabWidget.addTab( panel,os.path.basename(fileName))
    self.tabWidget.showPage(panel)

  def cata_tool(self):
    self.catalogTool=catalog.CatalogTool(self)
    self.catalogTool.show()
    return

  def view_log(self):
    if self.currentPanel:
      self.currentPanel.view_log()

  def LR(self,*args ):self.rankdir("LR")
  def RL(self,*args ):self.rankdir("RL")
  def TB(self,*args ):self.rankdir("TB")
  def BT(self,*args ):self.rankdir("BT")

  def rankdir(self,orient):
    if self.currentPanel and self.currentPanel.panelManager.visible:
      self.currentPanel.panelManager.visible.layout(orient)

  def updateCanvas(self):
    if self.currentPanel.selected:#item selected
      if isinstance(self.currentPanel.selected,Items.ItemComposedNode):
        #can update
        self.currentPanel.selected.graph.editor.updateCanvas()

  def addNode(self,node):
    if self.currentPanel and self.currentPanel.selected:#item selected
      if isinstance(self.currentPanel.selected,Items.ItemComposedNode):
        #can add node
        self.currentPanel.selected.addNode(node)

  def zoomIn(self):
    if self.currentPanel and self.currentPanel.panelManager.visible:
      if isinstance(self.currentPanel.panelManager.visible,Items.ItemComposedNode):
        #we can zoom
        self.currentPanel.panelManager.visible.graph.editor.zoomIn()

  def zoomOut(self):
    if self.currentPanel and self.currentPanel.panelManager.visible:
      if isinstance(self.currentPanel.panelManager.visible,Items.ItemComposedNode):
        #we can unzoom 
        self.currentPanel.panelManager.visible.graph.editor.zoomOut()

  def orthoLinks(self):
    if self.currentPanel and self.currentPanel.panelManager.visible:
      if isinstance(self.currentPanel.panelManager.visible,Items.ItemComposedNode):
        #it is a composed node with a graph
        self.currentPanel.panelManager.visible.graph.orthoLinks()

  def clearLinks(self):
    if self.currentPanel and self.currentPanel.panelManager.visible:
      if isinstance(self.currentPanel.panelManager.visible,Items.ItemComposedNode):
        #it is a composed node with a graph
        self.currentPanel.panelManager.visible.graph.clearLinks()

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


