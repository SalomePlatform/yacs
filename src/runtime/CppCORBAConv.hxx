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

#ifndef __CPPCORBACONV_HXX__
#define __CPPCORBACONV_HXX__

#include "CORBAPorts.hxx"

namespace YACS
{
  namespace ENGINE
  {

    // --- adaptator ports C++ -> Corba for several types

    class CppCorba : public ProxyPort
    {
    public:
      CppCorba(InputCorbaPort* p)
        : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode()) {}
      virtual void put(const void *data) throw(ConversionException);
      void put(Any *data) throw(ConversionException);
    };
    int isAdaptableCorbaCpp(const TypeCode *t1, const TypeCode *t2);
  }
}
#endif
