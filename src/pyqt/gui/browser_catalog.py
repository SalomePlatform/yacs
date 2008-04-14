
from qt import *
import browser
import cataitems

class Browser(browser.Browser):
  def init(self):
    self.setStretchFactor(self.hSplitter,10)
    row2=QHBox(self)
    but1=QPushButton( "Add node", row2 )
    but1.setFixedSize( but1.sizeHint())
    self.connect( but1, SIGNAL("clicked()"), self.handleBut1 )

  def handleBut1(self):
    print "handleBut1",self.selected
    if hasattr(self.selected,"addNode"):
      self.selected.addNode(self.appli)
    return

  def setCata(self,cata):
    self.rootItem=cataitems.Cata(cata)
    self.objectBrowser.additem(self.rootItem)
    self.boxManager.setRootItem(self.rootItem)

