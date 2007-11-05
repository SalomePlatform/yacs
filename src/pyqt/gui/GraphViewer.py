
import sys
from qt import *
from qtcanvas import *

class DynamicTip( QToolTip ):
  def __init__( self, parent ):
    QToolTip.__init__( self, parent )

  def maybeTip( self, pos ):
    pos2=self.parentWidget().viewportToContents(pos)
    point = self.parentWidget().inverseWorldMatrix().map(pos2)
    ilist = self.parentWidget().canvas().collisions(point) #QCanvasItemList ilist
    for each_item in ilist:
      if hasattr(each_item,"tooltip"):
        each_item.tooltip(self,pos)
        return
      elif hasattr(each_item,"getObj"):
        each_item.getObj().tooltip(self,pos)
        return

class GraphViewer(QCanvasView):
  def __init__(self,item,c,parent,name,f):
    QCanvasView.__init__(self,c,parent,name,f)
    self.item=item
    self.__moving=0
    self.__connecting=0
    self.__moving_start= 0
    # for highlighting selections
    self.selectPen=QPen(QColor(255,255,0),2,Qt.DashLine)
    self.selectBrush=QBrush(Qt.red)
    self.selectStyle = Qt.Dense5Pattern
    self.selected=None
    self.tooltip = DynamicTip( self )

  def contentsMouseDoubleClickEvent(self,e): # QMouseEvent e
    point = self.inverseWorldMatrix().map(e.pos())
    ilist = self.canvas().collisions(point) #QCanvasItemList ilist
    for each_item in ilist:
      if each_item.rtti()==984376:
        if not each_item.hit(point):
          continue
      if e.button()== Qt.LeftButton:
        if hasattr(each_item,"handleDoubleClick"):
          each_item.handleDoubleClick(point)
          self.canvas().update()
          return


  def contentsMousePressEvent(self,e): # QMouseEvent e
    p=e.globalPos()
    point = self.inverseWorldMatrix().map(e.pos())
    ilist = self.canvas().collisions(point) #QCanvasItemList ilist
    for each_item in ilist:
      if each_item.rtti()==984376:
        if not each_item.hit(point):
          continue
      if e.button()== Qt.RightButton:
        #Right button click
        self.__moving=0
        self.__connecting=0
        if hasattr(each_item,"popup"):
          menu=each_item.popup(self)
          if menu:
            menu.exec_loop( QCursor.pos() )
            self.canvas().update()
        elif hasattr(each_item,"getObj"):
          menu=each_item.getObj().popup(self)
          if menu:
            menu.exec_loop( QCursor.pos() )
            self.canvas().update()
      elif e.button()== Qt.LeftButton:
        #Left button click
        if self.__connecting:
          #We are linking objects
          if hasattr(each_item,"getObj"):
            #a connection is ending
            self.__connecting.link(each_item.getObj())
            self.canvas().update()
          self.__connecting=0
        else:
          #We are moving or selecting a composite object
          #if self.selected:
          #  try:
          #    self.deselectObj(self.selected)
          #  except:
          #    pass
          #  self.selected=None
          #self.selected=each_item
          #self.selectObj(self.selected)
          each_item.selected()
          self.__moving=each_item
          self.__moving_start=point
          self.canvas().update()
      return
    if e.button()== Qt.RightButton:
      menu=self.popup()
      if menu:
        menu.exec_loop( QCursor.pos() )
        self.canvas().update()
    self.__moving=0
    self.__connecting=0
    QCanvasView.contentsMousePressEvent(self,e)

  def selectItem(self,item):
    #print "selectItem",item
    if self.selected:
      try:
        self.deselectObj(self.selected)
      except:
        pass
      self.selected=None
    #need to find equivalent canvas item 
    for citem in self.canvas().allItems():
      if citem.item is item:
        self.selected=citem
        self.selectObj(self.selected)
        break
    self.canvas().update()

  def selectObj(self,obj):
    if obj:
      obj._origPen = obj.pen()
      obj._origBrush = obj.brush()
      obj._origStyle = obj.brush().style()
      #obj.setPen(self.selectPen)
      obj.setBrush(self.selectBrush)

  def deselectObj(self,obj):
    if obj:
      #obj.setPen(obj._origPen)
      obj.setBrush(obj._origBrush)

  def popup(self):
    menu=QPopupMenu()
    caption = QLabel( "<font color=darkblue><u><b>View Menu</b></u></font>", self )
    caption.setAlignment( Qt.AlignCenter )
    menu.insertItem( caption )
    menu.insertItem("add Node", self.addNode)
    return menu

  def layout(self,rankdir):
    print rankdir

  def updateCanvas(self):
    #Par defaut, Qt n'efface pas le background. Seul repaintContents
    #semble le faire. Utile apres un popup ou un resize avec scrollbars
    #Peut-on l'utiliser partout ? Pb de performance ?
    self.repaintContents(True)
    #self.canvas().update()

  #def addNode(self):
  #  self.item.addNode()

  def zoomIn(self):
    m = self.worldMatrix()
    m.scale( 2.0, 2.0 )
    self.setWorldMatrix( m )

  def zoomOut(self):
    m = self.worldMatrix()
    m.scale( 0.5, 0.5 )
    self.setWorldMatrix( m )

  def clear(self):
    ilist = self.canvas().allItems()
    for each_item in ilist:
      if each_item:
        each_item.setCanvas(None)
        del each_item
    self.canvas().update()

  def connecting(self,obj):
    """Method called by an item to notify canvasView a connection has begun"""
    print "connecting",obj
    self.__connecting=obj

  def contentsMouseMoveEvent(self,e):
    if  self.__moving :
      point = self.inverseWorldMatrix().map(e.pos())
      self.__moving.moveBy(point.x()-self.__moving_start.x(),point.y()-self.__moving_start.y())
      self.__moving_start = point
      self.canvas().update()
    else:
      #self.tooltip.maybeTip(point)
      QCanvasView.contentsMouseMoveEvent(self,e)

