# Copyright (C) 2006-2015  CEA/DEN, EDF R&D
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

import pilot
import SALOMERuntime
import loader
import unittest

class StdAloneYacsLoaderTest1(unittest.TestCase):

  def setUp(self):
    SALOMERuntime.RuntimeSALOME_setRuntime()
    self.r = pilot.getRuntime()
    self.l = loader.YACSLoader()# self.l.load("foreachImbr_tmp.xml")
    pass

  def test1(self):
    """tests imbrication of foreach loop."""
    SALOMERuntime.RuntimeSALOME_setRuntime()
    l=loader.YACSLoader()
    ex=pilot.ExecutorSwig()
    p=self.r.createProc("pr")
    td=p.createType("double","double")
    td2=p.createSequenceTc("seqdbl","seqdbl",td)
    td3=p.createSequenceTc("seqdblvec","seqdblvec",td2)
    td4=p.createSequenceTc("seqseqdblvec","seqseqdblvec",td3)
    node1=self.r.createScriptNode("","node1")
    node1.setScript("o1=[([1,1],[2,2,2]),([10],[11,11],[12,12,12]),([20],[21,21],[22,22,22],[23,23,23,23])]")
    o1=node1.edAddOutputPort("o1",td4)
    p.edAddChild(node1)
    node2=self.r.createForEachLoop("node2",td3)
    p.edAddChild(node2)
    p.edAddCFLink(node1,node2)
    p.edAddLink(o1,node2.edGetSeqOfSamplesPort())
    node2.edGetNbOfBranchesPort().edInitInt(2)
    #
    node20=self.r.createBloc("node20")
    node2.edAddChild(node20)
    node200=self.r.createForEachLoop("node200",td2)
    node20.edAddChild(node200)
    node200.edGetNbOfBranchesPort().edInitInt(2)
    p.edAddLink(node2.edGetSamplePort(),node200.edGetSeqOfSamplesPort())
    node2000=self.r.createForEachLoop("node2000",td)
    node2000.edGetNbOfBranchesPort().edInitInt(2)
    node200.edAddChild(node2000)
    p.edAddLink(node200.edGetSamplePort(),node2000.edGetSeqOfSamplesPort())
    node20000=self.r.createScriptNode("","node20000")
    node2000.edAddChild(node20000)
    i1=node20000.edAddInputPort("i1",td)
    o2=node20000.edAddOutputPort("o2",td)
    node20000.setScript("o2=i1+2")
    p.edAddLink(node2000.edGetSamplePort(),i1)
    #
    node3=self.r.createScriptNode("","node3")
    node3.setScript("o3=i2")
    p.edAddChild(node3)
    i2=node3.edAddInputPort("i2",td4)
    o3=node3.edAddOutputPort("o3",td4)
    p.edAddCFLink(node2,node3)
    p.edAddLink(o2,i2)
    ex = pilot.ExecutorSwig()
    self.assertEqual(p.getState(),pilot.READY)
    ex.RunW(p,0)
    self.assertEqual(p.getState(),pilot.DONE)
    zeResu=node3.getOutputPort("o3").get()
    self.assertEqual(zeResu,[[[3.,3.],[4.,4.,4.]],[[12.],[13.,13.],[14.,14.,14.]],[[22.],[23.,23.],[24.,24.,24.],[25.,25.,25.,25.]]])
    p.saveSchema("foreachImbrBuildFS.xml")
    pass

  def test2(self):
    """ Non regression test. When input/output declared as pyobj hiding a string type to go to or from a ForEachLoop it previous lead
    to an error.
    """
    fname="BugPyObjStrInYacs.xml"
    p=self.r.createProc("pr")
    tc0=p.createInterfaceTc("python:obj:1.0","pyobj",[])
    tc1=p.createSequenceTc("list[pyobj]","list[pyobj]",tc0)
    #
    node0=self.r.createScriptNode("Salome","node0")
    node0.setScript("o1=[\"a\",\"bc\"]")
    o1=node0.edAddOutputPort("o1",tc1)
    p.edAddChild(node0)
    #
    node1=self.r.createForEachLoop("node1",tc0)
    p.edAddChild(node1)
    p.edAddCFLink(node0,node1)
    node1.edGetNbOfBranchesPort().edInitInt(1)
    p.edAddLink(o1,node1.edGetSeqOfSamplesPort())
    #
    node10=self.r.createScriptNode("Salome","node10")
    node10.setScript("o1=3*i1")
    i10_1=node10.edAddInputPort("i1",tc0)
    o10_1=node10.edAddOutputPort("o1",tc0)
    node1.edAddChild(node10)
    p.edAddLink(node1.edGetSamplePort(),i10_1)
    #
    node2=self.r.createScriptNode("Salome","node2")
    node2.setScript("o1=i1")
    i2_1=node2.edAddInputPort("i1",tc1)
    o2_1=node2.edAddOutputPort("o1",tc1)
    p.edAddChild(node2)
    p.edAddCFLink(node1,node2)
    p.edAddLink(o10_1,i2_1)
    ##
    p.saveSchema(fname)
    p=self.l.load(fname)
    ##
    ex = pilot.ExecutorSwig()
    self.assertEqual(p.getState(),pilot.READY)
    ex.RunW(p,0)
    self.assertEqual(p.getState(),pilot.DONE)
    #
    self.assertEqual(p.getChildByName("node2").getOutputPort("o1").get(),['aaa','bcbcbc'])
    pass

  def test3(self):
    """ Non regression test Mantis 23234 CEA1726"""
    fname="test23234.xml"
    p=self.r.createProc("Test23234")
    ti=p.createType("int","int")
    initNode=self.r.createScriptNode("","init")
    initNode_n=initNode.edAddOutputPort("n",ti)
    initNode.setScript("n=10")
    p.edAddChild(initNode)
    #
    endNode=self.r.createScriptNode("","checkResu")
    endNode_n=endNode.edAddInputPort("n",ti)
    endNode_tot=endNode.edAddInputPort("tot",ti)
    endNode_error=endNode.edAddOutputPort("error",ti)
    endNode.setScript("error=tot-n*(n+1)/2")
    p.edAddChild(endNode)
    #
    fl=self.r.createForLoop("ForLoop_sum_1_n")
    p.edAddChild(fl)
    #
    p.edAddCFLink(initNode,fl)
    p.edAddCFLink(fl,endNode)
    #
    summ=self.r.createFuncNode("","sum")
    summ_i=summ.edAddInputPort("i",ti)
    summ_total=summ.edAddOutputPort("total",ti)
    summ.setScript("""n=0
def sum(i):
   global n
   n+=i+1
   return n""")
    summ.setFname("sum")
    fl.edAddChild(summ)
    #
    p.edAddLink(fl.edGetIndexPort(),summ_i)
    p.edAddLink(initNode_n,fl.edGetNbOfTimesInputPort())
    p.edAddLink(initNode_n,endNode_n)
    p.edAddLink(summ_total,endNode_tot)
    #
    p.saveSchema(fname)
    ex=pilot.ExecutorSwig()
    self.assertEqual(p.getState(),pilot.READY)
    ex.RunW(p,0)
    self.assertEqual(p.getState(),pilot.DONE)
    self.assertEqual(endNode_error.getPyObj(),0)
    pass

  def tearDown(self):
    del self.r
    del self.l
    pass

  pass

if __name__ == '__main__':
    unittest.main()
