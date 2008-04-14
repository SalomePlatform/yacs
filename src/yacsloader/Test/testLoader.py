
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
    expected="LogRecord: parser:ERROR:from node node5 does not exist in control link: node5->b2 context: b1. (samples/bid.xml:33)\n"
    self.assert_(p.getLogger("parser").getStr() == expected, "error not found")
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

import os
U = os.getenv('USER')
f=open("/tmp/" + U + "/UnitTestsResult", 'a')
f.write("  --- TEST src/yacsloader: testLoader.py\n")
suite = unittest.makeSuite(TestLoader)
unittest.TextTestRunner(f, descriptions=1, verbosity=1).run(suite)
f.close()
