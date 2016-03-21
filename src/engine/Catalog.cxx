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

#include "Catalog.hxx"
#include "ComponentDefinition.hxx"
#include "TypeCode.hxx"
#include "Node.hxx"
#include "ComposedNode.hxx"

//#define _DEVDEBUG_
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
