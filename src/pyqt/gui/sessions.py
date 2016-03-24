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

