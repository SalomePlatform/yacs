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
    float neededCores = 0.0; // needed by WorkloadManager
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
    unsigned int nbCores; // needed by WorkloadManager
    // parameters for client use, used by WorkloadManager to distinguish objects
    std::string name;
    int id;
    bool operator==(const Resource& other)const
    { return id == other.id && name == other.name;}
    bool operator<(const Resource& other)const
    {
      return (id < other.id) ||
             (id == other.id && name < other.name);
    }
  };
  
  struct Container
  {
    ContainerType type;
    Resource resource;
    unsigned int index; // worker index on the resource for this type
  };

  /**
  * @todo write docs
  */
  class Task
  {
  public:
    virtual ~Task(){};
    virtual const ContainerType& type()const =0;
    virtual void run(const Container& c)=0;
  };
}

#endif // _TASK_H_
