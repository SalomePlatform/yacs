// Copyright (C) 2006-2024  CEA, EDF
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

#ifndef _PARSERS_HXX_
#define _PARSERS_HXX_

#include "YACSloaderExport.hxx"

#include <map>
#include <parserBase.hxx>

namespace YACS
{
  namespace ENGINE
  {
    class Proc;
  }

  class YACSLOADER_EXPORT YACSLoader
  {
    public:
        YACSLoader();
        virtual ~YACSLoader();
        virtual YACS::ENGINE::Proc* load(const char * filename);
        void registerProcCataLoader();

    protected:
        std::map<std::string,parser*> _defaultParsersMap;
  };
}

#endif
