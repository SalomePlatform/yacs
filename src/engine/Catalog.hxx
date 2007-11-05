#ifndef __CATALOG_HXX__
#define __CATALOG_HXX__

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
    class Catalog
    {
    public:
      Catalog(const std::string& name);
      virtual ~Catalog();
      const std::string& getName() const { return _name; }
      std::map<std::string,TypeCode*> _typeMap;
      std::map<std::string,ComponentDefinition*> _componentMap;
      std::map<std::string,Node*> _nodeMap;
      std::map<std::string,ComposedNode*> _composednodeMap;
    protected:
      std::string _name;
    };

/*! \brief class for YACS catalog loader.
 *
 */
    class CatalogLoader
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
