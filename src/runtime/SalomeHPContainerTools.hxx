// Copyright (C) 2006-2020  CEA/DEN, EDF R&D
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

#ifndef __SALOMEHPCONTAINERTOOLS_HXX__
#define __SALOMEHPCONTAINERTOOLS_HXX__

#include "YACSRuntimeSALOMEExport.hxx"
#include "SalomeContainerHelper.hxx"

#include "RefCounter.hxx"

#include "Mutex.hxx"
#include "AutoRefCnt.hxx"

#include <map>
#include <vector>

namespace YACS
{
  namespace ENGINE
  {
    class Task;
    class SalomeComponent;
    class SalomeHPContainer;
    class SalomeContainerMonoHelper;
    class SalomeHPContainerVectOfHelper : public RefCounter
    {
    public:
      std::size_t size() const { return _launchModeType.size(); }
      void resize(std::size_t sz);
      std::size_t getNumberOfFreePlace() const;
      std::size_t getNumberOfFreePlaceAmong(const std::vector<std::size_t>& idsOfKernelContainers) const;
      void allocateFor(const std::vector<const Task *>& nodes);
      void allocateForAmong(const std::vector<std::size_t>& idsOfKernelContainers, const std::vector<const Task *>& nodes);
      void release(const Task *node);
      std::size_t locateTask(const Task *node) const;
      const SalomeContainerMonoHelper *at(std::size_t pos) const { checkPosInVec(pos); return _launchModeType[pos]; }
      SalomeContainerMonoHelper *at(std::size_t pos) { checkPosInVec(pos); return _launchModeType[pos]; }
      const SalomeContainerMonoHelper *getHelperOfTaskThreadSafe(const Task *node) const;
      const SalomeContainerMonoHelper *getHelperOfTask(const Task *node) const;
      SalomeContainerMonoHelper *getHelperOfTaskThreadSafe(const Task *node);
      SalomeContainerMonoHelper *getHelperOfTask(const Task *node);
      void shutdown();
      //! For thread safety for concurrent load operation on same Container.
      void lock();
      //! For thread safety for concurrent load operation on same Container.
      void unLock();
    public:
      std::vector<std::string> getKernelContainerNames() const;
    private:
      ~SalomeHPContainerVectOfHelper() { }
      void checkNoCurrentWork() const;
      void checkPosInVec(std::size_t pos) const;
    private:
      YACS::BASES::Mutex _mutex;
      std::vector<bool> _whichOccupied;
      std::vector< BASES::AutoRefCnt<YACS::ENGINE::SalomeContainerMonoHelper> > _launchModeType;
      std::map<const Task *,std::size_t > _currentlyWorking;
    };
  }
}

#endif
