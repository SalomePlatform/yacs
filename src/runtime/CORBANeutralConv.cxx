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
#include "CORBANeutralConv.hxx"

#include <iostream>

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

CorbaNeutral::CorbaNeutral(InputPort* p)
  : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode())
{
}

//!Convert a CORBA::Any that is convertible to a YACS::ENGINE::Any * and send it to proxy port
/*!
 *   \param data : CORBA::Any object as a void * pointer
 */

void CorbaNeutral::put(const void *data)
{
  put((CORBA::Any *)data);
}

//!Convert a CORBA::Any that is convertible to a YACS::ENGINE::Any * and send it to proxy port
/*!
 *   \param data : CORBA::Any object
 */
void CorbaNeutral::put(CORBA::Any *data)
{
  DEBTRACE( "CorbaNeutral::put" )
  YACS::ENGINE::Any *ob;
  ob=convertCorbaNeutral(edGetType(),data);
  DEBTRACE("before put refcnt: " << ob->getRefCnt())
  DEBTRACE( _port->getName() )
  _port->put(ob);
  // ob has been created in convertCorbaNeutral. _port has normally called incRef 
  ob->decrRef();
  DEBTRACE("after put refcnt: " << ob->getRefCnt())
}
