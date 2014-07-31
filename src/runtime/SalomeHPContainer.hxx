// Copyright (C) 2006-2014  CEA/DEN, EDF R&D
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

#ifndef __SALOMEHPCONTAINER_HXX__
#define __SALOMEHPCONTAINER_HXX__

#include "YACSRuntimeSALOMEExport.hxx"
#include "HomogeneousPoolContainer.hxx"
#include "SalomeContainerHelper.hxx"
#include "SalomeContainerTools.hxx"
#include "Mutex.hxx"
#include <string>
#include <vector>
#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(SALOME_Component)
#include CORBA_CLIENT_HEADER(SALOME_ContainerManager)

namespace YACS
{
  namespace ENGINE
  {
    class SalomeComponent;

    class YACSRUNTIMESALOME_EXPORT SalomeHPContainer : public HomogeneousPoolContainer
    {
    public:
      SalomeHPContainer();
      SalomeHPContainer(const SalomeHPContainer& other);
      void setSizeOfPool(int sz);
    protected:
#ifndef SWIG
      ~SalomeHPContainer();
#endif
    protected:
      YACS::BASES::Mutex _mutex;
      std::vector<std::string> _componentNames;
      std::vector<SalomeContainerMonoHelper> _launchModeType;
      int _shutdownLevel;
      SalomeContainerTools _sct;
    };
  }
}

#endif
