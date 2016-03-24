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

