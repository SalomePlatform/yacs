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
import pilot
import Item
from qt import *
from qtcanvas import *
from GraphViewer import GraphViewer
import Editor
import CItems
import pygraphviz
from pygraphviz import graphviz as gv
import traceback
import CONNECTOR
import bisect

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
    #initial canvas size : 1000x1000
    self.canvas=MyCanvas(1000,1000)
    self.editor=GraphViewer(self.canvas,parent,"example",0)
    self.createGraph()
    root=self.node.getRootNode()
    rootItem=Item.adapt(root)
    CONNECTOR.Connect(rootItem,"selected",self.selectItem,())
    CONNECTOR.Connect(self.item,"add",self.addItem,())
    CONNECTOR.Connect(self.item.datalinks,"add",self.addLink,())

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

  def addLink(self,link):
    print "graph.addLink",link
    #CItem for outport
    nodeS=self.citems[link.pout.getNode().ptr()]
    nodeE=self.citems[link.pin.getNode().ptr()]
    po=pi=None
    for p in nodeS.outports:
      if p.port == link.pout:
        po=p
        break
    for p in nodeE.inports:
      if p.port == link.pin:
        pi=p
        break

    if pi and po:
      l=CItems.LinkItem(po,pi,self.canvas)
      self.canvas.update()

  def addItem(self,item):
    #print "graph.addItem",item
    node=CItems.Cell(item.node,self.canvas)
    self.citems[item.node.ptr()]=node
    node.show()
    self.canvas.update()

  def selectItem(self,item):
    #print "graph.selectItem",item
    self.editor.selectItem(item)

  def layout(self,rankdir):
    """Compute graph layout with graphviz package"""
    G=pygraphviz.AGraph(strict=False,directed=True)
    G.graph_attr["rankdir"]=rankdir
    G.graph_attr["dpi"]="72"
    dpi=72.
    aspect=dpi/72
    for k,n in self.citems.items():
      #k is node address (YACS)
      #n is item in canvas
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
      h=item.height()/dpi #height in inch
      w=item.width()/dpi  #width in inch
      n.attr['height']=str(h)
      n.attr['width']=str(w)
      n.attr['fixedsize']="true"
      n.attr['shape']="box"
      #n.attr['label']=item.node.getName()

    G.layout(prog='dot') # use dot
    #G.write("layout.dot")
    #G.draw("layout.png")

    graph_attr=dict(attrs(G))
    bbox=graph_attr["bb"]
    x1,y1,x2,y2=eval(bbox)
    h=self.canvas.height()
    w=self.canvas.width()
    h2=max(h,y2-y1+100)
    w2=max(w,x2-x1+100)
    if h2 > h or w2 > w:
      self.canvas.resize(w2,h2)

    for n in G:
      pos=n.attr['pos'] #position is given in points (72 points par inch, so 1 point = dpi/72=1.34)
      x,y=eval(pos)
      x=aspect*x
      y=aspect*y
      item=self.citems[int(n)]
      x0=item.x()
      y0=item.y()
      x=x-x0
      y=y-y0
      item.moveBy(x,y)

    self.canvas.update()

  def clearLinks(self):
    items=self.citems.values()
    for node in items:
      for port in node.outports:
        if not hasattr(port,"links"):
          continue
        for link in port.links():
          link.clearPoints()
    self.canvas.update()

  def orthoLinks(self):
    items=self.citems.values()
    g=grid(items)
    for node in items:
      for port in node.outports:
        if not hasattr(port,"links"):
          continue
        for link in port.links():
          #clear all intermediate points of the link
          link.clearPoints()
          #if isinstance(link,CItems.ControlLinkItem):
          #  print port.port.getNode().getName() +"->"+link.toPort.port.getNode().getName()
          #else:
          #  print port.port.getNode().getName() +":"+port.port.getName()+"->"+link.toPort.port.getNode().getName()+":"+link.toPort.port.getName()
          #print (port.x(),port.y()),(link.toPort.x(),link.toPort.y())
          x0,y0=port.x()+5,port.y()
          while g.get((x0,y0)).blocked:
            x0=x0+1
          x1,y1=link.toPort.x()-5,link.toPort.y()
          while g.get((x1,y1)).blocked:
            x1=x1-1
          path=g.findPath((x0,y0),(x1,y1))
          #print path
          if len(path)==1:
            if port.y() == link.toPort.y():
              #near ports face to face
              continue
            else:
              x,y=path[0]
              path=[(x,port.y()),(x,link.toPort.y())]
          elif len(path)==2:
            x1,y1=path[0]
            x2,y2=path[1]
            if x1 == x2:
              #vertical line
              path=[(x1,port.y()),(x1,link.toPort.y())]
            else:
              #horizontal line
              if port.y() == link.toPort.y():
                #near ports face to face
                continue
              else:
                #transform it into a vertical line
                x=(x1+x2)/2
                path=[(x,port.y()),(x,link.toPort.y())]

          #adjust the first point to the same y as port
          x0,y0=path[0]
          x1,y1=path[1]
          if y0==y1:
            #remove first point and adjust second one
            del path[0]
            x0=x1
          path[0]=x0,port.y()
          #adjust the last point to the same y as link.toPort
          x0,y0=path[-1]
          x1,y1=path[-2]
          if y0==y1:
            #remove last point and adjust new last one
            del path[-1]
            x0=x1
          path[-1]=x0,link.toPort.y()
          #print path

          #add intermediate points
          for x,y in path:
            line=link.lines[-1]
            link.splitLine(line,x,y)
    self.canvas.update()

