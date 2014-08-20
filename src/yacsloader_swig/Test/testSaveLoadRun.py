# Copyright (C) 2006-2014  CEA/DEN, EDF R&D
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
import pilot
import SALOMERuntime
import loader

import datetime

class TestSaveLoadRun(unittest.TestCase):
  def setUp(self):
    SALOMERuntime.RuntimeSALOME.setRuntime()
    self.r=SALOMERuntime.getSALOMERuntime()
    pass

  def test0(self):
    """First test of HP Container no loop here only the 3 sorts of python nodes (the Distributed is it still used and useful ?) """
    fname="TestSaveLoadRun0.xml"
    nbOfNodes=8
    sqrtOfNumberOfTurn=1000 # 3000 -> 3.2s/Node, 1000 -> 0.1s/Node
    l=loader.YACSLoader()
    p=self.r.createProc("prTest0")
    td=p.createType("double","double")
    ti=p.createType("int","int")
    cont=p.createContainer("gg","HPSalome")
    cont.setSizeOfPool(4)
    cont.setProperty("name","localhost")
    cont.setProperty("hostname","localhost")
    script0="""
def ff(nb,dbg):
    from math import cos
    import datetime
    
    ref=datetime.datetime.now()
    t=0. ; pas=1./float(nb)
    for i in xrange(nb):
        for j in xrange(nb):
            x=j*pas
            t+=1.+cos(1.*(x*3.14159))
            pass
        pass
    print "coucou from script0-%i  -> %s"%(dbg,str(datetime.datetime.now()-ref))
    return t
"""
    script1="""
from math import cos
import datetime
ref=datetime.datetime.now()
o2=0. ; pas=1./float(i1)
for i in xrange(i1):
  for j in xrange(i1):
    x=j*pas
    o2+=1.+cos(1.*(x*3.14159))
    pass
print "coucou from script1-%i  -> %s"%(dbg,str(datetime.datetime.now()-ref))
"""
    for i in xrange(nbOfNodes):
      node0=self.r.createFuncNode("DistPython","node%i"%(i))
      p.edAddChild(node0)
      node0.setFname("ff")
      node0.setContainer(cont)
      node0.setScript(script0)
      nb=node0.edAddInputPort("nb",ti) ; nb.edInitInt(sqrtOfNumberOfTurn)
      dbg=node0.edAddInputPort("dbg",ti) ; dbg.edInitInt(i+1)
      out0=node0.edAddOutputPort("s",td)
      #
      nodeMiddle=self.r.createFuncNode("Salome","node%i_1"%(i))
      p.edAddChild(nodeMiddle)
      p.edAddCFLink(node0,nodeMiddle)
      nodeMiddle.setFname("ff")
      nodeMiddle.setContainer(cont)
      nodeMiddle.setScript(script0)
      nb=nodeMiddle.edAddInputPort("nb",ti) ; nb.edInitInt(sqrtOfNumberOfTurn)
      dbg=nodeMiddle.edAddInputPort("dbg",ti) ; dbg.edInitInt(i+1)
      out0=nodeMiddle.edAddOutputPort("s",td)
      nodeMiddle.setExecutionMode("remote")
      #
      nodeEnd=self.r.createScriptNode("Salome","node%i_2"%(i+1))
      p.edAddChild(nodeEnd)
      p.edAddCFLink(nodeMiddle,nodeEnd)
      nodeEnd.setContainer(cont)
      nodeEnd.setScript(script1)
      i1=nodeEnd.edAddInputPort("i1",ti) ; i1.edInitInt(sqrtOfNumberOfTurn)
      dbg=nodeEnd.edAddInputPort("dbg",ti) ; dbg.edInitInt(i)
      o2=nodeEnd.edAddOutputPort("o2",td)
      nodeEnd.setExecutionMode("remote")
      pass
    p.saveSchema(fname)
    p=l.load(fname)
    ex=pilot.ExecutorSwig()
    self.assertEqual(p.getState(),pilot.READY)
    st=datetime.datetime.now()
    # 1st exec
    ex.RunW(p,0)
    print "Time spend of test0 to run 1st %s"%(str(datetime.datetime.now()-st))
    self.assertEqual(p.getState(),pilot.DONE)
    # 2nd exec using the same already launched remote python interpreters
    st=datetime.datetime.now()
    ex.RunW(p,0)
    print "Time spend of test0 to run 2nd %s"%(str(datetime.datetime.now()-st))
    self.assertEqual(p.getState(),pilot.DONE)
    # 3rd exec using the same already launched remote python interpreters
    st=datetime.datetime.now()
    ex.RunW(p,0)
    print "Time spend of test0 to run 3rd %s"%(str(datetime.datetime.now()-st))
    self.assertEqual(p.getState(),pilot.DONE)
    pass

  def test1(self):
    """ HP Container again like test0 but the initialization key of HPContainer is used here."""
    fname="TestSaveLoadRun1.xml"
    nbOfNodes=8
    sqrtOfNumberOfTurn=1000 # 3000 -> 3.2s/Node, 1000 -> 0.1s/Node
    l=loader.YACSLoader()
    p=self.r.createProc("prTest1")
    td=p.createType("double","double")
    ti=p.createType("int","int")
    cont=p.createContainer("gg","HPSalome")
    cont.setSizeOfPool(4)
    cont.setProperty("InitializeScriptKey","aa=123.456")
    cont.setProperty("name","localhost")
    cont.setProperty("hostname","localhost")
    script0="""
def ff(nb,dbg):
    from math import cos
    import datetime
    
    ref=datetime.datetime.now()
    t=0. ; pas=1./float(nb)
    for i in xrange(nb):
        for j in xrange(nb):
            x=j*pas
            t+=1.+cos(1.*(x*3.14159))
            pass
        pass
    print "coucou from script0-%i  -> %s"%(dbg,str(datetime.datetime.now()-ref))
    return t
"""
    # here in script1 aa is refered ! aa will exist thanks to HPCont Init Script
    script1="""
from math import cos
import datetime
ref=datetime.datetime.now()
o2=0. ; pas=1./float(i1)
for i in xrange(i1):
  for j in xrange(i1):
    x=j*pas
    o2+=1.+cos(1.*(x*3.14159))
    pass
print "coucou %lf from script1-%i  -> %s"%(aa,dbg,str(datetime.datetime.now()-ref))
aa+=1.
"""
    #
    for i in xrange(nbOfNodes):
      nodeMiddle=self.r.createFuncNode("Salome","node%i_1"%(i)) # PyFuncNode remote
      p.edAddChild(nodeMiddle)
      nodeMiddle.setFname("ff")
      nodeMiddle.setContainer(cont)
      nodeMiddle.setScript(script0)
      nb=nodeMiddle.edAddInputPort("nb",ti) ; nb.edInitInt(sqrtOfNumberOfTurn)
      dbg=nodeMiddle.edAddInputPort("dbg",ti) ; dbg.edInitInt(i+1)
      out0=nodeMiddle.edAddOutputPort("s",td)
      nodeMiddle.setExecutionMode("remote")
      #
      nodeEnd=self.r.createScriptNode("Salome","node%i_2"%(i+1)) # PythonNode remote
      p.edAddChild(nodeEnd)
      p.edAddCFLink(nodeMiddle,nodeEnd)
      nodeEnd.setContainer(cont)
      nodeEnd.setScript(script1)
      i1=nodeEnd.edAddInputPort("i1",ti) ; i1.edInitInt(sqrtOfNumberOfTurn)
      dbg=nodeEnd.edAddInputPort("dbg",ti) ; dbg.edInitInt(i)
      o2=nodeEnd.edAddOutputPort("o2",td)
      nodeEnd.setExecutionMode("remote")
      pass
    #
    p.saveSchema(fname)
    p=l.load(fname)
    self.assertEqual(p.edGetDirectDescendants()[0].getContainer().getProperty("InitializeScriptKey"),"aa=123.456")
    # 1st exec
    ex=pilot.ExecutorSwig()
    self.assertEqual(p.getState(),pilot.READY)
    st=datetime.datetime.now()
    ex.RunW(p,0)
    print "Time spend of test1 to 1st run %s"%(str(datetime.datetime.now()-st))
    self.assertEqual(p.getState(),pilot.DONE)
    # 2nd exec
    st=datetime.datetime.now()
    ex.RunW(p,0)
    print "Time spend of test1 to 2nd run %s"%(str(datetime.datetime.now()-st))
    self.assertEqual(p.getState(),pilot.DONE)
    # 3rd exec
    st=datetime.datetime.now()
    ex.RunW(p,0)
    print "Time spend of test1 to 3rd run %s"%(str(datetime.datetime.now()-st))
    self.assertEqual(p.getState(),pilot.DONE)
    pass

  def test2(self):
    """ Test on HP Containers in foreach context."""
    script0="""def ff():
    global aa
    print "%%lf - %%s"%%(aa,str(my_container))
    return 16*[%i],0
"""
    script1="""from math import cos
import datetime
ref=datetime.datetime.now()
o2=0. ; pas=1./float(i1)
for i in xrange(i1):
  for j in xrange(i1):
    x=j*pas
    o2+=1.+cos(1.*(x*3.14159))
    pass
print "coucou %lf from script  -> %s"%(aa,str(datetime.datetime.now()-ref))
aa+=1.
o3=0
"""
    script2="""o9=sum(i8)
"""
    fname="TestSaveLoadRun2.xml"
    nbOfNodes=8
    sqrtOfNumberOfTurn=1000 # 3000 -> 3.2s/Node, 1000 -> 0.1s/Node
    l=loader.YACSLoader()
    p=self.r.createProc("prTest1")
    td=p.createType("double","double")
    ti=p.createType("int","int")
    tdi=p.createSequenceTc("seqint","seqint",ti)
    tdd=p.createSequenceTc("seqdouble","seqdouble",td)
    cont=p.createContainer("gg","HPSalome")
    cont.setSizeOfPool(4)
    cont.setProperty("InitializeScriptKey","aa=123.456")
    cont.setProperty("name","localhost")
    cont.setProperty("hostname","localhost")
    #
    node0=self.r.createFuncNode("Salome","PyFunction0") # PyFuncNode remote
    p.edAddChild(node0)
    node0.setFname("ff")
    node0.setContainer(cont)
    node0.setScript(script0%(sqrtOfNumberOfTurn))
    out0_0=node0.edAddOutputPort("o1",tdi)
    out1_0=node0.edAddOutputPort("o2",ti)
    node0.setExecutionMode("remote")
    #
    node1=self.r.createForEachLoop("node1",ti)
    p.edAddChild(node1)
    p.edAddCFLink(node0,node1)
    p.edAddLink(out0_0,node1.edGetSeqOfSamplesPort())
    node1.edGetNbOfBranchesPort().edInitInt(8)
    #
    node2=self.r.createScriptNode("Salome","PyScript3")
    node1.edAddChild(node2)
    node2.setContainer(cont)
    node2.setScript(script1)
    i1=node2.edAddInputPort("i1",ti)
    p.edAddLink(node1.edGetSamplePort(),i1)
    out0_2=node2.edAddOutputPort("o2",td)
    out1_2=node2.edAddOutputPort("o3",ti)
    node2.setExecutionMode("remote")
    #
    node3=self.r.createScriptNode("Salome","PyScript7")
    p.edAddChild(node3)
    node3.setScript(script2)
    p.edAddCFLink(node1,node3)
    i8=node3.edAddInputPort("i8",tdd)
    o9=node3.edAddOutputPort("o9",td)
    p.edAddLink(out0_2,i8)
    #
    p.saveSchema(fname)
    p=l.load(fname)
    o9=p.getChildByName("PyScript7").getOutputPort("o9")
    self.assertTrue(len(p.edGetDirectDescendants()[1].getChildByName("PyScript3").getContainer().getProperty("InitializeScriptKey"))!=0)
    # 1st exec
    refExpected=16016013.514623128
    ex=pilot.ExecutorSwig()
    self.assertEqual(p.getState(),pilot.READY)
    st=datetime.datetime.now()
    ex.RunW(p,0)
    print "Time spend of test2 to 1st run %s"%(str(datetime.datetime.now()-st))
    self.assertEqual(p.getState(),pilot.DONE)
    self.assertAlmostEqual(refExpected,o9.getPyObj(),5)
    # 2nd exec
    st=datetime.datetime.now()
    ex.RunW(p,0)
    print "Time spend of test2 to 2nd run %s"%(str(datetime.datetime.now()-st))
    self.assertEqual(p.getState(),pilot.DONE)
    self.assertAlmostEqual(refExpected,o9.getPyObj(),5)
    # 3rd exec
    st=datetime.datetime.now()
    ex.RunW(p,0)
    print "Time spend of test2 to 3rd run %s"%(str(datetime.datetime.now()-st))
    self.assertEqual(p.getState(),pilot.DONE)
    self.assertAlmostEqual(refExpected,o9.getPyObj(),5)
    pass

  def test3(self):
    """ Test that focuses on parallel load of containers."""
    script0="""def ff():
    global aa
    print "%%lf - %%s"%%(aa,str(my_container))
    return 100*[%i],0
"""
    script1="""from math import cos
import datetime
ref=datetime.datetime.now()
o2=0. ; pas=1./float(i1)
for i in xrange(i1):
  for j in xrange(i1):
    x=j*pas
    o2+=1.+cos(1.*(x*3.14159))
    pass
print "coucou %lf from script  -> %s"%(aa,str(datetime.datetime.now()-ref))
aa+=1.
o3=0
"""
    script2="""o9=sum(i8)
"""
    fname="TestSaveLoadRun3.xml"
    nbOfNodes=8
    sqrtOfNumberOfTurn=10
    l=loader.YACSLoader()
    p=self.r.createProc("prTest1")
    td=p.createType("double","double")
    ti=p.createType("int","int")
    tdi=p.createSequenceTc("seqint","seqint",ti)
    tdd=p.createSequenceTc("seqdouble","seqdouble",td)
    cont=p.createContainer("gg","HPSalome")
    cont.setSizeOfPool(8)
    cont.setProperty("InitializeScriptKey","aa=123.456")
    cont.setProperty("name","localhost")
    cont.setProperty("hostname","localhost")
    #
    node0=self.r.createFuncNode("Salome","PyFunction0") # PyFuncNode remote
    p.edAddChild(node0)
    node0.setFname("ff")
    node0.setContainer(cont)
    node0.setScript(script0%(sqrtOfNumberOfTurn))
    out0_0=node0.edAddOutputPort("o1",tdi)
    out1_0=node0.edAddOutputPort("o2",ti)
    node0.setExecutionMode("remote")
    #
    node1=self.r.createForEachLoop("node1",ti)
    p.edAddChild(node1)
    p.edAddCFLink(node0,node1)
    p.edAddLink(out0_0,node1.edGetSeqOfSamplesPort())
    node1.edGetNbOfBranchesPort().edInitInt(16)
    #
    node2=self.r.createScriptNode("Salome","PyScript3")
    node1.edAddChild(node2)
    node2.setContainer(cont)
    node2.setScript(script1)
    i1=node2.edAddInputPort("i1",ti)
    p.edAddLink(node1.edGetSamplePort(),i1)
    out0_2=node2.edAddOutputPort("o2",td)
    out1_2=node2.edAddOutputPort("o3",ti)
    node2.setExecutionMode("remote")
    #
    node3=self.r.createScriptNode("Salome","PyScript7")
    p.edAddChild(node3)
    node3.setScript(script2)
    p.edAddCFLink(node1,node3)
    i8=node3.edAddInputPort("i8",tdd)
    o9=node3.edAddOutputPort("o9",td)
    p.edAddLink(out0_2,i8)
    #
    p.saveSchema(fname)
    p=l.load(fname)
    o9=p.getChildByName("PyScript7").getOutputPort("o9")
    self.assertTrue(len(p.edGetDirectDescendants()[1].getChildByName("PyScript3").getContainer().getProperty("InitializeScriptKey"))!=0)
    # 1st exec
    refExpected=11000.008377058712
    ex=pilot.ExecutorSwig()
    self.assertEqual(p.getState(),pilot.READY)
    st=datetime.datetime.now()
    ex.RunW(p,0)
    print "Time spend of test3 to 1st run %s"%(str(datetime.datetime.now()-st))
    self.assertEqual(p.getState(),pilot.DONE)
    self.assertAlmostEqual(refExpected,o9.getPyObj(),5)
    # 2nd exec
    st=datetime.datetime.now()
    ex.RunW(p,0)
    print "Time spend of test3 to 2nd run %s"%(str(datetime.datetime.now()-st))
    self.assertEqual(p.getState(),pilot.DONE)
    self.assertAlmostEqual(refExpected,o9.getPyObj(),5)
    # 3rd exec
    st=datetime.datetime.now()
    ex.RunW(p,0)
    print "Time spend of test3 to 3rd run %s"%(str(datetime.datetime.now()-st))
    self.assertEqual(p.getState(),pilot.DONE)
    self.assertAlmostEqual(refExpected,o9.getPyObj(),5)
    pass
  pass

import os
U = os.getenv('USER')
f=open("/tmp/" + U + "/UnitTestsResult", 'a')
f.write("  --- TEST src/yacsloader: testSaveLoadRun.py\n")
suite = unittest.makeSuite(TestSaveLoadRun)
result=unittest.TextTestRunner(f, descriptions=1, verbosity=1).run(suite)
f.close()
sys.exit(not result.wasSuccessful())
