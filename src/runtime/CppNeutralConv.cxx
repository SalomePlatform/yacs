// Copyright (C) 2006-2024  CEA, EDF
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
#include "CppNeutralConv.hxx"

#include <iostream>
#include <sstream>
#include "Any.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace std;

namespace YACS
{
  namespace ENGINE 
  {
    Any* convertCppNeutral(const TypeCode *t, Any *a)
    {
      return convertNeutralNeutral(t, a);
    }

    CppNeutral::CppNeutral(InputPort* p)
      : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode())
    {
    }

    void CppNeutral::put(const void *data)
    {
      DEBTRACE(" CppNeutral::put(const void *data)");
      put((Any *)data);
    }

    //! Send received C++/Neutral (Any *) value to proxy port
    /*!
     *   \param data : Neutral (Any *) value
     */

    void CppNeutral::put(Any *data)
    {
      DEBTRACE("CppNeutral::put " << data);
      Any *ob;
      ob=convertCppNeutral(edGetType(), data);
      DEBTRACE("before put refcnt: " << ob->getRefCnt());
      _port->put(ob);
      DEBTRACE( "after put refcnt: " << ob->getRefCnt() );
      //_port has called incRef
      ob->decrRef();
    }

    int isAdaptableNeutralCpp(const TypeCode *t1, const TypeCode *t2)
    {
      return isAdaptableNeutralNeutral(t1, t2);
    }
  }
}
