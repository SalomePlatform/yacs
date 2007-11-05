
import sys,traceback
from qt import *
from qtcanvas import *
import pilot
import Item

dispatcher=pilot.Dispatcher.getDispatcher()

class TextItem(QCanvasText):
  """A text in a composite object"""
  def __init__(self,obj,canvas):
    QCanvasText.__init__(self,canvas)
    self.obj=obj
    self.item=None
  def getObj(self):
    """The composite object which contains the text"""
    return self.obj
  def moveBy(self,dx,dy):
    """Request the text move by x,y"""
    if self.obj:
      #the text is a part of a composite object
      self.obj.moveBy(dx,dy)
    else:
      #the text is independant
      self.myMove(dx,dy)
  def myMove(self,dx,dy):
    """The real move"""
    QCanvasText.moveBy(self,dx,dy)
  def selected(self):
    """The canvas item has been selected"""
    if self.obj:
      self.obj.selected()

class PointItem(QCanvasEllipse):
  def __init__(self,obj,x,y,canvas):
    """Create a point contained in a composite line (obj)"""
    QCanvasEllipse.__init__(self,6,6,canvas)
    self.obj=obj
    self.item=None
    self.inline=None
    self.outline=None
    self.setPen(QPen(Qt.black))
    self.setBrush(QBrush(Qt.red))
    self.setX(x)
    self.setY(y)
    self.setVisible(True)

  def setInline(self,inline):
    self.inline=inline
    if inline.z() >= self.z():
      self.setZ(inline.z()+1)
  def setOutline(self,outline):
    self.outline=outline
    if outline.z() >= self.z():
      self.setZ(outline.z()+1)

  def moveBy(self,dx,dy):
    """Request the point move by x,y"""
    self.myMove(dx,dy)

  def myMove(self,dx,dy):
    """The real move"""
    QCanvasEllipse.moveBy(self,dx,dy)
    if self.outline:
      self.outline.setFromPoint( int(self.x()), int(self.y()) )
    if self.inline:
      self.inline.setToPoint( int(self.x()), int(self.y()) )

  def getObj(self):
    """The object which contains the point"""
    return self.obj

  def handleDoubleClick(self,pos):
    self.obj.deletePoint(self,pos)
  #def __del__(self):
  #  print "PointItem.__del__"

  def clear(self):
    """To remove from canvas"""
    self.setCanvas(None)
    self.obj=None
    self.inline=None
    self.outline=None

  def selected(self):
    """The canvas item has been selected"""

class LineItem(QCanvasLine):
  """A line between 2 points"""
  def __init__(self,obj,fromPoint, toPoint,canvas):
    QCanvasLine.__init__(self,canvas)
    self.obj=obj
    self.item=None
    self.fromPoint=fromPoint
    self.toPoint=toPoint
    self.setPen(QPen(Qt.black))
    self.setBrush(QBrush(Qt.red))
    self.setPoints(int(fromPoint.x()),int(fromPoint.y()), int(toPoint.x()), int(toPoint.y()))
    self.setZ(min(fromPoint.z(),toPoint.z())-1)
    self.setVisible(True)

  def setFromPoint(self,x,y):
    self.setPoints(x,y,self.endPoint().x(),self.endPoint().y())
  def setToPoint(self,x,y):
    self.setPoints(self.startPoint().x(), self.startPoint().y(),x,y)
  def moveBy(self,dx,dy):
    """Disable line move"""
    pass

  def getObj(self):
    """The object which contains the line"""
    return self.obj
  def handleDoubleClick(self,pos):
    #split the line
    self.obj.splitline(self,pos)
  #def __del__(self):
  #  print "LineItem.__del__"
  def clear(self):
    """To remove from canvas"""
    self.setCanvas(None)
    self.fromPoint=None
    self.toPoint=None
    self.obj=None
  def selected(self):
    """The canvas item has been selected"""

