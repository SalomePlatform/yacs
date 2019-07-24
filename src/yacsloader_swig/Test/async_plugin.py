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
    ob=pickle.dumps(resu,protocol=0)
    assert(bytes([0]) in ob) # test is here presence of 0 in the pickelization
    return ob
