#include "Catalog.hxx"
#include "ComponentDefinition.hxx"
#include "TypeCode.hxx"
#include "Node.hxx"
#include "ComposedNode.hxx"

#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;
using namespace YACS::ENGINE;

Catalog::Catalog(const std::string& name):_name(name)
{
}

Catalog::~Catalog()
{
  DEBTRACE("Catalog::~Catalog");
  //decref type codes in map
  std::map<std::string, TypeCode *>::iterator pt;
  for(pt=_typeMap.begin();pt!=_typeMap.end();pt++)
    ((*pt).second)->decrRef();

  //get rid of component definitions in  map
  std::map<std::string, ComponentDefinition*>::const_iterator lt;
  for(lt=_componentMap.begin();lt!=_componentMap.end();lt++)
    delete (*lt).second;

  //get rid of nodes in  map
  std::map<std::string, Node*>::const_iterator nt;
  for(nt=_nodeMap.begin();nt!=_nodeMap.end();nt++)
    delete (*nt).second;

  //get rid of composed nodes in  map
  std::map<std::string, ComposedNode*>::const_iterator ct;
  for(ct=_composednodeMap.begin();ct!=_composednodeMap.end();ct++)
    delete (*ct).second;

}

CatalogLoader::CatalogLoader():_path("proto")
{
}

CatalogLoader::CatalogLoader(const std::string& path):_path(path)
{
}

CatalogLoader::~CatalogLoader()
{
  DEBTRACE("CatalogLoader::~CatalogLoader");
}

void CatalogLoader::load(Catalog* cata,const std::string& path)
{
  CatalogLoader* CL=newLoader(path);
  CL->loadCata(cata);
  delete CL;
}
