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

import unittest
import pilot
import SALOMERuntime

class TestContainerRef(unittest.TestCase):
  def setUp(self):
    SALOMERuntime.RuntimeSALOME_setRuntime()
    self.r=SALOMERuntime.getSALOMERuntime()
    self.p=self.r.createProc("pr")

  def test0(self):
    """test delete following creation from class"""
    co=self.r.createContainer()
    self.assertEqual(co.getRefCnt(), 1)
    self.assert_(co.thisown)
    del co

  def test1(self):
    """test delete following creation from createContainer and delitem from containerMap"""
    co=self.p.createContainer("c2")
    del self.p.containerMap["c2"]
    self.assert_(co.thisown)
    self.assertEqual(co.getRefCnt(), 1)
    del co

  def test2(self):
    """test delete following creation from createContainer and
       manipulations on containerMap
    """
    co=self.p.createContainer("c2")
    self.p.containerMap["c2"]=co
    del self.p.containerMap["c2"]
    self.assert_(co.thisown)
    self.assertEqual(co.getRefCnt(), 1)
    del co

  def test3(self):
    """test existence on getitem followed by delitem"""
    self.p.createContainer("c9")
    co=self.p.containerMap["c9"]
    self.assertEqual(co.getRefCnt(), 2)
    del self.p.containerMap["c9"]
    self.assertEqual(co.getName(), "c9")
    self.assertEqual(co.getRefCnt(), 1)
    self.assert_(co.thisown)
    del co

  def test4(self):
    """test delete from containerMap following creation from createContainer"""
    co=self.p.createContainer("c10")
    del self.p.containerMap["c10"]
    self.assertEqual(co.getName(), "c10")
    self.assertEqual(co.getRefCnt(), 1)
    self.assert_(co.thisown)
    del co

  def test5(self):
    """test existence container following delete proc"""
    co=self.p.createContainer("c10")
    del self.p
    self.assertEqual(co.getName(), "c10")
    self.assertEqual(co.getRefCnt(), 1)
    self.assert_(co.thisown)
    del co

  def test6(self):
    """test ownership of container on getitem from containerMap"""
    co=self.p.createContainer("c8")
    self.assertEqual(co.getRefCnt(), 2)
    self.assert_(co.thisown)
    del co
    self.assertEqual(self.p.containerMap["c8"].getRefCnt(), 2) # +1 for getitem
    co=self.p.containerMap["c8"]
    self.assertEqual(co.getRefCnt(), 2)
    self.assert_(co.thisown)
    del co
    self.assertEqual(self.p.containerMap["c8"].getRefCnt(), 2) # +1 for getitem
    del self.p.containerMap["c8"]

  def test7(self):
    """test getitem following creation from class"""
    co=self.r.createContainer()
    self.assertEqual(co.getRefCnt(), 1)
    self.p.containerMap["c8"]=co
    self.assertEqual(co.getRefCnt(), 2)
    d=self.p.containerMap["c8"]
    self.assertEqual(d.getRefCnt(), 3)
    del self.p.containerMap["c8"]
    self.assertEqual(d.getRefCnt(), 2)
    self.assertEqual(co.getRefCnt(), 2)
    del co
    self.assertEqual(d.getRefCnt(), 1)

  def test8(self):
    """test setitem following creation from class"""
    co=self.r.createContainer()
    self.p.containerMap["c8"]=co
    d=self.p.containerMap["c8"]
    self.p.containerMap["c9"]=d
    self.assertEqual(d.getRefCnt(), 4)

  def test9(self):
    """test method values"""
    self.p.createContainer("c8")
    for co in self.p.containerMap.values():
      self.assert_(co.thisown)
      self.assertEqual(co.getRefCnt(), 2)

  def test10(self):
    """test method items"""
    self.p.createContainer("c8")
    for k,co in self.p.containerMap.items():
      self.assert_(co.thisown)
      self.assertEqual(co.getRefCnt(), 2)

  def test11(self):
    """test method clear"""
    co=self.p.createContainer("c8")
    self.p.containerMap.clear()
    self.assert_(co.thisown)
    self.assertEqual(co.getRefCnt(), 1)

  def test12(self):
    """test method update"""
    co=self.p.createContainer("c8")
    d={"c1":co}
    self.p.containerMap.update(d)
    self.assert_(co.thisown)
    self.assertEqual(co.getRefCnt(), 3)

