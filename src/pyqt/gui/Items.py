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
import SALOMERuntime
import Item
import adapt
from qt import *
from qtcanvas import *
import Editor
import CItems
import pygraphviz
import traceback
import CONNECTOR
import graph
import panels

class DataLinkItem(Item.Item):
  def __init__(self,pin,pout):
    Item.Item.__init__(self)
    self.pin=pin
    self.pout=pout
    self.label= pout.getNode().getName()+":"+pout.getName()+"->"+pin.getNode().getName()+":"+pin.getName()

  def getIconName(self):
    return "datalink.png"

class StreamLinkItem(Item.Item):
  def __init__(self,pin,pout):
    Item.Item.__init__(self)
    self.pin=pin
    self.pout=pout
    self.label= pout.getNode().getName()+":"+pout.getName()+"->"+pin.getNode().getName()+":"+pin.getName()

  def getIconName(self):
    return "streamlink.png"

class ControlLinkItem(Item.Item):
  def __init__(self,nodeup,nodedown):
    Item.Item.__init__(self)
    self.nodedown=nodedown
    self.nodeup=nodeup
    self.label= nodeup.getName()+"->"+nodedown.getName()

  def getIconName(self):
    return "controllink.png"

class ControlLinksItem(Item.Item):
  """Item for all control links of a composed node"""
  def __init__(self,item):
    Item.Item.__init__(self)
    self.item=item
    self.label="Control Links"

  def getIconName(self):
    return "folder"
  def isExpandable(self):
    return True

  def getChildren(self):
    sublist=[]
    for n in self.item.node.edGetDirectDescendants():
      for p in n.getOutNodes():
        sublist.append(ControlLinkItem(n,p))
    return sublist

  def addLink(self,link):
    CONNECTOR.Emit(self,"add",link)

class DataLinksItem(Item.Item):
  """Item for all data links of a composed node"""
  def __init__(self,item):
    Item.Item.__init__(self)
    self.item=item
    self.label="Data Links"

  def getIconName(self):
    return "folder"
  def isExpandable(self):
    return True

  def getChildren(self):
    sublist=[]
    for pout,pin in self.item.node.getSetOfInternalLinks():
      if pout.getNode().getFather() != self.item.node and pin.getNode().getFather() != self.item.node:
        continue
      if isinstance(pin,pilot_InputDataStreamPort):
        sublist.append(StreamLinkItem(pin,pout))
      else:
        sublist.append(DataLinkItem(pin,pout))
    #for pout,pin in self.item.node.getSetOfLinksLeavingCurrentScope():
    #  sublist.append(DataLinkItem(pin,pout))
    #for pin,pout in self.item.node.getSetOfLinksComingInCurrentScope():
    #  sublist.append(DataLinkItem(pin,pout))
    return sublist

  def addLink(self,link):
    CONNECTOR.Emit(self,"add",link)

class ItemComposedNode(Item.Item):
  """Item pour les noeuds composes"""
  n=0
  def __init__(self,node):
    #node is an instance of YACS::ENGINE::ComposedNode
    Item.Item.__init__(self)
    self.node=node
    self.graph=None
    self.label=node.getName()
    self.datalinks=DataLinksItem(self)
    self.controllinks=ControlLinksItem(self)

  def isExpandable(self):
    return True

  def getChildren(self):
    #liste des noeuds fils
    liste=self.node.edGetDirectDescendants()
    #On les adapte en item avant de les retourner
    sublist=[]
    for n in liste:
      try:
        sublist.append(Item.adapt(n))
      except:
        #print n
        #traceback.print_exc()
        raise
    sublist.append(self.datalinks)
    sublist.append(self.controllinks)
    return sublist

  def dblselected(self):
    #print "ItemComposedNode dblselected"
    root=self.node.getRootNode()
    rootItem=Item.adapt(root)
    if not self.emitting:
      self.emitting=1
      CONNECTOR.Emit(rootItem,"dblselected",self)
      self.emitting=0

  def selected(self):
    #print "ItemComposedNode selected"
    root=self.node.getRootNode()
    rootItem=Item.adapt(root)
    if not self.emitting:
      self.emitting=1
      CONNECTOR.Emit(rootItem,"selected",self)
      self.emitting=0

  def getIconName(self):
    return "green-los"

  def panel(self,parent):
    """Retourne un tab widget pour browser/editer la proc"""
    tabWidget = QTabWidget( parent )
    for name,method in self.panels:
      tabWidget.addTab( method(self,tabWidget), name )
    return tabWidget

  def addNode(self,service):
    print "Composed.addNode",service
    #add node service in the parent self which is a ComposedNode
    new_node=service.clone(None)
    ItemComposedNode.n=ItemComposedNode.n+1
    name=service.getName()+"_%d" % ItemComposedNode.n
    new_node.setName(name)
    self.node.edAddChild(new_node)
    item=Item.adapt(new_node)
    CONNECTOR.Emit(self,"add",item)

  def panel1(self,parent):
    qvbox=QVBox(parent)
    self.graph=graph.Graph(self,qvbox)
    return qvbox

  def layout(self,rankdir):
    if self.graph:
      self.graph.layout(rankdir)

  panels=[("Panel1",panel1)]

  def addLink(self,link):
    print "Composed.addLink",link
    if isinstance(link,DataLinkItem):
      self.datalinks.addLink(link)
    elif isinstance(link,StreamLinkItem):
      self.datalinks.addLink(link)
    else:
      self.controllinks.addLink(link)


