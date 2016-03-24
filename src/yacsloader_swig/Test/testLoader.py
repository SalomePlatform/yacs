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

import pilot
import SALOMERuntime
import loader
import unittest

class TestLoader(unittest.TestCase):

  def setUp(self):
    SALOMERuntime.RuntimeSALOME_setRuntime()
    self.r = pilot.getRuntime()
    self.l = loader.YACSLoader()
    self.e = pilot.ExecutorSwig()
    pass

  def tearDown(self):
    del self.r
    del self.l
    del self.e

  def test1_FileNotExist(self):
    # --- File does not exist
    retex=None
    try:
      p = self.l.load("nonexisting")
    except IOError, ex:
      print "IO Error: ", ex
      retex=ex
    #except pilot.invalid_argument,ex:
    #  print "invalid_argument:",ex.what()
    #  retex=ex.what()
    self.assert_(retex is not None, "exception not raised, or wrong type")
    pass

  def test2_parseError(self):
    # --- File exists but parse error
    retex=None
    try:
      p = self.l.load("samples/bid.xml")
    except ValueError,ex:
      print "Caught ValueError Exception:",ex
      retex = ex
    expected="LogRecord: parser:ERROR:from node node5 does not exist in control link: node5->b2 context: b1. (samples/bid.xml:53)\n"
    self.assert_(p.getLogger("parser").getStr() == expected, "error not found: "+p.getLogger("parser").getStr())
    pass

  def test3_normal(self):
    # --- File exists and no parsing problem
    try:
      p = self.l.load("samples/aschema.xml")
      print p.getLogger("parser").getStr()
      print p
      print p.getName()
      for k in p.typeMap: print k
      for k in p.nodeMap: print k
      for k in p.inlineMap: print k
      for k in p.serviceMap: print k
      print self.e.getTasksToLoad()
      self.e.RunW(p,0)
      self.assertEqual(106, p.getChildByName('node48').getEffectiveState())
    except pilot.Exception,ex:
      print "YACS exception:",ex
      self.fail(ex)
      pass
    pass

if __name__ == '__main__':
  import os
  U = os.getenv('USER')
  f=open("/tmp/" + U + "/UnitTestsResult", 'a')
  f.write("  --- TEST src/yacsloader: testLoader.py\n")
  suite = unittest.makeSuite(TestLoader)
  result=unittest.TextTestRunner(f, descriptions=1, verbosity=1).run(suite)
  f.close()
  sys.exit(not result.wasSuccessful())