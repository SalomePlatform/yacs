#include "RuntimeSALOME.hxx"
#include "SessionCataLoader.hxx"
#include "ComponentDefinition.hxx"
#include "ServiceNode.hxx"

#include <iostream>
#include <sstream>

#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;

SessionCataLoader::SessionCataLoader(const std::string& path):CatalogLoader(path)
{
  //path should be a CORBA URI like that : corbaname::machine:port/NameService#Kernel.dir/ModulCatalog.object
  //or equivalent corbaname
}

/*
  enum DataStreamType {
    DATASTREAM_UNKNOWN,
    DATASTREAM_INTEGER,
    DATASTREAM_FLOAT,
    DATASTREAM_DOUBLE,
    DATASTREAM_STRING,
    DATASTREAM_BOOLEAN
  } ;
*/
std::map<SALOME_ModuleCatalog::DataStreamType,std::string> datastreamMap;

//! import a SALOME component definition in a YACS catalog
/*!
 * SALOME components can have several interfaces. 
 * YACS for the moment allows only one interface by component, so if there is only one interface we import it.
 * If we have more than one interface, we import the one with the same name as the component or the first one.
 *
 *   \param cata  : a YACS catalog
 *   \param compo : a CORBA reference on a component in a SALOME_ModuleCatalog
 */
void SessionCataLoader::importComponent(Catalog* cata,SALOME_ModuleCatalog::Acomponent_ptr compo)
{
  CORBA::String_var componame = compo->componentname();
  SALOME_ModuleCatalog::ListOfInterfaces_var interfacename_list = compo->GetInterfaceList();
  if(interfacename_list->length() == 0)
    return;
  int found=0;
  for (int j = 0; j < interfacename_list->length();j++) 
    {
      DEBTRACE ("Interface : " << interfacename_list[j]);
      if(strcmp(componame,interfacename_list[j]) == 0)
         {
           found=j;
           break;
         }
    }
  DEBTRACE ("Interface found : " << interfacename_list[found]);
  ComponentDefinition* compodef=new ComponentDefinition((const char*)componame);

  SALOME_ModuleCatalog::DefinitionInterface_var interface;
  interface= compo->GetInterface(interfacename_list[found]);
  SALOME_ModuleCatalog::ListOfInterfaceService services = interface->interfaceservicelist;
  for (int k = 0; k < services.length(); k++)
    {
      DEBTRACE("Service : " << services[k].ServiceName);
      const char* s=services[k].ServiceName;
      //create a new service node (prototype)
      ServiceNode* node=getRuntime()->createCompoNode("Salome",s);
      node->setRef((const char*)componame);
      node->setMethod(s);
      for (int kk=0;kk< services[k].ServiceinParameter.length();kk++)
        {
          DEBTRACE("Parameter : " <<  services[k].ServiceinParameter[kk].Parametername);
          DEBTRACE("Type : " <<  services[k].ServiceinParameter[kk].Parametertype);
          const char* ty=services[k].ServiceinParameter[kk].Parametertype;
          if(cata->_typeMap.count(ty)==0)
            {
              std::cerr << "Type " << ty << " does not exist" << " (" <<__FILE__ << ":" << __LINE__ << ")" << std::endl;
              delete node;
              node=0;
              break;
            }
          const char* name=services[k].ServiceinParameter[kk].Parametername;
          node->edAddInputPort(name,cata->_typeMap[ty]);
        }
      if(node == 0)continue;

      for (int kk=0;kk< services[k].ServiceoutParameter.length();kk++)
        {
          DEBTRACE("Parameter : " <<  services[k].ServiceoutParameter[kk].Parametername);
          DEBTRACE("Type : " <<  services[k].ServiceoutParameter[kk].Parametertype);
          const char* ty=services[k].ServiceoutParameter[kk].Parametertype;
          if(cata->_typeMap.count(ty)==0)
            {
              std::cerr << "Type " << ty << " does not exist" << " (" <<__FILE__ << ":" << __LINE__ << ")" << std::endl;
              delete node;
              node=0;
              break;
            }
          const char* name=services[k].ServiceoutParameter[kk].Parametername;
          node->edAddOutputPort(name,cata->_typeMap[ty]);
        }
      if(node == 0)continue;

      for (int kk=0;kk< services[k].ServiceinDataStreamParameter.length();kk++)
        {
          DEBTRACE("Parameter : " <<  services[k].ServiceinDataStreamParameter[kk].Parametername);
          DEBTRACE("Type : " <<  services[k].ServiceinDataStreamParameter[kk].Parametertype);
          DEBTRACE("Dependency : " <<  services[k].ServiceinDataStreamParameter[kk].Parameterdependency);
          std::string ty=datastreamMap[services[k].ServiceinDataStreamParameter[kk].Parametertype];
          if(cata->_typeMap.count(ty)==0)
            {
              std::cerr << "Type " << ty << " does not exist" << " (" <<__FILE__ << ":" << __LINE__ << ")" << std::endl;
              delete node;
              node=0;
              break;
            }
          const char* name=services[k].ServiceinDataStreamParameter[kk].Parametername;
          InputDataStreamPort* port;
          port=node->edAddInputDataStreamPort(name,cata->_typeMap[ty]);
        }
      if(node == 0)continue;

      for (int kk=0;kk< services[k].ServiceoutDataStreamParameter.length();kk++)
        {
          DEBTRACE("Parameter : " <<  services[k].ServiceoutDataStreamParameter[kk].Parametername);
          DEBTRACE("Type : " <<  services[k].ServiceoutDataStreamParameter[kk].Parametertype);
          DEBTRACE("Dependency : " <<  services[k].ServiceoutDataStreamParameter[kk].Parameterdependency);
          std::string ty=datastreamMap[services[k].ServiceoutDataStreamParameter[kk].Parametertype];
          if(cata->_typeMap.count(ty)==0)
            {
              std::cerr << "Type " << ty << " does not exist" << " (" <<__FILE__ << ":" << __LINE__ << ")" << std::endl;
              delete node;
              node=0;
              break;
            }
          const char* name=services[k].ServiceoutDataStreamParameter[kk].Parametername;
          OutputDataStreamPort* port;
          port=node->edAddOutputDataStreamPort(name,cata->_typeMap[ty]);
          // Set all properties for this port
          /*
          std::map<std::string, std::string>::const_iterator pt;
          for(pt=p._props.begin();pt!=p._props.end();pt++)
            port->setProperty((*pt).first,(*pt).second);
            */
        }
      if(node == 0)continue;
      compodef->_serviceMap[s]=node;
    }
  if(compodef->_serviceMap.size() > 0)
    {
      cata->_componentMap[(const char*)componame]=compodef;
    }
  else
    {
      delete compodef;
    }
}

