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

import Solveur__POA
import FreeFem

class FreeFem_i(Solveur__POA.FreeFem):

    def __init__(self):
        self.F = FreeFem.FreeFem();

    def Bords(self, b):
        self.F.Bords(b);

    def Flux(self, u1, u2):
        self.F.Flux(u1, u2);

    def Convection(self, cond_init, dt, n):
        self.F.Convection(cond_init, dt, n)

