import pilot
import SALOMERuntime
import loader

SALOMERuntime.RuntimeSALOME.setRuntime()
r=SALOMERuntime.getSALOMERuntime()
p=r.createProc("prTest1")
td=p.createType("double","double")
ti=p.createType("int","int")
cont=p.createContainer("gg","HPSalome")
ti=p.createType("int","int")
tsi=p.createSequenceTc("seqint","seqint",ti)
# Level0
n0=r.createScriptNode("","n0")
o0=n0.edAddOutputPort("o0",tsi)
n0.setScript("o0=[ elt for elt in range(6) ]")
p.edAddChild(n0)
n1=r.createForEachLoopDyn("n1",ti)
n1.setWeight(3)
n10=r.createScriptNode("","n10")
n10.setExecutionMode("remote")
n10.setContainer(cont)
n1.edAddChild(n10)
n10.setScript("""
import time
time.sleep(2)
o2=2*i1
""")
n10.setWeight(4.)
i1=n10.edAddInputPort("i1",ti)
o2=n10.edAddOutputPort("o2",ti)
p.edAddChild(n1)
p.edAddLink(o0,n1.edGetSeqOfSamplesPort())
p.edAddLink(n1.edGetSamplePort(),i1)
p.edAddCFLink(n0,n1)
n2=r.createScriptNode("","n2")
n2.setScript("o4=i3")
i3=n2.edAddInputPort("i3",tsi)
o4=n2.edAddOutputPort("o4",tsi)
n2.setScript("o4=i3")
p.edAddChild(n2)
p.edAddCFLink(n1,n2)
p.edAddLink(o2,i3)