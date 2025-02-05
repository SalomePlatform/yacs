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

import unittest
import tempfile
import os
import subprocess

import SALOMERuntime
import loader
import pilot
import salome

dir_test = tempfile.mkdtemp(suffix=".yacstest")

class TestDeco(unittest.TestCase):

    def setUp(self):
      SALOMERuntime.RuntimeSALOME_setRuntime()
      # We need a catalog which contains only one resource named "localhost"
      # with 16 cores. The modifications made here are not saved to the
      # catalog file.
      NB_NODE = 16
      salome.salome_init()
      resourceManager = salome.lcc.getResourcesManager()
      resource_definition = resourceManager.GetResourceDefinition("localhost")
      resource_definition.nb_node = NB_NODE
      resourceManager.AddResource(resource_definition, False, "")
      resource_required = salome.ResourceParameters()
      resource_required.can_run_containers = True
      res_list = resourceManager.GetFittingResources(resource_required)
      for r in res_list:
        if r != "localhost":
          resourceManager.RemoveResource(r, False, "")
      resource_definition = resourceManager.GetResourceDefinition("localhost")
      self.assertEqual(resource_definition.nb_node, NB_NODE)

    def tearDown(self):
        cm = salome.lcc.getContainerManager()
        cm.ShutdownContainers()

    def test_t1(self):
      """
      Schema:
      jdd -> foreach -> post
           > f2
         /
      f1 -> f3 -> f1
      """
      import testforeach
      from SALOME_PyNode import UnProxyObjectSimple
      expected_1, expected_2 = testforeach.main()
      yacs_schema_file = os.path.join(dir_test, "schema_t1.xml")
      yacs_build_command = "yacsbuild.py"
      test_script = "testforeach.py"
      main_function_name = "main"
      subprocess.run([yacs_build_command,
                      test_script, main_function_name, yacs_schema_file])
      l = loader.YACSLoader()
      ex = pilot.ExecutorSwig()
      proc = l.load(yacs_schema_file)
      ex.RunW(proc,0)
      obtained_1 = UnProxyObjectSimple( proc.getChildByName("post_0").getOutputPort("s").getPyObj() )
      obtained_2 = UnProxyObjectSimple( proc.getChildByName("f1_1").getOutputPort("r").getPyObj() )
      self.assertEqual(expected_1, obtained_1)
      self.assertEqual(expected_2, obtained_2)

    def test_t2(self):
      """
      Foreach initialized by value.
      """
      import testforeach
      from SALOME_PyNode import UnProxyObjectSimple
      expected_1, expected_2 = testforeach.mainblock()
      yacs_schema_file = os.path.join(dir_test, "schema_t2.xml")
      yacs_build_command = "yacsbuild.py"
      test_script = "testforeach.py"
      main_function_name = "mainblock"
      subprocess.run([yacs_build_command,
                      test_script, main_function_name, yacs_schema_file])
      l = loader.YACSLoader()
      ex = pilot.ExecutorSwig()
      proc = l.load(yacs_schema_file)
      ex.RunW(proc,0)
      obtained_1 = UnProxyObjectSimple( proc.getChildByName("output_fr_0").getOutputPort("s_0").getPyObj() )
      obtained_2 = UnProxyObjectSimple( proc.getChildByName("output_fr_0").getOutputPort("p_1").getPyObj() )
      self.assertEqual(expected_1, obtained_1)
      self.assertEqual(expected_2, obtained_2)

    def test_t3(self):
      """
      Foreach on 2 levels.
      """
      import testforeach
      from SALOME_PyNode import UnProxyObjectSimple
      expected = testforeach.maindoublefr()
      yacs_schema_file = os.path.join(dir_test, "schema_t3.xml")
      yacs_build_command = "yacsbuild.py"
      test_script = "testforeach.py"
      main_function_name = "maindoublefr"
      subprocess.run([yacs_build_command,
                      test_script, main_function_name, yacs_schema_file])
      l = loader.YACSLoader()
      ex = pilot.ExecutorSwig()
      proc = l.load(yacs_schema_file)
      ex.RunW(proc,0)
      obtained = UnProxyObjectSimple( proc.getChildByName("output_doublefr_0").getOutputPort("r_0_0").getPyObj() )
      self.assertEqual(expected, obtained)

    def test_t4(self):
      """
      Using specific containers.
      This test needs at least 4 cores declared in the catalog of resources.
      """
      import yacsdecorator
      cm = yacsdecorator.ContainerManager()
      cm.addContainer("c1", 1, False)
      cm.addContainer("c2", 4, True)
      cm.addContainer(yacsdecorator.ContainerManager.defaultContainerName, 1, False)
      cont_file = os.path.join(dir_test, "containers_t4.json")
      cm.saveFile(cont_file)
      script = """import yacsdecorator
@yacsdecorator.leaf("c1")
def f_c1(x,y):
  s = x + y
  return s

@yacsdecorator.leaf("c2")
def f_c2(x,y):
  p = x * y
  return p

@yacsdecorator.leaf
def f_def(x,y):
  d = x - y
  return d

@yacsdecorator.block
def main():
  s1 = f_c1(3,4)
  p1 = f_c2(5,6)
  r = f_def(p1, s1)
"""
      script_file = os.path.join(dir_test, "script_t4.py")
      with open(script_file, "w") as f:
        f.write(script)
      yacs_build_command = "yacsbuild.py"
      main_function_name = "main"
      yacs_schema_file = os.path.join(dir_test, "schema_t4.xml")
      subprocess.run([yacs_build_command,
                      script_file, main_function_name, yacs_schema_file,
                      "-c", cont_file])
      l = loader.YACSLoader()
      ex = pilot.ExecutorSwig()
      proc = l.load(yacs_schema_file)
      ex.RunW(proc,0)
      self.assertEqual(proc.getState(),pilot.DONE)
      c1 = proc.getChildByName("f_c1_0").getContainer()
      self.assertFalse(c1.isUsingPythonCache())
      self.assertEqual(c1.getProperty("nb_parallel_procs"), "1")
      c2 = proc.getChildByName("f_c2_0").getContainer()
      self.assertTrue(c2.isUsingPythonCache())
      self.assertEqual(c2.getProperty("nb_parallel_procs"), "4")
      c3 = proc.getChildByName("f_def_0").getContainer()
      self.assertFalse(c3.isUsingPythonCache())
      self.assertEqual(c3.getProperty("nb_parallel_procs"), "1")

if __name__ == '__main__':
  file_test = os.path.join(dir_test,"UnitTestsResult")
  with open(file_test, 'a') as f:
      f.write("  --- TEST src/py2yacs: testDeco.py\n")
      suite = unittest.makeSuite(TestDeco)
      result=unittest.TextTestRunner(f, descriptions=1, verbosity=1).run(suite)
  sys.exit(not result.wasSuccessful())
