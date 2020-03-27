import pilot
import SALOMERuntime
import loader
import datetime

fname="testFEDyn.xml"
fname2="REtestFEDyn.xml"
SALOMERuntime.RuntimeSALOME.setRuntime()
l=loader.YACSLoader()
r=SALOMERuntime.getSALOMERuntime()
p=r.createProc("prTest1")
td=p.createType("double","double")
ti=p.createType("int","int")
cont=p.createContainer("gg","HPSalome")
cont.setProperty("nb_proc_per_node","1")
#
pg=pilot.PlayGround()
#pg.loadFromKernelCatalog()
pg.setData([("localhost",8)])
#
cont.setSizeOfPool(1)
cont.setProperty("name","localhost")
cont.setProperty("hostname","localhost")
ti=p.createType("int","int")
tsi=p.createSequenceTc("seqint","seqint",ti)
# Level0
n0=r.createScriptNode("","n0")
o0=n0.edAddOutputPort("o0",tsi)
n0.setScript("o0=[ elt for elt in range(8) ]")
p.edAddChild(n0)
n1=r.createForEachLoopDyn("n1",ti)#Dyn
#n1.getInputPort("nbBranches").edInitPy(2)
n10=r.createScriptNode("","n10")
n10.setExecutionMode("remote")
n10.setContainer(cont)
n1.edAddChild(n10)
n10.setScript("""
import time
time.sleep(0.1)
o2=2*i1
""")
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
n2.setScript("o4=[a for a in i3]")
p.edAddChild(n2)
p.edAddCFLink(n1,n2)
p.edAddLink(o2,i3)
#
p.saveSchema(fname)
p=l.load(fname)
p.saveSchema(fname2)
p.init()
ex=pilot.ExecutorSwig()
assert(p.getState()==pilot.READY)
stt = datetime.datetime.now()
ex.RunW(p,0)
print(str(datetime.datetime.now()-stt))
assert(p.getState()==pilot.DONE)
o4=p.getChildByName("n2").getOutputPort("o4")
assert(o4.getPyObj()==[0, 2, 4, 6, 8, 10, 12, 14])
# Ideal ForEachDyn time = 22 s
# ForEach time = 40 s"""