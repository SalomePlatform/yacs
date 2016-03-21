// Copyright (C) 2006-2016  CEA/DEN, EDF R&D
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

#include "RuntimeSALOME.hxx"
#include "SessionCataLoader.hxx"
#include "ComponentDefinition.hxx"
#include "ServiceNode.hxx"
#include "TypeCode.hxx"

#include <iostream>
#include <sstream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

#define NEW_KERNEL

using namespace YACS::ENGINE;

SessionCataLoader::SessionCataLoader(const std::string& path):CatalogLoader(path)
{
  //path should be a CORBA URI like that : corbaname::machine:port/NameService#Kernel.dir/ModulCatalog.object
  //or equivalent corbaname
}

/*
 In salome catalog, the type names for CALCIUM are : INTEGER, FLOAT,DOUBLE,STRING,BOOLEAN
  enum DataStreamType {
    DATASTREAM_UNKNOWN,
    DATASTREAM_INTEGER,
    DATASTREAM_FLOAT,
    DATASTREAM_DOUBLE,
    DATASTREAM_STRING,
    DATASTREAM_BOOLEAN
  } ;
*/
#ifndef NEW_KERNEL
std::map<SALOME_ModuleCatalog::DataStreamType,std::string> datastreamMap;
#endif

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

  DEBTRACE("Implementation type : " << compo->implementation_type());
#ifdef NEW_KERNEL
  SALOME_ModuleCatalog::ImplType imptyp=compo->implementation_type();
  if(imptyp == SALOME_ModuleCatalog::SO)DEBTRACE("Dyn lib");
  if(imptyp == SALOME_ModuleCatalog::PY)DEBTRACE("Python module");
  if(imptyp == SALOME_ModuleCatalog::EXE)DEBTRACE("Executable");
  DEBTRACE("Implementation name : " << compo->implementation_name());
#endif

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
              std::stringstream msg; 
              msg << "Type " <<ty <<" does not exist. Service " << s << " of component " << componame << " is not available";
              _errors=_errors+"\n"+msg.str();
              std::cerr << msg.str() << std::endl;
              compodef->_serviceMap[s+std::string("_IS_INVALID")]=0;
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
              std::stringstream msg; 
              msg << "Type " <<ty <<" does not exist. Service " << s << " of component " << componame << " is not available";
              _errors=_errors+"\n"+msg.str();
              std::cerr << msg.str() << std::endl;
              compodef->_serviceMap[s+std::string("_IS_INVALID")]=0;
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
#ifdef NEW_KERNEL
          const char* ty=services[k].ServiceinDataStreamParameter[kk].Parametertype;
#else
          std::string ty=datastreamMap[services[k].ServiceinDataStreamParameter[kk].Parametertype];
