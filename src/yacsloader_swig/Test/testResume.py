#!/usr/bin/env python
# Copyright (C) 2006-2016  CEA/DEN, EDF R&D
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

class TestResume(unittest.TestCase):

    def setUp(self):
        SALOMERuntime.RuntimeSALOME_setRuntime(1)
        self.l = loader.YACSLoader()
        self.e = pilot.ExecutorSwig()
        self.p = self.l.load("samples/bloc2.xml")
        pass

    def test1_PartialExec(self):
        # --- stop execution after breakpoint
        time.sleep(1)

        print "================= Start of PARTIALEXEC ==================="
        brp=['b1.b2.node1']
        self.e.setListOfBreakPoints(brp)
        self.e.setExecMode(2) # YACS::STOPBEFORENODES
        #self.e.displayDot(self.p)
        run1 = threading.Thread(None, self.e.RunPy, "breakpoint", (self.p,0))
        run1.start()
        time.sleep(0.1)
        self.e.waitPause()
        #self.e.displayDot(self.p)
        self.e.saveState('dumpPartialBloc2.xml')
        #self.e.displayDot(self.p)
        self.e.stopExecution()
        #self.e.displayDot(self.p)
        self.assertEqual(101, self.p.getChildByName('b1.b2.node1').getEffectiveState())
        self.assertEqual(106, self.p.getChildByName('b1.node1').getEffectiveState())
        print "================= reach BREAKPOINT PARTIAL EXEC =========="
        pass

    def test2_ExecFromLoadState(self):
        # --- reload state from previous partial execution then exec
        time.sleep(1)

        print "================= Start of EXECLOADEDSTATE ==============="
        sp = loader.stateParser()
        sl = loader.stateLoader(sp,self.p)
        sl.parse('dumpPartialBloc2.xml')
        #self.e.displayDot(self.p)
        self.e.setExecMode(0) # YACS::CONTINUE
        run2 = threading.Thread(None, self.e.RunPy, "loadState", (self.p,0,True,True))
        run2.start()
        time.sleep(0.1)
        self.e.waitPause()
        #self.e.displayDot(self.p)
        run2.join()
        self.assertEqual(106, self.p.getChildByName('node1').getEffectiveState())
        self.assertEqual(106, self.p.getChildByName('node2').getEffectiveState())
        self.assertEqual(106, self.p.getChildByName('b1.node1').getEffectiveState())
        self.assertEqual(106, self.p.getChildByName('b1.node2').getEffectiveState())
        self.assertEqual(106, self.p.getChildByName('b1.b2.node1').getEffectiveState())
        self.assertEqual(106, self.p.getChildByName('b1.b2.node2').getEffectiveState())
        self.assertEqual(106, self.p.getChildByName('b1.b2.loop1.node1').getEffectiveState())
        print "================= End of EXECLOADEDSTATE ================="
                          
    pass

if __name__ == '__main__':
  import os
  U = os.getenv('USER')
  f=open("/tmp/" + U + "/UnitTestsResult", 'a')
  f.write("  --- TEST src/yacsloader: testResume.py\n")
  suite = unittest.makeSuite(TestResume)
  result=unittest.TextTestRunner(f, descriptions=1, verbosity=1).run(suite)
  f.close()
  sys.exit(not result.wasSuccessful())
