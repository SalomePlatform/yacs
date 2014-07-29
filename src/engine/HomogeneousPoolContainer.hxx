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

#ifndef __HOMOGENEOUSPOOLCONTAINER_HXX__
#define __HOMOGENEOUSPOOLCONTAINER_HXX__

#include "YACSlibEngineExport.hxx"
#include "Exception.hxx"
#include "Container.hxx"

#include <list>
#include <map>

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
    protected:
      HomogeneousPoolContainer();
#ifndef SWIG
      virtual ~HomogeneousPoolContainer();
#endif
    };
  }
}

#endif
