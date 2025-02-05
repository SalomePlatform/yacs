#!/usr/bin/env python3
# Copyright (C) 2006-2025  CEA, EDF
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

"""
Various non regression tests.
"""

import sys
import pilot
import SALOMERuntime
import loader
import unittest
import tempfile
import os
import salome

NB_NODE=15
class TestEdit(unittest.TestCase):

    def setUp(self):
        SALOMERuntime.RuntimeSALOME_setRuntime()
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
        resource_required = salome.ResourceParameters()
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

    def test_double_foreach_switch(self):
        """ Two layers of nested foreach and switch structures.
        """
        proc = self.l.load("samples/foreach_switch_double.xml")
        self.e.RunW(proc,0)
        self.assertEqual(proc.getState(),pilot.DONE)
        ok = proc.getChildByName("End").getOutputPort("ok")
        self.assertTrue(ok, "Wrong result!")

if __name__ == '__main__':
  dir_test = tempfile.mkdtemp(suffix=".yacstest")
  file_test = os.path.join(dir_test,"UnitTestsResult")
  with open(file_test, 'a') as f:
      f.write("  --- TEST src/yacsloader: testFixes.py\n")
      suite = unittest.makeSuite(TestEdit)
      result=unittest.TextTestRunner(f, descriptions=1, verbosity=1).run(suite)
  sys.exit(not result.wasSuccessful())
