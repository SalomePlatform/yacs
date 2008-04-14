import sys
from qt import *
import CONNECTOR
import adapt

class Item:
  def __init__(self,label=""):
    self.label=label
    self.emitting=0

  def isExpandable(self):
    return False
  
  def getChildren(self):
    return []
  def father(self):
    return None
  
  def getIconName(self):
    return "python"

  def panel(self,parent):
    """Retourne un widget pour browser/editer l'item"""
    qvbox=QVBox(parent)
    label=QLabel("Default Panel",qvbox)
    label.setAlignment( Qt.AlignHCenter | Qt.AlignVCenter )
    return qvbox

  def box(self,parent):
    """Retourne un widget pour browser/editer l'item"""
    qvbox=QVBox(parent)
    label=QLabel("Default Panel",qvbox)
    label.setAlignment( Qt.AlignHCenter | Qt.AlignVCenter )
    return qvbox

  def selected(self):
    """Method called on selection"""
    #print "Item selected"
  def dblselected(self):
    """Method called on double selection"""
    #print "Item dblselected"

ADAPT=adapt.adapt
items={}
def adapt(obj):
  if items.has_key(obj.ptr()):
    return items[obj.ptr()]
  else:
    item= ADAPT(obj,Item)
    items[obj.ptr()]=item
    return item

if __name__ == "__main__":
  app = QApplication(sys.argv)
  t=Item("label").panel(None)
  app.setMainWidget(t)
  t.show()
  app.exec_loop()

