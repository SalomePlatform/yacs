// Copyright (C) 2007-2016  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// Author : InckA


#include "PMMLBasicsTest1.hxx"
#include "PMMLlib.hxx"
#include "tools.hxx"

using namespace std;

void PMMLBasicsTest1::setUp()
{
    resourcesDir = "samples/";
#ifdef WIN32
    const char* user = std::getenv("USERPROFILE");
    std::string strUser("");
    if (user)
        strUser = std::string(user);
    else
        throw std::string("unable to get USERPROFILE");
    tmpDir = strUser;
    tmpDir += "\\tmp";
    tmpDir += "\\PmmlUnitTest\\";
    std::string cmd = "mkdir " + tmpDir; 
    system( cmd.c_str() );  
#else
    tmpDir = "/tmp/";
    tmpDir += getenv("USER");
    tmpDir += "/PmmlUnitTest/";
    std::string cmd = "mkdir -p " + tmpDir; 
    system( cmd.c_str() );  
#endif
}

void PMMLBasicsTest1::tearDown()
{
#ifdef WIN32
    string cmd = "rmdir " + tmpDir + " /s /q "; 
    system( cmd.c_str() );  
#else
    string cmd = "rm -rf " + tmpDir; 
    system( cmd.c_str() );    
#endif
}

//**************************************************************
//                                                             *
//                                                             *
//                                                             *
//  méthodes communes à tous les types de modèles              *
//                                                             *
//                                                             *
//                                                             *
//**************************************************************

void PMMLBasicsTest1::testMakeLog()
{
    string pmmlFile = resourcesDir + "ann_model.pmml";
    bool b = true;
    string logRef;
    logRef =  "**\n**** Display of PMMLlib ****\n";
    logRef += " **  _pmmlFile[";
    logRef += pmmlFile;
    logRef += "]\n";
    logRef += " **  _log[";
    logRef += (b?"1":"0");
    logRef += "]\n";  
    logRef += "**\n**** End of display of PMMLlib ****\n";    
    PMMLlib::PMMLlib p(pmmlFile, b);
    p.SetCurrentModel("sANNName", PMMLlib::kANN);
    CPPUNIT_ASSERT_EQUAL( p.makeLog(), logRef );
}

void PMMLBasicsTest1::testConstructorFileDoesNotExist()
{
    CPPUNIT_ASSERT_THROW( PMMLlib::PMMLlib p("0.mml", true) , std::string );  
}
  
void PMMLBasicsTest1::testSetCurrentModelWithNameAndTypekANNUnknownModel()
{
    PMMLlib::PMMLlib p(resourcesDir + "ann_model.pmml");
    CPPUNIT_ASSERT_THROW( p.SetCurrentModel("toto", PMMLlib::kANN), std::string );
}

void PMMLBasicsTest1::testSetCurrentModelWithNameAndTypekLRUnknownModel()
{
    PMMLlib::PMMLlib p(resourcesDir + "lr_model.pmml");
    CPPUNIT_ASSERT_THROW( p.SetCurrentModel( "toto", PMMLlib::kLR), std::string );
}

void PMMLBasicsTest1::testSetCurrentModelWithNameAndTypekUNDEFINEDUnknownModel()
{
    PMMLlib::PMMLlib p(resourcesDir + "lr_model.pmml");
    CPPUNIT_ASSERT_THROW( p.SetCurrentModel("toto", PMMLlib::kUNDEFINED), std::string );
}

void PMMLBasicsTest1::testSetCurrentModelWithNameAndType()
{
    PMMLlib::PMMLlib p(resourcesDir + "ann_model.pmml");
    p.SetCurrentModel("sANNName", PMMLlib::kANN);
    CPPUNIT_ASSERT_EQUAL( p.GetModelsNb(), 1 );
    CPPUNIT_ASSERT_EQUAL( p.GetCurrentModelType(), PMMLlib::kANN );
}

void PMMLBasicsTest1::testSetCurrentModelWithNameAndTypeTwoModelsWithSameNames()
{
    PMMLlib::PMMLlib p(resourcesDir + "two_models_ann_lr.pmml");
    p.SetCurrentModel("modelName", PMMLlib::kLR);
    CPPUNIT_ASSERT_EQUAL( p.GetModelsNb(), 2 );
    CPPUNIT_ASSERT_EQUAL( p.GetCurrentModelType(), PMMLlib::kLR ); 
}

void PMMLBasicsTest1::testSetCurrentModelWithNameWrongName()
{
    PMMLlib::PMMLlib pANN(resourcesDir + "ann_model.pmml");
    CPPUNIT_ASSERT_THROW( pANN.SetCurrentModel("toto"), std::string );      
}

void PMMLBasicsTest1::testSetCurrentModelWithNameTwoModelsWithSameNames()
{
    PMMLlib::PMMLlib p(resourcesDir + "two_models_ann_lr.pmml");
    CPPUNIT_ASSERT_THROW( p.SetCurrentModel("modelName"), std::string );
}

void PMMLBasicsTest1::testSetCurrentModelWithName()
{
    // kANN
    PMMLlib::PMMLlib pANN(resourcesDir + "ann_model.pmml");
    pANN.SetCurrentModel("sANNName");
    CPPUNIT_ASSERT_EQUAL( pANN.GetModelsNb(), 1 );
    CPPUNIT_ASSERT_EQUAL( pANN.GetCurrentModelType(), PMMLlib::kANN );
    // kLR
    string strModel("Modeler[LinearRegression]Tds[steamplant]Predictor[x6:x8:x6x8:x6x6x8]Target[x1]");
    PMMLlib::PMMLlib pLR(resourcesDir + "lr_model.pmml"); 
    pLR.SetCurrentModel(strModel);    
    CPPUNIT_ASSERT_EQUAL( pLR.GetModelsNb(), 1 );
    CPPUNIT_ASSERT_EQUAL( pLR.GetCurrentModelType(), PMMLlib::kLR );
}
  
void PMMLBasicsTest1::testSetCurrentModelNoModel()
{
    PMMLlib::PMMLlib p(resourcesDir + "no_model.pmml");
    CPPUNIT_ASSERT_THROW( p.SetCurrentModel(), std::string );
}

void PMMLBasicsTest1::testSetCurrentModelMoreThanOneModel()
{
    PMMLlib::PMMLlib p(resourcesDir + "two_models_ann_lr.pmml");
    CPPUNIT_ASSERT_THROW( p.SetCurrentModel(), std::string );
}

