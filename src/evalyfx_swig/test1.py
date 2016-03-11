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
buildScheme(fname)
efx=evalyfx.YACSEvalYFX.BuildFromFile(fname)
efx.setParallelizeStatus(False)
inps=efx.getFreeInputPorts()
outps=efx.getFreeOutputPorts()
for inp in inps:
    inp.setSequenceOfValuesToEval([0.2,0.3])
efx.lockPortsForEvaluation(inps,outps)
rss=efx.giveResources()
rss[0][0].setWantedMachine("localhost")
assert(rss.isInteractive())
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
