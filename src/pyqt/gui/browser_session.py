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
import browser
import sessions
import pilot

class Browser(browser.Browser):
  def init(self):
    self.rootItem=sessions.Sessions(port=2809)
    self.objectBrowser.additem(self.rootItem)
    self.boxManager.setRootItem(self.rootItem)
    self.setStretchFactor(self.hSplitter,10)

class MainBrowser(QMainWindow):
  def __init__(self,parent,appli):
    QMainWindow.__init__(self,parent)
    self.appli=appli
    self.catalogTool=parent
    self.createWidgets()

  def createWidgets(self):
    box=QVBox(self)

    hb=QHBox(box)
    self.browser=Browser(hb,self.appli)

    row2=QHBox(box)
    but1=QPushButton( "Import catalog", row2 )
    but1.setFixedSize( but1.sizeHint())
    #but2=QPushButton( "Cancel", row2 )
    self.connect( but1, SIGNAL("clicked()"), self.handleBut1 )
    #box.connect( but2, SIGNAL("clicked()"), self.handleCancel )
    self.setCentralWidget(box)
    #self.resize(800,600)

  def handleBut1(self):
    addr=self.browser.selected.addr+"#Kernel.dir/ModulCatalog.object"
    cata=pilot.getRuntime().loadCatalog("session",addr)
    self.catalogTool.register(cata,self.browser.selected.addr)

if __name__ == "__main__":
  app = QApplication(sys.argv)
  t=MainBrowser(None,None)
  app.setMainWidget(t)
  t.show()
  app.exec_loop()

