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

#ifndef __CATALOG_HXX__
#define __CATALOG_HXX__

#include "YACSlibEngineExport.hxx"
#include "RefCounter.hxx"

#include <string>
#include <map>

namespace YACS
{
  namespace ENGINE
  {
    class TypeCode;
    class ComponentDefinition;
    class Node;
    class ComposedNode;
/*! \brief class for YACS catalogs.
 *
 *
 */
    class YACSLIBENGINE_EXPORT Catalog : public RefCounter
    {
    public:
      Catalog(const std::string& name);
      virtual ~Catalog();
      const std::string& getName() const { return _name; }
      std::string getErrors(){return _errors;}
      void setErrors(const std::string& errors){_errors=errors;}
      std::map<std::string,TypeCode*> _typeMap;
      std::map<std::string,ComponentDefinition*> _componentMap;
      std::map<std::string,Node*> _nodeMap;
      std::map<std::string,ComposedNode*> _composednodeMap;
      Node* getNodeFromNodeMap(std::string name) {return _nodeMap[name];}
    protected:
      std::string _name;
      std::string _errors;
    };

/*! \brief class for YACS catalog loader.
 *
 */
    class YACSLIBENGINE_EXPORT CatalogLoader
    {
    public:
      CatalogLoader();
      CatalogLoader(const std::string& path);
      virtual ~CatalogLoader();
      virtual CatalogLoader* newLoader(const std::string& path)=0 ;
      virtual void loadCata(Catalog* cata)=0 ;
      virtual void load(Catalog* cata,const std::string& path) ;
    protected:
      std::string _path;
    };
  }
}

#endif
