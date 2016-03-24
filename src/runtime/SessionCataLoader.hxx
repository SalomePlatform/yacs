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
    protected:
       std::string _errors;
    };
  }
}

#endif
