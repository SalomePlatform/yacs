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

#ifndef __SALOMECONTAINERTMPFORHP_HXX__
#define __SALOMECONTAINERTMPFORHP_HXX__

#include "YACSRuntimeSALOMEExport.hxx"
#include "SalomeContainer.hxx"

#include <sstream>

namespace YACS
{
  namespace ENGINE
  {
    class SalomeHPContainer;
    class SalomeContainerTmpForHP : public SalomeContainer
    {
    public:
      SalomeContainerTmpForHP(const Container& other, const SalomeContainerTools& sct, const SalomeContainerHelper *lmt,
                              const std::vector<std::string>& componentNames, int shutdownLev,
                              const SalomeHPContainer *zeOriginCont, std::size_t pos):SalomeContainer(other,sct,lmt,componentNames,shutdownLev),_zeOriginCont(zeOriginCont),_pos(pos) { }
      std::string getDiscreminantStrOfThis(const Task *askingNode) const { std::ostringstream oss; oss << _zeOriginCont << "_" << _pos; return oss.str(); }
      CORBA::Object_ptr loadComponent(Task *inst);
      static SalomeContainerTmpForHP *BuildFrom(const SalomeHPContainer *cont, const Task *askingNode);
    private:
      const SalomeHPContainer *_zeOriginCont;
      std::size_t _pos;
    };
  }
}

#endif