class ItemForLoop(ItemComposedNode):
  def box(self,parent):
    return panels.PanelForLoop(parent,self)

  def addNode(self,service):
    new_node=service.clone(None)
    ItemComposedNode.n=ItemComposedNode.n+1
    name=service.getName()+"_%d" % ItemComposedNode.n
    new_node.setName(name)
    #replace the old node (if it exists) with the new one
    nodes=self.node.edGetDirectDescendants()
    if nodes:
      old_item=Item.adapt(nodes[0])
      CONNECTOR.Emit(old_item,"remove")
    self.node.edSetNode(new_node)
    item=Item.adapt(new_node)
    CONNECTOR.Emit(self,"add",item)
    CONNECTOR.Emit(self,"changed")

class ItemWhile(ItemForLoop):
  pass

class ItemForEach(ItemForLoop):
  pass

class ItemSwitch(ItemComposedNode):
  def addNode(self,service):
    return

class ItemProc(ItemComposedNode):
  """Item pour la procedure"""
  def connecting(self,item):
    print "ItemProc.connecting",item
    self._connecting=item

class ItemPort(Item.Item):
  """Item pour les ports """
  panels=[]
  def __init__(self,port,root=None):
    Item.Item.__init__(self)
    self.port=port
    self.label=port.getName()
    if root:
      self.root=root
    elif self.port.getNode().getFather():
      root=self.port.getNode().getRootNode()
      self.root=Item.adapt(root)
    else:
      self.root=None

  def selected(self):
    #print "ItemPort selected"
    if not self.root:
      return
    if not self.emitting:
      self.emitting=1
      CONNECTOR.Emit(self.root,"selected",self)
      self.emitting=0

  def getIconName(self):
    return "port.png"

  def panel(self,parent):
    """Retourne un tab widget pour browser/editer l'item"""
    tabWidget = QTabWidget( parent )
    for name,method in self.panels:
      tabWidget.addTab( method(self,tabWidget), name )
    return tabWidget
  box=panel

  def link(self,other):
    print "ItemPort.link:",self,other

  def connect(self):
    print "ItemPort.connect:"
    self.root.connecting(self)

class ItemInPort(ItemPort):
  def getIconName(self):
    return "inport.png"

  def panel1(self,parent):
    return panels.PanelInPort(parent,self)

  panels=[("Panel1",panel1)]

class ItemOutPort(ItemPort):
  def getIconName(self):
    return "outport.png"

  def panel1(self,parent):
    return panels.PanelOutPort(parent,self)

  panels=[("Panel1",panel1)]

  def link(self,other):
    nodeS=self.port.getNode()
    nodeE=other.port.getNode()
    father=nodeS.getFather()
    if father != nodeE.getFather():
      #not same father : do nothing for the moment
      return
    try:
      #cflink=nodeS.getOutGate().isAlreadyInSet(nodeE.getInGate())
      cflink= nodeE.getInGate() in nodeS.getOutGate().edSetInGate()
      father.edAddDFLink(self.port,other.port)
      l=DataLinkItem(other.port,self.port)
      fitem=Item.adapt(father)
      fitem.addLink(l)
      if not cflink:
        #add also a control flow link
        fitem.addLink(ControlLinkItem(nodeS,nodeE))
    except ValueError,ex:
      traceback.print_exc()
      QMessageBox.warning(None,"YACS error",str(ex))
      return

