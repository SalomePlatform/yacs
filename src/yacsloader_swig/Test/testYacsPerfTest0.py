#!/usr/bin/env python3
# Copyright (C) 2023-2025  CEA, EDF
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

class TestYacsPerf0(unittest.TestCase):
    def test0(self):
        """
        [EDF28531] : Perf to check when number of threads in charge scheduler side is lower than number of // tasks
        """
        NB_OF_PARALLEL_NODES = 100
        NB_OF_PARALLEL_THREADS = 10
        salome.salome_init()
        SALOMERuntime.RuntimeSALOME.setRuntime()
        r=SALOMERuntime.getSALOMERuntime()
        p=r.createProc("PerfTest0")
        p.setProperty("executor","workloadmanager") # important line here to avoid that gg container treat several tasks in //.
        ti=p.createType("int","int")
        td=p.createType("double","double")
        tdd=p.createSequenceTc("seqdouble","seqdouble",td)
        tddd=p.createSequenceTc("seqseqdouble","seqseqdouble",tdd)
        tdddd=p.createSequenceTc("seqseqseqdouble","seqseqseqdouble",tddd)
        pyobj=p.createInterfaceTc("python:obj:1.0","pyobj",[])
        seqpyobj=p.createSequenceTc("list[pyobj]","list[pyobj]",pyobj)
        cont=p.createContainer("gg","Salome")
        cont.setProperty("nb_parallel_procs","1")
        cont.setAttachOnCloningStatus(True)
        cont.setProperty("attached_on_cloning","1")
        cont.setProperty("type","multi")
        cont.setProperty("container_name","gg")
        ######## Level 0
        startNode = r.createScriptNode("Salome","start")
        startNode.setExecutionMode("local")
        startNode.setScript("""o2 = list(range({}))""".format(NB_OF_PARALLEL_NODES))
        po2 = startNode.edAddOutputPort("o2",seqpyobj)
        p.edAddChild(startNode)
        #
        fe = r.createForEachLoopDyn("fe",pyobj)
        p.edAddChild(fe)
        p.edAddCFLink(startNode,fe)
        p.edAddLink(po2,fe.edGetSeqOfSamplesPort())
        internalNode = r.createScriptNode("Salome","internalNode")
        internalNode.setExecutionMode("remote")
        internalNode.setContainer(cont)
        internalNode.setScript("""
ret = 3*ppp
""")
        fe.edSetNode(internalNode)
        ix = internalNode.edAddInputPort("ppp",pyobj)
        oret = internalNode.edAddOutputPort("ret",pyobj)
        p.edAddLink( fe.edGetSamplePort(), ix )
        #
        endNode = r.createScriptNode("Salome","end")
        endNode.setExecutionMode("local")
        endNode.setContainer(None)
        ozeret = endNode.edAddOutputPort("ozeret",seqpyobj)
        izeret = endNode.edAddInputPort("izeret",seqpyobj)
        endNode.setScript("""ozeret = izeret""")
        p.edAddChild(endNode)
        p.edAddCFLink(fe,endNode)
        p.edAddLink( oret, izeret )
        if False:
            fname = "PerfTest0.xml"
            p.saveSchema(fname)
            
            from salome.yacs import loader
            l=loader.YACSLoader()
            p=l.load(fname)
        print("Start computation")
        import datetime
        st = datetime.datetime.now()
        ex=pilot.ExecutorSwig()
        ex.setMaxNbOfThreads(NB_OF_PARALLEL_THREADS)
        ex.RunW(p,0)
        self.assertEqual(p.getState(),pilot.DONE)
        salome.cm.ShutdownContainers()
        print("End of computation {}".format( str(datetime.datetime.now()-st) ) )
        if p.getChildByName("end").getOutputPort("ozeret").getPyObj() != [3*i for i in range(NB_OF_PARALLEL_NODES)]:
            raise RuntimeError("Ooops")
        
    def test1(self):
        """
        [EDF28562] : test of exclusion output port of squeezeMemory mecanism
        """
        salome.salome_init()
        from salome.kernel import KernelBasis
        KernelBasis.SetVerbosityActivated(False)
        SALOMERuntime.RuntimeSALOME.setRuntime()
        r=SALOMERuntime.getSALOMERuntime()
        p=r.createProc("Squeeze")
        pyobj=p.createInterfaceTc("python:obj:1.0","pyobj",[])
        cont=p.createContainer("gg","Salome")
        cont.setProperty("nb_parallel_procs","1")
        cont.setAttachOnCloningStatus(True)
        cont.setProperty("attached_on_cloning","1")
        cont.setProperty("type","multi")
        cont.setProperty("container_name","gilles")
        startNode = r.createScriptNode("Salome","startNode")
        startNode.setExecutionMode("remote")
        startNode.setContainer(cont)
        startNode.setSqueezeStatus(True)
        startNode.setScript("""
o1,o2,o3,o4 = 31,32,33,34
""")
        o1 = startNode.edAddOutputPort("o1",pyobj)
        o2 = startNode.edAddOutputPort("o2",pyobj)
        o3 = startNode.edAddOutputPort("o3",pyobj)
        o4 = startNode.edAddOutputPort("o4",pyobj)
        p.edAddChild(startNode)
        endNode = r.createScriptNode("Salome","endNode")
        endNode.setExecutionMode("remote")
        endNode.setContainer(cont)
        endNode.setSqueezeStatus(True)
        endNode.setScript("""
o5,o6 = 45,46
""")
        o5 = endNode.edAddOutputPort("o5",pyobj)
        o6 = endNode.edAddOutputPort("o6",pyobj)
        p.edAddChild(endNode)
        p.edAddCFLink(startNode,endNode)
        # disable proxy
        salome.cm.SetBigObjOnDiskThreshold(-1)
        # First part squeeze. 
        ex=pilot.ExecutorSwig()
        ex.RunW(p,0)
        self.assertEqual(p.getState(),pilot.DONE)
        for outp in ["o1","o2","o3","o4"]:
            self.assertTrue( p.getChildByName("startNode").getOutputPort(outp).getPyObj() is None )
        for outp in ["o5","o6"]:
            self.assertTrue( p.getChildByName("endNode").getOutputPort(outp).getPyObj() is None )
        #### Now it s time
        #KernelBasis.SetVerbosityActivated(True)
        startNode.setSqueezeStatusWithExceptions(True,["o1","o3"])#<- The key point is here
        endNode.setSqueezeStatusWithExceptions(True,["o5"])#<- The key point is here
        ex=pilot.ExecutorSwig()
        ex.RunW(p,0)
        self.assertEqual(p.getState(),pilot.DONE)
        salome.cm.ShutdownContainers()
        for outp in ["o2","o4"]:
            self.assertTrue( p.getChildByName("startNode").getOutputPort(outp).getPyObj() is None )
        for outp in ["o6"]:
            self.assertTrue( p.getChildByName("endNode").getOutputPort(outp).getPyObj() is None )
        #
        for outp,valExp in [("o1",31),("o3",33)]:
            self.assertEqual( p.getChildByName("startNode").getOutputPort(outp).getPyObj(), valExp )
        for outp,valExp in [("o5",45)]:
            self.assertEqual( p.getChildByName("endNode").getOutputPort(outp).getPyObj(), valExp )


if __name__ == '__main__':
  with tempfile.TemporaryDirectory() as dir_test:
    file_test = os.path.join(dir_test,"UnitTestsResult")
    with open(file_test, 'a') as f:
        f.write("  --- TEST src/yacsloader: testYacsPerfTest0.py\n")
        suite = unittest.makeSuite(TestYacsPerf0)
        result=unittest.TextTestRunner(f, descriptions=1, verbosity=1).run(suite)
        if not result.wasSuccessful():
           raise RuntimeError("Test failed !")
