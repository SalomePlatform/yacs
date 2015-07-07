# Copyright (C) 2006-2015  CEA/DEN, EDF R&D
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
  
  def test4(self):
    """Non regression test of multi pyScriptNode, pyFuncNode sharing the same HPContainer instance."""
    fname="TestSaveLoadRun4.xml"
    script1="""nb=7
ii=0
o1=nb*[None]
for i in xrange(nb):
    tmp=(i+10)*[None]
    for j in xrange(i+10):
        tmp[j]=ii
        ii+=1
        pass
    o1[i]=tmp
    pass
"""
    l=loader.YACSLoader()
    ex=pilot.ExecutorSwig()
    p=self.r.createProc("pr")
    cont=p.createContainer("gg","HPSalome")
    cont.setSizeOfPool(10)
    td=p.createType("int","int")
    td2=p.createSequenceTc("seqint","seqint",td)
    td3=p.createSequenceTc("seqintvec","seqintvec",td2)
    node1=self.r.createScriptNode("","node1")
    node1.setScript(script1)
    o1=node1.edAddOutputPort("o1",td3)
    p.edAddChild(node1)
    #
    node2=self.r.createForEachLoop("node2",td2)
    p.edAddChild(node2)
    p.edAddCFLink(node1,node2)
    p.edAddLink(o1,node2.edGetSeqOfSamplesPort())
    node2.edGetNbOfBranchesPort().edInitInt(2)
    node20=self.r.createBloc("node20")
    node2.edAddChild(node20)
    node200=self.r.createForEachLoop("node200",td)
    node20.edAddChild(node200)
    node200.edGetNbOfBranchesPort().edInitInt(10)
    p.edAddLink(node2.edGetSamplePort(),node200.edGetSeqOfSamplesPort())
    node2000=self.r.createScriptNode("","node2000")
    node2000.setContainer(cont)
    node2000.setExecutionMode("remote")
    node200.edAddChild(node2000)
    i5=node2000.edAddInputPort("i5",td)
    o6=node2000.edAddOutputPort("o6",td)
    node2000.setScript("o6=2+i5")
    p.edAddLink(node200.edGetSamplePort(),i5)
    #
    node3=self.r.createForEachLoop("node3",td)
    p.edAddChild(node3)
    p.edAddCFLink(node2,node3)
    p.edAddLink(o6,node3.edGetSeqOfSamplesPort())
    node3.edGetNbOfBranchesPort().edInitInt(2)
    node30=self.r.createBloc("node30")
    node3.edAddChild(node30)
    node300=self.r.createForEachLoop("node300",td)
    node30.edAddChild(node300)
    node300.edGetNbOfBranchesPort().edInitInt(10)
    p.edAddLink(node3.edGetSamplePort(),node300.edGetSeqOfSamplesPort())
    node3000=self.r.createScriptNode("","node3000")
    node3000.setContainer(cont)
    node3000.setExecutionMode("remote")
    node300.edAddChild(node3000)
    i14=node3000.edAddInputPort("i14",td)
    o15=node3000.edAddOutputPort("o15",td)
    node3000.setScript("o15=3+i14")
    p.edAddLink(node300.edGetSamplePort(),i14)
    #
    node4=self.r.createScriptNode("","node4")
    node4.setScript("o9=i8")
    p.edAddChild(node4)
    i8=node4.edAddInputPort("i8",td3)
    o9=node4.edAddOutputPort("o9",td3)
    p.edAddCFLink(node3,node4)
    p.edAddLink(o15,i8)
    p.saveSchema(fname)
    p=l.load(fname)
    ex = pilot.ExecutorSwig()
    self.assertEqual(p.getState(),pilot.READY)
    ex.RunW(p,0)
    self.assertEqual(p.getState(),pilot.DONE)
    zeResu=p.getChildByName("node4").getOutputPort("o9").get()
    self.assertEqual(zeResu,[[5,6,7,8,9,10,11,12,13,14],[15,16,17,18,19,20,21,22,23,24,25],[26,27,28,29,30,31,32,33,34,35,36,37],[38,39,40,41,42,43,44,45,46,47,48,49,50],[51,52,53,54,55,56,57,58,59,60,61,62,63,64],[65,66,67,68,69,70,71,72,73,74,75,76,77,78,79], [80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95]])
    pass

  def test5(self):
    """Non regression test 2 of multi pyNode, pyFuncNode sharing the same HPContainer instance."""
    fname="TestSaveLoadRun5.xml"
    script1="""nb=7
ii=0
o1=nb*[None]
for i in xrange(nb):
    tmp=(i+10)*[None]
    for j in xrange(i+10):
        tmp[j]=ii
        ii+=1
        pass
    o1[i]=tmp
    pass
"""
    l=loader.YACSLoader()
    ex=pilot.ExecutorSwig()
    p=self.r.createProc("pr")
    cont=p.createContainer("gg","HPSalome")
    cont.setSizeOfPool(10)
    td=p.createType("int","int")
    td2=p.createSequenceTc("seqint","seqint",td)
    td3=p.createSequenceTc("seqintvec","seqintvec",td2)
    node1=self.r.createScriptNode("","node1")
    node1.setScript(script1)
    o1=node1.edAddOutputPort("o1",td3)
    p.edAddChild(node1)
    #
    node2=self.r.createForEachLoop("node2",td2)
    p.edAddChild(node2)
    p.edAddCFLink(node1,node2)
    p.edAddLink(o1,node2.edGetSeqOfSamplesPort())
    node2.edGetNbOfBranchesPort().edInitInt(2)
    node20=self.r.createBloc("node20")
    node2.edAddChild(node20)
    node200=self.r.createForEachLoop("node200",td)
    node20.edAddChild(node200)
    node200.edGetNbOfBranchesPort().edInitInt(10)
    p.edAddLink(node2.edGetSamplePort(),node200.edGetSeqOfSamplesPort())
    node2000=self.r.createFuncNode("Salome","node2000")
    node2000.setFname("ff")
    node2000.setContainer(cont)
    node2000.setExecutionMode("remote")
    node200.edAddChild(node2000)
    i5=node2000.edAddInputPort("i5",td)
    o6=node2000.edAddOutputPort("o6",td)
    node2000.setScript("def ff(x):\n  return 2+x")
    p.edAddLink(node200.edGetSamplePort(),i5)
    #
    node3=self.r.createForEachLoop("node3",td)
    p.edAddChild(node3)
    p.edAddCFLink(node2,node3)
    p.edAddLink(o6,node3.edGetSeqOfSamplesPort())
    node3.edGetNbOfBranchesPort().edInitInt(2)
    node30=self.r.createBloc("node30")
    node3.edAddChild(node30)
    node300=self.r.createForEachLoop("node300",td)
    node30.edAddChild(node300)
    node300.edGetNbOfBranchesPort().edInitInt(10)
    p.edAddLink(node3.edGetSamplePort(),node300.edGetSeqOfSamplesPort())
    node3000=self.r.createFuncNode("Salome","node3000")
    node3000.setFname("ff")
    node3000.setContainer(cont)
    node3000.setExecutionMode("remote")
    node300.edAddChild(node3000)
    i14=node3000.edAddInputPort("i14",td)
    o15=node3000.edAddOutputPort("o15",td)
    node3000.setScript("def ff(x):\n  return 3+x")
    p.edAddLink(node300.edGetSamplePort(),i14)
    #
    node4=self.r.createScriptNode("","node4")
    node4.setScript("o9=i8")
    p.edAddChild(node4)
    i8=node4.edAddInputPort("i8",td3)
    o9=node4.edAddOutputPort("o9",td3)
    p.edAddCFLink(node3,node4)
    p.edAddLink(o15,i8)
    p.saveSchema(fname)
    p=l.load(fname)
    ex = pilot.ExecutorSwig()
    self.assertEqual(p.getState(),pilot.READY)
    ex.RunW(p,0)
    self.assertEqual(p.getState(),pilot.DONE)
    zeResu=p.getChildByName("node4").getOutputPort("o9").get()
    self.assertEqual(zeResu,[[5,6,7,8,9,10,11,12,13,14],[15,16,17,18,19,20,21,22,23,24,25],[26,27,28,29,30,31,32,33,34,35,36,37],[38,39,40,41,42,43,44,45,46,47,48,49,50],[51,52,53,54,55,56,57,58,59,60,61,62,63,64],[65,66,67,68,69,70,71,72,73,74,75,76,77,78,79], [80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95]])
    pass

  def test6(self):
    fname="test6.xml"
    p=self.r.createProc("prTest0")
    td=p.createType("double","double")
    ti=p.createType("int","int")
    tsi=p.createSequenceTc("seqint","seqint",ti)
    tsd=p.createSequenceTc("seqdbl","seqdbl",td)
    n0=self.r.createScriptNode("","n0")
    o0=n0.edAddOutputPort("o0",tsi)
    n0.setScript("o0=[3,6,8,9,-2,5]")
    p.edAddChild(n0)
    n1=self.r.createForEachLoop("n1",ti)
    n10=self.r.createScriptNode("","n10")
    n1.edAddChild(n10)
    n10.setScript("o2=2*i1")
    i1=n10.edAddInputPort("i1",ti)
    o2=n10.edAddOutputPort("o2",ti)
    p.edAddChild(n1)
    p.edAddLink(o0,n1.edGetSeqOfSamplesPort())
    p.edAddLink(n1.edGetSamplePort(),i1)
    p.edAddCFLink(n0,n1)
    n1.edGetNbOfBranchesPort().edInitPy(1)
    n2=self.r.createScriptNode("","n2")
    n2.setScript("o4=i3")
    i3=n2.edAddInputPort("i3",tsi)
    o4=n2.edAddOutputPort("o4",tsi)
    n2.setScript("o4=i3")
    p.edAddChild(n2)
    p.edAddCFLink(n1,n2)
    p.edAddLink(o2,i3)
    p.saveSchema(fname)
    #
    l=loader.YACSLoader()
    p=l.load(fname)
    n1=p.getChildByName("n1")
    ex=pilot.ExecutorSwig()
    #
    ex.RunW(p,0)
    #
    self.assertEqual(n1.getState(),pilot.DONE)
    n1.edGetSeqOfSamplesPort().getPyObj()
    a,b,c=n1.getPassedResults(ex)
    self.assertEqual(a,range(6))
    self.assertEqual([elt.getPyObj() for elt in b],[[6L, 12L, 16L, 18L, -4L, 10L]])
    self.assertEqual(c,['n10.o2'])
    pass

  def test7(self):
    fname="test7.xml"
    p=self.r.createProc("prTest1")
    cont=p.createContainer("gg","Salome")
    cont.setProperty("name","localhost")
    cont.setProperty("hostname","localhost")
    cont.setProperty("type","multi")
    td=p.createType("double","double")
    ti=p.createType("int","int")
    tsi=p.createSequenceTc("seqint","seqint",ti)
    tsd=p.createSequenceTc("seqdbl","seqdbl",td)
    n0=self.r.createScriptNode("","n0")
    o0=n0.edAddOutputPort("o0",tsi)
    n0.setScript("o0=[3,6,8,9,-2,5]")
    p.edAddChild(n0)
    n1=self.r.createForEachLoop("n1",ti)
    n10=self.r.createScriptNode("","n10")
    n10.setExecutionMode("remote")
    n10.setContainer(cont)
    n1.edAddChild(n10)
    n10.setScript("""
import time
if i1==9:
  time.sleep(2)
  raise Exception("Simulated error !")
else:
  o2=2*i1
""")
    i1=n10.edAddInputPort("i1",ti)
    o2=n10.edAddOutputPort("o2",ti)
    p.edAddChild(n1)
    p.edAddLink(o0,n1.edGetSeqOfSamplesPort())
    p.edAddLink(n1.edGetSamplePort(),i1)
    p.edAddCFLink(n0,n1)
    n1.edGetNbOfBranchesPort().edInitPy(1)
    n2=self.r.createScriptNode("","n2")
    n2.setScript("o4=i3")
    i3=n2.edAddInputPort("i3",tsi)
    o4=n2.edAddOutputPort("o4",tsi)
    n2.setScript("o4=i3")
    p.edAddChild(n2)
    p.edAddCFLink(n1,n2)
    p.edAddLink(o2,i3)
    p.saveSchema(fname)
    #
    l=loader.YACSLoader()
    p=l.load(fname)
    n1=p.getChildByName("n1")
    ex=pilot.ExecutorSwig()
    #
    ex.RunW(p,0)
    #
    self.assertEqual(n1.getState(),pilot.FAILED)
    n1.edGetSeqOfSamplesPort().getPyObj()
    a,b,c=n1.getPassedResults(ex)
    self.assertEqual(a,range(3))
    self.assertEqual([elt.getPyObj() for elt in b],[[6L,12L,16L]])
    self.assertEqual(c,['n10.o2'])
    pass

  def test8(self):
    from datetime import datetime
    fname="test8.xml"
    p=self.r.createProc("prTest2")
    cont=p.createContainer("gg","Salome")
    cont.setProperty("name","localhost")
    cont.setProperty("hostname","localhost")
    cont.setProperty("type","multi")
    td=p.createType("double","double")
    ti=p.createType("int","int")
    tsi=p.createSequenceTc("seqint","seqint",ti)
    tsd=p.createSequenceTc("seqdbl","seqdbl",td)
    n0=self.r.createScriptNode("","n0")
    o0=n0.edAddOutputPort("o0",tsi)
    n0.setScript("o0=[3,6,8,9,-2,5]")
    p.edAddChild(n0)
    n1=self.r.createForEachLoop("n1",ti)
    n10=self.r.createScriptNode("","n10")
    n10.setExecutionMode("remote")
    n10.setContainer(cont)
    n1.edAddChild(n10)
    n10.setScript("""
import time
if i1==9:
  raise Exception("Simulated error !")
else:
  time.sleep(0.1)
  o2=2*i1
""")
    i1=n10.edAddInputPort("i1",ti)
    o2=n10.edAddOutputPort("o2",ti)
    p.edAddChild(n1)
    p.edAddLink(o0,n1.edGetSeqOfSamplesPort())
    p.edAddLink(n1.edGetSamplePort(),i1)
    p.edAddCFLink(n0,n1)
    n1.edGetNbOfBranchesPort().edInitPy(2)
    n2=self.r.createScriptNode("","n2")
    n2.setScript("o4=i3")
    i3=n2.edAddInputPort("i3",tsi)
    o4=n2.edAddOutputPort("o4",tsi)
    n2.setScript("o4=i3")
    p.edAddChild(n2)
    p.edAddCFLink(n1,n2)
    p.edAddLink(o2,i3)
    p.saveSchema(fname)
    #
    l=loader.YACSLoader()
    p=l.load(fname)
    n1=p.getChildByName("n1")
    ex=pilot.ExecutorSwig()
    ex.setKeepGoingProperty(True)
    #
    startt=datetime.now()
    ex.RunW(p,0)
    t0=datetime.now()-startt
    #
    self.assertEqual(n1.getState(),pilot.FAILED)
    n1.edGetSeqOfSamplesPort().getPyObj()
    a,b,c=n1.getPassedResults(ex)
    self.assertEqual(a,[0,1,2,4,5])
    self.assertEqual([elt.getPyObj() for elt in b],[[6L,12L,16L,-4L,10L]])
    self.assertEqual(c,['n10.o2'])
    
    p.getChildByName("n1").getChildByName("n10").setScript("""
import time
if i1==3:
  time.sleep(2)
  raise Exception("Simulated error !")
else:
  o2=2*i1
""")
    ex=pilot.ExecutorSwig()
    ex.setKeepGoingProperty(True)
    #
    startt=datetime.now()
    ex.RunW(p,0)
    t1=datetime.now()-startt
    #
    self.assertEqual(n1.getState(),pilot.FAILED)
    n1.edGetSeqOfSamplesPort().getPyObj()
    a,b,c=n1.getPassedResults(ex)
    self.assertEqual(a,[1,2,3,4,5])
    self.assertEqual([elt.getPyObj() for elt in b],[[12L,16L,18L,-4L,10L]])
    self.assertEqual(c,['n10.o2'])
    pass

  def test9(self):
    """ Test of assignation of already computed values for foreach node."""
    fname="test9.xml"
    from datetime import datetime
    p=self.r.createProc("prTest2")
    cont=p.createContainer("gg","Salome")
    cont.setProperty("name","localhost")
    cont.setProperty("hostname","localhost")
    cont.setProperty("type","multi")
    td=p.createType("double","double")
    ti=p.createType("int","int")
    tsi=p.createSequenceTc("seqint","seqint",ti)
    tsd=p.createSequenceTc("seqdbl","seqdbl",td)
    n0=self.r.createScriptNode("","n0")
    o0=n0.edAddOutputPort("o0",tsi)
    n0.setScript("o0=[3,6,8,9,-2,5]")
    p.edAddChild(n0)
    n1=self.r.createForEachLoop("n1",ti)
    n10=self.r.createScriptNode("","n10")
    n10.setExecutionMode("remote")
    n10.setContainer(cont)
    n1.edAddChild(n10)
    n10.setScript("""
import time
if i1==9:
  raise Exception("Simulated error !")
else:
  time.sleep(0.1)
  o2=2*i1
""")
    i1=n10.edAddInputPort("i1",ti)
    o2=n10.edAddOutputPort("o2",ti)
    p.edAddChild(n1)
    p.edAddLink(o0,n1.edGetSeqOfSamplesPort())
    p.edAddLink(n1.edGetSamplePort(),i1)
    p.edAddCFLink(n0,n1)
    n1.edGetNbOfBranchesPort().edInitPy(2)
    n2=self.r.createScriptNode("","n2")
    n2.setScript("o4=i3")
    i3=n2.edAddInputPort("i3",tsi)
    o4=n2.edAddOutputPort("o4",tsi)
    n2.setScript("o4=i3")
    p.edAddChild(n2)
    p.edAddCFLink(n1,n2)
    p.edAddLink(o2,i3)
    p.saveSchema(fname)
    #
    l=loader.YACSLoader()
    p=l.load(fname)
    n1=p.getChildByName("n1")
    ex=pilot.ExecutorSwig()
    ex.setKeepGoingProperty(True)
    #
    startt=datetime.now()
    ex.RunW(p,0)
    t0=datetime.now()-startt
    #
    self.assertEqual(p.getState(),pilot.FAILED)
    self.assertEqual(n1.getState(),pilot.FAILED)
    n1.edGetSeqOfSamplesPort().getPyObj()
    a,b,c=n1.getPassedResults(ex)
    self.assertEqual(a,[0,1,2,4,5])
    self.assertEqual([elt.getPyObj() for elt in b],[[6L,12L,16L,-4L,10L]])
    self.assertEqual(c,['n10.o2'])
    
    p.getChildByName("n1").getChildByName("n10").setScript("""
import time
time.sleep(2)
o2=7*i1
""")
    ex=pilot.ExecutorSwig()
    ex.setKeepGoingProperty(True)
    p.getChildByName("n1").assignPassedResults(a,b,c)
    #
    startt=datetime.now()
    ex.RunW(p,0)
    t1=datetime.now()-startt
    #
    self.assertEqual(n1.getState(),pilot.DONE)
    self.assertEqual(p.getState(),pilot.DONE)
    self.assertEqual(p.getChildByName("n2").getOutputPort("o4").getPyObj(),[6L,12L,16L,63L,-4L,10L])
    pass

  def test10(self):
    fname="test10.xml"
    from datetime import datetime
    p=self.r.createProc("prTest2")
    cont=p.createContainer("gg","Salome")
    cont.setProperty("name","localhost")
    cont.setProperty("hostname","localhost")
    cont.setProperty("type","multi")
    td=p.createType("double","double")
    ti=p.createType("int","int")
    tsi=p.createSequenceTc("seqint","seqint",ti)
    tsd=p.createSequenceTc("seqdbl","seqdbl",td)
    n0=self.r.createScriptNode("","n0")
    o0=n0.edAddOutputPort("o0",tsi)
    n0.setScript("o0=[ 3*elt for elt in range(12) ]")
    p.edAddChild(n0)
    n1=self.r.createForEachLoop("n1",ti)
    n10=self.r.createScriptNode("","n10")
    n10.setExecutionMode("remote")
    n10.setContainer(cont)
    n1.edAddChild(n10)
    n10.setScript("""
import time
if i1%2==0:
  raise Exception("Simulated error !")
else:
  time.sleep(0.1)
  o2=4*i1
""")
    i1=n10.edAddInputPort("i1",ti)
    o2=n10.edAddOutputPort("o2",ti)
    p.edAddChild(n1)
    p.edAddLink(o0,n1.edGetSeqOfSamplesPort())
    p.edAddLink(n1.edGetSamplePort(),i1)
    p.edAddCFLink(n0,n1)
    n1.edGetNbOfBranchesPort().edInitPy(2)
    n2=self.r.createScriptNode("","n2")
    n2.setScript("o4=i3")
    i3=n2.edAddInputPort("i3",tsi)
    o4=n2.edAddOutputPort("o4",tsi)
    n2.setScript("o4=i3")
    p.edAddChild(n2)
    p.edAddCFLink(n1,n2)
    p.edAddLink(o2,i3)
    p.saveSchema(fname)
    #
    l=loader.YACSLoader()
    p=l.load(fname)
    n1=p.getChildByName("n1")
    ex=pilot.ExecutorSwig()
    ex.setKeepGoingProperty(True)
    #
    startt=datetime.now()
    ex.RunW(p,0)
    t0=datetime.now()-startt
    #
    self.assertEqual(p.getState(),pilot.FAILED)
    self.assertEqual(n1.getState(),pilot.FAILED)
    n1.edGetSeqOfSamplesPort().getPyObj()
    a,b,c=n1.getPassedResults(ex)
    self.assertEqual(a,[1,3,5,7,9,11])
    self.assertEqual([elt.getPyObj() for elt in b],[[12L,36L,60L,84L,108L,132L]])
    self.assertEqual(c,['n10.o2'])
    
    p.getChildByName("n1").getChildByName("n10").setScript("""
import time
if i1%2==1:
  raise Exception("Simulated error !")
else:
  time.sleep(1)
  o2=5*i1
""")
    ex=pilot.ExecutorSwig()
    ex.setKeepGoingProperty(True)
    p.getChildByName("n1").assignPassedResults(a,b,c)
    #
    startt=datetime.now()
    ex.RunW(p,0)
    t1=datetime.now()-startt
    #assert(t1.total_seconds()<6.+1.)# normally 6/2+1 s (6 remaining elts in 2 // branches + 1s to launch container)
    #
    self.assertEqual(n1.getState(),pilot.DONE)
    self.assertEqual(p.getState(),pilot.DONE)
    self.assertEqual(p.getChildByName("n2").getOutputPort("o4").getPyObj(),[0L,12L,30L,36L,60L,60L,90L,84L,120L,108L,150L,132L])
    pass

  pass


  def test11(self):
    "test if we do not restart from the begining of the schema after an error in a foreach"
    fname="test11.xml"
    from datetime import datetime
    p=self.r.createProc("prTest2")
    cont=p.createContainer("gg","Salome")
    cont.setProperty("name","localhost")
    cont.setProperty("hostname","localhost")
    cont.setProperty("type","multi")
    td=p.createType("double","double")
    ti=p.createType("int","int")
    tsi=p.createSequenceTc("seqint","seqint",ti)
    tsd=p.createSequenceTc("seqdbl","seqdbl",td)
    n0=self.r.createScriptNode("","n0")
    o0=n0.edAddOutputPort("o0",tsi)
    n0.setScript("o0=[ elt for elt in range(12) ]")
    p.edAddChild(n0)
    n1=self.r.createForEachLoop("n1",ti)
    n10=self.r.createScriptNode("","n10")
    n10.setExecutionMode("remote")
    n10.setContainer(cont)
    n1.edAddChild(n10)
    n10.setScript("""
import time
if i1%2==1:
  raise Exception("Simulated error !")
else:
  time.sleep(0.1)
  o2=2*i1
""")
    i1=n10.edAddInputPort("i1",ti)
    o2=n10.edAddOutputPort("o2",ti)
    p.edAddChild(n1)
    p.edAddLink(o0,n1.edGetSeqOfSamplesPort())
    p.edAddLink(n1.edGetSamplePort(),i1)
    p.edAddCFLink(n0,n1)
    n1.edGetNbOfBranchesPort().edInitPy(2)
    n2=self.r.createScriptNode("","n2")
    n2.setScript("o4=i3")
    i3=n2.edAddInputPort("i3",tsi)
    o4=n2.edAddOutputPort("o4",tsi)
    n2.setScript("o4=i3")
    p.edAddChild(n2)
    p.edAddCFLink(n1,n2)
    p.edAddLink(o2,i3)
    p.saveSchema(fname)
    #
    l=loader.YACSLoader()
    p=l.load(fname)
    n1=p.getChildByName("n1")
    ex=pilot.ExecutorSwig()
    ex.setKeepGoingProperty(True)
    #
    startt=datetime.now()
    ex.RunW(p,0)
    t0=datetime.now()-startt
    #
    self.assertEqual(p.getState(),pilot.FAILED)
    self.assertEqual(n1.getState(),pilot.FAILED)
    n1.edGetSeqOfSamplesPort().getPyObj()
    a,b,c=n1.getPassedResults(ex)

    self.assertEqual(a,[0,2,4,6,8,10])
    self.assertEqual([elt.getPyObj() for elt in b],[[0L,4L,8L,12L,16L,20L]])
    
    p.getChildByName("n0").setScript("o0=[ 3*elt for elt in range(12) ]")
    p.getChildByName("n1").getChildByName("n10").setScript("""
import time
if i1%2==0:
  raise Exception("Simulated error !")
else:
  time.sleep(1)
  o2=5*i1
""")
    p.resetState(1)


    p.getChildByName("n1").assignPassedResults(a,b,c)
    p.exUpdateState();
    #
    startt=datetime.now()
    ex.RunW(p,0,False)
    t1=datetime.now()-startt
    #
    self.assertEqual(n1.getState(),pilot.DONE)
    self.assertEqual(p.getState(),pilot.DONE)
    self.assertEqual(p.getChildByName("n2").getOutputPort("o4").getPyObj(),[0L,5L,4L,15L,8L,25L,12L,35L,16L,45L,20L,55L])
    pass

  pass

if __name__ == '__main__':
  import os,sys
  U = os.getenv('USER')
  f=open("/tmp/" + U + "/UnitTestsResult", 'a')
  f.write("  --- TEST src/yacsloader: testSaveLoadRun.py\n")
  suite = unittest.makeSuite(TestSaveLoadRun)
  result=unittest.TextTestRunner(f, descriptions=1, verbosity=1).run(suite)
  f.close()
  sys.exit(not result.wasSuccessful())
