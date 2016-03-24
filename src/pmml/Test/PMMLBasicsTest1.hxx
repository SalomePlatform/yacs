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

#ifndef __PMMLBASICSTEST1_HXX__
#define __PMMLBASICSTEST1_HXX__

#include "PMMLBasicsTest.hxx"

#include <string>

//classe test de PMMLlib
  
class PMMLBasicsTest1 : public PMMLBasicsTest
{
    CPPUNIT_TEST_SUITE(PMMLBasicsTest1);
    
    // test des méthodes communes
    CPPUNIT_TEST(testMakeLog);
    
    CPPUNIT_TEST(testConstructorFileDoesNotExist);     
     
    CPPUNIT_TEST(testSetCurrentModelWithNameAndTypekANNUnknownModel);
    CPPUNIT_TEST(testSetCurrentModelWithNameAndTypekLRUnknownModel);  
    CPPUNIT_TEST(testSetCurrentModelWithNameAndTypekUNDEFINEDUnknownModel);
    CPPUNIT_TEST(testSetCurrentModelWithNameAndType);
    CPPUNIT_TEST(testSetCurrentModelWithNameAndTypeTwoModelsWithSameNames);
    CPPUNIT_TEST(testSetCurrentModelWithNameWrongName);
    CPPUNIT_TEST(testSetCurrentModelWithNameTwoModelsWithSameNames);
    CPPUNIT_TEST(testSetCurrentModelWithName);   
    CPPUNIT_TEST(testSetCurrentModelNoModel);
    CPPUNIT_TEST(testSetCurrentModelMoreThanOneModel);
    CPPUNIT_TEST(testSetCurrentModel);      
    
    CPPUNIT_TEST(testGetModelsNbkANN);
    CPPUNIT_TEST(testGetModelsNbkLR);

    CPPUNIT_TEST( testWrite );  
    CPPUNIT_TEST( testWriteNotExistingFile );
    
    CPPUNIT_TEST(testUnlinkNode );
    CPPUNIT_TEST(testBackupNode );

    // test des méthodes NeuralNetwork
    CPPUNIT_TEST( testGetNbInputsForbiddenModelType ) ;
    CPPUNIT_TEST( testGetNbInputs8 ) ;

    CPPUNIT_TEST( testGetNbOutputsForbiddenModelType ) ;
    CPPUNIT_TEST( testGetNbOutputs2 ) ;

    CPPUNIT_TEST( testGetNameInputForbiddenModelType ) ;
    CPPUNIT_TEST( testGetNameInputIndexOutOfRange ) ;    
    CPPUNIT_TEST( testGetNameInput ) ;

    CPPUNIT_TEST( testGetNameOutputForbiddenModelType ) ;
    CPPUNIT_TEST( testGetNameOutputIndexOutOfRange ) ;    
    CPPUNIT_TEST( testGetNameOutput ) ;

    CPPUNIT_TEST( testGetNormalisationInputForbiddenType ) ;
    CPPUNIT_TEST( testGetNormalisationInputIndexUnknown ) ;
    CPPUNIT_TEST( testGetNormalisationInputkMinusOneOne ) ;
    CPPUNIT_TEST( testGetNormalisationInputkCRkZeroOne ) ;

    CPPUNIT_TEST( testGetNormalisationOutputForbiddenType ) ; 
    CPPUNIT_TEST( testGetNormalisationOutputIndexUnknown ) ;
    CPPUNIT_TEST( testGetNormalisationOutputkMinusOneOne ) ;
    CPPUNIT_TEST( testGetNormalisationOutputkCRkZeroOne ) ;

    CPPUNIT_TEST( testGetNbLayersForbiddenType ) ;
    CPPUNIT_TEST( testGetNbLayers ) ;

    CPPUNIT_TEST( testGetNbNeuronsAtLayerForbiddenType );
    CPPUNIT_TEST( testGetNbNeuronsAtLayerFromIndexOutOfRange );
    CPPUNIT_TEST( testGetNbNeuronsAtLayer );

    CPPUNIT_TEST( testGetNeuronBiasForbiddenType );
    CPPUNIT_TEST( testGetNeuronBiasFromLayerIndexOutOfRange );
    CPPUNIT_TEST( testGetNeuronBiasFromNeuronIndexOutOfRange );
    CPPUNIT_TEST( testGetNeuronBias );
       
