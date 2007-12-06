import time

import salome
salome.salome_init()

import YACSGui_ORB
comp = salome.lcc.FindOrLoadComponent( "YACSContainer","YACSGui" )
yacsgen = comp._narrow(YACSGui_ORB.YACSGui_Gen)

# -----------------------------------------------------------------------------
# --- schema OK

procEx = yacsgen.LoadProc("samples/legendre7.xml")
procEx.setExecMode(YACSGui_ORB.CONTINUE)
procEx.Run()

# --- wait until executor is paused, finised or stopped

isRunning = 1
while isRunning:
    time.sleep(0.5)
    state = procEx.getExecutorState()
    isRunning = (state < 304)
    print "executorState: ", state
    pass

procEx.saveState("res.xml")
numids,names = procEx.getIds()

dico = {}
i=0
for name in names:
    dico[name] = numids[i]
    i+=1
    pass

procEx.getOutPortValue(dico["poly_7"],"Pn")
procEx.getInPortValue(dico["poly_7"],"x")
procEx.getInPortValue(dico["poly_7"],"notAPort")
procEx.getInPortValue(dico["Legendre.loopIter"],"nsteps")

# -----------------------------------------------------------------------------
# --- schema with errors (echoSrv must be launched)

procEx = yacsgen.LoadProc("samples/aschema.xml")
procEx.setExecMode(YACSGui_ORB.CONTINUE)
procEx.Run()

# --- wait until executor is paused, finised or stopped

isRunning = 1
while isRunning:
    time.sleep(0.5)
    state = procEx.getExecutorState()
    isRunning = (state < 304)
    print "executorState: ", state
    pass

procEx.saveState("res2.xml")
numids,names = procEx.getIds()

dico = {}
i=0
for name in names:
    dico[name] = numids[i]
    i+=1
    pass

procEx.getErrorDetails(dico["c1"])
procEx.getErrorDetails(dico["node13"])

# -----------------------------------------------------------------------------
# --- schema with errors

procEx = yacsgen.LoadProc("samples/triangle5error.xml")
procEx.setExecMode(YACSGui_ORB.CONTINUE)
procEx.setStopOnError(1,"execError2.xml")
#procEx.unsetStopOnError()
procEx.Run()

isRunning = 1
while isRunning:
    time.sleep(0.5)
    state = procEx.getExecutorState()
    isRunning = (state < 304)
    print "executorState: ", state
    pass

#procEx.resumeCurrentBreakPoint()

procEx.saveState("res3.xml")

# -----------------------------------------------------------------------------
# --- schema with breakpoints

procEx = yacsgen.LoadProc("samples/legendre7.xml")
procEx.setListOfBreakPoints(["Legendre.loopIter.deuxIter.iter2"])
procEx.setExecMode(YACSGui_ORB.STOPBEFORENODES)
procEx.Run()

isRunning = 1
while isRunning:
    time.sleep(0.5)
    state = procEx.getExecutorState()
    isRunning = (state < 304)
    print "executorState: ", state
    pass

procEx.saveState("partialExec.xml")

procEx = yacsgen.LoadProc("samples/legendre7.xml")
procEx.setExecMode(YACSGui_ORB.CONTINUE)
procEx.RunFromState("partialExec.xml")

isRunning = 1
while isRunning:
    time.sleep(0.5)
    state = procEx.getExecutorState()
    isRunning = (state < 304)
    print "executorState: ", state
    pass

procEx.saveState("finishExec.xml")

