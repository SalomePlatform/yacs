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

# bug revealed by otgui on 8/2/16. Several lock/unlock session. test1.xml is Cogeneration.xml.

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

fname="test2.xml"
import evalyfx
session=evalyfx.YACSEvalSession()
session.launch()
buildScheme(fname)
efx=evalyfx.YACSEvalYFX.BuildFromFile(fname)
#efx.setParallelizeStatus(False)
efx.getParams().setStopASAPAfterErrorStatus(False)
inps=efx.getFreeInputPorts()
assert(len(inps)==1)
outps=efx.getFreeOutputPorts()
inps[0].setSequenceOfValuesToEval([1.,2.,3.,4.,5.,6.])
efx.lockPortsForEvaluation(inps,outps)
rss=efx.giveResources()
rss[0][0].setWantedMachine("localhost")
assert(rss.isInteractive())
a,b=efx.run(session)
assert(efx.getStatusOfRunStr()=='SOME_SAMPLES_FAILED_AND_ALL_OF_THEM_FAILED_DETERMINISTICALLY')
c,d=efx.getResultsInCaseOfFailure()
assert(d==[0,1,2,4,5])# case 3 is not in !
assert(len(c)==1)
c=c[0]
for f,g in zip(c,[0.3333333333333333,0.5,1.0,-1.0,-0.5]):
    assert(abs(f-g)<1e-12)
    pass
st=efx.getErrorDetailsInCaseOfFailure()
assert("NODE = toto.PyScript0" in st)
assert("float division by zero" in st)