def attrs(g,t=gv.AGRAPH):
  ah=None
  while 1:
    ah=gv.agnxtattr(g.handle,t,ah)
    value=gv.agxget(g.handle,ah)
    yield gv.agattrname(ah),value

def h(x,y,destx,desty):
  return abs(destx-x)+abs(desty-y)

def distance(node,new_node):
  x,y=node.coord
  x1,y1=new_node.coord
  d= abs(x1-x)+abs(y1-y)
  if node.parent != None:
    x0,y0=node.parent.coord
    if (x1-x)*(y0-y)-(y1-y)*(x0-x) != 0:
      #corner add some cost to penalize
      d=d+1
  return d

class node:
  def __init__(self,coord,index):
    self.coord=coord
    self.index=index
    self.blocked=0
    self.total=0
    self.path_cost=0
    self.parent=None
    self.open=0
    self.close=0

class grid:
  def __init__(self,graph):
    self.graph=graph
    xs=set()
    ys=set()
    xmargin=5
    ymargin=5
    for n in graph:
      h=n.height()
      w=n.width()
      x=n.x()
      y=n.y()
      xs.add(x-xmargin)
      xs.add(x+w+xmargin)
      ys.add(y-ymargin)
      ys.add(y+h+ymargin)

    xs=list(xs)
    xs.sort()
    x0=xs[0]-36
    xs.insert(0,x0)
    x0=xs[-1]+36
    xs.append(x0)

    ys=list(ys)
    ys.sort()
    y0=ys[0]-36
    ys.insert(0,y0)
    y0=ys[-1]+36
    ys.append(y0)

    self.xs=xs
    self.ys=ys
    self.cols=[]
    for w in xrange(len(xs)-1):
      col=[]
      x=(xs[w]+xs[w+1])/2
      for h in xrange(len(ys)-1):
        y=(ys[h]+ys[h+1])/2
        col.append(node((x,y),(w,h)))
      self.cols.append(col)

    for n in graph:
      h=n.height()
      w=n.width()
      x=n.x()
      y=n.y()
      l1,l2=bisect.bisect_left(ys,y-ymargin),bisect.bisect_left(ys,y+h+ymargin)
      i1,i2=bisect.bisect_left(xs,x-xmargin),bisect.bisect_left(xs,x+w+xmargin)
      for c in self.cols[i1:i2]:
        for e in c[l1:l2]:
          e.blocked=1

    #for col in self.cols:
    #  print [e.coord +(e.blocked,) for e in col]

  def reset(self):
    for c in self.cols:
      for n in c:
        n.open=n.close=0
        n.total= n.path_cost=0
        n.parent=None

  def get(self,coord):
    x,y=coord
    col= bisect.bisect_left(self.xs,x)-1
    if col < 0 or col >= len(self.cols):
      return None
    col=self.cols[col]
    row=bisect.bisect_left(self.ys,y)-1
    if row < 0 or row >= len(col):
      return None
    return col[row]

  def getPath(self,node):
    path=[node.coord]
    parent=node.parent
    while parent:
      prev=node
      node=parent
      parent=node.parent
      if parent:
        #if points are aligned don't keep the middle point
        x,y=node.coord
        x0,y0=prev.coord
        x1,y1=parent.coord
        if (x1-x)*(y0-y)-(y1-y)*(x0-x) == 0: 
          continue
      path.append(node.coord)
    path.reverse()
    return path

  def neighbours(self,node):
    col,row=node.index
    l=[]
    steps=((0,  +1), (+1,  0), (0,  -1), (-1,  0 ))
    for step in steps:
      c=col+step[0]
      r=row+step[1]
      if c < 0 or c >=len(self.cols):
        continue
      co=self.cols[c]
      if r <0 or r >= len(co):
        continue
      n=co[r]
      if n.blocked:
        continue
      l.append(n)
    return l

  def findPath(self,fromLoc,toLoc):
    """Find shortest path from fromLoc to toLoc"""
    self.reset()
    self.open=[]
    fromNode=self.get(fromLoc)
    self.open.append((fromNode.total,fromNode))
    toNode=self.get(toLoc)
    if toNode.blocked:
      print "toNode is blocked"
      return []
    destx,desty=toNode.coord
    while self.open:
      #if open is not void, take the best node (the first one) 
      t,node=self.open.pop(0)
      node.close=1
      if node == toNode:
        #got toLoc
        return self.getPath(node)

      for new_node in self.neighbours(node):
        if new_node.close :
          continue
        x,y =new_node.coord
        path_cost=node.path_cost+distance(node,new_node)
        total=path_cost+h(x,y,destx,desty)
        if new_node.open :
          #the node is already in open
          if total < new_node.total:
            self.open.remove((new_node.total,new_node))
            new_node.path_cost=path_cost
            new_node.parent=node
            new_node.total=total
            bisect.insort(self.open,(new_node.total,new_node))
        else:
          #the node is not yet in open
          new_node.path_cost=path_cost
          new_node.parent=node
          new_node.total=total
          bisect.insort(self.open,(new_node.total,new_node))
          new_node.open=1

    return []