void PMMLBasicsTest1::testSetCurrentModel()
{
    // kANN
    PMMLlib::PMMLlib pANN(resourcesDir + "ann_model.pmml");
    pANN.SetCurrentModel();
    CPPUNIT_ASSERT_EQUAL( pANN.GetModelsNb(), 1 );
    CPPUNIT_ASSERT_EQUAL( pANN.GetCurrentModelType(), PMMLlib::kANN );
    CPPUNIT_ASSERT_EQUAL( pANN.GetCurrentModelName(), string("sANNName") );    
    // kLR
    string strModel("Modeler[LinearRegression]Tds[steamplant]Predictor[x6:x8:x6x8:x6x6x8]Target[x1]");
    PMMLlib::PMMLlib pLR(resourcesDir + "lr_model.pmml"); 
    pLR.SetCurrentModel();    
    CPPUNIT_ASSERT_EQUAL( pLR.GetModelsNb(), 1 );
    CPPUNIT_ASSERT_EQUAL( pLR.GetCurrentModelType(), PMMLlib::kLR );
    CPPUNIT_ASSERT_EQUAL( pLR.GetCurrentModelName(), strModel );       
}

void PMMLBasicsTest1::testGetModelsNbkANN()
{
    PMMLlib::PMMLlib p(resourcesDir + "ann_model.pmml");
    CPPUNIT_ASSERT_EQUAL( p.GetModelsNb(), 1 );    
}

void PMMLBasicsTest1::testGetModelsNbkLR()
{
    PMMLlib::PMMLlib p(resourcesDir + "lr_model.pmml");
    CPPUNIT_ASSERT_EQUAL( p.GetModelsNb(), 1 );    
}

void PMMLBasicsTest1::testWrite() 
{
    string refPmmlFilename = resourcesDir + "ann_model.pmml";
    string pmmlFilename = tmpDir + "unittest_generated_ann_model.pmml";
    
    PMMLlib::PMMLlib p(refPmmlFilename);
    p.SetCurrentModel( string("sANNName"), PMMLlib::kANN);                  
    p.Write(pmmlFilename);
    CPPUNIT_ASSERT_EQUAL( areFilesEqual( refPmmlFilename, pmmlFilename), 0 ); 
}

void PMMLBasicsTest1::testWriteNotExistingFile() 
{   
    PMMLlib::PMMLlib p;             
    CPPUNIT_ASSERT_THROW( p.Write(), std::string ); 
}

void PMMLBasicsTest1::testUnlinkNode()
{
    string pmmlFilename = tmpDir + "unittest_unlinked_ann_model.pmml";  
    string refPmmlFilename = resourcesDir + "ann_model.pmml";
    PMMLlib::PMMLlib refP(refPmmlFilename);
    refP.SetCurrentModel( string("sANNName"), PMMLlib::kANN);  
    refP.UnlinkNode();
    refP.Write(pmmlFilename);
    PMMLlib::PMMLlib p2(pmmlFilename);
    CPPUNIT_ASSERT_EQUAL( p2.GetModelsNb(), 0 );
}

void PMMLBasicsTest1::testBackupNode()
{
    string pmmlFilename = tmpDir + "unittest_backup_ann_model.pmml";  
    string refPmmlFilename = resourcesDir + "ann_model.pmml";
    PMMLlib::PMMLlib refP(refPmmlFilename, true);
    refP.SetCurrentModel( string("sANNName"), PMMLlib::kANN);  
    refP.BackupNode(); // rename sANNName to sANNName_0
    refP.BackupNode(); // rename sANNName_0 to sANNName_1
    // Create the PMML test file
    refP.Write(pmmlFilename);
    PMMLlib::PMMLlib p(pmmlFilename, true);
    // Still one model: 
    CPPUNIT_ASSERT_EQUAL( p.GetModelsNb(), 1 );
    // Its name is sANNName_1:
    p.SetCurrentModel(string("sANNName_1"), PMMLlib::kANN);
    // XML tree for NeuralNetwork sANNName_1 is the same than in sANNName
    CPPUNIT_ASSERT_EQUAL( p.GetNbOutputs(), 1 );    
    // sANNName is not there anymore: SetCurrentModel throws an exception
    // You'll have to build the new structure from scratch 
    CPPUNIT_ASSERT_THROW( p.SetCurrentModel(string("sANNName"), PMMLlib::kANN), std::string );
}

//**************************************************************
//                                                             *
//                                                             *
//                                                             *
//  méthodes propres au NeuralNetwork                          *
//                                                             *
//                                                             *
//                                                             *
//**************************************************************

void PMMLBasicsTest1::testGetNbInputsForbiddenModelType()
{
    string strModel("Modeler[LinearRegression]Tds[steamplant]Predictor[x6:x8:x6x8:x6x6x8]Target[x1]");
    PMMLlib::PMMLlib p(resourcesDir + "lr_model.pmml");
    p.SetCurrentModel( strModel, PMMLlib::kLR);  
    CPPUNIT_ASSERT_THROW( p.GetNbInputs(), std::string );     
}

void PMMLBasicsTest1::testGetNbInputs8()
{  
    PMMLlib::PMMLlib p(resourcesDir + "ann_model.pmml");
    p.SetCurrentModel( string("sANNName"), PMMLlib::kANN );
    CPPUNIT_ASSERT_EQUAL( p.GetNbInputs(), 8 );      
}


void PMMLBasicsTest1::testGetNbOutputsForbiddenModelType()
{
    string strModel("Modeler[LinearRegression]Tds[steamplant]Predictor[x6:x8:x6x8:x6x6x8]Target[x1]");
    PMMLlib::PMMLlib p(resourcesDir + "lr_model.pmml");
    p.SetCurrentModel( strModel, PMMLlib::kLR);  
    CPPUNIT_ASSERT_THROW( p.GetNbOutputs(), std::string ); 
}

void PMMLBasicsTest1::testGetNbOutputs2()
{    
    PMMLlib::PMMLlib p(resourcesDir + "ann_model_2.pmml");
    p.SetCurrentModel( string("sANNName"), PMMLlib::kANN );
    CPPUNIT_ASSERT_EQUAL( p.GetNbOutputs(), 2 );     
}


void PMMLBasicsTest1::testGetNameInputForbiddenModelType()
{  
    string strModel("Modeler[LinearRegression]Tds[steamplant]Predictor[x6:x8:x6x8:x6x6x8]Target[x1]");
    PMMLlib::PMMLlib p(resourcesDir + "lr_model.pmml");
    p.SetCurrentModel( strModel, PMMLlib::kLR);  
    CPPUNIT_ASSERT_THROW( p.GetNameInput(0), std::string );   
}

void PMMLBasicsTest1::testGetNameInputIndexOutOfRange()
{  
    PMMLlib::PMMLlib p(resourcesDir + "ann_model.pmml");
    p.SetCurrentModel( string("sANNName"), PMMLlib::kANN );  
    CPPUNIT_ASSERT_EQUAL( p.GetNameInput(18), string("") );  
}