class TestTypeCodeRef(unittest.TestCase):
  def setUp(self):
    self.r=SALOMERuntime.getSALOMERuntime()
    self.p=self.r.createProc("pr")

  def test0(self):
    """test delete following creation from createSequenceTc"""
    tc=pilot.TypeCode(pilot.Double)
    self.assertEqual(tc.getRefCnt(), 1)
    self.assert_(tc.thisown)

  def test1(self):
    """test delete following creation from createInterfaceTc and delitem from typeMap"""
    tc=self.p.createInterfaceTc("","obj",[])
    del self.p.typeMap["obj"]
    self.assert_(tc.thisown)
    self.assertEqual(tc.getRefCnt(), 1)

  def test2(self):
    """test delete following creation from createInterfaceTc and
       manipulations on typeMap
    """
    tc=self.p.createInterfaceTc("","obj",[])
    self.p.typeMap["obj"]=tc
    del self.p.typeMap["obj"]
    self.assert_(tc.thisown)
    self.assertEqual(tc.getRefCnt(), 1)

  def test3(self):
    """test existence on getitem followed by delitem"""
    self.p.createInterfaceTc("","obj",[])
    tc=self.p.typeMap["obj"]
    self.assertEqual(tc.getRefCnt(), 2)
    del self.p.typeMap["obj"]
    self.assertEqual(tc.getRefCnt(), 1)
    self.assert_(tc.thisown)

  def test4(self):
    """test delete from typeMap following creation from createInterfaceTc"""
    tc=self.p.createInterfaceTc("","obj",[])
    del self.p.typeMap["obj"]
    self.assertEqual(tc.getRefCnt(), 1)
    self.assert_(tc.thisown)

  def test5(self):
    """test existence TypeCode following delete proc"""
    tc=self.p.createInterfaceTc("","obj",[])
    del self.p
    self.assertEqual(tc.getRefCnt(), 1)
    self.assert_(tc.thisown)

  def test6(self):
    """test ownership of TypeCode on getitem from typeMap"""
    tc=self.p.createInterfaceTc("","obj",[])
    self.assertEqual(tc.getRefCnt(), 2)
    self.assert_(tc.thisown)
    del tc
    self.assertEqual(self.p.typeMap["obj"].getRefCnt(), 2) # +1 for getitem
    tc=self.p.typeMap["obj"]
    self.assertEqual(tc.getRefCnt(), 2)
    self.assert_(tc.thisown)
    del tc
    self.assertEqual(self.p.typeMap["obj"].getRefCnt(), 2) # +1 for getitem
    del self.p.typeMap["obj"]

  def test7(self):
    """test getitem following creation from class"""
    tc=pilot.TypeCode.interfaceTc("obj","obj")
    self.assertEqual(tc.getRefCnt(), 1)
    self.p.typeMap["obj"]=tc
    self.assertEqual(tc.getRefCnt(), 2)
    d=self.p.typeMap["obj"]
    self.assertEqual(d.getRefCnt(), 3)
    del self.p.typeMap["obj"]
    self.assertEqual(d.getRefCnt(), 2)
    self.assertEqual(tc.getRefCnt(), 2)
    del tc
    self.assertEqual(d.getRefCnt(), 1)

  def test8(self):
    """test setitem following creation from class"""
    tc=pilot.TypeCodeObjref("obj","obj")
    self.p.typeMap["obj"]=tc
    d=self.p.typeMap["obj"]
    self.p.typeMap["t9"]=d
    self.assertEqual(d.getRefCnt(), 4)

  def test9(self):
    """test method values"""
    self.p.createInterfaceTc("","obj",[])
    for tc in self.p.typeMap.values():
      if tc.name()!="obj":continue
      self.assert_(tc.thisown)
      self.assertEqual(tc.getRefCnt(), 2)

  def test10(self):
    """test method items"""
    self.p.createInterfaceTc("","obj",[])
    for k,tc in self.p.typeMap.items():
      if tc.name()!="obj":continue
      self.assert_(tc.thisown)
      self.assertEqual(tc.getRefCnt(), 2)

  def test11(self):
    """test method clear"""
    tc=self.p.createInterfaceTc("","obj",[])
    self.p.typeMap.clear()
    self.assert_(tc.thisown)
    self.assertEqual(tc.getRefCnt(), 1)

  def test12(self):
    """test method update"""
    tc=self.p.createInterfaceTc("","obj",[])
    d={"c1":tc}
    self.p.typeMap.update(d)
    self.assert_(tc.thisown)
    self.assertEqual(tc.getRefCnt(), 3)

if __name__ == '__main__':
  import os
  U = os.getenv('USER')
  f=open("/tmp/" + U + "/UnitTestsResult", 'a')
  f.write("  --- TEST src/yacsloader: testRefcount.py\n")
  suite1 = unittest.makeSuite(TestContainerRef)
  suite2 = unittest.makeSuite(TestTypeCodeRef)
  suite = unittest.TestSuite((suite1, suite2))
  result=unittest.TextTestRunner(f, descriptions=1, verbosity=1).run(suite)
  f.close()
  sys.exit(not result.wasSuccessful())