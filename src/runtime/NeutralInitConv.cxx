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
#include "NeutralInitConv.hxx"
#include "TypeCode.hxx"

#include <iostream>
#include <sstream>
#include "Any.hxx"

//#define _DEVDEBUG_
#include "YacsTrace.hxx"

using namespace YACS::ENGINE;
using namespace std;

NeutralInit::NeutralInit(InputPort* p)
  : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode())
{
}

void NeutralInit::put(const void *data) throw(ConversionException)
{
  DEBTRACE(" NeutralInit::put(const void *data)");
  put((YACS::ENGINE::Any *)data);
}

//!Initialize Neutral proxy port with received Any object after type check
 /*!
  *   \param data : YACS::ENGINE::Any object
  */

void NeutralInit::put(YACS::ENGINE::Any *data) throw(ConversionException)
{
  DEBTRACE("NeutralInit::put " << data);
  if(!edGetType()->isAdaptable(data->getType()))
    throw ConversionException("Value type is not compatible with port type");
  _port->put((const void *)data);
}
