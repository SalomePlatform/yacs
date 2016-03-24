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

def buildScheme(fname):
    import SALOMERuntime
    import loader
    SALOMERuntime.RuntimeSALOME.setRuntime()
    r=SALOMERuntime.getSALOMERuntime()
    p=r.createProc("run")
    td=p.createType("double","double")
    #
    cont=p.createContainer("zeCont","Salome")
    #
    n0=r.createFuncNode("Salome","node0")
    p.edAddChild(n0)
    n0.setFname("func0")
    n0.setContainer(cont)
    n0.setScript("""def func0(a,b):
  return a*b
""")
    n0.setExecutionMode("remote")
    a=n0.edAddInputPort("a",td)
    b=n0.edAddInputPort("b",td)  ; b.edInitPy(1.3)
    c=n0.edAddOutputPort("c",td)
    #
    n1=r.createFuncNode("Salome","node1")
    p.edAddChild(n1)
    n1.setFname("func1")
    n1.setContainer(cont)
    n1.setScript("""def func1(a,b):
  return a+b,3*(a+b)
""")
    n1.setExecutionMode("remote")
    d=n1.edAddInputPort("d",td)
    e=n1.edAddInputPort("e",td)  ; e.edInitPy(2.5) # agy : useless but for test
    f=n1.edAddOutputPort("f",td)
    g=n1.edAddOutputPort("g",td)
    #
    p.edAddCFLink(n0,n1)
    p.edAddLink(c,d)
    #
    p.saveSchema(fname)

fileName="test0.xml"
import evalyfx
session=evalyfx.YACSEvalSession()
session.launch()
buildScheme(fileName)
efx=evalyfx.YACSEvalYFX.BuildFromFile(fileName)
inps=efx.getFreeInputPorts()
#
inps[0].setDefaultValue(3.4)
#
inps[0].setDefaultValue(None)
inps[2].setDefaultValue(2.7)
inps[2].setDefaultValue(None)
#
outps=efx.getFreeOutputPorts()
# prepare for execution
inps[0].setDefaultValue(1.1)
inps[1].setSequenceOfValuesToEval([10.1,10.2,10.3])
a=inps[2].hasSequenceOfValuesToEval()
inps[2].setSequenceOfValuesToEval([20.1,20.2,30.3,40.4])
a=inps[2].hasSequenceOfValuesToEval()
inps[2].setSequenceOfValuesToEval([20.1,20.2,30.3])
efx.lockPortsForEvaluation([inps[1],inps[2]],[outps[0],outps[2]])
#
"""
g=efx.getUndergroundGeneratedGraph()
g.saveSchema("toto.xml")
rss=efx.giveResources()
rss[0][0].setWantedMachine("localhost")
a,b=efx.run(session)
assert(a)
assert(b==4)
assert(efx.getResults()==[[11.110000000000001, 11.22, 11.330000000000002], [93.63, 94.26, 124.89000000000001]])
"""
#import loader
#import pilot
#l=loader.YACSLoader()
#p=l.load("/home/H87074/aaaaaaa.xml")
#ex=pilot.ExecutorSwig()
#ex.RunW(efx.getUndergroundGeneratedGraph())
#
"""
import pilot
import SALOMERuntime
import loader
import os,sys
zePath=os.path.join(os.environ["KERNEL_ROOT_DIR"],"bin","salome","appliskel")
sys.path.append(zePath)
import salome_test_session
port=salome_test_session.startSession(shutdownAtExit=False)
omniorb_cfg=os.environ["OMNIORB_CONFIG"]
SALOMERuntime.RuntimeSALOME.setRuntime()
r=SALOMERuntime.getSALOMERuntime()
l=loader.YACSLoader()
p=l.load("/home/H87074/aaaaaaa.xml")
ex=pilot.ExecutorSwig()
ex.RunW(p)
salome_test_session.terminateSession(port)
"""
