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

class TestExec(unittest.TestCase):

    def setUp(self):
        SALOMERuntime.RuntimeSALOME_setRuntime(1)
        self.l = loader.YACSLoader()
        self.e = pilot.ExecutorSwig()
        self.p = self.l.load("samples/aschema.xml")
        pass
        
    def test1_StepByStep(self):
        # --- execution step by step
       
        print "================= Start of STEPBYSTEP ==================="
        self.e.setExecMode(1) # YACS::STEPBYSTEP
        
        run1 = threading.Thread(None, self.e.RunPy, "stepbystep", (self.p,0))
        run1.start()
        time.sleep(0.1)       # let the thread be initialised 
        #e.displayDot(self.p)
       
        tocont = True
        while tocont:
            self.e.waitPause()
            #e.displayDot(p)
            bp = self.e.getTasksToLoad()
            print "nexts possible steps = ", bp
            if len(bp) > 0:
                tte= bp[-1:] # only one node at each step, the last one in the list
                r = self.e.setStepsToExecute(tte)
                self.e.resumeCurrentBreakPoint()
                tocont = self.e.isNotFinished()
            else:
                tocont = False
                pass
            print "toContinue = ", tocont
            pass
        
        self.e.resumeCurrentBreakPoint()
        run1.join()
        self.assertEqual(106, self.p.getChildByName('node48').getEffectiveState())
        self.assertEqual(999, self.p.getChildByName('node13').getEffectiveState())
        self.assertEqual(888, self.p.getChildByName('node14').getEffectiveState())
        self.assertEqual(777, self.p.getChildByName('c1').getEffectiveState())
        self.assertEqual(777, self.p.getChildByName('c1.c1.n2').getEffectiveState())
        self.assertEqual(106, self.p.getChildByName('c0.c1.n1').getEffectiveState())
        self.assertEqual(999, self.p.getChildByName('c0.n2').getEffectiveState())
        self.assertEqual(888, self.p.getChildByName('node62').getEffectiveState())
        print "================= End of STEPBYSTEP ====================="
        pass

    def test2_StopToBreakpoint(self):
        # --- start execution, set a breakpoint before node48, then continue
        time.sleep(1)
        print "================= Start of BREAKPOINT ==================="
        brp=['node48']
        self.e.setListOfBreakPoints(brp)
        self.e.setExecMode(2) # YACS::STOPBEFORENODES
        self.run2 = threading.Thread(None, self.e.RunPy, "breakpoint", (self.p,0))
        self.run2.start()
        time.sleep(0.1)
        self.e.waitPause()
        #self.e.displayDot(p)
        print "================= reach BREAKPOINT ======================"
        # --- resume from breakpoint
        print "=========== BREAKPOINT, start RESUME ===================="
        time.sleep(1)
        self.e.setExecMode(0) # YACS::CONTINUE
        self.e.resumeCurrentBreakPoint()
        time.sleep(0.1)
        self.e.waitPause()
        #self.e.displayDot(p)
        self.run2.join()
        self.assertEqual(106, self.p.getChildByName('node48').getEffectiveState())
        self.assertEqual(999, self.p.getChildByName('node13').getEffectiveState())
        self.assertEqual(888, self.p.getChildByName('node14').getEffectiveState())
        self.assertEqual(777, self.p.getChildByName('c1').getEffectiveState())
        self.assertEqual(777, self.p.getChildByName('c1.c1.n2').getEffectiveState())
        self.assertEqual(106, self.p.getChildByName('c0.c1.n1').getEffectiveState())
        self.assertEqual(999, self.p.getChildByName('c0.n2').getEffectiveState())
        self.assertEqual(888, self.p.getChildByName('node62').getEffectiveState())
        print "================= End of RESUME ========================="
        pass
    
    def test3_RunWithoutBreakpoints(self):
        # --- start execution, run without breakpoints
        time.sleep(1)
        
        print "================= Start of CONTINUE ====================="
        self.e.setExecMode(0) # YACS::CONTINUE
        run3 = threading.Thread(None, self.e.RunPy, "continue", (self.p,0))
        run3.start()
        time.sleep(0.1)
        self.e.waitPause()
        #self.e.displayDot(p)
        run3.join()
        self.assertEqual(106, self.p.getChildByName('node48').getEffectiveState())
        self.assertEqual(999, self.p.getChildByName('node13').getEffectiveState())
        self.assertEqual(888, self.p.getChildByName('node14').getEffectiveState())
        self.assertEqual(777, self.p.getChildByName('c1').getEffectiveState())
        self.assertEqual(777, self.p.getChildByName('c1.c1.n2').getEffectiveState())
        self.assertEqual(106, self.p.getChildByName('c0.c1.n1').getEffectiveState())
        self.assertEqual(999, self.p.getChildByName('c0.n2').getEffectiveState())
        self.assertEqual(888, self.p.getChildByName('node62').getEffectiveState())
        print "================= End of CONTINUE ======================="
        pass

    def test4_StopOnError(self):
        # --- stop execution on first error and save state
        time.sleep(1)

        print "================= Start of STOPONERROR =================="
        self.e.setStopOnError()
        run4 = threading.Thread(None, self.e.RunPy, "continue", (self.p,0))
        run4.start()
        time.sleep(0.1)
        self.e.waitPause()
        self.e.saveState("dumpErrorASchema.xml")
        self.e.stopExecution()
        run4.join()
        #self.e.displayDot(self.p)
        s13 = self.p.getChildByName('node13').getEffectiveState()
        s43 = self.p.getChildByName('node43').getEffectiveState()
        self.assert_((s13==999) or (s43==999))
        print "================= End of STOPONERROR ====================="
        pass

    def test5_PartialExec(self):
        # --- stop execution after breakpoint
        time.sleep(1)

        print "================= Start of PARTIALEXEC ==================="
        brp=['node35']
        self.e.setListOfBreakPoints(brp)
        self.e.setExecMode(2) # YACS::STOPBEFORENODES
        #self.e.displayDot(self.p)
        run5 = threading.Thread(None, self.e.RunPy, "breakpoint", (self.p,0))
        run5.start()
        time.sleep(0.1)
        self.e.waitPause()
        #self.e.displayDot(self.p)
        self.e.saveState('dumpPartialASchema.xml')
        #self.e.displayDot(self.p)
        self.e.stopExecution()
        run5.join()
        #self.e.displayDot(self.p)
        self.assertEqual(106, self.p.getChildByName('node34').getEffectiveState())
        self.assertEqual(101, self.p.getChildByName('node35').getEffectiveState())
        print "================= reach BREAKPOINT PARTIAL EXEC =========="
        pass
                          
    pass

if __name__ == '__main__':
  import os
  U = os.getenv('USER')
  f=open("/tmp/" + U + "/UnitTestsResult", 'a')
  f.write("  --- TEST src/yacsloader: testExec.py\n")
  suite = unittest.makeSuite(TestExec)
  result=unittest.TextTestRunner(f, descriptions=1, verbosity=1).run(suite)
  f.close()
  sys.exit(not result.wasSuccessful())