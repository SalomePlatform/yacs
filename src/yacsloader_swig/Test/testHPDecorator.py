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

import pilot
import SALOMERuntime
import unittest

class TestHPDecortator(unittest.TestCase):
    def test0(self):
        """ First test coming from the big boss."""
        SALOMERuntime.RuntimeSALOME.setRuntime()
        r=SALOMERuntime.getSALOMERuntime()
        pg=pilot.PlayGround()
        pg.loadFromKernelCatalog()
        assert(len(pg.getData())!=0)
        p=r.createProc("p0")
        td=p.createType("double","double")
        tdd=p.createSequenceTc("seqdouble","seqdouble",td)
        hp1=p.createContainer("HP1","HPSalome")
        hp4=p.createContainer("HP4","HPSalome")
        #
        n0=r.createScriptNode("Salome","n0")
        n0.setExecutionMode("remote")
        out0_0=n0.edAddOutputPort("o1",tdd)
        n0.setScript("""o1=[float(i)+0.1 for i in range(1000)]""")
        n0.setContainer(hp4)
        p.edAddChild(n0)
        #
        n1_0=r.createForEachLoop("n1_0",td)
        p.edAddChild(n1_0)
        p.edAddCFLink(n0,n1_0)
        p.edAddLink(out0_0,n1_0.edGetSeqOfSamplesPort())
        ##
        n1_0_sc=r.createScriptNode("Salome","n1_0_sc")
        n1_0.edAddChild(n1_0_sc)
        n1_0_sc.setExecutionMode("remote")
        n1_0_sc.setScript("""2*i1""")
        i1_0_sc=n1_0_sc.edAddInputPort("i1",td)
        p.edAddLink(n1_0.edGetSamplePort(),i1_0_sc)
        n1_0_sc.setContainer(hp4)
        ##
        #
        n1_1=r.createForEachLoop("n1_1",td)
        p.edAddChild(n1_1)
        p.edAddCFLink(n0,n1_1)
        p.edAddLink(out0_0,n1_1.edGetSeqOfSamplesPort())
        ##
        n1_1_sc=r.createScriptNode("Salome","n1_1_sc")
        n1_1.edAddChild(n1_1_sc)
        n1_1_sc.setExecutionMode("remote")
        n1_1_sc.setScript("""3*i1""")
        i1_1_sc=n1_1_sc.edAddInputPort("i1",td)
        p.edAddLink(n1_1.edGetSamplePort(),i1_1_sc)
        n1_1_sc.setContainer(hp1)
        ##
        hp1.setProperty("nb_proc_per_node","1")
        hp4.setProperty("nb_proc_per_node","4")
        pg.setData([("m0",8),("m1",8),("m2",8),("m3",8)]) # virtual machine with 32 cores spread over 4 nodes
        assert(n1_0.getWeight()==1.)
        assert(n1_1.getWeight()==1.)
        p.fitToPlayGround(pg)########### ZE CALL
        fyto=pilot.ForTestOmlyHPContCls()
        assert(hp4.getSizeOfPool()==8)# 32/4
        n1_0_sc.getContainer().forYourTestsOnly(fyto)
        assert(fyto.getContainerType()=="HPContainerShared")
        pd=fyto.getPD()
        assert(isinstance(pd,pilot.ContigPartDefinition))
        assert(pd.getStart()==0 and pd.getStop()==16)
        assert(fyto.getIDS()==(0,1,2,3))
        #
        assert(hp1.getSizeOfPool()==32)# 32/1
        fyto=pilot.ForTestOmlyHPContCls()
        n1_1_sc.getContainer().forYourTestsOnly(fyto)
        assert(fyto.getContainerType()=="HPContainerShared")
        pd=fyto.getPD()
        assert(isinstance(pd,pilot.ContigPartDefinition))
        assert(pd.getStart()==16 and pd.getStop()==32)
        assert(fyto.getIDS()==(16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31))
        assert(n1_0.edGetNbOfBranchesPort().getPyObj()==4)
        assert(n1_1.edGetNbOfBranchesPort().getPyObj()==16)
        #
        fyto=pilot.ForTestOmlyHPContCls()
        n0.getContainer().forYourTestsOnly(fyto)
        assert(fyto.getContainerType()=="HPContainerShared")
        pd=fyto.getPD()
        assert(isinstance(pd,pilot.AllPartDefinition))
        assert(list(fyto.getIDS())==range(8))
        #############################
        #  Change weight of ForEach #
        #############################
        n1_0.setWeight(2)
        p.fitToPlayGround(pg)########### ZE CALL
        assert(hp4.getSizeOfPool()==8)# 32/4
        n1_0_sc.getContainer().forYourTestsOnly(fyto)
        assert(fyto.getContainerType()=="HPContainerShared")
        pd=fyto.getPD()
        assert(isinstance(pd,pilot.ContigPartDefinition))
        assert(pd.getStart()==0 and pd.getStop()==21)
        assert(fyto.getIDS()==(0,1,2,3,4))
        assert(hp1.getSizeOfPool()==32)# 32/1
        fyto=pilot.ForTestOmlyHPContCls()
        n1_1_sc.getContainer().forYourTestsOnly(fyto)
        assert(fyto.getContainerType()=="HPContainerShared")
        pd=fyto.getPD()
        assert(isinstance(pd,pilot.ContigPartDefinition))
        assert(pd.getStart()==21 and pd.getStop()==32)
        assert(fyto.getIDS()==(21,22,23,24,25,26,27,28,29,30,31))
        assert(n1_0.edGetNbOfBranchesPort().getPyObj()==6)
        assert(n1_1.edGetNbOfBranchesPort().getPyObj()==11)
        #
        fyto=pilot.ForTestOmlyHPContCls()
        n0.getContainer().forYourTestsOnly(fyto)
        assert(fyto.getContainerType()=="HPContainerShared")
        pd=fyto.getPD()
        assert(isinstance(pd,pilot.AllPartDefinition))
        assert(list(fyto.getIDS())==range(8))
        pass

    def test1(self):
        """ Second test as test0 + script node in // with 2 FE"""
        SALOMERuntime.RuntimeSALOME.setRuntime()
        r=SALOMERuntime.getSALOMERuntime()
        pg=pilot.PlayGround()
        pg.loadFromKernelCatalog()
        assert(len(pg.getData())!=0)
        p=r.createProc("p0")
        td=p.createType("double","double")
        tdd=p.createSequenceTc("seqdouble","seqdouble",td)
        hp1=p.createContainer("HP1","HPSalome")
        hp4=p.createContainer("HP4","HPSalome")
        #
        n0=r.createScriptNode("Salome","n0")
        n0.setExecutionMode("remote")
        out0_0=n0.edAddOutputPort("o1",tdd)
        n0.setScript("""o1=[float(i)+0.1 for i in range(1000)]""")
        n0.setContainer(hp4)
        p.edAddChild(n0)
        #
        n1_0=r.createForEachLoop("n1_0",td)
        p.edAddChild(n1_0)
        p.edAddCFLink(n0,n1_0)
        p.edAddLink(out0_0,n1_0.edGetSeqOfSamplesPort())
        ##
        n1_0_sc=r.createScriptNode("Salome","n1_0_sc")
        n1_0.edAddChild(n1_0_sc)
        n1_0_sc.setExecutionMode("remote")
        n1_0_sc.setScript("""2*i1""")
        i1_0_sc=n1_0_sc.edAddInputPort("i1",td)
        p.edAddLink(n1_0.edGetSamplePort(),i1_0_sc)
        n1_0_sc.setContainer(hp4)
        ##
        #
        n1_1=r.createForEachLoop("n1_1",td)
        p.edAddChild(n1_1)
        p.edAddCFLink(n0,n1_1)
        p.edAddLink(out0_0,n1_1.edGetSeqOfSamplesPort())
        ##
        n1_1_sc=r.createScriptNode("Salome","n1_1_sc")
        n1_1.edAddChild(n1_1_sc)
        n1_1_sc.setExecutionMode("remote")
        n1_1_sc.setScript("""3*i1""")
        i1_1_sc=n1_1_sc.edAddInputPort("i1",td)
        p.edAddLink(n1_1.edGetSamplePort(),i1_1_sc)
        n1_1_sc.setContainer(hp1)
        #
        n1_2=r.createScriptNode("Salome","n1_2")
        p.edAddChild(n1_2)
        n1_2.setExecutionMode("remote")
        n1_2.setContainer(hp4)
        n1_2.setScript("""my_container""")
        p.edAddCFLink(n0,n1_2)
        ##
        hp1.setProperty("nb_proc_per_node","1")
        hp4.setProperty("nb_proc_per_node","4")
        pg.setData([("m0",8),("m1",8),("m2",8),("m3",8)]) # virtual machine with 32 cores spread over 4 nodes
        assert(n1_0.getWeight()==1.)
        assert(n1_1.getWeight()==1.)
        p.fitToPlayGround(pg)########### ZE CALL
        assert(hp4.getSizeOfPool()==8)# 32/4
        fyto=pilot.ForTestOmlyHPContCls()
        n1_0_sc.getContainer().forYourTestsOnly(fyto)
        assert(fyto.getContainerType()=="HPContainerShared")
        pd=fyto.getPD()
        assert(isinstance(pd,pilot.ContigPartDefinition))
        assert(pd.getStart()==0 and pd.getStop()==16)
        assert(fyto.getIDS()==(0,1,2,3))
        #
        assert(hp1.getSizeOfPool()==32)# 32/1
        fyto=pilot.ForTestOmlyHPContCls()
        n1_1_sc.getContainer().forYourTestsOnly(fyto)
        assert(fyto.getContainerType()=="HPContainerShared")
        pd=fyto.getPD()
        assert(isinstance(pd,pilot.ContigPartDefinition))
        assert(pd.getStart()==16 and pd.getStop()==32)
        assert(fyto.getIDS()==(16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31))
        assert(n1_0.edGetNbOfBranchesPort().getPyObj()==4)
        assert(n1_1.edGetNbOfBranchesPort().getPyObj()==16)
        #
        fyto=pilot.ForTestOmlyHPContCls()
        n0.getContainer().forYourTestsOnly(fyto)
        assert(fyto.getContainerType()=="HPContainerShared")
        pd=fyto.getPD()
        assert(isinstance(pd,pilot.AllPartDefinition))
        assert(list(fyto.getIDS())==range(8))
        fyto=pilot.ForTestOmlyHPContCls()
        n1_2.getContainer().forYourTestsOnly(fyto)
        assert(fyto.getContainerType()=="HPContainerShared")
        pd=fyto.getPD()
        assert(isinstance(pd,pilot.AllPartDefinition))
        assert(list(fyto.getIDS())==range(8))
        pass

    def test2(self):
        """ This test is not launched here because it requires 2 machines in catalog"""
        m1="dsp0764200"
        m2="dsp0764412"
        SALOMERuntime.RuntimeSALOME.setRuntime()
        r=SALOMERuntime.getSALOMERuntime()
        pg=pilot.PlayGround()
        pg.loadFromKernelCatalog()
        assert(len(pg.getData())!=0)
        p=r.createProc("p0")
        td=p.createType("double","double")
        tdd=p.createSequenceTc("seqdouble","seqdouble",td)
        hp1=p.createContainer("HP1","HPSalome")
        #
        n0=r.createScriptNode("Salome","n0")
        n0.setExecutionMode("remote")
        out0_0=n0.edAddOutputPort("o1",tdd)
        n0.setScript("""o1=[float(i)+0.1 for i in range(1000)]""")
        n0.setContainer(hp1)
        p.edAddChild(n0)
        ##
        n1_0_sc=r.createScriptNode("Salome","n1_0_sc")
        p.edAddChild(n1_0_sc)
        p.edAddCFLink(n0,n1_0_sc)
        n1_0_sc.setExecutionMode("remote")
        n1_0_sc.setScript("""assert(my_container.getHostName()=="%s")"""%m1)
        n1_0_sc.setContainer(hp1)
        ##
        n1_1_sc=r.createScriptNode("Salome","n1_1_sc")
        p.edAddChild(n1_1_sc)
        p.edAddCFLink(n0,n1_1_sc)
        n1_1_sc.setExecutionMode("remote")
        n1_1_sc.setScript("""assert(my_container.getHostName()=="%s")"""%m2)
        n1_1_sc.setContainer(hp1)
        ##
        hp1.setProperty("nb_proc_per_node","1")
        pg.setData([(m1,8),(m2,8)])
        p.fitToPlayGround(pg)########### ZE CALL
        assert(hp1.getSizeOfPool()==16)# 16/1
        fyto=pilot.ForTestOmlyHPContCls()
        n1_0_sc.getContainer().forYourTestsOnly(fyto)
        assert(fyto.getContainerType()=="HPContainerShared")
        pd=fyto.getPD()
        assert(isinstance(pd,pilot.ContigPartDefinition))
        assert(pd.getStart()==0 and pd.getStop()==8)
        assert(fyto.getIDS()==(0,1,2,3,4,5,6,7))
        #
        fyto=pilot.ForTestOmlyHPContCls()
        n1_1_sc.getContainer().forYourTestsOnly(fyto)
        assert(fyto.getContainerType()=="HPContainerShared")
        pd=fyto.getPD()
        assert(isinstance(pd,pilot.ContigPartDefinition))
        assert(pd.getStart()==8 and pd.getStop()==16)
        assert(fyto.getIDS()==(8,9,10,11,12,13,14,15))
        #
        exe=pilot.ExecutorSwig()
        assert(p.getState()==pilot.READY)
        exe.RunW(p,0)
        assert(p.getState()==pilot.DONE)
        pass
    
    pass

if __name__ == '__main__':
    unittest.main()
