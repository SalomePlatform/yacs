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
#include "CppCORBAConv.hxx"
#include "RuntimeSALOME.hxx"

#include <iostream>
#include <sstream>

using namespace std;

namespace YACS
{
  namespace ENGINE
  {
    CORBA::Any *convertCppCorba(const TypeCode *t,Any *data)
    {
      return convertNeutralCorba(t, data);
    }

    void CppCorba::put(const void *data)  throw(ConversionException)
    {
      put((Any *)data);
    }

    //!Convert a C++ object into CORBA::Any object
    /*!
     *   \param data : python object
     */

    void CppCorba::put(Any *data)  throw(ConversionException)
    {
      CORBA::Any* a = convertCppCorba(edGetType(), data);
      _port->put(a);
      //delete Any that has been allocated by convertCppCorba
      delete a;
    }

    int isAdaptableCorbaCpp(const TypeCode *t1, const TypeCode *t2)
    {
      return isAdaptableCorbaNeutral(t1, t2);
    }
  }
}
