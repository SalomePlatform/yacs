#  Copyright (C) 2006-2010  CEA/DEN, EDF R&D
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

import CORBA

orb = CORBA.ORB_init();

f = open('AlgLin.ior');
s = f.read();
f.close();

o = orb.string_to_object(s);

import Distant
o = o._narrow(Distant.AlgLin)


v1 = [ 1, 2, 3 ]
v2 = [ 3, 4, 5 ]

v3 = o.addvec(v1, v2)
print v3

print o.prdscl(v1, v3)
