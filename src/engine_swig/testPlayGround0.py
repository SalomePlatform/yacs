#!/usr/bin/env python3
# Copyright (C) 2006-2019  CEA/DEN, EDF R&D
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

class TestPlayGround0(unittest.TestCase):
    def test0(self):
        pg=pilot.PlayGround([("a0",28),("a1",28),("a2",28)])
        pd=pilot.ContigPartDefinition(pg,0,3*28)
        cw=pilot.ComplexWeight(0.,1.,4)
        res=pg.partition([(pd,cw),(pd,cw)],[4,4])
        assert(len(res)==2)
        assert(isinstance(res[0],pilot.ContigPartDefinition))
        assert(isinstance(res[1],pilot.ContigPartDefinition))
        assert(res[0].getStart()==0 and res[0].getStop()==44)
        assert(res[1].getStart()==44 and res[1].getStop()==84)
        assert(sum([elt.getNumberOfCoresConsumed() for elt in res])==pg.getNumberOfCoresAvailable())
        pd2=pilot.AllPartDefinition(pg)
        assert(pd2.getNumberOfCoresConsumed()==84)
        res=pg.partition([(pd2,cw),(pd2,cw),(pd2,cw)],[4,4,4])
        assert(len(res)==3)
        assert(isinstance(res[0],pilot.ContigPartDefinition))
        assert(isinstance(res[1],pilot.ContigPartDefinition))
        assert(isinstance(res[2],pilot.ContigPartDefinition))
        assert(res[0].getStart()==0 and res[0].getStop()==28)
        assert(res[1].getStart()==28 and res[1].getStop()==56)
        assert(res[2].getStart()==56 and res[2].getStop()==84)
        #
        pg.setData([("a0",2),("a1",8),("a2",8)])
        cw2=pilot.ComplexWeight(0.,4.,1)
        res=pg.partition([(pilot.AllPartDefinition(pg),cw),(pilot.AllPartDefinition(pg),cw)],[4,1])
        assert(len(res)==2)
        assert(isinstance(res[0],pilot.ContigPartDefinition))
        assert(isinstance(res[1],pilot.NonContigPartDefinition))
        assert(res[0].getStart()==2 and res[0].getStop()==10)
        assert(res[1].getIDs()==(0,1,10,11,12,13,14,15,16,17))
        pass

    def test1(self):
        """ test focused on complicated cut due to lack of cores"""
        pg=pilot.PlayGround([("a0",13)])
        pd=pilot.ContigPartDefinition(pg,0,13)
        cw=pilot.ComplexWeight(0.,1.,4)
        cw2=pilot.ComplexWeight(0.,2.,4)
        res=pg.partition([(pd,cw),(pd,cw2)],[4,4])
        assert(len(res)==2)
        assert(isinstance(res[0],pilot.ContigPartDefinition) and isinstance(res[1],pilot.ContigPartDefinition))
        assert(res[0].getStart()==0 and res[0].getStop()==4)
        assert(res[1].getStart()==4 and res[1].getStop()==12)# 1 core lost
        #
        pg=pilot.PlayGround([("a0",2),("a1",27)])
        cw3=pilot.ComplexWeight(0.,20,1)
        cw4=pilot.ComplexWeight(0.,1.,8)
        pd=pilot.ContigPartDefinition(pg,0,29)
        res=pg.partition([(pd,cw3),(pd,cw4)],[4,8])
        assert(len(res)==2)
        assert(isinstance(res[0],pilot.ContigPartDefinition) and isinstance(res[1],pilot.ContigPartDefinition))
        assert(res[0].getStart()==10 and res[0].getStop()==26)
        assert(res[1].getStart()==2 and res[1].getStop()==10)# 5 cores lost
        pass
    
    pass

if __name__ == '__main__':
    unittest.main()