void PMMLBasicsTest1::testGetNameInput()
{ 
    PMMLlib::PMMLlib p(resourcesDir + "ann_model.pmml");
    p.SetCurrentModel( string("sANNName"), PMMLlib::kANN );
    CPPUNIT_ASSERT_EQUAL( p.GetNameInput(2), string("tu") ); 
}  

void PMMLBasicsTest1::testGetNameOutputForbiddenModelType()
{  
    string strModel("Modeler[LinearRegression]Tds[steamplant]Predictor[x6:x8:x6x8:x6x6x8]Target[x1]");
    PMMLlib::PMMLlib p(resourcesDir + "lr_model.pmml");
    p.SetCurrentModel( strModel, PMMLlib::kLR);  
    CPPUNIT_ASSERT_THROW( p.GetNameOutput(0), std::string );   
}

void PMMLBasicsTest1::testGetNameOutputIndexOutOfRange()
{  
    PMMLlib::PMMLlib p(resourcesDir + "ann_model.pmml");
    p.SetCurrentModel( string("sANNName"), PMMLlib::kANN );  
    CPPUNIT_ASSERT_EQUAL( p.GetNameOutput(3), string("") );  
}

void PMMLBasicsTest1::testGetNameOutput()
{ 
    PMMLlib::PMMLlib p(resourcesDir + "ann_model.pmml");
    p.SetCurrentModel( string("sANNName"), PMMLlib::kANN );
    CPPUNIT_ASSERT_EQUAL( p.GetNameOutput(0), string("yhat") ); 
}  


void PMMLBasicsTest1::testGetNormalisationInputForbiddenType()
{
    double * dnorm = new double[2];  
    string strModel("Modeler[LinearRegression]Tds[steamplant]Predictor[x6:x8:x6x8:x6x6x8]Target[x1]");
    PMMLlib::PMMLlib p(resourcesDir + "lr_model.pmml");
    p.SetCurrentModel( strModel, PMMLlib::kLR);  
    CPPUNIT_ASSERT_THROW( p.GetNormalisationInput(0, dnorm), std::string ); 
    delete [] dnorm ;    
}

void PMMLBasicsTest1::testGetNormalisationInputIndexUnknown()
{
    double * dnorm = new double[2];
    PMMLlib::PMMLlib p(resourcesDir + "ann_model.pmml");
    p.SetCurrentModel( string("sANNName"), PMMLlib::kANN );
    p.GetNormalisationInput(18, dnorm) ;
    CPPUNIT_ASSERT_EQUAL( dnorm[0], 0.0 ); 
    CPPUNIT_ASSERT_EQUAL( dnorm[1], 0.0 );   
    delete [] dnorm ;
}

void PMMLBasicsTest1::testGetNormalisationInputkMinusOneOne()
{
    // Valeurs de reference
    double dorig1Ref = 2;   
    double dorig2Ref = 1.400018e+03;
    double * dnormRef = new double[2]; 
    dnormRef[0] = dorig1Ref ;
    dnormRef[1] = dorig2Ref ;
    double * dnorm = new double[2];
    PMMLlib::PMMLlib p(resourcesDir + "ann_model_2.pmml");
    p.SetCurrentModel( string("sANNName"), PMMLlib::kANN );
    p.GetNormalisationInput(6, dnorm) ;
    CPPUNIT_ASSERT_EQUAL( dnorm[0], dnormRef[0] ); 
    CPPUNIT_ASSERT_EQUAL( dnorm[1], dnormRef[1] );
    delete [] dnormRef ;
    delete [] dnorm ;  
}

void PMMLBasicsTest1::testGetNormalisationInputkCRkZeroOne()
{
    // Valeurs de reference
    double dnorm1Ref = -5.780019e-02;   
    double dorig2Ref = 1.095001e+04;
    double * dnormRef = new double[2]; 
    dnormRef[0] = dorig2Ref ;
    dnormRef[1] = -1.0 * dorig2Ref * dnorm1Ref ;
    double * dnorm = new double[2];
    PMMLlib::PMMLlib p(resourcesDir + "ann_model.pmml");
    p.SetCurrentModel( string("sANNName"), PMMLlib::kANN );
    p.GetNormalisationInput(7, dnorm) ;
    CPPUNIT_ASSERT_EQUAL( dnorm[0], dnormRef[0] ); 
    CPPUNIT_ASSERT_EQUAL( dnorm[1], dnormRef[1] );
    delete [] dnormRef ;
    delete [] dnorm ;
}

void PMMLBasicsTest1::testGetNormalisationOutputForbiddenType()
{
    double * dnorm = new double[2];  
    string strModel("Modeler[LinearRegression]Tds[steamplant]Predictor[x6:x8:x6x8:x6x6x8]Target[x1]");
    PMMLlib::PMMLlib p(resourcesDir + "lr_model.pmml");
    p.SetCurrentModel( strModel, PMMLlib::kLR);  
    CPPUNIT_ASSERT_THROW( p.GetNormalisationOutput(0, dnorm), std::string ); 
    delete [] dnorm ;    
}

void PMMLBasicsTest1::testGetNormalisationOutputIndexUnknown()
{
    double * dnorm = new double[2];
    PMMLlib::PMMLlib p(resourcesDir + "ann_model.pmml");
    p.SetCurrentModel( string("sANNName"), PMMLlib::kANN );
    p.GetNormalisationOutput(18, dnorm) ;
    CPPUNIT_ASSERT_EQUAL( dnorm[0], 0.0 ); 
    CPPUNIT_ASSERT_EQUAL( dnorm[1], 0.0 );   
    delete [] dnorm ;
}

void PMMLBasicsTest1::testGetNormalisationOutputkMinusOneOne()
{
    // Valeurs de reference
    double dorig1Ref = 2;   
    double dorig2Ref = 5.781171e+01;
    double * dnormRef = new double[2]; 
    dnormRef[0] = dorig1Ref ;
    dnormRef[1] = dorig2Ref ;
    double * dnorm = new double[2];
    PMMLlib::PMMLlib p(resourcesDir + "ann_model_2.pmml");
    p.SetCurrentModel( string("sANNName"), PMMLlib::kANN );
    p.GetNormalisationOutput(1, dnorm) ;
    CPPUNIT_ASSERT_EQUAL( dnorm[0], dnormRef[0] ); 
    CPPUNIT_ASSERT_EQUAL( dnorm[1], dnormRef[1] );
    delete [] dnormRef ;
    delete [] dnorm ;  
}

