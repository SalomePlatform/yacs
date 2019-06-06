# Copyright (C) 2019  CEA/DEN, EDF R&D
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
import unittest
import pickle
from math import sin

class TestBase64Conv(unittest.TestCase):
    
    def test0(self):
        """
        This test checks that internal method for Base64 is OK with pickled objects.
        Reason of home made conversion :
        old boost base64 conversion failed with (see commit b0f05b249ace88109a4a3d) string of size 124 due to incorrect output after pilot.FromBase64Swig
        old boost base64 conversion failed with string of size 157 due to an exception thrown by boost
        """
        for i in range(124,624):
            st = i*"/"
            a = pickle.dumps(st,protocol=4) # protocol 4 is important here to generate complex
            self.assertTrue( a == pilot.FromBase64Swig(pilot.ToBase64Swig(a) ) )
            pass
        pass

    def test1(self):
        l = [sin(float(i)) for i in range(1000)]
        a = pickle.dumps(l,protocol=4)
        self.assertTrue( a == pilot.FromBase64Swig(pilot.ToBase64Swig(a) ) )
        pass
    
    pass

if __name__ == '__main__':
    unittest.main()
