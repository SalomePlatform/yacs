# bug revealed by otgui on 8/2/16. Several lock/unlock session. test1.xml is Cogeneration.xml.

def buildScheme(fname):
    import SALOMERuntime
    import loader
    SALOMERuntime.RuntimeSALOME.setRuntime()
    r=SALOMERuntime.getSALOMERuntime()
    p=r.createProc("run")
    cont=p.createContainer("MyWonderfulContainer","Salome")
    td=p.createType("double","double")
    n0=r.createScriptNode("Salome","PyScript0")
    p.edAddChild(n0)
    q=n0.edAddInputPort("q",td)
    e=n0.edAddInputPort("e",td)
    c=n0.edAddInputPort("c",td)
    ep=n0.edAddOutputPort("ep",td)
    n0.setScript("ep=1-(q/((e/((1-0.05)*0.54))+(c/0.8)))")
    n0.setExecutionMode("remote")
    n0.setContainer(cont)
    p.saveSchema(fname)
    pass

fname="test1.xml"
import evalyfx
session=evalyfx.YACSEvalSession()
session.launch()
efx=evalyfx.YACSEvalYFX.BuildFromFile(fname)
efx.setParallelizeStatus(False)
inps=efx.getFreeInputPorts()
outps=efx.getFreeOutputPorts()
for inp in inps:
    inp.setSequenceOfValuesToEval([0.2,0.3])
efx.lockPortsForEvaluation(inps,outps)
rss=efx.giveResources()
rss[0][0].setWantedMachine("localhost")
a,b=efx.run(session) ; assert(a)
efx.unlockAll()
for ii,inp in enumerate(inps):
    inp.setSequenceOfValuesToEval([0.5,0.6,0.7])
efx.lockPortsForEvaluation(inps,outps)
a,b=efx.run(session) ; assert(a)
efx.unlockAll()
for ii,inp in enumerate(inps):
    inp.setSequenceOfValuesToEval([0.5,0.6,0.7])
efx.lockPortsForEvaluation(inps,outps)
a,b=efx.run(session) ; assert(a)
