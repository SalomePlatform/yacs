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

import unittest
import tempfile
import os
import subprocess

import SALOMERuntime
import loader
import pilot

dir_test = tempfile.mkdtemp(suffix=".yacstest")

class TestDeco(unittest.TestCase):

    def setUp(self):
      SALOMERuntime.RuntimeSALOME_setRuntime()

    def test_t1(self):
      """
      Schema:
      jdd -> foreach -> post
           > f2
         /
      f1 -> f3 -> f1
      """
      import testforeach
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
      obtained_1 = proc.getChildByName("post_0").getOutputPort("s").getPyObj()
      obtained_2 = proc.getChildByName("f1_1").getOutputPort("r").getPyObj()
      self.assertEqual(expected_1, obtained_1)
      self.assertEqual(expected_2, obtained_2)

    def test_t2(self):
      """
      Foreach initialized by value.
      """
      import testforeach
      expected_1, expected_2 = testforeach.mainbloc()
      yacs_schema_file = os.path.join(dir_test, "schema_t2.xml")
      yacs_build_command = "yacsbuild.py"
      test_script = "testforeach.py"
      main_function_name = "mainbloc"
      subprocess.run([yacs_build_command,
                      test_script, main_function_name, yacs_schema_file])
      l = loader.YACSLoader()
      ex = pilot.ExecutorSwig()
      proc = l.load(yacs_schema_file)
      ex.RunW(proc,0)
      obtained_1 = proc.getChildByName("output_fr_0").getOutputPort("s_0").getPyObj()
      obtained_2 = proc.getChildByName("output_fr_0").getOutputPort("p_1").getPyObj()
      self.assertEqual(expected_1, obtained_1)
      self.assertEqual(expected_2, obtained_2)

    def test_t3(self):
      """
      Foreach on 2 levels.
      """
      import testforeach
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
      obtained = proc.getChildByName("output_doublefr_0").getOutputPort("r_0_0").getPyObj()
      self.assertEqual(expected, obtained)

if __name__ == '__main__':
  file_test = os.path.join(dir_test,"UnitTestsResult")
  with open(file_test, 'a') as f:
      f.write("  --- TEST src/py2yacs: testDeco.py\n")
      suite = unittest.makeSuite(TestDeco)
      result=unittest.TextTestRunner(f, descriptions=1, verbosity=1).run(suite)
  sys.exit(not result.wasSuccessful())