void PMMLBasicsTest1::testGetNormalisationOutputkCRkZeroOne()
{
    // Valeurs de reference
    double dnorm1Ref = -5.873935e-01;   
    double dorig2Ref = 7.781171e+01;
    double * dnormRef = new double[2]; 
    dnormRef[0] = dorig2Ref ;
    dnormRef[1] = -1.0 * dorig2Ref * dnorm1Ref ;
    double * dnorm = new double[2];
    PMMLlib::PMMLlib p(resourcesDir + "ann_model.pmml");
    p.SetCurrentModel( string("sANNName"), PMMLlib::kANN );
    p.GetNormalisationOutput(0, dnorm) ;
    CPPUNIT_ASSERT_EQUAL( dnorm[0], dnormRef[0] ); 
    CPPUNIT_ASSERT_EQUAL( dnorm[1], dnormRef[1] );
    delete [] dnormRef ;
    delete [] dnorm ;
}

void PMMLBasicsTest1::testGetNbLayersForbiddenType()
{
    double * dnorm = new double[2];  
    string strModel("Modeler[LinearRegression]Tds[steamplant]Predictor[x6:x8:x6x8:x6x6x8]Target[x1]");
    PMMLlib::PMMLlib p(resourcesDir + "lr_model.pmml");
    p.SetCurrentModel( strModel, PMMLlib::kLR);  
    CPPUNIT_ASSERT_THROW( p.GetNbLayers(), std::string ); 
    delete [] dnorm ;    
}

void PMMLBasicsTest1::testGetNbLayers()
{    
    PMMLlib::PMMLlib p(resourcesDir + "ann_model.pmml");
    p.SetCurrentModel( string("sANNName"), PMMLlib::kANN );
    CPPUNIT_ASSERT_EQUAL( p.GetNbHiddenLayers(), 2 );     
    CPPUNIT_ASSERT_EQUAL( p.GetNbLayers(), 4 );     
}

void PMMLBasicsTest1::testGetNbNeuronsAtLayerForbiddenType()
{
    double * dnorm = new double[2];  
    string strModel("Modeler[LinearRegression]Tds[steamplant]Predictor[x6:x8:x6x8:x6x6x8]Target[x1]");
    PMMLlib::PMMLlib p(resourcesDir + "lr_model.pmml");
    p.SetCurrentModel( strModel, PMMLlib::kLR);  
    CPPUNIT_ASSERT_THROW( p.GetNbNeuronsAtLayer(0), std::string ); 
    delete [] dnorm ;    
}

void PMMLBasicsTest1::testGetNbNeuronsAtLayerFromIndexOutOfRange()
{    
    PMMLlib::PMMLlib p(resourcesDir + "ann_model.pmml");
    p.SetCurrentModel( string("sANNName"), PMMLlib::kANN );
    CPPUNIT_ASSERT_EQUAL( p.GetNbNeuronsAtLayer(18), 0 );    
}

void PMMLBasicsTest1::testGetNbNeuronsAtLayer()
{    
    PMMLlib::PMMLlib p(resourcesDir + "ann_model.pmml");
    p.SetCurrentModel( string("sANNName"), PMMLlib::kANN );
    CPPUNIT_ASSERT_EQUAL( p.GetNbNeuronsAtLayer(0), 1 );    
}

void PMMLBasicsTest1::testGetNeuronBiasForbiddenType()
{
    double * dnorm = new double[2];  
    string strModel("Modeler[LinearRegression]Tds[steamplant]Predictor[x6:x8:x6x8:x6x6x8]Target[x1]");
    PMMLlib::PMMLlib p(resourcesDir + "lr_model.pmml");
    p.SetCurrentModel( strModel, PMMLlib::kLR);  
    CPPUNIT_ASSERT_THROW( p.GetNeuronBias(0,0), std::string ); 
    delete [] dnorm ;    
}

void PMMLBasicsTest1::testGetNeuronBiasFromLayerIndexOutOfRange()
{    
    PMMLlib::PMMLlib p(resourcesDir + "ann_model.pmml");
    p.SetCurrentModel( string("sANNName"), PMMLlib::kANN );
    CPPUNIT_ASSERT_EQUAL( p.GetNeuronBias(10,1), 0. );    
}

void PMMLBasicsTest1::testGetNeuronBiasFromNeuronIndexOutOfRange()
{    
    PMMLlib::PMMLlib p(resourcesDir + "ann_model.pmml");
    p.SetCurrentModel( string("sANNName"), PMMLlib::kANN );
    CPPUNIT_ASSERT_EQUAL( p.GetNeuronBias(0,10), 0. );    
}

void PMMLBasicsTest1::testGetNeuronBias()
{    
    PMMLlib::PMMLlib p(resourcesDir + "ann_model.pmml");
    p.SetCurrentModel( string("sANNName"), PMMLlib::kANN );
    CPPUNIT_ASSERT_EQUAL( p.GetNeuronBias(0,0), -1.263572e+00 );    
}

void PMMLBasicsTest1::testGetPrecNeuronSynapseForbiddenType()
{
    double * dnorm = new double[2];  
    string strModel("Modeler[LinearRegression]Tds[steamplant]Predictor[x6:x8:x6x8:x6x6x8]Target[x1]");
    PMMLlib::PMMLlib p(resourcesDir + "lr_model.pmml");
    p.SetCurrentModel( strModel, PMMLlib::kLR );  
    CPPUNIT_ASSERT_THROW( p.GetPrecNeuronSynapse(0,0,0), std::string ); 
    delete [] dnorm ;    
}

void PMMLBasicsTest1::testGetPrecNeuronSynapseFromLayerIndexOutOfRange()
{    
    PMMLlib::PMMLlib p(resourcesDir + "ann_model.pmml");
    p.SetCurrentModel( string("sANNName"), PMMLlib::kANN );
    CPPUNIT_ASSERT_EQUAL( p.GetPrecNeuronSynapse(10,1,1), 0. );    
}

void PMMLBasicsTest1::testGetPrecNeuronSynapseFromNeuronIndexOutOfRange()
{    
    PMMLlib::PMMLlib p(resourcesDir + "ann_model.pmml");
    p.SetCurrentModel( string("sANNName"), PMMLlib::kANN );
    CPPUNIT_ASSERT_EQUAL( p.GetPrecNeuronSynapse(1,10,1), 0. );    
}

void PMMLBasicsTest1::testGetPrecNeuronSynapseFromPrecIndexOutOfRange()
{    
    PMMLlib::PMMLlib p(resourcesDir + "ann_model.pmml");
    p.SetCurrentModel( string("sANNName"), PMMLlib::kANN );
    CPPUNIT_ASSERT_EQUAL( p.GetPrecNeuronSynapse(1,0,10), 0. );    
}

