
from qt import *

class PanelManager(QWidgetStack):
  def __init__(self,parent):
    QWidgetStack.__init__(self,parent)
    self.panels={}

  def setview(self,item):
    if not self.panels.has_key(item):
      panel=item.panel(self)
      self.panels[item]=panel
      idd=self.addWidget(panel)
      self.raiseWidget(panel)
    self.raiseWidget(self.panels[item])