    CPPUNIT_TEST( testGetPrecNeuronSynapseForbiddenType );
    CPPUNIT_TEST( testGetPrecNeuronSynapseFromLayerIndexOutOfRange );
    CPPUNIT_TEST( testGetPrecNeuronSynapseFromNeuronIndexOutOfRange );
    CPPUNIT_TEST( testGetPrecNeuronSynapseFromPrecIndexOutOfRange );
    CPPUNIT_TEST( testGetPrecNeuronSynapse );
    
    CPPUNIT_TEST( testReadNetworkStructure );    

    CPPUNIT_TEST( testExportNeuralNetworkCpp );
    CPPUNIT_TEST( testExportNeuralNetworkFortran );
    CPPUNIT_TEST( testExportNeuralNetworkPython );
    CPPUNIT_TEST( testCreatePmmlNeuralNetwork );

    // test des méthodes RegressionModel    
    CPPUNIT_TEST( testHasInterceptForbiddenType );
    CPPUNIT_TEST( testHasIntercept );
    CPPUNIT_TEST( testHasInterceptNo );
    CPPUNIT_TEST( testGetRegressionTableInterceptForbiddenType );    
    CPPUNIT_TEST( testGetRegressionTableIntercept );
    
    CPPUNIT_TEST( testReadRegressionStructure );
   
    CPPUNIT_TEST( testGetNumericPredictorNb );
    CPPUNIT_TEST( testGetNumericPredictorNbForbiddenType );

    CPPUNIT_TEST( testGetNumericPredictorName );
    CPPUNIT_TEST( testGetNumericPredictorNameForbiddenType );
    CPPUNIT_TEST( testGetNumericPredictorNamePredictorOutOfRange );

    CPPUNIT_TEST( testGetPredictorTermName );
    CPPUNIT_TEST( testGetPredictorTermNameForbiddenType ); 
    CPPUNIT_TEST( testGetPredictorTermNamePredictorOutOfRange );
    
    CPPUNIT_TEST( testGetNumericPredictorCoefficient );
    CPPUNIT_TEST( testGetNumericPredictorCoefficientForbiddenType );
    CPPUNIT_TEST( testGetNumericPredictorCoefficientPredictorOutOfRange );

    CPPUNIT_TEST( testGetPredictorTermCoefficient );
    CPPUNIT_TEST( testGetPredictorTermCoefficientForbiddenType );
    CPPUNIT_TEST( testGetPredictorTermCoefficientPredictorOutOfRange );

    CPPUNIT_TEST( testGetPredictorTermFieldRefNb );
    CPPUNIT_TEST( testGetPredictorTermFieldRefNbForbiddenType );    
    CPPUNIT_TEST( testGetPredictorTermFieldRefNbPredictorOutOfRange );

    CPPUNIT_TEST( testGetPredictorTermFieldRefName );
    CPPUNIT_TEST( testGetPredictorTermFieldRefNameForbiddenType );
    CPPUNIT_TEST( testGetPredictorTermFieldRefNamePredictorOutOfRange );
    CPPUNIT_TEST( testGetPredictorTermFieldRefNameFieldOutOfRange );
 
    CPPUNIT_TEST( testExportLinearRegressionCpp );
    CPPUNIT_TEST( testExportLinearRegressionFortran );
    CPPUNIT_TEST( testExportLinearRegressionPython );
    CPPUNIT_TEST( testCreatePmmlRegression );

    CPPUNIT_TEST_SUITE_END();
  
public:
    
    void setUp(); 
    void tearDown();
    
    void testMakeLog();
    
    void testConstructorFileDoesNotExist();
    
    void testSetCurrentModelWithNameAndTypekANNUnknownModel();
    void testSetCurrentModelWithNameAndTypekLRUnknownModel();  
    void testSetCurrentModelWithNameAndTypekUNDEFINEDUnknownModel();
    void testSetCurrentModelWithNameAndType();
    void testSetCurrentModelWithNameAndTypeTwoModelsWithSameNames();
    void testSetCurrentModelWithNameWrongName();
    void testSetCurrentModelWithNameTwoModelsWithSameNames();
    void testSetCurrentModelWithName();
    void testSetCurrentModelNoModel();
    void testSetCurrentModelMoreThanOneModel();
    void testSetCurrentModel(); 
    
