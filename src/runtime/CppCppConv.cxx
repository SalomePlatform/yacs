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

#include "TypeConversions.hxx"
#include "CppCppConv.hxx"
#include "CppPorts.hxx"

#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

namespace YACS
{
  namespace ENGINE
  {
    Any * convertCppCpp(const TypeCode *t, Any *data)
    {
      return convertNeutralNeutral(t, data);
    }

    CppCpp::CppCpp(InputCppPort* p)
      : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode())
    {
    }

    //!Convert a Any to a Any 
    /*!
     *   transition method from const void* to Any*
     *   \param data : const void * data
     */
    void CppCpp::put(const void *data) throw(ConversionException)
    {
      put((Any *)data);
    }

    //!Convert a Any to a Any 
    /*!
     *   \param data : Any object
     */
    void CppCpp::put(Any *data) throw(ConversionException)
    {
      Any *a = convertCppCpp(edGetType(),data);
      DEBTRACE( "before put refcnt: " << a->getRefCnt() );
      _port->put(a);
      DEBTRACE( "after put refcnt: " << a->getRefCnt() );
      //_port has called incRef
      a->decrRef();
    }

    int isAdaptableCppCpp(const TypeCode *t1, const TypeCode *t2)
    {
      return isAdaptableNeutralNeutral(t1, t2);
    }
  }
}
