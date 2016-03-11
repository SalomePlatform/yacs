# first test using cluster

import os,getpass

CatalogResources="""<?xml version="1.0"?>
<resources>
  <machine name="localhost" hostname="dsp0698184" type="single_machine" appliPath="" batchQueue="" userCommands="" protocol="ssh" iprotocol="ssh" workingDirectory="" canLaunchBatchJobs="false" canRunContainers="true" batch="none" mpi="no mpi" userName="" OS="" memInMB="0" CPUFreqMHz="0" nbOfNodes="1" nbOfProcPerNode="4"/>
  <machine name="athos" hostname="athos" type="cluster" appliPath="/home/H87074/ATHOS_V771/appli_V7_7_1" batchQueue="" userCommands="" protocol="ssh" iprotocol="ssh" workingDirectory="" canLaunchBatchJobs="true" canRunContainers="false" batch="slurm" mpi="no mpi" userName="" OS="" memInMB="0" CPUFreqMHz="0" nbOfNodes="250" nbOfProcPerNode="28"/>
</resources>"""

def buildScheme(fname):
    import SALOMERuntime
    import loader
    SALOMERuntime.RuntimeSALOME.setRuntime()
    r=SALOMERuntime.getSALOMERuntime()
    p0=r.createProc("run")
    #
    p=r.createBloc("toto")
    p0.edAddChild(p)
    #
    cont=p0.createContainer("MyWonderfulContainer","Salome")
    td=p0.createType("double","double")
    n0=r.createScriptNode("Salome","PyScript0")
    p.edAddChild(n0)
    q=n0.edAddInputPort("q",td)
    ep=n0.edAddOutputPort("ep",td)
    n0.setScript("ep=1./(4.-q)") # <- force division by 0
    n0.setExecutionMode("remote")
    n0.setContainer(cont)
    p0.saveSchema(fname)
    pass

fname="test3.xml"
import evalyfx
session=evalyfx.YACSEvalSession()
session.launch()
buildScheme(fname)
efx=evalyfx.YACSEvalYFX.BuildFromFile(fname)
efx.getParams().setStopASAPAfterErrorStatus(False)
inps=efx.getFreeInputPorts()
assert(len(inps)==1)
outps=efx.getFreeOutputPorts()
inps[0].setSequenceOfValuesToEval([-7.,2.,3.,5.,6.])
efx.lockPortsForEvaluation(inps,outps)
#
rss=efx.giveResources()
rss[0][0].setWantedMachine("athos")
cp=rss.getAddParamsForCluster() ; cp.setRemoteWorkingDir(os.path.join("/scratch",getpass.getuser(),"TMP3")) ; cp.setLocalWorkingDir(os.path.join(os.path.expanduser("~"),"TMP52"))
cp.setWCKey("P11U50:CARBONES") ; cp.setNbProcs(5) ; cp.setMaxDuration("00:05")
assert(not rss.isInteractive())
a,b=efx.run(session)
print("************",a,b)
print efx.getResults()
