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

# -*- coding: iso-8859-15 -*-
#
"""Ce module permet de créer des vues sous forme d'arbre
   Ces vues sont construites à partir des informations
   fournies par un modèle
"""

import sys
from qt import *
import Icons
import CONNECTOR

class Tree(QListView):
  """Tree(parent=None)
     Classe pour faire une vue d'un arbre
  """
  def __init__(self,parent=None,onSelect=None,onDblSelect=None):
    QListView.__init__(self,parent)
    self.setCaption("Tree")
    self.setRootIsDecorated(1)
    self.setSorting(-1)
    self.addColumn("Name")
    self.children=[]
    self.last=None
    self.onSelect=onSelect
    self.onDblSelect=onDblSelect
    self.connect(self,SIGNAL('selectionChanged(QListViewItem *)'),
                     self.handleSelected)
    self.connect(self,SIGNAL('doubleClicked(QListViewItem *, const QPoint &, int)'),
                     self.handleDblSelected)

  def handleSelected(self,node):
    node.item.selected()
    if self.onSelect:
      self.onSelect(node.item)

  def handleDblSelected(self,node,point,col):
    node.item.dblselected()
    if self.onDblSelect:
      self.onDblSelect(node.item)

  def additem(self,item):
    node=Node(self,item.label,item,self.last)
    self.last=node
    self.children.append(node)
    CONNECTOR.Connect(item,"selected",self.selectItem,())
    CONNECTOR.Connect(item,"add",node.addNode,())
    return node

  def selectNodeItem(self,item,node):
    #print "selectNodeItem",node,item
    self.setSelected(node,True)

  def selectItem(self,item):
    #print "selectItem",item
    node=self.selectedItem()
    if node.item is item:
      #print "item has been selected at item level"
      return

    #print "item has not been selected at item level"
    #try another way
    #find its father ???
    it = QListViewItemIterator(self)
    node = it.current()
    while node:
      if node.item is item:
        break
      it += 1
      node = it.current()

    if node:
      self.setSelected(node,True)

class Node(QListViewItem):
  """Node(parent,text,item,after)
       Classe pour faire une vue d'un noeud d'un arbre
  """
  def __init__(self,parent,text,item,after=None):
    if after is None:
      QListViewItem.__init__(self,parent,text)
    else:
      QListViewItem.__init__(self,parent,after,text)
    self.item=item
    self.setPixmap(0,Icons.get_image(item.getIconName()))
    self.setExpandable(self.item.isExpandable())
    self.children = []

  def selectNode(self,item):
    self.listView().setSelected(self,True)

  def addNode(self,item):
    #print "Tree.addNode",item
    self.additem(item)

  def delNode(self,node):
    #print "Tree.delNode",node,node.item
    CONNECTOR.Disconnect(node.item,"selected",node.selectNode,())
    CONNECTOR.Disconnect(node.item,"add",node.addNode,())
    CONNECTOR.Disconnect(node.item,"remove",self.delNode,(node,))
    self.takeItem(node)
    self.children.remove(node)

  def additem(self,item):
    if self.children:
      node=Node(self,item.label,item,self.children[-1])
    else:
      node=Node(self,item.label,item)
    self.children.append(node)
    #CONNECTOR.Connect(item,"selected",self.listView().selectNodeItem,(node,))
    CONNECTOR.Connect(item,"selected",node.selectNode,())
    CONNECTOR.Connect(item,"add",node.addNode,())
    CONNECTOR.Connect(item,"remove",self.delNode,(node,))
    return node

  def setOpen(self,o):
    if o and not self.childCount():
      for child in self.item.getChildren():
        self.additem(child)
    QListViewItem.setOpen(self,o)

  def setOpen_old(self,o):
    if o:
      #open
      for child in self.item.getChildren():
        self.additem(child)
    else:
      #close
      for node in self.children:
        #CONNECTOR.Disconnect(node.item,"selected",self.listView().selectNodeItem,(node,))
        CONNECTOR.Disconnect(node.item,"selected",node.selectNode,())
        CONNECTOR.Disconnect(node.item,"add",node.addNode,())
        self.takeItem(node)
        del node
      self.children=[]
    QListViewItem.setOpen(self,o)
       

if __name__ == "__main__":
  from Item import Item
  app = QApplication(sys.argv)
  t=Tree()
  t.additem(Item("item1"))
  #n=t.additem(Item("item2"))
  #n.additem(Item("item3"))
  app.setMainWidget(t)
  t.show()
  app.exec_loop()

