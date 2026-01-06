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
import subprocess as sp

class TestYacsDriverOverrides(unittest.TestCase):
    def test0(self):
        """
        [EDF28531] : Perf to check when number of threads in charge scheduler side is lower than number of // tasks
        """
        NB_OF_PARALLEL_NODES = 100
        NB_OF_PARALLEL_THREADS = 1000
        salome.salome_init()
        with tempfile.TemporaryDirectory() as tmpdirname:
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
            cont.setProperty("nb_parallel_procs","2")
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
            internalNode.setScript("""from salome.kernel import KernelBasis
ret = KernelBasis.tony*ppp
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
            endNode.setScript("""ozeret = izeret
with open("res.txt","w") as f:
  f.write( str( sum(izeret) ) )
""")
            p.edAddChild(endNode)
            p.edAddCFLink(fe,endNode)
            p.edAddLink( oret, izeret )
            fname = os.path.join( tmpdirname,"YacsOverrides.xml")
            p.saveSchema(fname)
            # important part of test is here
            with open(os.path.join(tmpdirname,"yacs_driver_overrides.py"),"w") as fover:
               fover.write("""import logging
import multiprocessing as mp
                           
def customize(cm, allresources):
  cm.SetCodeOnContainerStartUp("from salome.kernel import KernelBasis\\nKernelBasis.tony = 5")
  #
  cpy = {elt:allresources[elt] for elt in allresources.GetListOfEntries()}
  allresources.DeleteAllResourcesInCatalog()
  for it in cpy:
    cpy[it].nb_node = mp.cpu_count()
    cpy[it].protocol = "ssh"
    allresources.AddResourceInCatalogNoQuestion(cpy[it])
  logging.debug( repr( allresources ) )""")
            # end of important
            print("Start computation")
            spp = sp.Popen(["driver","-v","-vl","DEBUG","--activate-custom-overrides",fname],cwd = tmpdirname)# --activate-custom-overrides is the key point of the test
            spp.communicate()
            self.assertEqual(spp.returncode,0,"Driver process has failed !")
            with open(os.path.join(tmpdirname,"res.txt")) as f:
               self.assertEqual( int( f.read() ), 24750 )

if __name__ == '__main__':
  with tempfile.TemporaryDirectory() as dir_test:
    file_test = os.path.join(dir_test,"UnitTestsResult")
    with open(file_test, 'a') as f:
        f.write("  --- TEST src/yacsloader_swig : TestYacsOverrides.py\n")
        suite = unittest.makeSuite(TestYacsDriverOverrides)
        result=unittest.TextTestRunner(f, descriptions=1, verbosity=1).run(suite)
        if not result.wasSuccessful():
           raise RuntimeError("Test failed !")
