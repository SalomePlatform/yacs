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
#include "CORBACppConv.hxx"

#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

namespace YACS
{
  namespace ENGINE
  {
    Any * convertCorbaCpp(const TypeCode *t, CORBA::Any *A)
    {
      return convertCorbaNeutral(t, A);
    }

    CorbaCpp::CorbaCpp(InputPort* p)
      : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode())
    {
    }

    //!Convert a CORBA::Any that is convertible to a YACS::ENGINE::Any * and send it to proxy port
    /*!
     *   \param data : CORBA::Any object as a void * pointer
     */

    void CorbaCpp::put(const void *data) throw(ConversionException)
    {
      put((CORBA::Any *)data);
    }

    //!Convert a CORBA::Any that is convertible to a YACS::ENGINE::Any * and send it to proxy port
    /*!
     *   \param data : CORBA::Any object
     */
    void CorbaCpp::put(CORBA::Any *data) throw(ConversionException)
    {
      DEBTRACE("CorbaCpp::put");
      YACS::ENGINE::Any *ob;
      ob= convertCorbaCpp(edGetType(),data);
      DEBTRACE("refcnt: " << ob->getRefCnt());
      DEBTRACE(_port->getName());
      _port->put(ob);
      // ob has been created in convertCorbaNeutral. _port has normally called incrRef
      ob->decrRef();
      DEBTRACE("after put refcnt: " << ob->getRefCnt())
    }

    int isAdaptableCppCorba(const TypeCode *t1,const TypeCode *t2)
    {
      return isAdaptableNeutralCorba(t1, t2);
    }
  }
}

