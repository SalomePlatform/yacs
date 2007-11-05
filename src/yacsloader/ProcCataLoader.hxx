#ifndef _PROCCATALOADER_HXX_
#define _PROCCATALOADER_HXX_

#include "Catalog.hxx"

#include <string>
#include <iostream>
#include <list>

namespace YACS
{
  class YACSLoader;

  namespace ENGINE
  {
    class ProcCataLoader: public CatalogLoader
    {
    public:
       ProcCataLoader(YACS::YACSLoader* xmlLoader):CatalogLoader(),_xmlLoader(xmlLoader){};
       ProcCataLoader(YACS::YACSLoader* xmlLoader,const std::string& path);
       virtual ~ProcCataLoader();
       virtual void loadCata(Catalog* cata);
       virtual CatalogLoader* newLoader(const std::string& path) {return new ProcCataLoader(_xmlLoader,path);}
    protected:
       YACS::YACSLoader* _xmlLoader;
    };
  }
}

#endif
