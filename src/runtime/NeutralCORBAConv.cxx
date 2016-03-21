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
#include "RuntimeSALOME.hxx"
#include "NeutralCORBAConv.hxx"

#include <iostream>

using namespace YACS::ENGINE;
using namespace std;


void NeutralCorbaInt::put(const void *data)  throw(ConversionException)
{
  put((YACS::ENGINE::Any *)data);
}

//!Convert a YACS::ENGINE::Any (integer) to CORBA::Any (integer)
/*!
 *   \param data : YACS::ENGINE::Any object
 */

void NeutralCorbaInt::put(YACS::ENGINE::Any *data)  throw(ConversionException)
{
  CORBA::Any *a =convertNeutralCorba(edGetType(),data);
  _port->put(a);
  //delete Any that has been allocated by convertNeutralCorba
  delete a;
}

void NeutralCorbaBool::put(const void *data)  throw(ConversionException)
{
  put((YACS::ENGINE::Any *)data);
}

//!Convert a YACS::ENGINE::Any (boolean) to CORBA::Any (boolean)
/*!
 *   \param data : YACS::ENGINE::Any object
 */

void NeutralCorbaBool::put(YACS::ENGINE::Any *data)  throw(ConversionException)
{
  CORBA::Any *a =convertNeutralCorba(edGetType(),data);
  _port->put(a);
  //delete Any that has been allocated by convertNeutralCorba
  delete a;
}


void NeutralCorbaDouble::put(const void *data)  throw(ConversionException)
{
  put((YACS::ENGINE::Any *)data);
}

//!Convert a YACS::ENGINE::Any (double) to CORBA::Any (double)
/*!
 *   \param data : YACS::ENGINE::Any object
 */

void NeutralCorbaDouble::put(YACS::ENGINE::Any *data)  throw(ConversionException)
{
  CORBA::Any *a =convertNeutralCorba(edGetType(),data);
  _port->put(a);
  //delete Any that has been allocated by convertNeutralCorba
  delete a;
}

void NeutralCorbaSequence::put(const void *data) throw(ConversionException)
{
  put((YACS::ENGINE::Any *)data);
}

//!Convert a Neutral::Any sequence to CORBA::Any* Sequence
/*!
 *   \param data : Neutral::Any object
 */
void NeutralCorbaSequence::put(YACS::ENGINE::Any *data) throw(ConversionException)
{
  CORBA::Any *a =convertNeutralCorba(edGetType(),data);
  _port->put(a);
  //delete Any that has been allocated by convertNeutralCorba
  delete a;
}

NeutralCorbaString::NeutralCorbaString(InputCorbaPort* p):ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode())
{
}

void NeutralCorbaString::put(const void *data) throw(ConversionException)
{
  put((YACS::ENGINE::Any *)data);
}

//!Convert a Neutral::Any string to CORBA::Any* string
/*!
 *   \param data : Neutral::Any object
 */
void NeutralCorbaString::put(YACS::ENGINE::Any *data) throw(ConversionException)
{
  CORBA::Any *a =convertNeutralCorba(edGetType(),data);
  _port->put(a);
  //delete Any that has been allocated by convertNeutralCorba
  delete a;
}

NeutralCorbaObjref::NeutralCorbaObjref(InputCorbaPort* p):ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode())
{
}

void NeutralCorbaObjref::put(const void *data) throw(ConversionException)
{
  put((YACS::ENGINE::Any *)data);
}

//!Convert a Neutral::Any Objref to CORBA::Any* Objref
/*!
 *   \param data : Neutral::Any object
 */
void NeutralCorbaObjref::put(YACS::ENGINE::Any *data) throw(ConversionException)
{
  CORBA::Any *a =convertNeutralCorba(edGetType(),data);
  _port->put(a);
  //delete Any that has been allocated by convertNeutralCorba
  delete a;
}


NeutralCorbaStruct::NeutralCorbaStruct(InputCorbaPort* p)
  : ProxyPort(p), DataPort(p->getName(), p->getNode(), p->edGetType()), Port(p->getNode())
{
}

void NeutralCorbaStruct::put(const void *data) throw(ConversionException)
{
  put((YACS::ENGINE::Any *)data);
}

//!Convert a Neutral::Any struct to CORBA::Any* struct
/*!
 *   \param data : Neutral::Any object
 */
void NeutralCorbaStruct::put(YACS::ENGINE::Any *data) throw(ConversionException)
{
  CORBA::Any *a =convertNeutralCorba(edGetType(),data);
  _port->put(a);
  //delete Any that has been allocated by convertNeutralCorba
  delete a;
}
