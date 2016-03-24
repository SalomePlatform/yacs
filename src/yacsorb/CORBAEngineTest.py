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

import time

import salome
salome.salome_init()

import YACS_ORB
comp = salome.lcc.FindOrLoadComponent( "YACSContainer","YACS" )
yacsgen = comp._narrow(YACS_ORB.YACS_Gen)

# -----------------------------------------------------------------------------
# --- schema OK

procEx = yacsgen.LoadProc("samples/legendre7.xml")
procEx.setExecMode(YACS_ORB.CONTINUE)
procEx.Run()

# --- wait until executor is paused, finished or stopped

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

print procEx.getOutPortValue(dico["poly_7"],"Pn")
print procEx.getInPortValue(dico["poly_7"],"x")
print procEx.getInPortValue(dico["poly_7"],"notAPort")
print procEx.getInPortValue(dico["Legendre.loopIter"],"nsteps")

# -----------------------------------------------------------------------------
# --- schema with errors (echoSrv must be launched)

procEx = yacsgen.LoadProc("samples/aschema.xml")
procEx.setExecMode(YACS_ORB.CONTINUE)
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

print procEx.getErrorDetails(dico["c1"])
print procEx.getErrorDetails(dico["node13"])

# -----------------------------------------------------------------------------
# --- schema with errors

procEx = yacsgen.LoadProc("samples/triangle5error.xml")
procEx.setExecMode(YACS_ORB.CONTINUE)
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
procEx.setExecMode(YACS_ORB.STOPBEFORENODES)
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
procEx.setExecMode(YACS_ORB.CONTINUE)
procEx.RunFromState("partialExec.xml")

isRunning = 1
while isRunning:
    time.sleep(0.5)
    state = procEx.getExecutorState()
    isRunning = (state < 304)
    print "executorState: ", state
    pass

procEx.saveState("finishExec.xml")

