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

#ifndef __DEPLOYMENTTREE_HXX__
#define __DEPLOYMENTTREE_HXX__

#include "YACSlibEngineExport.hxx"

#include <vector>

namespace YACS
{
  namespace ENGINE
  {
    class Task;
    class Container;
    class Scheduler;
    class ComponentInstance;

    class YACSLIBENGINE_EXPORT DeploymentTreeOnHeap
    {
    public:
      DeploymentTreeOnHeap();
      ~DeploymentTreeOnHeap();
      bool decrRef();
      void incrRef() const;
      //
      unsigned char appendTask(Task *task, Scheduler *cloner);
      //
      unsigned getNumberOfCTDefContainer() const;
      unsigned getNumberOfRTODefContainer() const;
      unsigned getNumberOfCTDefComponentInstances() const;
      unsigned getNumberOfRTODefComponentInstances() const;
      //
      std::vector<Container *> getAllContainers() const;
      std::vector<Container *> getAllCTDefContainers() const;
      std::vector<Container *> getAllRTODefContainers() const;
      std::vector<Task *> getTasksLinkedToComponent(ComponentInstance *comp) const;
      std::vector<Task *> getTasksLinkedToContainer(Container *cont) const;
      std::vector<ComponentInstance *> getComponentsLinkedToContainer(Container *cont) const;
      //
      bool presenceOfDefaultContainer() const;
      std::vector<Task *> getFreeDeployableTasks() const;
    private:
      mutable int _cnt;
      std::vector< std::pair<Task *,Scheduler *> > _freePlacableTasks;
      //! internal representation of tree. Scheduler is the duplicating Task, \b if it exists, on runtime unpredictable times on compil-time
      std::vector< std::vector< std::vector< std::pair<Task *, Scheduler * > > > > _tree;
    };

    class YACSLIBENGINE_EXPORT DeploymentTree
    {
    public:
      DeploymentTree();
      ~DeploymentTree();
      DeploymentTree(const DeploymentTree& other);
      const DeploymentTree &operator=(const DeploymentTree& other);
      unsigned char appendTask(Task *task, Scheduler *cloner);
      //
      unsigned getNumberOfCTDefContainer() const;
      unsigned getNumberOfRTODefContainer() const;
      unsigned getNumberOfCTDefComponentInstances() const;
      unsigned getNumberOfRTODefComponentInstances() const;
      //
      bool presenceOfDefaultContainer() const;
      std::vector<Container *> getAllContainers() const;
      std::vector<Container *> getAllCTDefContainers() const;
      std::vector<Container *> getAllRTODefContainers() const;
      std::vector<Task *> getTasksLinkedToComponent(ComponentInstance *comp) const;
      std::vector<Task *> getTasksLinkedToContainer(Container *cont) const;
      std::vector<ComponentInstance *> getComponentsLinkedToContainer(Container *cont) const;
      //
      bool isNull() const;
      std::vector<Task *> getFreeDeployableTasks() const;
    public:
      //possible return of appendTask method.
      static const unsigned char NULL_TASK = 3;
      static const unsigned char APPEND_OK = 0;
      static const unsigned char NULL_TREE = 199;
      static const unsigned char ALREADY_IN_TREE = 1;
      static const unsigned char NOT_DEPLOYABLE_TASK = 2;
      static const unsigned char DEPLOYABLE_BUT_NOT_SPECIFIED = 5;
      static const unsigned char DUP_TASK_NOT_COMPATIBLE_WITH_EXISTING_TREE = 4;
    private:
      DeploymentTreeOnHeap *_treeHandle;
    };
  }
}

#endif
