//////////////////////////////////////////////////////////////
// Copyright (C) 2013-2016 CEA/DEN
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or any
// later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//////////////////////////////////////////////////////////////
/*!
  \file   PMMLlib.hxx
  \author InckA
  \date   Wed Nov 20 11:04:17 2013

  \brief  Header de la classe PMMLlib

 */

#ifndef __PMMLLIB_H__
#define __PMMLLIB_H__

#include "PMMLwin.hxx"
 
#include <libxml/xpathInternals.h>
#include <string>
#include <vector>
#include <sstream>

namespace PMMLlib
{

template <typename T>
std::string NumberToString ( T Number )
{
     std::ostringstream ss;
     ss << Number;
     return ss.str();
}

/**
 * Enumeration to type the PMML file.
 * UNDEFINED: not yet defined
 * ANN: Artificial Neural Network
 * LR:  Linear Regression
 *
 * @see http://www.dmg.org/v4-1/GeneralStructure.html#xsdGroup_MODEL-ELEMENT
 */
enum PMMLType{kUNDEFINED, kANN, kLR};


/**
 * @see http://www.dmg.org/v4-1/NeuralNetwork.html#xsdType_ACTIVATION-FUNCTION
 */
enum PMMLActivationFunction{kIDENTITY, kTANH, kLOGISTIC};


/**
 * @see http://www.dmg.org/v4-1/GeneralStructure.html#xsdType_MINING-FUNCTION
 */
enum PMMLMiningFunction{kREGRESSION};


/**
 * Class PMMLlib
 */
class PMMLlib
{
  
private:
    bool _log;                      //!< Log Printing
    std::string _pmmlFile;          //!< Name of the associated PMML file
    xmlDocPtr _doc;                 //!< Associated DOM documents
    xmlNodePtr _rootNode;           //!< Root node of the document
    xmlNodePtr _currentNode;        //!< Pointer to the current node    
    int _nbModels;                  //!< Number of models (all kinds)
    std::string _currentModelName;  //!< Name of the current model    
    PMMLType _currentModelType;     //!< Type of the current model 
    xmlNodePtr _currentModelNode;   //!< Pointer to the current model node    

    /** @defgroup general General methods
     *  Common methods to all kinds of PMML files and models
     *  @{
     */
public:    
    PMMLLIB_EXPORT PMMLlib(std::string file, 
                           bool log=false) ;     
    PMMLLIB_EXPORT PMMLlib(bool log=false); 
    PMMLLIB_EXPORT ~PMMLlib();
    PMMLLIB_EXPORT void SetCurrentModel(std::string modelName, 
                                        PMMLType type); 
    PMMLLIB_EXPORT void SetCurrentModel(std::string modelName); 
    PMMLLIB_EXPORT void SetCurrentModel();                                         
    PMMLLIB_EXPORT std::string makeLog() const; 
    PMMLLIB_EXPORT void printLog() const;   

    PMMLLIB_EXPORT void AddDataField(std::string name, 
                                     std::string displayName,
                                     std::string optype, 
                                     std::string dataType, 
                                     std::string closure, 
                                     double leftMargin, 
                                     double rightMargin,
                                     bool interval=false);
    PMMLLIB_EXPORT void AddMiningSchema(std::string name, 
                                        std::string usageType);
    PMMLLIB_EXPORT void SetHeader(std::string copyright, 
                                  std::string description, 
                                  std::string appName, 
                                  std::string appVersion, 
                                  std::string annotation);
    PMMLLIB_EXPORT void UnlinkNode();
    PMMLLIB_EXPORT void BackupNode();
    PMMLLIB_EXPORT int GetModelsNb();    
    PMMLLIB_EXPORT void Write();
    PMMLLIB_EXPORT void Write(std::string file);  
    PMMLLIB_EXPORT PMMLType GetCurrentModelType(); 
    PMMLLIB_EXPORT std::string GetCurrentModelName();    
private: 
    xmlNodePtr GetChildByName(xmlNodePtr node, 
                              std::string nodename);
    xmlNodePtr GetPtr(int ann_index, 
                      std::string name);
    xmlNodePtr GetPtr(std::string ann_name, 
                      std::string name);
    void CountModels();
    int CountNeuralNetModels();
    int CountRegressionModels();
    void SetRootNode();
    std::string GetModelName(xmlNodePtr node);
    std::string GetTypeString();
    
    /** @} */ // end of group general   


    /** @defgroup ann Methods dedicated to neural networks
     *  Methods dedicated to neural networks
     *  @{
     */
public:     
    PMMLLIB_EXPORT void AddNeuralNetwork(std::string modelName, 
                                         PMMLMiningFunction functionName);    
    PMMLLIB_EXPORT void AddNeuralInput(int id, 
                                       std::string inputName, 
                                       std::string optype, 
                                       std::string dataType, 
                                       double orig1, double norm1, 
                                       double orig2, double norm2);
    PMMLLIB_EXPORT void AddNeuralLayer(PMMLActivationFunction activationFunction);
    PMMLLIB_EXPORT void AddNeuron(int id, 
                                  double bias, 
                                  int conNb, 
                                  int firstFrom, 
                                  std::vector<double> weights);
    PMMLLIB_EXPORT void AddNeuralOutput(int outputNeuron, 
                                        std::string outputName, 
                                        std::string optype, 
                                        std::string dataType, 
                                        double orig1, double norm1, 
                                        double orig2, double norm2);
    PMMLLIB_EXPORT int GetNbInputs();
    PMMLLIB_EXPORT int GetNbOutputs();
    PMMLLIB_EXPORT std::string GetNameInput(int input_index);
    PMMLLIB_EXPORT std::string GetNameOutput(int output_index);
    PMMLLIB_EXPORT int GetNormalizationType();
    PMMLLIB_EXPORT void GetNormalisationInput(int input_index, 
                                              double *dnorm);
    PMMLLIB_EXPORT void GetNormalisationOutput(int output_index, 
                                               double *dnorm);
    PMMLLIB_EXPORT int GetNbHiddenLayers();
    PMMLLIB_EXPORT int GetNbLayers();
    PMMLLIB_EXPORT int GetNbNeuronsAtLayer(int layer_index);
    PMMLLIB_EXPORT double GetNeuronBias(int layer_index, 
                                        int neu_index);
    PMMLLIB_EXPORT double GetPrecNeuronSynapse(int layer_index, 
                                               int neu_index, 
                                               int prec_index);
    PMMLLIB_EXPORT void SetNeuralNetName(int ann_index, 
                                         std::string ann_name); 
    PMMLLIB_EXPORT std::string ReadNetworkStructure();    
private:   
    xmlNodePtr GetNeuralNetPtr(std::string ann_name);
    xmlNodePtr GetNeuralNetPtr(int ann_index);
    void CheckNeuralNetwork();
    /** @} */ // end of group ann