class ItemInStream(ItemPort):
  def getIconName(self):
    return "instream.png"

class ItemOutStream(ItemPort):
  def getIconName(self):
    return "outstream.png"

  def link(self,other):
    father=self.port.getNode().getFather()
    if father != other.port.getNode().getFather():
      #not same father : not for the moment
      return
    try:
      father.edAddLink(self.port,other.port)
      l=StreamLinkItem(other.port,self.port)
      fitem=Item.adapt(father)
      fitem.addLink(l)
    except ValueError,ex:
      traceback.print_exc()
      QMessageBox.warning(None,"YACS error",str(ex))
      return

class ItemInGate(ItemPort):
  """Item for InGate"""
  def __init__(self,port):
    Item.Item.__init__(self)
    self.port=port
class ItemOutGate(ItemPort):
  """Item for OutGate"""
  def __init__(self,port):
    Item.Item.__init__(self)
    self.port=port

class ItemNode(Item.Item):
  """Item pour les noeuds elementaires
     Il n a pas de fils
  """
  #attr donnant la liste des panels du noeud (nom,method)
  panels=[]
  def __init__(self,node):
    Item.Item.__init__(self)
    self.node=node
    self.label=node.getName()
    self.father=Item.adapt(node.getFather())

  def selected(self):
    #print "ItemNode selected"
    root=self.node.getRootNode()
    rootItem=Item.adapt(root)
    if not self.emitting:
      self.emitting=1
      #for those that have subscribed to item level
      CONNECTOR.Emit(self,"selected",self)
      #for those that have subscribed to rootItem level
      CONNECTOR.Emit(rootItem,"selected",self)
      self.emitting=0

  def isExpandable(self):
    return True

  def getChildren(self):
    sublist=[]
    for n in self.node.getSetOfInputPort():
      sublist.append(Item.adapt(n))
    for n in self.node.getSetOfOutputPort():
      sublist.append(Item.adapt(n))
    for n in self.node.getSetOfInputDataStreamPort():
      sublist.append(Item.adapt(n))
    for n in self.node.getSetOfOutputDataStreamPort():
      sublist.append(Item.adapt(n))
    return sublist

  def panel(self,parent):
    """Retourne un tab widget pour browser/editer l'item"""
    tabWidget = QTabWidget( parent )
    for name,method in self.panels:
      tabWidget.addTab( method(self,tabWidget), name )
    return tabWidget
  box=panel

class ItemScriptNode(ItemNode):

  def panel1(self,parent):
    return panels.PanelScript(parent,self)

  panels=[("Panel1",panel1)]

  def getIconName(self):
    return "green-ball"

class ItemFuncNode(ItemNode):
  def panel1(self,parent):
    return panels.PanelFunc(parent,self)

  panels=[("Panel1",panel1)]

  def getIconName(self):
    return "green-ball"

  def FuncChanged(self, newText ):
    self.myFunc=str(newText)

