
from qt import *
import traceback
import Editor
import Item

class PanelScript(QVBox):
  def __init__(self,parent,item):
    QVBox.__init__(self,parent)
    self.item=item
    self.setSpacing( 5 )

    row0=QHBox(self)
    label=QLabel("Name: ",row0)
    self.lined0 = QLineEdit(item.node.getName(),row0)

    label=QLabel("Script: ",self)
    self.mle=Editor.Editor(self,"multiLineEdit" )
    self.mle.setText(item.node.getScript())

    row2=QHBox(self)
    but1=QPushButton( "Save", row2 )
    but1.setFixedSize( but1.sizeHint())
    but2=QPushButton( "Cancel", row2 )
    but2.setFixedSize( but2.sizeHint())
    self.connect( but1, SIGNAL("clicked()"), self.handleSave )
    self.connect( but2, SIGNAL("clicked()"), self.handleCancel )

  def handleSave(self):
    self.item.node.setScript(str(self.mle.text()))

  def handleCancel(self):
    self.lined0.setText(self.item.node.getName())
    self.mle.setText(self.item.node.getScript())

class PanelFunc(QVBox):
  def __init__(self,parent,item):
    QVBox.__init__(self,parent)
    self.item=item
    self.setSpacing( 5 )

    row0=QHBox(self)
    label=QLabel("Name: ",row0)
    self.lined0 = QLineEdit(self.item.node.getName(),row0)

    row1=QHBox(self)
    label=QLabel("Fname: ",row1)
    self.lined1 = QLineEdit(self.item.node.getFname(),row1)

    label=QLabel("Function: ",self)
    self.mle=Editor.Editor(self,"multiLineEdit" )
    self.mle.setText(self.item.node.getScript())

    row2=QHBox(self)
    but1=QPushButton( "Save", row2 )
    but1.setFixedSize( but1.sizeHint())
    but2=QPushButton( "Cancel", row2 )
    but2.setFixedSize( but2.sizeHint())
    self.connect( but1, SIGNAL("clicked()"), self.handleSave )
    self.connect( but2, SIGNAL("clicked()"), self.handleCancel )

  def handleSave(self):
    self.item.node.setFname(str(self.lined1.text()))
    self.item.node.setScript(str(self.mle.text()))

  def handleCancel(self):
    self.lined0.setText(self.item.node.getName())
    self.lined1.setText(self.item.node.getFname())
    self.mle.setText(self.item.node.getScript())

class PanelForLoop(QVBox):
  def __init__(self,parent,item):
    QVBox.__init__(self,parent)
    self.item=item
    vsplit=QSplitter(Qt.Vertical,self,"VSplitter")
    vbox=QVBox(vsplit)
    vbox.layout().setAlignment(Qt.AlignTop|Qt.AlignLeft)

    row0=QHBox(vbox)
    label=QLabel("Name: ",row0)
    self.lined0 = QLineEdit(item.node.getName(),row0)

    #row1=QVBox(self)
    #self.setStretchFactor(row1,10)

    row2=QHBox(vbox)
    but1=QPushButton( "Save", row2 )
    but1.setFixedSize( but1.sizeHint())
    but2=QPushButton( "Cancel", row2 )
    but2.setFixedSize( but2.sizeHint())
    self.connect( but1, SIGNAL("clicked()"), self.handleSave )
    self.connect( but2, SIGNAL("clicked()"), self.handleCancel )

    nodes= item.node.edGetDirectDescendants()
    if nodes:
      node=nodes[0]
      subitem=Item.adapt(node)
      panel=subitem.box(vsplit)

  def handleSave(self):
    return
  def handleCancel(self):
    return

class PanelInPort(QVBox):
  def __init__(self,parent,item):
    QVBox.__init__(self,parent)
    self.item=item
    self.layout().setAlignment(Qt.AlignTop|Qt.AlignLeft)
    self.setSpacing( 5 )
    row0=QHBox(self)
    label=QLabel("Name: ",row0)
    lined0 = QLineEdit(self.item.port.getName(),row0)
    label=QLabel("Type: ",row0)
    QLineEdit(self.item.port.edGetType().name(),row0)

    label=QLabel("Value: ",self)
    self.value=QTextEdit(self)
    self.value.setText("Empty")
    self.handleRestore()

    row3=QHBox(self)
    but1=QPushButton( "Save", row3 )
    but1.setFixedSize( but1.sizeHint())
    self.connect( but1, SIGNAL("clicked()"), self.handleSave )
    but2=QPushButton( "Restore", row3 )
    but2.setFixedSize( but2.sizeHint())
    self.connect( but2, SIGNAL("clicked()"), self.handleRestore )

  def handleSave(self):
    try:
      self.item.port.edInitXML(str(self.value.text()))
    except:
      traceback.print_exc()
      self.value.setText(self.item.port.dump())

  def handleRestore(self):
    if not self.item.port.isEmpty():
      self.value.setText(self.item.port.dump())

class PanelOutPort(PanelInPort):
  def handleRestore(self):
    try:
      self.value.setText(self.item.port.dump())
    except:
      traceback.print_exc()
