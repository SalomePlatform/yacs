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

#ifndef __HOMOGENEOUSPOOLCONTAINER_HXX__
#define __HOMOGENEOUSPOOLCONTAINER_HXX__

#include "YACSlibEngineExport.hxx"
#include "Exception.hxx"
#include "Container.hxx"

#include <vector>

namespace YACS
{
  namespace ENGINE
  {
    /*!
     * This is an abstract class, that represents a set of fixed number of worker "kernelcontainers" homegenous in the sense that can be used indifferently each other.
     * But each of those worker pool can be used once at a time.
     */
    class YACSLIBENGINE_EXPORT HomogeneousPoolContainer : public Container
    {
    public:
      void attachOnCloning() const;
      void dettachOnCloning() const;
      bool isAttachedOnCloning() const;
      void setAttachOnCloningStatus(bool val) const;
      //
      virtual void setSizeOfPool(int sz) = 0;
      virtual int getSizeOfPool() const = 0;
      virtual std::size_t getNumberOfFreePlace() const = 0;
      virtual void allocateFor(const std::vector<const Task *>& nodes) = 0;
      virtual void release(const Task *node) = 0;
      static const char SIZE_OF_POOL_KEY[];
      static const char INITIALIZE_SCRIPT_KEY[];
    protected:
      HomogeneousPoolContainer();
#ifndef SWIG
      virtual ~HomogeneousPoolContainer();
#endif
    };
  }
}

#endif
