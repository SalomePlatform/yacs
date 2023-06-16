#!/usr/bin/env python3
# Copyright (C) 2018-2023  CEA/DEN, EDF R&D
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

import unittest
import pilot
import SALOMERuntime
import loader


SALOMERuntime.RuntimeSALOME.setRuntime()
r=SALOMERuntime.getSALOMERuntime()

p=r.createProc("prTest1")
ti=p.createType("int","int")
tdi=p.createSequenceTc("seqint","seqint",ti)

node0=r.createScriptNode("Salome","Func0")
node0.setScript("""o1=[1,4,3,2,0,6,7,8]""")
out0=node0.edAddOutputPort("o1",tdi)
p.edAddChild(node0)
#
node1=r.createForEachLoop("ForEachLoop_pyobj1",ti)
p.edAddChild(node1)
p.edAddCFLink(node0,node1)
p.edAddLink(out0,node1.edGetSeqOfSamplesPort())
node1.edGetNbOfBranchesPort().edInitInt(2)
#
node2=r.createScriptNode("Salome","Func1")
node2.setScript("""o2=i1*i1""")
i1=node2.edAddInputPort("i1",ti)
o2=node2.edAddOutputPort("o2",ti)
node1.edAddChild(node2)
p.edAddLink(node1.edGetSamplePort(),i1)

p.saveSchema("thomas.xml")

ex=pilot.ExecutorSwig()
assert(p.getState()==pilot.READY)
pilot.LoadObserversPluginIfAny(p,ex)
ex.RunW(p,0)
assert(p.getState()==pilot.DONE)
pilot.UnLoadObserversPluginIfAny()


for i in range(100):
    print(30*"*"+" %d"%i)
    l=loader.YACSLoader()
    p=l.load("thomas.xml")
    ex=pilot.ExecutorSwig()
    assert(p.getState()==pilot.READY)
    pilot.LoadObserversPluginIfAny(p,ex)
    ex.RunW(p,0)
    assert(p.getState()==pilot.DONE)
    pilot.UnLoadObserversPluginIfAny()