SessionCataLoader::~SessionCataLoader()
{
  DEBTRACE ("SessionCataLoader::~SessionCataLoader");
}

TypeCode * createInterfaceTc(const std::string& id, const std::string& name,
                             std::list<TypeCodeObjref *> ltc)
{
    std::string myName;
    if(id == "") myName = "IDL:" + name + ":1.0";
    else myName = id;
    return TypeCode::interfaceTc(myName.c_str(),name.c_str(),ltc);
}

void SessionCataLoader::loadCata(Catalog* cata)
{
  DEBTRACE("SessionCataLoader::load")
  datastreamMap[SALOME_ModuleCatalog::DATASTREAM_UNKNOWN]="CALCIUM_unknown";
  datastreamMap[SALOME_ModuleCatalog::DATASTREAM_INTEGER]="CALCIUM_integer";
  datastreamMap[SALOME_ModuleCatalog::DATASTREAM_FLOAT]="CALCIUM_real";
  datastreamMap[SALOME_ModuleCatalog::DATASTREAM_DOUBLE]="CALCIUM_double";
  datastreamMap[SALOME_ModuleCatalog::DATASTREAM_BOOLEAN]="CALCIUM_boolean";
  datastreamMap[SALOME_ModuleCatalog::DATASTREAM_STRING]="CALCIUM_string";

  CORBA::ORB_ptr orb = getSALOMERuntime()->getOrb();
  CORBA::Object_var obj;
  SALOME_ModuleCatalog::ModuleCatalog_var catalog;
  obj=orb->string_to_object(_path.c_str());
  catalog= SALOME_ModuleCatalog::ModuleCatalog::_narrow(obj);
  // Fill the types map with built in types
  Runtime* r=getRuntime();
  std::map<std::string,TypeCode*>& typeMap=cata->_typeMap;
  r->_tc_double->incrRef();
  cata->_typeMap["double"]=r->_tc_double;
  r->_tc_double->incrRef();
  cata->_typeMap["float"]=r->_tc_double;
  r->_tc_string->incrRef();
  cata->_typeMap["string"]=r->_tc_string;
  r->_tc_string->incrRef();
  cata->_typeMap["char"]=r->_tc_string;
  r->_tc_int->incrRef();
  cata->_typeMap["int"]=r->_tc_int;
  r->_tc_int->incrRef();
  cata->_typeMap["long"]=r->_tc_int;
  r->_tc_int->incrRef();
  cata->_typeMap["short"]=r->_tc_int;
  r->_tc_bool->incrRef();
  cata->_typeMap["bool"]=r->_tc_bool;
  r->_tc_bool->incrRef();
  cata->_typeMap["boolean"]=r->_tc_bool;
  cata->_typeMap["Study"]=TypeCode::interfaceTc("IDL:SALOMEDS/Study:1.0","Study");
  cata->_typeMap["SObject"]=TypeCode::interfaceTc("IDL:SALOMEDS/SObject:1.0","SObject");
  cata->_typeMap["GEOM_Object"]=TypeCode::interfaceTc("IDL:GEOM/GEOM_Object:1.0","GEOM_Object");
  cata->_typeMap["ListOfLong"]=TypeCode::sequenceTc("ListOfLong","ListOfLong",r->_tc_int);
  cata->_typeMap["GEOM_List"]=TypeCode::interfaceTc("IDL:GEOM/GEOM_List:1.0","GEOM_List");
  cata->_typeMap["GEOM_Superv"]=TypeCode::interfaceTc("IDL:GEOM/GEOM_Superv:1.0","GEOM_Superv");
  cata->_typeMap["ListOfGO"]=TypeCode::sequenceTc("ListOfGO","ListOfGO",cata->_typeMap["GEOM_Object"]);
  cata->_typeMap["string_array"]=TypeCode::sequenceTc("string_array","string_array",r->_tc_string);
  r->_tc_int->incrRef();
  typeMap["BCErrorType"]=r->_tc_int;
  typeMap["BCError"]=TypeCode::structTc("","BCError");
  ((TypeCodeStruct*)typeMap["BCError"])->addMember("incriminated",typeMap["ListOfLong"]);
  typeMap["BCErrors"]=TypeCode::sequenceTc("BCErrors","BCErrors",typeMap["BCError"]);
  typeMap["GEOM_Gen"]=TypeCode::interfaceTc("IDL:GEOM/GEOM_Gen:1.0","GEOM_Gen");
  typeMap["GEOM_Object"]->incrRef();
  typeMap["GEOM_Shape"]=typeMap["GEOM_Object"];
  typeMap["SMESH_Mesh"]=TypeCode::interfaceTc("IDL:Mesh/SMESH_Mesh:1.0","SMESH_Mesh");
  typeMap["SMESH_Hypothesis"]=TypeCode::interfaceTc("IDL:SMESH/SMESH_Hypothesis:1.0","SMESH_Hypothesis");

  typeMap["MED"]=TypeCode::interfaceTc("IDL:SALOME_MED/MED:1.0","MED");
  typeMap["FIELD"]=TypeCode::interfaceTc("IDL:SALOME_MED/FIELD:1.0","FIELD");
  // TODO : FIELDDOUBLE inherits FIELD
  typeMap["FIELDDOUBLE"]=TypeCode::interfaceTc("IDL:SALOME_MED/FIELDDOUBLE:1.0","FIELDDOUBLE");

  typeMap["AddComponent"]=TypeCode::interfaceTc("IDL:SuperVisionTest/AddComponent:1.0","AddComponent");
  typeMap["AddComponent"]->incrRef();
  typeMap["AddInterface"]=typeMap["AddComponent"];
  typeMap["SuperVisionTest/Adder"]=TypeCode::interfaceTc("IDL:SuperVisionTest/Adder:1.0","SuperVisionTest/Adder");
  typeMap["AdditionComponent/Adder"]=TypeCode::interfaceTc("IDL:AdditionComponent/Adder:1.0","AdditionComponent/Adder");
  typeMap["Syr"]=TypeCode::interfaceTc("IDL:SuperVisionTest/Syr:1.0","Syr");
  typeMap["ListOfSyr"]=TypeCode::interfaceTc("IDL:SuperVisionTest/ListOfSyr:1.0","ListOfSyr");
  //CALCIUM
  typeMap["CALCIUM_integer"]=TypeCode::interfaceTc("IDL:Ports/Calcium_Ports/Calcium_Integer_Port:1.0","CALCIUM_integer");
  typeMap["CALCIUM_real"]=TypeCode::interfaceTc("IDL:Ports/Calcium_Ports/Calcium_Real_Port:1.0","CALCIUM_real");
  typeMap["CALCIUM_double"]=TypeCode::interfaceTc("IDL:Ports/Calcium_Ports/Calcium_Double_Port:1.0","CALCIUM_double");
  typeMap["CALCIUM_string"]=TypeCode::interfaceTc("IDL:Ports/Calcium_Ports/Calcium_String_Port:1.0","CALCIUM_string");
  typeMap["CALCIUM_boolean"]=TypeCode::interfaceTc("IDL:Ports/Calcium_Ports/Calcium_Logical_Port:1.0","CALCIUM_boolean");

  // Get component list
  SALOME_ModuleCatalog::ListOfComponents_var componentname_list;
  componentname_list  = catalog->GetComponentList();
  for (int i=0; i<componentname_list->length(); i++)
    {
      DEBTRACE ("Component : " << componentname_list[i]);
      SALOME_ModuleCatalog::Acomponent_var component = catalog->GetComponent(componentname_list[i]);
      importComponent(cata,component);
    }
}