class ItemService(ItemNode):
  def panel1(self,parent):
    """Retourne un widget pour browser/editer l'item"""
    self.myName=self.node.getName()

    qvbox=QVBox(parent)
    qvbox.layout().setAlignment(Qt.AlignTop|Qt.AlignLeft)
    qvbox.setSpacing( 5 )

    row0=QHBox(qvbox)
    label=QLabel("Name: ",row0)
    self.lined0 = QLineEdit(self.node.getName(),row0)
    qvbox.connect( self.lined0, SIGNAL("textChanged(const QString &)"), self.NameChanged )
    qvbox.connect( self.lined0, SIGNAL("returnPressed()"), self.NameReturn )
    QToolTip.add( self.lined0, "Node name" )

    row1=QHBox(qvbox)
    label1=QLabel("Ref: ",row1)
    self.lined1 = QLineEdit(row1)
    if self.node.getComponent():
      self.lined1.setText(self.node.getComponent().getName())
    else:
      self.lined1.setText("NO_COMPONENT_NAME")

    row2=QHBox(qvbox)
    label2=QLabel("Method: ",row2)
    self.lined2 = QLineEdit(row2)
    self.lined2.setText(self.node.getMethod())

    row3=QHBox(qvbox)
    but1=QPushButton( "Save", row3 )
    but2=QPushButton( "Cancel", row3 )
    qvbox.connect( but1, SIGNAL("clicked()"), self.handleSave )
    qvbox.connect( but2, SIGNAL("clicked()"), self.handleCancel )

    return qvbox

  panels=[("Panel1",panel1)]

  def NameChanged(self, newText ):
    self.myName=str(newText)

  def NameReturn(self):
    pass

  def getIconName(self):
    return "green-square"

  def handleSave(self):
    self.node.setRef(str(self.lined1.text()))
    self.node.setMethod(str(self.lined2.text()))
  def handleCancel(self):
    self.lined0.setText(self.node.getName())
    self.lined1.setText(self.node.getComponent().getName())
    self.lined2.setText(self.node.getMethod())

def adapt_Proc_to_Item(obj, protocol, alternate):
  return ItemProc(obj)

def adapt_Node_to_Item(obj, protocol, alternate):
  return ItemNode(obj)

def adapt_ComposedNode_to_Item(obj, protocol, alternate):
  return ItemComposedNode(obj)

def adapt_ForLoop_to_Item(obj, protocol, alternate):
  return ItemForLoop(obj)

def adapt_Switch_to_Item(obj, protocol, alternate):
  return ItemSwitch(obj)

def adapt_While_to_Item(obj, protocol, alternate):
  return ItemWhile(obj)

def adapt_ForEach_to_Item(obj, protocol, alternate):
  return ItemForEach(obj)

def adapt_InlineFuncNode_to_Item(obj, protocol, alternate):
  return ItemFuncNode(obj)

def adapt_InlineScriptNode_to_Item(obj, protocol, alternate):
  return ItemScriptNode(obj)

def adapt_ServiceNode_to_Item(obj, protocol, alternate):
  return ItemService(obj)

def adapt_Port_to_Item(obj, protocol, alternate):
  return ItemPort(obj)

def adapt_InPort_to_Item(obj, protocol, alternate):
  return ItemInPort(obj)

def adapt_OutPort_to_Item(obj, protocol, alternate):
  return ItemOutPort(obj)

def adapt_InStream_to_Item(obj, protocol, alternate):
  return ItemInStream(obj)

def adapt_OutStream_to_Item(obj, protocol, alternate):
  return ItemOutStream(obj)

def adapt_InGate_to_Item(obj, protocol, alternate):
  return ItemInGate(obj)

def adapt_OutGate_to_Item(obj, protocol, alternate):
  return ItemOutGate(obj)

if hasattr(pilot,"ProcPtr"):
  #SWIG 1.3.24
  adapt.registerAdapterFactory(pilot.ProcPtr, Item.Item, adapt_Proc_to_Item)
  adapt.registerAdapterFactory(pilot.BlocPtr, Item.Item, adapt_ComposedNode_to_Item)
  adapt.registerAdapterFactory(pilot.ForLoopPtr, Item.Item, adapt_ComposedNode_to_Item)
  adapt.registerAdapterFactory(pilot.WhileLoopPtr, Item.Item, adapt_ComposedNode_to_Item)
  adapt.registerAdapterFactory(pilot.ForEachLoopPtr, Item.Item, adapt_ComposedNode_to_Item)
  adapt.registerAdapterFactory(pilot.SwitchPtr, Item.Item, adapt_ComposedNode_to_Item)
  adapt.registerAdapterFactory(pilot.ComposedNodePtr, Item.Item, adapt_ComposedNode_to_Item)

  adapt.registerAdapterFactory(pilot.ServiceNodePtr, Item.Item, adapt_ServiceNode_to_Item)
  #adapt.registerAdapterFactory(pilot.ServiceNodeNodePtr, Item.Item, adapt_Node_to_Item)
  adapt.registerAdapterFactory(pilot.InlineNodePtr, Item.Item, adapt_InlineScriptNode_to_Item)
  adapt.registerAdapterFactory(pilot.InlineFuncNodePtr, Item.Item, adapt_InlineFuncNode_to_Item)
  adapt.registerAdapterFactory(pilot.NodePtr, Item.Item, adapt_Node_to_Item)

  adapt.registerAdapterFactory(pilot.OutputPortPtr, Item.Item, adapt_OutPort_to_Item)
  adapt.registerAdapterFactory(pilot.InputPortPtr, Item.Item, adapt_InPort_to_Item)
  adapt.registerAdapterFactory(pilot.OutputDataStreamPortPtr, Item.Item, adapt_OutStream_to_Item)
  adapt.registerAdapterFactory(pilot.InputDataStreamPortPtr, Item.Item, adapt_InStream_to_Item)

  pilot_InputDataStreamPort=pilot.InputDataStreamPortPtr