    void testGetModelsNbkANN();
    void testGetModelsNbkLR();
    
    void testWrite(); 
    void testWriteNotExistingFile();
    
    void testUnlinkNode();
    void testBackupNode(); 
    
    void testGetNbInputsForbiddenModelType();
    void testGetNbInputs8();  
    
    void testGetNbOutputsForbiddenModelType();
    void testGetNbOutputs2();  
    
    void testGetNameInputForbiddenModelType();
    void testGetNameInputIndexOutOfRange();
    void testGetNameInput();
    
    void testGetNameOutputForbiddenModelType();
    void testGetNameOutputIndexOutOfRange();
    void testGetNameOutput();
    
    void testGetNormalisationInputForbiddenType();     
    void testGetNormalisationInputIndexUnknown();
    void testGetNormalisationInputkMinusOneOne();
    void testGetNormalisationInputkCRkZeroOne();

    void testGetNormalisationOutputForbiddenType();        
    void testGetNormalisationOutputIndexUnknown();
    void testGetNormalisationOutputkMinusOneOne();
    void testGetNormalisationOutputkCRkZeroOne();
 
    void testGetNbLayersForbiddenType();
    void testGetNbLayers() ;
  
    void testGetNbNeuronsAtLayerForbiddenType();
    void testGetNbNeuronsAtLayerFromIndexOutOfRange(); 
    void testGetNbNeuronsAtLayer(); 
     
    void testGetNeuronBiasForbiddenType();
    void testGetNeuronBiasFromLayerIndexOutOfRange(); 
    void testGetNeuronBiasFromNeuronIndexOutOfRange();
    void testGetNeuronBias();    
    
    void testGetPrecNeuronSynapseForbiddenType(); 
    void testGetPrecNeuronSynapseFromForbiddenTypeName();
    void testGetPrecNeuronSynapseFromLayerIndexOutOfRange();
    void testGetPrecNeuronSynapseFromNeuronIndexOutOfRange();
    void testGetPrecNeuronSynapseFromPrecIndexOutOfRange();
    void testGetPrecNeuronSynapse();
    
    void testReadNetworkStructure();
    
    void testExportNeuralNetworkCpp();
    void testExportNeuralNetworkFortran();
    void testExportNeuralNetworkPython();
    void testCreatePmmlNeuralNetwork();
    
    void testHasInterceptForbiddenType();
    void testHasIntercept();
    void testHasInterceptNo();
    void testGetRegressionTableInterceptForbiddenType(); 
    void testGetRegressionTableIntercept();

    void testReadRegressionStructure();
    
    void testGetNumericPredictorNb();
    void testGetNumericPredictorNbForbiddenType();

    void testGetPredictorTermNb();
    void testGetPredictorTermNbForbiddenType();
    
    void testGetNumericPredictorName();
    void testGetNumericPredictorNameForbiddenType();    
    void testGetNumericPredictorNamePredictorOutOfRange();  
    
    void testGetPredictorTermName();
    void testGetPredictorTermNameForbiddenType();    
    void testGetPredictorTermNamePredictorOutOfRange();      
    
    void testGetNumericPredictorCoefficient();
    void testGetNumericPredictorCoefficientForbiddenType();      
    void testGetNumericPredictorCoefficientPredictorOutOfRange();      

    void testGetPredictorTermCoefficient();
    void testGetPredictorTermCoefficientForbiddenType();  
    void testGetPredictorTermCoefficientPredictorOutOfRange();  
    
    void testGetPredictorTermFieldRefNb();
    void testGetPredictorTermFieldRefNbForbiddenType();   
    void testGetPredictorTermFieldRefNbPredictorOutOfRange();      

    void testGetPredictorTermFieldRefName();
    void testGetPredictorTermFieldRefNameForbiddenType();  
    void testGetPredictorTermFieldRefNamePredictorOutOfRange();     
    void testGetPredictorTermFieldRefNameFieldOutOfRange();    
    
    void testExportLinearRegressionCpp(); 
    void testExportLinearRegressionFortran();
    void testExportLinearRegressionPython();
    void testCreatePmmlRegression();

private :
    std::string resourcesDir;
    std::string tmpDir;
};


#endif
