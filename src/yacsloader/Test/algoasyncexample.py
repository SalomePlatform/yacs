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

import SALOMERuntime

class myalgoasync(SALOMERuntime.OptimizerAlgASync):
  def __init__(self):
    SALOMERuntime.OptimizerAlgASync.__init__(self, None)
    r=SALOMERuntime.getSALOMERuntime()
    self.tin=r.getTypeCode("double")
    self.tout=r.getTypeCode("int")
    self.tAlgoInit=r.getTypeCode("int")
    self.tAlgoResult=r.getTypeCode("int")

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
    print "Algo initialize, input = ", input.getIntValue()

  def startToTakeDecision(self):
    """This method is called only once to launch the algorithm. It must
       first fill the pool with samples to evaluate and call
       self.signalMasterAndWait() to block until a sample has been
       evaluated. When returning from this method, it MUST check for an
       eventual termination request (with the method
       self.isTerminationRequested()). If the termination is requested, the
       method must perform any necessary cleanup and return as soon as
       possible. Otherwise it can either add new samples to evaluate in the
       pool, do nothing (wait for more samples), or empty the pool and
       return to finish the evaluation.
    """
    print "startToTakeDecision"
    # fill the pool with samples
    iter=0
    self.pool.pushInSample(0, 0.5)
    
    # 
    self.signalMasterAndWait()
    while not self.isTerminationRequested():
      currentId=self.pool.getCurrentId()
      valIn = self.pool.getCurrentInSample().getDoubleValue()
      valOut = self.pool.getCurrentOutSample().getIntValue()
      print "Compute currentId=%s, valIn=%s, valOut=%s" % (currentId, valIn, valOut)
      iter=iter+1
      
      if iter < 3:
        nextSample = valIn + 1
        self.pool.pushInSample(iter, nextSample)
        
      self.signalMasterAndWait()

  def finish(self):
    """Optional method called when the algorithm has finished, successfully
       or not, to perform any necessary clean up."""
    print "Algo finish"
    self.pool.destroyAll()

  def getAlgoResult(self):
    """return the result of the algorithm.
       The object returned is of type indicated by getTCForAlgoResult.
    """
    return 42