void PMMLBasicsTest1::testGetPrecNeuronSynapse()
{    
    PMMLlib::PMMLlib p(resourcesDir + "ann_model.pmml");
    p.SetCurrentModel( string("sANNName"), PMMLlib::kANN );
    CPPUNIT_ASSERT_EQUAL( p.GetPrecNeuronSynapse(0,0,7), 8.559675e-02 );    
}

void PMMLBasicsTest1::testReadNetworkStructure()
{
    PMMLlib::PMMLlib p(resourcesDir + "ann_model.pmml");
    p.SetCurrentModel( string("sANNName"), PMMLlib::kANN );
    string str = p.ReadNetworkStructure();
    CPPUNIT_ASSERT_EQUAL( p.ReadNetworkStructure(), string("rw:r:tu:tl:hu:hl:l:kw,1,@yhat") );
}

void PMMLBasicsTest1::testExportNeuralNetworkCpp()
{
    string refCppFilename = resourcesDir + "unittest_ref_ann_model.cpp";
    string cppFilename = tmpDir + "unittest_ann_model.cpp";
    PMMLlib::PMMLlib p(resourcesDir + "ann_model.pmml");
    p.SetCurrentModel( string("sANNName"), PMMLlib::kANN );
    p.ExportCpp(cppFilename, 
                "myTestFunc", 
                "File used by unit test\n PMMLBasicsTest1::testExportNeuralNetworkCpp" );
     CPPUNIT_ASSERT_EQUAL( areFilesEqual( refCppFilename, cppFilename), 0 ); 
}

void PMMLBasicsTest1::testExportNeuralNetworkFortran()
{
    string refFortranFilename = resourcesDir + "unittest_ref_ann_model.f";
    string fortranFilename = tmpDir + "unittest_ann_model.f";
    PMMLlib::PMMLlib p(resourcesDir + "ann_model.pmml");
    p.SetCurrentModel( string("sANNName"), PMMLlib::kANN );
    p.ExportFortran(fortranFilename, 
                    "myTestFunc", 
                    "File used by unit test\n PMMLBasicsTest1::testExportNeuralNetworkFortran" );
    CPPUNIT_ASSERT_EQUAL( areFilesEqual( refFortranFilename, fortranFilename), 0 ); 
}

void PMMLBasicsTest1::testExportNeuralNetworkPython()
{
    string refPyFilename = resourcesDir + "unittest_ref_ann_model.py";
    string pyFilename = tmpDir + "unittest_ann_model.py";    
    PMMLlib::PMMLlib p(resourcesDir + "ann_model.pmml");
    p.SetCurrentModel( string("sANNName"), PMMLlib::kANN );
    p.ExportPython(pyFilename, 
                   "myTestFunc", 
                   "File used by unit test\n PMMLBasicsTest1::testExportNeuralNetworkPython" );
    CPPUNIT_ASSERT_EQUAL( areFilesEqual( refPyFilename, pyFilename), 0 ); 
}

void PMMLBasicsTest1::testCreatePmmlNeuralNetwork() 
{
#ifdef WIN32
    std::string refPmmlFilename = resourcesDir + "win32_ann_model.pmml";
#else
    std::string refPmmlFilename = resourcesDir + "ann_model.pmml";
#endif
    std::string pmmlFilename = tmpDir + "unittest_generated_ann_model.pmml";
    
    // Données
    int nInput = 8;
    int nOutput = 1; 
    int nHidden = 1;
    int nWeights = 9;
    vector<string>fieldName(nInput+1);
    fieldName[0] = "rw";
    fieldName[1] = "r";
    fieldName[2] = "tu";
    fieldName[3] = "tl";
    fieldName[4] = "hu";
    fieldName[5] = "hl";
    fieldName[6] = "l";
    fieldName[7] = "kw";
    fieldName[8] = "yhat";     
    double orig1=0, norm2=0;
    vector<double>norm1(nInput);
    norm1[0] = -2.889932e-01;
    norm1[1] = -5.756638e-01;
    norm1[2] = -1.699313e-01;
    norm1[3] = -1.707007e-01;
    norm1[4] = -3.302777e-02;
    norm1[5] = -4.562070e-02;
    norm1[6] = -1.155882e-01;
    norm1[7] = -5.780019e-02;
    vector<double>orig2(nInput);
    orig2[0] = 9.999901e-02;
    orig2[1] = 2.504894e+04;
    orig2[2] = 8.933486e+04;
    orig2[3] = 8.955232e+01;
    orig2[4] = 1.050003e+03;
    orig2[5] = 7.600007e+02;
    orig2[6] = 1.400018e+03;
    orig2[7] = 1.095001e+04;
    vector<double>weights(nWeights);
    weights[0] = 7.536629e-01;
    weights[1] = 1.653660e-03;
    weights[2] = 4.725001e-03;
    weights[3] = 9.969786e-03;
    weights[4] = 1.787976e-01;
    weights[5] = -1.809809e-01;
    weights[6] = -1.735688e-01;
    weights[7] = 8.559675e-02; 
    weights[8] = 6.965512e+00;     

    // Construction
    PMMLlib::PMMLlib p;

    // Header node--------------------------------------------------------
    p.SetHeader("myCopyright", "Tests unitaires", "PMMLlib", "myVersion", "Tests unitaires PMMLlib");  
    // DataDictionnary node--------------------------------------------------------
    for(int j=0 ; j<nInput+1 ; j++)
    {
        p.AddDataField(fieldName[j], fieldName[j], "continuous", "float", "ClosedClosed", 0., 0.);
    }            
    // Set Neural Network (and type)---------------------------------------------   
    p.AddNeuralNetwork( string("sANNName"), PMMLlib::kREGRESSION);
    // Set Mining schema  
    for(int j=0 ; j<nInput ; j++)
    {
        p.AddMiningSchema(fieldName[j], "active");
    }
    p.AddMiningSchema(fieldName[nInput], "predicted"); 
    // Set NeuralInput
    for(int j=0 ; j<nInput ; j++)
    {
        p.AddNeuralInput(j, fieldName[j], "continuous", "float", orig1, norm1[j], orig2[j], norm2);
    }
    // Set neural layers
    p.AddNeuralLayer(PMMLlib::kTANH);
    for(int j=0 ; j<nHidden ; j++) // hidden layers
    {
        vector<double> tmp_weights(nInput);
        for(int i=0 ; i<nInput ; i++) 
        {
            tmp_weights[i] = weights[i];
        }        
        p.AddNeuron(nInput+j, -1.263572, nInput, 0, tmp_weights);
    }
    //
    p.AddNeuralLayer(PMMLlib::kIDENTITY);
    for(int j=0 ; j<nOutput ; j++) 
    {
        vector<double> tmp_weights(nHidden);        
        tmp_weights[0] =  weights[nWeights-1];
        p.AddNeuron(nInput+nHidden+j, -1.745483, nHidden, nInput, tmp_weights);
    }      
    // Set NeuralOutput
    p.AddNeuralOutput(nInput+nHidden, "yhat", "continuous", "float",
                          0,-5.873935e-01 , 
                          7.781171e+01, 0);
    p.Write( pmmlFilename );
    CPPUNIT_ASSERT_EQUAL( areFilesEqual( refPmmlFilename, pmmlFilename), 0 ); 
}

