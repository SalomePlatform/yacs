# Copyright (C) 2006-2013  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
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

  def setPool(self,pool):
    """Must be implemented to set the pool"""
    self.pool=pool

  def getTCForIn(self):
    """returns typecode of type expected as Input"""
    return self.tin

  def getTCForOut(self):
    """returns typecode of type expected as Output"""
    return self.tout

  def startToTakeDecision(self):
    """This method is called only once to launch the algorithm. It must first fill the
       pool with samples to evaluate and call self.signalMasterAndWait() to block until a
       sample has been evaluated. When returning from this method, it MUST check for an
       eventual termination request (with the method self.isTerminationRequested()). If
       the termination is requested, the method must perform any necessary cleanup and
       return as soon as possible. Otherwise it can either add new samples to evaluate in
       the pool, do nothing (wait for more samples), or empty the pool and return to
       finish the evaluation.
    """
    val=1.2
    for iter in xrange(5):
      #push a sample in the input of the slave node
      self.pool.pushInSample(iter,val)
      #wait until next sample is ready
      self.signalMasterAndWait()
      #check error notification
      if self.isTerminationRequested():
        self.pool.destroyAll()
        return

      #get a sample from the output of the slave node
      currentId=self.pool.getCurrentId()
      v=self.pool.getCurrentOutSample()
      val=val+v.getIntValue()

    #in the end destroy the pool content
    self.pool.destroyAll()