    /** @defgroup ln Methods dedicated to linear regression
     *  Methods dedicated to linear regression
     *  @{
     */
public:    
    PMMLLIB_EXPORT void AddRegressionModel(std::string modelName, 
                                           PMMLMiningFunction functionName, 
                                           std::string targetFieldName); 
    PMMLLIB_EXPORT void AddRegressionTable();
    PMMLLIB_EXPORT void AddRegressionTable(double intercept);
    PMMLLIB_EXPORT void AddNumericPredictor(std::string neuronName, 
                                            int exponent, 
                                            double coefficient);
    PMMLLIB_EXPORT void AddPredictorTerm(double coefficient, 
                                         std::vector<std::string> fieldRef);
    PMMLLIB_EXPORT bool HasIntercept();    
    PMMLLIB_EXPORT double GetRegressionTableIntercept();
    PMMLLIB_EXPORT int GetNumericPredictorNb();
    PMMLLIB_EXPORT int GetPredictorTermNb();    
    PMMLLIB_EXPORT std::string GetNumericPredictorName(int num_pred_index);
    PMMLLIB_EXPORT std::string GetPredictorTermName(int num_pred_index);
    PMMLLIB_EXPORT double GetNumericPredictorCoefficient(int num_pred_index);
    PMMLLIB_EXPORT double GetPredictorTermCoefficient(int pred_term_index);
    PMMLLIB_EXPORT int GetPredictorTermFieldRefNb(int pred_term_index);
    PMMLLIB_EXPORT std::string GetPredictorTermFieldRefName(int pred_term_index, 
                                                            int field_index);
    PMMLLIB_EXPORT std::string ReadRegressionStructure();                                                              
private:    
    xmlNodePtr GetRegressionPtr(int reg_index); 
    xmlNodePtr GetRegressionPtr(std::string reg_name);     
    void CheckRegression();
    
    /** @} */ // end of group ln
    

    /** @defgroup export Methods dedicated to file export
     *  Methods dedicated to file export
     *  @{
     */
private:
    void fillVectorsForExport(int nInput, int nOutput, int nHidden, int normType,
            std::vector<double> &minInput, std::vector<double> &maxInput,
            std::vector<double> &minOutput, std::vector<double> &maxOutput,
            std::vector<double> &valW );
public:
    PMMLLIB_EXPORT void ExportCpp(std::string file, 
                                  std::string functionName, 
                                  std::string header);
    PMMLLIB_EXPORT void ExportFortran(std::string file, 
                                      std::string functionName, 
                                      std::string header);
    PMMLLIB_EXPORT void ExportPython(std::string file, 
                                     std::string functionName, 
                                     std::string header);
    PMMLLIB_EXPORT std::string ExportPyStr(std::string functionName, 
                                           std::string header);
private:    
    void ExportNeuralNetworkCpp(std::string file, 
                                std::string functionName, 
                                std::string header);
    void ExportNeuralNetworkFortran(std::string file, 
                                    std::string functionName, 
                                    std::string header);
    void ExportNeuralNetworkPython(std::string file, 
                                   std::string functionName, 
                                   std::string header);
    std::string ExportNeuralNetworkPyStr(std::string functionName, 
                                         std::string header);
    
    void ExportLinearRegressionCpp(std::string, 
                                   std::string, 
                                   std::string);
    void ExportLinearRegressionFortran(std::string, 
                                       std::string, 
                                       std::string);
    void ExportLinearRegressionPython(std::string, 
                                      std::string, 
                                      std::string);
    std::string ExportLinearRegressionPyStr(std::string functionName, 
                                            std::string header);
    /** @} */ // end of group export

  private:
    /*!     * Conversion from a libxml2 string (xmlChar *) to a standard C++ string.
     *
     *    \param xs a constant libxml string.
     *    \return a C++ std::string (contains the same text as xs).
     */
     std::string _xmlCharToString(const xmlChar *xs) const;  
    /*!
    * Conversion from a standard C++ string to a libxml2 string (xmlChar *)
    *
    *    \param s a constant C++ std::string.
    *    \return a libxml string (contains the same text as s)
    *
    * The caller of this function must free the result when it's not needed
    * anymore (using xmlFree) 
    */
    xmlChar * _stringToXmlChar(const std::string &s) const;

    std::string _getProp(const xmlNodePtr node, 
                         std::string const & prop ) const;

};

}

#endif  //__PMMLLIB_H__
