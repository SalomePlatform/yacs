#!/usr/bin/env python3
# Copyright (C) 2021-2025  CEA, EDF
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
import loader
import os
import datetime
import salome
import tempfile
import NamingService

class TestBasicFirstSSL(unittest.TestCase):
    def test0(self):
        """
        First SSL test with YACS. This test launches SALOME_Container_No_NS_Serv servers to perform it's job.
        These extra SALOME_Container_No_NS_Serv servers are shut down at the end
        """
        salome.standalone()
        NamingService.NamingService.SetLogContainersFile()
        SALOMERuntime.RuntimeSALOME.setRuntime()
        rrr=SALOMERuntime.getSALOMERuntime()
        """First test of HP Container no loop here only the 3 sorts of python nodes (the Distributed is it still used and useful ?) """
        fname= "TestSaveLoadRun0.xml"
        nbOfNodes=2
        sqrtOfNumberOfTurn=1000 # 3000 -> 3.2s/Node, 1000 -> 0.1s/Node
        l=loader.YACSLoader()
        p=rrr.createProc("prTest0")
        td=p.createType("double","double")
        ti=p.createType("int","int")
        pg=pilot.PlayGround()
        pg.setData([("localhost",4)])

        script0="""
def ff(nb,dbg):
    from math import cos
    import datetime

    ref=datetime.datetime.now()
    t=0. ; pas=1./float(nb)
    for i in range(nb):
        for j in range(nb):
            x=j*pas
            t+=1.+cos(1.*(x*3.14159))
            pass
        pass
    print("coucou from script0-%i  -> %s"%(dbg,str(datetime.datetime.now()-ref)))
    return t
        """
        for i in range(nbOfNodes):
            cont=p.createContainer("gg{}".format(i),"Salome")
            cont.setProperty("name","localhost")
            cont.setProperty("hostname","localhost")
            cont.setProperty("nb_proc_per_node","1")
            node0=rrr.createFuncNode("DistPython","node%i"%(i))
            p.edAddChild(node0)
            node0.setFname("ff")
            node0.setContainer(cont)
            node0.setScript(script0)
            nb=node0.edAddInputPort("nb",ti) ; nb.edInitInt(sqrtOfNumberOfTurn)
            dbg=node0.edAddInputPort("dbg",ti) ; dbg.edInitInt(i+1)
            out0=node0.edAddOutputPort("s",td)
            pass
        with tempfile.TemporaryDirectory() as tmpdir:
            tmp_fname = os.path.join(tmpdir, fname)
            p.saveSchema(tmp_fname)
            p=l.load(tmp_fname)
        ex=pilot.ExecutorSwig()
        self.assertEqual(p.getState(),pilot.READY)
        st=datetime.datetime.now()
        p.propagePlayGround(pg)
        # 1st exec
        #input(os.getpid())
        ex.RunW(p,0)
        print("Time spend of test0 to run 1st %s"%(str(datetime.datetime.now()-st)))
        self.assertEqual(p.getState(),pilot.DONE)
        # 2nd exec using the same already launched remote python interpreters
        st=datetime.datetime.now()
        ex.RunW(p,0)
        print("Time spend of test0 to run 2nd %s"%(str(datetime.datetime.now()-st)))
        self.assertEqual(p.getState(),pilot.DONE)
        # 3rd exec using the same already launched remote python interpreters
        st=datetime.datetime.now()
        ex.RunW(p,0)
        print("Time spend of test0 to run 3rd %s"%(str(datetime.datetime.now()-st)))
        self.assertEqual(p.getState(),pilot.DONE)
        print( "file containing list of IORS of containers : \"{}\"".format( NamingService.NamingService.GetLogContainersFile() ) )
        print(NamingService.NamingService.IOROfNS())
        print(NamingService.NamingService.RefOfNS())
        print("Killing all containers")
        NamingService.NamingService.KillContainersInFile( NamingService.NamingService.GetLogContainersFile() )


if __name__ == '__main__':
    unittest.main()