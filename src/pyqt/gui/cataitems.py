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

import Item
import CONNECTOR
import Items

class Obj(Item.Item):
  def __init__(self,root=None):
    Item.Item.__init__(self)
    if root:
      self.root=root
    else:
      self.root=self

  def selected(self):
    if not self.emitting:
      self.emitting=1
      CONNECTOR.Emit(self.root,"selected",self)
      self.emitting=0

  def panel(self,parent):
    """Retourne un tab widget pour browser/editer la proc"""
    tabWidget = QTabWidget( parent )
    for name,method in self.panels:
      tabWidget.addTab( method(self,tabWidget), name )
    return tabWidget

  box=panel

  def panel1(self,parent):
    qvbox=QVBox(parent)
    self.text=QTextEdit(qvbox,"log")
    self.text.setFrameShape(QTextEdit.NoFrame)
    self.text.setTextFormat(QTextEdit.PlainText)
    self.text.setWordWrap(QTextEdit.FixedColumnWidth)
    #self.text.setWrapColumnOrWidth(120)
    self.text.setReadOnly(1)
    return qvbox

  panels=[("Panel1",panel1)]

class ItemService(Item.Item):
  def __init__(self,service,root):
    Item.Item.__init__(self)
    self.root=root
    self.service=service
    self.label=service.getName()

  def isExpandable(self):
    return True

  def addNode(self,appli):
    appli.addNode(self.service)

  def getChildren(self):
    sublist=[]
    for port in self.service.getSetOfInputPort():
      sublist.append(Items.ItemInPort(port,self.root))
    for port in self.service.getSetOfOutputPort():
      sublist.append(Items.ItemOutPort(port,self.root))
    for port in self.service.getSetOfInputDataStreamPort():
      sublist.append(Items.ItemInStream(port,self.root))
    for port in self.service.getSetOfOutputDataStreamPort():
      sublist.append(Items.ItemOutStream(port,self.root))
    return sublist

  def selected(self):
    if not self.emitting:
      self.emitting=1
      CONNECTOR.Emit(self.root,"selected",self)
      self.emitting=0

class ItemType(Item.Item):
  def __init__(self,typ,root,name=""):
    Item.Item.__init__(self)
    self.typ=typ
    self.root=root
    if name:
      self.label=name
    else:
      self.label=typ.name()

  def isExpandable(self):
    return True

  def getChildren(self):
    sublist=[]
    return sublist

  def selected(self):
    if not self.emitting:
      self.emitting=1
      CONNECTOR.Emit(self.root,"selected",self)
      self.emitting=0

class ItemCompo(Item.Item):
  def __init__(self,compo,root):
    Item.Item.__init__(self)
    self.compo=compo
    self.root=root
    self.label=compo.getName()

  def isExpandable(self):
    return True

  def getChildren(self):
    sublist=[]
    for service in self.compo._serviceMap.values():
      itemservice=ItemService(service,self.root)
      sublist.append(itemservice)
    return sublist

  def selected(self):
    if not self.emitting:
      self.emitting=1
      CONNECTOR.Emit(self.root,"selected",self)
      self.emitting=0

class ItemNode(Item.Item):
  def __init__(self,node,root):
    Item.Item.__init__(self)
    self.node=node
    self.root=root
    self.label=node.getName()

  def isExpandable(self):
    return True

  def addNode(self,appli):
    appli.addNode(self.node)

  def getChildren(self):
    sublist=[]
    return sublist

  def selected(self):
    if not self.emitting:
      self.emitting=1
      CONNECTOR.Emit(self.root,"selected",self)
      self.emitting=0

class ItemComposedNode(Item.Item):
  def __init__(self,node,root):
    Item.Item.__init__(self)
    self.node=node
    self.root=root
    self.label=node.getName()

  def addNode(self,appli):
    appli.addNode(self.node)

  def isExpandable(self):
    return True

  def getChildren(self):
    sublist=[]
    return sublist

  def selected(self):
    if not self.emitting:
      self.emitting=1
      CONNECTOR.Emit(self.root,"selected",self)
      self.emitting=0

class TypesItem(Item.Item):
  """Item for types folder"""
  def __init__(self,typeMap,root):
    Item.Item.__init__(self)
    self.typeMap=typeMap
    self.label="Types"
    self.root=root

  def getIconName(self):
    return "folder"

  def isExpandable(self):
    return True

  def getChildren(self):
    sublist=[]
    for name,typ in self.typeMap.items():
      itemtype=ItemType(typ,self.root,name)
      sublist.append(itemtype)
    return sublist

class ComponentsItem(Item.Item):
  """Item for components folder"""
  def __init__(self,compoMap,root):
    Item.Item.__init__(self)
    self.compoMap=compoMap
    self.label="Components"
    self.root=root

  def getIconName(self):
    return "folder"

  def isExpandable(self):
    return True

  def getChildren(self):
    sublist=[]
    for compo in self.compoMap.values():
      itemcompo=ItemCompo(compo,self.root)
      sublist.append(itemcompo)
    return sublist

class NodesItem(Item.Item):
  """Item for nodes folder"""
  def __init__(self,nodesMap,root):
    Item.Item.__init__(self)
    self.nodesMap=nodesMap
    self.label="Nodes"
    self.root=root

  def getIconName(self):
    return "folder"

  def isExpandable(self):
    return True

  def getChildren(self):
    sublist=[]
    for node in self.nodesMap.values():
      itemnode=ItemNode(node,self.root)
      sublist.append(itemnode)
    return sublist

class ComposedNodesItem(Item.Item):
  """Item for composed nodes folder"""
  def __init__(self,composedMap,root):
    Item.Item.__init__(self)
    self.composedMap=composedMap
    self.label="ComposedNodes"
    self.root=root

  def getIconName(self):
    return "folder"

  def isExpandable(self):
    return True

  def getChildren(self):
    sublist=[]
    for node in self.composedMap.values():
      itemnode=ItemComposedNode(node,self.root)
      sublist.append(itemnode)
    return sublist

class Cata(Obj):
  def __init__(self,cata):
    Obj.__init__(self)
    self.cata=cata
    self.label=cata.getName()

  def isExpandable(self):
    return True

  def getChildren(self):
    sublist=[]
    sublist.append(TypesItem(self.cata._typeMap,self))
    sublist.append(NodesItem(self.cata._nodeMap,self))
    sublist.append(ComposedNodesItem(self.cata._composednodeMap,self))
    sublist.append(ComponentsItem(self.cata._componentMap,self))
    return sublist

  def dblselected(self):
    if not self.emitting:
      self.emitting=1
      CONNECTOR.Emit(self,"dblselected",self)
      self.emitting=0

