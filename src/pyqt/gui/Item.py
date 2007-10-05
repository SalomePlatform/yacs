import sys
from qt import *

class Item:
  def __init__(self,label):
    self.label=label

  def isExpandable(self):
    return False
  
  def getChildren(self):
    return []
  
  def getIconName(self):
    return "python"

  def panel(self,parent):
    """Retourne un widget pour browser/editer l'item"""
    qvbox=QVBox(parent)
    label=QLabel("Default Panel",qvbox)
    label.setAlignment( Qt.AlignHCenter | Qt.AlignVCenter )
    return qvbox


if __name__ == "__main__":
  app = QApplication(sys.argv)
  t=Item("label").panel(None)
  app.setMainWidget(t)
  t.show()
  app.exec_loop()

