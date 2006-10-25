
#include "CORBACORBAConv.hxx"
#include "TypeConversions.hxx"

using namespace YACS::ENGINE;
using namespace std;

CorbaCorba::CorbaCorba(InputCorbaPort* p)
  : ProxyPort(p), Port(p->getNode())
{
}

//!Convertit un Any convertible en CORBA::Any 
/*!
 *   \param data : CORBA::Any object
 */

void CorbaCorba::put(const void *data) throw(ConversionException)
{
  put((CORBA::Any *)data);
}

void CorbaCorba::put(CORBA::Any *data) throw(ConversionException)
{
  //conversion du Any data en any attendu (de type type())

  CORBA::Any *a = convertCorbaCorba(type(),data);
  _port->put(a);
}
