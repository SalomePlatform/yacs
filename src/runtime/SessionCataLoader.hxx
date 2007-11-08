#ifndef _SESSIONCATALOADER_HXX_
#define _SESSIONCATALOADER_HXX_

#include "Catalog.hxx"
#include "SALOME_ModuleCatalog.hh"

#include <string>
#include <iostream>
#include <list>

namespace YACS
{
  namespace ENGINE
  {
    class SessionCataLoader: public CatalogLoader
    {
    public:
       SessionCataLoader():CatalogLoader(){};
       SessionCataLoader(const std::string& path);
       virtual ~SessionCataLoader();
       virtual void loadCata(Catalog* cata);
       virtual void loadTypesOld(Catalog* cata);
       virtual void loadTypes(Catalog* cata,SALOME_ModuleCatalog::ModuleCatalog_ptr catalog);
       virtual CatalogLoader* newLoader(const std::string& path) {return new SessionCataLoader(path);}
       virtual void importComponent(Catalog* cata,SALOME_ModuleCatalog::Acomponent_ptr compo);
    };
  }
}

#endif