class LinkItem:
  def __init__(self,fromPort, toPort,canvas):
    self.fromPort=fromPort
    self.toPort=toPort
    self.canvas=canvas
    self.item=None
    fromPort.addOutLink(self)
    toPort.addInLink(self)
    self.lines=[]
    self.points=[]
    self.lines.append(LineItem(self,fromPort, toPort,canvas))

  def deletePoint(self,point,pos):
    """Delete intermediate point"""
    if point not in self.points:
      return
    self.points.remove(point)
    inline=point.inline
    outline=point.outline
    inline.toPoint=outline.toPoint
    inline.setToPoint(outline.toPoint.x(),outline.toPoint.y())
    self.lines.remove(outline)
    if inline.toPoint in self.points:
      inline.toPoint.setInline(inline)
    #remove from canvas
    point.clear()
    outline.clear()

  def splitline(self,line,pos):
    """Split line at position pos"""
    #The new point
    point=PointItem(self,pos.x(),pos.y(),self.canvas)
    self.points.append(point)
    i=self.lines.index(line)

    newline=LineItem(self,point,line.toPoint,self.canvas)
    if line.toPoint in self.points:
      #line not connected to port : reconnect newline
      line.toPoint.setInline(newline)
    self.lines.insert(i+1,newline)

    line.setToPoint(pos.x(),pos.y())
    line.toPoint=point
    point.setInline(line)
    point.setOutline(newline)

  def setFromPoint(self,x,y):
    first=self.lines[0]
    first.setFromPoint(x,y)

  def setToPoint(self,x,y):
    last=self.lines[-1]
    last.setToPoint(x,y)

  def moveBy(self,dx,dy):
    pass

  def popup(self,canvasView):
    menu=QPopupMenu()
    caption = QLabel( "<font color=darkblue><u><b>Link Menu</b></u></font>",menu )
    caption.setAlignment( Qt.AlignCenter )
    menu.insertItem( caption )
    menu.insertItem("Delete", self.delete)
    return menu

  def delete(self):
    print "delete link"

  def tooltip(self,view,pos):
    r = QRect(pos.x(), pos.y(), pos.x()+10, pos.y()+10)
    s = QString( "link: "+self.fromPort.port.getNode().getName() +":"+self.fromPort.port.getName()+"->"+self.toPort.port.getNode().getName()+":"+self.toPort.port.getName()  )
    view.tip( r, s )
    #QToolTip(view).tip( r, s )
  def selected(self):
    """The canvas item has been selected"""

class ControlLinkItem(LinkItem):
  def tooltip(self,view,pos):
    r = QRect(pos.x(), pos.y(), pos.x()+10, pos.y()+10)
    s = QString( "link: "+self.fromPort.port.getNode().getName()+"->"+self.toPort.port.getNode().getName())
    view.tip( r, s )
    #QToolTip(view).tip( r, s )

class ControlItem(QCanvasRectangle):
  def __init__(self,node,port,canvas):
    QCanvasRectangle.__init__(self,canvas)
    self.setSize(6,6)
    self.port=port
    self.setPen(QPen(Qt.black))
    self.setBrush(QBrush(Qt.red))
    self.setZ(node.z()+1)
    self.node=node
    self.item=Item.adapt(self.port)

  def moveBy(self,dx,dy):
    self.node.moveBy(dx,dy)
  def myMove(self,dx,dy):
    QCanvasRectangle.moveBy(self,dx,dy)
  def getObj(self):
    return self
  def popup(self,canvasView):
    self.context=canvasView
    menu=QPopupMenu()
    caption = QLabel( "<font color=darkblue><u><b>Port Menu</b></u></font>",menu )
    caption.setAlignment( Qt.AlignCenter )
    menu.insertItem( caption )
    menu.insertItem("Connect", self.connect)
    return menu
  def connect(self):
    print "connect",self.context
    self.context.connecting(self)
  def link(self,obj):
    print "link:",obj
  def tooltip(self,view,pos):
    r = QRect(pos.x(), pos.y(), self.width(), self.height())
    s = QString( "gate:")
    view.tip( r, s )
    #QToolTip(view).tip( r, s )
  def selected(self):
    """The canvas item has been selected"""
    #print "control port selected"
    item=Item.adapt(self.port)
    item.selected()

class InControlItem(ControlItem):
  def __init__(self,node,port,canvas):
    ControlItem.__init__(self,node,port,canvas)
    self.__inList=[]

  def myMove(self,dx,dy):
    ControlItem.myMove(self,dx,dy)
    for link in self.__inList:
      link.setToPoint( int(self.x()), int(self.y()) )

  def link(self,obj):
    print "link:",obj
    if isinstance(obj,OutControlItem):
      #Connection possible
      l=LinkItem(obj,self,self.canvas())

  def addInLink(self,link):
    self.__inList.append(link)

  def tooltip(self,view,pos):
    r = QRect(pos.x(), pos.y(), self.width(), self.height())
    s = QString( "ingate:")
    view.tip( r, s )
    #QToolTip(view).tip( r, s )

