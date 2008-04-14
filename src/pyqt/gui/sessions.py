
import sys
import omniORB
from omniORB import CORBA
import CosNaming

import Item
import CONNECTOR

class Sessions(Item.Item):
  def __init__(self,port):
    Item.Item.__init__(self)
    self.label="Sessions"
    self.orb=CORBA.ORB_init(sys.argv, CORBA.ORB_ID)
    self.port=port

  def getChildren(self):
    sublist=[]
    port=self.port
    while 1:
      port=port+1
      try:
        session=Session(self,self.orb,port)
        sublist.append(session)
      except:
        #traceback.print_exc()
        break
    return sublist

  def selected(self):
    if not self.emitting:
      self.emitting=1
      CONNECTOR.Emit(self,"selected",self)
      self.emitting=0

  def getIconName(self):
    return "folder"

  def isExpandable(self):
    return True

class Session(Item.Item):
  def __init__(self,root,orb,port):
    Item.Item.__init__(self)
    self.root=root
    self.port=port
    self.addr="corbaname::localhost:%d/NameService" % port
    obj=orb.string_to_object(self.addr)
    context=obj._narrow(CosNaming.NamingContext)
    self.label="Session on port: %d" % port

  def selected(self):
    if not self.emitting:
      self.emitting=1
      CONNECTOR.Emit(self.root,"selected",self)
      self.emitting=0

