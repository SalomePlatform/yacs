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


class DataLinkItem(Item.Item):
  def __init__(self,pin,pout):
    self.pin=pin
    self.pout=pout
    self.label= pout.getNode().getName()+":"+pout.getName()+"->"+pin.getNode().getName()+":"+pin.getName()

  def getIconName(self):
    return "datalink.png"

class StreamLinkItem(Item.Item):
  def __init__(self,pin,pout):
    self.pin=pin
    self.pout=pout
    self.label= pout.getNode().getName()+":"+pout.getName()+"->"+pin.getNode().getName()+":"+pin.getName()

  def getIconName(self):
    return "streamlink.png"

class ControlLinkItem(Item.Item):
  def __init__(self,nodeup,nodedown):
    self.nodedown=nodedown
    self.nodeup=nodeup
    self.label= nodeup.getName()+"->"+nodedown.getName()

  def getIconName(self):
    return "controllink.png"

class ControlLinksItem(Item.Item):
  """Item pour les liens controle d'un noeud compose"""
  def __init__(self,item):
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

class MyCanvas(QCanvas):
  def customEvent(self,event):
    object=event.data()
    object.customEvent(event)
    self.update()

class ItemComposedNode(Item.Item):
  """Item pour les noeuds composes"""
  n=0
  def __init__(self,node):
    #node is an instance of YACS::ENGINE::ComposedNode
    self.node=node
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
        sublist.append(adapt.adapt(n,Item.Item))
      except:
        print n
        #traceback.print_exc()
        raise
    sublist.append(DataLinksItem(self))
    sublist.append(ControlLinksItem(self))
    return sublist

  def getIconName(self):
    return "green-los"

  def panel(self,parent):
    """Retourne un tab widget pour browser/editer la proc"""
    tabWidget = QTabWidget( parent )
    for name,method in self.panels:
      tabWidget.addTab( method(self,tabWidget), name )
    return tabWidget

  def addNode(self):
    r=pilot.getRuntime()
    ItemComposedNode.n=ItemComposedNode.n+1
    n1=r.createScriptNode("","unknown_%d" % ItemComposedNode.n)
    node=CItems.Cell(n1,self.canvas)
    node.show()
    self.citems[n1.ptr()]=node
    self.canvas.update()

  def panel1(self,parent):
    qvbox=QVBox(parent)
    #Canvas size : 10000x10000
    self.canvas=MyCanvas(10000,10000)
    self.editor=GraphViewer(self,self.canvas,qvbox,"example",0)

    #permet de retrouver un item node dans le canvas a partir 
    #d'un proxy swig quelconque (astuce)
    #Pour retrouver un item node faire : citems[node.ptr()]
    citems={}
    self.citems=citems
    #permet de retrouver un item port dans le canvas a partir 
    #d'un proxy swig quelconque (astuce)
    #Pour retrouver un item port faire : pitems[port.ptr()]
    pitems={}
    #self.pitems=pitems

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

    return qvbox

  panels=[("Panel1",panel1)]

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

class ItemProc(ItemComposedNode):
  """Item pour la procedure"""

class ItemPort(Item.Item):
  """Item pour les ports """
  panels=[]
  def __init__(self,port):
    self.port=port
    self.label=port.getName()

  def getIconName(self):
    return "port.png"

  def panel(self,parent):
    """Retourne un tab widget pour browser/editer l'item"""
    tabWidget = QTabWidget( parent )
    for name,method in self.panels:
      tabWidget.addTab( method(self,tabWidget), name )
    return tabWidget

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

class ItemNode(Item.Item):
  """Item pour les noeuds elementaires
     Il n a pas de fils
  """
  #attr donnant la liste des panels du noeud (nom,method)
  panels=[]
  def __init__(self,node):
    self.node=node
    self.label=node.getName()

  def isExpandable(self):
    return True

  def getChildren(self):
    sublist=[]
    for n in self.node.getSetOfInputPort():
      sublist.append(adapt.adapt(n,Item.Item))
    for n in self.node.getSetOfOutputPort():
      sublist.append(adapt.adapt(n,Item.Item))
    for n in self.node.getSetOfInputDataStreamPort():
      sublist.append(adapt.adapt(n,Item.Item))
    for n in self.node.getSetOfOutputDataStreamPort():
      sublist.append(adapt.adapt(n,Item.Item))
    return sublist

  def panel(self,parent):
    """Retourne un tab widget pour browser/editer l'item"""
    tabWidget = QTabWidget( parent )
    for name,method in self.panels:
      tabWidget.addTab( method(self,tabWidget), name )
    return tabWidget

class ItemScriptNode(ItemNode):

  def panel1(self,parent):
    qvbox=QVBox(parent)
    qvbox.setSpacing( 5 )

    row0=QHBox(qvbox)
    label=QLabel("Name: ",row0)
    lined0 = QLineEdit(self.node.getName(),row0)

    label=QLabel("Script: ",qvbox)
    mle=Editor.Editor(qvbox,"multiLineEdit" )
    mle.setText(self.node.getScript())

    row2=QHBox(qvbox)
    but1=QPushButton( "Save", row2 )
    but2=QPushButton( "Cancel", row2 )
    qvbox.connect( but1, SIGNAL("clicked()"), self.handleSave )
    qvbox.connect( but2, SIGNAL("clicked()"), self.handleCancel )

    return qvbox

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

if hasattr(pilot,"ProcPtr"):
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
  adapt.registerAdapterFactory(pilot.Proc, Item.Item, adapt_Proc_to_Item)
  adapt.registerAdapterFactory(pilot.Bloc, Item.Item, adapt_ComposedNode_to_Item)
  adapt.registerAdapterFactory(pilot.ForLoop, Item.Item, adapt_ComposedNode_to_Item)
  adapt.registerAdapterFactory(pilot.WhileLoop, Item.Item, adapt_ComposedNode_to_Item)
  adapt.registerAdapterFactory(pilot.ForEachLoop, Item.Item, adapt_ComposedNode_to_Item)
  adapt.registerAdapterFactory(pilot.Switch, Item.Item, adapt_ComposedNode_to_Item)
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

  pilot_InputDataStreamPort=pilot.InputDataStreamPort