class OutControlItem(ControlItem):
  def __init__(self,node,port,canvas):
    ControlItem.__init__(self,node,port,canvas)
    self.__outList=[]

  def myMove(self,dx,dy):
    ControlItem.myMove(self,dx,dy)
    for link in self.__outList:
      link.setFromPoint( int(self.x()), int(self.y()) )

  def link(self,obj):
    print "link:",obj
    if isinstance(obj,InControlItem):
      #Connection possible
      l=LinkItem(self,obj,self.canvas())

  def addOutLink(self,link):
    self.__outList.append(link)

  def tooltip(self,view,pos):
    r = QRect(pos.x(), pos.y(), self.width(), self.height())
    s = QString( "outgate:")
    view.tip( r, s )
    #QToolTip(view).tip( r, s )

class PortItem(QCanvasEllipse):
  def __init__(self,node,port,canvas):
    QCanvasEllipse.__init__(self,6,6,canvas)
    self.port=port
    self.item=None
    self.setPen(QPen(Qt.black))
    self.setBrush(QBrush(Qt.red))
    self.setZ(node.z()+1)
    self.node=node

  def moveBy(self,dx,dy):
    self.node.moveBy(dx,dy)

  def myMove(self,dx,dy):
    QCanvasEllipse.moveBy(self,dx,dy)

  def getObj(self):
    return self

  def popup(self,canvasView):
    self.context=canvasView
    menu=QPopupMenu()
    caption = QLabel( "<font color=darkblue><u><b>Port Menu</b></u></font>",menu )
    caption.setAlignment( Qt.AlignCenter )
    menu.insertItem( caption )
    menu.insertItem("Connect", self.connect)
    return menu

  def connect(self):
    print "connect",self.context
    self.context.connecting(self)

  def link(self,obj):
    print "link:",obj

  def tooltip(self,view,pos):
    r = QRect(pos.x(),pos.y(),self.width(), self.height())
    t=self.port.edGetType()
    s = QString( "port: " + self.port.getName() + ":" + t.name())
    view.tip( r, s )
    #QToolTip(view).tip( r, s )
  def selected(self):
    """The canvas item has been selected"""
    #print "port selected"
    item=Item.adapt(self.port)
    item.selected()

class InPortItem(PortItem):
  def __init__(self,node,port,canvas):
    PortItem.__init__(self,node,port,canvas)
    self.__inList=[]

  def myMove(self,dx,dy):
    PortItem.myMove(self,dx,dy)
    for link in self.__inList:
      link.setToPoint( int(self.x()), int(self.y()) )

  def link(self,obj):
    print "link:",obj
    if isinstance(obj,OutPortItem):
      #Connection possible
      l=LinkItem(obj,self,self.canvas())

  def addInLink(self,link):
    self.__inList.append(link)



class OutPortItem(PortItem):
  def __init__(self,node,port,canvas):
    PortItem.__init__(self,node,port,canvas)
    self.__outList=[]

  def myMove(self,dx,dy):
    PortItem.myMove(self,dx,dy)
    for link in self.__outList:
      link.setFromPoint( int(self.x()), int(self.y()) )

  def link(self,obj):
    print "link:",obj
    if isinstance(obj,InPortItem):
      #Connection possible
      l=LinkItem(self,obj,self.canvas())

  def addOutLink(self,link):
    self.__outList.append(link)

class InStreamItem(InPortItem):
  def __init__(self,node,port,canvas):
    InPortItem.__init__(self,node,port,canvas)
    self.setBrush(QBrush(Qt.green))

class OutStreamItem(OutPortItem):
  def __init__(self,node,port,canvas):
    OutPortItem.__init__(self,node,port,canvas)
    self.setBrush(QBrush(Qt.green))

