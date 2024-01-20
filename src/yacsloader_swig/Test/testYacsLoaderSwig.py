#!/usr/bin/env python3
# Copyright (C) 2023-2024  CEA, EDF
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

import salome
import NamingService
import os
import sys
import subprocess
salome.salome_init()
ior = NamingService.NamingService.IOROfNS()

from testEdit import TestEdit
from testExec import TestExec
from testLoader import TestLoader
from testSave import TestSave

p = subprocess.Popen(["../yacsloader/echoSrv",ior])
import time
time.sleep(3)
import tempfile
import unittest
zezeResult = True
zeResultStr = []
with tempfile.TemporaryDirectory(suffix=".yacstest") as dir_test:
  file_test = os.path.join(dir_test,"UnitTestsResult")
  with open(file_test, 'a') as f:
      f.write("  --- TEST src/yacsloader: testLoader.py\n")
      for zeTest in [TestEdit,TestExec,TestLoader,TestSave]:
        suite = unittest.makeSuite(zeTest)
        result=unittest.TextTestRunner(f, descriptions=1, verbosity=1).run(suite)
        zeResult = result.wasSuccessful()
        zezeResult = zeResult and zezeResult
        zeResultStr.append( "Result for {} is {}".format(str(zeTest),zeResult) )
p.terminate()
zeResultStr.append("So at the end the result is {}".format(zezeResult))
print("\n".join(zeResultStr))
returnCode = int( not zezeResult )
print("Return code is {}".format( returnCode ))
sys.exit(returnCode)