//**************************************************************
//                                                             *
//                                                             *
//                                                             *
//  méthodes propres au RegressionModel                        *
//                                                             *
//                                                             *
//                                                             *
//**************************************************************

void PMMLBasicsTest1::testHasInterceptForbiddenType()
{   
    PMMLlib::PMMLlib p(resourcesDir + "ann_model.pmml" );
    p.SetCurrentModel( string("sANNName"), PMMLlib::kANN);      
    CPPUNIT_ASSERT_THROW( p.HasIntercept(), std::string );    
}

void PMMLBasicsTest1::testHasIntercept()
{
    string strModel = "Modeler[LinearRegression]Tds[steamplant]Predictor[x6:x8:x6x8:x6x6x8]Target[x1]";
    PMMLlib::PMMLlib p(resourcesDir + "lr_model.pmml");
    p.SetCurrentModel( strModel, PMMLlib::kLR );
    CPPUNIT_ASSERT_EQUAL( p.HasIntercept(), true );  
}

void PMMLBasicsTest1::testHasInterceptNo()
{
    string strModel = "Modeler[LinearRegression]Tds[steamplant]Predictor[x6:x8:x6x8:x6x6x8]Target[x1]";
    PMMLlib::PMMLlib p(resourcesDir + "lr_model_2.pmml");
    p.SetCurrentModel( strModel, PMMLlib::kLR );
    CPPUNIT_ASSERT_EQUAL( p.HasIntercept(), false );  
}

void PMMLBasicsTest1::testGetRegressionTableInterceptForbiddenType()
{
    PMMLlib::PMMLlib p(resourcesDir + "ann_model.pmml");
    p.SetCurrentModel( string("sANNName"), PMMLlib::kANN);  
    CPPUNIT_ASSERT_THROW( p.GetRegressionTableIntercept(), std::string );  
}

void PMMLBasicsTest1::testGetRegressionTableIntercept()
{
    string strModel = "Modeler[LinearRegression]Tds[steamplant]Predictor[x6:x8:x6x8:x6x6x8]Target[x1]";
    PMMLlib::PMMLlib p(resourcesDir + "lr_model.pmml");
    p.SetCurrentModel( strModel, PMMLlib::kLR );
    CPPUNIT_ASSERT_EQUAL( p.GetRegressionTableIntercept(), 3.837365 );
}

void PMMLBasicsTest1::testReadRegressionStructure()
{
    string strModel = "Modeler[LinearRegression]Tds[steamplant]Predictor[x6:x8:x6x8:x6x6x8]Target[x1]";
    PMMLlib::PMMLlib p(resourcesDir + "lr_model.pmml");
    p.SetCurrentModel( strModel, PMMLlib::kLR );
    CPPUNIT_ASSERT_EQUAL( p.ReadRegressionStructure(), string("x6:x8,1,@x1") );  
}

void PMMLBasicsTest1::testGetNumericPredictorNb()
{
    string strModel = "Modeler[LinearRegression]Tds[steamplant]Predictor[x6:x8:x6x8:x6x6x8]Target[x1]";
    PMMLlib::PMMLlib p(resourcesDir + "lr_model.pmml");
    p.SetCurrentModel( strModel, PMMLlib::kLR );
    CPPUNIT_ASSERT_EQUAL( p.GetNumericPredictorNb(), 2 );
}

void PMMLBasicsTest1::testGetNumericPredictorNbForbiddenType()
{
    PMMLlib::PMMLlib p(resourcesDir + "ann_model.pmml");
    p.SetCurrentModel( string("sANNName"), PMMLlib::kANN); 
    CPPUNIT_ASSERT_THROW( p.GetNumericPredictorNb(), std::string );  
}

void PMMLBasicsTest1::testGetPredictorTermNb()
{
    string strModel = "Modeler[LinearRegression]Tds[steamplant]Predictor[x6:x8:x6x8:x6x6x8]Target[x1]";
    PMMLlib::PMMLlib p(resourcesDir + "lr_model.pmml");
    p.SetCurrentModel( strModel, PMMLlib::kLR );
    CPPUNIT_ASSERT_EQUAL( p.GetPredictorTermNb(), 2 );
}

void PMMLBasicsTest1::testGetPredictorTermNbForbiddenType()
{
    PMMLlib::PMMLlib p(resourcesDir + "ann_model.pmml");
    p.SetCurrentModel( string("sANNName"), PMMLlib::kANN); 
    CPPUNIT_ASSERT_THROW( p.GetNumericPredictorNb(), std::string ); 
}

void PMMLBasicsTest1::testGetNumericPredictorName()
{
    string strModel = "Modeler[LinearRegression]Tds[steamplant]Predictor[x6:x8:x6x8:x6x6x8]Target[x1]";
    PMMLlib::PMMLlib p(resourcesDir + "lr_model.pmml");
    p.SetCurrentModel( strModel, PMMLlib::kLR );
    CPPUNIT_ASSERT_EQUAL( p.GetNumericPredictorName(1), string("x8") );
}

void PMMLBasicsTest1::testGetNumericPredictorNameForbiddenType()
{
    PMMLlib::PMMLlib p(resourcesDir + "ann_model.pmml");
    p.SetCurrentModel( string("sANNName"), PMMLlib::kANN); 
    CPPUNIT_ASSERT_THROW( p.GetNumericPredictorName(0), std::string );  
}

void PMMLBasicsTest1::testGetNumericPredictorNamePredictorOutOfRange()
{
    string strModel = "Modeler[LinearRegression]Tds[steamplant]Predictor[x6:x8:x6x8:x6x6x8]Target[x1]";
    PMMLlib::PMMLlib p(resourcesDir + "lr_model.pmml");
    p.SetCurrentModel( strModel, PMMLlib::kLR );
    CPPUNIT_ASSERT_EQUAL( p.GetNumericPredictorName(5), string("") );  
} 

