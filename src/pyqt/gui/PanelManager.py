
from qt import *
import CONNECTOR

class PanelManager(QWidgetStack):
  """ A PanelManager manages a collection of widget
      Each widget is associated to an item
      A widget is displayed when the associated item is double selected
      To get the associated widget : item.panel(self)
  """
  def __init__(self,parent):
    QWidgetStack.__init__(self,parent)
    self.visible=None
    self.panels={}

  def setRootItem(self,rootItem):
    self.rootItem=rootItem
    CONNECTOR.Connect(self.rootItem,"dblselected",self.setview,())

  def setview(self,item):
    if not self.panels.has_key(item):
      panel=item.panel(self)
      self.panels[item]=panel
      idd=self.addWidget(panel)
    self.visible=item
    self.raiseWidget(self.panels[item])
