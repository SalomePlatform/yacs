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

