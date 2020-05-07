// Copyright (C) 2020  CEA/DEN, EDF R&D
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
#ifndef _TASK_H_
#define _TASK_H_

#include <string>

namespace WorkloadManager
{
  struct ContainerType
  {
    // parameters needed by WorkloadManager
    float neededCores = 0.0;
    bool ignoreResources = false; // if true, the task can be run as soon as
                                  // added to the manager without any resource
                                  // allocation
    // parameters for client use, used by WorkloadManager to distinguish objects
    std::string name;
    int id = 0;
    bool operator==(const ContainerType& other)const
    { return id == other.id && name == other.name;}
    bool operator<(const ContainerType& other)const
    {
      return (id < other.id) ||
             (id == other.id && name < other.name);
    }
  };
  
  struct Resource
  {
    unsigned int nbCores = 0; // needed by WorkloadManager
    // parameters for client use, used by WorkloadManager to distinguish objects
    std::string name;
    int id = 0;
    bool operator==(const Resource& other)const
    { return id == other.id && name == other.name;}
    bool operator<(const Resource& other)const
    {
      return (id < other.id) ||
             (id == other.id && name < other.name);
    }
  };
  
  struct RunInfo
  {
    ContainerType type;
    Resource resource;
    unsigned int index=0; // worker index on the resource for this type
  };

  /**
  * @todo write docs
  */
  class Task
  {
  public:
    virtual ~Task(){};
    virtual const ContainerType& type()const =0;
    virtual void run(const RunInfo& c)=0;

    // Is it possible to run the task on this resource?
    virtual bool isAccepted(const Resource& r)
    {
      // by default, a task can be run on any resource.
      return true;
    }
  };
}

#endif // _TASK_H_
