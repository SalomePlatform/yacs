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
import salome

class TestEdit(unittest.TestCase):

    def setUp(self):
        SALOMERuntime.RuntimeSALOME_setRuntime()
        self.r = pilot.getRuntime()
        self.l = loader.YACSLoader()
        self.e = pilot.ExecutorSwig()
        # We need a catalog which contains only one resource named "localhost"
        # with 16 cores. The modifications made here are not saved to the
        # catalog file.
        salome.salome_init()
        resourceManager = salome.lcc.getResourcesManager()
        resource_definition = resourceManager.GetResourceDefinition("localhost")
        resource_definition.nb_node = 16
        resourceManager.AddResource(resource_definition, False, "")
        resource_required = salome.ResourceParameters()
        resource_required.can_run_containers = True
        res_list = resourceManager.GetFittingResources(resource_required)
        for r in res_list:
          if r != "localhost":
            resourceManager.RemoveResource(r, False, "")
        #resource_definition = resourceManager.GetResourceDefinition("localhost")
        #self.assertEqual(resource_definition.nb_node, 16)

    def tearDown(self):
        cm = salome.lcc.getContainerManager()
        cm.ShutdownContainers()

    def test1(self):
        """ Two parallel foreach-s with different containers
        """
        proc = self.l.load("samples/wlm_2foreach.xml")
        self.e.RunW(proc,0)
        res_port = proc.getChildByName("End").getOutputPort("r")
        # theoretical time should be 15s
        execution_time = res_port.getPyObj()
        # lower time means some resources are overloaded
        self.assertTrue(execution_time > 13)
        # The containers may need some time to be launched.
        # We need some delay to add to the 15s.
        self.assertTrue(execution_time < 20)

    def test2(self):
        """ Two parallel foreach-s with different containers and python nodes
            using cache.
        """
        proc = self.l.load("samples/wlm_2foreach_with_cache.xml")
        self.e.RunW(proc,0)
        ok = proc.getChildByName("End").getOutputPort("ok")
        self.assertTrue(ok)
        total_time = proc.getChildByName("End").getOutputPort("total_time")
        # theoretical time should be 16s
        execution_time = total_time.getPyObj()
        # lower time means some resources are overloaded
        self.assertTrue(execution_time > 14)
        # The containers may need some time to be launched.
        # We need some delay to add to the 16s.
        self.assertTrue(execution_time < 20)

    def test3(self):
        """ Launch 8 independent nodes in parallel.
        """
        proc = self.l.load("samples/wlm_8nodes.xml")
        self.e.RunW(proc,0)
        ok = proc.getChildByName("End").getOutputPort("ok")
        if not ok :
          err_message = proc.getChildByName("End").getOutputPort("err_message")
          self.fail(err_message)

if __name__ == '__main__':
  dir_test = tempfile.mkdtemp(suffix=".yacstest")
  file_test = os.path.join(dir_test,"UnitTestsResult")
  with open(file_test, 'a') as f:
      f.write("  --- TEST src/yacsloader: testWorkloadManager.py\n")
      suite = unittest.makeSuite(TestEdit)
      result=unittest.TextTestRunner(f, descriptions=1, verbosity=1).run(suite)
  sys.exit(not result.wasSuccessful())
