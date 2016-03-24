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
  \file   PMMLlib.cxx
  \author Incka
  \date   Wed Nov 20 11:04:17 2013

  \brief  Implémentation de la classe PMMLlib

 */

// includes Salomé
#include "PMMLlib.hxx"

// includes C
#include <stdlib.h>

// includes C++
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

namespace PMMLlib
{
  
//**************************************************************
//                                                             *
//                                                             *
//                                                             *
//  méthodes communes à tous les types de modèles              *
//                                                             *
//                                                             *
//                                                             *
//**************************************************************
  
/**
 * Constructor to read a PMML file.
 * @param file Name of the PMML file to read
 * @param log Flag to print logs or not
 */
PMMLlib::PMMLlib(std::string file,bool log) : 
            _log(log),
            _pmmlFile(file),
            _doc(NULL),
            _rootNode(NULL),
            _currentNode(NULL),
            _nbModels(0),
            _currentModelName(""),
            _currentModelType(kUNDEFINED)   
{
    try
    {
        xmlKeepBlanksDefault(0);
        xmlInitParser();
        _doc = xmlParseFile(_pmmlFile.c_str());         
        if ( _doc != NULL )
        {
            _rootNode = xmlDocGetRootElement(_doc);
            CountModels(); 
        }
        else 
            throw string("Unable to read PMML file.");
    }
    catch ( std::string msg )
    {
        std::cerr << msg;
        xmlFreeDoc(_doc);
        xmlCleanupParser();  
        throw;
    }
}

/**
 * Constructor to create a PMML file.
 * @brief This constructor is mandatory for Swig because it can be used with no parameters.
 * @param log  Flag to print logs or not
 */
PMMLlib::PMMLlib(bool log):
            _log(log),
            _pmmlFile(""),
            _doc(NULL),
            _rootNode(NULL),
            _currentNode(NULL),
            _nbModels(0),
            _currentModelName(""),
            _currentModelType(kUNDEFINED)           
{
    SetRootNode();
}

/**
 * Destructor of the class.
 */
PMMLlib::~PMMLlib()
{
    if (_doc)
        xmlFreeDoc(_doc);
    xmlCleanupParser();
    if ( _log )
        cout << "~PMMLlib" << endl;
}

/**
 * Set the current model and its type.
 * @param modelName Name of the model to load (ie content of 'modelName' attribute)
 * @param type Type of PMML to read: one of kANN or kLR
 */
void PMMLlib::SetCurrentModel(std::string modelName, 
                              PMMLType type)
{
    _currentModelName = modelName;
    _currentModelType = type;
    switch(type)
    {
        case kANN:
            _currentModelNode = GetNeuralNetPtr(modelName);
            break;
        case kLR:
            _currentModelNode = GetRegressionPtr(modelName);
            break;
        default:
            throw string("Unknown PMML type.");
            break;
    } 
    if ( _currentModelNode == NULL )
        throw string("Model not found.");
}

/**
 * Set the current model and its type.
 * @brief Throw an exception if there is no model or more than one model with name "modelName" in the PMML file
 * @param modelName Name of the model to load (ie content of 'modelName' attribute)
 */
void PMMLlib::SetCurrentModel(std::string modelName)
{
    if (_rootNode == NULL)
        throw string("No PMML file set.");
    xmlNodePtr node = NULL;
    int nC = 0;
    node = _rootNode->children;
    while (node)
    {         
        string nodeModelName = _getProp(node, string("modelName"));
        if ( nodeModelName == modelName )
        {
            nC++;
            _currentModelNode = node;
            _currentModelName = modelName;
            _currentModelType = GetCurrentModelType();
        }
        node = node->next;
    }
    if ( nC != 1 ) 
    {
        std::ostringstream oss;
        oss << nC;
        string msg = "SetCurrentModel(modelName) : found " + oss.str() + " model(s) in PMML file.\n";
        msg += "Use SetCurrentModel(modelName,type).";
        throw msg;
    }   
}

/**
 * Set the current model and its type.
 * @brief Throw an exception if no model is found or if there are more than one model in the PMLL file
 */
void PMMLlib::SetCurrentModel()
{
    int nC = _nbModels;
    if ( nC != 1 ) 
    {
        std::ostringstream oss;
        oss << nC;
        string msg = "SetCurrentModel() : found " + oss.str() + " model(s) in PMML file.\n";
        msg += "Use SetCurrentModel(modelName) or SetCurrentModel(modelName,type).";
        throw msg;
    }   
    _currentModelNode = GetChildByName(_rootNode,"NeuralNetwork"); 
    _currentModelType = kANN;  
    if (_currentModelNode == NULL)
    {
        _currentModelNode = GetChildByName(_rootNode,"RegressionModel"); 
        _currentModelType = kLR;
    }
    if (_currentModelNode == NULL)
    {
        string msg("Couldn't get node in SetCurrentModel().");
        throw msg;      
    }
    _currentModelName = _getProp(_currentModelNode, string("modelName"));
}

/**
 * Make the string used by PMMLlib::printLog.
 * @return The log
 */
std::string PMMLlib::makeLog() const
{
    ostringstream out;
    out << "**\n**** Display of PMMLlib ****" << endl;
    out << " **  _pmmlFile[" << _pmmlFile << "]" << endl;
    out << " **  _log[" << (_log?1:0) << "]" << endl;
    out << "**\n**** End of display of PMMLlib ****" << endl;
    return out.str(); 
}

/**
 * Print some information about the current PMML object.
 */
void PMMLlib::printLog() const
{
    string log = makeLog();
    cout << log << endl;
}

/**
 * Set the root node in the tree:
 * @code
 * <PMML version="4.1" xmlns="http://www.dmg.org/PMML-4_1">
 * @endcode
 */
void PMMLlib::SetRootNode()
{ 
    xmlChar * xs = _stringToXmlChar("1.0");
    _doc = xmlNewDoc(xs);
    xmlFree(xs);
    
    xmlChar *xp = _stringToXmlChar("PMML");
    _rootNode = xmlNewNode(0, xp);
    xmlFree(xp);
    
    xmlNewProp(_rootNode, (const xmlChar*)"xmlns", (const xmlChar*)"http://www.dmg.org/PMML-4_1");
    xmlNewProp(_rootNode, (const xmlChar*)"version", (const xmlChar*)"4.1");
    
    xmlDocSetRootElement(_doc, _rootNode);
}


/**
 * Set the header node in the tree.
 * @param copyright Copyright of the PMML file
 * @param description Description of the model
 * @param appName Name of the application that produced the file
 * @param appVersion Version of the application that produced the file
 * @param annotation Some annotation
 */
void PMMLlib::SetHeader(std::string copyright, 
                        std::string description, 
                        std::string appName, 
                        std::string appVersion, 
                        std::string annotation)
{
    xmlNodePtr headerNode = xmlNewChild(_rootNode, 0, (const xmlChar*)"Header", 0);
    xmlNewProp(headerNode, (const xmlChar*)"copyright", (const xmlChar*)(copyright.c_str()));
    xmlNewProp(headerNode, (const xmlChar*)"description", (const xmlChar*)(description.c_str()));

    xmlNodePtr appNode = xmlNewChild(headerNode, 0, (const xmlChar*)"Application", 0);
    xmlNewProp(appNode, (const xmlChar*)"name", (const xmlChar*)(appName.c_str()));
    xmlNewProp(appNode, (const xmlChar*)"version", (const xmlChar*)(appVersion.c_str()));

    xmlNewChild(headerNode, 0, (const xmlChar*)"Annotation", (const xmlChar*)(annotation.c_str()));
}

/**
 * Add the MiningSchema node. 
 * @brief Common to all models.
 * @param name Value of property "name".
 * @param usageType Value of property "usageType".
 */
void PMMLlib::AddMiningSchema(std::string name, 
                              std::string usageType)
{
    xmlNodePtr netNode = _currentModelNode; 

    // if 'MiningSchema' node does not exist, create it
    xmlNodePtr miningSchemaNode = GetChildByName(netNode, "MiningSchema");
    if(!miningSchemaNode)
    {
        miningSchemaNode = xmlNewChild(netNode, 0, (const xmlChar*)"MiningSchema", 0);
    }

    // then append the node
    xmlNodePtr miningFieldNode = xmlNewChild(miningSchemaNode, 0, (const xmlChar*)"MiningField", 0);
    xmlNewProp(miningFieldNode, (const xmlChar*)"name", (const xmlChar*)(name.c_str()) );
    xmlNewProp(miningFieldNode, (const xmlChar*)"usageType", (const xmlChar*)(usageType.c_str()) );
}

/**
 * Get the child of a node from the name of this node
 * @param node Start node for the research
 * @param nodeName Name of the node to find
 * @return Pointer to the node found
 */
xmlNodePtr PMMLlib::GetChildByName(xmlNodePtr node, 
                                   std::string nodeName)
{
    if ( node == NULL )
        return node;
    
    xmlNodePtr childNode = node->children;
    if ( childNode == NULL )   
        return childNode;
    
    const xmlChar* name = childNode->name;
    string strName("");  
    if ( name != NULL )
        strName =  _xmlCharToString(name);        
    
    while( (childNode != NULL) && (strName != nodeName) )
    {
      childNode = childNode->next;
      if ( childNode == NULL )
          return childNode;
      name = childNode->name; 
      if ( name != NULL )
          strName =  _xmlCharToString(name);        
    }
    return childNode;
}

/**
 * Count the tags of all types of models (NeuralNetwork and RegressionModel).
 * @return Number of models
 */
void PMMLlib::CountModels()
{
    int nCount = 0;
    nCount = CountNeuralNetModels() + CountRegressionModels();
    if ( _log)
        cout << " ** End Of Count Models nCount[" << nCount << "]" << endl;
    _nbModels = nCount ; 
}

/**
 * Count NeuralNetwork models tags in the PMML file.
 * @return Number of models
 */
int PMMLlib::CountNeuralNetModels()
{
    int nCount = 0;
    xmlNodePtr ptr = GetChildByName(_rootNode,"NeuralNetwork");  
    // Count the models
    while (ptr != NULL && _xmlCharToString(ptr->name) == "NeuralNetwork")
    {
        nCount++;
        if (_log)
            cout << " ** nCount[" << nCount << "]" << endl;
        ptr = ptr->next;
    }   
    if ( _log)
        cout << " ** End Of CountNetworks nCount[" << nCount << "]" << endl;    
    return nCount;
}

/**
 * Count RegressionModel models tags in the PMML file.
 * @return Number of models
 */
int PMMLlib::CountRegressionModels()
{
    int nCount = 0;
    xmlNodePtr ptr = GetChildByName(_rootNode,"RegressionModel");  
    // Count the models
    while (ptr != NULL && _xmlCharToString(ptr->name) == "RegressionModel")
    {
        nCount++;
        if (_log)
            cout << " ** nCount[" << nCount << "]" << endl;
        ptr = ptr->next;
    }   
    if ( _log)
        cout << " ** End Of CountRegressions nCount[" << nCount << "]" << endl;    
    return nCount;
}

/**
 * Get the number of models
 * @return Number of models
 */
int PMMLlib::GetModelsNb()
{
    return _nbModels;
}

/**
 * Get the name of the XML node of a given model
 * @param node Model node
 * @return value of attribute "modelName" of the model node
 */
std::string PMMLlib::GetModelName(xmlNodePtr node)
{
    string name("");
    name = _getProp(node, string("modelName") );
    return name;
}

/**
 * Get a pointer to the index-th node named name
 * @param index Index of the node to search
 * @param name Name of the node
 * @return Pointer to the node found
 */
xmlNodePtr PMMLlib::GetPtr(int index, 
                           std::string name)
{
    xmlNodePtr node = NULL;

    if (_doc != NULL)
    {
        _rootNode = xmlDocGetRootElement(_doc);
        node = GetChildByName(_rootNode, name);

        int i=0;
        while ((i != index) && (node != NULL))
        {
            node = node->next;
            i++;
        }
    }
    return node;
}

/**
 * Get a pointer to the node named name whose 'modelName' attribute is ann_name
 * @param modelName Model name of the node to search
 * @param nodeName Name of the node
 * @return Pointer to the node found
 */
xmlNodePtr PMMLlib::GetPtr(std::string myModelName, 
                           std::string nodeName)
{
    xmlNodePtr node = NULL;
    if (_doc != NULL)
    {
        node = GetChildByName(_rootNode, nodeName);
        if( node )
        {
            string modelName = _getProp(node, string("modelName"));

            while ( (node != NULL) && modelName != myModelName )
            {
                node = node->next;
                if( node )
                {
                    modelName = _getProp(node, string("modelName"));
                }
            }
        }
    }
    return node;
}

/**
 * Get the tag of the current model.
 * @return Current model tag
 */
std::string PMMLlib::GetTypeString()
{
    string name = "";
    switch(_currentModelType)
    {
    case kANN:
        name = "NeuralNetwork";
        break;
    case kLR:
        name = "RegressionModel";
        break;
    default:
        throw string("Unknown PMML type.");
        break;
    }
    return name;
}

/**
 * Get the current model type.
 * @brief type is kUNDEFINED if no model is set or if model type is not handled
 * @return the type
 */
PMMLType PMMLlib::GetCurrentModelType()
{
    PMMLType type = kUNDEFINED ; 
    if ( ! _currentModelNode )
        return type;
    string name = _xmlCharToString(_currentModelNode->name);
    if ( name == "NeuralNetwork" )
        type = kANN;
    else if ( name == "RegressionModel" )
        type = kLR;
    return type;
}

/**
 * Get the current model name.
 * @brief name is "" if no model is set 
 * @return the type
 */
std::string PMMLlib::GetCurrentModelName()
{
    if ( ! _currentModelNode )
        return string("");
    string name = _getProp(_currentModelNode, string("modelName"));
    return name;  
}

/**
 * Unlink the current model node.
 */
void PMMLlib::UnlinkNode()
{
    xmlNodePtr ptr = _currentModelNode ;
    xmlUnlinkNode( ptr );
    xmlFreeNode( ptr );
}

/**
 * Make a backup of the current model node.
 */
void PMMLlib::BackupNode()
{
    // Node name depending of PMML type
    string name = GetTypeString();
    // Find the last save index number
    int nCrtIndex = 0;
    stringstream ss;
    ss << _currentModelName << "_" << nCrtIndex;
    xmlNodePtr ptr = GetPtr(ss.str(), name);
    while( ptr )
    {
        nCrtIndex++;
        if (_log)
            cout << " ** nCrtIndex[" << nCrtIndex << "]" << endl;

        ss.str("");
        ss << _currentModelName << "_" << nCrtIndex;
        ptr = GetPtr(ss.str(), name);
    }
    if(_log)
        cout << " *** Node \"" << _currentModelName << "\" found, then backup it with index [" << nCrtIndex << "]" << endl;
    // Rename model
    xmlUnsetProp(_currentModelNode, (const xmlChar*)"modelName");
    xmlNewProp(_currentModelNode, (const xmlChar*)"modelName", (const xmlChar*)(ss.str().c_str()));
}

/**
 * Save the XML tree in the PMML file
 */
void PMMLlib::Write()
{
    // Enregistrement de l'arbre DOM dans le fichier pmml
    Write(_pmmlFile);
    // Mise à jour du nombre de modèles
    CountModels();
}

/**
 * Save the XML tree in a given file
 * @param Name of the file  
 */
void PMMLlib::Write(std::string file)
{
    // Enregistrement de l'arbre DOM sous forme de fichier pmml
    int ret = xmlSaveFormatFile( file.c_str(), _doc, 1);
    if ( ret == -1 )
    {
        std::string msg("  *** Error :: unable to write the PMML file \"" + file + "\"") ; 
        cout << msg << endl;
        throw msg;
    } 
    if ( _log )
        cout << "  *** Write the PMML file \"" << file <<"\"" << endl;
}

/**
 * Export the current model as a function in a Cpp file.
 * @param file Name of the file  
 * @param functionName Name of the function  
 * @param header Header of the function  
 */
void PMMLlib::ExportCpp(std::string file, 
                        std::string functionName, 
                        std::string header)
{
    if ( _currentModelType == kANN )
        ExportNeuralNetworkCpp(file,functionName, header);
    else if ( _currentModelType == kLR )
    {
        ExportLinearRegressionCpp(file, functionName, header);
    }
    else 
        throw string("ExportCpp : PMML type not handled.");
}

/**
 * Export the current model as a function in a Fortran file.
 * @param file Name of the file  
 * @param functionName Name of the function  
 * @param header Header of the function  
 */
void PMMLlib::ExportFortran(std::string file, 
                            std::string functionName, 
                            std::string header)
{
    if ( _currentModelType == kANN )
        ExportNeuralNetworkFortran(file,functionName, header);
    else if ( _currentModelType == kLR )
        ExportLinearRegressionFortran(file,functionName, header);
    else 
        throw string("ExportFortran : PMML type not handled.");  
}

/**
 * Export the current model as a function in a Python file.
 * @param file Name of the file  
 * @param functionName Name of the function  
 * @param header Header of the function  
 */
void PMMLlib::ExportPython(std::string file, 
                           std::string functionName, 
                           std::string header)
{
    if ( _currentModelType == kANN )
        ExportNeuralNetworkPython(file,functionName, header);
    else if ( _currentModelType == kLR )
        ExportLinearRegressionPython(file,functionName, header);
    else 
        throw string("ExportPython : PMML type not handled.");    
}

/**
 * Export the current model as a function in a Python string.
 * @param file Name of the file  
 * @param functionName Name of the function  
 * @param header Header of the function  
 * @return Function as a string
 */
std::string PMMLlib::ExportPyStr(std::string functionName, 
                                 std::string header)
{
    if ( _currentModelType == kANN )
        return ExportNeuralNetworkPyStr(functionName, header);
    else if ( _currentModelType == kLR )
        return ExportLinearRegressionPyStr(functionName, header);
    else 
        throw string("ExportPyStr : PMML type not handled.");    
}

/*!
 * Conversion from a libxml2 string (xmlChar *) to a standard C++ string.
 *    \param xs a constant libxml string.
 *    \return a C++ std::string (contains the same text as xs).
 */
std::string PMMLlib::_xmlCharToString(const xmlChar *xs) const
{
    size_t i, L = xmlStrlen(xs);
    std::string s;
    s.resize(L);
    for (i=0; *xs; s[i++] = *xs++);
    return s;
}

/*!
 * Conversion from a a standard C++ string to a libxml2 string (xmlChar *).
 *    \param s Constant C++ std::string (contains the same text as xs)
 *    \return Constant libxml string.
 */
xmlChar * PMMLlib::_stringToXmlChar(const std::string &s) const
{
    return xmlCharStrdup(s.c_str());
}

/*!
 * Get the value of a node property.
 *    \param node Tag
 *    \param prop Property
 *    \return Constant libxml string.
 */
std::string PMMLlib::_getProp(const xmlNodePtr node, 
                              std::string const & prop ) const
{
    std::string name("");
    if (_doc != NULL)
    {
        xmlChar *xp = _stringToXmlChar(prop);
        xmlChar * attr ;
        attr = xmlGetProp(node, xp );
        if ( attr ) 
        {
            name = _xmlCharToString(attr );
            xmlFree(attr);   
        }  
        xmlFree(xp);
    }
    return name;
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

/*!
 * Check if the current model type is kANN.
 *    \brief Called in all methods specific to the NeuralNetwork model.
 *    \brief Throw an exception if the model type is not kANN.
 */
void PMMLlib::CheckNeuralNetwork()
{
    if ( _currentModelType != kANN )
        throw string("Use this method with NeuralNetwork models.");
}

/**
 * Get the XML node of a given network from the index
 * @param index Index of the neural network
 * @return Pointer to the XML node
 */
xmlNodePtr PMMLlib::GetNeuralNetPtr(int index)
{  
    return GetPtr(index, GetTypeString() );
}

/**
 * Get the XML node of a given network model
 * @param name Name of the neural network
 * @return Pointer to the XML node
 */
xmlNodePtr PMMLlib::GetNeuralNetPtr(std::string name)
{   
    return GetPtr(name, GetTypeString() );
}

/**
 * Read the structure of the network 
 * @brief Specific to NeuralNetwork 
 * @return Structure read
 */
std::string PMMLlib::ReadNetworkStructure()
{   
    CheckNeuralNetwork(); 
    
    string structure("");
    // Treatment of the input
    xmlNodePtr inputNodes = GetChildByName(_currentModelNode,"NeuralInputs");
    if ( inputNodes != NULL )
    {
        xmlNodePtr inputNode = GetChildByName(inputNodes,"NeuralInput");
        if ( inputNode != NULL ) 
        {
            while (inputNode != NULL)
            {
                xmlNodePtr child = GetChildByName(inputNode,"DerivedField");
                if ( child != NULL )
                {
                    xmlNodePtr fieldName = child->children; // NormContinuous
                    if ( fieldName != NULL )
                    {
                        string field = _getProp(fieldName, string("field"));
                        structure += field;
                        structure += ":";
                    }
                } 
                inputNode = inputNode->next;
            }
            // Delete the last comma
            structure.erase(structure.size()-1);
        }
    }
    // Intermediary layers
    xmlNodePtr node_layer = GetChildByName(_currentModelNode,"NeuralLayer");
    if ( node_layer != NULL )
    {
        string name = string((const char*)(node_layer->name));
        structure += ",";

        while ( node_layer != NULL &&
                (string((const char*)(node_layer->name)) == "NeuralLayer") &&
                node_layer->next != NULL &&
                (string((const char*)(node_layer->next->name)) != "NeuralOutputs") )
        {
            // Get the number of neurons of the current layer
            string nbneurons = _getProp(node_layer, string("numberOfNeurons"));
            structure += nbneurons;
            structure += ",";
            node_layer = node_layer->next;
        }
    }
    // Output layers
    xmlNodePtr node_outputs = GetChildByName(_currentModelNode,"NeuralOutputs");
    if ( node_outputs != NULL )
    {    
        xmlNodePtr node_output = GetChildByName(node_outputs,"NeuralOutput");
        if ( node_output != NULL )
        {   
            while (node_output != NULL)
            {
                // Get the input of the current layer
                xmlNodePtr child = GetChildByName(node_output,"DerivedField");
                if ( child != NULL )
                {                
                    xmlNodePtr fieldName = child->children; // NormContinuous
                    if ( fieldName != NULL )
                    {
                        if (string((const char*)(fieldName->name)) == "NormContinuous")
                            structure += "@";

                        string field = _getProp(fieldName, string("field"));
                        structure += field;
                        structure += ":"; 
                    }    
                }
                node_output = node_output->next;
            }
            // Delete the last comma
            structure.erase(structure.size()-1);
        }
    }
    return structure;
}

/**
 * Get the number of inputs, ie the number of NeuralInputs nodes.
 * @brief Specific to NeuralNetwork 
 * @return Number of input nodes
 */
int PMMLlib::GetNbInputs()
{
    CheckNeuralNetwork();  
    
    int nb=0;
    xmlNodePtr node_inputs = GetChildByName(_currentModelNode,"NeuralInputs");
    if ( node_inputs == NULL )
      return nb;
    
    node_inputs = node_inputs->children;
    while (node_inputs != NULL)
    {
        nb++;
        node_inputs = node_inputs->next;
    }

    return nb;
}

/**
 * Recover the number of outputs
 * @brief Specific to NeuralNetwork 
 * @return Number of outputs
 */
int PMMLlib::GetNbOutputs()
{
    CheckNeuralNetwork();   
    
    int nb=0;
    xmlNodePtr node_outputs = GetChildByName(_currentModelNode,"NeuralOutputs");
    if ( node_outputs == NULL )
      return nb;    
    
    node_outputs = node_outputs->children;

    while (node_outputs != NULL)
    {
        nb++;
        node_outputs = node_outputs->next;
    }

    return nb;
}

/**
 * Recovery of the name of an input in the current model.
 * @brief Specific to NeuralNetwork 
 * @param index Index of the input
 * @return Name of the input
 */
std::string PMMLlib::GetNameInput(int index)
{
    CheckNeuralNetwork();  
    
    string name("");
    xmlNodePtr node_inputs = GetChildByName(_currentModelNode,"NeuralInputs");
    if ( node_inputs == NULL )
        return name;   
    
    node_inputs = node_inputs->children;
    if ( node_inputs == NULL )
        return name; 
    
    for(int i = 0;i<index;i++)
    {
        node_inputs = node_inputs->next;
        if ( node_inputs == NULL )
            return name;         
    }

    node_inputs = node_inputs->children;
    if ( node_inputs == NULL )
        return name; 
    
    node_inputs = node_inputs->children;
    if ( node_inputs == NULL )
        return name;     

    name = _getProp(node_inputs, string("field"));
    
    return name;
}

/**
 * Get the name of an output in the current model.
 * @brief Specific to NeuralNetwork 
 * @param index Index of the output
 * @return Name of the output
 */
std::string PMMLlib::GetNameOutput(int index)
{
    CheckNeuralNetwork();  
    
    string name("");
    xmlNodePtr node_outputs = GetChildByName(_currentModelNode,"NeuralOutputs");
    if ( node_outputs == NULL )
      return name;       
    node_outputs = node_outputs->children;
    if ( node_outputs == NULL )
      return name;   
    for(int i = 0;i<index;i++)
    {
        node_outputs = node_outputs->next;
        if ( node_outputs == NULL )
          return name;          
    }

    node_outputs = node_outputs->children;
    if ( node_outputs == NULL )
      return name;  
    node_outputs = node_outputs->children;
    if ( node_outputs == NULL )
      return name;      

    name = _getProp(node_outputs, string("field") );    

    return name;
}

/**
 * Get the normalization type of the current model
 * @brief Specific to NeuralNetwork
 * @return Normalization type of the neural network
 */
int PMMLlib::GetNormalizationType()
{
    CheckNeuralNetwork();  
    
    xmlNodePtr node_inputs = GetChildByName(_currentModelNode,"NeuralInputs");
    node_inputs = GetChildByName(node_inputs,"NeuralInput");
    xmlNodePtr nodeTmp = GetChildByName(node_inputs,"DerivedField");
    xmlNodePtr node_field = nodeTmp->children;
    xmlNodePtr node_linearnorm;
    string str_tmp;
    double dorig1, dnorm1;
    double dorig2, dnorm2;
    if (string((const char*)(node_field->name)) == "NormContinuous")
    {
        // Get mean and standard deviation
        node_linearnorm = node_field->children;
        str_tmp = _getProp(node_linearnorm, string("orig"));
        dorig1 = atof(str_tmp.c_str());
        str_tmp = _getProp(node_linearnorm, string("norm"));
        dnorm1 = atof(str_tmp.c_str());
        node_linearnorm = node_linearnorm->next;
        str_tmp = _getProp(node_linearnorm, string("orig"));
        dorig2 = atof(str_tmp.c_str());
        str_tmp = _getProp(node_linearnorm, string("norm"));
        dnorm2 = atof(str_tmp.c_str());
        if ( dnorm1 * dnorm2  < -0.5 )
        {   // case of kMinusOneOne
            return 0;
        }
        else
        {   // case of kCR, kZeroOne
            return 1;
        }
    }
    string msg("Unable to retrieve the normalization type.");
    throw msg;
}

/**
 * Get the input parameters on the normalization
 * @brief Specific to NeuralNetwork 
 * @param node_ann Neural network node
 * @param index Index of the input
 * @param[out] dnorm Array that contains the mean and the standard deviation
 */
void PMMLlib::GetNormalisationInput(int index, 
                                    double *dnorm)
{
    CheckNeuralNetwork();     
    dnorm[0] = 0.0;
    dnorm[1] = 0.0;
    xmlNodePtr node_inputs = GetChildByName(_currentModelNode,"NeuralInputs");
    if ( node_inputs == NULL )
        return ;       
    node_inputs = GetChildByName(node_inputs,"NeuralInput");
    if ( node_inputs == NULL )
        return ;    
    // Positionnement sur la bonne entree
    for(int i=0;i<index;i++)
    {
        node_inputs = node_inputs->next;
        if ( node_inputs == NULL )
            return ;  
    }
    xmlNodePtr tmpNode = GetChildByName(node_inputs,"DerivedField");
    if ( tmpNode == NULL )
        return ; 
    xmlNodePtr node_field = GetChildByName(tmpNode,"NormContinuous");
    if ( node_field == NULL )
        return ; 
    if (string((const char*)(node_field->name)) == "NormContinuous")
    {
        //Get mean and standard deviation
        string str_tmp;        
        xmlNodePtr node_linearnorm = node_field->children;
        str_tmp = _getProp(node_linearnorm, string("orig"));
        double dorig1 = atof(str_tmp.c_str());
        str_tmp = _getProp(node_linearnorm, string("norm"));
        double dnorm1 = atof(str_tmp.c_str());
        node_linearnorm = node_linearnorm->next;
        str_tmp = _getProp(node_linearnorm, string("orig"));
        double dorig2 = atof(str_tmp.c_str());
        str_tmp = _getProp(node_linearnorm, string("norm"));
        double dnorm2 = atof(str_tmp.c_str());
        if ( dnorm1 * dnorm2  < -0.5 ) // <=> GetNormalizationType == 0
        {
            // case of kMinusOneOne
            dnorm[0] = dorig1;
            dnorm[1] = dorig2;
        } 
        else // <=> GetNormalizationType == 1
        {
            // case of kCR, kZeroOne
            dnorm[0] = dorig2;
            dnorm[1] = -1.0 * dnorm1 * dorig2; //dorig2 / dnorm1; 
        }
    }
}

/**
 * Get the parameters on the normalization of an output for the current model.
 * @brief Specific to NeuralNetwork 
 * @param index Output index
 * @param[out] dnorm Array that contains the mean and the standard deviation
 */
void PMMLlib::GetNormalisationOutput(int index, 
                                     double *dnorm)
{
    CheckNeuralNetwork();     
    dnorm[0] = 0.0;
    dnorm[1] = 0.0;
    
    xmlNodePtr node_outputs = GetChildByName(_currentModelNode,"NeuralOutputs");
    if ( node_outputs == NULL )
        return ;      
    node_outputs = GetChildByName(node_outputs,"NeuralOutput");
    if ( node_outputs == NULL )
        return ;   
    // Positionnement sur la bonne sortie
    for(int i=0;i< index;i++)
    {
        node_outputs = node_outputs->next;
        if ( node_outputs == NULL )
            return ;        
    }
    xmlNodePtr tmpNode = GetChildByName(node_outputs,"DerivedField");
    if ( tmpNode == NULL )
        return ;   
    xmlNodePtr node_field = GetChildByName(tmpNode,"NormContinuous"); 
    if ( node_field == NULL )
        return ;    

    if (string((const char*)(node_field->name)) == "NormContinuous")
    {
        // Recuperation de la moyenne et de l'ecart type
        string str_tmp;
        xmlNodePtr node_linearnorm = node_field->children;
        str_tmp = _getProp(node_linearnorm, string("orig"));
        double dorig1 = atof(str_tmp.c_str());
        str_tmp = _getProp(node_linearnorm, string("norm"));
        double dnorm1 = atof(str_tmp.c_str());
        node_linearnorm = node_linearnorm->next;
        str_tmp = _getProp(node_linearnorm,string("orig"));
        double dorig2 = atof(str_tmp.c_str());
        str_tmp = _getProp(node_linearnorm, string("norm"));
        double dnorm2 = atof(str_tmp.c_str());
        if ( dnorm1 * dnorm2  < -0.5 ) 
        {
            // case of kMinusOneOne
            dnorm[0] = dorig1;
            dnorm[1] = dorig2;
        } 
        else 
        {
            // case of kCR, kZeroOne
            dnorm[0] = dorig2;
            dnorm[1] = -1.0 * dorig2 * dnorm1; //-1.0 * dorig2 / dnorm1;
        }
    }
}

/**
 * Get the number of hidden layers
 * @brief Specific to NeuralNetwork 
 * @return Number of hidden layers
 */
int PMMLlib::GetNbHiddenLayers()
{
    CheckNeuralNetwork();
    
    int nb_layers = 0;  
    xmlNodePtr node_layers = GetChildByName(_currentModelNode,"NeuralLayer");
    if ( node_layers == NULL )
      return nb_layers;
    
    while (string((const char*)(node_layers->name)) == "NeuralLayer")
    {
        nb_layers++;
        node_layers = node_layers->next;
        if ( node_layers == NULL )
          return nb_layers;    
    }
    return nb_layers;
}

/**
 * Get the total number of layers
 * @return Total number of layers
 */
int PMMLlib::GetNbLayers()
{
    return (GetNbHiddenLayers() + 2);
}

/**
 * Get the number of neurons at a given layer
 * @param index Index of the layer
 * @return Number of neurons at given layer
 */
int PMMLlib::GetNbNeuronsAtLayer(int index)
{
    CheckNeuralNetwork();  
    
    int nb_neurons = 0;
    xmlNodePtr node_layers = GetChildByName(_currentModelNode,"NeuralLayer");
    if ( node_layers == NULL )
        return nb_neurons;  

    // Positionnement à la bonne couche
    for(int i=0;i<index;i++)
    {
        node_layers = node_layers->next;
        if ( node_layers == NULL )
            return nb_neurons;         
    }

    xmlNodePtr node_neurons = GetChildByName(node_layers,"Neuron");
    while(node_neurons != NULL)
    {
        nb_neurons++;
        node_neurons = node_neurons->next;
    }

    return nb_neurons;
}

/**
 * Get the bias of a neuron
 * @brief Specific to NeuralNetwork 
 * @param layer_index Index of the layer to get bias
 * @param neu_index Index of the neuron
 * @return Bias of the specified neuron
 */
double PMMLlib::GetNeuronBias(int layer_index, 
                              int neu_index)
{
    CheckNeuralNetwork();  
    
    double bias = 0.;
    xmlNodePtr node_layers = GetChildByName(_currentModelNode,"NeuralLayer");
    if ( node_layers == NULL )
        return bias;  
    // Positionnement a la bonne couche
    for(int i=0;i<layer_index;i++)
    {
        node_layers = node_layers->next;
        if ( node_layers == NULL )
            return bias;          
    }
    xmlNodePtr node_neurons = GetChildByName(node_layers,"Neuron");
    // Positionnement sur le bon neurone
    for(int j=0;j<neu_index;j++)
    {
        node_neurons = node_neurons->next;
        if ( node_neurons == NULL )
            return bias;           
    }
    string str_tmp  = _getProp(node_neurons, string("bias"));
    bias = atof(str_tmp.c_str());
    return bias;
}

/**
 * Get the synaptic weight
 * @brief Specific to NeuralNetwork 
 * @param layer_index Index of the layer to get synaptic weight
 * @param neu_index Index of the neuron
 * @param prec_index Index of the synapse
 * @return Synaptic weight
 */
double PMMLlib::GetPrecNeuronSynapse(int layer_index, 
                                     int neu_index, 
                                     int prec_index)
{
    CheckNeuralNetwork();
    
    double weight = 0.;
    xmlNodePtr node_layers = GetChildByName(_currentModelNode,"NeuralLayer");
    if ( node_layers == NULL )
        return weight;     
    // Positionnement a la bonne couche
    for(int i=0;i<layer_index;i++)
    {
        node_layers = node_layers->next;
        if ( node_layers == NULL )
            return weight;        
    }
    xmlNodePtr node_neurons = GetChildByName(node_layers,"Neuron");
    // Positionnement sur le bon neurone
    for(int i=0;i<neu_index;i++)
    {
        node_neurons = node_neurons->next;
        if ( node_neurons == NULL )
            return weight;          
    }
    xmlNodePtr node_con = GetChildByName(node_neurons,"Con");
    // Positionnement sur la bonne synapse
    for(int i=0;i<prec_index;i++)
    {
        node_con = node_con->next;
        if ( node_con == NULL )
            return weight;          
    }
    string str_tmp  = _getProp(node_con, string("weight"));
    weight = atof(str_tmp.c_str());
    return weight;
}

/**
 * Set the name of the neural network
 * @brief Not tested 
 * @param index Neural network index
 * @param name Neural network name to set
 */
// LCOV_EXCL_START
void PMMLlib::SetNeuralNetName(int index, 
                               std::string name)
{
    CheckNeuralNetwork();
    
    int i=0;
    if (_doc != NULL)
    {
        xmlNodePtr node_ann = GetChildByName(_rootNode,"NeuralNetwork");
        while ((i != index) && (node_ann != NULL))
        {
            node_ann = node_ann->next;
            i++;
        }
        xmlNewProp(node_ann, (const xmlChar*)"modelName", (const xmlChar*)(name.c_str()));
    }
    xmlSaveFormatFile( string(_pmmlFile+".pmml").c_str(), _doc, 1);
}
// LCOV_EXCL_STOP

/**
 * Add a DataField node to the DataDictionnary node
 * @param fieldName Value of property "name"
 * @param displayName Value of property "displayName"
 * @param optype Value of property "optype"
 * @param dataType Value of property "dataType"
 * @param closure Value of property "closure" in node Interval
 * @param leftMargin Value of property "leftMargin" in node Interval
 * @param rightMargin Value of property "rightMargin" in node Interval
 * @param interval Flag to add a node Interval (if true)
 */
void PMMLlib::AddDataField(std::string fieldName, 
                           std::string displayName, 
                           std::string optype,
                           std::string dataType, 
                           std::string closure, 
                           double leftMargin, 
                           double rightMargin, 
                           bool interval)
{
    // if 'DataDictionary' node does not exist, create it
    xmlNodePtr dataDictNode = GetChildByName(_rootNode, "DataDictionary");
    if(!dataDictNode)
    {
        dataDictNode = xmlNewChild(_rootNode, 0, (const xmlChar*)"DataDictionary", 0);
    }

    // then append the node
    xmlNodePtr dataFieldNode = xmlNewChild(dataDictNode, 0, (const xmlChar*)"DataField", 0);
    xmlNewProp(dataFieldNode, (const xmlChar*)"name", (const xmlChar*)(fieldName.c_str()) );
    xmlNewProp(dataFieldNode, (const xmlChar*)"displayName", (const xmlChar*)(displayName.c_str()) );
    xmlNewProp(dataFieldNode, (const xmlChar*)"optype", (const xmlChar*)(optype.c_str()) );
    xmlNewProp(dataFieldNode, (const xmlChar*)"dataType", (const xmlChar*)(dataType.c_str()) );

    if ( interval ) 
    {
        xmlNodePtr intervalNode = xmlNewChild(dataFieldNode, 0, (const xmlChar*)"Interval", 0);
        xmlNewProp(intervalNode, (const xmlChar*)"closure", (const xmlChar*)(closure.c_str()) );
        stringstream ss;
        ss << scientific << leftMargin;
        xmlNewProp(intervalNode, (const xmlChar*)"leftMargin", (const xmlChar*)(ss.str().c_str()) );
        ss.str("");
        ss << scientific << rightMargin;
        xmlNewProp(intervalNode, (const xmlChar*)"rightMargin", (const xmlChar*)(ss.str().c_str()) );
    }
}

/**
 * Add a NeuralNetwork node to the root node
  * @brief Specific to NeuralNetwork
 * @param modelName Model name
 * @param functionName PMMLMiningFunction. One of : kREGRESSION.
 */ 
void PMMLlib::AddNeuralNetwork(std::string modelName, 
                               PMMLMiningFunction functionName)
{
    _currentModelType = kANN;
    _currentModelName = modelName;
    
    CheckNeuralNetwork();
    
    string function;
    switch(functionName)
    {
    case kREGRESSION:
        function = "regression"; 
        break;
    }

    xmlNodePtr netNode = xmlNewChild(_rootNode, 0, (const xmlChar*)"NeuralNetwork", 0);
    xmlNewProp(netNode, (const xmlChar*)"modelName", (const xmlChar*)(_currentModelName.c_str()) );
    xmlNewProp(netNode, (const xmlChar*)"functionName", (const xmlChar*)(function.c_str()) );
    xmlNewProp(netNode, (const xmlChar*)"numberOfLayers", (const xmlChar*)"0" );
    _currentModelNode = netNode;
}

 /**
 * Add a NeuralInput node to the current model.
  * @brief Specific to NeuralNetwork
 * @param id Id of the input
 * @param inputName Name of the input
 * @param optype Value of property "optype"
 * @param dataType Value of property "dataType" 
 * @param orig1 Value of the first origin
 * @param norm1 Value of the first norm
 * @param orig2 Value of the second origin
 * @param norm2 Value of the second norm
 */ 
void PMMLlib::AddNeuralInput(int id, 
                             std::string inputName, 
                             std::string optype, 
                             std::string dataType, 
                             double orig1, double norm1, 
                             double orig2, double norm2)
{
    CheckNeuralNetwork();

    xmlNodePtr netNode = _currentModelNode; 
    // if 'NeuralInputs' node does not exist, create it
    xmlNodePtr neuralInputsNode = GetChildByName(netNode, "NeuralInputs");
    if(!neuralInputsNode)
    {
        neuralInputsNode = xmlNewChild(netNode, 0, (const xmlChar*)"NeuralInputs", 0);
        xmlNewProp(neuralInputsNode, (const xmlChar*)"numberOfInputs", (const xmlChar*)"0" );
    }
    // increment the number of inputs
    string numberOfInputsStr = _getProp(neuralInputsNode, string("numberOfInputs"));
    int numberOfInputs;
    istringstream( numberOfInputsStr ) >> numberOfInputs;        
    numberOfInputs++;
    stringstream ss;
    ss << numberOfInputs;
    xmlSetProp(neuralInputsNode, (const xmlChar*)"numberOfInputs", (const xmlChar*)(ss.str().c_str()) );
    // then append the node and its children
    xmlNodePtr neuralInputNode = xmlNewChild(neuralInputsNode, 0, (const xmlChar*)"NeuralInput", 0);
    ss.str(""); ss << id;
    xmlNewProp(neuralInputNode, (const xmlChar*)"id", (const xmlChar*)(ss.str().c_str()) );

    xmlNodePtr derivedFieldNode = xmlNewChild(neuralInputNode, 0, (const xmlChar*)"DerivedField", 0);
    xmlNewProp(derivedFieldNode, (const xmlChar*)"optype", (const xmlChar*)(optype.c_str()) );
    xmlNewProp(derivedFieldNode, (const xmlChar*)"dataType", (const xmlChar*)(dataType.c_str()) );

    xmlNodePtr normcontNode = xmlNewChild(derivedFieldNode, 0, (const xmlChar*)"NormContinuous", 0);
    xmlNewProp(normcontNode, (const xmlChar*)"field", (const xmlChar*)(inputName.c_str()) );

    xmlNodePtr node_linearnorm1 = xmlNewChild(normcontNode, 0, (const xmlChar*)"LinearNorm", 0);
    ss.str(""); ss << scientific << orig1;
    xmlNewProp(node_linearnorm1, (const xmlChar*)"orig", (const xmlChar*)(ss.str().c_str()) );
    ss.str(""); ss << scientific << norm1;
    xmlNewProp(node_linearnorm1, (const xmlChar*)"norm", (const xmlChar*)(ss.str().c_str()) );
    xmlNodePtr node_linearnorm2 = xmlNewChild(normcontNode, 0, (const xmlChar*)"LinearNorm", 0);
    ss.str(""); ss << scientific << orig2;
    xmlNewProp(node_linearnorm2, (const xmlChar*)"orig", (const xmlChar*)(ss.str().c_str()) );
    ss.str(""); ss << scientific << norm2;
    xmlNewProp(node_linearnorm2, (const xmlChar*)"norm", (const xmlChar*)(ss.str().c_str()) );
}

 /**
 * Add a NeuralOutput node to the current model.
 * @brief Specific to NeuralNetwork
 * @param outputNeuron Id of the output
 * @param outputName Name of the output
 * @param optype Value of property "optype"
 * @param dataType Value of property "dataType" 
 * @param orig1 Value of the first origin
 * @param norm1 Value of the first norm
 * @param orig2 Value of the second origin
 * @param norm2 Value of the second norm
 */ 
void PMMLlib::AddNeuralOutput(int outputNeuron, 
                              std::string outputName, 
                              std::string optype, 
                              std::string dataType, 
                              double orig1, double norm1, 
                              double orig2, double norm2)
{
    CheckNeuralNetwork();

     xmlNodePtr netNode = _currentModelNode; 
    // if 'NeuralOutputs' node does not exist, create it
    xmlNodePtr neuralOutputsNode = GetChildByName(netNode, "NeuralOutputs");
    if(!neuralOutputsNode)
    {
        neuralOutputsNode = xmlNewChild(netNode, 0, (const xmlChar*)"NeuralOutputs", 0);
        xmlNewProp(neuralOutputsNode, (const xmlChar*)"numberOfOutputs", (const xmlChar*)"0" );
    }
    // increment the number of inputs
    string numberOfOutputsStr = _getProp(neuralOutputsNode, string("numberOfOutputs"));
    int numberOfOutputs;
    istringstream( numberOfOutputsStr ) >> numberOfOutputs;       
    numberOfOutputs++;
    stringstream ss;
    ss << numberOfOutputs;
    xmlSetProp(neuralOutputsNode, (const xmlChar*)"numberOfOutputs", (const xmlChar*)(ss.str().c_str()) );

    // then append the node and its children
    xmlNodePtr neuralOutputNode = xmlNewChild(neuralOutputsNode, 0, (const xmlChar*)"NeuralOutput", 0);
    ss.str(""); ss << outputNeuron;
    xmlNewProp(neuralOutputNode, (const xmlChar*)"outputNeuron", (const xmlChar*)(ss.str().c_str()) );

    xmlNodePtr derivedFieldNode = xmlNewChild(neuralOutputNode, 0, (const xmlChar*)"DerivedField", 0);
    xmlNewProp(derivedFieldNode, (const xmlChar*)"optype", (const xmlChar*)(optype.c_str()) );
    xmlNewProp(derivedFieldNode, (const xmlChar*)"dataType", (const xmlChar*)(dataType.c_str()) );

    xmlNodePtr normcontNode = xmlNewChild(derivedFieldNode, 0, (const xmlChar*)"NormContinuous", 0);
    xmlNewProp(normcontNode, (const xmlChar*)"field", (const xmlChar*)(outputName.c_str()) );

    xmlNodePtr node_linearnorm1 = xmlNewChild(normcontNode, 0, (const xmlChar*)"LinearNorm", 0);
    ss.str(""); ss << scientific << orig1;
    xmlNewProp(node_linearnorm1, (const xmlChar*)"orig", (const xmlChar*)(ss.str().c_str()) );
    ss.str(""); ss << scientific << norm1;
    xmlNewProp(node_linearnorm1, (const xmlChar*)"norm", (const xmlChar*)(ss.str().c_str()) );
    xmlNodePtr node_linearnorm2 = xmlNewChild(normcontNode, 0, (const xmlChar*)"LinearNorm", 0);
    ss.str(""); ss << scientific << orig2;
    xmlNewProp(node_linearnorm2, (const xmlChar*)"orig", (const xmlChar*)(ss.str().c_str()) );
    ss.str(""); ss << scientific << norm2;
    xmlNewProp(node_linearnorm2, (const xmlChar*)"norm", (const xmlChar*)(ss.str().c_str()) );
}

 /**
 * Add a NeuralLayer node to the current model.
  * @brief Specific to NeuralNetwork
 * @param activationFunction Activation function. One of kIDENTITY, kTANH, kLOGISTIC.
 */ 
void PMMLlib::AddNeuralLayer(PMMLActivationFunction activationFunction)
{
    CheckNeuralNetwork();
    
    string functionName;
    switch(activationFunction)
    {
    case kIDENTITY:
        functionName = "identity";
        break;
    case kTANH:
        functionName = "tanh";
        break;
    case kLOGISTIC:
        functionName = "logistic";
        break;
    }
    xmlNodePtr netNode = _currentModelNode; 
    // Increment the number of layers
    string numberOfLayersStr = _getProp(_currentModelNode, string("numberOfLayers"));
    int numberOfLayers;
    istringstream( numberOfLayersStr ) >> numberOfLayers;       
    numberOfLayers++;
    stringstream ss;
    ss << numberOfLayers;
    xmlSetProp(netNode, (const xmlChar*)"numberOfLayers", (const xmlChar*)(ss.str().c_str()) );
    // Add the neural layer node
    xmlNodePtr neuralLayerNode = xmlNewChild(netNode, 0, (const xmlChar*)"NeuralLayer", 0);
    xmlNewProp(neuralLayerNode, (const xmlChar*)"activationFunction", (const xmlChar*)(functionName.c_str()) );
    xmlNewProp(neuralLayerNode, (const xmlChar*)"numberOfNeurons", (const xmlChar*)"0" );
    // Save the current layer in the _currentNode attribute
    _currentNode = neuralLayerNode;
}

 /**
 * Add a NeuralLayer node to the current model.
  * @brief Specific to NeuralNetwork
 * @param id Id of the layer
 * @param bias Value of property "bias"
 * @param conNb Number of Con nodes
 * @param firstFrom Value of property "from" for the first Con
 * @param weights Vector of weights (One per Con node)
 */ 
void PMMLlib::AddNeuron(int id, 
                        double bias, 
                        int conNb, 
                        int firstFrom, 
                        vector<double> weights)
{
    CheckNeuralNetwork();
    
    stringstream ss;

    // increment the number of neurons
    string numberOfNeuronsStr = _getProp(_currentNode, string("numberOfNeurons"));
    int numberOfNeurons;
    istringstream( numberOfNeuronsStr ) >> numberOfNeurons;    
    numberOfNeurons++;
    ss << numberOfNeurons;
    xmlSetProp(_currentNode, (const xmlChar*)"numberOfNeurons", (const xmlChar*)(ss.str().c_str()) );

    // append a neuron
    xmlNodePtr neuronNode = xmlNewChild(_currentNode, 0, (const xmlChar*)"Neuron", 0);
    ss.str(""); ss << id;
    xmlNewProp(neuronNode, (const xmlChar*)"id", (const xmlChar*)(ss.str().c_str()) );
    ss.str(""); ss << scientific << bias;
    xmlNewProp(neuronNode, (const xmlChar*)"bias", (const xmlChar*)(ss.str().c_str()) );

    // append multiple 'Con' to the neuron
    for(int k=0 ; k<conNb ; k++)
    {
        xmlNodePtr conNode = xmlNewChild(neuronNode, 0, (const xmlChar*)"Con", 0);
        ss.str(""); ss << firstFrom+k;
        xmlNewProp(conNode, (const xmlChar*)"from", (const xmlChar*)(ss.str().c_str()) ); // !!! ce n'est pas k !!!
        ss.str(""); ss << scientific << weights[k];
        xmlNewProp(conNode, (const xmlChar*)"weight", (const xmlChar*)(ss.str().c_str()) );
    }
}

 /**
 * Fill the vectors used by the ExportXXX methods.
  * @brief Specific to NeuralNetwork
 * @param nInput
 * @param nOutput
 * @param nHidden
 * @param normType
 * @param minInput
 * @param maxInput
 * @param minOutput
 * @param maxOutput
 * @param valW
 */ 
void PMMLlib::fillVectorsForExport(int nInput, 
                                   int nOutput, 
                                   int nHidden, 
                                   int normType,
                                   vector<double> &minInput,
                                   vector<double> &maxInput,
                                   vector<double> &minOutput,
                                   vector<double> &maxOutput,
                                   vector<double> &valW )
{
    CheckNeuralNetwork();
    
    xmlNodePtr netNode = _currentModelNode ; 
    // Get the different values required
    // Build min/max input/output vectors
    for(int i=0 ; i<nInput ; i++)
    {
        xmlNodePtr node_inputs = GetChildByName(netNode,"NeuralInputs");
        node_inputs = node_inputs->children;
        for(int j = 0;j<i;j++)
        {
            node_inputs = node_inputs->next;
        }
        node_inputs = node_inputs->children; // DerivedField
        node_inputs = node_inputs->children; // NormContinuous
        node_inputs = node_inputs->children; // LinearNorm
        string strOrig1 = _getProp(node_inputs, string("orig") );
        double orig1 = atof( strOrig1.c_str() );
        string strNorm1 = _getProp(node_inputs, string("norm") );        
        double norm1 = atof( strNorm1.c_str() );
        node_inputs = node_inputs->next;
        string strOrig2 = _getProp(node_inputs, string("orig") );
        double orig2 = atof( strOrig2.c_str() );
        string strNorm2 = _getProp(node_inputs, string("norm") );    
        if( normType==0 )
        {   // kMinusOneOne
            minInput[i] = orig1;
            maxInput[i] = orig2;
        }
        else
        {   //  kCR, kZeroOne
            minInput[i] = orig2;
            maxInput[i] = -1.0*norm1*orig2;
        }
    }
    xmlNodePtr node_outputs = GetChildByName(netNode,"NeuralOutputs");
    node_outputs = node_outputs->children;
    node_outputs = node_outputs->children; // DerivedField
    node_outputs = node_outputs->children; // NormContinuous
    node_outputs = node_outputs->children; // LinearNorm  
    string strOrig1 = _getProp(node_outputs, string("orig") );
    double orig1 = atof( strOrig1.c_str() );
    string strNorm1 = _getProp(node_outputs, string("norm") );        
    double norm1 = atof( strNorm1.c_str() ); 
    node_outputs = node_outputs->next;
    string strOrig2 = _getProp(node_outputs, string("orig") );
    double orig2 = atof( strOrig2.c_str() );    
    if( normType==0 )
    {   // kMinusOneOne
        minOutput[0] = orig1;
        maxOutput[0] = orig2;
    }
    else
    {   //  kCR, kZeroOne
        minOutput[0] = orig2;
        maxOutput[0] = -1.0*norm1*orig2;
    }
    // Build weight vector
    for(int j=0 ; j<nHidden ; j++) // hidden layers
    {
        valW[j*(nInput+nOutput+1)+2] = GetNeuronBias( 0, j);
        for(int i=0 ; i<nInput ; i++)
        {
            valW[j*(nInput+nOutput+1)+3+i] = GetPrecNeuronSynapse( 0, j, i);        
        }
    }  
    for(int j=0 ; j<nOutput ; j++) // output layers
    {
        valW[0] = GetNeuronBias( 1, j);                
        for(int i=0 ; i<nHidden ; i++)
        {
            valW[i*(nInput+nOutput+1)+1] = GetPrecNeuronSynapse( 1, j, i);           
        }
    }
}

/**
 * Export the current model as a NeuralNetwork function in a Cpp file.
 * @brief Specific to NeuralNetwork
 * @param file Name of the file  
 * @param functionName Name of the function  
 * @param header Header of the function  
 */
void PMMLlib::ExportNeuralNetworkCpp(std::string file, 
                                     std::string functionName, 
                                     std::string header)
{
    CheckNeuralNetwork();
    
    // Get the different values required
    int nInput = GetNbInputs();
    int nOutput = GetNbOutputs();
    int nHidden = GetNbNeuronsAtLayer(0);
    int nNeurons = nInput+nOutput+nHidden;
    int nWeights = nHidden*(nInput+nOutput+1)+nOutput;
    int normType = GetNormalizationType();
    // Build min/max input/output vectors
    vector<double> minInput(nInput);
    vector<double> maxInput(nInput);
    vector<double> minOutput(nOutput);
    vector<double> maxOutput(nOutput);
    vector<double> valW(nWeights);
    fillVectorsForExport(nInput,nOutput,nHidden,normType,minInput,maxInput,minOutput,maxOutput,valW);
    // Write the file
    ofstream sourcefile(file.c_str());
    // ActivationFunction
    if( normType==0 )
    {   // kMinusOneOne
        sourcefile << "#define ActivationFunction(sum)         ( tanh(sum) )" << endl;
    }
    else
    {   //  kCR, kZeroOne
        sourcefile << "#define ActivationFunction(sum) ( 1.0 / ( 1.0 + exp( -1.0 * sum )) )" << endl;
    }
    //
    sourcefile << "void " << functionName <<"(double *param, double *res)" << endl;
    sourcefile << "{" << endl;
    // header
    sourcefile << "  ////////////////////////////// " << endl;
    sourcefile << "  //" << endl;
    // insert comments in header
    header = "  // " + header;
    size_t pos = 0;
    while ((pos = header.find("\n", pos)) != std::string::npos) 
    {
        header.replace(pos, 1, "\n  //");
        pos += 5;
    }
    sourcefile << header << endl;
    sourcefile << "  //" << endl;
    sourcefile << "  ////////////////////////////// " << endl;
    sourcefile << endl;
    sourcefile << "  int nInput   = " <<  nInput << ";" << endl;
    sourcefile << "  int nOutput   = " <<  nOutput << ";" << endl;
    //  sourcefile << "  int nWeights = " <<  _nWeight << ";" << endl;
    sourcefile << "  int nHidden  = " <<  nHidden << ";" << endl;
    sourcefile << "  const int nNeurones  = " <<  nNeurons << ";" << endl;
    sourcefile << "  double " << functionName << "_act[nNeurones];" << endl;
    sourcefile << endl;
    sourcefile << "  // --- Preprocessing of the inputs and outputs" << endl;
    sourcefile << "  double " << functionName << "_minInput[] = {" << endl << "  ";
    for(int i=0 ; i<nInput ; i++)
    {
        sourcefile << minInput[i] << ", ";
        if( (i+1)%5==0 )
            sourcefile << "\n  ";
    }
    if( nInput%5 != 0 )
        sourcefile << endl;
    sourcefile << "  };" << endl;
    //
    sourcefile << "  double " << functionName << "_minOutput[] = {" << endl << "  ";
    sourcefile << minOutput[0] << ", ";
    sourcefile << "  };" << endl;
    //
    sourcefile << "  double " << functionName << "_maxInput[] = {" << endl << "  ";
    for(int i=0 ; i<nInput ; i++)
    {
        sourcefile << maxInput[i] << ", ";
        if( (i+1)%5==0 )
            sourcefile << "\n  ";
    }
    if( nInput%5 != 0 )
        sourcefile << endl;
    sourcefile << "  };" << endl;
    //
    sourcefile << "  double " << functionName << "_maxOutput[] = {" << endl << "  ";
    sourcefile << maxOutput[0] << ", ";
    sourcefile << "  };" << endl;
    // Weights vector
    sourcefile << endl;
    sourcefile << "  // --- Values of the weights" << endl;
    sourcefile << "  double " << functionName << "_valW[] = {" << endl << "  ";
    for(int i=0 ; i<nWeights ; i++)
    {
        sourcefile << valW[i] << ", ";
        if ( (i+1)%5 == 0 )
            sourcefile << endl << "  ";
    }
    sourcefile << endl << "  };"<<endl;
    //
    sourcefile << "  // --- Constants";
    sourcefile << endl;
    sourcefile << "  int indNeurone = 0;"<<endl;
    sourcefile << "  int CrtW;"<<endl;
    sourcefile << "  double sum;"<<endl;

    // couche  entree
    sourcefile << endl;
    sourcefile << "  // --- Input Layers"<<endl;
    sourcefile << "  for(int i = 0; i < nInput; i++) {"<<endl;
    if( normType==0 )
    {   // kMinusOneOne
        sourcefile << "     " << functionName << "_act[indNeurone++] = 2.0 * ( param[i] - "
                   << functionName << "_minInput[i] ) / ( " << functionName << "_maxInput[i] - "
                   << functionName << "_minInput[i] ) - 1.0;"<<endl;
    }
    else
    {   //  kCR, kZeroOne
        sourcefile << "     " << functionName << "_act[indNeurone++] = ( param[i] - "
                   << functionName << "_minInput[i] ) / " << functionName << "_maxInput[i];"
                   << endl;
    }
    sourcefile << "  }"<<endl;


    // couche cachee
    sourcefile << endl;
    sourcefile << "  // --- Hidden Layers"<<endl;
    sourcefile << "  for (int member = 0; member < nHidden; member++) {"<<endl;
    sourcefile << "     int CrtW = member * ( nInput + 2) + 2;" << endl;
    sourcefile << "     sum = " << functionName << "_valW[CrtW++];" << endl;
    sourcefile << "     for (int source = 0; source < nInput; source++) {" << endl;
    sourcefile << "         sum += " << functionName << "_act[source] * " << functionName << "_valW[CrtW++];" << endl;
    sourcefile << "       }" << endl;
    sourcefile << "       " << functionName << "_act[indNeurone++] = ActivationFunction(sum);" << endl;
    sourcefile << "  }"<<endl;
    // couche sortie
    sourcefile << endl;
    sourcefile << "  // --- Output"<<endl;
    sourcefile << "  for (int member = 0; member < nOutput; member++) {"<<endl;
    sourcefile << "    sum = " << functionName << "_valW[0];"<<endl;
    sourcefile << "    for (int source = 0; source < nHidden; source++) {"<<endl;
    sourcefile << "      CrtW = source * ( nInput + 2) + 1;"<<endl;
    sourcefile << "      sum += " << functionName << "_act[nInput+source] * " << functionName << "_valW[CrtW];"<<endl;
    sourcefile << "    }"<<endl;
    sourcefile << "    " << functionName << "_act[indNeurone++] = sum;"<<endl;
    if( normType==0 )
    {   // kMinusOneOne
        sourcefile << "    res[member] = " << functionName
                   << "_minOutput[member] + 0.5 * ( " << functionName
                   << "_maxOutput[member] - " << functionName
                   << "_minOutput[member] ) * ( sum + 1.0);" << endl;
    }
    else
    {   //  kCR, kZeroOne
        sourcefile << "    res[member] = " << functionName
                   << "_minOutput[member] + " << functionName
                   << "_maxOutput[member] * sum;" << endl;
    }
    sourcefile << "  }"<<endl;
    //
    sourcefile << "}" << endl;
    sourcefile.close();
}

/**
 * Export the current model as a NeuralNetwork function in a Fortran file.
 * @brief Specific to NeuralNetwork
 * @param file Name of the file  
 * @param functionName Name of the function  
 * @param header Header of the function  
 */
void PMMLlib::ExportNeuralNetworkFortran(std::string file, 
                                         std::string functionName, 
                                         std::string header)
{
    CheckNeuralNetwork();  
    
    // Get the different values required
    int nInput = GetNbInputs();
    int nOutput = GetNbOutputs();
    int nHidden = GetNbNeuronsAtLayer(0);
    int nWeights = nHidden*(nInput+nOutput+1)+nOutput;
    int normType = GetNormalizationType();
    // Build min/max input/output vectors
    vector<double> minInput(nInput);
    vector<double> maxInput(nInput);
    vector<double> minOutput(nOutput);
    vector<double> maxOutput(nOutput);
    vector<double> valW(nWeights);
    fillVectorsForExport(nInput,nOutput,nHidden,normType,minInput,maxInput,minOutput,maxOutput,valW);
    // Write the file
    ofstream sourcefile(file.c_str());

    sourcefile << "      SUBROUTINE " << functionName << "(";
    for(int i=0 ; i<GetNbInputs() ; i++)
    {
        sourcefile << GetNameInput(i) << ",";
    }
    sourcefile << GetNameOutput(0) << ")" << endl;
    // header
    sourcefile << "C --- *********************************************" << endl;
    sourcefile << "C --- " << endl;
    // insert comments in header
    header = "C ---  " + header;
    size_t pos = 0;
    while ((pos = header.find("\n", pos)) != std::string::npos) 
    {
        header.replace(pos, 1, "\nC --- ");
        pos += 5;
    }
    sourcefile << header << endl;
    sourcefile << "C --- " << endl;
    sourcefile << "C --- *********************************************" << endl;

    sourcefile << "      IMPLICIT DOUBLE PRECISION (V)" << endl;
    for(int i=0 ; i<GetNbInputs() ; i++)
    {
        sourcefile << "      DOUBLE PRECISION " << GetNameInput(i) << endl;
    }
    sourcefile << "      DOUBLE PRECISION " << GetNameOutput(0) << endl;
    sourcefile << endl;

    sourcefile << "C --- Preprocessing of the inputs" << endl;
    for(int i=0 ; i<GetNbInputs() ; i++)
    {
        sourcefile << "      VXN" << GetNameInput(i) << " = ";

        if( normType==0 )
        {   // kMinusOneOne
            sourcefile << "2.D0 * ( " << GetNameInput(i) << " - " << minInput[i] << "D0 ) / " << maxInput[i] - minInput[i] << "D0 - 1.0" << endl;
        }
        else
        {   //  kCR, kZeroOne
            sourcefile << "( " << GetNameInput(i) << " - " << minInput[i] << "D0 ) / " << maxInput[i] << "D0" << endl;
        }
    }

    // Weights vector
    sourcefile << endl;
    sourcefile << "C --- Values of the weights" << endl;
    for(int i=0 ; i<nWeights ; i++)
    {
        sourcefile << "      VW" << i+1 << " = " << valW[i] << endl;
    }
    // Loop on hidden neurons
    sourcefile << endl;
    for(int member = 0; member < nHidden; member++) 
    {
        sourcefile << "C --- hidden neural number " << member+1 << endl;
        int CrtW = member * ( nInput + 2) + 3;
        sourcefile << "      VAct" << member+1 << " = VW" << CrtW++ << endl;
        for (int source = 0; source < nInput; source++)
        {
            sourcefile << "     1      + VW"<< CrtW++ << " * VXN" << GetNameInput(source) << endl;
        }
        sourcefile << endl;


        if( normType==0 )
        {   // kMinusOneOne
            sourcefile << "      VPot" << member+1 << " = 2.D0 / (1.D0 + DEXP(-2.D0 * VAct" << member+1 <<")) - 1.D0" << endl;
        }
        else
        {   //  kCR, kZeroOne
            sourcefile << "      VPot" << member+1 << " = 1.D0 / (1.D0 + DEXP(-1.D0 * VAct" << member+1 <<"))" << endl;
        }
        sourcefile << endl;
    }

    // Ouput of the model
    sourcefile << "C --- Output" << endl;
    sourcefile << "      VOut = VW1" << endl;
    for(int source=0 ; source < nHidden ; source++)
    {
        int CrtW = source * ( nInput + 2) + 2;
        sourcefile << "     1    + VW"<< CrtW  << " * VPot" << source+1 << endl;
    }

    // Denormalize Output
    sourcefile << endl;
    sourcefile << "C --- Pretraitment of the output" << endl;
    if( normType==0 )
    {   // kMinusOneOne
        sourcefile << "      VDelta = " << 0.5*(maxOutput[0]-minOutput[0]) << "D0 * ( VOut + 1.0D0)" << endl;
        sourcefile << "      " << GetNameOutput(0) << " = " << minOutput[0] << "D0 + VDelta" << endl;

    }
    else
    {   //  kCR, kZeroOne
        sourcefile << "      " << GetNameOutput(0) << " = "<< minOutput[0] << "D0 + " << maxOutput[0] << "D0 * VOut;" << endl;
    }

    sourcefile << endl;
    sourcefile << "C --- " << endl;
    sourcefile << "      RETURN" << endl;
    sourcefile << "      END" << endl;

    sourcefile.close();
}

/**
 * Export the current model as a NeuralNetwork function in a Python file.
 * @brief Specific to NeuralNetwork
 * @param file Name of the file  
 * @param functionName Name of the function  
 * @param header Header of the function  
 */
void PMMLlib::ExportNeuralNetworkPython(std::string file, 
                                        std::string functionName, 
                                        std::string header)
{
    string str(ExportNeuralNetworkPyStr(functionName, header));
    // Write the file
    ofstream exportfile(file.c_str()); 
    exportfile << str;
    exportfile.close();  
}


/**
 * Export the current model as a function in a Python string.
 * @brief Specific to NeuralNetwork
 * @param functionName Name of the function  
 * @param header Header of the function  
 * @return Function as a string
 */ 
std::string PMMLlib::ExportNeuralNetworkPyStr(std::string functionName, 
                                              std::string header)
{
    CheckNeuralNetwork();
    
    ostringstream out;
  
    // Get the different values required
    int nInput = GetNbInputs();
    int nOutput = GetNbOutputs();
    int nHidden = GetNbNeuronsAtLayer(0);
    int nNeurons = nInput+nOutput+nHidden;
    int nWeights = nHidden*(nInput+nOutput+1)+nOutput;
    int normType = GetNormalizationType();
    // Build min/max input/output vectors
    vector<double> minInput(nInput);
    vector<double> maxInput(nInput);
    vector<double> minOutput(nOutput);
    vector<double> maxOutput(nOutput);
    vector<double> valW(nWeights);
    fillVectorsForExport(nInput,nOutput,nHidden,normType,minInput,maxInput,minOutput,maxOutput,valW);

    // Shebang et imports
    out << "#!/usr/bin/env python" << endl;
    out << "# -*- coding: utf-8 -*-" << endl;
    out << endl;
    out << "from math import tanh, exp" << endl;
    out << endl;

    // ActivationFunction
    if( normType==0 )
    {   // kMinusOneOne
        out << "def ActivationFunction(sum): " << endl;
        out << "    return tanh(sum); " << endl;
    }
    else
    {   //  kCR, kZeroOne
        out << "def ActivationFunction(sum): " << endl;
        out << "    return ( 1.0 / ( 1.0 + exp( -1.0 * sum ) ) ); " << endl;        
    }

    out << endl;
    out << "def " << functionName <<"(param):" << endl;
    out << endl;

    // header
    out << "    ############################## " << endl;
    out << "    #" << endl;
    // insert comments in header
    header = "    # " + header;
    size_t pos = 0;
    while ((pos = header.find("\n", pos)) != std::string::npos) 
    {
        header.replace(pos, 1, "\n    #");
        pos += 5;
    }
    out << header << endl;
    out << "    #" << endl;
    out << "    ############################## " << endl;
    out << endl;

    // Initialisations
    out << "    nInput = " << nInput << ";" << endl;
    out << "    nOutput = " << nOutput << ";" << endl;
    out << "    nHidden = " <<  nHidden << ";" << endl;
    out << "    nNeurones = " <<  nNeurons << ";" << endl;
    out << "    " << functionName << "_act = [];" << endl;
    out << "    res = [];" << endl;    
    out << endl;

    out << "    # --- Preprocessing of the inputs and outputs" << endl;
    out << "    " << functionName << "_minInput = [" << endl << "  ";
    out << "    " ;
    for(int i=0 ; i<nInput ; i++)
    {
        out << minInput[i] << ", ";
        if( (i+1)%5==0 )
        {
            out << endl ;
            out << "    " ;
        }
    }
    out << endl <<  "    ];" << endl;

    out << "    " << functionName << "_minOutput = [" << endl << "    ";
    out << "    " << minOutput[0] ;
    out << endl << "    ];" << endl;

    out << "    " << functionName << "_maxInput = [" << endl << "    ";
    for(int i=0 ; i<nInput ; i++)
    {
        out << maxInput[i] << ", ";
        if( (i+1)%5==0 )
        {
            out << endl;
            out << "    " ;
        }
    }
    out << endl << "    ];" << endl;

    out << "    " << functionName << "_maxOutput = [" << endl << "    ";
    out << "    " << maxOutput[0] ;
    out << endl << "    ];" << endl;

    // Weights vector
    out << "    # --- Values of the weights" << endl;
    out << "    " << functionName << "_valW = [" << endl << "    ";
    for(int i=0 ; i<nWeights ; i++)
    {
        out << valW[i] << ", ";
        if ( (i+1)%5 == 0 )
        {
            out << endl;
            out << "    " ;
        }
    }
    out << endl << "    ];"<<endl;

    out << "    # --- Constants" << endl;
    out << "    indNeurone = 0;" << endl;
    out << endl;
    
    // couche entree
    out << "    # --- Input Layers" << endl;
    out << "    for i in range(nInput) :" << endl;
    if( normType==0 )
    {   // kMinusOneOne
        out << "        " << functionName << "_act.append( 2.0 * ( param[i] - "
                   << functionName << "_minInput[i] ) / ( " << functionName << "_maxInput[i] - "
                   << functionName << "_minInput[i] ) - 1.0 ) ;"
                   << endl;                  
    }
    else
    {   //  kCR, kZeroOne
        out << "        " << functionName << "_act.append( ( param[i] - "
                   << functionName << "_minInput[i] ) / " << functionName << "_maxInput[i] ) ;"
                   << endl;
    }   
    out << "        indNeurone += 1 ;" << endl;
    out << "        pass" << endl;
    
    // couche cachee
    out << endl;
    out << "    # --- Hidden Layers" << endl;
    out << "    for member in range(nHidden):" << endl;
    out << "        CrtW = member * ( nInput + 2) + 2;" << endl;
    out << "        sum = " << functionName << "_valW[CrtW];" << endl;
    out << "        CrtW += 1 ;" << endl;     
    out << "        for source in range(nInput) :" << endl;
    out << "            sum += " << functionName << "_act[source] * " << functionName << "_valW[CrtW];" << endl;
    out << "            CrtW += 1 ;" << endl; 
    out << "            pass" << endl;
    out << "        " << functionName << "_act.append( ActivationFunction(sum) ) ;" << endl;
    out << "        indNeurone += 1 ;" << endl;
    out << "        pass" << endl;
    out << endl;
    
    // couche sortie
    out << "    # --- Output"<<endl;
    out << "    for member in range(nOutput):" << endl; 
    out << "        sum = " << functionName << "_valW[0];" << endl;
    out << "        for source in range(nHidden):" << endl;
    out << "            CrtW = source * ( nInput + 2) + 1;"<<endl;
    out << "            sum += " << functionName << "_act[nInput+source] * " << functionName << "_valW[CrtW];" << endl;
    out << "            pass" << endl;
    out << "        " << functionName << "_act.append( sum );" << endl;
    out << "        indNeurone += 1 ;" << endl;
    if( normType==0 )
    {   // kMinusOneOne
        out << "        res[member] = " << functionName
                   << "_minOutput[member] + 0.5 * ( " << functionName
                   << "_maxOutput[member] - " << functionName
                   << "_minOutput[member] ) * ( sum + 1.0);" << endl;
    }
    else
    {   //  kCR, kZeroOne
        out << "        res.append( " << functionName
                   << "_minOutput[member] + " << functionName
                   << "_maxOutput[member] * sum );" << endl;
    }
    out << "        pass" << endl;
    out << endl;
    
    // return result
    out << "    return res;" << endl << endl;
    out << endl;    
   
    return out.str(); 
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

/**
 * Check if the current model type is kLR.
 *    \brief Called in all methods specific to the RegressionModel model.
 *    \brief Throw an exception if the model type is not kLR.
 */
void PMMLlib::CheckRegression()
{
    if ( _currentModelType != kLR )
        throw string("Use this method with Regression models."); 
}

/**
 * Get the pointeur to the regression model node.
 * @param name Name of the regression model 
 * @return Pointer to the XML node
 */
xmlNodePtr PMMLlib::GetRegressionPtr(std::string name)
{
    return GetPtr(name, GetTypeString() );  
}   

/**
 * Add a RegressionModel to the root node
 * @brief Specific to RegressionModel
 * @param modelName Name of the model (Value of property "modelName")
 * @param functionName Value of property "functionName"
 * @param targetFieldName Value of Property "targetFieldName"
 */
void PMMLlib::AddRegressionModel(std::string modelName, 
                                 PMMLMiningFunction functionName, 
                                 std::string targetFieldName)
{
    _currentModelType = kLR;
    _currentModelName = modelName;
    // Check regression after setting model type!
    CheckRegression();
    
    string function;
    switch(functionName)
    {
        case kREGRESSION:
            function = "regression"; 
            break;
    }
    xmlNodePtr netNode = xmlNewChild(_rootNode, 0, (const xmlChar*)"RegressionModel", 0);     
    xmlNewProp(netNode, (const xmlChar*)"functionName", (const xmlChar*)(function.c_str()) );
    xmlNewProp(netNode, (const xmlChar*)"modelName", (const xmlChar*)(_currentModelName.c_str()) );
    xmlNewProp(netNode, (const xmlChar*)"targetFieldName", (const xmlChar*)(targetFieldName.c_str()) );
    _currentModelNode = netNode ;
}

/**
 * Add a RegressionTable to the Regression model.
 * @brief No property "intercept" will be set.
 * @brief Specific to RegressionModel 
 */
void PMMLlib::AddRegressionTable()
{
    CheckRegression();  
    xmlNodePtr tableNode = xmlNewChild(_currentModelNode, 0, (const xmlChar*)"RegressionModel", 0);
    _currentNode = tableNode;
}

/**
 * Add a RegressionTable to the Regression model with a given value of property "intercept".
 * @brief Specific to RegressionModel 
 * @param intercept Value of property "intercept"
 */
void PMMLlib::AddRegressionTable(double intercept)
{
    CheckRegression();

    stringstream ss;
    xmlNodePtr tableNode = xmlNewChild(_currentModelNode, 0, (const xmlChar*)"RegressionTable", 0);
    if(intercept!=0.0)
    {
        ss << scientific << intercept;
        xmlNewProp(tableNode, (const xmlChar*)"intercept", (const xmlChar*)(ss.str().c_str()) );
    }
    _currentNode = tableNode;
}

/**
 * Add a numeric predictor to the Regression model.
 * @brief Specific to RegressionModel 
 * @param neuronName  Value of property "name"
 * @param exponent Value of property "exponent"
 * @param coefficient Value of property "coefficient"
 */
void PMMLlib::AddNumericPredictor(std::string neuronName, 
                                  int exponent, 
                                  double coefficient)
{
    CheckRegression(); 
    stringstream ss;
    xmlNodePtr numPrecNode = xmlNewChild(_currentNode, 0, (const xmlChar*)"NumericPredictor", 0);
    xmlNewProp(numPrecNode, (const xmlChar*)"name", (const xmlChar*)(neuronName.c_str()) );
    ss.str("");  ss << exponent;
    xmlNewProp(numPrecNode, (const xmlChar*)"exponent", (const xmlChar*)(ss.str().c_str()) );
    ss.str("");  ss << scientific << coefficient;
    xmlNewProp(numPrecNode, (const xmlChar*)"coefficient", (const xmlChar*)(ss.str().c_str()) );
}

/**
 * Add a predictor term to the Regression model.
 * @brief Specific to RegressionModel  
 * @param coefficient Value of property "coefficient"
 * @param fieldRef List of values for property "field", one per FieldRef to add to the PredictorTerm
 */
void PMMLlib::AddPredictorTerm(double coefficient,
                               std::vector<std::string> fieldRef)
{
    CheckRegression();
    stringstream ss;
    xmlNodePtr predTermNode = xmlNewChild(_currentNode, 0, (const xmlChar*)"PredictorTerm", 0);
    ss.str("");  ss << scientific << coefficient;
    xmlNewProp(predTermNode, (const xmlChar*)"coefficient", (const xmlChar*)(ss.str().c_str()) );
    vector<string>::iterator it;
    for(it=fieldRef.begin() ; it!=fieldRef.end() ; it++)
    {
        xmlNodePtr fieldRefNode = xmlNewChild(predTermNode, 0, (const xmlChar*)"FieldRef", 0);
        ss.str(""); ss << (*it);
        xmlNewProp(fieldRefNode, (const xmlChar*)"field", (const xmlChar*)(ss.str().c_str()) );
    }
}

/**
 * Check if the RegressionTable has a property called "intercept".
 * @brief Specific to RegressionModel  
 * @return true if it has, false otherwise
 */
bool PMMLlib::HasIntercept()
{
    CheckRegression();
    bool b = false;   
    xmlNodePtr tableNode = GetChildByName(_currentModelNode,"RegressionTable");
    if ( tableNode == NULL )
      return b;         
    xmlChar *xp = _stringToXmlChar("intercept");
    xmlChar * attr ;
    attr = xmlGetProp(tableNode, xp);
    if ( attr ) 
    {
        xmlFree(attr);
        xmlFree(xp);  
        return true;
    }  
    xmlFree(xp);  
    return false;
}

/**
 * Get the value of property "intercept" in the RegressionTable.
 * @brief Specific to RegressionModel  
 * @return Value of property "intercept"
 */
double PMMLlib::GetRegressionTableIntercept()
{
    CheckRegression();
    double reg = 0.;
    xmlNodePtr tableNode = GetChildByName(_currentModelNode,"RegressionTable");
    if ( tableNode == NULL )
        return reg; 
    string strValue = _getProp(tableNode, string("intercept") );
    return atof(strValue.c_str());
}

/**
 * Get the number of numeric predictors.
 * @brief Specific to RegressionModel  
 * @return Number of numeric predictors
 */
int PMMLlib::GetNumericPredictorNb()
{
    CheckRegression();
    
    int nb=0;   
    xmlNodePtr tableNode = GetChildByName(_currentModelNode,"RegressionTable");
    if ( tableNode == NULL )
        return nb;
    xmlNodePtr numPredNodes =  tableNode->children;
    while (numPredNodes != NULL )
    {
        if ( string((const char*)(numPredNodes->name)) == "NumericPredictor" )
            nb++;
        numPredNodes = numPredNodes->next;
    }    
    return nb;
}

/**
 * Get the number of predictor terms.
 * @brief Specific to RegressionModel  
 * @return Number of predictor terms
 */
int PMMLlib::GetPredictorTermNb()
{
    CheckRegression();
    int nb=0;    
    xmlNodePtr tableNode = GetChildByName(_currentModelNode,"RegressionTable");
    if ( tableNode == NULL )
      return nb;  
    xmlNodePtr numPredNodes =  tableNode->children;
    while ( numPredNodes != NULL )      
    {
        if ( string((const char*)(numPredNodes->name)) == "PredictorTerm" )
            nb++;
        numPredNodes = numPredNodes->next;
    }    
    return nb;
}

/**
 * Get the name of the numeric predictor given by its index.
 * @brief Specific to RegressionModel  
 * @param num_pred_index Index of the numeric predictor
 * @return Name of the numeric predictor
 */
std::string PMMLlib::GetNumericPredictorName(int num_pred_index)
{
    CheckRegression();
    string strName("");   
    xmlNodePtr numPredNodes = GetChildByName(_currentModelNode,"RegressionTable");
    if ( numPredNodes == NULL )
        return strName;
    
    numPredNodes = GetChildByName(numPredNodes,"NumericPredictor");
    if ( numPredNodes == NULL )
        return strName;
    // Positionnement sur la bonne sortie PredictorTerm
    for(int i=0;i<num_pred_index;i++)
    {
        numPredNodes = numPredNodes->next;
        if ( numPredNodes == NULL || 
             string((const char*)(numPredNodes->name)) != "NumericPredictor" )
            return strName;        
    }        
    strName = _getProp(numPredNodes, string("name"));
    return strName;
}

/**
 * Get the name of the predictor term given by its index.
 * @brief Specific to RegressionModel  
 * @param pred_term_index Index of the predictor term 
 * @return Name of the predictor term
 */
std::string PMMLlib::GetPredictorTermName(int pred_term_index)
{
    CheckRegression();
    string strName("");   
    xmlNodePtr fieldRefNodes = GetChildByName(_currentModelNode,"RegressionTable");
    if ( fieldRefNodes == NULL )
        return strName;    
    
    fieldRefNodes = GetChildByName(fieldRefNodes,"PredictorTerm");
    if ( fieldRefNodes == NULL )
        return strName;
    // Positionnement sur la bonne sortie
    for(int i=0;i<pred_term_index;i++)
    {
        fieldRefNodes = fieldRefNodes->next;
        if ( fieldRefNodes == NULL || 
            string((const char*)(fieldRefNodes->name)) != "PredictorTerm" )
            return strName;        
    }  
    
    fieldRefNodes = fieldRefNodes->children;
    while (fieldRefNodes != NULL)
    {
        strName += _getProp(fieldRefNodes, string("field"));
        fieldRefNodes = fieldRefNodes->next;
    }        
    return strName;
}

/**
 * Get the coefficient of the numeric predictor given by its index.
 * @brief (The coefficient is the value of property "coefficient")
 * @brief Specific to RegressionModel  
 * @param num_pred_index Index of the numeric predictor
 * @return Coefficient of the numeric predictor
 */
double PMMLlib::GetNumericPredictorCoefficient(int num_pred_index)
{
    CheckRegression();
    
    double coef = 0.;  
    xmlNodePtr numPredNodes = GetChildByName(_currentModelNode,"RegressionTable");
    if ( numPredNodes == NULL )
        return coef;    
    numPredNodes = GetChildByName(numPredNodes,"NumericPredictor");
    if ( numPredNodes == NULL )
        return coef;
    // Positionnement sur la bonne sortie
    for(int i=0;i<num_pred_index;i++)
    {
        numPredNodes = numPredNodes->next;
        if ( numPredNodes == NULL || 
             string((const char*)(numPredNodes->name)) != "NumericPredictor" )
            return coef;        
    }  
    string strValue = _getProp(numPredNodes, string("coefficient")); 
    coef = atof(strValue.c_str());
    return coef;  
}

/**
 * Get the coefficient of the predictor term given by its index.
 * @brief (The coefficient is the value of property "coefficient")
 * @brief Specific to RegressionModel  
 * @param pred_term_index Index of the predictor term
 * @return Coefficient of the predictor term
 */
double PMMLlib::GetPredictorTermCoefficient(int pred_term_index)
{
    CheckRegression();
    
    double coef = 0.;   
    xmlNodePtr predTermNodes = GetChildByName(_currentModelNode,"RegressionTable");
    if ( predTermNodes == NULL )
        return coef;    
    predTermNodes = GetChildByName(predTermNodes,"PredictorTerm");
    if ( predTermNodes == NULL )
        return coef;
    // Positionnement sur la bonne sortie
    for(int i=0;i<pred_term_index;i++)
    {
        predTermNodes = predTermNodes->next;
        if ( predTermNodes == NULL || 
             string((const char*)(predTermNodes->name)) != "PredictorTerm" )
            return coef;        
    }  
    string strValue = _getProp(predTermNodes, string("coefficient")); 
    coef = atof(strValue.c_str());
    return coef;
}

/**
 * Get the number of FieldRef for the predictor term given by its index.
 * @brief Specific to RegressionModel   
 * @param indsex Index of the predictor term
 * @return Number of FieldRef
 */
int PMMLlib::GetPredictorTermFieldRefNb(int index)
{
    CheckRegression();
    
    int nb=0;
    xmlNodePtr fieldRefNodes = GetChildByName(_currentModelNode,"RegressionTable");
    if ( fieldRefNodes == NULL )
      return nb;
    fieldRefNodes = GetChildByName(fieldRefNodes,"PredictorTerm");
    if ( fieldRefNodes == NULL )
        return nb;
    // Positionnement sur la bonne sortie
    for(int i=0;i<index;i++)
    {
        fieldRefNodes = fieldRefNodes->next;
        if ( fieldRefNodes == NULL || 
             string((const char*)(fieldRefNodes->name)) != "PredictorTerm" )
            return nb;        
    }  
    fieldRefNodes = fieldRefNodes->children;
    while (fieldRefNodes != NULL)
    {
        nb++;
        fieldRefNodes = fieldRefNodes->next;
    }    
    return nb;
}

/**
 * Get the name of the field_index-th FieldRef for the pred_term_index-th predictor term.
 * @brief (The name is the value of property "field")
 * @brief Specific to RegressionModel   
 * @param pred_term_index Index of the predictor term
 * @param field_index Index of the FieldRef
 * @return Name of the FieldRef
 */
std::string PMMLlib::GetPredictorTermFieldRefName(int pred_term_index, int field_index)   
{
    CheckRegression();
    
    string strName("");    
    xmlNodePtr fieldRefNodes = GetChildByName(_currentModelNode,"RegressionTable");
    if ( fieldRefNodes == NULL )
      return strName;
    fieldRefNodes = GetChildByName(fieldRefNodes,"PredictorTerm");
    if ( fieldRefNodes == NULL )
        return strName;
    // Positionnement sur la bonne sortie PredictorTerm
    for(int i=0;i<pred_term_index;i++)
    {
        fieldRefNodes = fieldRefNodes->next;
        if ( fieldRefNodes == NULL || 
             string((const char*)(fieldRefNodes->name)) != "PredictorTerm" )
            return strName;        
    }  
    fieldRefNodes = fieldRefNodes->children;
    if ( fieldRefNodes == NULL )
        return strName;
    // Positionnement sur la bonne sortie FieldRef
    for(int i=0;i<field_index;i++)
    {
        fieldRefNodes = fieldRefNodes->next;
        if ( fieldRefNodes == NULL )
            return strName;        
    }     
    strName = _getProp(fieldRefNodes, string("field"));
    return strName;
}

/**
 * Export the current model as a NeuralNetwork function in a Cpp file.
 * @brief Specific to RegressionModel  
 * @param file Name of the file  
 * @param functionName Name of the function  
 * @param header Header of the function  
 */
void PMMLlib::ExportLinearRegressionCpp(std::string file, 
                                        std::string functionName, 
                                        std::string header)
{
    CheckRegression();

    // Write the file
    ofstream exportfile(file.c_str());
    
    exportfile << "void " << functionName <<"(double *param, double *res)" << endl;
    exportfile << "{" << endl;    
    // header
    exportfile << "  ////////////////////////////// " << endl;
    exportfile << "  //" << endl;
    // insert comments in header
    header = "  // " + header;
    size_t pos = 0;
    while ((pos = header.find("\n", pos)) != std::string::npos) 
    {
        header.replace(pos, 1, "\n  //");
        pos += 5;
    }
    exportfile << header << endl;
    exportfile << "  //" << endl;
    exportfile << "  ////////////////////////////// " << endl << endl;  

    double intercept = 0.0;    
    if ( HasIntercept() ) 
    {
        exportfile << "  // Intercept"<< endl;
        intercept = GetRegressionTableIntercept();
    }
    else 
        exportfile << "  // No Intercept"<< endl;
    exportfile << "  double y = " << intercept << ";";
    exportfile << endl << endl;
     
    int nPred = GetNumericPredictorNb();
    for (int i=0; i<nPred; i++)
    {
       exportfile << "  // Attribute : " << GetNumericPredictorName(i) << endl;
       exportfile << "  y += param["<<i<<"]*" << GetNumericPredictorCoefficient(i) << ";";
       exportfile << endl << endl;
    }
    nPred = GetPredictorTermNb();
    for (int i=0; i<nPred; i++)
    {
       exportfile << "  // Attribute : " << GetPredictorTermName(i) << endl;
       exportfile << "  y += param["<<(i+nPred)<<"]*" << GetPredictorTermCoefficient(i) << ";";
       exportfile << endl << endl;
    }    
    
    exportfile << "  // Return the value"<< endl;
    exportfile << "  res[0] = y;" << endl;
    exportfile << "}" << endl;    
    exportfile.close(); 
}

/**
 * Export the current model as a NeuralNetwork function in a Fortran file.
 * @brief Specific to Regression
 * @param file Name of the file  
 * @param functionName Name of the function  
 * @param header Header of the function  
 */
void PMMLlib::ExportLinearRegressionFortran(std::string file, 
                                            std::string functionName, 
                                            std::string header)
{
    CheckRegression();

    int nNumPred = GetNumericPredictorNb();
    int nPredTerm = GetPredictorTermNb(); 
    vector<string>strParam(nNumPred+nPredTerm);
    for(int i=0; i<(nNumPred+nPredTerm); i++)
    {
        strParam[i] = "P" + NumberToString(i) ;
    }    
    
    // Write the file
    ofstream exportfile(file.c_str());
    
    exportfile << "      SUBROUTINE " << functionName <<"(";
    for(int i=0; i<(nNumPred+nPredTerm); i++)
    {
        exportfile << strParam[i] << ", ";
    }    
    exportfile << "RES)" << endl;    
 
    // header
    exportfile << "C --- *********************************************" << endl;
    exportfile << "C --- " << endl;
    // insert comments in header
    header = "C ---  " + header;
    size_t pos = 0;
    while ((pos = header.find("\n", pos)) != std::string::npos) 
    {
        header.replace(pos, 1, "\nC --- ");
        pos += 5;
    }
    exportfile << header << endl;
    exportfile << "C --- " << endl;
    exportfile << "C --- *********************************************" << endl << endl;  

    exportfile << "      IMPLICIT DOUBLE PRECISION (P)" << endl;
    exportfile << "      DOUBLE PRECISION RES" << endl;    
    exportfile << "      DOUBLE PRECISION Y" << endl;
    exportfile << endl;    
    
    double intercept = 0.0;    
    if ( HasIntercept() ) 
    {
        exportfile << "C --- Intercept"<< endl;
        intercept = GetRegressionTableIntercept();
    }
    else 
        exportfile << "C --- No Intercept"<< endl;
    exportfile << "      Y = " << intercept << ";";
    exportfile << endl << endl;
     
    for (int i=0; i<nNumPred; i++)
    {
       exportfile << "C --- Attribute : " << GetNumericPredictorName(i) << endl;
       exportfile << "      Y += P["<<i<<"]*" << GetNumericPredictorCoefficient(i) << ";";
       exportfile << endl << endl;
    }

    for (int i=0; i<nPredTerm; i++)
    {
       exportfile << "C --- Attribute : " << GetPredictorTermName(i) << endl;
       exportfile << "      Y += P["<<(i+nNumPred)<<"]*" << GetPredictorTermCoefficient(i) << ";";
       exportfile << endl << endl;
    }    
    
    exportfile << "C --- Return the value"<< endl;
    exportfile << "      RES = Y " << endl;
    exportfile << "      RETURN" << endl;    
    exportfile << "      END" << endl;    
    exportfile.close(); 
}

/**
 * Export the current model as a NeuralNetwork function in a Python file.
 * @brief Specific to Regression
 * @param file Name of the file  
 * @param functionName Name of the function  
 * @param header Header of the function  
 */
void PMMLlib::ExportLinearRegressionPython(std::string file, 
                                           std::string functionName, 
                                           std::string header)
{
    string str(ExportLinearRegressionPyStr(functionName, header));
    // Write the file
    ofstream exportfile(file.c_str()); 
    exportfile << str;
    exportfile.close(); 
}

/**
 * Export the current model as a NeuralNetwork function in a Python string.
 * @brief Specific to Regression 
 * @param functionName Name of the function  
 * @param header Header of the function  
 */
std::string PMMLlib::ExportLinearRegressionPyStr(std::string functionName, 
                                                 std::string header)
{
    CheckRegression();

    ostringstream out;
    
    // Shebang et imports
    out << "#!/usr/bin/env python" << endl;
    out << "# -*- coding: utf-8 -*-" << endl;
    out << endl;    
    
    // Function
    out << "def " << functionName <<"(param):" << endl;
    out << endl; 
    
    // header
    out << "    ############################## " << endl;
    out << "    # " << endl;
    // insert comments in header
    header = "    # " + header;
    size_t pos = 0;
    while ((pos = header.find("\n", pos)) != std::string::npos) 
    {
        header.replace(pos, 1, "\n    #");
        pos += 5;
    }    
    out << header << endl;
    out << "    # " << endl;
    out << "    ############################## " << endl << endl;  

    double intercept = 0.0;    
    if ( HasIntercept() ) 
    {
        out << "    #  Intercept"<< endl;
        intercept = GetRegressionTableIntercept();
    }
    else 
        out << "    #  No Intercept"<< endl;
    out << "    y = " << intercept << ";";
    out << endl << endl;
     
    int nPred = GetNumericPredictorNb();
    for (int i=0; i<nPred; i++)
    {
       out << "    #  Attribute : " << GetNumericPredictorName(i) << endl;
       out << "    y += param["<<i<<"]*" << GetNumericPredictorCoefficient(i) << ";";
       out << endl << endl;
    }
    nPred = GetPredictorTermNb();
    for (int i=0; i<nPred; i++)
    {
       out << "    #  Attribute : " << GetPredictorTermName(i) << endl;
       out << "    y += param["<<(i+nPred)<<"]*" << GetPredictorTermCoefficient(i) << ";";
       out << endl << endl;
    }    
    
    out << "    #  Return the value"<< endl;
    out << "    return [y];" << endl;
        
    return out.str() ;
}

/**
 * Read the structure of the regression model
 * @brief Specific to RegressionModel 
 * @return Structure read
 */
std::string PMMLlib::ReadRegressionStructure()
{   
    CheckRegression(); 

    string structure("");    
    string structureActive("");
    string structurePredicted("@");
    int nPred = 0;
    xmlNodePtr mNode = GetChildByName(_currentModelNode,"MiningSchema");
    if ( mNode != NULL )
    {
        xmlNodePtr dNode = GetChildByName(mNode,"MiningField");     
        while (dNode != NULL)
        {
            string name = _getProp(dNode, string("name"));
            string usage = _getProp(dNode, string("usageType"));
            if ( usage == "active" )
            {
                structureActive += name;
                structureActive += ":"; 
            }    
            else if ( usage == "predicted" )
            {
                structurePredicted += name;
                structurePredicted += ":"; 
                nPred++;
            }                 
            dNode = dNode->next;             
        }
        // Delete the last ":"
        if ( structureActive.length() > 0 )
            structureActive.erase(structureActive.size()-1);        
        structurePredicted.erase(structurePredicted.size()-1);
    }
    std::ostringstream oss;
    oss << nPred;   
    structure = structureActive + ","  + oss.str() + "," + structurePredicted;
    return structure;
}

} // end of namespace


