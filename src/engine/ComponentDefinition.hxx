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

#ifndef __COMPONENTDEFINITION_HXX__
#define __COMPONENTDEFINITION_HXX__

#include "YACSlibEngineExport.hxx"

#include <map>
#include <string>

namespace YACS
{
  namespace ENGINE
  {
    class ServiceNode;

/*! \brief Base class for component definition.
 *
 *
 */
    class YACSLIBENGINE_EXPORT ComponentDefinition 
    {
    public:
      ComponentDefinition(const std::string& name);
      const std::string& getName() const { return _name; }
      virtual ~ComponentDefinition();
      std::map<std::string,ServiceNode*> _serviceMap;
    protected:
      std::string _name;
    };
  }
}

#endif
