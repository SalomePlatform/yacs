#!/usr/bin/env python3
# Copyright (C) 2023-2026  CEA, EDF
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
import tempfile
import os

from salome.yacs import pilot
from salome.yacs import SALOMERuntime
from salome.yacs import loader
from salome.kernel import salome

class TestYacsProxy(unittest.TestCase):
  def test0(self):
    """
    [EDF27816] : test to check
    """
    salome.salome_init()
    with tempfile.TemporaryDirectory() as tmpdirname:
      print(tmpdirname)
      salome.cm.SetBigObjOnDiskDirectory(str(tmpdirname))
      salome.cm.SetBigObjOnDiskThreshold(1)
      ####
      SALOMERuntime.RuntimeSALOME.setRuntime()
      r=SALOMERuntime.getSALOMERuntime()
      p=r.createProc("StressTest")
      ti=p.createType("int","int")
      td=p.createType("double","double")
      tdd=p.createSequenceTc("seqdouble","seqdouble",td)
      tddd=p.createSequenceTc("seqseqdouble","seqseqdouble",tdd)
      tdddd=p.createSequenceTc("seqseqseqdouble","seqseqseqdouble",tddd)
      pyobj=p.createInterfaceTc("python:obj:1.0","pyobj",[])
      seqpyobj=p.createSequenceTc("list[pyobj]","list[pyobj]",pyobj)
      seqseqpyobj=p.createSequenceTc("list[list[pyobj]]","list[list[pyobj]]",seqpyobj)
      seqseqseqpyobj=p.createSequenceTc("list[list[list[pyobj]]]","list[list[list[pyobj]]]",seqseqpyobj)
      cont=p.createContainer("gg","Salome")
      cont.setProperty("name","localhost")
      cont.setProperty("hostname","localhost")

      ######## Level 0
      startNode = r.createScriptNode("Salome","start")
      startNode.setExecutionMode("local")
      startNode.setContainer(None)
      startNode.setSqueezeStatus(True)
      startNode.setScript("""o2 = [[[ {k:2*k for k in range(23,55)}  ]]]""")
      po2 = startNode.edAddOutputPort("o2",seqseqseqpyobj)
      p.edAddChild(startNode)
      #
      fe = r.createForEachLoopDyn("fe",seqseqpyobj)
      p.edAddChild(fe)
      p.edAddCFLink(startNode,fe)
      p.edAddLink(po2,fe.edGetSeqOfSamplesPort())
      #
      gather2Node = r.createScriptNode("Salome","gather2")
      p.edAddChild(gather2Node)
      gather2Node.setExecutionMode("local")
      gather2Node.setContainer(None)
      gather2Node.setSqueezeStatus(True)
      pi5 = gather2Node.edAddInputPort("i5",seqpyobj)
      po5 = gather2Node.edAddOutputPort("o5",seqpyobj)
      gather2Node.setScript("""
from glob import glob
from salome.kernel import KernelBasis
import os
_,zeDir = KernelBasis.GetBigObjOnDiskProtocolAndDirectory()
if len( glob( os.path.join( zeDir, "*.pckl" ) ) ) != 1:
  raise RuntimeError("Fail !")
print("gather2")
o5 = i5""")
      p.edAddCFLink(fe,gather2Node)
      ####### Level 1
      n1b = r.createBloc("n1b")
      fe.edSetNode(n1b)
      #
      gather1Node = r.createScriptNode("Salome","gather1")
      gather1Node.setExecutionMode("local")
      gather1Node.setContainer(None)
      gather1Node.setSqueezeStatus(True)
      pi6 = gather1Node.edAddInputPort("i6",seqpyobj)
      po6 = gather1Node.edAddOutputPort("o6",seqpyobj)
      gather1Node.setScript("""print("gather1")
print(i6)
o6 = i6""")
      n1b.edAddChild(gather1Node)
      fe1 = r.createForEachLoopDyn("fe1",seqpyobj)
      n1b.edAddChild(fe1)
      n1b.edAddCFLink(fe1,gather1Node)
      ####### Level2
      n2b = r.createBloc("n2b")
      fe1.edSetNode(n2b)
      fe2 = r.createForEachLoopDyn("fe2",pyobj)
      n2b.edAddChild(fe2)
      #
      gather0Node = r.createScriptNode("Salome","gather0")
      gather0Node.setExecutionMode("local")
      gather0Node.setContainer(None)
      gather0Node.setSqueezeStatus(True)
      pi7 = gather0Node.edAddInputPort("i7",seqpyobj)
      po7 = gather0Node.edAddOutputPort("o7",seqpyobj)
      gather0Node.setScript("""
print("gather0")
print(i7)
o7 = i7""")
      n2b.edAddChild(gather0Node)
      n2b.edAddCFLink(fe2,gather0Node)
      heatNode = r.createScriptNode("Salome","HeatMarcelNode")
      heatNode.setExecutionMode("remote")
      heatNode.setContainer(cont)
      heatNode.setSqueezeStatus(True)
      heatNode.setScript("""o3 = list(range(100,200))""")
      pi3 = heatNode.edAddInputPort("i3",pyobj)
      po3 = heatNode.edAddOutputPort("o3",pyobj)
      fe2.edSetNode(heatNode)
      fe2.edAddLink(fe2.edGetSamplePort(),pi3)
      # connection part
      p.edAddLink( fe1.edGetSamplePort(), fe2.edGetSeqOfSamplesPort() )
      p.edAddLink( fe.edGetSamplePort(), fe1.edGetSeqOfSamplesPort() )
      p.edAddLink( po6, pi5 )
      p.edAddLink( po7, pi6 )
      p.edAddLink( po3, pi7 )

      pp = p
      fname = "stressTest3.xml"
      #pp.saveSchema(fname)
      ####
      l=loader.YACSLoader()
      #p=l.load(fname)
      import datetime
      st = datetime.datetime.now()
      ex=pilot.ExecutorSwig()
      ex.setMaxNbOfThreads(1000)
      ex.RunW(pp,0)
      salome.cm.ShutdownContainers()
      print("End of computation {}".format( str(datetime.datetime.now()-st) ) )
      self.assertTrue( pp.getState() == pilot.DONE )
      from glob import glob
      self.assertEqual(len( glob(os.path.join(str(tmpdirname),"*.pckl") ) ), 0 )
  
if __name__ == '__main__':
  with tempfile.TemporaryDirectory() as dir_test:
    file_test = os.path.join(dir_test,"UnitTestsResult")
    with open(file_test, 'a') as f:
        f.write("  --- TEST src/yacsloader: testYacsProxy.py\n")
        suite = unittest.makeSuite(TestYacsProxy)
        result=unittest.TextTestRunner(f, descriptions=1, verbosity=1).run(suite)
        if not result.wasSuccessful():
           raise RuntimeError("Test failed !")
        
