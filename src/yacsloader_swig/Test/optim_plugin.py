import SALOMERuntime

class myalgosync(SALOMERuntime.OptimizerAlgSync):
  def __init__(self):
    SALOMERuntime.OptimizerAlgSync.__init__(self, None)
    r=SALOMERuntime.getSALOMERuntime()
    self.tin=r.getTypeCode("int")
    self.tout=r.getTypeCode("int")
    self.tAlgoInit=r.getTypeCode("int")
    self.tAlgoResult=r.getTypeCode("int")

  def setPool(self,pool):
    """Must be implemented to set the pool"""
    self.pool=pool

  def getTCForIn(self):
    """return typecode of type expected as Input the internal node """
    return self.tin

  def getTCForOut(self):
    """return typecode of type expected as Output the internal node"""
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

  def start(self):
    """Start to fill the pool with samples to evaluate."""
    print "Algo start "
    self.iter=0
    # pushInSample(id, value)
    self.pool.pushInSample(self.iter, 1)

  def takeDecision(self):
    """ This method is called each time a sample has been evaluated. It can
        either add new samples to evaluate in the pool, do nothing (wait for
        more samples), or empty the pool to finish the evaluation.
    """
    currentId=self.pool.getCurrentId()
    valIn = self.pool.getCurrentInSample().getIntValue()
    valOut = self.pool.getCurrentOutSample().getIntValue()
    print "Algo takeDecision currentId=%s, valIn=%s, valOut=%s" % (currentId, valIn, valOut)

    self.iter=self.iter+1
    if self.iter < 3:
      # continue
      nextSample = valIn + 1
      self.pool.pushInSample(self.iter, nextSample)

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


