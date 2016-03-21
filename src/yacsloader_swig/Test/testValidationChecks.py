#! /usr/bin/env python
# -*- coding: utf-8 -*-
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

import unittest
import os
import sys
import pilot
import SALOMERuntime

class TestValidationChecks(unittest.TestCase):
  def test_foreach_links(self):
    """ Test tha add of illegal links within a foreach loop.
    """
    SALOMERuntime.RuntimeSALOME_setRuntime()
    runtime = pilot.getRuntime()
    
    schema = runtime.createProc("schema")
    ti=schema.getTypeCode("int")
    tiset = schema.createSequenceTc("", "seqint", ti)

    nw = runtime.createScriptNode("", "Worker")
    nw.edAddInputPort("i1", ti)
    nw.edAddInputPort("i2", ti)
    nw.edAddOutputPort("o1", ti)
    nw.setScript("o1=i1+i2")

    ni = runtime.createScriptNode("", "Init")
    ni.edAddInputPort("ii", ti)
    ni.edAddOutputPort("oi", ti)
    ni.setScript("o1=ii")

    nf = runtime.createScriptNode("", "Fin")
    nf.edAddInputPort("ifin", ti)
    nf.edAddOutputPort("ofin", ti)
    nf.setScript("ofin=ifin")

    npre = runtime.createScriptNode("", "PreProc")
    npre.edAddOutputPort("opre", ti)
    npre.setScript("opre=5")

    npost = runtime.createScriptNode("", "PostProc")
    npost.edAddInputPort("ipost", ti)

    fe = runtime.createForEachLoop("ForEach", ti)
    fe.getInputPort("nbBranches").edInitPy(2)
    fe.getInputPort("SmplsCollection").edInitPy([1, 2, 3, 4])
    fe.edSetNode(nw)
    fe.edSetInitNode(ni)
    fe.edSetFinalizeNode(nf)
    
    schema.edAddChild(fe)
    schema.edAddChild(npost)
    
    def excpTest(op, ip):
      self.assertRaises(ValueError, schema.edAddLink, op, ip)
      pass
    
    # ForEachLoop tests
    excpTest(fe.getOutputPort("evalSamples"), npost.getInputPort("ipost"))
    excpTest(fe.getOutputPort("evalSamples"), ni.getInputPort("ii"))
    excpTest(fe.getOutputPort("evalSamples"), nf.getInputPort("ifin"))
    excpTest(fe.getOutputPort("evalSamples"), fe.getInputPort("nbBranches"))
    excpTest(ni.getOutputPort("oi"), fe.getInputPort("nbBranches"))
    excpTest(nw.getOutputPort("o1"), fe.getInputPort("nbBranches"))
    excpTest(nf.getOutputPort("ofin"), fe.getInputPort("nbBranches"))
    excpTest(fe.getOutputPort("evalSamples"), fe.getInputPort("SmplsCollection"))
    excpTest(ni.getOutputPort("oi"), fe.getInputPort("SmplsCollection"))
    excpTest(nw.getOutputPort("o1"), fe.getInputPort("SmplsCollection"))
    excpTest(nf.getOutputPort("ofin"), fe.getInputPort("SmplsCollection"))
    excpTest(nw.getOutputPort("o1"), nf.getInputPort("ifin"))
    excpTest(nw.getOutputPort("o1"), ni.getInputPort("ii"))
    excpTest(ni.getOutputPort("oi"), npost.getInputPort("ipost"))
    excpTest(nf.getOutputPort("ofin"), npost.getInputPort("ipost"))
    excpTest(nf.getOutputPort("ofin"), ni.getInputPort("ii"))
    
  def test_optim_links(self):
    """ Test tha add of illegal links within an optimization loop.
    """
    SALOMERuntime.RuntimeSALOME_setRuntime()
    runtime = pilot.getRuntime()
    #
    schema = runtime.createProc("schema")
    ti=schema.getTypeCode("int")
    #
    nw = runtime.createScriptNode("", "Worker")
    nw.edAddInputPort("i1", ti)
    nw.edAddInputPort("i2", ti)
    nw.edAddOutputPort("o1", ti)
    nw.setScript("o1=i1+i2")
    #
    ni = runtime.createScriptNode("", "Init")
    ni.edAddInputPort("ii", ti)
    ni.edAddOutputPort("oi", ti)
    ni.setScript("o1=ii")
    #
    nf = runtime.createScriptNode("", "Fin")
    nf.edAddInputPort("ifin", ti)
    nf.edAddOutputPort("ofin", ti)
    nf.setScript("ofin=ifin")
    #
    npre = runtime.createScriptNode("", "PreProc")
    npre.edAddOutputPort("opre", ti)
    npre.setScript("opre=5")
    #
    npost = runtime.createScriptNode("", "PostProc")
    npost.edAddInputPort("ipost", ti)
    #
    fe = runtime.createOptimizerLoop("OptLoop", "optim_plugin.py","myalgosync",True)
    fe.getInputPort("nbBranches").edInitPy(2)
    fe.getInputPort("algoInit").edInitPy(7)
    fe.edSetNode(nw)
    fe.edSetInitNode(ni)
    fe.edSetFinalizeNode(nf)
    #
    schema.edAddChild(fe)
    schema.edAddChild(npost)
    schema.edAddChild(npre)
    
    def excpTest(op, ip):
      self.assertRaises(ValueError, schema.edAddLink, op, ip)
      pass
    
    # ForEachLoop tests
    excpTest(fe.getOutputPort("evalSamples"), npost.getInputPort("ipost"))
    excpTest(fe.getOutputPort("evalSamples"), ni.getInputPort("ii"))
    excpTest(fe.getOutputPort("evalSamples"), nf.getInputPort("ifin"))
    excpTest(fe.getOutputPort("evalSamples"), fe.getInputPort("nbBranches"))
    excpTest(nw.getOutputPort("o1"), nf.getInputPort("ifin"))
    excpTest(nw.getOutputPort("o1"), ni.getInputPort("ii"))
    excpTest(ni.getOutputPort("oi"), npost.getInputPort("ipost"))
    excpTest(nf.getOutputPort("ofin"), npost.getInputPort("ipost"))
    excpTest(nf.getOutputPort("ofin"), ni.getInputPort("ii"))
    # Specific OptimizerLoop tests
    excpTest(nw.getOutputPort("o1"), fe.getInputPort("nbBranches"))
    excpTest(ni.getOutputPort("oi"), fe.getInputPort("nbBranches"))
    excpTest(nf.getOutputPort("ofin"), fe.getInputPort("nbBranches"))
    excpTest(nw.getOutputPort("o1"), fe.getInputPort("algoInit"))
    excpTest(ni.getOutputPort("oi"), fe.getInputPort("algoInit"))
    excpTest(nf.getOutputPort("ofin"), fe.getInputPort("algoInit"))
    excpTest(ni.getOutputPort("oi"), fe.getInputPort("evalResults"))
    excpTest(nf.getOutputPort("ofin"), fe.getInputPort("evalResults"))
    excpTest(npre.getOutputPort("opre"), fe.getInputPort("evalResults"))
    excpTest(fe.getOutputPort("evalSamples"), fe.getInputPort("algoInit"))
    excpTest(fe.getOutputPort("algoResults"), nw.getInputPort("i1"))
    excpTest(fe.getOutputPort("algoResults"), ni.getInputPort("ii"))
    excpTest(fe.getOutputPort("algoResults"), nf.getInputPort("ifin"))
    excpTest(nw.getOutputPort("o1"), npost.getInputPort("ipost"))
    excpTest(ni.getOutputPort("oi"), npost.getInputPort("ipost"))
    excpTest(nf.getOutputPort("ofin"), npost.getInputPort("ipost"))

if __name__ == '__main__':
    unittest.main()