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

#ifndef __SALOMECONTAINERHELPER_HXX__
#define __SALOMECONTAINERHELPER_HXX__

#include "YACSRuntimeSALOMEExport.hxx"

#include "SALOMEconfig.h"
#include CORBA_CLIENT_HEADER(SALOME_Component)

#include <map>
#include <string>

namespace YACS
{
  namespace ENGINE
  {
    class ComponentInstance;

    class SalomeContainerHelper
    {
    public:
      virtual std::string getType() const = 0;
      virtual SalomeContainerHelper *deepCpyOnlyStaticInfo() const = 0;
      virtual Engines::Container_var getContainer(const ComponentInstance *inst) const = 0;
      virtual bool isAlreadyStarted(const ComponentInstance *inst) const = 0;
      virtual void setContainer(const ComponentInstance *inst, Engines::Container_var cont) = 0;
      virtual void shutdown() = 0;
      virtual ~SalomeContainerHelper();
    };

    class SalomeContainerMonoHelper : public SalomeContainerHelper
    {
    public:
      SalomeContainerMonoHelper();
      std::string getType() const;
      SalomeContainerMonoHelper *deepCpyOnlyStaticInfo() const;
      Engines::Container_var getContainer(const ComponentInstance *inst) const;
      bool isAlreadyStarted(const ComponentInstance *inst) const;
      void setContainer(const ComponentInstance *inst, Engines::Container_var cont);
      void shutdown();
      ~SalomeContainerMonoHelper();
    public:
      static const char TYPE_NAME[];
      static const char DFT_LAUNCH_MODE[];
    private:
      Engines::Container_var _trueCont;
    };

    class SalomeContainerMultiHelper : public SalomeContainerHelper
    {
    public:
      std::string getType() const;
      SalomeContainerMultiHelper *deepCpyOnlyStaticInfo() const;
      Engines::Container_var getContainer(const ComponentInstance *inst) const;
      bool isAlreadyStarted(const ComponentInstance *inst) const;
      void setContainer(const ComponentInstance *inst, Engines::Container_var cont);
      void shutdown();
      ~SalomeContainerMultiHelper();
    public:
      static const char TYPE_NAME[];
      static const char DFT_LAUNCH_MODE[];
    private:
      std::map<const ComponentInstance *,Engines::Container_var> _trueContainers;
    };
  }
}

#endif
