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

import time
import unittest
import threading

import SALOMERuntime
import loader
import pilot

import os
import tempfile

class TestExecForEachGeoMesh(unittest.TestCase):

    def setUp(self):
        SALOMERuntime.RuntimeSALOME.setRuntime(1)
        self.l = loader.YACSLoader()
        self.e = pilot.ExecutorSwig()
        self.p = self.l.load("samples/geomesh0ForEach.xml")
        pass
        
    @unittest.skipIf("SMESH_ROOT_DIR" not in os.environ, "requires SMESH (and GEOM)")
    def test1_Run(self):
        # --- start execution, run without breakpoints
        time.sleep(1)
        
        print("================= Start of CONTINUE =====================")
        self.e.setExecMode(0) # YACS::CONTINUE
        run1 = threading.Thread(None, self.e.RunPy, "continue", (self.p,0))
        run1.start()
        time.sleep(0.1)
        self.e.waitPause()
        #self.e.displayDot(p)
        run1.join()
        self.assertEqual(106, self.p.getChildByName('PyScript0').getEffectiveState())
        print("================= End of CONTINUE =======================")
        pass

    pass

if __name__ == '__main__':
  dir_test = tempfile.mkdtemp(suffix=".yacstest")
  file_test = os.path.join(dir_test,"UnitTestsResult")
  with open(file_test, 'a') as f:
        f.write("  --- TEST src/yacsloader: testExec.py\n")
        suite = unittest.makeSuite(TestExecForEachGeoMesh)
        result = unittest.TextTestRunner(f, descriptions=1, verbosity=1).run(suite)

  sys.exit(not result.wasSuccessful())
