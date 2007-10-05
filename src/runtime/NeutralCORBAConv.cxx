
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
  CORBA::Long l= data->getIntValue();
  CORBA::Any a;
  a <<= l;
  _port->put(&a);
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
  CORBA::Double d = data->getDoubleValue();
  CORBA::Any a;
  a <<= d;
  _port->put(&a);
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
  string val=data->getStringValue();
  CORBA::Any a;
  a <<= val.c_str();
  _port->put(&a);
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
