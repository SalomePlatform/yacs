
from qt import *
import Tree
from BoxManager import BoxManager

class Browser(QVBox):
  def __init__(self,parent,appli):
    QVBox.__init__(self,parent)
    self.appli=appli
    self.hSplitter = QSplitter(self,"hSplitter")
    self.objectBrowser=Tree.Tree(self.hSplitter,self.onSelect,self.onDblSelect)
    self.boxManager=BoxManager(self.hSplitter)
    self.selected=None
    self.init()

  def init(self):
    pass

  def onDblSelect(self,item):
    #item is instance of Item.Item
    pass

  def onSelect(self,item):
    #item is instance of Item.Item
    self.selected=item

