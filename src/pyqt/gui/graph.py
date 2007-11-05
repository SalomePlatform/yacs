import sys
import pilot
import Item
from qt import *
from qtcanvas import *
from GraphViewer import GraphViewer
import Editor
import CItems
import pygraphviz
import traceback
import CONNECTOR

class MyCanvas(QCanvas):
  def customEvent(self,event):
    object=event.data()
    object.customEvent(event)
    self.update()

class Graph:
  def __init__(self,item,parent):
    self.parent=parent
    self.item=item
    self.node=item.node
    #Canvas size : 10000x10000
    self.canvas=MyCanvas(10000,10000)
    self.editor=GraphViewer(self,self.canvas,parent,"example",0)
    self.createGraph()
    root=self.node.getRootNode()
    rootItem=Item.adapt(root)
    CONNECTOR.Connect(rootItem,"selected",self.selectItem,())
    CONNECTOR.Connect(self.item,"add",self.addItem,())

  def createGraph(self):
    #citems dict helps finding items in canvas from swig proxy
    #To find an item node make : citems[node.ptr()]
    citems={}
    self.citems=citems
    #pitems dict helps finding items in canvas from swig proxy
    #To find an item port make : pitems[port.ptr()]
    pitems={}

    y=0
    lnode=self.node.edGetDirectDescendants()
    for n in lnode:
      c=CItems.Cell(n,self.canvas)
      citems[n.ptr()]=c
      c.show()

    for k,n in citems.items():
      for p in n.inports:
        pitems[p.port.ptr()]=p
      for p in n.outports:
        pitems[p.port.ptr()]=p

    for pout,pin in self.node.getSetOfInternalLinks():
      if pout.getNode().getFather() != self.node and pin.getNode().getFather() != self.node:
        continue
      po=pitems.get(pout.ptr())
      pi=pitems.get(pin.ptr())
      if pi and po:
        CItems.LinkItem(po,pi,self.canvas)

    for n in lnode:
      itemup=citems[n.ptr()]
      for ndown in n.getOutNodes():
        itemdown=citems[ndown.ptr()]
        CItems.ControlLinkItem(itemup.outgate,itemdown.ingate,self.canvas)

    self.layout("LR")

  def addItem(self,item):
    print "graph.addItem",item
    node=CItems.Cell(item.node,self.canvas)
    self.citems[item.node.ptr()]=node
    node.show()
    self.canvas.update()

  def selectItem(self,item):
    print "graph.selectItem",item
    self.editor.selectItem(item)

  def layout(self,rankdir):
    """Compute graph layout with graphviz package"""
    G=pygraphviz.AGraph(strict=False,directed=True)
    G.graph_attr["rankdir"]=rankdir
    for k,n in self.citems.items():
      #k est l'adresse du node (YACS)
      #n est l'item dans le canvas
      G.add_node(k)

    for pout,pin in self.node.getSetOfInternalLinks():
      if pout.getNode().ptr() not in self.citems :
        continue
      if pin.getNode().ptr() not in self.citems:
        continue
      G.add_edge(pout.getNode().ptr(),pin.getNode().ptr())

    for k,n in self.citems.items():
      for ndown in n.node.getOutNodes():
        G.add_edge(n.node.ptr(),ndown.ptr())

    #By default graphviz uses 96.0 pixel per inch (dpi=96.0)
    for n in G.nodes():
      item=self.citems[int(n)]
      h=item.height()/96. #height in inch
      w=item.width()/96.  #width in inch
      n.attr['height']=str(h)
      n.attr['width']=str(w)
      n.attr['fixedsize']="true"
      n.attr['shape']="box"
      #n.attr['label']=item.node.getName()

    G.layout(prog='dot') # use dot
    #G.write("layout.dot")
    #G.draw("layout.png")

    #from pygraphviz import graphviz as gv
    #bbox= gv.agget(G.handle,"bb")#bounding box to resize
    #x1,y1,x2,y2=eval(bbox)
    #self.canvas.resize(w,h)

    for n in G:
      pos=n.attr['pos'] #position is given in points (72 points par inch, so 1 point = 96./72=1.34)
      x,y=eval(pos)
      x=96./72*x+10
      y=96./72*y+10
      item=self.citems[int(n)]
  #    x0=item.x()+item.width()/2.
  #    y0=item.y()+item.height()/2.
      x0=item.x()
      y0=item.y()
      x=x-x0
      y=y-y0
      item.moveBy(x,y)

    self.canvas.update()

