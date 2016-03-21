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

import sys
import pilot
import SALOMERuntime
import loader
import unittest

class TestEdit(unittest.TestCase):

    def setUp(self):
        SALOMERuntime.RuntimeSALOME_setRuntime()
        self.r = pilot.getRuntime()
        self.l = loader.YACSLoader()
        self.e = pilot.ExecutorSwig()
        pass

    def test_progress(self):
  
        p=self.r.createProc("pr")
        ti=p.getTypeCode("int")
        td=p.getTypeCode("double")
        ts=p.getTypeCode("string")

        #BLOC
        b=self.r.createBloc("b1")
        p.edAddChild(b)
        n1=self.r.createScriptNode("","node1")
        b.edAddChild(n1)
        n1.setScript("p1=p1+10")
        n1.edAddInputPort("p1",ti)
        n1.edAddOutputPort("p1",ti)
        n2=self.r.createScriptNode("","node2")
        b.edAddChild(n2)
        n2.setScript("p1=2*p1")
        n2.edAddInputPort("p1",ti)
        n2.edAddOutputPort("p1",ti)
        b.edAddDFLink(n1.getOutputPort("p1"),n2.getInputPort("p1"))
        #initialisation ports
        n1.getInputPort("p1").edInitPy(5)

        #FOR LOOP
        loop=self.r.createForLoop("l1")
        p.edAddChild(loop)
        ip=loop.getInputPort("nsteps")
        ip.edInitPy(3)
        n10=self.r.createScriptNode("","node10")
        loop.edSetNode(n10)
        n10.setScript("p1=p1+10")
        n10.edAddInputPort("p1",ti)
        n10.edAddOutputPort("p1",ti)
        n10.getInputPort("p1").edInitPy(5)
        

        #WHILE LOOP
        wh=self.r.createWhileLoop("w1")
        p.edAddChild(wh)
        n20=self.r.createScriptNode("","node3")
        n20.setScript("p1=0")
        n20.edAddOutputPort("p1",ti)
        wh.edSetNode(n20)
        cport=wh.getInputPort("condition")
        cport.edInitBool(True)
        p.edAddLink(n20.getOutputPort("p1"),cport)


        #FOR EACH LOOP
        fe=self.r.createForEachLoop("fe1",td)
        p.edAddChild(fe)
        n30=self.r.createScriptNode("","node3")
        n30.setScript("import time \ntime.sleep(1) \np1=p1+3.\n")
        n30.edAddInputPort("p1",td)
        n30.edAddOutputPort("p1",td)
        fe.edSetNode(n30)
        p.edAddLink(fe.getOutputPort("evalSamples"),n30.getInputPort("p1"))
        fe.getInputPort("nbBranches").edInitPy(2)
        fe.getInputPort("SmplsCollection").edInitPy([1.,2.,3.,4.,5.,6.])

        #SWITCH
        n40=self.r.createScriptNode("","node3")
        n40.setScript("p1=3.5")
        n40.edAddOutputPort("p1",td)
        p.edAddChild(n40)
        #switch
        sw=self.r.createSwitch("sw1")
        p.edAddChild(sw)
        nk1=self.r.createScriptNode("","ncas1")
        nk1.setScript("p1=p1+3.")
        nk1.edAddInputPort("p1",td)
        nk1.edAddOutputPort("p1",td)
        sw.edSetNode(1,nk1)
        ndef=self.r.createScriptNode("","ndefault")
        ndef.setScript("p1=p1+5.")
        ndef.edAddInputPort("p1",td)
        ndef.edAddOutputPort("p1",td)
        sw.edSetDefaultNode(ndef)
        #initialise the select port
        sw.getInputPort("select").edInitPy(1)
        #connection of internal nodes
        p.edAddDFLink(n40.getOutputPort("p1"),nk1.getInputPort("p1"))
        p.edAddDFLink(n40.getOutputPort("p1"),ndef.getInputPort("p1"))

        import time
        import threading
        self.assertEqual(p.getGlobalProgressPercent(),0)
        self.assertEqual(p.getState(),pilot.READY)
        myRun = threading.Thread(None, self.e.RunW, None, (p,0))
        myRun.start()
        time.sleep(1.5)
        self.assertGreater(p.getGlobalProgressPercent(),0)
        self.assertLess(p.getGlobalProgressPercent(),100)
        myRun.join()
        self.assertEqual(p.getState(),pilot.DONE)
        self.assertEqual(p.getGlobalProgressPercent(),100)
         
if __name__ == '__main__':
  import os
  U = os.getenv('USER')
  f=open("/tmp/" + U + "/UnitTestsResult", 'a')
  f.write("  --- TEST src/yacsloader: testProgress.py\n")
  suite = unittest.makeSuite(TestEdit)
  result=unittest.TextTestRunner(f, descriptions=1, verbosity=3).run(suite)
  f.close()
  sys.exit(not result.wasSuccessful())