class ImageItem(QCanvasRectangle):
    def __init__(self,img,canvas):
      QCanvasRectangle.__init__(self,canvas)

      self.imageRTTI=984376
      self.image=img
      self.pixmap=QPixmap()
      self.setSize(self.image.width(), self.image.height())
      self.pixmap.convertFromImage(self.image, Qt.OrderedAlphaDither);

    def rtti(self):
      return self.imageRTTI

    def hit(self,p):
      ix = p.x()-self.x()
      iy = p.y()-self.y()
      if not self.image.valid( ix , iy ):
        return False
      self.pixel = self.image.pixel( ix, iy )
      return  (qAlpha( self.pixel ) != 0)

    def drawShape(self,p):
      p.drawPixmap( self.x(), self.y(), self.pixmap )


class NodeItem(QCanvasEllipse):
    def __init__(self,canvas):
      QCanvasEllipse.__init__(self,6,6,canvas)
      self.__inList=[]
      self.__outList=[]
      self.setPen(QPen(Qt.black))
      self.setBrush(QBrush(Qt.red))
      self.setZ(128)

    def addInEdge(self,edge):
      self.__inList.append(edge)

    def addOutEdge(self,edge):
      self.__outList.append(edge)

    def moveBy(self,dx,dy):
      QCanvasEllipse.moveBy(self,dx,dy)
      for each_edge in self.__inList:
        each_edge.setToPoint( int(self.x()), int(self.y()) )
      for each_edge in self.__outList:
        each_edge.setFromPoint( int(self.x()), int(self.y()) )

