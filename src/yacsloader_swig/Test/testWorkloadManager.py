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
        salome.salome_init()
        resourceManager = salome.lcc.getResourcesManager()
        resource_definition = resourceManager.GetResourceDefinition("localhost")
        resource_definition.nb_node = 16
        resourceManager.AddResource(resource_definition, False, "")
        #resource_definition = resourceManager.GetResourceDefinition("localhost")
        #self.assertEqual(resource_definition.nb_node, 16)

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
        # The containers need some time to be launched.
        # We need some room for that.
        self.assertTrue(execution_time < 20)

if __name__ == '__main__':
  dir_test = tempfile.mkdtemp(suffix=".yacstest")
  file_test = os.path.join(dir_test,"UnitTestsResult")
  with open(file_test, 'a') as f:
      f.write("  --- TEST src/yacsloader: testWorkloadManager.py\n")
      suite = unittest.makeSuite(TestEdit)
      result=unittest.TextTestRunner(f, descriptions=1, verbosity=1).run(suite)
  sys.exit(not result.wasSuccessful())
