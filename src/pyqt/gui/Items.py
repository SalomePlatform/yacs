import sys
import pilot
import Item
import adapt
from qt import *
from qtcanvas import *
from GraphViewer import GraphViewer
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
  """Item pour les liens controle d'un noeud compose"""
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

class DataLinksItem(Item.Item):
  """Item pour les liens data d'un noeud compose"""
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

class ItemComposedNode(Item.Item):
  """Item pour les noeuds composes"""
  n=0
  def __init__(self,node):
    #node is an instance of YACS::ENGINE::ComposedNode
    Item.Item.__init__(self)
    self.node=node
    self.graph=None
    self.label=node.getName()

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
    sublist.append(DataLinksItem(self))
    sublist.append(ControlLinksItem(self))
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

class ItemForLoop(ItemComposedNode):
  """Item pour la procedure"""
  def addNode(self,service):
    print "ForLoop.addNode",service
    new_node=service.clone(None)
    ItemComposedNode.n=ItemComposedNode.n+1
    name=service.getName()+"_%d" % ItemComposedNode.n
    new_node.setName(name)
    self.node.edSetNode(new_node)
    item=Item.adapt(new_node)
    CONNECTOR.Emit(self,"add",item)

class ItemWhile(ItemForLoop):
  pass

class ItemForEach(ItemForLoop):
  pass

class ItemSwitch(ItemComposedNode):
  def addNode(self,service):
    return

class ItemProc(ItemComposedNode):
  """Item pour la procedure"""

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

class ItemInPort(ItemPort):
  def getIconName(self):
    return "inport.png"

  def panel1(self,parent):
    qvbox=QVBox(parent)
    qvbox.layout().setAlignment(Qt.AlignTop|Qt.AlignLeft)
    qvbox.setSpacing( 5 )
    row0=QHBox(qvbox)
    label=QLabel("Name: ",row0)
    lined0 = QLineEdit(self.port.getName(),row0)
    label=QLabel("Type: ",row0)
    QLineEdit(self.port.edGetType().name(),row0)

    label=QLabel("Value: ",qvbox)
    #self.value=QLabel("Empty",qvbox)
    self.value=QTextEdit(qvbox)
    self.value.setText("Empty")
    if not self.port.isEmpty():
      self.value.setText(self.port.dump())

    row3=QHBox(qvbox)
    but2=QPushButton( "Refresh", row3 )
    qvbox.connect( but2, SIGNAL("clicked()"), self.handleRefresh )

    return qvbox

  def handleRefresh(self):
    if not self.port.isEmpty():
      self.value.setText(self.port.dump())

  panels=[("Panel1",panel1)]

class ItemOutPort(ItemPort):
  def getIconName(self):
    return "outport.png"

  def panel1(self,parent):
    qvbox=QVBox(parent)
    qvbox.layout().setAlignment(Qt.AlignTop|Qt.AlignLeft)
    qvbox.setSpacing( 5 )

    row0=QHBox(qvbox)
    QLabel("Name: ",row0)
    QLineEdit(self.port.getName(),row0)
    QLabel("Type: ",row0)
    QLineEdit(self.port.edGetType().name(),row0)

    QLabel("Value: ",qvbox)
    self.value=QTextEdit(qvbox)
    self.value.setText("Empty")
    try:
      self.value.setText(self.port.dump())
    except:
      traceback.print_exc()

    row3=QHBox(qvbox)
    but2=QPushButton( "Refresh", row3 )
    qvbox.connect( but2, SIGNAL("clicked()"), self.handleRefresh )

    return qvbox

  def handleRefresh(self):
    try:
      self.value.setText(self.port.dump())
    except:
      traceback.print_exc()

  panels=[("Panel1",panel1)]


class ItemInStream(ItemPort):
  def getIconName(self):
    return "instream.png"
class ItemOutStream(ItemPort):
  def getIconName(self):
    return "outstream.png"
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

  def handleSave(self):
    self.node.setScript(str(self.mle.text()))

  def handleCancel(self):
    self.lined0.setText(self.node.getName())
    self.mle.setText(self.node.getScript())

class ItemFuncNode(ItemNode):
  def panel1(self,parent):
    """Retourne un widget pour browser/editer l'item"""
    qvbox=QVBox(parent)
    qvbox.setSpacing( 5 )

    row0=QHBox(qvbox)
    label=QLabel("Name: ",row0)
    self.lined0 = QLineEdit(self.node.getName(),row0)

    row1=QHBox(qvbox)
    label=QLabel("Fname: ",row1)
    self.lined1 = QLineEdit(self.node.getFname(),row1)

    label=QLabel("Function: ",qvbox)
    self.mle=Editor.Editor(qvbox,"multiLineEdit" )
    self.mle.setText(self.node.getScript())

    row2=QHBox(qvbox)
    but1=QPushButton( "Save", row2 )
    but2=QPushButton( "Cancel", row2 )
    qvbox.connect( but1, SIGNAL("clicked()"), self.handleSave )
    qvbox.connect( but2, SIGNAL("clicked()"), self.handleCancel )

    return qvbox

  panels=[("Panel1",panel1)]

  def getIconName(self):
    return "green-ball"
  def FuncChanged(self, newText ):
    self.myFunc=str(newText)

  def handleSave(self):
    self.node.setFname(str(self.lined1.text()))
    self.node.setScript(str(self.mle.text()))
  def handleCancel(self):
    self.lined0.setText(self.node.getName())
    self.lined1.setText(self.node.getFname())
    self.mle.setText(self.node.getScript())

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
  print "adapt_ForLoop_to_Item",obj
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
  adapt.registerAdapterFactory(pilot.Bloc, Item.Item, adapt_ComposedNode_to_Item)
  adapt.registerAdapterFactory(pilot.ForLoop, Item.Item, adapt_ForLoop_to_Item)

  adapt.registerAdapterFactory(pilot.WhileLoop, Item.Item, adapt_While_to_Item)
  adapt.registerAdapterFactory(pilot.ForEachLoop, Item.Item, adapt_ForEach_to_Item)
  adapt.registerAdapterFactory(pilot.Switch, Item.Item, adapt_Switch_to_Item)
  adapt.registerAdapterFactory(pilot.ComposedNode, Item.Item, adapt_ComposedNode_to_Item)

  adapt.registerAdapterFactory(pilot.ServiceNode, Item.Item, adapt_ServiceNode_to_Item)
  #adapt.registerAdapterFactory(pilot.ServiceNodeNode, Item.Item, adapt_Node_to_Item)
  adapt.registerAdapterFactory(pilot.InlineNode, Item.Item, adapt_InlineScriptNode_to_Item)
  adapt.registerAdapterFactory(pilot.InlineFuncNode, Item.Item, adapt_InlineFuncNode_to_Item)
  adapt.registerAdapterFactory(pilot.Node, Item.Item, adapt_Node_to_Item)

  adapt.registerAdapterFactory(pilot.OutputPort, Item.Item, adapt_OutPort_to_Item)
  adapt.registerAdapterFactory(pilot.InputPort, Item.Item, adapt_InPort_to_Item)
  adapt.registerAdapterFactory(pilot.OutputDataStreamPort, Item.Item, adapt_OutStream_to_Item)
  adapt.registerAdapterFactory(pilot.InputDataStreamPort, Item.Item, adapt_InStream_to_Item)
  adapt.registerAdapterFactory(pilot.OutGate, Item.Item, adapt_OutGate_to_Item)
  adapt.registerAdapterFactory(pilot.InGate, Item.Item, adapt_InGate_to_Item)

  pilot_InputDataStreamPort=pilot.InputDataStreamPort
