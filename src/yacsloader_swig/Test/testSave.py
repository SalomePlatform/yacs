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

class TestSave(unittest.TestCase):

    def setUp(self):
        SALOMERuntime.RuntimeSALOME_setRuntime(1)
        pass

    def test0_saveAndExec(self):
        """Execute twice the scheme. Each time the final state is dumped
        and the scheme is written. The second exeuction is done with the
        saved scheme file. Final state dumps and scheme files produced must
        be identical for the 2 executions. Nodes are not always written in
        the same order, so the comparison is done after sort of lines...
        """
        schemaList = []
        schemaList += ["aschema","bschema","cschema","dschema","eschema","fschema"]
        schemaList += ["bloc1","bloc2","bloc3","bloc4"]
        schemaList += ["foreach1","foreach2","foreach4","foreach5"]
        schemaList += ["foreach_LongCorba","foreach_LongPython"]
        schemaList += ["forloop1","forloop2","forloop3","forloop4","forloop5","forloop6","forloop7"]
        schemaList += ["forwhile1"]
        schemaList += ["legendre7"]
        schemaList += ["switch1","switch2","switch3","switch4","switch5","switch6","switch7","switch8","switch9"]
        schemaList += ["while1","while2","while3"]
        r = pilot.getRuntime()
        l = loader.YACSLoader()
        e = pilot.ExecutorSwig()
        for schema in schemaList:
            fileOrig = "samples/" + schema + ".xml"
            saveSchema1 = "schema1_" + schema
            dumpSchema1 = "dump1_" + schema
            saveSchema2 = "schema2_" + schema
            dumpSchema2 = "dump2_" + schema
            try:
                p = l.load(fileOrig)
                s = pilot.SchemaSave(p)
                s.save(saveSchema1)
                e.RunW(p,0)
                e.saveState(dumpSchema1)
                p = l.load(saveSchema1)
                s = pilot.SchemaSave(p)
                s.save(saveSchema2)
                e.RunW(p,0)
                e.saveState(dumpSchema2)
            except ValueError, ex:
                print "Value Error: ", ex
                pb = "problem on " + fileOrig + " : ValueError"
                self.fail(pb)
            except pilot.Exception,ex:
                print ex.what()
                pb = "problem on " + fileOrig + " : " + ex.what()
                self.fail(pb)
            except:
                pb = "unknown problem on " + fileOrig
                self.fail(pb)                
            s1=open(saveSchema1,'r')
            s2=open(saveSchema2,'r')
            d1=open(dumpSchema1,'r')
            d2=open(dumpSchema2,'r')
            ls1 = s1.readlines().sort()
            ls2 = s2.readlines().sort()
            ld1 = d1.readlines().sort()
            ld2 = d2.readlines().sort()
            pb1 = "file schemes produced by successive executions are not identical: " + fileOrig 
            pb2 = "final dump states produced by successive executions are not identical: " + fileOrig 
            self.assertEqual(ls1,ls2,pb1)
            self.assertEqual(ld1,ld2,pb2)            
            pass

if __name__ == '__main__':
  import os
  U = os.getenv('USER')
  f=open("/tmp/" + U + "/UnitTestsResult", 'a')
  f.write("  --- TEST src/yacsloader: testSave.py\n")
  suite = unittest.makeSuite(TestSave)
  result=unittest.TextTestRunner(f, descriptions=1, verbosity=1).run(suite)
  f.close()
  sys.exit(not result.wasSuccessful())