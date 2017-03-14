# Copyright (C) 2006-2017  CEA/DEN, EDF R&D
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
        pd=pilot.ContigPartDefinition(pg,4,0,3*28)
        res=pg.partition([(pd,1.),(pd,1.)])
        assert(len(res)==2)
        assert(isinstance(res[0],pilot.ContigPartDefinition))
        assert(isinstance(res[1],pilot.ContigPartDefinition))
        assert(res[0].getStart()==0 and res[0].getStop()==40)
        assert(res[1].getStart()==40 and res[1].getStop()==84)
        assert(sum([elt.getNumberOfCoresConsumed() for elt in res])==pg.getNumberOfCoresAvailable())
        pd2=pilot.AllPartDefinition(pg,4)
        assert(pd2.getNumberOfCoresConsumed()==84)
        res=pg.partition([(pd2,1.),(pd2,1.),(pd2,1.)])
        assert(len(res)==3)
        assert(isinstance(res[0],pilot.ContigPartDefinition))
        assert(isinstance(res[1],pilot.ContigPartDefinition))
        assert(isinstance(res[2],pilot.ContigPartDefinition))
        assert(res[0].getStart()==0 and res[0].getStop()==28)
        assert(res[1].getStart()==28 and res[1].getStop()==56)
        assert(res[2].getStart()==56 and res[2].getStop()==84)
        #
        pg.setData([("a0",2),("a1",8),("a2",8)])
        res=pg.partition([(pilot.AllPartDefinition(pg,4),1.),(pilot.AllPartDefinition(pg,1),1.)])
        assert(len(res)==2)
        assert(isinstance(res[0],pilot.ContigPartDefinition))
        assert(isinstance(res[1],pilot.NonContigPartDefinition))
        assert(res[0].getStart()==2 and res[0].getStop()==10)
        assert(res[1].getIDs()==(0,1,10,11,12,13,14,15,16,17))
        pass

    def test1(self):
        """ test focused on complicated cut due to lack of cores"""
        pg=pilot.PlayGround([("a0",13)])
        pd=pilot.ContigPartDefinition(pg,4,0,13)
        res=pg.partition([(pd,1.),(pd,1.)])
        assert(len(res)==2)
        assert(isinstance(res[0],pilot.ContigPartDefinition) and isinstance(res[1],pilot.ContigPartDefinition))
        assert(res[0].getStart()==0 and res[0].getStop()==4)
        assert(res[1].getStart()==4 and res[1].getStop()==12)# 1 core lost
        #
        pg=pilot.PlayGround([("a0",2),("a1",11)])
        pd=pilot.ContigPartDefinition(pg,4,0,13)
        res=pg.partition([(pd,1.),(pd,1.)])
        assert(len(res)==2)
        assert(isinstance(res[0],pilot.ContigPartDefinition) and isinstance(res[1],pilot.ContigPartDefinition))
        assert(res[0].getStart()==2 and res[0].getStop()==6)
        assert(res[1].getStart()==6 and res[1].getStop()==10)# 5 cores lost
        pass
    
    pass

if __name__ == '__main__':
    unittest.main()
