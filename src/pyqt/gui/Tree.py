# -*- coding: iso-8859-15 -*-
"""Ce module permet de créer des vues sous forme d'arbre
   Ces vues sont construites à partir des informations
   fournies par un modèle
"""

import sys
from qt import *
import Icons

class Tree(QListView):
  """Tree(parent=None)
     Classe pour faire une vue d'un arbre
  """
  def __init__(self,parent=None,onSelect=None):
    QListView.__init__(self,parent)
    self.setCaption("Tree")
    self.setRootIsDecorated(1)
    self.setSorting(-1)
    self.addColumn("Name")
    self.children=[]
    self.last=None
    self.onSelect=onSelect
    self.connect(self,SIGNAL('selectionChanged(QListViewItem *)'),
                     self.handleSelected)

  def handleSelected(self,node):
    if self.onSelect:self.onSelect(node.item)

  def additem(self,item):
    node=Node(self,item.label,item,self.last)
    self.last=node
    self.children.append(node)
    return node


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

  def additem(self,item):
    if self.children:
      node=Node(self,item.label,item,self.children[-1])
    else:
      node=Node(self,item.label,item)
    self.children.append(node)
    return node

  def setOpen(self,o):
    if o:
      #open
      for child in self.item.getChildren():
        self.additem(child)
    else:
      #close
      for node in self.children:
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