#endif
          if(cata->_typeMap.count(ty)==0)
            {
              std::stringstream msg; 
              msg << "Type " <<ty <<" does not exist. Service " << s << " of component " << componame << " is not available";
              _errors=_errors+"\n"+msg.str();
              std::cerr << msg.str() << std::endl;
              compodef->_serviceMap[s+std::string("_IS_INVALID")]=0;
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
#ifdef NEW_KERNEL
          const char* ty=services[k].ServiceoutDataStreamParameter[kk].Parametertype;
#else
          std::string ty=datastreamMap[services[k].ServiceoutDataStreamParameter[kk].Parametertype];
#endif
          if(cata->_typeMap.count(ty)==0)
            {
              std::stringstream msg; 
              msg << "Type " <<ty <<" does not exist. Service " << s << " of component " << componame << " is not available";
              _errors=_errors+"\n"+msg.str();
              std::cerr << msg.str() << std::endl;
              compodef->_serviceMap[s+std::string("_IS_INVALID")]=0;
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

  /* Even if a component has no service, put it in the catalog
  if(compodef->_serviceMap.size() > 0)
    {
      cata->_componentMap[(const char*)componame]=compodef;
    }
  else
    {
      delete compodef;
    }
    */
  cata->_componentMap[(const char*)componame]=compodef;
}

SessionCataLoader::~SessionCataLoader()
{
  DEBTRACE ("SessionCataLoader::~SessionCataLoader");
}

void SessionCataLoader::loadTypes(Catalog* cata,SALOME_ModuleCatalog::ModuleCatalog_ptr catalog)
{
  Runtime* r=getRuntime();
  std::map<std::string,TypeCode*>& typeMap=cata->_typeMap;
  // Get types list
  SALOME_ModuleCatalog::ListOfTypeDefinition_var types_list=catalog->GetTypes();
  for (int i=0; i< types_list->length(); i++)
    {
      const char* name=types_list[i].name;
      DEBTRACE ("type : " << types_list[i].name << " " << types_list[i].kind);
      if(types_list[i].kind == SALOME_ModuleCatalog::Dble)
        {
          r->_tc_double->incrRef();
          typeMap[name]=r->_tc_double;
        }
      else if(types_list[i].kind == SALOME_ModuleCatalog::Int)
        {
          r->_tc_int->incrRef();
          typeMap[name]=r->_tc_int;
        }
      else if(types_list[i].kind == SALOME_ModuleCatalog::Bool)
        {
          r->_tc_bool->incrRef();
          typeMap[name]=r->_tc_bool;
        }
      else if(types_list[i].kind == SALOME_ModuleCatalog::Str)
        {
          r->_tc_string->incrRef();
          typeMap[name]=r->_tc_string;
        }
      else if(types_list[i].kind == SALOME_ModuleCatalog::Seq)
        {
          const char* content=types_list[i].content;
          if ( typeMap.find(content) != typeMap.end() )
            typeMap[name]=r->createSequenceTc(name,name,typeMap[content]);
          //else ignored !!
        }
      else if(types_list[i].kind == SALOME_ModuleCatalog::Array)
        {
          //ignored, for the moment !!
        }
      else if(types_list[i].kind == SALOME_ModuleCatalog::Objref)
        {
          std::list<TypeCodeObjref *> ltc;
          const char* id=types_list[i].id;
          for (int m=0; m< types_list[i].bases.length(); m++)
            {
              const char* b_name=types_list[i].bases[m];
              if(typeMap.find(b_name) != typeMap.end())
                ltc.push_back((TypeCodeObjref *)typeMap[b_name]);
              //else ignored !!!
            }
          typeMap[name]=r->createInterfaceTc(id,name,ltc);
        }
      else if(types_list[i].kind == SALOME_ModuleCatalog::Struc)
        {
          TypeCodeStruct* t=r->createStructTc("",name);
          for (int m=0; m< types_list[i].members.length(); m++)
            {
              const char* m_name=types_list[i].members[m].name;
              const char* m_type=types_list[i].members[m].type;
              if(typeMap.find(m_type) != typeMap.end())
                t->addMember(m_name,typeMap[m_type]);
              //else ignored !!!
            }
          typeMap[name]=t;
        }
      else
        {
          std::cerr << "Unknown kind: " << types_list[i].kind << std::endl;
        }
    }
}

void SessionCataLoader::loadTypesOld(Catalog* cata)
{
  // Fill the types map with built in types
  Runtime* r=getRuntime();
  std::map<std::string,TypeCode*>& typeMap=cata->_typeMap;

#ifdef NEW_KERNEL
  //Add old Calcium data types for KERNEL
  if(typeMap.count("CALCIUM_double")!=0)
    {
      typeMap["DOUBLE"]=typeMap["CALCIUM_double"];
      typeMap["DOUBLE"]->incrRef();
    }
  if(typeMap.count("CALCIUM_real")!=0)
    {
      typeMap["FLOAT"]=typeMap["CALCIUM_real"];
      typeMap["FLOAT"]->incrRef();
    }
  if(typeMap.count("CALCIUM_integer")!=0)
    {
      typeMap["INTEGER"]=typeMap["CALCIUM_integer"];
      typeMap["INTEGER"]->incrRef();
    }
  if(typeMap.count("CALCIUM_boolean")!=0)
    {
      typeMap["BOOLEAN"]=typeMap["CALCIUM_boolean"];
      typeMap["BOOLEAN"]->incrRef();
    }
  if(typeMap.count("CALCIUM_string")!=0)
    {
      typeMap["STRING"]=typeMap["CALCIUM_string"];
      typeMap["STRING"]->incrRef();
    }
#endif

  //GEOM
  /*
  typeMap["GEOM_Object"]=TypeCode::interfaceTc("IDL:GEOM/GEOM_Object:1.0","GEOM_Object");
  typeMap["ListOfLong"]=TypeCode::sequenceTc("ListOfLong","ListOfLong",r->_tc_int);
  typeMap["GEOM_List"]=TypeCode::interfaceTc("IDL:GEOM/GEOM_List:1.0","GEOM_List");
  typeMap["GEOM_Superv"]=TypeCode::interfaceTc("IDL:GEOM/GEOM_Superv:1.0","GEOM_Superv");
  typeMap["ListOfGO"]=TypeCode::sequenceTc("ListOfGO","ListOfGO",cata->_typeMap["GEOM_Object"]);
  typeMap["string_array"]=TypeCode::sequenceTc("string_array","string_array",r->_tc_string);
  r->_tc_int->incrRef();
  typeMap["BCErrorType"]=r->_tc_int;
  typeMap["BCError"]=TypeCode::structTc("","BCError");
  ((TypeCodeStruct*)typeMap["BCError"])->addMember("incriminated",typeMap["ListOfLong"]);
  typeMap["BCErrors"]=TypeCode::sequenceTc("BCErrors","BCErrors",typeMap["BCError"]);
  typeMap["GEOM_Gen"]=TypeCode::interfaceTc("IDL:GEOM/GEOM_Gen:1.0","GEOM_Gen");
  typeMap["GEOM_Object"]->incrRef();
  typeMap["GEOM_Shape"]=typeMap["GEOM_Object"];
  */

  //SMESH
  /*
  typeMap["SMESH_Mesh"]=TypeCode::interfaceTc("IDL:Mesh/SMESH_Mesh:1.0","SMESH_Mesh");
  typeMap["SMESH_Hypothesis"]=TypeCode::interfaceTc("IDL:SMESH/SMESH_Hypothesis:1.0","SMESH_Hypothesis");
  */

  //MED
  /*
  typeMap["MED"]=TypeCode::interfaceTc("IDL:SALOME_MED/MED:1.0","MED");
  typeMap["FIELD"]=TypeCode::interfaceTc("IDL:SALOME_MED/FIELD:1.0","FIELD");
  // TODO : FIELDDOUBLE inherits FIELD
  typeMap["FIELDDOUBLE"]=TypeCode::interfaceTc("IDL:SALOME_MED/FIELDDOUBLE:1.0","FIELDDOUBLE");
  */

  //COMPONENT
  /*
  typeMap["AddComponent"]=TypeCode::interfaceTc("IDL:SuperVisionTest/AddComponent:1.0","AddComponent");
  typeMap["AddComponent"]->incrRef();
  typeMap["AddInterface"]=typeMap["AddComponent"];
  typeMap["SuperVisionTest/Adder"]=TypeCode::interfaceTc("IDL:SuperVisionTest/Adder:1.0","SuperVisionTest/Adder");
  typeMap["AdditionComponent/Adder"]=TypeCode::interfaceTc("IDL:AdditionComponent/Adder:1.0","AdditionComponent/Adder");
  typeMap["Syr"]=TypeCode::interfaceTc("IDL:SuperVisionTest/Syr:1.0","Syr");
  typeMap["ListOfSyr"]=TypeCode::interfaceTc("IDL:SuperVisionTest/ListOfSyr:1.0","ListOfSyr");
  */
}

void SessionCataLoader::loadCata(Catalog* cata)
{
  DEBTRACE("SessionCataLoader::load")
#ifndef NEW_KERNEL
  datastreamMap[SALOME_ModuleCatalog::DATASTREAM_UNKNOWN]="CALCIUM_unknown";
  datastreamMap[SALOME_ModuleCatalog::DATASTREAM_INTEGER]="CALCIUM_integer";
  datastreamMap[SALOME_ModuleCatalog::DATASTREAM_FLOAT]="CALCIUM_real";
  datastreamMap[SALOME_ModuleCatalog::DATASTREAM_DOUBLE]="CALCIUM_double";
  datastreamMap[SALOME_ModuleCatalog::DATASTREAM_BOOLEAN]="CALCIUM_boolean";
  datastreamMap[SALOME_ModuleCatalog::DATASTREAM_STRING]="CALCIUM_string";
#endif

  CORBA::ORB_ptr orb = getSALOMERuntime()->getOrb();
  CORBA::Object_var obj;
  SALOME_ModuleCatalog::ModuleCatalog_var catalog;
  obj=orb->string_to_object(_path.c_str());
  catalog= SALOME_ModuleCatalog::ModuleCatalog::_narrow(obj);
  _errors="";

  // Get types in catalog
  loadTypes(cata,catalog);
  // Get old types 
  loadTypesOld(cata);

  // Get component list
  SALOME_ModuleCatalog::ListOfComponents_var componentname_list;
  componentname_list  = catalog->GetComponentList();
  for (int i=0; i<componentname_list->length(); i++)
    {
      DEBTRACE ("Component : " << componentname_list[i]);
      SALOME_ModuleCatalog::Acomponent_var component = catalog->GetComponent(componentname_list[i]);
      importComponent(cata,component);
    }
  cata->setErrors(_errors);
}