class EdgeItem(QCanvasLine):
    __c=0
    def __init__(self,fromNode, toNode,canvas):
      QCanvasLine.__init__(self,canvas)
      self.__c=self.__c+1
      self.setPen(QPen(Qt.black))
      self.setBrush(QBrush(Qt.red))
      fromNode.addOutEdge(self)
      toNode.addInEdge(self)
      self.setPoints(int(fromNode.x()),int(fromNode.y()), int(toNode.x()), int(toNode.y()))
      self.setZ(127)

    def setFromPoint(self,x,y):
      self.setPoints(x,y,self.endPoint().x(),self.endPoint().y())

    def setToPoint(self,x,y):
      self.setPoints(self.startPoint().x(), self.startPoint().y(),x,y)

    def count(self):
      return self.__c

    def moveBy(self,dx,dy):
      pass

class LinkItem(QCanvasLine):
  def __init__(self,fromPort, toPort,canvas):
    QCanvasLine.__init__(self,canvas)
    self.setPen(QPen(Qt.black))
    self.setBrush(QBrush(Qt.red))
    fromPort.addOutLink(self)
    toPort.addInLink(self)
    self.setPoints(int(fromPort.x()),int(fromPort.y()), int(toPort.x()), int(toPort.y()))
    self.setZ(min(fromPort.z(),toPort.z())-1)
    self.setVisible(True)

  def setFromPoint(self,x,y):
    self.setPoints(x,y,self.endPoint().x(),self.endPoint().y())

  def setToPoint(self,x,y):
    self.setPoints(self.startPoint().x(), self.startPoint().y(),x,y)

  def moveBy(self,dx,dy):
    pass

  def popup(self,canvasView):
    menu=QPopupMenu()
    caption = QLabel( "<font color=darkblue><u><b>Node Menu</b></u></font>",menu )
    caption.setAlignment( Qt.AlignCenter )
    menu.insertItem( caption )
    menu.insertItem("Delete", self.delete)
    return menu

  def delete(self):
    print "delete link"

  def tooltip(self,view,pos):
    r = QRect(pos.x(), pos.y(), pos.x()+10, pos.y()+10)
    s = QString( "link: %d,%d" % (r.center().x(), r.center().y()) )
    QToolTip(view).tip( r, s )

class PortItem(QCanvasEllipse):
  def __init__(self,node,canvas):
    QCanvasEllipse.__init__(self,6,6,canvas)
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
    r = QRect(self.x(), self.y(), self.width(), self.height())
    s = QString( "port: %d,%d" % (r.center().x(), r.center().y()) )
    QToolTip(view).tip( r, s )

class InPortItem(PortItem):
  def __init__(self,node,canvas):
    PortItem.__init__(self,node,canvas)
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
  def __init__(self,node,canvas):
    PortItem.__init__(self,node,canvas)
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

class Cell(QCanvasRectangle):
  def __init__(self,canvas):
    QCanvasRectangle.__init__(self,canvas)
    self.setSize(50,50)

    self.inports=[]
    self.outports=[]

    p=InPortItem(self,canvas)
    p.myMove(0,25)
    self.inports.append(p)
    p=OutPortItem(self,canvas)
    p.myMove(50,25)
    self.outports.append(p)

  def moveBy(self,dx,dy):
    QCanvasRectangle.moveBy(self,dx,dy)
    for p in self.inports:
      p.myMove(dx,dy)
    for p in self.outports:
      p.myMove(dx,dy)

  def show(self):
    QCanvasRectangle.show(self)
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
    r = QRect(self.x(), self.y(), self.width(), self.height())
    s = QString( "node: %d,%d" % (r.center().x(), r.center().y()) )
    QToolTip(view).tip( r, s )

  def browse(self):
    print "browse"

if __name__=='__main__':
  app=QApplication(sys.argv)
  qvbox=QVBox()
  QToolBar(qvbox,"toolbar")
  canvas=QCanvas(800,600)
  canvas.setAdvancePeriod(30)
  m=GraphViewer(canvas,qvbox,"example",0)
  for x,y in ((150,150),(150,250)):
    c=Cell(canvas)
    c.moveBy(x,y)
    c.show()

  qApp.setMainWidget(qvbox)
  qvbox.show()
  app.exec_loop()


