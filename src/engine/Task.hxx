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

#ifndef __TASK_HXX__
#define __TASK_HXX__

#include "YACSlibEngineExport.hxx"
#include "define.hxx"
#include <set>

namespace YACS
{
  namespace ENGINE
  {
    class ComponentInstance;
    class Container;

    class YACSLIBENGINE_EXPORT Task
    {
    public:
      virtual void begin() = 0;
      virtual bool isReady() = 0;
      virtual void execute() = 0;
      virtual void load() = 0;
      virtual void loaded() = 0;
      virtual void connected() = 0;
      virtual void initService() = 0;
      virtual void connectService() = 0;
      virtual void disconnectService() = 0;
      virtual void getCoupledTasks(std::set<Task*>& coupledSet) = 0;
      virtual bool isDeployable() const = 0;
      virtual ComponentInstance *getComponent() = 0;
      virtual const ComponentInstance *getComponent() const = 0;
      virtual Container *getContainer() = 0;
      virtual YACS::StatesForNode getState() const = 0;
      virtual void finished() = 0;
      virtual void aborted() = 0;
      virtual ~Task();
    };
  }
}

#endif
