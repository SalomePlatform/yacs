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

    def test1_edit(self):
        p = self.r.createProc("pr")
        print p.typeMap
        t=p.getTypeCode("double")
        print t.kind()
        t=p.typeMap["double"]
        
        td=p.createType("double","double")
        ti=p.createType("int","int")
        tc1=p.createInterfaceTc("","Obj",[])
        print tc1.name(),tc1.id()
        tc2=p.createInterfaceTc("","Obj2",[tc1])
        print tc2.name(),tc2.id()
        tc3=p.createSequenceTc("","seqdbl",td)
        print tc3.name(),tc3.id(),tc3.contentType()
        tc4=p.createSequenceTc("","seqObj2",tc2)
        tc5=p.createSequenceTc("","seqint",ti)
        print tc4.name(),tc4.id()
        print tc4.isA(tc1),0
        print tc2.isA(tc1),1
        print tc1.isA(tc2),0
        print td.isA(ti),0
        print td.isAdaptable(ti),1
        print ti.isAdaptable(td),0
        print tc5.isAdaptable(tc3),0
        print tc3.isAdaptable(tc5),1
        
        n=self.r.createScriptNode("","node1")
        n.setScript("print 'coucou1'")
        n.edAddInputPort("p1",ti)
        n.edAddOutputPort("p1",ti)
        p.edAddChild(n)
        inport=n.getInputPort("p1");
        retex=None
        try:
            inport.edInitXML("<value><intt>5</int></value>")
        except ValueError, ex:
            print "Value Error: ", ex
            retex=ex
        except pilot.Exception,ex:
            print "YACS exception:",ex.what()
            retex=ex.what()
        self.assert_(retex is not None, "exception not raised, or wrong type")
        inport.edInitXML("<value><int>5</int></value>")

        # --- create script node node2
        n2=self.r.createScriptNode("","node2")
        n2.setScript("print 'coucou2'")
        n2.edAddInputPort("p1",ti)
        p.edAddChild(n2)
        # --- end of node

        # --- control link between nodes n and n2
        p.edAddCFLink(n,n2)
        # --- end control link

        # --- datalink between ports p1 of nodes n1 and n2
        p.edAddLink(n.getOutputPort("p1"),n2.getInputPort("p1"))
        # --- end datalink

        n=self.r.createFuncNode("","node3")
        n.setScript("""
        def f():
        print 'coucou3'
        """)
        n.setFname("f")
        p.edAddChild(n)

        n4=self.r.createRefNode("","node4")
        n4.setRef("corbaname:rir:#test.my_context/Echo.Object")
        n4.setMethod("echoDouble")
        n4.edAddInputDataStreamPort("pin",ti)
        n4.edAddOutputDataStreamPort("pout",ti)
        p.edAddChild(n4)
        
        n5=self.r.createRefNode("","node5")
        n5.setRef("corbaname:rir:#test.my_context/Echo.Object")
        n5.setMethod("echoDouble")
        n5.edAddInputDataStreamPort("pin",ti)
        n5.edAddOutputDataStreamPort("pout",ti)
        p.edAddChild(n5)

        p.edAddLink(n4.getOutputDataStreamPort("pout"),n5.getInputDataStreamPort("pin"))

        #n=self.r.createCompoNode("","node5")
        #n.setRef("PYHELLO")
        #n.setMethod("makeBanner")
        #p.edAddChild(n)

        # --- create a bloc with one node
        b=self.r.createBloc("b1")
        p.edAddChild(b)
        
        n=self.r.createScriptNode("","b1.node2")
        n.setScript("print 'coucou2'")
        b.edAddChild(n)
        # --- end bloc

        # --- create a for loop with one node
        lo=self.r.createForLoop("l1")
        p.edAddChild(lo)
        ip=lo.edGetNbOfTimesInputPort()
        ip.edInitInt(3)

        n=self.r.createScriptNode("","l1.node2")
        n.setScript("print 'coucou2'")
        lo.edSetNode(n)
        # --- end loop

        # --- control link between bloc b1 and loop l1
        p.edAddCFLink(b,lo)
        # --- end control link

        # --- create a while loop with one node
        wh=self.r.createWhileLoop("w1")
        p.edAddChild(wh)
        n=self.r.createFuncNode("","w1.node3")
        n.setScript("""
def f():
  print 'coucou3'
  return 0
""")
        n.setFname("f")
        n.edAddOutputPort("p1",ti)
        wh.edSetNode(n)
        cport=wh.edGetConditionPort()
        cport.edInitBool(True)
        # --- end loop
        p.edAddLink(n.getOutputPort("p1"),wh.getInputPort("condition")) #or cport

        # --- create a switch 
        sw=self.r.createSwitch("sw1")
        p.edAddChild(sw)
        n=self.r.createFuncNode("","sw1.node3")
        n.setScript("""
def f():
  print 'case1'
  return 0
""")
        n.setFname("f")
        n.edAddOutputPort("p1",ti)
        sw.edSetNode(1,n)
        n=self.r.createFuncNode("","sw1.node4")
        n.setScript("""
def f():
  print 'default'
  return 0
""")
        n.setFname("f")
        n.edAddOutputPort("p1",ti)
        sw.edSetDefaultNode(n)
        sw.edGetConditionPort().edInitInt(1)
        # --- end switch

        try:
          self.e.RunW(p,0)
        except pilot.Exception,ex:
          print ex.what()
          self.fail(ex)
        
        #self.e.displayDot(p)


if __name__ == '__main__':
  import os
  U = os.getenv('USER')
  f=open("/tmp/" + U + "/UnitTestsResult", 'a')
  f.write("  --- TEST src/yacsloader: testEdit.py\n")
  suite = unittest.makeSuite(TestEdit)
  result=unittest.TextTestRunner(f, descriptions=1, verbosity=1).run(suite)
  f.close()
  sys.exit(not result.wasSuccessful())
