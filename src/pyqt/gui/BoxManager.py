
from qt import *
import CONNECTOR

class BoxManager(QWidgetStack):
  """ A BoxManager manages a collection of widget 
      Each widget is associated to an item
      A widget is displayed when the associated item is selected
      To get the associated widget : item.box(self)
  """
  def __init__(self,parent):
    QWidgetStack.__init__(self,parent)
    self.panels={}

  def setRootItem(self,rootItem):
    self.rootItem=rootItem
    CONNECTOR.Connect(self.rootItem,"selected",self.setview,())

  def setview(self,item):
    if not self.panels.has_key(item):
      CONNECTOR.Connect(item,"changed",self.changePanel,(item,))
      panel=item.box(self)
      self.panels[item]=panel
      idd=self.addWidget(panel)
    self.raiseWidget(self.panels[item])

  def changePanel(self,item):
    print "changePanel",item
    if self.panels.has_key(item):
      self.removeWidget(self.panels[item])
    panel=item.box(self)
    self.panels[item]=panel
    idd=self.addWidget(panel)
    self.raiseWidget(self.panels[item])
