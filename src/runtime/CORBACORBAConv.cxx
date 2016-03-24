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
#include "CORBACORBAConv.hxx"
#include "CORBAPorts.hxx"

using namespace YACS::ENGINE;
using namespace std;

CorbaCorba::CorbaCorba(InputCorbaPort* p)
  : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode())
{
}

//!Convert a CORBA::Any to a CORBA::Any 
/*!
 *   transition method from const void* to CORBA::Any*
 *   \param data : const void * data
 */
void CorbaCorba::put(const void *data) throw(ConversionException)
{
  put((CORBA::Any *)data);
}

//!Convert a CORBA::Any to a CORBA::Any 
/*!
 *   \param data : CORBA::Any object
 */
void CorbaCorba::put(CORBA::Any *data) throw(ConversionException)
{
  CORBA::Any *a = convertCorbaCorba(edGetType(),data);
  _port->put(a);
}