void PMMLBasicsTest1::testGetPredictorTermName()
{
    string strModel = "Modeler[LinearRegression]Tds[steamplant]Predictor[x6:x8:x6x8:x6x6x8]Target[x1]";
    PMMLlib::PMMLlib p(resourcesDir + "lr_model.pmml");
    p.SetCurrentModel( strModel, PMMLlib::kLR );
    CPPUNIT_ASSERT_EQUAL( p.GetPredictorTermName(1), string("x6x6x8") );
}

void PMMLBasicsTest1::testGetPredictorTermNameForbiddenType()
{
    PMMLlib::PMMLlib p(resourcesDir + "ann_model.pmml");
    p.SetCurrentModel( string("sANNName"), PMMLlib::kANN); 
    CPPUNIT_ASSERT_THROW( p.GetPredictorTermName(0), std::string );
}

void PMMLBasicsTest1::testGetPredictorTermNamePredictorOutOfRange()
{
    string strModel = "Modeler[LinearRegression]Tds[steamplant]Predictor[x6:x8:x6x8:x6x6x8]Target[x1]";
    PMMLlib::PMMLlib p(resourcesDir + "lr_model.pmml");
    p.SetCurrentModel( strModel, PMMLlib::kLR );
    CPPUNIT_ASSERT_EQUAL( p.GetPredictorTermName(5), string("") );  
} 
    
void PMMLBasicsTest1::testGetNumericPredictorCoefficient()
{
    string strModel = "Modeler[LinearRegression]Tds[steamplant]Predictor[x6:x8:x6x8:x6x6x8]Target[x1]";
    PMMLlib::PMMLlib p(resourcesDir + "lr_model.pmml");
    p.SetCurrentModel( strModel, PMMLlib::kLR );
    CPPUNIT_ASSERT_EQUAL( p.GetNumericPredictorCoefficient(1), 0.1428838 );
}

void PMMLBasicsTest1::testGetNumericPredictorCoefficientForbiddenType()
{
    PMMLlib::PMMLlib p(resourcesDir + "ann_model.pmml");
    p.SetCurrentModel( string("sANNName"), PMMLlib::kANN); 
    CPPUNIT_ASSERT_THROW( p.GetNumericPredictorCoefficient(0), std::string );
}

void PMMLBasicsTest1::testGetNumericPredictorCoefficientPredictorOutOfRange()
{
    string strModel = "Modeler[LinearRegression]Tds[steamplant]Predictor[x6:x8:x6x8:x6x6x8]Target[x1]";
    PMMLlib::PMMLlib p(resourcesDir + "lr_model.pmml");
    p.SetCurrentModel( strModel, PMMLlib::kLR );
    CPPUNIT_ASSERT_EQUAL( p.GetNumericPredictorCoefficient(2), 0. );  
}

void PMMLBasicsTest1::testGetPredictorTermCoefficient()
{
    string strModel = "Modeler[LinearRegression]Tds[steamplant]Predictor[x6:x8:x6x8:x6x6x8]Target[x1]";
    PMMLlib::PMMLlib p(resourcesDir + "lr_model.pmml");
    p.SetCurrentModel( strModel, PMMLlib::kLR );
    CPPUNIT_ASSERT_EQUAL( p.GetPredictorTermCoefficient(0), -0.02201903 );
}

void PMMLBasicsTest1::testGetPredictorTermCoefficientForbiddenType()
{
    PMMLlib::PMMLlib p(resourcesDir + "ann_model.pmml");
    p.SetCurrentModel( string("sANNName"), PMMLlib::kANN); 
    CPPUNIT_ASSERT_THROW( p.GetPredictorTermCoefficient(0), std::string );
}

void PMMLBasicsTest1::testGetPredictorTermCoefficientPredictorOutOfRange()
{
    string strModel = "Modeler[LinearRegression]Tds[steamplant]Predictor[x6:x8:x6x8:x6x6x8]Target[x1]";
    PMMLlib::PMMLlib p(resourcesDir + "lr_model.pmml");
    p.SetCurrentModel( strModel, PMMLlib::kLR );
    CPPUNIT_ASSERT_EQUAL( p.GetPredictorTermCoefficient(2), 0.);  
}

void PMMLBasicsTest1::testGetPredictorTermFieldRefNb()
{
    string strModel = "Modeler[LinearRegression]Tds[steamplant]Predictor[x6:x8:x6x8:x6x6x8]Target[x1]";
    PMMLlib::PMMLlib p(resourcesDir + "lr_model.pmml");
    p.SetCurrentModel( strModel, PMMLlib::kLR );
    CPPUNIT_ASSERT_EQUAL( p.GetPredictorTermFieldRefNb(1), 3 );
}

void PMMLBasicsTest1::testGetPredictorTermFieldRefNbForbiddenType()
{
    PMMLlib::PMMLlib p(resourcesDir + "ann_model.pmml");
    p.SetCurrentModel( string("sANNName"), PMMLlib::kANN); 
    CPPUNIT_ASSERT_THROW( p.GetPredictorTermFieldRefNb(0), std::string );  
}

void PMMLBasicsTest1::testGetPredictorTermFieldRefNbPredictorOutOfRange()
{
    string strModel = "Modeler[LinearRegression]Tds[steamplant]Predictor[x6:x8:x6x8:x6x6x8]Target[x1]";
    PMMLlib::PMMLlib p(resourcesDir + "lr_model.pmml");
    p.SetCurrentModel( strModel, PMMLlib::kLR );
    CPPUNIT_ASSERT_EQUAL( p.GetPredictorTermFieldRefNb(12), 0 );  
}

void PMMLBasicsTest1::testGetPredictorTermFieldRefName()
{
    string strModel = "Modeler[LinearRegression]Tds[steamplant]Predictor[x6:x8:x6x8:x6x6x8]Target[x1]";
    PMMLlib::PMMLlib p(resourcesDir + "lr_model.pmml");
    p.SetCurrentModel( strModel, PMMLlib::kLR );
    CPPUNIT_ASSERT_EQUAL( p.GetPredictorTermFieldRefName(1,1), string("x6") );
}

void PMMLBasicsTest1::testGetPredictorTermFieldRefNameForbiddenType()
{
    PMMLlib::PMMLlib p(resourcesDir + "ann_model.pmml");
    p.SetCurrentModel( string("sANNName"), PMMLlib::kANN); 
    CPPUNIT_ASSERT_THROW( p.GetPredictorTermFieldRefName(1,1), std::string );  
}

void PMMLBasicsTest1::testGetPredictorTermFieldRefNamePredictorOutOfRange()   
{
    string strModel = "Modeler[LinearRegression]Tds[steamplant]Predictor[x6:x8:x6x8:x6x6x8]Target[x1]";
    PMMLlib::PMMLlib p(resourcesDir + "lr_model.pmml");
    p.SetCurrentModel( strModel, PMMLlib::kLR );
    CPPUNIT_ASSERT_EQUAL( p.GetPredictorTermFieldRefName(3,1), string("") );  
}

