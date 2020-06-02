#!/usr/bin/env python3
# Copyright (C) 2006-2020  CEA/DEN, EDF R&D
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
import pilot
import SALOMERuntime
import loader
import unittest
import tempfile
import os

class TestEdit(unittest.TestCase):

    def setUp(self):
        SALOMERuntime.RuntimeSALOME_setRuntime()
        self.r = pilot.getRuntime()
        self.l = loader.YACSLoader()
        self.e = pilot.ExecutorSwig()
        pass

    def test1(self):
      """ Test the conservation of the python context between two nodes sharing
          the same container.
          Schema: n1 -> n2
      """
      runtime=self.r
      executor=self.e
      proc=runtime.createProc("MySchema")
      ti=proc.createType("int","int")
      cont=proc.createContainer("MyContainer","Salome")
      # type "multi" : the workload manager chooses the resource
      # type "mono" : the workload manager does not choose the resource
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
      proc.setProperty("executor", "workloadmanager")
      # reuse the same python context for every execution
      cont.setStoreContext(True)
      #proc.saveSchema("mini_wlm.xml")
      executor=pilot.ExecutorSwig()
      # default run method of the executor which uses the property "executor"
      # in order to choose the actual run method
      executor.RunW(proc,0)
      # you can also impose the executor, ignoring the property "executor"
      #executor.RunB(proc,0) # use the "historical" executor
      #executor.runWlm(proc,0) # use the workload manager based executor
      
      self.assertEqual(res_port.getPyObj(), 42)

if __name__ == '__main__':
  dir_test = tempfile.mkdtemp(suffix=".yacstest")
  file_test = os.join(dir_test,"UnitTestsResult")
  with open(file_test, 'a') as f:
      f.write("  --- TEST src/yacsloader: testWorkloadManager.py\n")
      suite = unittest.makeSuite(TestEdit)
      result=unittest.TextTestRunner(f, descriptions=1, verbosity=1).run(suite)
  sys.exit(not result.wasSuccessful())
