import SALOMERuntime

class myalgosync(SALOMERuntime.OptimizerAlgSync):
  def __init__(self):
    SALOMERuntime.OptimizerAlgSync.__init__(self, None)
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

  def initialize(self,input):
    """Optional method called on initialization. Do nothing here"""

  def start(self):
    """Start to fill the pool with samples to evaluate."""
    self.iter=0
    self.pool.pushInSample(4,1.2)
    self.pool.pushInSample(9,3.4)

  def takeDecision(self):
    """ This method is called each time a sample has been evaluated. It can either add
        new samples to evaluate in the pool, do nothing (wait for more samples), or empty
        the pool to finish the evaluation.
    """
    currentId=self.pool.getCurrentId()

    if self.iter==1:
      self.pool.pushInSample(16,5.6)
      self.pool.pushInSample(25,7.8)
      self.pool.pushInSample(36,9.)
      self.pool.pushInSample(49,12.3)
    elif self.iter==4:
      self.pool.pushInSample(64,45.6)
      self.pool.pushInSample(81,78.9)
    else:
      val=self.pool.getCurrentInSample()
      if abs(val.getDoubleValue()-45.6) < 1.e-12:
        self.pool.destroyAll()
    self.iter=self.iter+1

  def finish(self):
    """Optional method called when the algorithm has finished, successfully or not, to
       perform any necessary clean up. Do nothing here"""
