#!/usr/bin/env python3
# Copyright (C) 2006-2026  CEA, EDF
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

import sys
from salome.yacs import pilot
from salome.yacs import SALOMERuntime
from salome.kernel import salome
from salome.yacs import loader
from salome.kernel import pylauncher
import unittest
import tempfile
import os
from salome.kernel.SALOME_PyNode import UnProxyObjectSimple
from pathlib import Path

class TestEdit(unittest.TestCase):

    def setUp(self):
        SALOMERuntime.RuntimeSALOME.setRuntime()
        self.r = pilot.getRuntime()
        self.l = loader.YACSLoader()
        self.e = pilot.ExecutorSwig()
        pass

    def tearDown(self):
        cm = salome.lcc.getContainerManager()
        cm.ShutdownContainers()

    def test1(self):
      """ Test the conservation of the python context between two nodes sharing
          the same container.
          Schema: n1 -> n2
      """
      runtime=self.r
      executor=self.e
      yacsloader=self.l
      proc=runtime.createProc("MySchema")
      ti=proc.createType("int","int")
      cont=proc.createContainer("MyContainer","Salome")
      # type "multi" : the workload manager chooses the resource
      # type "mono" : the resource is chosen by kernel, using the old rules.
      cont.setProperty("type","multi")
      # number of cores used by the container
      cont.setProperty("nb_parallel_procs", "1")
      n1=runtime.createScriptNode("","n1")
      n2=runtime.createScriptNode("","n2")
      n1.setExecutionMode("remote")
      n2.setExecutionMode("remote")
      n1.setContainer(cont)
      n2.setContainer(cont)
      n1.setScript("v=42")
      res_port=n2.edAddOutputPort("v", ti)
      proc.edAddChild(n1)
      proc.edAddChild(n2)
      proc.edAddCFLink(n1,n2)
      # set the default execution mode using the workload manager
      # if no property is set, the old executor is used
      proc.setProperty("executor", "workloadmanager")
      # reuse the same python context for every execution
      cont.usePythonCache(True)
      # save & reload
      schema_file = os.path.join(dir_test,"pynode_with_cache1.xml")
      proc.saveSchema(schema_file)
      reloaded_proc = yacsloader.load(schema_file)
      # default run method of the executor which uses the property "executor"
      # in order to choose the actual run method
      executor.RunW(reloaded_proc,0)
      # you can also directly call the executor you wish, ignoring the property
      #executor.RunB(proc,0) # always use the "old" executor
      #executor.runWlm(proc,0) # always use the workload manager based executor
      reloaded_res_port = reloaded_proc.getChildByName("n2").getOutputPort("v")
      self.assertEqual(UnProxyObjectSimple( reloaded_res_port.getPyObj() ), 42)

    def test2(self):
      """ Same as test1, but using the old executor instead of workload manager.
      """
      runtime=self.r
      executor=self.e
      yacsloader=self.l
      proc=runtime.createProc("MySchema")
      ti=proc.createType("int","int")
      cont=proc.createContainer("MyContainer","Salome")
      # With the old executor the type multi imposes the creation of a new
      # container for every node. We need the type "mono" in order to have
      # the same container used for both yacs nodes.
      cont.setProperty("type","mono")
      n1=runtime.createScriptNode("","n1")
      n2=runtime.createScriptNode("","n2")
      n1.setExecutionMode("remote")
      n2.setExecutionMode("remote")
      n1.setContainer(cont)
      n2.setContainer(cont)
      n1.setScript("v=42")
      res_port=n2.edAddOutputPort("v", ti)
      proc.edAddChild(n1)
      proc.edAddChild(n2)
      proc.edAddCFLink(n1,n2)
      # reuse the same python context for every execution
      cont.usePythonCache(True)
      # save & reload
      schema_file = os.path.join(dir_test,"pynode_with_cache2.xml")
      proc.saveSchema(schema_file)
      reloaded_proc = yacsloader.load(schema_file)
      # default run method of the executor which uses the property "executor"
      # in order to choose the actual run method
      executor.RunW(reloaded_proc,0)
      # you can also directly call the executor you wish, ignoring the property
      #executor.RunB(proc,0) # always use the "old" executor
      reloaded_res_port = reloaded_proc.getChildByName("n2").getOutputPort("v")
      self.assertEqual(UnProxyObjectSimple( reloaded_res_port.getPyObj() ), 42)

if __name__ == '__main__':
  salome.salome_init()
  # EDF33455 : force single core
  rmcpp = pylauncher.RetrieveRMCppSingleton()
  a = rmcpp.GetResourceDefinition("localhost")
  rmcpp.DeleteAllResourcesInCatalog()
  res = pylauncher.CreateContainerResource("localhost", applipath=a.applipath, protocol="sh", nbOfNodes=1)
  res.hostname = a.hostname
  rmcpp.AddResourceInCatalogNoQuestion( res )
  # EDF33455 : end of forcing single core
  with  tempfile.TemporaryDirectory() as dir_test:
    file_test = Path(dir_test) / "UnitTestsResult"
    with open( str( file_test ), 'a') as f:
        f.write("  --- TEST src/yacsloader: testPynodeWithCache.py\n")
        suite = unittest.makeSuite(TestEdit)
        result=unittest.TextTestRunner(f, descriptions=1, verbosity=1).run(suite)
    sys.exit(not result.wasSuccessful())