void PMMLBasicsTest1::testGetPredictorTermFieldRefNameFieldOutOfRange()
{
    string strModel = "Modeler[LinearRegression]Tds[steamplant]Predictor[x6:x8:x6x8:x6x6x8]Target[x1]";
    PMMLlib::PMMLlib p(resourcesDir + "lr_model.pmml");
    p.SetCurrentModel( strModel, PMMLlib::kLR );
    CPPUNIT_ASSERT_EQUAL( p.GetPredictorTermFieldRefName(1,5), string("") );  
}

void PMMLBasicsTest1::testExportLinearRegressionCpp()
{
    string refCppFilename = resourcesDir + "unittest_ref_lr_model.cpp";
    string cppFilename = tmpDir + "unittest_lr_model.cpp"; 
    string strModel = "Modeler[LinearRegression]Tds[steamplant]Predictor[x6:x8:x6x8:x6x6x8]Target[x1]";   
    PMMLlib::PMMLlib p(resourcesDir + "lr_model.pmml");
    p.SetCurrentModel( strModel, PMMLlib::kLR );
    p.ExportCpp(cppFilename, 
                string("myTestFunc"), 
                "File used by unit test\n PMMLBasicsTest1::testExportLinearRegressionCpp" );    
    CPPUNIT_ASSERT_EQUAL( areFilesEqual( refCppFilename, cppFilename), 0 );  
}

void PMMLBasicsTest1::testExportLinearRegressionFortran()
{
    string refFortranFilename = resourcesDir + "unittest_ref_lr_model.f";
    string fortranFilename = tmpDir + "unittest_lr_model.f"; 
    string strModel = "Modeler[LinearRegression]Tds[steamplant]Predictor[x6:x8:x6x8:x6x6x8]Target[x1]";   
    PMMLlib::PMMLlib p(resourcesDir + "lr_model.pmml");
    p.SetCurrentModel( strModel, PMMLlib::kLR );
    p.ExportFortran(fortranFilename, 
                    string("myTestFunc"), 
                    "File used by unit test\n PMMLBasicsTest1::testExportLinearRegressionFortran" );    
    CPPUNIT_ASSERT_EQUAL( areFilesEqual( refFortranFilename, fortranFilename), 0 );  
}

void PMMLBasicsTest1::testExportLinearRegressionPython()
{
    string refPyFilename = resourcesDir + "unittest_ref_lr_model.py";
    string pyFilename = tmpDir + "unittest_lr_model.py"; 
    string strModel = "Modeler[LinearRegression]Tds[steamplant]Predictor[x6:x8:x6x8:x6x6x8]Target[x1]";   
    PMMLlib::PMMLlib p(resourcesDir + "lr_model.pmml");
    p.SetCurrentModel( strModel, PMMLlib::kLR );
    p.ExportPython(pyFilename, 
                   string("myTestFunc"), 
                   "File used by unit test\n PMMLBasicsTest1::testExportLinearRegressionPython" );
    CPPUNIT_ASSERT_EQUAL( areFilesEqual( refPyFilename, pyFilename), 0 );  
}

void PMMLBasicsTest1::testCreatePmmlRegression()
{
#ifdef WIN32
    std::string refPmmlFilename = resourcesDir + "win32_lr_model.pmml";
#else
    std::string refPmmlFilename = resourcesDir + "lr_model.pmml";
#endif
    std::string pmmlFilename = tmpDir + "unittest_generated_lr_model.pmml";
    // Données
    string strModel = "Modeler[LinearRegression]Tds[steamplant]Predictor[x6:x8:x6x8:x6x6x8]Target[x1]";
    int nData = 3;
    vector<string>fieldName(nData);
    fieldName[0] = "x6";
    fieldName[1] = "x8";
    fieldName[2] = "x1";
    vector<string>displayName(nData);
    displayName[0] = " x_{6}";
    displayName[1] = " x_{8}";
    displayName[2] = " x_{1}";    
    vector<double>leftMargin(nData);
    leftMargin[0] = 1.1e+01;
    leftMargin[1] = 2.81e+01;
    leftMargin[2] = 6.36e+00;
    vector<double>rightMargin(nData);
    rightMargin[0] = 2.3e+01;
    rightMargin[1] = 7.67e+01;
    rightMargin[2] = 1.251e+01;
    vector<double>numPredCoeff(nData-1);
    numPredCoeff[0] = 0.4759134;
    numPredCoeff[1] = 0.1428838;    
    vector<double>predTermCoeff(nData-1);
    predTermCoeff[0] = -0.02201903;
    predTermCoeff[1] = 0.000536256;      
    vector< vector<string> > fieldRefVect(nData-1);
    vector<string>fieldRef0(2);
    fieldRef0[0] = fieldName[0];
    fieldRef0[1] = fieldName[1];
    vector<string>fieldRef1(3);
    fieldRef1[0] = fieldName[0];
    fieldRef1[1] = fieldName[0];
    fieldRef1[2] = fieldName[1];
    fieldRefVect[0] = fieldRef0;
    fieldRefVect[nData-2] = fieldRef1; 

    // Construction
    PMMLlib::PMMLlib p;
    // Header node--------------------------------------------------------
    p.SetHeader("myCopyright", "Tests unitaires", "PMMLlib", "myVersion", "Tests unitaires PMMLlib");  
    // DataDictionnary node--------------------------------------------------------
    for(int j=0; j<nData; j++)
    {
        p.AddDataField(fieldName[j], displayName[j], 
                       "continuous", "double", "ClosedClosed", 
                       leftMargin[j], rightMargin[j], true);
    } 
    // Set Model (and type)----------------------------------------------   
    p.AddRegressionModel( strModel, PMMLlib::kREGRESSION,  fieldName[2]);
    // Set Mining schema------------------------------------------------
    for(int j=0 ; j<nData-1 ; j++)
    {
        p.AddMiningSchema(fieldName[j], "active");
    }
    p.AddMiningSchema(fieldName[nData-1], "predicted");
    // Set Regression table
    double intercept = 3.837365;
    p.AddRegressionTable(intercept);
    // Set numeric predictor
    for(int j=0; j<nData-1; j++) 
    {       
        p.AddNumericPredictor(fieldName[j], 1, numPredCoeff[j]);
    }
    // Set Predictor term
    for(int j=0; j<nData-1; j++) 
    {
        p.AddPredictorTerm(predTermCoeff[j], fieldRefVect[j]);
    }      
    p.Write( pmmlFilename );
    CPPUNIT_ASSERT_EQUAL( areFilesEqual( refPmmlFilename, pmmlFilename), 0 );   
}

