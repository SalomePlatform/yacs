# Copyright (C) 2015-2023  CEA, EDF
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

import SALOMERuntime

class myalgosync(SALOMERuntime.OptimizerAlgSync):
  def __init__(self):
    SALOMERuntime.OptimizerAlgSync.__init__(self, None)
    r=SALOMERuntime.getSALOMERuntime()
    self.tin=r.getTypeCode("double")
    self.tout=r.getTypeCode("int")
    self.tAlgoInit=r.getTypeCode("pyobj")
    self.tAlgoResult=r.getTypeCode("pyobj")

  def setPool(self,pool):
    print("Algo setPool")

  def getTCForIn(self):
    return self.tin

  def getTCForOut(self):
    return self.tout

  def getTCForAlgoInit(self):
    return self.tAlgoInit

  def getTCForAlgoResult(self):
    return self.tAlgoResult

  def initialize(self,input):
    print ("Algo initialize")

  def start(self):
    print ("Algo start")

  def takeDecision(self):
    print ("Algo takeDecision")

  def finish(self):
    print ("Algo finish")

  def getAlgoResult(self):
    print("Algo getAlgoResult : on charge un objet complet obtenu en pickle 9.2 avant tuyau")
    import pickle
    import numpy as np
    resu = np.array(range(1),dtype=np.int32)
    ob=pickle.dumps(resu)
    #assert(bytes([0]) in ob) # test is here presence of 0 in the pickelization
    return ob
