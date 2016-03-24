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
