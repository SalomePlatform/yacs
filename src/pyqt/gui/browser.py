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

