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
