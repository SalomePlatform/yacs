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
from salome.yacs import loader
import unittest
import tempfile
import os
from salome.kernel import salome
from salome.kernel.LifeCycleCORBA import ResourceParameters

NB_NODE=15
class TestEdit(unittest.TestCase):

    def setUp(self):
        SALOMERuntime.RuntimeSALOME.setRuntime()
        self.r = pilot.getRuntime()
        self.l = loader.YACSLoader()
        self.e = pilot.ExecutorSwig()
        # We need a catalog which contains only one resource named "localhost"
        # with NB_NODE cores. The modifications made here are not saved to the
        # catalog file.
        salome.salome_init()
        resourceManager = salome.lcc.getResourcesManager()
        resource_definition = resourceManager.GetResourceDefinition("localhost")
        resource_definition.nb_node = NB_NODE
        resource_definition.nb_proc_per_node = 1
        resourceManager.AddResource(resource_definition, False, "")
        resource_required = ResourceParameters()
        resource_required.can_run_containers = True
        res_list = resourceManager.GetFittingResources(resource_required)
        for r in res_list:
          if r != "localhost":
            resourceManager.RemoveResource(r, False, "")
        resource_definition = resourceManager.GetResourceDefinition("localhost")
        self.assertEqual(resource_definition.nb_node, NB_NODE)
        self.assertEqual(resource_definition.nb_proc_per_node, 1)

    def tearDown(self):
        cm = salome.lcc.getContainerManager()
        cm.ShutdownContainers()

    def test1(self):
        """ Two parallel foreach-s with different containers
        """
        proc = self.l.load("samples/wlm_2foreach.xml")
        self.e.RunW(proc,0)
        self.assertEqual(proc.getState(),pilot.DONE)
        res_port = proc.getChildByName("End").getOutputPort("r")
        # theoretical time should be 15s
        execution_time = res_port.getPyObj()
        # lower time means some resources are overloaded
        msg = "Execution time is too short : {}s".format(execution_time)
        self.assertTrue(execution_time > 13, msg)
        # The containers may need some time to be launched.
        # We need some delay to add to the 15s.
        msg = "Execution time is too long : {}s".format(execution_time)
        self.assertTrue(execution_time < 20, msg)

    def test2(self):
        """ Two parallel foreach-s with different containers and python nodes
            using cache.
        """
        proc = self.l.load("samples/wlm_2foreach_with_cache.xml")
        self.e.RunW(proc,0)
        self.assertEqual(proc.getState(),pilot.DONE)
        ok = proc.getChildByName("End").getOutputPort("ok")
        self.assertTrue(ok)
        total_time = proc.getChildByName("End").getOutputPort("total_time")
        # theoretical time should be 16s
        execution_time = total_time.getPyObj()
        # lower time means some resources are overloaded
        msg = "Execution time is too short : {}s".format(execution_time)
        self.assertTrue(execution_time > 14, msg)
        # The containers may need some time to be launched.
        # We need some delay to add to the 16s.
        msg = "Execution time is too long : {}s".format(execution_time)
        self.assertTrue(execution_time < 20, msg)
        coeff_cont = proc.getChildByName("End").getOutputPort("coeff_cont").getPyObj()
        msg = "coeff_cont too low:"+str(coeff_cont)
        self.assertTrue(coeff_cont >= NB_NODE, msg)
        msg = "coeff_cont too high:"+str(coeff_cont)
        self.assertTrue(coeff_cont <= 2*NB_NODE, msg)

    def test3(self):
        """ Launch 8 independent nodes in parallel.
        """
        proc = self.l.load("samples/wlm_8nodes.xml")
        self.e.RunW(proc,0)
        self.assertEqual(proc.getState(),pilot.DONE)
        ok = proc.getChildByName("End").getOutputPort("ok")
        if not ok :
          err_message = proc.getChildByName("End").getOutputPort("err_message").getPyObj()
          self.fail(err_message)

    def test4(self):
        """ Verify the execution is stoped if no resource can run a task.
        """
        proc = self.l.load("samples/wlm_error.xml")
        self.e.RunW(proc,0)
        self.assertEqual(proc.getState(),pilot.FAILED)
        self.assertEqual(proc.getChildByName("ErrorNode").getState(),pilot.ERROR)

    def test5(self):
        """ Foreach with 1000 points and several nodes in the block.
        """
        proc = self.l.load("samples/wlm_complex_foreach.xml")
        self.e.RunW(proc,0)
        self.assertEqual(proc.getState(),pilot.DONE)

if __name__ == '__main__':
  dir_test = tempfile.mkdtemp(suffix=".yacstest")
  file_test = os.path.join(dir_test,"UnitTestsResult")
  with open(file_test, 'a') as f:
      f.write("  --- TEST src/yacsloader: testWorkloadManager.py\n")
      suite = unittest.makeSuite(TestEdit)
      result=unittest.TextTestRunner(f, descriptions=1, verbosity=1).run(suite)
  sys.exit(not result.wasSuccessful())
