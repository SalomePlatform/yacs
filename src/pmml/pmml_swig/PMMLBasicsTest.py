# -*- coding: utf-8 -*-

# imports Salomé
from PMML import PMMLlib, kANN, kLR

# imports python
import unittest
import exceptions
from exceptions import RuntimeError
import os
import shutil

class PMMLBasicsTest(unittest.TestCase):

    def setUp(self):
        pmmlRootDir = os.getenv("YACS_ROOT_DIR");
        self.resourcesDir = os.path.join(pmmlRootDir,"share","salome","resources","pmml");
        self.resourcesDir += os.sep ;
        self.tmpDir = "/tmp/";
        self.tmpDir += os.environ['LOGNAME']; # ("USER");
        self.tmpDir += "/PmmlUnitTest/";
        if ( not os.path.exists(self.tmpDir) ):
            os.mkdir(self.tmpDir);
            pass
        pass

    def tearDown(self):
        if ( os.path.exists(self.tmpDir) ):
            shutil.rmtree(self.tmpDir);
            pass 
        pass

    def testExportPythonNeuralNet(self):
        pmmlFile = self.resourcesDir + "ann_model.pmml";
        model = "sANNName";
        exportPyScript = self.tmpDir + "swigTestExportPythonNeuralNet.py";
        refPyFilename = self.resourcesDir + "unittest_ref_ann_model.py";
        refLines = file(refPyFilename).readlines(); 
        #
        p = PMMLlib( pmmlFile );
        p.SetCurrentModel( model, kANN );
        p.ExportPython( exportPyScript, "myTestFunc", 
                        "File used by unit test\n PMMLBasicsTest1::testExportNeuralNetworkPython" );
        myLines = file(exportPyScript).readlines();
        self.assertEqual( len(myLines), len(refLines) );
        for (i,line) in enumerate(myLines):
            self.assertEqual( line, refLines[i] );
            pass
        pass
  
    def testExportPythonRegression(self):
        pmmlFile = self.resourcesDir + "lr_model.pmml";
        model = "Modeler[LinearRegression]Tds[steamplant]Predictor[x6:x8:x6x8:x6x6x8]Target[x1]";
        exportPyScript = self.tmpDir + "swigTestExportPythonRegression.py";
        refPyFilename = self.resourcesDir + "unittest_ref_lr_model.py";
        refLines = file(refPyFilename).readlines(); 
        #
        p = PMMLlib( pmmlFile );
        p.SetCurrentModel( model, kLR );
        p.ExportPython( exportPyScript, "myTestFunc", 
                               "File used by unit test\n PMMLBasicsTest1::testExportLinearRegressionPython" );
        myLines = file(exportPyScript).readlines();
        self.assertEqual( len(myLines), len(refLines) );
        for (i,line) in enumerate(myLines):
            self.assertEqual( line, refLines[i] );
            pass
        pass

    def testPmmlFileNotReadable(self):
        self.assertRaises( RuntimeError, PMMLlib, "0.mml" );
        pass

    def testPmmlFileNotReadable(self):
        pmmlFile = self.resourcesDir + "ann_model.pmml";
        model = "sANNName";
        p = PMMLlib(pmmlFile);
        self.assertRaises( RuntimeError, p.SetCurrentModel, model, kLR );
        pass

    def testPmmlFileNotWritable(self):
        p = PMMLlib();
        self.assertRaises( RuntimeError, p.Write );
        pass
    pass

unittest.main()
