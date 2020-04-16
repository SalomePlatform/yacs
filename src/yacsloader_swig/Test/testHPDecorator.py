# Copyright (C) 2006-2020  CEA/DEN, EDF R&D
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
        n1_1.setWeight(4.)
        i1_1_sc=n1_1_sc.edAddInputPort("i1",td)
        p.edAddLink(n1_1.edGetSamplePort(),i1_1_sc)
        n1_1_sc.setContainer(hp1)
        ##
        hp1.setProperty("nb_proc_per_node","1")
        hp4.setProperty("nb_proc_per_node","4")
        pg.setData([("m0",8),("m1",8),("m2",8),("m3",8)]) # virtual machine with 32 cores spread over 4 nodes
        assert(n1_0.getWeight().getSimpleLoopWeight()==-1.)
        assert(n1_1.getWeight().getSimpleLoopWeight()==4.)
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
        assert(list(fyto.getIDS())==list(range(8)))
        #############################
        #  Change weight of ForEach #
        #############################
        n1_0.setWeight(1)
        p.fitToPlayGround(pg)########### ZE CALL
        assert(hp4.getSizeOfPool()==8)# 32/4
        n1_0_sc.getContainer().forYourTestsOnly(fyto)
        assert(fyto.getContainerType()=="HPContainerShared")
        pd=fyto.getPD()
        assert(isinstance(pd,pilot.ContigPartDefinition))
        assert(pd.getStart()==0 and pd.getStop()==16)
        assert(fyto.getIDS()==(0,1,2,3))
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
        assert(list(fyto.getIDS())==list(range(8)))
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
        n1_0.setWeight(1.)
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
        n1_1.setWeight(4.)
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
        pg.setData([("m0",8),("m1",8),("m2",8),("m3",8),("m4",4)]) # virtual machine with 32 cores spread over 4 nodes
        assert(n1_0.getWeight().getSimpleLoopWeight()==1.)
        assert(n1_1.getWeight().getSimpleLoopWeight()==4.)
        p.fitToPlayGround(pg)########### ZE CALL
        assert(hp4.getSizeOfPool()==9)# 36/4
        fyto=pilot.ForTestOmlyHPContCls()
        n1_0_sc.getContainer().forYourTestsOnly(fyto)
        assert(fyto.getContainerType()=="HPContainerShared")
        pd=fyto.getPD()
        assert(isinstance(pd,pilot.ContigPartDefinition))
        assert(pd.getStart()==0 and pd.getStop()==16)
        assert(fyto.getIDS()==(0,1,2,3))
        #
        assert(hp1.getSizeOfPool()==36)# 36/1
        fyto=pilot.ForTestOmlyHPContCls()
        n1_1_sc.getContainer().forYourTestsOnly(fyto)
        assert(fyto.getContainerType()=="HPContainerShared")
        pd=fyto.getPD()
        assert(isinstance(pd,pilot.ContigPartDefinition))
        assert(pd.getStart()==20 and pd.getStop()==36)
        assert(fyto.getIDS()==(20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35))
        assert(n1_0.edGetNbOfBranchesPort().getPyObj()==4)
        assert(n1_1.edGetNbOfBranchesPort().getPyObj()==16)
        #
        fyto=pilot.ForTestOmlyHPContCls()
        n0.getContainer().forYourTestsOnly(fyto)
        assert(fyto.getContainerType()=="HPContainerShared")
        pd=fyto.getPD()
        assert(isinstance(pd,pilot.AllPartDefinition))
        assert(list(fyto.getIDS())==list(range(9)))
        fyto=pilot.ForTestOmlyHPContCls()
        n1_2.getContainer().forYourTestsOnly(fyto)
        assert(fyto.getContainerType()=="HPContainerShared")
        pd=fyto.getPD()
        assert(isinstance(pd,pilot.ContigPartDefinition))
        assert(list(fyto.getIDS())==[4])
        pass

    @unittest.skip("requires 2 machines in catalog")
    def test2(self):
        """ This test is desactivated because it requires multi nodes. To be moved at the right place to support this.
        This test is not launched here because it requires 2 machines in catalog"""
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

    def test3(self):
        """ First test coming from the big boss."""
        SALOMERuntime.RuntimeSALOME.setRuntime()
        r=SALOMERuntime.getSALOMERuntime()
        pg=pilot.PlayGround()
        pg.loadFromKernelCatalog()
        assert(len(pg.getData())!=0)
        p=r.createProc("p0")
        td=p.createType("double","double")
        tdd=p.createSequenceTc("seqdouble","seqdouble",td)
        hp1=r.createContainer("HPSalome") ; hp1.setName("HP1")
        #
        n0=r.createScriptNode("Salome","n0")
        n0.setExecutionMode("local")
        out0_0=n0.edAddOutputPort("o1",tdd)
        n0.setScript("""o1=[float(i)+0.1 for i in range(1000)]""")
        p.edAddChild(n0)
        #
        n1_0=r.createForEachLoop("n1_0",td)
        n2_0=r.createForEachLoop("n2_0",td)
        p.edAddChild(n1_0)
        p.edAddChild(n2_0)
        p.edAddCFLink(n0,n1_0)
        p.edAddCFLink(n1_0,n2_0)
        p.edAddLink(out0_0,n1_0.edGetSeqOfSamplesPort())
        p.edAddLink(out0_0,n2_0.edGetSeqOfSamplesPort())
        ##
        n1_0_sc=r.createScriptNode("Salome","n1_0_sc")
        n1_0.edAddChild(n1_0_sc)
        n1_0_sc.setExecutionMode("remote")
        n1_0_sc.setScript("""2*i1""")
        i1_0_sc=n1_0_sc.edAddInputPort("i1",td)
        p.edAddLink(n1_0.edGetSamplePort(),i1_0_sc)
        n1_0_sc.setContainer(hp1)
        ##
        n2_0_sc=r.createScriptNode("Salome","n2_0_sc")
        n2_0.edAddChild(n2_0_sc)
        n2_0_sc.setExecutionMode("remote")
        n2_0_sc.setScript("""2*i1""")
        i2_0_sc=n2_0_sc.edAddInputPort("i1",td)
        p.edAddLink(n2_0.edGetSamplePort(),i2_0_sc)
        n2_0_sc.setContainer(hp1)
        ##
        hp1.setProperty("nb_proc_per_node","1")
        pg.setData([("localhost",3)])
        p.fitToPlayGround(pg)########### ZE CALL
        assert(hp1.getSizeOfPool()==3)
        fyto=pilot.ForTestOmlyHPContCls()
        n1_0_sc.getContainer().forYourTestsOnly(fyto)
        assert(fyto.getContainerType()=="HPContainerShared")
        pd=fyto.getPD()
        assert(isinstance(pd,pilot.AllPartDefinition))
        assert(fyto.getIDS()==(0,1,2))
        fyto=pilot.ForTestOmlyHPContCls()
        n2_0_sc.getContainer().forYourTestsOnly(fyto)
        assert(fyto.getContainerType()=="HPContainerShared")
        pd=fyto.getPD()
        assert(isinstance(pd,pilot.AllPartDefinition))
        assert(fyto.getIDS()==(0,1,2))
        assert(n1_0.edGetNbOfBranchesPort().getPyObj()==3)
        assert(n2_0.edGetNbOfBranchesPort().getPyObj()==3)
        #
        exe=pilot.ExecutorSwig()
        assert(p.getState()==pilot.READY)
        exe.RunW(p,0)
        assert(len(set(hp1.getKernelContainerNames()))==3)
        pass
    
    def test4(self):
        """ two branch whose on with one elementary node and on foreach"""
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
        ##
        n0=r.createScriptNode("Salome","n0")
        n0.setExecutionMode("remote")
        out0_0=n0.edAddOutputPort("o1",tdd)
        n0.setScript("""o1=[float(i)+0.1 for i in range(1000)]""")
        n0.setContainer(hp1)
        p.edAddChild(n0)
        #
        b0 = r.createBloc("Bloc0")
        p.edAddChild(b0)
        p.edAddCFLink(n0,b0)
        ##
        n1_0=r.createForEachLoop("n1_0",td)
        b0.edAddChild(n1_0)
        p.edAddLink(out0_0,n1_0.edGetSeqOfSamplesPort())
        n1_0.setWeight(10.)
        ##
        n1_0_sc=r.createScriptNode("Salome","n1_0_sc")
        n1_0.edAddChild(n1_0_sc)
        n1_0_sc.setExecutionMode("remote")
        n1_0_sc.setScript("""2*i1""")
        i1_0_sc=n1_0_sc.edAddInputPort("i1",td)
        p.edAddLink(n1_0.edGetSamplePort(),i1_0_sc)
        n1_0_sc.setContainer(hp1)
        ##
        n1_0_1=r.createScriptNode("Salome","n1_0_1")
        n1_0_1.setExecutionMode("remote")
        n1_0_1.setScript("""a=2""")
        b0.edAddChild(n1_0_1)
        p.edAddCFLink(n1_0,n1_0_1)
        n1_0_1.setContainer(hp1)  
        n1_0_1.setWeight(20.)        
        ##
        #
        n1_1=r.createForEachLoop("n1_1",td)
        p.edAddChild(n1_1)
        p.edAddCFLink(n0,n1_1)
        p.edAddLink(out0_0,n1_1.edGetSeqOfSamplesPort())
        n1_1.setWeight(100.)
        ##
        n1_1_sc=r.createScriptNode("Salome","n1_1_sc")
        n1_1.edAddChild(n1_1_sc)
        n1_1_sc.setExecutionMode("remote")
        n1_1_sc.setScript("""3*i1""")
        i1_1_sc=n1_1_sc.edAddInputPort("i1",td)
        p.edAddLink(n1_1.edGetSamplePort(),i1_1_sc)
        n1_1_sc.setContainer(hp1)
        
        hp1.setProperty("nb_proc_per_node","1")
        pg.setData([("m0",12)])
        w=pilot.ComplexWeight()
        b0.getWeightRegardingDPL(w)
        assert(w.getElementaryWeight()==20.)
        assert(w.calculateTotalLength(1)==30.)
        assert(w.calculateTotalLength(2)==25.)
        p.fitToPlayGround(pg)########### ZE CALL
        assert(n1_0.edGetNbOfBranchesPort().getPyObj()==7)
        assert(n1_1.edGetNbOfBranchesPort().getPyObj()==5)
        #
        fyto=pilot.ForTestOmlyHPContCls()
        n1_0_sc.getContainer().forYourTestsOnly(fyto)
        assert(fyto.getContainerType()=="HPContainerShared")
        pd=fyto.getPD()
        assert(isinstance(pd,pilot.ContigPartDefinition))
        print (pd.getStart(),pd.getStop())
        assert(pd.getStart()==0 and pd.getStop()==7)
        
        #########################
        ## change HPcontainer
        ## very important: if you change HPcontainer you have to reload the graph
        #########################
        p=r.createProc("p0")
        n0=r.createScriptNode("Salome","n0")
        n0.setExecutionMode("remote")
        out0_0=n0.edAddOutputPort("o1",tdd)
        n0.setScript("""o1=[float(i)+0.1 for i in range(1000)]""")
        n0.setContainer(hp1)
        p.edAddChild(n0)
        #
        b0 = r.createBloc("Bloc0")
        p.edAddChild(b0)
        p.edAddCFLink(n0,b0)
        ##
        n1_0=r.createForEachLoop("n1_0",td)
        b0.edAddChild(n1_0)
        p.edAddLink(out0_0,n1_0.edGetSeqOfSamplesPort())
        ##
        n1_0_sc=r.createScriptNode("Salome","n1_0_sc")
        n1_0.edAddChild(n1_0_sc)
        n1_0_sc.setExecutionMode("remote")
        n1_0_sc.setScript("""2*i1""")
        i1_0_sc=n1_0_sc.edAddInputPort("i1",td)
        p.edAddLink(n1_0.edGetSamplePort(),i1_0_sc)
        n1_0_sc.setContainer(hp1)
        ##
        n1_0_1=r.createForEachLoop("n1_0_1",td)
        b0.edAddChild(n1_0_1)
        p.edAddLink(out0_0,n1_0_1.edGetSeqOfSamplesPort())
        p.edAddCFLink(n1_0,n1_0_1)
        ##
        n1_0_1sc=r.createScriptNode("Salome","n1_0_1sc")
        n1_0_1.edAddChild(n1_0_1sc)
        n1_0_1sc.setExecutionMode("remote")
        n1_0_1sc.setScript("""a=2""")
        i1_0_1sc=n1_0_1sc.edAddInputPort("i1",td)
        p.edAddLink(n1_0_1.edGetSamplePort(),i1_0_1sc)
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
        ##
        hp4=p.createContainer("HP4","HPSalome")
        hp4.setProperty("nb_proc_per_node","4")
        n1_0.setWeight(40.)
        n1_0_1.setWeight(20)
        n1_1.setWeight(100.)
        n1_0_1sc.setContainer(hp4)
        w=pilot.ComplexWeight()
        b0.getWeightRegardingDPL(w)
        assert(w.getElementaryWeight()==-1.)
        assert(w.calculateTotalLength(4)==30.)
        assert(w.calculateTotalLength(8)==15.)
        pg.setData([("m0",120)])
        p.fitToPlayGround(pg)########### ZE CALL
        assert(n1_0.edGetNbOfBranchesPort().getPyObj()==64)
        assert(n1_0_1.edGetNbOfBranchesPort().getPyObj()==16)
        assert(n1_1.edGetNbOfBranchesPort().getPyObj()==56)
        #
        fyto=pilot.ForTestOmlyHPContCls()
        n1_0_sc.getContainer().forYourTestsOnly(fyto)
        assert(fyto.getContainerType()=="HPContainerShared")
        pd=fyto.getPD()
        assert(isinstance(pd,pilot.ContigPartDefinition))
        assert(pd.getStart()==0 and pd.getStop()==64)
        pass

    pass

if __name__ == '__main__':
    unittest.main()
