# CEA/LGLS 2009, Francis KLOSS (OCC)
# ==================================

print "JOB DISTANT: started"
#      --------------------

import pilot
import loader
import SALOMERuntime

print "JOB DISTANT: set parameters"
#      ---------------------------

xml = "graph_user.xml"

print "JOB DISTANT: xml graph = ", xml

print "JOB DISTANT: init"
#      -----------------

state = True

try:
    SALOMERuntime.RuntimeSALOME_setRuntime(True)

    yacsloader = loader.YACSLoader()

    executor = pilot.ExecutorSwig()

except Exception, message:
    print "JOB DISTANT: Exception: in init: ", message
    state = False

print "JOB DISTANT: load graph"
#      -----------------------

if state:
    try:
        graph = yacsloader.load(xml)

    except Exception, message:
        print "JOB DISTANT: Exception: in loading graph: ", message
        state = False

print "JOB DISTANT: run graph"
#      ----------------------

if state:
    try:
        executor.setExecMode(pilot.CONTINUE)

        executor.RunB(graph, 0, True)

    except Exception, message:
        print "JOB DISTANT: Exception: in runing graph: ", message
        state = False

print "JOB DISTANT: status"
#      -------------------

print "JOB DISTANT: status = ", state

print "JOB DISTANT: finished"
#      ---------------------
