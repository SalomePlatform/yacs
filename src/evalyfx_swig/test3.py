# Copyright (C) 2015-2016  CEA/DEN, EDF R&D
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