class Cell(QCanvasRectangle,pilot.PyObserver):
  colors={
      "pink":Qt.cyan,
      "green":Qt.green,
      "magenta":Qt.magenta,
      "purple":Qt.darkMagenta,
      "blue":Qt.blue,
      "red":Qt.red,
      "orange":Qt.yellow,
      "grey":Qt.gray,
      "white":Qt.white,
    }

  def __init__(self,node,canvas):
    print "CItems.Cell"
    QCanvasRectangle.__init__(self,canvas)
    pilot.PyObserver.__init__(self)
    self.inports=[]
    self.outports=[]
    self.setSize(50,50)
    #node is an instance of YACS::ENGINE::Node
    self.node=node
    self.item=Item.adapt(self.node)
    dispatcher.addObserver(self,node,"status")
    self.label=TextItem(self,canvas)
    self.label.setText(self.node.getName())
    self.label.setFont(QFont("Helvetica",8))
    rect=self.label.boundingRect()
    self.label.setZ(self.z()+1)
    self.label.myMove(self.x()+self.width()/2-rect.width()/2,self.y()+self.height()/2-rect.height()/2)
    color= self.colors.get(node.getColorState(node.getEffectiveState()),Qt.white)
    self.setBrush(QBrush(color))

    print "Cell.getSetOfInputPort",
    liste= self.node.getSetOfInputPort()
    print liste
    dy=6
    y=0
    for inport in self.node.getSetOfInputPort():
      p=InPortItem(self,inport,canvas)
      y=y+dy
      p.myMove(0,y)
      self.inports.append(p)

    for instream in self.node.getSetOfInputDataStreamPort():
      p=InStreamItem(self,instream,canvas)
      y=y+dy
      p.myMove(0,y)
      self.inports.append(p)

    ymax=y

    liste= self.node.getSetOfOutputPort()
    dy=6
    y=0
    for outport in self.node.getSetOfOutputPort():
      p=OutPortItem(self,outport,canvas)
      y=y+dy
      p.myMove(50,y)
      self.outports.append(p)

    for outstream in self.node.getSetOfOutputDataStreamPort():
      p=OutStreamItem(self,outstream,canvas)
      y=y+dy
      p.myMove(50,y)
      self.outports.append(p)

    ymax=max(y,ymax)

    #Control ports
    y=ymax+dy
    if y < 44:y=44
    p=InControlItem(self,self.node.getInGate(),canvas)
    p.myMove(0,y)
    self.inports.append(p)
    self.ingate=p
    p=OutControlItem(self,self.node.getOutGate(),canvas)
    p.myMove(44,y)
    self.outports.append(p)
    self.outgate=p
    y=y+dy
    self.setSize(50,y)


  events={
      "status":QEvent.User+1,
      }

  def pynotify(self,object,event):
    #print "pynotify",event,object
    try:
      evType=self.events[event]
      ev=QCustomEvent(evType)
      ev.setData(self)
      ev.yacsEvent=event
      QApplication.postEvent(self.canvas(), ev)
      #request immediate processing (deadlock risk ???)
      #QApplication.sendPostedEvents(self.canvas(), evType)
      #print "pynotify end"
    except:
      #traceback.print_exc()
      raise

  def customEvent(self,event):
    if event.yacsEvent=="status":
      object=self.node
      state=object.getEffectiveState()
      color=object.getColorState(state)
      color= self.colors.get(color,Qt.white)
      self.setBrush(QBrush(color))
    else:
      print "Unknown custom event type:", event.type()

  def moveBy(self,dx,dy):
    QCanvasRectangle.moveBy(self,dx,dy)
    self.label.myMove(dx,dy)
    for p in self.inports:
      p.myMove(dx,dy)
    for p in self.outports:
      p.myMove(dx,dy)

  def show(self):
    QCanvasRectangle.show(self)
    self.label.show()
    for p in self.inports:
      p.show()
    for p in self.outports:
      p.show()

  def getObj(self):
    return self

  def popup(self,canvasView):
    menu=QPopupMenu()
    caption = QLabel( "<font color=darkblue><u><b>Node Menu</b></u></font>",menu )
    caption.setAlignment( Qt.AlignCenter )
    menu.insertItem( caption )
    menu.insertItem("Browse", self.browse)
    return menu

  def tooltip(self,view,pos):
    r = QRect(pos.x(), pos.y(), self.width(), self.height())
    s = QString( "node: " + self.node.getName())
    view.tip( r, s )
    #QToolTip(view).tip( r, s )

  def browse(self):
    print "browse"

  def selected(self):
    """The canvas item has been selected"""
    #print "node selected"
    item=Item.adapt(self.node)
    item.selected()
