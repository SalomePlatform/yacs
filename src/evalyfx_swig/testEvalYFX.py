# Copyright (C) 2015-2016  CEA/DEN, EDF R&D
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

class TestEvalYFX(unittest.TestCase):
    def test0(self):
        fileName="test0.xml"
        self.__buildScheme(fileName)
        import evalyfx
        efx=evalyfx.YACSEvalYFX.BuildFromFile(fileName)
        inps=efx.getFreeInputPorts()
        self.assertEqual([elt.getName() for elt in inps],['a','b','e'])
        self.assertEqual([elt.hasDefaultValueDefined() for elt in inps],[False,True,True])
        self.assertAlmostEqual(inps[1].getDefaultValueDefined(),1.3,12)
        self.assertAlmostEqual(inps[2].getDefaultValueDefined(),2.5,12)
        #
        inps[0].setDefaultValue(3.4)
        self.assertEqual([elt.hasDefaultValueDefined() for elt in inps],[True,True,True])
        self.assertAlmostEqual(inps[0].getDefaultValueDefined(),3.4,12)
        #
        inps[0].setDefaultValue(None)
        self.assertEqual([elt.hasDefaultValueDefined() for elt in inps],[False,True,True])
        inps[2].setDefaultValue(2.7)
        self.assertAlmostEqual(inps[2].getDefaultValueDefined(),2.7,12)
        inps[2].setDefaultValue(None)
        self.assertEqual([elt.hasDefaultValueDefined() for elt in inps],[False,True,False])
        self.assertRaises(ValueError,inps[0].getDefaultValueDefined)
        self.assertAlmostEqual(inps[1].getDefaultValueDefined(),1.3,12)
        self.assertRaises(ValueError,inps[2].getDefaultValueDefined)
        #
        outps=efx.getFreeOutputPorts()
        self.assertEqual([elt.getName() for elt in outps],['c','f','g'])
        # prepare for execution
        inps[0].setDefaultValue(1.1)
        inps[1].setSequenceOfValuesToEval([10.1,10.2,10.3])
        self.assertRaises(ValueError,efx.lockPortsForEvaluation,[inps[1]],[outps[0],outps[2]]) # because e is not set
        a=inps[2].hasSequenceOfValuesToEval()
        self.assertTrue(not a)
        inps[2].setSequenceOfValuesToEval([20.1,20.2,30.3,40.4])
        a=inps[2].hasSequenceOfValuesToEval()
        self.assertTrue(a)
        efx.lockPortsForEvaluation([inps[1],inps[2]],[outps[0],outps[2]])
        inps[2].setSequenceOfValuesToEval([20.1,20.2,30.3])
        #
        g=efx.getUndergroundGeneratedGraph()
        g.saveSchema("toto.xml")
        #
        
        pass

    def __buildScheme(self,fname):
        import SALOMERuntime
        import loader
        SALOMERuntime.RuntimeSALOME.setRuntime()
        r=SALOMERuntime.getSALOMERuntime()
        p=r.createProc("run")
        td=p.createType("double","double")
        #
        cont=p.createContainer("zeCont","Salome")
        #
        n0=r.createFuncNode("Salome","node0")
        p.edAddChild(n0)
        n0.setFname("func0")
        n0.setContainer(cont)
        n0.setScript("""def func0(a,b):
  return a*b
""")
        n0.setExecutionMode("remote")
        a=n0.edAddInputPort("a",td)
        b=n0.edAddInputPort("b",td)  ; b.edInitPy(1.3)
        c=n0.edAddOutputPort("c",td)
        #
        n1=r.createFuncNode("Salome","node1")
        p.edAddChild(n1)
        n1.setFname("func1")
        n1.setContainer(cont)
        n1.setScript("""def func1(a,b):
  return a+b,3*(a+b)
""")
        n1.setExecutionMode("remote")
        d=n1.edAddInputPort("d",td)
        e=n1.edAddInputPort("e",td)  ; e.edInitPy(2.5) # agy : useless but for test
        f=n1.edAddOutputPort("f",td)
        g=n1.edAddOutputPort("g",td)
        #
        p.edAddCFLink(n0,n1)
        p.edAddLink(c,d)
        #
        p.saveSchema(fname)
        pass
    pass

if __name__ == '__main__':
    unittest.main()