else:
  #SWIG 1.3.29
  adapt.registerAdapterFactory(pilot.Proc, Item.Item, adapt_Proc_to_Item)
  adapt.registerAdapterFactory(SALOMERuntime.SalomeProc, Item.Item, adapt_Proc_to_Item)
  adapt.registerAdapterFactory(pilot.Bloc, Item.Item, adapt_ComposedNode_to_Item)
  adapt.registerAdapterFactory(pilot.ForLoop, Item.Item, adapt_ForLoop_to_Item)

  adapt.registerAdapterFactory(pilot.WhileLoop, Item.Item, adapt_While_to_Item)
  adapt.registerAdapterFactory(pilot.ForEachLoop, Item.Item, adapt_ForEach_to_Item)
  adapt.registerAdapterFactory(pilot.Switch, Item.Item, adapt_Switch_to_Item)
  adapt.registerAdapterFactory(pilot.ComposedNode, Item.Item, adapt_ComposedNode_to_Item)

  adapt.registerAdapterFactory(pilot.ServiceNode, Item.Item, adapt_ServiceNode_to_Item)
  adapt.registerAdapterFactory(SALOMERuntime.CORBANode, Item.Item, adapt_ServiceNode_to_Item)
  adapt.registerAdapterFactory(SALOMERuntime.SalomeNode, Item.Item, adapt_ServiceNode_to_Item)
  #adapt.registerAdapterFactory(pilot.ServiceNodeNode, Item.Item, adapt_Node_to_Item)
  adapt.registerAdapterFactory(pilot.InlineNode, Item.Item, adapt_InlineScriptNode_to_Item)
  adapt.registerAdapterFactory(SALOMERuntime.PythonNode, Item.Item, adapt_InlineScriptNode_to_Item)
  adapt.registerAdapterFactory(pilot.InlineFuncNode, Item.Item, adapt_InlineFuncNode_to_Item)
  adapt.registerAdapterFactory(SALOMERuntime.PyFuncNode, Item.Item, adapt_InlineFuncNode_to_Item)
  adapt.registerAdapterFactory(pilot.Node, Item.Item, adapt_Node_to_Item)

  adapt.registerAdapterFactory(pilot.OutputPort, Item.Item, adapt_OutPort_to_Item)
  adapt.registerAdapterFactory(SALOMERuntime.OutputPyPort, Item.Item, adapt_OutPort_to_Item)
  adapt.registerAdapterFactory(SALOMERuntime.OutputCorbaPort, Item.Item, adapt_OutPort_to_Item)
  adapt.registerAdapterFactory(pilot.InputPort, Item.Item, adapt_InPort_to_Item)
  adapt.registerAdapterFactory(SALOMERuntime.InputPyPort, Item.Item, adapt_InPort_to_Item)
  adapt.registerAdapterFactory(SALOMERuntime.InputCorbaPort, Item.Item, adapt_InPort_to_Item)
  adapt.registerAdapterFactory(pilot.OutputDataStreamPort, Item.Item, adapt_OutStream_to_Item)
  adapt.registerAdapterFactory(pilot.InputDataStreamPort, Item.Item, adapt_InStream_to_Item)
  adapt.registerAdapterFactory(pilot.OutGate, Item.Item, adapt_OutGate_to_Item)
  adapt.registerAdapterFactory(pilot.InGate, Item.Item, adapt_InGate_to_Item)

  pilot_InputDataStreamPort=pilot.InputDataStreamPort
