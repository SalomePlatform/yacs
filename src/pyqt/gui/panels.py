
from qt import *
import Editor

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

