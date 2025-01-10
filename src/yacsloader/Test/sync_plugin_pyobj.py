# Copyright (C) 2015-2025  CEA, EDF
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
import pickle

class myalgosync(SALOMERuntime.OptimizerAlgSync):
  def __init__(self):
    SALOMERuntime.OptimizerAlgSync.__init__(self, None)
    r=SALOMERuntime.getSALOMERuntime()
    self.tin=r.getTypeCode("pyobj")
    self.tout=r.getTypeCode("pyobj")
    self.tAlgoInit=r.getTypeCode("pyobj")
    self.tAlgoResult=r.getTypeCode("pyobj")

  def setPool(self,pool):
    """Must be implemented to set the pool"""
    self.pool=pool

  def getTCForIn(self):
    """return typecode of type expected as Input of the internal node """
    return self.tin

  def getTCForOut(self):
    """return typecode of type expected as Output of the internal node"""
    return self.tout

  def getTCForAlgoInit(self):
    """return typecode of type expected as input for initialize """
    return self.tAlgoInit

  def getTCForAlgoResult(self):
    """return typecode of type expected as output of the algorithm """
    return self.tAlgoResult

  def initialize(self,input):
    """Optional method called on initialization.
       The type of "input" is returned by "getTCForAlgoInit"
    """
    self.data=input.getPyObj()
    self.result = 0

  def start(self):
    """Start to fill the pool with samples to evaluate."""
    r=SALOMERuntime.getSALOMERuntime()
    self.iter=0
    for i in self.data:
      # "range" is for a python object which is not of basic type
      self.pool.pushInSample(self.iter, r.createAnyPyObject(range(i)))
      self.iter=self.iter+1

  def takeDecision(self):
    """ This method is called each time a sample has been evaluated. It can
        either add new samples to evaluate in the pool, do nothing (wait for
        more samples), or empty the pool to finish the evaluation.
    """
    currentId=self.pool.getCurrentId()
    in_value = self.pool.getCurrentInSample().getPyObj()
    result = self.pool.getCurrentOutSample().getPyObj()
    self.result = self.result + len(result)

  def finish(self):
    """Optional method called when the algorithm has finished, successfully
       or not, to perform any necessary clean up."""
    self.pool.destroyAll()
    self.result = 0 # the result object is destroyed

  def getAlgoResult(self):
    """return the result of the algorithm.
       The object returned is of type indicated by getTCForAlgoResult.
    """
    r=SALOMERuntime.getSALOMERuntime()
    # Force the creation of a python object into the result port.
    # If a basic python type is used, it will be converted to a basic c++ type
    # (int, double, std::string) and this is not what the result port expects as
    # it is declared of type pyobj.
    self.result = r.createAnyPyObject(self.result)
    # do not return a local variable created with "createAnyPyObject" (crash)
    return self.result
